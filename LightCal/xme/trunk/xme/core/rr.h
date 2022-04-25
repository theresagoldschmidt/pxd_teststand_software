/*
 * Copyright (c) 2011-2012, fortiss GmbH.
 * Licensed under the Apache License, Version 2.0.
 *
 * Use, modification and distribution are subject to the terms specified
 * in the accompanying license file LICENSE.txt located at the root directory
 * of this software distribution. A copy is available at
 * http://chromosome.fortiss.org/.
 *
 * This file is part of CHROMOSOME.
 *
 * $Id$
 */

/**
 * \file
 *         Request/response communication (RR).
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 */

/*
 * TODO: See ticket #795
 *  - What happens when multiple servers exist that can provide the requested functionality?
 *    Currently, there is no specific mechanism in place to resolve such situations (the
 *    response callback function would probably just be called multiple times, so the
 *    receiving component is responsible for handling this).
 *  - When do requests time out on forward nodes?
 *  - User input as use case for RR: arbitrary processing delay!
 *  - RR semantics: "only one server", "all servers", "all or none", etc.
 */

#ifndef XME_CORE_RR_H
#define XME_CORE_RR_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/core.h"
#include "xme/core/md.h"
#include "xme/core/topic.h"

#include "xme/hal/table.h"
#include "xme/hal/time.h"

#include <stdbool.h>
#include <stdint.h>

/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/

#define XME_CORE_RR_DEFAULT_REQUEST_TIMEOUT 5000

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
/**
 * \typedef xme_core_rr_responseStatus_t
 *
 * \brief  Response status codes.
 *
 *         Used as return value in xme_core_rr_receiveRequestCallback_t and as
 *         the type of the status parameter in xme_core_rr_receiveResponseCallback_t.
 */
typedef enum
{
	XME_CORE_RR_STATUS_SUCCESS = 0,           ///< Request handled successfully.
	XME_CORE_RR_STATUS_TIMEOUT = 1,           ///< Request timed out.
	XME_CORE_RR_STATUS_SERVER_ERROR = 2,      ///< Server encountered an unrecoverable error while processing the request.
	XME_CORE_RR_STATUS_BUFFER_TOO_SMALL = 12, ///< Response buffer is too small.
	XME_CORE_RR_STATUS_RESPONSE_DELAYED = 13, ///< Response will follow later in a call to xme_core_rr_sendResponse().
	XME_CORE_RR_STATUS_USER = 16,             ///< Start of user-defined error codes.
	XME_CORE_RR_STATUS_USER_MAX = 255         ///< End of user-defined error codes.
}
xme_core_rr_responseStatus_t;

/**
 * \typedef xme_core_rr_requestHandle_t
 *
 * \brief  Locally valid identifier of a request.
 */
typedef enum
{
	XME_CORE_RR_INVALID_REQUEST_HANDLE = 0, ///< Invalid request handle.
	XME_CORE_RR_MAX_REQUEST_HANDLE = XME_HAL_TABLE_MAX_ROW_HANDLE ///< Largest possible request handle.
}
xme_core_rr_requestHandle_t;

/**
 * \typedef xme_core_rr_requestHandlerHandle_t
 *
 * \brief  Locally valid handle for a request handler.
 */
typedef enum
{
	XME_CORE_RR_INVALID_REQUEST_HANDLER_HANDLE = 0, ///< Invalid request handler handle.
	XME_CORE_RR_MAX_REQUEST_HANDLER_HANDLE = XME_HAL_TABLE_MAX_ROW_HANDLE ///< Largest possible handle.
}
xme_core_rr_requestHandlerHandle_t;

/**
 * \typedef xme_core_rr_requestInstanceHandle_t
 *
 * \brief  Locally valid handle for a request instance.
 */
typedef enum
{
	XME_CORE_RR_INVALID_REQUEST_INSTANCE_HANDLE = 0, ///< Invalid request instance handle.
	XME_CORE_RR_MAX_REQUEST_INSTANCE_HANDLE = XME_HAL_TABLE_MAX_ROW_HANDLE ///< Largest possible request instance handle.
}
xme_core_rr_requestInstanceHandle_t;

/**
 * \typedef xme_core_rr_responseInstanceHandle_t
 *
 * \brief  Locally valid handle for a response instance.
 */
typedef enum
{
	XME_CORE_RR_INVALID_RESPONSE_INSTANCE_HANDLE = 0, ///< Invalid response instance handle.
	XME_CORE_RR_MAX_RESPONSE_INSTANCE_HANDLE = XME_HAL_TABLE_MAX_ROW_HANDLE ///< Largest possible response instance handle.
}
xme_core_rr_responseInstanceHandle_t;

