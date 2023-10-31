#include <napi.h>

#include "EchoAsyncWorker.h"
#include "FindAsyncWorker.h"
#include "GetAsyncWorker.h"
#include "MoveAsyncWorker.h"
#include "StoreAsyncWorker.h"
#include "ServerAsyncWorker.h"
#include "ParseAsyncWorker.h"
#include "CompressAsyncWorker.h"
#include "ShutdownAsyncWorker.h"

#include <iostream>

using namespace Napi;

/**
 * The `DoEcho` function is an interface to Node.js for the DICOM ECHO operation.
 *
 * This function acts as a bridge between the Node.js runtime and the C++ backend
 * logic for the DICOM ECHO operation. This operation is an asynchronous operation,
 * and it's performed using the `EchoAsyncWorker` class.
 *
 * @param info: Contains arguments passed from the Node.js runtime.
 *              - info[0] is expected to be a string which might represent some
 *                input or configuration for the ECHO operation.
 *              - info[1] is expected to be a callback function that will be called
 *                once the asynchronous operation is completed. This allows for
 *                handling results or errors in the Node.js side.
 *
 * @returns: The function immediately returns an undefined value, indicating that the
 *           result will be available later, asynchronously, through the provided
 *           callback.
 */
Value DoEcho(const CallbackInfo &info)
{
    // Extract the first argument as a string `input` which could be some
    // configuration or message for the ECHO operation.
    std::string input = info[0].As<String>().Utf8Value();
    // Extract the second argument as a function `cb` which will be the callback
    // to be executed after the operation finishes.
    Function cb = info[1].As<Function>();
    // Create a new instance of `EchoAsyncWorker`, initializing it with the
    // `input` and `cb`.
    auto worker = new EchoAsyncWorker(input, cb);
    // Queue the `EchoAsyncWorker` for asynchronous execution.
    // - Behind the scenes, Node.js will run this worker in a separate thread
    //   so that the main event loop is not blocked.
    // - Once the worker's task completes (either successfully or with an error),
    //   it'll call the provided callback with the results.
    worker->Queue();
    // Immediately return an undefined value to the Node.js runtime, signaling
    // that the function has completed its execution, but the results will be
    // delivered asynchronously via the provided callback.
    return info.Env().Undefined();
}

/**
 * The `DoFind` function serves as an interface between Node.js and the C++ backend
 * for the DICOM FIND operation.
 *
 * The purpose of this function is to bridge the Node.js runtime with the C++ logic
 * related to the DICOM FIND operation, which is an asynchronous process implemented
 * using the `FindAsyncWorker` class.
 *
 * @param info: Contains arguments passed from the Node.js runtime.
 *              - info[0] is expected to be a string which might represent some input
 *                or configuration for the FIND operation.
 *              - info[1] is expected to be a callback function that will be called
 *                once the asynchronous operation is completed. This callback ensures
 *                that the results or errors can be processed on the Node.js side.
 *
 * @returns: The function immediately returns an undefined value. This indicates to the
 *           caller that the result will be available later, asynchronously, through
 *           the provided callback.
 */
Value DoFind(const CallbackInfo &info)
{
    // Extract the first argument as a string named `input`. This could potentially be
    // some configuration or message related to the FIND operation.
    std::string input = info[0].As<String>().Utf8Value();
    // Extract the second argument as a function named `cb`. This function will be
    // executed once the asynchronous operation completes, allowing for results or
    // errors to be processed in the Node.js environment.
    Function cb = info[1].As<Function>();
    // A new instance of `FindAsyncWorker` is created, initializing it with the
    // `input` and `cb`.
    auto worker = new FindAsyncWorker(input, cb);
    // The `FindAsyncWorker` instance is queued for asynchronous execution.
    // - Internally, Node.js will manage this worker in a separate thread to ensure
    //   the main event loop remains unblocked.
    // - When the worker's task completes (either successfully or due to an error),
    //   the provided callback (`cb`) is executed with the corresponding results.
    worker->Queue();
    // The function concludes by returning an undefined value to the Node.js runtime,
    // indicating that while its execution is complete, the results are still pending
    // and will be provided asynchronously via the callback.
    return info.Env().Undefined();
}

/**
 * The `DoGet` function serves as an interface between Node.js and the C++ backend
 * for the DICOM GET operation.
 *
 * The purpose of this function is to bridge the Node.js runtime with the C++ logic
 * related to the DICOM GET operation, which is an asynchronous process implemented
 * using the `GetAsyncWorker` class.
 *
 * @param info: Contains arguments passed from the Node.js runtime.
 *              - info[0] is expected to be a string which might represent some input
 *                or configuration for the GET operation.
 *              - info[1] is expected to be a callback function that will be called
 *                once the asynchronous operation is completed. This callback ensures
 *                that the results or errors can be processed on the Node.js side.
 *
 * @returns: The function immediately returns an undefined value. This indicates to the
 *           caller that the result will be available later, asynchronously, through
 *           the provided callback.
 */
