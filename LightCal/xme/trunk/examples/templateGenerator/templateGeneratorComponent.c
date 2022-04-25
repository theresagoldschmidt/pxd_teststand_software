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
 *         Chat example project.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "templateGeneratorComponent.h"
#include "xme/hal/mem.h"
#include "xme/hal/sharedPtr.h"

#include <ctype.h>

/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/
#define _TEMPLATEGENERATORCOMPONENT_INPUT_HEADER_TEMPLATE "%s/componentTemplate.h.in"
#define _TEMPLATEGENERATORCOMPONENT_INPUT_SOURCE_TEMPLATE "%s/componentTemplate.c.in"
#define _TEMPLATEGENERATORCOMPONENT_INPUT_CMAKE_TEMPLATE "%s/CMakeListsTemplate.txt.in"
#define _TEMPLATEGENERATORCOMPONENT_OUTPUT_HEADER_TEMPLATE "%s/xme/%s/%s.h"
#define _TEMPLATEGENERATORCOMPONENT_OUTPUT_SOURCE_TEMPLATE "%s/xme/%s/%s.c"
#define _TEMPLATEGENERATORCOMPONENT_OUTPUT_CMAKE_TEMPLATE "%s/xme/%s/CMakeLists.txt"

/******************************************************************************/
/***   Static variables                                                     ***/
/******************************************************************************/
static const char* classes[] =
{
	"adv",
	"prim",
	"hal",
	NULL
};

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
static
xme_hal_sharedPtr_t
_templateGeneratorComponent_inputString(unsigned int minLength, const char** choices)
{
	char temp[1024];
	size_t len;

	while (fgets(temp, 1024, stdin) != NULL)
	{
		len = strlen(temp);
		if (len < minLength+1)
		{
			XME_LOG(XME_LOG_NOTE, "Too short. Please try again:\n > ");
		}
		else if (temp[len-1] == '\n')
		{
			xme_hal_sharedPtr_t handle;
			char* memory;
			bool valid = true;

			temp[len-1] = 0;

			if (NULL != choices)
			{
				const char** c = choices;
				const char* choice;

				valid = false;
				while (NULL != (choice = *c++))
				{
					if (0 == strcmp(temp, choice))
					{
						valid = true;
						break;
					}
				}
			}

			if (!valid)
			{
				XME_LOG(XME_LOG_NOTE, "Invalid value. Please try again:\n > ");
				continue;
			}

			handle = xme_hal_sharedPtr_create(len);
			XME_ASSERT_RVAL(XME_HAL_SHAREDPTR_INVALID_POINTER != handle, XME_HAL_SHAREDPTR_INVALID_POINTER);

			memory = (char*)xme_hal_sharedPtr_getPointer(handle);
			XME_ASSERT_RVAL(NULL != memory, XME_HAL_SHAREDPTR_INVALID_POINTER);

			xme_hal_mem_copy(memory, temp, len);

			return handle;
		}
	}

	return XME_HAL_SHAREDPTR_INVALID_POINTER;
}