/**
 * \typedef xme_core_rr_receiveRequestCallback_t
 *
 * \brief  Callback function for request reception.
 *
 * \param  requestTopic Topic for which data have been received.
 * \param  requestData The request data that have been received. The size,
 *         format and semantics depend on the type of request topic.
 * \param  responseInstanceHandle Identifier of the current response issue.
 *         When the function returns XME_CORE_RR_RESPONSE_DELAYED, the
 *         value of this parameter has to be passed on to the respective
 *         call of the xme_core_rr_sendResponse() function.
 * \param  responseData Response data to send to the client.
 * \param  responseSize Initially, the number of bytes available in
 *         responseData. The callback function should modify this value to
 *         represent the number of bytes actually used in responseData.
 *         If the initial number of bytes is not enough to represent the
 *         response, the callback function should set responseSize to the
 *         required size and return XME_CORE_RR_STATUS_BUFFER_TOO_SMALL.
 * \param  responseTimeoutMs Timeout of the request in milliseconds.
 *         Note that this parameter is just a rule of thumb when
 *         it might be too late to deliver a response, since the
 *         transmission of the request to the request handler and vice
 *         versa takes additional time.
 * \param  userData User-defined data passed to the callback function.
 *         The value of this parameter is specified in the call to
 *         xme_core_rr_publishRequestHandler() where this callback function
 *         has been registered.
 *
 * \return The function should return one of the following response status
 *         codes:
 *          - XME_CORE_RR_STATUS_SUCCESS if the request was handled
 *            successfully. In this case, the response data have to be located
 *            at the address pointed to by responseData and the address
 *            pointed to by responseSize should contain the size in bytes of
 *            the response.
 *          - XME_CORE_RR_STATUS_SERVER_ERROR if the server encountered an
 *            unrecoverable error while processing the request.
 *          - XME_CORE_RR_STATUS_BUFFER_TOO_SMALL if the response buffer size
 *            denoted by responseSize is too small to represent the response.
 *          - XME_CORE_RR_STATUS_RESPONSE_DELAYED if a response can not be
 *            provided at this point in time, but it will follow later with
 *            an explicit call to xme_core_rr_sendResponse().
 *          - a value between XME_CORE_RR_STATUS_USER and
 *            XME_CORE_RR_STATUS_USER_MAX to indicate a specific, application
 *            defined status to the client.
 */
typedef xme_core_rr_responseStatus_t (*xme_core_rr_receiveRequestCallback_t)
	(
		xme_core_topic_t requestTopic,
		void* requestData,
		xme_core_rr_responseInstanceHandle_t responseInstanceHandle,
		void* responseData,
		uint16_t* responseSize,
		xme_hal_time_interval_t responseTimeoutMs,
		void* userData
	);

/**
 * \typedef xme_core_rr_receiveResponseCallback_t
 *
 * \brief  Callback function for response reception.
 *
 * \param  status One of the following response status code or a server
 *         defined value larger than or equal to XME_CORE_RR_STATUS_USER
 *         indicating whether the request completed successfully:
 *          - XME_CORE_RR_STATUS_SUCCESS if the request was served
 *            successfully.
 *          - XME_CORE_RR_STATUS_TIMEOUT if the timout interval specified in
 *            the call to xme_core_rr_sendRequest() has elapsed. If no server
 *            could be found for the given request, the callback function will
 *            be called only once with the status parameter set to this value.
 *            Otherwise, multiple invocations with success status may occur
 *            before this status is finally passed to the callback function.
 *          - XME_CORE_RR_STATUS_SERVER_ERROR if the server encountered an
 *            error while processing the given request.
 * \param  request Request handle that denotes which request type the response
 *         belongs to. This is the same value returned by the respective call
 *         to the xme_core_rr_publishRequest() function.
 * \param  requestInstanceHandle Request instance handle that denotes which
 *         request the response belongs to. This is the same value returned
 *         by the respective call to the xme_core_rr_sendRequest() function.
 * \param  responseTopic Topic for which data have been received.
 * \param  responseData The response data that have been received. The size,
 *         format and semantics depend on the type of request topic.
 * \param  responseMetaData Meta data associated with the response.
 * \param  userData User-defined data passed to the callback function.
 *         The value of this parameter is specified in the call to
 *         xme_core_rr_publishRequest() where this callback function has been
 *         registered.
 * \param  instanceUserData User-defined data passed to the callback function.
 *         The value of this parameter is specified in the call to
 *         xme_core_rr_sendRequest().
 */