Value DoGet(const CallbackInfo &info)
{
    // Extract the first argument as a string named `input`. This could potentially be
    // some configuration or message related to the GET operation.
    std::string input = info[0].As<String>().Utf8Value();
    // Extract the second argument as a function named `cb`. This function will be
    // executed once the asynchronous operation completes, allowing for results or
    // errors to be processed in the Node.js environment.
    Function cb = info[1].As<Function>();
    // A new instance of `GetAsyncWorker` is created, initializing it with the
    // `input` and `cb`.
    auto worker = new GetAsyncWorker(input, cb);
    // The `GetAsyncWorker` instance is queued for asynchronous execution.
    // - Internally, Node.js will manage this worker in a separate thread to ensure
    //   the main event loop remains unblocked.
    // - When the worker's task completes (either successfully or due to an error),
    //   the provided callback (`cb`) is executed with the corresponding results.
    worker->Queue();
    // The function concludes by returning an undefined value to the Node.js runtime,
    // indicating that while its execution is complete, the results are still pending
    // and will be provided asynchronously via the callback.
    return info.Env().Undefined();
}

/**
 * The `DoMove` function serves as an interface between Node.js and the C++ backend
 * for the DICOM MOVE operation.
 *
 * The purpose of this function is to bridge the Node.js runtime with the C++ logic
 * related to the DICOM MOVE operation, which is an asynchronous process implemented
 * using the `MoveAsyncWorker` class.
 *
 * @param info: Contains arguments passed from the Node.js runtime.
 *              - info[0] is expected to be a string which might represent some input
 *                or configuration for the MOVE operation.
 *              - info[1] is expected to be a callback function that will be called
 *                once the asynchronous operation is completed. This callback ensures
 *                that the results or errors can be processed on the Node.js side.
 *
 * @returns: The function immediately returns an undefined value. This indicates to the
 *           caller that the result will be available later, asynchronously, through
 *           the provided callback.
 */
Value DoMove(const CallbackInfo &info)
{
    // Extract the first argument as a string named `input`. This could potentially be
    // some configuration or message related to the MOVE operation.
    std::string input = info[0].As<String>().Utf8Value();
    // Extract the second argument as a function named `cb`. This function will be
    // executed once the asynchronous operation completes, allowing for results or
    // errors to be processed in the Node.js environment.
    Function cb = info[1].As<Function>();
    // A new instance of `MoveAsyncWorker` is created, initializing it with the
    // `input` and `cb`.
    auto worker = new MoveAsyncWorker(input, cb);
    // The `MoveAsyncWorker` instance is queued for asynchronous execution.
    // - Internally, Node.js will manage this worker in a separate thread to ensure
    //   the main event loop remains unblocked.
    // - When the worker's task completes (either successfully or due to an error),
    //   the provided callback (`cb`) is executed with the corresponding results.
    worker->Queue();
    // The function concludes by returning an undefined value to the Node.js runtime,
    // indicating that while its execution is complete, the results are still pending
    // and will be provided asynchronously via the callback.
    return info.Env().Undefined();
}

/**
 * The `DoStore` function serves as an interface between Node.js and the C++ backend
 * for the DICOM STORE operation.
 *
 * The purpose of this function is to bridge the Node.js runtime with the C++ logic
 * related to the DICOM STORE operation, which is an asynchronous process implemented
 * using the `StoreAsyncWorker` class.
 *
 * @param info: Contains arguments passed from the Node.js runtime.
 *              - info[0] is expected to be a string which might represent some input
 *                or configuration for the STORE operation.
 *              - info[1] is expected to be a callback function that will be called
 *                once the asynchronous operation is completed. This callback ensures
 *                that the results or errors can be processed on the Node.js side.
 *
 * @returns: The function immediately returns an undefined value. This indicates to the
 *           caller that the result will be available later, asynchronously, through
 *           the provided callback.
 */
Value DoStore(const CallbackInfo &info)
{
    // Extract the first argument as a string named `input`. This could potentially be
    // some configuration or message related to the STORE operation.
    std::string input = info[0].As<String>().Utf8Value();
    // Extract the second argument as a function named `cb`. This function will be
    // executed once the asynchronous operation completes, allowing for results or
    // errors to be processed in the Node.js environment.
    Function cb = info[1].As<Function>();
    // A new instance of `StoreAsyncWorker` is created, initializing it with the
    // `input` and `cb`.
    auto worker = new StoreAsyncWorker(input, cb);
    // The `StoreAsyncWorker` instance is queued for asynchronous execution.
    // - Internally, Node.js will manage this worker in a separate thread to ensure
    //   the main event loop remains unblocked.
    // - When the worker's task completes (either successfully or due to an error),
    //   the provided callback (`cb`) is executed with the corresponding results.
    worker->Queue();
    // The function concludes by returning an undefined value to the Node.js runtime,
    // indicating that while its execution is complete, the results are still pending
    // and will be provided asynchronously via the callback.
    return info.Env().Undefined();
}

