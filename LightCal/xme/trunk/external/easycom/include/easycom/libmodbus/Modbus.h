#ifndef _EASYCOM_MODBUS_MODBUS_H_
#define _EASYCOM_MODBUS_MODBUS_H_

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "easycom/libmodbus/ModbusIntf.h"

#include "easycom/com/ComFwd.h"

namespace easycom
{
	namespace modbus
	{
		/******************************************************************************/
		/***   Forward declarations                                                 ***/
		/******************************************************************************/
		class ModbusImpl;

		/******************************************************************************/
		/***   Definition of class Modbus                                           ***/
		/******************************************************************************/
		/// The Modbus class implements the Modbus communication protocol.
		/// It is actually only a wrapper for the ModbusImpl class.
		class Modbus : public ModbusIntf
		{
		public:
			// Constructors / Destructors //

			/// @brief Constructs a new Modbus communication object.
			/// @param com Communication channel to use.
			Modbus (easycom::com::ComPtr com);

			virtual ~Modbus (void);

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
			virtual int readMultipleCoils (slave_t slave, addr_t startAddr, length_t count, unit_t* dest);

			/// @brief Same as readMultipleCoils() but reads the slaves input table.
			/// @param slave Slave id where the input registers should be read.
			/// @param startAddr Start address of the first input register.
			/// @param count Number of registers that should be read.
			/// @param dest Pointer to the results.
			/// @return Returns 0 on success or a negative value in case of an error.
			virtual int readMultipleInputs (slave_t slave, addr_t startAddr, length_t count, unit_t* dest);

			/// @brief Read the holding registers in a slave and put the data into
			/// 	an array.
			/// @param slave Slave id where the holding registers should be read.
			/// @param startAddr Start address of the first holding register.
			/// @param count Number of registers that should be read.
			/// @param dest Pointer to the result array.
			/// @return Returns 0 on success or a negative value in case of an error.
			virtual int readMultipleHoldingRegisters (slave_t slave, addr_t startAddr, length_t count, unit_t* dest);

			/// @brief Read the input registers in a slave and put the data into
			/// 	an array.
			/// @param slave Slave id where the input registers should be read.
			/// @param startAddr Start address of the first input register.
			/// @param count Number of registers that should be read.
			/// @param dest Pointer to the result array.
			/// @return Returns 0 on success or a negative value in case of an error.
			virtual int readMultipleInputRegisters (slave_t slave, addr_t startAddr, length_t count, unit_t* dest);

			/// @brief Turn on or off a single coil on the slave device.
			/// @param slave Slave id where the coil should be set.
			/// @param coil_addr Address of the coil.
			/// @param state Value that should be set.
			/// @return Returns 0 on success or a negative value in case of an error.
			virtual int writeSingleCoil (slave_t slave, addr_t coil_addr, unit_t state);

			/// @brief Sets a value in one holding register in the slave device.
			/// @param slave Slave id where the register should be set.
			/// @param reg_addr Address of the register.
			/// @param value Value that should be set.
			/// @return Returns 0 on success or a negative value in case of an error.
			virtual int writeSingleHoldingRegister (slave_t slave, addr_t reg_addr, unit_t value);

			/// @brief Takes an array of values and sets or resets the coils on a
			/// 	slave appropriatly.
			/// @param slave Slave id where the coils should be set.
			/// @param startAddr Address of the first coil.
			/// @param count Number of coils that should be set.
			/// @param data Array of values that should be set.
			/// @return Returns 0 on success or a negative value in case of an error.
			virtual int writeMultipleCoils (slave_t slave, addr_t startAddr, length_t count, unit_t* data);

			/// @brief Copies the values in an array to an array on the slave.
			/// @param slave Slave id where the registers should be set.
			/// @param startAddr Address of the first register.
			/// @param numRegs Number of registers.
			/// @param data Array of values that should be set.
			/// @return Returns 0 on success or a negative value in case of an error.
			virtual int writeMultipleHoldingRegisters (slave_t slave, addr_t startAddr, length_t numRegs, unit_t* data);

			/// @brief Sets up the communication channel for Modbus RTU communication.
			/// @return Returns 0 on success or a negative value in case of an error.
			virtual int open (void);

			/// @brief Closes the communication channel.
			virtual void close (void);

		private:
			// Member variables //

			/// Implementation class.
			ModbusImpl* _impl;
		};

	} // namespace modbus
} // namespace easycom

#endif // #ifndef _EASYCOM_MODBUS_MODBUS_H_
