#include "ServerAsyncWorker.h"

#include <iostream>
#include <sstream>
#include <memory>
#include <list>

#include "json.h"
#include "Utils.h"

using json = nlohmann::json;

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */
#include "dcmtk/oflog/oflog.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmqrdb/dcmqrsrv.h"
#include "dcmtk/dcmqrdb/dcmqrcbf.h"
#include "dcmtk/dcmqrdb/dcmqrcbm.h"
#include "dcmtk/dcmqrdb/dcmqrcbg.h"
#include "dcmtk/dcmqrdb/dcmqrcbs.h"
#include "dcmtk/dcmqrdb/dcmqropt.h"

#include "dcmsqlhdl.h"
#include "RetrieveScp.h"

ServerAsyncWorker::ServerAsyncWorker(std::string data, Function &callback) : BaseAsyncWorker(data, callback)
{
  // Register DICOM related codecs using DCMTK library.
  // This ensures that various compression and decompression algorithms
  // used in DICOM, such as RLE, JPEG, JPEG-LS, and JPEG2000, are properly set up.
  // The function employs a guard mechanism to ensure codecs are registered only once.
  ns::registerCodecs();
}

void ServerAsyncWorker::Execute(const ExecutionProgress &progress)
{
  ns::sInput in = ns::parseInputJson(_input);
  EnableVerboseLogging(in.verbose);
  SendInfo("Starting Server Async Worker", progress);
  if (!InitializeAndValidateInput(in, progress))
    return;
  if (!HandleDirectory(in))
    return;

  T_ASC_Network *net;
  if (!InitializeNetwork(in, net))
    return;

  if (in.storeOnly)
  {
    if (!HandleStoreOnly(in, net, progress))
      return;
  }
  else
  {
    if (!HandleQueryRetrieve(in, net))
      return;
  }
  ShutdownNetwork(net);
}

bool ServerAsyncWorker::InitializeAndValidateInput(const ns::sInput &in, const ExecutionProgress &progress)
{
  if (!in.source.valid())
  {
    SetErrorJson("Source not set");
    return false;
  }
  if (in.storagePath.empty())
  {
    in.storagePath = "./data";
    SendInfo("storage path not set, defaulting to " + in.storagePath, progress);
  }
  return true;
}

bool ServerAsyncWorker::HandleDirectory(ns::sInput &in)
{
  /* make sure data dictionary is loaded */
  if (!dcmDataDict.isDictionaryLoaded())
  {
    SetErrorJson(std::string("No data dictionary loaded"));
    return false;
  }
  static OFString opt_outputDirectory = OFString(in.storagePath.c_str());
  /* if the output directory does not equal "." (default directory) */
  if (opt_outputDirectory != ".")
  {
    OFStandard::normalizeDirName(opt_outputDirectory, opt_outputDirectory);
    if (!OFStandard::dirExists(opt_outputDirectory))
    {
      SetErrorJson(std::string("Specified output directory does not exist: ") + std::string(opt_outputDirectory.c_str()));
      return false;
    }
  }
  if (!OFStandard::isWriteable(opt_outputDirectory))
  {
    SetErrorJson(std::string("Specified output directory is not writeable: ") + std::string(opt_outputDirectory.c_str()));
    return false;
  }

  return true;
}

bool ServerAsyncWorker::InitializeNetwork(const ns::sInput &in, T_ASC_Network *&net)
{
  DcmAssociationConfiguration asccfg;
  OFStandard::initializeNetwork();
  int opt_port = in.source.port;
  OFCondition cond = ASC_initializeNetwork(NET_ACCEPTOR, opt_port, 30, &net);
  if (cond.bad())
  {
    SetErrorJson(std::string("Cannot create network: ") + std::string(cond.text()));
    return false;
  }
  if (OFStandard::dropPrivileges().bad())
  {
    SetErrorJson(std::string("setuid() failed, maximum number of threads for uid already running"));
    return false;
  }
  T_ASC_Network *network = NULL;
  cond = ASC_initializeNetwork(NET_REQUESTOR, 0, 10000, &network);
  if (cond.bad())
  {
    OFString temp_str;
    DCMNET_ERROR("Failed to create requestor network: " << DimseCondition::dump(temp_str, cond));
    return false;
  }

  return true;
}

bool ServerAsyncWorker::HandleStoreOnly(const ns::sInput &in, T_ASC_Network *net, const ExecutionProgress &progress)
{
  OFString opt_outputDirectory = OFString(in.storagePath.c_str());
  RetrieveScp scp(opt_outputDirectory, in.source.aet.c_str(), in.writeFile);
  OFCondition cond;
  while (cond.good())
  {
    cond = scp.waitForAssociation(net, progress);
  }
  return true;
}

bool ServerAsyncWorker::HandleQueryRetrieve(const ns::sInput &in, T_ASC_Network *net)
{
  DcmQueryRetriveConfigExt cfg;
  for (auto peer : in.peers)
  {
    cfg.addPeer(peer.aet.c_str(), peer.ip.c_str(), peer.port);
  }
  cfg.setStorageArea(in.storagePath.c_str());
  cfg.setPermissiveMode(in.permissive);
  DcmQueryRetrieveOptions options;
  options.net_ = network;
  options.allowShutdown_ = true;
  options.disableGetSupport_ = true;
  options.maxAssociations_ = 128;
  DcmXfer netTransPrefer = in.netTransferPrefer.empty() ? DcmXfer(EXS_Unknown) : DcmXfer(in.netTransferPrefer.c_str());
  DcmXfer netTransPropose = in.netTransferPropose.empty() ? DcmXfer(EXS_Unknown) : DcmXfer(in.netTransferPropose.c_str());
  DcmXfer writeTrans = in.writeTransfer.empty() ? DcmXfer(EXS_Unknown) : DcmXfer(in.writeTransfer.c_str());
  DCMNET_INFO("preferred (accepted) network transfer syntax for incoming associations: " << netTransPrefer.getXferName());
  DCMNET_INFO("proposed network transfer syntax for outgoing associations: " << netTransPropose.getXferName());
  DCMNET_INFO("write transfer syntax (recompress if different to accepted ts): " << writeTrans.getXferName());
  DCMNET_INFO("max associations: " << options.maxAssociations_);
  DCMNET_INFO("permissive mode: " << in.permissive);
  options.networkTransferSyntax_ = netTransPrefer.getXfer();
  options.networkTransferSyntaxOut_ = netTransPropose.getXfer();
  options.writeTransferSyntax_ = writeTrans.getXfer();
  DcmQueryRetrieveSQLiteDatabaseHandleFactory factory(&cfg);
  DcmAssociationConfiguration associationConfiguration;
  DcmQueryRetrieveSCP scp(cfg, options, factory, associationConfiguration);
  while (cond.good())
  {
    cond = scp.waitForAssociation(net);
  }
  return true;
}

void ServerAsyncWorker::ShutdownNetwork(T_ASC_Network *&net)
{
  OFCondition cond = ASC_dropNetwork(&net);
  if (cond.bad())
  {
    SetErrorJson(std::string(cond.text()));
  }
  OFStandard::shutdownNetwork();
}
