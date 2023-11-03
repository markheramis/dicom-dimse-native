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
  ns::registerCodecs();
}

void ServerAsyncWorker::Execute(const ExecutionProgress &progress)
{
  ns::sInput in;
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
bool ServerAsyncWorker::InitializeAndValidateInput(ns::sInput &in, const ExecutionProgress &progress)
{
    in = ns::parseInputJson(_input);
    EnableVerboseLogging(in.verbose);
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
    // Check if the data dictionary is loaded
    if (!dcmDataDict.isDictionaryLoaded())
    {
        SetErrorJson(std::string("No data dictionary loaded"));
        return false;
    }

    static OFString opt_outputDirectory = OFString(in.storagePath.c_str());

    // Normalize the directory name
    OFStandard::normalizeDirName(opt_outputDirectory, opt_outputDirectory);

    // Check if the specified directory exists
    if (opt_outputDirectory != "." && !OFStandard::dirExists(opt_outputDirectory))
    {
        SetErrorJson(std::string("Specified output directory does not exist: ") + std::string(opt_outputDirectory.c_str()));
        return false;
    }

    // Check if the output directory is writeable
    if (!OFStandard::isWriteable(opt_outputDirectory))
    {
        SetErrorJson(std::string("Specified output directory is not writeable: ") + std::string(opt_outputDirectory.c_str()));
        return false;
    }

    return true;
}



bool ServerAsyncWorker::InitializeNetwork(const ns::sInput &in, T_ASC_Network *&net)
{
    OFStandard::initializeNetwork();

    int opt_port = in.source.port;

    // Initialize network as an acceptor
    OFCondition cond = ASC_initializeNetwork(NET_ACCEPTOR, opt_port, 30, &net);
    if (cond.bad())
    {
        SetErrorJson(std::string("Cannot create network: ") + std::string(cond.text()));
        return false;
    }

    // Drop root privileges for security (if running as setuid root)
    if (OFStandard::dropPrivileges().bad())
    {
        SetErrorJson(std::string("setuid() failed, maximum number of threads for uid already running"));
        return false;
    }

    return true;
}


bool ServerAsyncWorker::HandleStoreOnly(const ns::sInput &in, T_ASC_Network *net, const ExecutionProgress &progress)
{
    // Set the storage directory and parameters for the Service Class Provider (SCP)
    static OFString opt_outputDirectory = OFString(in.storagePath.c_str());
    RetrieveScp scp(opt_outputDirectory, in.source.aet.c_str(), in.writeFile);

    // Initialize condition for association handling
    OFCondition cond = EC_Normal;  // Initialize to normal condition for loop entry

    // Continuously wait for incoming associations and process them
    while (cond.good())
    {
        cond = scp.waitForAssociation(net, progress);
        if (cond.bad())
        {
            // Handle the error or log it
            DCMNET_ERROR("Failed to handle incoming association: " << cond.text());
        }
    }

    // For simplicity, we return true if the condition remains good after handling all associations
    return cond.good();
}


bool ServerAsyncWorker::HandleQueryRetrieve(const ns::sInput &in, T_ASC_Network *net)
{
    DcmQueryRetriveConfigExt cfg;

    // Add peers to the configuration
    for (auto peer: in.peers) 
    {
        cfg.addPeer(peer.aet.c_str(), peer.ip.c_str(), peer.port);
    }

    cfg.setStorageArea(in.storagePath.c_str());
    cfg.setPermissiveMode(in.permissive);

    DcmQueryRetrieveOptions options;
    T_ASC_Network* network = NULL;

    // Initialize the network as a requestor
    OFCondition cond = ASC_initializeNetwork(NET_REQUESTOR, 0, 10000, &network);
    if (cond.bad()) 
    {
        OFString temp_str;
        DCMNET_ERROR("Failed to create requestor network: " << DimseCondition::dump(temp_str, cond));
        return false;
    }

    options.net_ = network;
    options.allowShutdown_ = true;
    options.disableGetSupport_ = true;
    options.maxAssociations_ = 128;

    // Set transfer syntax preferences
    DcmXfer netTransPrefer = in.netTransferPrefer.empty() ? DcmXfer(EXS_Unknown) : DcmXfer(in.netTransferPrefer.c_str());
    DcmXfer netTransPropose = in.netTransferPropose.empty() ? DcmXfer(EXS_Unknown) : DcmXfer(in.netTransferPropose.c_str());
    DcmXfer writeTrans = in.writeTransfer.empty() ? DcmXfer(EXS_Unknown) : DcmXfer(in.writeTransfer.c_str());

    options.networkTransferSyntax_ = netTransPrefer.getXfer();
    options.networkTransferSyntaxOut_ = netTransPropose.getXfer();
    options.writeTransferSyntax_ = writeTrans.getXfer();

    // Create SCP for query and retrieve
    DcmQueryRetrieveSQLiteDatabaseHandleFactory factory(&cfg);
    DcmAssociationConfiguration associationConfiguration;
    DcmQueryRetrieveSCP scp(cfg, options, factory, associationConfiguration);

    while (cond.good()) 
    {
        cond = scp.waitForAssociation(net);
    }

    return cond.good();
}

void ServerAsyncWorker::ShutdownNetwork(T_ASC_Network *&net)
{
    // Network shutdown logic
    OFCondition cond = ASC_dropNetwork(&net);
    if (cond.bad())
    {
        SetErrorJson(std::string(cond.text()));
    }
    OFStandard::shutdownNetwork();
}
