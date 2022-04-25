#ifndef _EASYCOM_COM_SERIALINTF_H_
#define _EASYCOM_COM_SERIALINTF_H_

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "easycom/com/SerialParam.h"

namespace easycom
{
	namespace com
	{
		/******************************************************************************/
		/***   Declaration of class SerialIntf                                      ***/
		/******************************************************************************/
		class SerialIntf
		{
		public:
			// Methods //

			/// Tries to opens the serial communication channel.
			virtual void open (void) = 0;

			/// Closes the serial communication channel.
			virtual void close (void) = 0;

			/// Waits until read or write operations are available, or until
			/// the given timeout interval expires.
			/// @note Some functionality may not be supported on all platforms!
			/// @param read If true, the function returns as soon as at least
			/// 	one byte is available to read from the serial interface.
			/// @param write If true, the function returns as soon as at least
			/// 	one byte may be written to the serial interface.
			/// @param timeout Timeout interval in seconds after which the
			/// 	function should return even if no read or write operations
			/// 	are available.
			/// @return Returns the number of file descriptors that can be read
			/// 	or written, or 0 if the timeout interval has expired.
			virtual size_t select (const bool& read, const bool& write, const double& timeout) = 0;

			/// Reads up to \a count bytes into \a buf and returns the
			/// number of bytes actually read.
			/// @param buf Buffer to place read data in. Must be at
			/// 	least of size \a count.
			/// @param count Number of bytes to read.
			/// @return Number of bytes actually read.
			virtual size_t read (void* buf, const size_t& count) = 0;

			/// Writes up to \a count bytes from \a buf to the serial interface
			/// and returns the number of bytes actually written.
			/// @param buf Buffer with data to write. Must be at least of size
			/// 	\a count.
			/// @param count Number of bytes to write.
			/// @return Number of bytes actually written.
			virtual size_t write (const void* buf, const size_t& count) = 0;

			/// Flushes the read and/or write direction of the
			/// communication channel.
			/// @param read Flag indicating whether to flush the
			/// 	incoming direction of the communication channel.
			/// @param write Flag indicating whether to flush the
			/// 	outgoing direction of the communication channel.
			virtual bool flush (const bool& read, const bool& write) = 0;

			/// Returns the current baud rate.
			/// @return Current baud rate.
			virtual baudRate_t getBaudRate (void) const = 0;

			/// Sets the current baud rate.
			/// @note On some platforms, the new setting may only be applied
			/// 	after the connection has been re-established.
			/// @param baudRate New baud rate.
			virtual void setBaudRate (const baudRate_t& baudRate) = 0;

			/// Returns the current number of data bits.
			/// @return Current number of data bits.
			virtual dataBits_t getDataBits (void) const = 0;

			/// Sets the current number of data bits.
			/// @note On some platforms, the new setting may only be applied
			/// 	after the connection has been re-established.
			/// @param dataBits New number of data bits.
			virtual void setDataBits (const dataBits_t& dataBits) = 0;

			/// Returns the current type of flow control.
			/// @return Current type of flow control.
			virtual flowControl_t getFlowControl (void) const = 0;

			/// Sets the current type of flow control.
			/// @note On some platforms, the new setting may only be applied
			/// 	after the connection has been re-established.
			/// @param flowControl New type of flow control.
			virtual void setFlowControl (const flowControl_t& flowControl) = 0;

			/// Returns whether low latency should be used
			/// (may not be supported on all platforms).
			/// @return Whether low latency should be used.
			virtual bool getLowLatency (void) const = 0;

			/// Sets whether low latency should be used
			/// (may not be supported on all platforms).
			/// @param lowLatency Whether low latency should be used.
			virtual void setLowLatency (bool lowLatency) = 0;

			/// Returns whether and how the parity bit should be calculated.
			/// @return Whether and how the parity bit should be calculated.
			virtual parityMode_t getParityMode (void) const = 0;

			/// Sets whether and how to calculate the parity bit.
			/// @note On some platforms, the new setting may only be applied
			/// 	after the connection has been re-established.
			/// @param flowControl New parity calculation mode.
			virtual void setParityMode (const parityMode_t& parityMode) = 0;

			/// Returns the number of stop bits after a telegram.
			/// @return Number of stop bits after a telegram.
			virtual stopBits_t getStopBits (void) const = 0;

			/// Sets the number of stop bits after a telegram.
			/// @note On some platforms, the new setting may only be applied
			/// 	after the connection has been re-established.
			/// @param flowControl New number of stop bits after a telegram.
			virtual void setStopBits (const stopBits_t& stopBits) = 0;

			/// Returns the name of the serial communication channel
			/// (e.g., "COM1").
			/// @return Name of the communication channel.
			virtual const char* getName (void) const = 0;

			/// Sets the name of the communication channel (e.g., "COM1").
			/// @note The new setting will only be applied after the
			/// 	connection has been re-established.
			/// @param name New name of the communication channel.
			virtual void setName (const char* name) = 0;

			virtual void changeParameters (void) = 0;
		};

	} // namespace com
} // namespace easycom

#endif // #ifndef _EASYCOM_COM_SERIALINTF_H_
