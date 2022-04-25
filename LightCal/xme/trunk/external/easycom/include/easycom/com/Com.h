#ifndef _EASYCOM_COM_COM_H_
#define _EASYCOM_COM_COM_H_

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "easycom/com/ComIntf.h"

#include "easycom/com/ComFwd.h"

namespace easycom
{
	namespace com
	{
		/******************************************************************************/
		/***   Forward declarations                                                 ***/
		/******************************************************************************/
		class ComImpl;

		/******************************************************************************/
		/***   Declaration of class Com                                             ***/
		/******************************************************************************/
		/// The Com class is the base class for communication channels.
		class Com : public ComIntf
		{
		public:
			// Constructors / Destructors //

			/// Virtual destructor.
			virtual ~Com (void);

			static ComPtr null;
			static Com* getPointer(ComPtr ptr);
			static bool isValid (ComPtr &ptr);
			static void destroy (ComPtr *ptr);

		protected:
			// Member variables //

			/// Implementation class.
			ComImpl* _impl;

			// Constructors / Destructors //

			/// Forbidden constructor.
			/// Use static create() method of a derived class instead.
			Com (blockedCallback_t blockedCallback = 0);
		};

	} // namespace com
} // namespace easycom

#endif // #ifndef _EASYCOM_COM_COM_H_