/**
 * The `DoParse` function serves as an interface between Node.js and the C++ backend
 * for the DICOM FILE PARSING operation.
 *
 * The purpose of this function is to bridge the Node.js runtime with the C++ logic
 * related to parsing DICOM files, which is an asynchronous process implemented using 
 * the `ParseAsyncWorker` class.
 *
 * @param info: Contains arguments passed from the Node.js runtime.
 *              - info[0] is expected to be a string which might represent the file path
 *                or some input related to the DICOM file to be parsed.
 *              - info[1] is expected to be a callback function that will be called 
 *                once the asynchronous operation is completed. This callback ensures 
 *                that the results or errors can be processed on the Node.js side.
 *
 * @returns: The function immediately returns an undefined value. This indicates to the
 *           caller that the result will be available later, asynchronously, through 
 *           the provided callback.
 */
Value DoParse(const CallbackInfo &info)
{
    // Extract the first argument as a string named `input`, which might represent 
    // the file path or some other input related to the DICOM file to be parsed.
    std::string input = info[0].As<String>().Utf8Value();
    // Extract the second argument as a function named `cb`. This function will be 
    // executed once the asynchronous operation completes, allowing for results or 
    // errors to be processed in the Node.js environment.
    Function cb = info[1].As<Function>();
    // A new instance of `ParseAsyncWorker` is created, initializing it with the 
    // `input` and `cb`.
    auto worker = new ParseAsyncWorker(input, cb);
    // The `ParseAsyncWorker` instance is queued for asynchronous execution.
    // - Internally, Node.js will manage this worker in a separate thread to ensure 
    //   the main event loop remains unblocked.
    // - When the worker's task completes (either successfully or due to an error),
    //   the provided callback (`cb`) is executed with the corresponding results.
    worker->Queue();
    // The function concludes by returning an undefined value to the Node.js runtime, 
    // indicating that while its execution is complete, the results are still pending 
    // and will be provided asynchronously via the callback.
    return info.Env().Undefined();
}


/**
 * The `DoCompress` function serves as an interface between Node.js and the C++ backend
 * for the DICOM FILE COMPRESSION operation.
 *
 * This function is designed to bridge the Node.js runtime with the C++ logic
 * related to compressing DICOM files. The compression process is asynchronous and is
 * implemented using the `CompressAsyncWorker` class.
 *
 * @param info: Contains arguments passed from the Node.js runtime.
 *              - info[0] is expected to be a string which might represent the file path
 *                or some specific configuration/input related to the DICOM file compression.
 *              - info[1] is expected to be a callback function that will be called 
 *                once the asynchronous operation is completed. This callback ensures 
 *                that the results or errors are sent back to the Node.js side.
 *
 * @returns: The function immediately returns an undefined value. This indicates to the
 *           caller that the result will be available later, asynchronously, through 
 *           the provided callback.
 */
Value DoCompress(const CallbackInfo &info)
{
    // Extract the first argument as a string named `input`. This could represent 
    // the file path or other relevant details regarding the DICOM file to be compressed.
    std::string input = info[0].As<String>().Utf8Value();
    // Extract the second argument as a function named `cb`. This function will be 
    // executed once the asynchronous operation completes, facilitating results or 
    // error handling in the Node.js environment.
    Function cb = info[1].As<Function>();
    // A new instance of `CompressAsyncWorker` is created, initializing it with the 
    // `input` and `cb`.
    auto worker = new CompressAsyncWorker(input, cb);
    // The `CompressAsyncWorker` instance is queued for asynchronous execution.
    // - Internally, Node.js will manage this worker in a separate thread to ensure 
    //   the main event loop is not blocked.
    // - Upon the worker's task completion (be it successful or due to an error),
    //   the provided callback (`cb`) is executed with the corresponding results.
    worker->Queue();
    // The function finishes by returning an undefined value to the Node.js runtime, 
    // indicating that even though its execution has ended, the results are still forthcoming 
    // and will be provided asynchronously via the callback.
    return info.Env().Undefined();
}

