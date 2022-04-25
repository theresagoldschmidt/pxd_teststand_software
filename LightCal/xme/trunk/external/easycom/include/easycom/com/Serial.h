#ifndef _EASYCOM_COM_SERIAL_H_
#define _EASYCOM_COM_SERIAL_H_

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "easycom/com/Com.h"
#include "easycom/com/SerialIntf.h"

namespace easycom
{
	namespace com
	{
		/******************************************************************************/
		/***   Forward declarations                                                 ***/
		/******************************************************************************/
		class SerialImpl;

		/******************************************************************************/
		/***   Declaration of class Serial                                          ***/
		/******************************************************************************/
		class Serial : public Com
		             , public SerialIntf
		{
		public:
			// Constructors / Destructors //

			/// Virtual destructor.
			virtual ~Serial (void);

			// Methods //

			/// Creates a new Serial object.
			/// @param filename Name of the serial interface (e.g., "COM1").
			/// @param baudRate Baud rate of the serial line.
			/// @param parityMode Whether and how the parity bit should be calculated.
			/// @param dataBits Number of data bits in a telegram.
			/// @param stopBits Number of stop bits after a telegram.
			/// @param flowControl Type of flow control, if desired.
			/// @param lowLatency Whether low latency should be used (may not
			/// 	be supported on all platforms).
			/// @param blockedHandler Function to be periodically called when
			/// 	a blocking I/O operation is in progress.
			static ComPtr create (
				const char* filename,
				const baudRate_t& baudRate = BAUDRATE_9600BPS,
				const dataBits_t& dataBits = DATABITS_8BITS,
				const flowControl_t& flowControl = FLOWCONTROL_OFF,
				const parityMode_t& parityMode = PARITY_NOPARITY,
				const stopBits_t& stopBits = STOPBITS_1BIT,
				const bool& lowLatency = false,
				Com::blockedCallback_t blockedHandler = 0
			);

			/// Tries to opens the serial communication channel.
			virtual void open (void);

			/// Closes the serial communication channel.
			virtual void close (void);

			/// Returns whether the communication channel is currently open.
			/// @return Whether the communication channel is currently open.
			virtual bool isConnected (void) const;

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
			virtual size_t select (const bool& read, const bool& write, const double& timeout);

			/// Reads up to \a count bytes into \a buf and returns the
			/// number of bytes actually read.
			/// @param buf Buffer to place read data in. Must be at
			/// 	least of size \a count.
			/// @param count Number of bytes to read.
			/// @return Number of bytes actually read.
			virtual size_t read (void* buf, const size_t& count);

			/// Writes up to \a count bytes from \a buf to the serial interface
			/// and returns the number of bytes actually written.
			/// @param buf Buffer with data to write. Must be at least of size
			/// 	\a count.
			/// @param count Number of bytes to write.
			/// @return Number of bytes actually written.
			virtual size_t write (const void* buf, const size_t& count);

			/// Flushes the read and/or write direction of the
			/// communication channel.
			/// @param read Flag indicating whether to flush the
			/// 	incoming direction of the communication channel.
			/// @param write Flag indicating whether to flush the
			/// 	outgoing direction of the communication channel.
			virtual bool flush (const bool& read, const bool& write);

			/// Returns the current baud rate.
			/// @return Current baud rate.
			baudRate_t getBaudRate (void) const;

			/// Sets the current baud rate.
			/// @note On some platforms, the new setting may only be applied
			/// 	after the connection has been re-established.
			/// @param baudRate New baud rate.
			void setBaudRate (const baudRate_t& baudRate);

			/// Returns the current number of data bits.
			/// @return Current number of data bits.
			dataBits_t getDataBits (void) const;

			/// Sets the current number of data bits.
			/// @note On some platforms, the new setting may only be applied
			/// 	after the connection has been re-established.
			/// @param dataBits New number of data bits.
			void setDataBits (const dataBits_t& dataBits);

			/// Returns the current type of flow control.
			/// @return Current type of flow control.
			flowControl_t getFlowControl (void) const;

			/// Sets the current type of flow control.
			/// @note On some platforms, the new setting may only be applied
			/// 	after the connection has been re-established.
			/// @param flowControl New type of flow control.
			void setFlowControl (const flowControl_t& flowControl);

			/// Returns whether low latency should be used
			/// (may not be supported on all platforms).
			/// @return Whether low latency should be used.
			bool getLowLatency (void) const;

			/// Sets whether low latency should be used
			/// (may not be supported on all platforms).
			/// @param lowLatency Whether low latency should be used.
			void setLowLatency (bool lowLatency);

			/// Returns whether and how the parity bit should be calculated.
			/// @return Whether and how the parity bit should be calculated.
			parityMode_t getParityMode (void) const;

			/// Sets whether and how to calculate the parity bit.
			/// @note On some platforms, the new setting may only be applied
			/// 	after the connection has been re-established.
			/// @param flowControl New parity calculation mode.
			void setParityMode (const parityMode_t& parityMode);

			/// Returns the number of stop bits after a telegram.
			/// @return Number of stop bits after a telegram.
			stopBits_t getStopBits (void) const;

			/// Sets the number of stop bits after a telegram.
			/// @note On some platforms, the new setting may only be applied
			/// 	after the connection has been re-established.
			/// @param flowControl New number of stop bits after a telegram.
			void setStopBits (const stopBits_t& stopBits);

			/// Returns the name of the serial communication channel
			/// (e.g., "COM1").
			/// @return Name of the communication channel.
			virtual const char* getName (void) const;

			/// Sets the name of the communication channel (e.g., "COM1").
			/// @note The new setting will only be applied after the
			/// 	connection has been re-established.
			/// @param name New name of the communication channel.
			void setName (const char* name);

			void changeParameters (void);

			/// Detects the set of communication ports available in the system and stores it
			/// in the \a buf parameter as a concatenated list of null-terminated strings.
			/// The last item in the list is terminated with an extra null character.
			/// Note that if the list is empty, there is only a single null character
			/// for list termination.
			/// The \a size parameter specifies the length of the buffer. If an item does not
			/// fit into the buffer, it is omitted. The function returns the number of bytes
			/// actually required to store the list of communication ports. If the \a count
			/// parameter is specified, it is set to the number of items added to the list.
			/// @note Since the runtime of the function can be quite high, it is not
			/// 	recommended to call it with an initial buffer size of zero to determine
			/// 	the number of bytes required. Instead, call it with a resonable sized
			/// 	buffer and only allocate a larger buffer if more is required.
			/// @param buf Buffer to write the names of the detected communication ports to.
			/// @param size Size of \a buf.
			/// @param count Optional parameter receiving the number of items printed to \a buf.
			/// @return Number of bytes (i.e., characters) actually required for \a buf.
			static size_t enumeratePorts (/*non-const*/ char* buf, size_t size, unsigned int *count = 0);

#ifdef WIN32
			/// Determines the file name of a serial port given its name
			/// in the Windows device manager.
			/// @note The caller is responsible for calling free() on the
			/// 	return value if it is not used any more.
			/// @param name Name of device in device manager.
			/// @return Filename of serial port (e.g., "COM1").
			static char* getDeviceByName (const char* name);
#endif // #ifdef WIN32

		protected:
			// Constructors / Destructors //

			/// Creates a new Serial object.
			/// @param filename Name of the serial interface (e.g., "COM1").
			/// @param baudRate Baud rate of the serial line.
			/// @param parityMode Whether and how the parity bit should be calculated.
			/// @param dataBits Number of data bits in a telegram.
			/// @param stopBits Number of stop bits after a telegram.
			/// @param flowControl Type of flow control, if desired.
			/// @param lowLatency Whether low latency should be used (may not
			/// 	be supported on all platforms).
			/// @param blockedHandler Function to be periodically called when
			/// 	a blocking I/O operation is in progress.
			Serial (
				const char* filename,
				const baudRate_t& baudRate,
				const dataBits_t& dataBits,
				const flowControl_t& flowControl,
				const parityMode_t& parityMode,
				const stopBits_t& stopBits,
				const bool& lowLatency,
				Com::blockedCallback_t blockedHandler = 0
			);
		};

	} // namespace com
} // namespace easycom

#endif // #ifndef _EASYCOM_COM_SERIAL_H_
