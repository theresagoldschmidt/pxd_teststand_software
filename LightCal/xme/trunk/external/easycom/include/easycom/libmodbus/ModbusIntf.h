#ifndef _EASYCOM_MODBUS_MODBUSINTF_H_
#define _EASYCOM_MODBUS_MODBUSINTF_H_

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "easycom/com/ComFwd.h"

namespace easycom
{
	namespace modbus
	{
		/******************************************************************************/
		/***   Definition of class ModbusIntf                                       ***/
		/******************************************************************************/
		/// The ModbusInts class defines the Modbus API.
		class ModbusIntf
		{
		public:
			// Type definitions //

			// Basic data types.
			typedef unsigned char slave_t;
			typedef char func_t;
			typedef unsigned short addr_t;
			typedef short unit_t;
			typedef unsigned short length_t;

			// Static member constants //

			// Constants.
			static const length_t MAX_READ_HOLD_REGS;
			static const length_t MAX_READ_INPUT_REGS;
			static const length_t MAX_WRITE_COILS;
			static const length_t MAX_WRITE_REGS;

			// Protocol exceptions.
			static const int ILLEGAL_FUNCTION;
			static const int ILLEGAL_DATA_ADDRESS;
			static const int ILLEGAL_DATA_VALUE;
			static const int SLAVE_DEVICE_FAILURE;
			static const int SERVER_FAILURE;
			static const int ACKNOWLEDGE;
			static const int SLAVE_DEVICE_BUSY;
			static const int SERVER_BUSY;
			static const int NEGATIVE_ACKNOWLEDGE;
			static const int MEMORY_PARITY_ERROR;
			static const int GATEWAY_PROBLEM_PATH;
			static const int GATEWAY_PROBLEM_TARGET;

			// Local.
			static const int COMM_TIME_OUT;
			static const int PORT_SOCKET_FAILURE;
			static const int SELECT_FAILURE;
			static const int TOO_MANY_DATAS;
			static const int INVALID_CRC;
			static const int INVALID_EXCEPTION_CODE;

			// Methods //

			/// Returns the communication channel.
			/// @return Communication channel.
			easycom::com::ComPtr com (void) const;

			/// @brief Reads the boolean status of coils and sets the array elements
			/// 	in the destination to true or false.
			/// @param slave Slave id where the coils should be read.
			/// @param startAddr Start address of the first coil.
			/// @param count Number of coils that should be read.
			/// @param dest Pointer to the results.
			/// @return Returns 0 on success or a negative value in case of an error.
			virtual int readMultipleCoils (slave_t slave, addr_t startAddr, length_t count, unit_t* dest) = 0;

			/// @brief Same as readMultipleCoils() but reads the slaves input table.
			/// @param slave Slave id where the input registers should be read.
			/// @param startAddr Start address of the first input register.
			/// @param count Number of registers that should be read.
			/// @param dest Pointer to the results.
			/// @return Returns 0 on success or a negative value in case of an error.
			virtual int readMultipleInputs (slave_t slave, addr_t startAddr, length_t count, unit_t* dest) = 0;

			/// @brief Read the holding registers in a slave and put the data into
			/// 	an array.
			/// @param slave Slave id where the holding registers should be read.
			/// @param startAddr Start address of the first holding register.
			/// @param count Number of registers that should be read.
			/// @param dest Pointer to the result array.
			/// @return Returns 0 on success or a negative value in case of an error.
			virtual int readMultipleHoldingRegisters (slave_t slave, addr_t startAddr, length_t count, unit_t* dest) = 0;

			/// @brief Read the input registers in a slave and put the data into
			/// 	an array.
			/// @param slave Slave id where the input registers should be read.
			/// @param startAddr Start address of the first input register.
			/// @param count Number of registers that should be read.
			/// @param dest Pointer to the result array.
			/// @return Returns 0 on success or a negative value in case of an error.
			virtual int readMultipleInputRegisters (slave_t slave, addr_t startAddr, length_t count, unit_t* dest) = 0;

			/// @brief Turn on or off a single coil on the slave device.
			/// @param slave Slave id where the coil should be set.
			/// @param coil_addr Address of the coil.
			/// @param state Value that should be set.
			/// @return Returns 0 on success or a negative value in case of an error.
			virtual int writeSingleCoil (slave_t slave, addr_t coil_addr, unit_t state) = 0;

			/// @brief Sets a value in one holding register in the slave device.
			/// @param slave Slave id where the register should be set.
			/// @param reg_addr Address of the register.
			/// @param value Value that should be set.
			/// @return Returns 0 on success or a negative value in case of an error.
			virtual int writeSingleHoldingRegister (slave_t slave, addr_t reg_addr, unit_t value) = 0;

			/// @brief Takes an array of values and sets or resets the coils on a
			/// 	slave appropriatly.
			/// @param slave Slave id where the coils should be set.
			/// @param startAddr Address of the first coil.
			/// @param count Number of coils that should be set.
			/// @param data Array of values that should be set.
			/// @return Returns 0 on success or a negative value in case of an error.
			virtual int writeMultipleCoils (slave_t slave, addr_t startAddr, length_t count, unit_t* data) = 0;

			/// @brief Copies the values in an array to an array on the slave.
			/// @param slave Slave id where the registers should be set.
			/// @param startAddr Address of the first register.
			/// @param numRegs Number of registers.
			/// @param data Array of values that should be set.
			/// @return Returns 0 on success or a negative value in case of an error.
			virtual int writeMultipleHoldingRegisters (slave_t slave, addr_t startAddr, length_t numRegs, unit_t* data) = 0;

			/// @brief Sets up the communication channel for Modbus RTU communication.
			/// @return Returns 0 on success or a negative value in case of an error.
			virtual int open (void) = 0;

			/// @brief Closes the communication channel.
			virtual void close (void) = 0;
		};

	} // namespace modbus
} // namespace easycom

#endif // #ifndef _EASYCOM_MODBUS_MODBUSINTF_H_
