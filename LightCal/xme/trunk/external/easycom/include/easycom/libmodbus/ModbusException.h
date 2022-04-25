#ifndef _EASYCOM_MODBUS_MODBUSEXCEPTION_H_
#define _EASYCOM_MODBUS_MODBUSEXCEPTION_H_

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "easycom/com/ComException.h"

namespace easycom
{
	namespace modbus
	{
		/******************************************************************************/
		/***   Definition of class ModbusException                                  ***/
		/******************************************************************************/
		/// The ModbusException class is used to signal error conditions in the Modbus
		/// protocol.
		class ModbusException : public easycom::com::ComException
		{
		public:
			// Constructors / Destructors //

			/// Creates a new ModbusException object with the given message string.
			ModbusException (const char* message);
		};

	} // namespace modbus
} // namespace easycom

#endif // #ifndef _EASYCOM_MODBUS_MODBUSEXCEPTION_H_
