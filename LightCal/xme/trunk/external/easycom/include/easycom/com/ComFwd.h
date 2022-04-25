#ifndef _EASYCOM_COM_COMFWD_H_
#define _EASYCOM_COM_COMFWD_H_

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#ifndef NO_BOOST
#	include <boost/shared_ptr.hpp>
#endif // #ifndef NO_BOOST

namespace easycom
{
	namespace com
	{
		/******************************************************************************/
		/***   Forward declarations                                                 ***/
		/******************************************************************************/
		class Com;

		/******************************************************************************/
		/***   Definition of type ComPtr                                            ***/
		/******************************************************************************/
#ifdef NO_BOOST
		/// \typedef ComPtr
		///
		/// When Boost is not available, ComPtr resolves to a normal pointer.
		/// Otherwise it resolves to a smart pointer.
		typedef Com* ComPtr;
#else // #ifndef NO_BOOST
		/// \typedef ComPtr
		///
		/// Smart pointer that is used to hold dynamically allocated
		/// Com objects.
		typedef boost::shared_ptr<Com> ComPtr;
#endif // #ifndef NO_BOOST
	}
}

#endif // #ifndef _EASYCOM_COM_COMFWD_H_