typedef void (*xme_core_rr_receiveResponseCallback_t)
	(
		xme_core_rr_responseStatus_t status,
		xme_core_rr_requestHandle_t request,
		xme_core_rr_requestInstanceHandle_t requestInstance,
		xme_core_topic_t responseTopic,
		void* responseData,
		xme_core_md_topicMetaDataHandle_t responseMetaData,
		void* userData,
		void* instanceUserData
	);

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
/**
 * \brief  Initializes the request/response communication component.
 *         At most one component of this type can be present on every node.
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_SUCCESS if the request/response communication component
 *             has been properly initialized.
 *          - XME_CORE_STATUS_OUT_OF_RESOURCES if initialization failed.
 */
xme_core_status_t
xme_core_rr_init();

/**
 * \brief  Frees all resources occupied by the request/response communicatio
 *         component. At most one component of this type can be present on
 *         every node.
 */
void
xme_core_rr_fini();

/**
 * \brief  Makes the runtime system aware that the calling component intends
 *         to publish (i.e., send) the given request topic and expects the
 *         given response topic as reply.
 *
 *         This function does not indicate when data of the given topic are
 *         sent. This can happen at any point in time after the call to this
 *         function. When a component does not intend to send requests under
 *         this topic any more, the xme_core_rr_unpublishRequest() function
 *         should be called.
 *
 * \param  requestTopic Topic of the request.
 * \param  requestMetaDataHandle Meta data associated to the request.
 * \param  responseTopic Topic of the response.
 * \param  responseMetaDataFilter Meta data filter to apply to the response.
 * \param  manyResponses Defines if request wants to get only one or as many as possilbe responses.
 * \param  onlyLocal Defines if request shall be limited to the local node or not.
 * \param  receiveResponseCallback Callback function to be called when the
 *         response arrives or when the timeout interval specified in
 *         the timeoutMs parameter of the respective call to
 *         xme_core_rr_sendRequest() elapses.
 * \param  userData User-specified data to pass to the callback function.
 *
 * \return On success, returns a nonzero request handle, otherwise returns
 *         XME_CORE_RR_INVALID_REQUEST_HANDLE.
 */
xme_core_rr_requestHandle_t
xme_core_rr_publishRequest
(
	xme_core_topic_t requestTopic,
	xme_core_md_topicMetaDataHandle_t requestMetaDataHandle,
	xme_core_topic_t responseTopic,
	xme_core_md_topicMetaDataHandle_t responseMetaDataFilter,
	bool manyResponses,
	bool onlyLocal,
	xme_core_rr_receiveResponseCallback_t receiveResponseCallback,
	void* userData
);

/**
 * \brief  Makes the runtime system aware that the calling component does not
 *         intend to send requests of the given type any more.
 *
 * \param  requestHandle Handle of the request to unpublish.
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the request was unpublished
 *            successfully.
 *          - XME_CORE_STATUS_PERMISSION_DENIED if the calling component is
 *            not allowed to perform this action.
 *          - XME_CORE_STATUS_INVALID_HANDLE if the given request handle was
 *            invalid.
 */
xme_core_status_t
xme_core_rr_unpublishRequest
(
	xme_core_rr_requestHandle_t requestHandle
);

/**
 * \brief  Makes the runtime system aware that the calling component can
 *         handle requests of the given type and, upon success, will respond
 *         to them with the given response topic.
 *
 * \param  requestTopic Topic of the request that is accepted by the component.
 * \param  requestFilter Meta data filter for incoming requests.
 * \param  responseTopic Topic of the response the component will send after
 *         processing a request matching the given parameters.
 * \param  responseMetaData Static meta data of the response topic.
 * \param  onlyLocal Defines if request handler shall be limited to the local node
 *         or not.
 * \param  receiveRequestCallback Callback function for handling the given
 *         request.
 * \param  userData User-defined data to pass to the callback function.
 *
 * \return On success, returns a nonzero request/response handle, otherwise
 *         returns XME_CORE_RR_INVALID_REQUEST_HANDLER_HANDLE.
 */
xme_core_rr_requestHandlerHandle_t
xme_core_rr_publishRequestHandler
(
	xme_core_topic_t requestTopic,
	xme_core_md_topicMetaDataHandle_t requestFilter,
	xme_core_topic_t responseTopic,
	xme_core_md_topicMetaDataHandle_t responseMetaData,
	bool onlyLocal,
	xme_core_rr_receiveRequestCallback_t receiveRequestCallback,
	void* userData
);

/**
 * \brief  Makes the runtime system aware that the calling component
 *         discontinues handling the given request.
 *
 * \param  requestHandlerHandle handling to discontinue.
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the request handling capability has
 *            been successfully discontinued.
 *          - XME_CORE_STATUS_PERMISSION_DENIED if the calling component is
 *            not allowed to perform this action.
 *          - XME_CORE_STATUS_INVALID_HANDLE if the given request/response
 *            handle was not valid.
 */
xme_core_status_t
xme_core_rr_unpublishRequestHandler
(
	xme_core_rr_requestHandlerHandle_t requestHandlerHandle
);

