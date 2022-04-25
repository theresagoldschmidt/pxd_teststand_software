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
 *         Data centric communication (DCC).
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 */

#ifndef XME_CORE_DCC_H
#define XME_CORE_DCC_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/core.h"
#include "xme/core/md.h"
#include "xme/core/topic.h"

#include "xme/hal/sharedPtr.h"

#include <stdbool.h>
#include <stdint.h>

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
/**
 * \typedef xme_core_dcc_publicationHandle_t
 *
 * \brief Locally valid publication handle.
 */
typedef enum
{
	XME_CORE_DCC_INVALID_PUBLICATION_HANDLE = 0, ///< Invalid publication handle.
	XME_CORE_DCC_MAX_PUBLICATION_HANDLE = XME_HAL_TABLE_MAX_ROW_HANDLE ///< Largest possible publication handle.
}
xme_core_dcc_publicationHandle_t;

/**
 * \typedef xme_core_dcc_subscriptionHandle_t
 *
 * \brief Locally valid subscription handle.
 */
typedef enum
{
	XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE = 0, ///< Invalid subscription handle.
	XME_CORE_DCC_MAX_SUBSCRIPTION_HANDLE = XME_HAL_TABLE_MAX_ROW_HANDLE ///< Largest possible subscription handle.
}
xme_core_dcc_subscriptionHandle_t;

/**
 * \typedef xme_core_dcc_demandCallback_t
 *
 * \brief  Callback function for data demand announcement.
 *
 * \param  topic Topic for which the demand is announced.
 * \param  topicMetaDataHandle Handle of the meta data of the topic for which
 *         the demand is announced.
 * \param  demand Boolean flag that announces whether data should be
 *         generated. A value of true indicates that data should be generated
 *         as intended. A value of false indicates that data do not have to
 *         be generated until the component's life ends or demand is announced
 *         by a subsequent call to the demand callback function.
 * \param  frequency Frequency in Hertz at which data are to be generated. A
 *         value of zero indicates that an optimal frequency value is not
 *         available. In this case, the component should generate the data at
 *         the default frequency.
 */
typedef void (*xme_core_dcc_demandCallback_t)
(
	xme_core_topic_t topic,
	xme_core_md_topicMetaDataHandle_t topicMetaDataHandle,
	bool demand,
	float frequency
);

/**
 * \typedef xme_core_dcc_receiveTopicCallback_t
 *
 * \brief  Callback function for topic data reception.
 *
 * \param  dataHandle Handle pointing to a data location managed by shm.
 * \param  userData User-defined data passed to the callback function.
 *         The value of this parameter is specified in the call to
 *         xme_core_dcc_subscribeTopic() where this callback function has
 *         been registered.
 */
typedef void (*xme_core_dcc_receiveTopicCallback_t)
(
	xme_hal_sharedPtr_t dataHandle,
	void* userData
);

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
/**
 * \brief  Initializes the data centric communication component.
 *         Exactly one component of this type must be present on every node.
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_SUCCESS if the data centric communication component has
 *            been properly initialized.
 *          - XME_CORE_STATUS_OUT_OF_RESOURCES if initialization failed.
 */
xme_core_status_t
xme_core_dcc_init();

/**
 * \brief  Frees all resources occupied by the data centric communication
 *         component. Exactly one component of this type must be present on
 *         every node.
 */
void
xme_core_dcc_fini();

/**
 * \brief  Makes the runtime system aware that the calling component intends
 *         to publish (i.e., send) data of the given topic.
 *
 *         This function does not indicate when data of the given topic are
 *         sent. This can happen at any point in time after the call to this
 *         function. When a component does not publish the topic any more, the
 *         xme_core_dcc_unpublishTopic() function should be called.
 *
 * \param  topic Topic to publish/announce.
 * \param  topicMetaDataHandle Handle of the meta data to associate with the
 *         topic. These can be informational meta data (e.g., topographic
 *         information) or quality of service parameters.
 * \param  onlyLocal Defines if publication shall be limited to the local node or not.
 * \param  demandCallback Function to be called by the runtime system when the
 *         demand to generate the given data changes. This is an optimization
 *         measure used to decide whether data whose generation is very
 *         expensive (regarding computation time, energy consumption or
 *         resource allication) should actually be generated. Furthermore,
 *         the runtime system might call this function to optimize data
 *         generation, for example to announce the optimal frequency at which
 *         data are to be generated. Specifying NULL for this parameter is
 *         allowed and indicates that the announced data will always be
 *         generated with the properties specified in the meta data handle.
 *         If a non-NULL value is specified, the runtime system will call the
 *         given function after the component has been initialized to specify
 *         the intended behavior.
 *         Note that this concept slightly breaks the semantics of data driven
 *         communication, where components usually have a "fire & forget"
 *         philosophy. However, for some applications, a hybrid approach is
 *         necessary to ensure system performance.
 *
 * \return Returns a non-zero publication handle for providing data of the
 *         given topic on success and XME_CORE_DCC_INVALID_PUBLICATION_HANDLE
 *         on error.
 */