static
void
_templateGeneratorComponent_processFile(templateGeneratorComponent_configStruct_t* config, const char* inFilename, const char* outFilename, bool append)
{
	FILE* infile = NULL;
	FILE* outfile = NULL;
	char temp[1024];
	unsigned int line = 0;

	if (!append)
	{
		// Check whether output file already exists
		fopen_s(&outfile, outFilename, "r");
		if (NULL != outfile)
		{
			char c[2];

			fclose(outfile);

			XME_LOG(XME_LOG_NOTE, "File \"%s\" already exists. Overwrite? [ y | n ]\n > ", outFilename);

			while (fgets(c, 2, stdin) != NULL)
			{
				if ('n' == tolower(c[0]))
				{
					return;
				}
				else if ('y' == tolower(c[0]))
				{
					break;
				}
			}
		}
	}

	fopen_s(&infile, inFilename, "r");
	XME_CHECK_REC
	(
		NULL != infile,
		,
		{
			XME_LOG(XME_LOG_ERROR, "Unable to open template file \"%s\"! The respective file will not be generated!\n", inFilename);
		}
	);

	fopen_s(&outfile, outFilename, append ? "a" : "w");
	XME_CHECK_REC
	(
		NULL != outfile,
		,
		{
			XME_LOG(XME_LOG_ERROR, "Unable to open output file \"%s\"! The respective file will not be generated!\n", outFilename);
		}
	);

	while (fgets(temp, 1024, infile) != NULL)
	{
		const char* pos = 0;
		char* found;

		line++;

		do
		{
			const char* at = (const char*)((int)temp+pos);
			size_t processed = 0;

			found = (char*)strstr(at, "@");
			if (NULL != found)
			{
				char* found2;

				found[0] = 0; // Replace '@' by '\0'
				fputs(at, outfile); // Print prefix to file
				//processed += strlen(at);

				found2 = (char*)strstr(found+1, "@"); // Find next '@'
				if (NULL == found2)
				{
					XME_LOG(XME_LOG_WARNING, "Unbalanced number of '@' characters in %s:%d!\n", inFilename, line);
					fputs("@", outfile);
					//processed++;
				}
				else
				{
					found2[0] = 0; // Replace '@' by '\0'
					if (0 == strcmp(found+1, "COMPONENT_NAME_PRETTY"))
					{
						fputs((const char*)xme_hal_sharedPtr_getPointer(config->componentNamePretty), outfile);
					}
					else if (0 == strcmp(found+1, "COMPONENT_NAME_PREPROCESSOR"))
					{
						fputs((const char*)xme_hal_sharedPtr_getPointer(config->componentNamePreprocessor), outfile);
					}
					else if (0 == strcmp(found+1, "COMPONENT_NAME_IDENTIFIER"))
					{
						fputs((const char*)xme_hal_sharedPtr_getPointer(config->componentNameIdentifier), outfile);
					}
					else if (0 == strcmp(found+1, "COMPONENT_CLASS"))
					{
						fputs((const char*)xme_hal_sharedPtr_getPointer(config->componentClass), outfile);
					}
					else if (0 == strcmp(found+1, "COMPONENT_CLASS_UPPER"))
					{
						fputs((const char*)xme_hal_sharedPtr_getPointer(config->componentClassUpper), outfile);
					}
					else if (0 == strcmp(found+1, "AUTHOR_NAME"))
					{
						fputs((const char*)xme_hal_sharedPtr_getPointer(config->authorName), outfile);
					}
					else if (0 == strcmp(found+1, "AUTHOR_MAIL"))
					{
						fputs((const char*)xme_hal_sharedPtr_getPointer(config->authorMail), outfile);
					}
					else
					{
						XME_LOG(XME_LOG_WARNING, "Unknown key \"@%s@\" in %s:%d, leaving as-is!\n", found+1, inFilename, line);
						fputs("@", outfile);
						fputs(found+1, outfile);
						fputs("@", outfile);
					}
					
					processed += (found2 - at) + 1;
				}
			}
			else
			{
				// Print the rest of the line
				fputs(at, outfile);
				processed += strlen(at);
			}

			pos += processed;
		}
		while (NULL != found);
	}
	fclose(outfile);
	fclose(infile);
}