/**
 * \brief  Sends a request and schedules the given callback function for
 *         execution when the reply arrives or the timeout interval specified
 *         in timeoutMs elapses.
 *         In case multiple servers respond to the request, the callback
 *         function may be called multiple times.
 *         In any case, the callback function will be called with the status
 *         parameter set to XME_CORE_RR_STATUS_TIMEOUT when the timeout
 *         interval elapses to indicate that no more responses will arrive.
 *
 * \param  request Handle describing the type of request to send.
 * \param  requestData Content of the request.
 * \param  requestSize Size in bytes of the request data to send.
 * \param  instanceUserData User-specified data to pass to the callback
 *         function.
 * \param  timeoutMs Timeout interval in milliseconds that specifies how
 *         long the client wants to wait for a reply from any of the servers.
 *
 * \return On success, returns a nonzero request instance handle, otherwise
 *         returns XME_CORE_RR_INVALID_REQUEST_INSTANCE_HANDLE.
 *         The request/response instance handle can be used to identify the
 *         exact request a response belongs to and to cancel a request while
 *         it is still pending using xme_core_rr_cancelRequest().
 */
xme_core_rr_requestInstanceHandle_t
xme_core_rr_sendRequest
(
	xme_core_rr_requestHandle_t request,
	void* requestData,
	uint16_t requestSize,
	void* instanceUserData,
	//TODO: period. See ticket #796
	xme_hal_time_interval_t timeoutMs
);

/**
 * \brief  Send a response asynchronously.
 *
 *         If the response to a request can not be determined immediately by a
 *         request handler, it returns the status code
 *         XME_CORE_RR_STATUS_RESPONSE_DELAYED and uses this function to supply
 *         the response once it is available.
 *
 * \param  status Should be one of the following status codes:
 *          - XME_CORE_RR_STATUS_SUCCESS if the request was handled
 *            successfully. In this case, the response data have to be located
 *            at the address pointed to by responseData and the address
 *            pointed to by responseSize should contain the size in bytes of
 *            the response.
 *          - XME_CORE_RR_STATUS_SERVER_ERROR if the server encountered an
 *            unrecoverable error while processing the request.
 *          - XME_CORE_RR_STATUS_BUFFER_TOO_SMALL if the response buffer size
 *            denoted by responseSize is too small to represent the response.
 *          - a value between XME_CORE_RR_STATUS_USER and
 *            XME_CORE_RR_STATUS_USER_MAX to indicate a specific, application
 *            defined status to the client.
 *         This parameter corresponds to the return value of the
 *         xme_core_rr_receiveRequestCallback_t function when using the
 *         synchronous semantics of request/response.
 * \param  requestHandler Request handler that issued the response.
 * \param  responseInstanceHandle Identifier of the response issue. The value
 *         of this parameter has to be set to the value of the
 *         responseInstanceHandle parameter of the respective call from
 *         xme_core_rr_receiveRequestCallback_t.
 * \param  responseData Response data to send to the client.
 * \param  responseSize Number of bytes used in responseData.
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the response has been sent
 *            successfully.
 *          - XME_CORE_STATUS_INVALID_PARAMETER if the specified request
 *            handler handle was invalid.
 *          - XME_CORE_STATUS_PERMISSION_DENIED if another component than the
 *            specified request handler called this function.
 *          - XME_CORE_STATUS_INVALID_HANDLE if the specified request
 *            instance handle was invalid.
 */
xme_core_status_t
xme_core_rr_sendResponse
(
	xme_core_rr_responseStatus_t status,
	xme_core_rr_requestHandlerHandle_t requestHandler,
	xme_core_rr_responseInstanceHandle_t responseInstanceHandle,
	void* responseData,
	uint16_t responseSize
);

/**
 * \brief  Cancels a pending request.
 *         A request can be canceled at any time before its timeout occurs.
 *
 *         A typical use case is to cancel a request from within the response
 *         callback function once the first acceptable response has been
 *         received from a server, to prevent responses from other servers
 *         being subsequently received.
 *
 *         If a request is successfully canceled, the runtime system will not
 *         deliver any subsequent responses to the client. Furthermore, the
 *         response callback function will not be called with the status
 *         parameter set to XME_CORE_RR_STATUS_TIMEOUT.
 *
 * \param  requestInstanceHandle Handle of the request instance to cancel.
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the request has been successfully
 *            canceled.
 *          - XME_CORE_STATUS_INVALID_HANDLE if the specified request/response
 *            instance handle is not valid. If this status code is returned,
 *            the request has most probably already timed out.
 */
xme_core_status_t
xme_core_rr_cancelRequest
(
	xme_core_rr_requestInstanceHandle_t requestInstanceHandle
);

#endif // #ifndef XME_CORE_RR_H
