#ifndef _EASYCOM_COM_TIMEOUTEXCEPTION_H_
#define _EASYCOM_COM_TIMEOUTEXCEPTION_H_

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "easycom/com/ComException.h"

namespace easycom
{
	namespace com
	{
/******************************************************************************/
/***   Declaration of class TimeoutException                                ***/
/******************************************************************************/
		class TimeoutException : public ComException
		{
		public:
			TimeoutException();
			
			virtual ~TimeoutException() throw();
			
		protected:
			
		private:
			
		};
	}
}

#endif // #ifndef _EASYCOM_COM_TIMEOUTEXCEPTION_H_
