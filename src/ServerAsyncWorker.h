#pragma once

#include "BaseAsyncWorker.h"

using namespace Napi;

class ServerAsyncWorker : public BaseAsyncWorker
{
public:
    ServerAsyncWorker(std::string data, Function &callback);
    /**
     * @brief Orchestrates the main operations of the ServerAsyncWorker.
     *
     * Initiates the server asynchronous worker by parsing the input and setting up the logging. The method then goes through a series of initializations, validations, and operational steps, such as directory handling, network setup, and either storing or querying and retrieving DICOM data. Each step is designed to return early if it encounters issues, ensuring a fail-fast mechanism. The DICOM network is shut down gracefully at the end.
     *
     * @param progress A reference to the execution progress indicator, used for reporting the progress during the server worker's operation.
     */
    void Execute(const ExecutionProgress &progress);

private:
    /**
     * @brief Initializes the provided input parameters, ensures the source is valid, and sets a default storage path if none is specified.
     *
     * If the source information is not valid, an error message "Source not set" is set. If a storage path is not provided, it defaults to "./data", and an informational message is logged regarding this defaulting.
     *
     * @param in A constant reference to the input structure.
     * @param progress A reference to the execution progress indicator.
     *
     * @return Returns 'true' if the initialization and validation succeed. Otherwise, returns 'false' and sets an appropriate error message.
     */
    bool InitializeAndValidateInput(ns::sInput &in, const ExecutionProgress &progress);
    /**
     * @brief Manages and validates directory-related configurations for the server worker.
     *
     * This method ensures that the DICOM data dictionary is loaded, validates the specified output directory, and checks its write permissions. If the output directory is not specified, it defaults to the current directory ("."). If the provided directory does not exist or is not writeable, appropriate error messages are set.
     *
     * @param in A reference to the input structure, which contains directory-related configurations.
     *
     * @return Returns 'true' if all directory-related validations pass. Otherwise, returns 'false' and sets an error message.
     */
    bool HandleDirectory(ns::sInput &in);
    /**
     * @brief Initializes the DICOM network and sets up necessary configurations.
     *
     * This method is responsible for initializing the DICOM network based on the provided input parameters. It starts by initializing the network and then attempts to set up an acceptor on a specified port. Furthermore, it ensures privilege dropping for security and tries to set up a requestor network.
     *
     * In case any step fails, relevant error messages are set, making it easier to diagnose issues during the network initialization process.
     *
     * @param in A constant reference to the input structure containing network configurations.
     * @param net A reference to a pointer of the T_ASC_Network type, which will be initialized by this method.
     *
     * @return Returns 'true' if the network initialization is successful. Otherwise, returns 'false' and sets an error message.
     */
    bool InitializeNetwork(const ns::sInput &in, T_ASC_Network *&net);
    /**
     * @brief Manages the storing of DICOM data when operating in "store only" mode.
     *
     * This method facilitates the storage of DICOM data by setting up the appropriate SCP (Service Class Provider) using the provided storage path and AE title. It then continuously waits for incoming associations and processes them until a termination condition is met.
     *
     * @param in A constant reference to the input structure containing storage configurations and other related parameters.
     * @param net A pointer to the initialized T_ASC_Network.
     * @param progress A reference to the execution progress indicator, used for reporting progress during the storage process.
     *
     * @return Returns 'true' when the method completes its operation.
     */
    bool HandleStoreOnly(const ns::sInput &in, T_ASC_Network *net, const ExecutionProgress &progress);
    /**
     * @brief Manages the querying and retrieving of DICOM data when not operating in "store only" mode.
     *
     * This method sets up configurations for querying and retrieving DICOM data. It initializes the appropriate configurations, transfer syntax preferences, and SCP (Service Class Provider) settings using provided input. The method also logs essential information about transfer syntax and operational mode. It then continuously waits for and processes incoming associations until a termination condition is met.
     *
     * @param in A constant reference to the input structure containing query, retrieve, and other related parameters.
     * @param net A pointer to the initialized T_ASC_Network.
     *
     * @return Returns 'true' when the method completes its operation.
     */
    bool HandleQueryRetrieve(const ns::sInput &in, T_ASC_Network *net);
    /**
     * @brief Gracefully shuts down the DICOM network.
     *
     * This method attempts to drop the established DICOM network and ensures proper cleanup. If any issues arise during the network shutdown, appropriate error messages are set for further diagnosis.
     *
     * @param net A reference to a pointer of the T_ASC_Network type, which represents the established DICOM network. This pointer will be nullified after the network is dropped.
     */
    void ShutdownNetwork(T_ASC_Network *&net);
};
