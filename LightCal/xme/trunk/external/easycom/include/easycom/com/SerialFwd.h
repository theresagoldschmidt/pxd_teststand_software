#ifndef _EASYCOM_COM_SERIALFWD_H_
#define _EASYCOM_COM_SERIALFWD_H_
/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include <boost/shared_ptr.hpp>

namespace easycom
{
	namespace com
	{
		/******************************************************************************/
		/***   Forward declarations                                                 ***/
		/******************************************************************************/
		class Serial;

		/******************************************************************************/
		/***   Definition of type SerialPtr                                         ***/
		/******************************************************************************/
		/// Shared pointer that is used to hold dynamically allocated
		/// Serial objects.
		typedef boost::shared_ptr<Serial> SerialPtr;
	}
}

#endif // #ifndef _EASYCOM_COM_SERIALFWD_H_
