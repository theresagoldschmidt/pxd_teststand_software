#ifndef _EASYCOM_COM_MPIOVERSERIALINTF_H_
#define _EASYCOM_COM_MPIOVERSERIALINTF_H_

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/

namespace easycom
{
	namespace com
	{
		/******************************************************************************/
		/***   Declaration of class MpiOverSerialIntf                               ***/
		/******************************************************************************/
		class MpiOverSerialIntf
		{
		public:
			// Methods //

			/// Returns the name of the serial communication channel (e.g., "COM1").
			/// @return Name of the serial communication channel.
			virtual const char* getFilename (void) const = 0;
		};

	} // namespace com
} // namespace easycom

#endif // #ifndef _EASYCOM_COM_MPIOVERSERIALINTF_H_
