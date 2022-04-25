#ifndef _EASYCOM_COM_COMEXCEPTION_H_
#define _EASYCOM_COM_COMEXCEPTION_H_

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "easycom/exception/Exception.h"

namespace easycom
{
	namespace com
	{
		/******************************************************************************/
		/***   Declaration of class ComException                                    ***/
		/******************************************************************************/
		class ComException : public util::Exception
		{
		public:
			ComException(const std::string& what_arg);
			
			virtual ~ComException() throw();
		};
	}
}

#endif // #ifndef _EASYCOM_COM_COMEXCEPTION_H_
