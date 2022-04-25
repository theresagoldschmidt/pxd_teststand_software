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
 *         Chat Calculator component.
 *
 * \author
 *         fortiss <chromosome@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/adv/chatCalculator.h"

#include <float.h>
#include <math.h>

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
static
bool
_xme_adv_chatCalculator_isOperationValid(char operation)
{
	return ('+' == operation || '-' == operation ||
		'/' == operation || '*' == operation);
}

static
bool
_xme_adv_chatCalculator_isDivisionByZero(char operation , double operand2)
{
	return ('/' == operation && fabs(operand2) < DBL_EPSILON);
}

static
double
_xme_adv_chatCalculator_calculateResult(double operand1, char operation,
                                                         double operand2)
{
	if ('+' == operation)
	{
		return operand1 + operand2;
	}
	if ('-' == operation)
	{
		return operand1 - operand2;
	}
	if ('*' == operation)
	{
		return operand1 * operand2;
	}
	if ('/' == operation)
	{
		// Division by zero is handled in isDivisionByZero()
		return operand1 / operand2;
	}

	// Invalid operation is handled in isOperationValid()
	return 0;
}

static
void
_xme_adv_chatCalculator_getResultString(char* in, char* out, int outSize)
{
	// Error messages
	const char* error_syntax = "The expression could not be evaluated";
	const char* error_zero = "Division by zero";
	const char* error_op = "Invalid operator";

	double operand1 , operand2 , result;

	char op;
	char* pos;

	// Parse string
	pos = strchr(in, 0x20);
	if (NULL == pos)
	{
		strcpy_s(out, outSize, error_syntax);
		return;
	}
	operand1 = atof(pos+1);

	pos = strchr(pos+1, 0x20);
	if (NULL == pos)
	{
		strcpy_s(out, outSize, error_syntax);
		return;
	}
	op = *(pos+1);

	// Ignore own announcement message
	if ('<' == op)
	{
		out[0] = 0;
		return;
	}

	pos = strchr(pos+1, 0x20);
	if (NULL == pos)
	{
		strcpy_s(out, outSize, error_syntax);
		return;
	}
	operand2 = atof(pos+1);

	// Check for operator and division by zero
	if (!_xme_adv_chatCalculator_isOperationValid(op))
	{
		strcpy_s(out, outSize, error_op);
		return;
	}
	if (_xme_adv_chatCalculator_isDivisionByZero(op, operand2))
	{
		strcpy_s(out, outSize, error_zero);
		return;
	}

	// Calculate result
	result =
		_xme_adv_chatCalculator_calculateResult(operand1 , op, operand2);

	// Output result
	sprintf_s
	(
		out, outSize, "%lf %c %lf = %lf",
		operand1, op, operand2, result
	);
}

static
void
_xme_adv_chatCalculator_receiveDataCallback(xme_hal_sharedPtr_t dataHandle, void* userData)
{
	xme_adv_chatCalculator_configStruct_t* config = (xme_adv_chatCalculator_configStruct_t*)userData;

	char* input = (char*)xme_hal_sharedPtr_getPointer(dataHandle);
	int size = xme_hal_sharedPtr_getSize(dataHandle);

	// Output data
	char output[512];

	// Sanitize input
	input[size -1] = 0;

	// Strip person’s name
	input = strchr(input , ':');
	if (NULL == input)
	{
	// Invalid message format
	return;
	}
	input += 2;

	if (0 != strncmp(input , "!calc ", 6))
	{
	// Not a calculation command
	return;
	}

	_xme_adv_chatCalculator_getResultString
	(
		input , output , sizeof(output)
	);

	if (0 != strlen(output))
	{
		xme_core_dcc_sendTopicData
		(
			config ->publicationHandle ,
			(void*)output , strlen(output)+1
		);
	}
}

static
void
_xme_adv_chatCalculator_taskCallback(void* userData)
{
	xme_adv_chatCalculator_configStruct_t* config = (xme_adv_chatCalculator_configStruct_t*)userData;

	{
		static const char* message = "The calculator is waiting for "
			"commands.\nUsage: !calc <operand1> <operation> <operand2>";
		xme_core_dcc_sendTopicData(config->publicationHandle, (void*)message, strlen(message)+1);
	}
}

xme_core_status_t
xme_adv_chatCalculator_create(xme_adv_chatCalculator_configStruct_t* config)
{
	// Publish topic
	config->publicationHandle =
		xme_core_dcc_publishTopic
		(
			config->topic,
			XME_CORE_MD_EMPTY_META_DATA,
			false,
			NULL
		);

	// Check for errors
	if (XME_CORE_DCC_INVALID_PUBLICATION_HANDLE == config->publicationHandle)
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	// Subscribe to topic
	config->subscriptionHandle =
		xme_core_dcc_subscribeTopic
		(
			config->topic,
			XME_CORE_MD_EMPTY_META_DATA,
			false,
			_xme_adv_chatCalculator_receiveDataCallback,
			config
		);

	// Check for errors
	if (XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE == config->subscriptionHandle)
	{
		// Unpublish topic since subscription does not work
		xme_core_dcc_unpublishTopic(config->publicationHandle);
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
xme_adv_chatCalculator_activate(xme_adv_chatCalculator_configStruct_t* config)
{
	// Start task
	config->taskHandle =
		xme_core_resourceManager_scheduleTask
		(
			1000,
			30000,
			XME_HAL_SCHED_PRIORITY_NORMAL,
			_xme_adv_chatCalculator_taskCallback,
			config
		);

	// Check for errors
	if (XME_CORE_RESOURCEMANAGER_TASK_INFEASIBLE == config->taskHandle)
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	return XME_CORE_STATUS_SUCCESS;
}

void
xme_adv_chatCalculator_deactivate(xme_adv_chatCalculator_configStruct_t* config)
{
	// Remove the task
	xme_core_resourceManager_killTask(config->taskHandle);
}

void
xme_adv_chatCalculator_destroy(xme_adv_chatCalculator_configStruct_t* config)
{
	// Unsubscribe topic
	xme_core_dcc_unsubscribeTopic(config->subscriptionHandle);

	// Unpublish topic
	xme_core_dcc_unpublishTopic(config->publicationHandle);
}