static
void
_templateGeneratorComponent_taskCallback(void* userData)
{
	templateGeneratorComponent_configStruct_t* config = (templateGeneratorComponent_configStruct_t*)userData;
	const char* pathToXmeRoot = NULL;
	const char* pathToTemplates = NULL;

	// Determine path to XME_ROOT to place the generated files at the correct location
	{
		FILE* file = NULL;
		fopen_s(&file, "templateGenerator.exe", "r");
		if (NULL != file)
		{
			fclose(file);

			fopen_s(&file, "../../src/xme/CMakeLists.txt", "r");
			if (NULL != file)
			{
				fclose(file);

				// We are running in a binary distribution's bin directory
				pathToXmeRoot = "../../src";
			}
			else
			{
				// We are probably running in a build directory's target/<Configuration> directory
				pathToXmeRoot = "../../../../..";
			}
		}
		else
		{
			// We are probably running from a build directory (launched from within the IDE)
			pathToXmeRoot = "../../..";
		}
	}

	// Determine path to templates
	{
		FILE* file = NULL;
		char inFilename[1024];

#ifdef WIN32
		sprintf_s
		(
			inFilename, 1024,
#else // #ifdef WIN32
		sprintf
		(
			buf,
#endif // #ifdef WIN32
			_TEMPLATEGENERATORCOMPONENT_INPUT_HEADER_TEMPLATE,
			"."
		);

		fopen_s(&file, inFilename, "r");
		if (NULL != file)
		{
			fclose(file);
			pathToTemplates = ".";
		}
		else
		{
#ifdef WIN32
			sprintf_s
			(
				inFilename, 1024,
#else // #ifdef WIN32
			sprintf
			(
				buf,
#endif // #ifdef WIN32
				_TEMPLATEGENERATORCOMPONENT_INPUT_HEADER_TEMPLATE,
				"../.."
			);

			fopen_s(&file, inFilename, "r");
			if (NULL != file)
			{
				fclose(file);
				pathToTemplates = "../..";
			}
			else
			{
				XME_LOG(XME_LOG_ERROR, "Unable to find code templates! Please make sure the working directory of this application is set up correctly!\n");
				return;
			}
		}
	}

	XME_LOG(XME_LOG_NOTE, "Welcome to the Template Generator!\n");
	XME_LOG(XME_LOG_NOTE, "This application will assist you in creating your own CHROMOSOME components.\n");
	XME_LOG(XME_LOG_NOTE, "It will ask you some questions about the new component\nand generate corresponding C source and header files.\n");
	XME_LOG(XME_LOG_NOTE, "Close this window to cancel at any point in time.\n");
	XME_LOG(XME_LOG_NOTE, "\n");

	XME_LOG(XME_LOG_NOTE, "Please enter the name of the new component to create, words separated by spaces\n(e.g., \"Random Number Generator\"):\n > ");
	config->componentName = _templateGeneratorComponent_inputString(3, NULL);
	
	XME_LOG(XME_LOG_NOTE, "Please enter the class of the new component\n(one of \"adv\", \"prim\" and \"hal\"):\n > ");
	config->componentClass = _templateGeneratorComponent_inputString(0, classes);

	XME_LOG(XME_LOG_NOTE, "Please enter your full name\n(e.g., \"John Doe\"; it will appear as author of the generated files):\n > ");
	config->authorName = _templateGeneratorComponent_inputString(0, NULL);

	XME_LOG(XME_LOG_NOTE, "Please enter your e-mail address\n(e.g., \"john.doe@example.com\"; it will appear as contact to the author):\n > ");
	config->authorMail = _templateGeneratorComponent_inputString(0, NULL);

	config->componentNamePretty = config->componentName;

	// Preprocessor: all upper-case, non-alphanumeric characters replaced by underscores.
	// C identifier: first letters of every word uppercase except first work, spaces stripped.
	{
		const char* s;
		char* dp;
		char* di;
		char c;
		char firstLetter = 1;
		char firstLetterInWord = 1;
		uint16_t size;

		size = xme_hal_sharedPtr_getSize(config->componentName);

		config->componentNamePreprocessor = xme_hal_sharedPtr_create(size);
		config->componentNameIdentifier = xme_hal_sharedPtr_create(size);

		s = (const char*)xme_hal_sharedPtr_getPointer(config->componentName);

		dp = (char*)xme_hal_sharedPtr_getPointer(config->componentNamePreprocessor);
		di = (char*)xme_hal_sharedPtr_getPointer(config->componentNameIdentifier);

		while (c = *s++)
		{
			if (isalpha(c) || isdigit(c))
			{
				*dp++ = toupper(c);

				if (firstLetter)
				{
					firstLetter = 0;
					firstLetterInWord = 0;
					*di++ = tolower(c);
				}
				else if (firstLetterInWord)
				{
					firstLetterInWord = 0;
					*di++ = toupper(c);
				}
				else
				{
					*di++ = c;
				}
			}
			else
			{
				*dp++ = '_';
				firstLetterInWord = 1;
			}
		}

		*dp = 0;
		*di = 0;
	}

	{
		const char* s;
		char* dp;
		char c;
		uint16_t size;

		size = xme_hal_sharedPtr_getSize(config->componentClass);

		config->componentClassUpper = xme_hal_sharedPtr_create(size);

		s = (const char*)xme_hal_sharedPtr_getPointer(config->componentClass);

		dp = (char*)xme_hal_sharedPtr_getPointer(config->componentClassUpper);

		while (c = *s++)
		{
			*dp++ = toupper(c);
		}

		*dp = 0;
	}

	{
		char inFilename[1024];
		char outFilename[1024];

#ifdef WIN32
		sprintf_s
		(
			inFilename, 1024,
#else // #ifdef WIN32
		sprintf
		(
			inFilename,
#endif // #ifdef WIN32
			_TEMPLATEGENERATORCOMPONENT_INPUT_HEADER_TEMPLATE,
			pathToTemplates
		);

#ifdef WIN32
		sprintf_s
		(
			outFilename, 1024,
#else // #ifdef WIN32
		sprintf
		(
			outFilename,
#endif // #ifdef WIN32
			_TEMPLATEGENERATORCOMPONENT_OUTPUT_HEADER_TEMPLATE,
			pathToXmeRoot,
			(const char*)xme_hal_sharedPtr_getPointer(config->componentClass),
			(const char*)xme_hal_sharedPtr_getPointer(config->componentNameIdentifier)
		);

		XME_LOG(XME_LOG_NOTE, "Generating \"%s\"...\n", outFilename);
		_templateGeneratorComponent_processFile(config, inFilename, outFilename, false);

#ifdef WIN32
		sprintf_s
		(
			inFilename, 1024,
#else // #ifdef WIN32
		sprintf
		(
			inFilename,
#endif // #ifdef WIN32
			_TEMPLATEGENERATORCOMPONENT_INPUT_SOURCE_TEMPLATE,
			pathToTemplates
		);

#ifdef WIN32
		sprintf_s
		(
			outFilename, 1024,
#else // #ifdef WIN32
		sprintf
		(
			outFilename,
#endif // #ifdef WIN32
			_TEMPLATEGENERATORCOMPONENT_OUTPUT_SOURCE_TEMPLATE,
			pathToXmeRoot,
			(const char*)xme_hal_sharedPtr_getPointer(config->componentClass),
			(const char*)xme_hal_sharedPtr_getPointer(config->componentNameIdentifier)
		);

		XME_LOG(XME_LOG_NOTE, "Generating \"%s\"...\n", outFilename);
		_templateGeneratorComponent_processFile(config, inFilename, outFilename, false);

#ifdef WIN32
		sprintf_s
		(
			inFilename, 1024,
#else // #ifdef WIN32
		sprintf
		(
			inFilename,
#endif // #ifdef WIN32
			_TEMPLATEGENERATORCOMPONENT_INPUT_CMAKE_TEMPLATE,
			pathToTemplates
		);

#ifdef WIN32
		sprintf_s
		(
			outFilename, 1024,
#else // #ifdef WIN32
		sprintf
		(
			outFilename,
#endif // #ifdef WIN32
			_TEMPLATEGENERATORCOMPONENT_OUTPUT_CMAKE_TEMPLATE,
			pathToXmeRoot,
			(const char*)xme_hal_sharedPtr_getPointer(config->componentClass)
		);

		XME_LOG(XME_LOG_NOTE, "Patching \"%s\"...\n", outFilename);
		_templateGeneratorComponent_processFile(config, inFilename, outFilename, true);
	}

	XME_LOG(XME_LOG_NOTE, "Finished!\n");
}

xme_core_status_t
templateGeneratorComponent_create(templateGeneratorComponent_configStruct_t* config)
{
	// Nothing to do

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
templateGeneratorComponent_activate(templateGeneratorComponent_configStruct_t* config)
{
	config->taskHandle = xme_core_resourceManager_scheduleTask(0, 0, XME_HAL_SCHED_PRIORITY_NORMAL, _templateGeneratorComponent_taskCallback, config);

	// Check for errors
	if (XME_CORE_RESOURCEMANAGER_TASK_INFEASIBLE == config->taskHandle)
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	return XME_CORE_STATUS_SUCCESS;
}

void
templateGeneratorComponent_deactivate(templateGeneratorComponent_configStruct_t* config)
{
	xme_core_resourceManager_killTask(config->taskHandle);
}

void
templateGeneratorComponent_destroy(templateGeneratorComponent_configStruct_t* config)
{
	// Nothing to do
}