xme_core_dcc_publicationHandle_t
xme_core_dcc_publishTopic
(
	xme_core_topic_t topic,
	xme_core_md_topicMetaDataHandle_t metaData,
	bool onlyLocal,
	xme_core_dcc_demandCallback_t demandCallback
);

/**
 * \brief  Makes the runtime system aware that the calling component
 *         discontinues emission of the given topic.
 *
 * \param  publicationHandle Handle of the topic that is to be
 *         unpublished/discontinued.
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the topic has been successfully
 *            discontinued. In this case, publicationHandle is now an invalid
 *            handle and should be discarded.
 *          - XME_CORE_STATUS_PERMISSION_DENIED if the calling component is
 *            not allowed to perform this action.
 *          - XME_CORE_STATUS_INVALID_HANDLE if the given publication handle
 *            was invalid.
 */
xme_core_status_t
xme_core_dcc_unpublishTopic
(
	xme_core_dcc_publicationHandle_t publicationHandle
);

/**
 * \brief  Sends data for a given topic.
 *
 * \param  publicationHandle Handle of the publication under which the data is
 *         to be sent. The topic must have been previously published using
 *         xme_core_dcc_publishTopic().
 * \param  data Data to be published. The structure of the data must match
 *         the specification for the given topic.
 * \param  size Size in bytes of the data to publish.
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the data have been published.
 *          - XME_CORE_STATUS_INVALID_HANDLE if the given publication handle
 *            was invalid.
 */
xme_core_status_t
xme_core_dcc_sendTopicData
(
	xme_core_dcc_publicationHandle_t publicationHandle,
	void* data,
	uint16_t size
);

/**
 * \brief  Subscribes the calling component to the given topic.
 *         The topicFilter parameter can be used to specify criteria that have
 *         to be met for the data associated to the topic to be relevant to the
 *         receiver. Whenever a transmission passes the filter, the specified
 *         callback function will be invoked and the given userData will be
 *         passed to the callback function along with the data received.
 *
 * \param  topic Topic to subscribe to.
 * \param  topicFilter Handle pointing to filter criteria that have to be met
 *         for the data to be relevant to the caller.
 * \param  onlyLocal Defines if subscription shall be limited to the local node or not.
 * \param  callback Callback function to invoke when new data arrive.
 * \param  userData User-specified data to be passed to the callback function.
 *
 * \return Returns a non-zero subscription handle for receiving data of the
 *         given topic on success and XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE
 *         on error.
 */
xme_core_dcc_subscriptionHandle_t
xme_core_dcc_subscribeTopic
(
	xme_core_topic_t topic,
	xme_core_md_topicMetaDataHandle_t filter,
	bool onlyLocal,
	xme_core_dcc_receiveTopicCallback_t callback,
	void* userData
);

/**
 * \brief  Unsubscribes the calling component from the given topic.
 *         The parameters passed to this function must meet the ones specified
 *         in the call to xme_core_dcc_subscribeTopic() that was used to
 *         subscribe to the respective topic.
 * \note   Note that unsubscriptions do not have to be performed as part of
 *         component destruction. Any topic subscriptions of destroyed
 *         components will automatically be canceled.
 *
 * \param  subscriptionHandle Subscription handle describing the topic to
 *         unsubscribe from.
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the topic has been successfully
 *            unsubscribed from. In this case, subscriptionHandle is now an
 *            invalid handle and should be discarded.
 *          - XME_CORE_STATUS_PERMISSION_DENIED if the calling component is
 *            not allowed to perform this action.
 *          - XME_CORE_STATUS_INVALID_HANDLE if the given subscription handle
 *            was invalid.
 */
xme_core_status_t
xme_core_dcc_unsubscribeTopic
(
	xme_core_dcc_subscriptionHandle_t subscriptionHandle
);

/**
 * \brief  Provides PortID associated with provided topicHandle 
 *
 * \param  topicHandle Handle for which PortID is requested
 * \return PortID associated with topicHandle; NULL if handle
 *         was invalid.
 */
xme_core_component_port_t
xme_core_dcc_getPortFromPublicationHandle
(
	xme_core_dcc_publicationHandle_t publicationHandle
);

/**
 * \brief  Provides ComponentID associated with provided topicHandle 
 *
 * \param  topicHandle Handle for which ComponentID is requested
 * \return ComponentID associated with topicHandle; NULL if handle
 *         was invalid.
 */
xme_core_component_t
xme_core_dcc_getComponentFromPublicationHandle
(
	xme_core_dcc_publicationHandle_t publicationHandle
);

// TODOC. See ticket #722
xme_core_component_port_t
xme_core_dcc_getPortFromSubscriptionHandle
(
	xme_core_dcc_subscriptionHandle_t subscriptionHandle
);

// TODOC. See ticket #722
xme_core_component_t
xme_core_dcc_getComponentFromSubscriptionHandle
(
	xme_core_dcc_subscriptionHandle_t subscriptionHandle
);

#endif // #ifndef XME_CORE_DCC_H