/**
 * The `StartScp` function serves as an interface between Node.js and the C++ backend
 * for initiating the DICOM Storage Service Class Provider (SCP) operation.
 *
 * This function bridges the Node.js runtime with the C++ logic related to starting the 
 * DICOM SCP operation. Given that this is an asynchronous process, it is implemented
 * using the `ServerAsyncWorker` class.
 *
 * @param info: Contains arguments passed from the Node.js runtime.
 *              - info[0] is expected to be a string which might represent a configuration
 *                or other input specifics for initiating the SCP operation.
 *              - info[1] is expected to be a callback function that will be called 
 *                once the asynchronous SCP initialization is completed. This callback ensures 
 *                that the results or errors are relayed back to the Node.js side.
 *
 * @returns: The function immediately returns an undefined value. This communicates to the
 *           caller that the result will be made available later, asynchronously, through 
 *           the provided callback.
 */
Value StartScp(const CallbackInfo &info)
{
    // Extract the first argument as a string named `input`. This could be a configuration 
    // or other details required to start the DICOM SCP operation.
    std::string input = info[0].As<String>().Utf8Value();
    // Extract the second argument as a function named `cb`. This function will be 
    // executed once the asynchronous SCP initiation completes, providing a channel for 
    // results or error management in the Node.js environment.
    Function cb = info[1].As<Function>();
    // A new instance of `ServerAsyncWorker` is created, initializing it with the 
    // `input` and `cb`.
    auto worker = new ServerAsyncWorker(input, cb);
    // The `ServerAsyncWorker` instance is queued for asynchronous execution.
    // - Internally, Node.js will manage this worker in a separate thread to guarantee 
    //   the main event loop doesn't get blocked.
    // - Upon completion of the worker's task (either successfully or due to an error),
    //   the provided callback (`cb`) is called with the relevant results.
    worker->Queue();
    // The function concludes by returning an undefined value to the Node.js runtime, 
    // signifying that while its execution is complete, the results are yet to be provided 
    // and will be made available asynchronously via the callback.
    return info.Env().Undefined();
}


/**
 * The `DoShutdown` function serves as an interface between Node.js and the C++ backend
 * for the DICOM SHUTDOWN operation.
 *
 * This function connects the Node.js runtime to the C++ logic associated with executing 
 * a DICOM SHUTDOWN process. This operation, being asynchronous, is implemented using 
 * the `ShutdownAsyncWorker` class.
 *
 * @param info: Contains arguments passed from the Node.js runtime.
 *              - info[0] is presumed to be a string which might represent specific input 
 *                or parameters related to the SHUTDOWN process.
 *              - info[1] is expected to be a callback function that will be triggered
 *                after the asynchronous SHUTDOWN operation is finished. This callback ensures 
 *                that any results or errors are communicated back to the Node.js realm.
 *
 * @returns: The function promptly returns an undefined value. This signifies to the 
 *           caller that the actual result will be delivered asynchronously at a later 
 *           time via the provided callback.
 */
Value DoShutdown(const CallbackInfo &info)
{
    // Extract the first argument, which is designated as a string named `input`. This 
    // might be a parameter or any other details vital to the DICOM SHUTDOWN procedure.
    std::string input = info[0].As<String>().Utf8Value();
    // The second argument is extracted as a function named `cb`. This function gets 
    // invoked once the asynchronous SHUTDOWN task concludes, facilitating the return 
    // of results or error handling in the Node.js environment.
    Function cb = info[1].As<Function>();
    // An instance of `ShutdownAsyncWorker` is generated, using the `input` and `cb` 
    // for initialization.
    auto worker = new ShutdownAsyncWorker(input, cb);
    // The `ShutdownAsyncWorker` instance is positioned in queue for asynchronous operation.
    // - Node.js internally will handle this worker in an isolated thread, ensuring 
    //   the primary event loop is not interrupted.
    // - Upon the worker's task finalization (either through success or error),
    //   the designated callback (`cb`) is triggered with the corresponding output.
    worker->Queue();
    // The function ends by returning an undefined value to the Node.js runtime, 
    // signaling that even though its task has ended, the outcome is still pending 
    // and will be supplied asynchronously through the callback mechanism.
    return info.Env().Undefined();
}

Object Init(Env env, Object exports)
{
    exports.Set(String::New(env, "echoScu"), Function::New(env, DoEcho));
    exports.Set(String::New(env, "findScu"), Function::New(env, DoFind));
    exports.Set(String::New(env, "getScu"), Function::New(env, DoGet));
    exports.Set(String::New(env, "moveScu"), Function::New(env, DoMove));
    exports.Set(String::New(env, "storeScu"), Function::New(env, DoStore));
    exports.Set(String::New(env, "startScp"), Function::New(env, StartScp));
    exports.Set(String::New(env, "shutdownScu"), Function::New(env, DoShutdown));
    exports.Set(String::New(env, "parseFile"), Function::New(env, DoParse));
    exports.Set(String::New(env, "recompress"), Function::New(env, DoCompress));
    return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)
