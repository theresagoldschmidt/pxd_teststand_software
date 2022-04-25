#ifndef _EASYCOM_COM_MPIINTF_H_
#define _EASYCOM_COM_MPIINTF_H_

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "easycom/com/Com.h"

namespace easycom
{
	namespace com
	{
		/******************************************************************************/
		/***   Type definitions                                                     ***/
		/******************************************************************************/
		// Defined according to libnodave.
		typedef enum
		{
			AREA_RAW = 0x00,             ///< Just the raw memory.
			AREA_SYS_INFO = 0x03,        ///< System info of 200 family.
			AREA_SYS_FLAGS = 0x05,       ///< System flags of 200 family.
			AREA_ANALOG_INPUT = 0x06,    ///< Analog inputs of 200 family.
			AREA_ANALOG_OUTPUT = 0x07,   ///< Analog outputs of 200 family.
			AREA_COUNTER = 0x1C,         ///< S7 counters.
			AREA_TIMERS = 0x1D,          ///< S7 timers.
			AREA_COUNTER_200 = 0x1E,     ///< IEC counters (200 family).
			AREA_TIMER_200 = 0x1F,       ///< IEC timers (200 family).
			AREA_DIRECT = 0x80,          ///< Direct peripheral access.
			AREA_INPUT = 0x81,           ///< Digital inputs.
			AREA_OUTPUT = 0x82,          ///< Digital outputs.
			AREA_FLAGS = 0x83,           ///< Flags.
			AREA_DATA_BLOCK = 0x84,      ///< Data blocks.
			AREA_DATA_BLOCK_INST = 0x85, ///< Data block instances.
		}
		mpiArea_t;

		/******************************************************************************/
		/***   Declaration of class MpiIntf                                         ***/
		/******************************************************************************/
		class MpiIntf
		{
		public:
			// Methods //

			/// Tries to opens the communication channel.
			virtual void open (void) = 0;

			/// Closes the communication channel.
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

			/// Reads up to \a count bytes into \a buf and returns the
			/// number of bytes actually read.
			/// @param buf Buffer to place read data in. Must be at
			/// 	least of size \a count.
			/// @param area MPI area to read.
			/// @param db Number of the data block to be used. Set it to
			///        zero for area types other than AREA_DATA_BLOCK(_INST).
			/// @param start Start offset for read operation.
			/// @param count Number of bytes to read.
			/// @return Number of bytes actually read.
			virtual size_t read (void* buf, const mpiArea_t& area, const size_t& db, const size_t& start, const size_t& count) = 0;

			/// Writes up to \a count bytes from \a buf to the serial interface
			/// and returns the number of bytes actually written.
			/// @param buf Buffer with data to write. Must be at least of size
			/// 	\a count.
			/// @param area MPI area to write.
			/// @param db Number of the data block to be used. Set it to
			///        zero for area types other than AREA_DATA_BLOCK(_INST).
			/// @param start Start offset for write operation.
			/// @param count Number of bytes to write.
			/// @return Number of bytes actually written.
			virtual size_t write (const void* buf, const mpiArea_t& area, const size_t& db, const size_t& start, const size_t& count) = 0;

			/// Flushes the read and/or write direction of the
			/// communication channel.
			/// @param read Flag indicating whether to flush the
			/// 	incoming direction of the communication channel.
			/// @param write Flag indicating whether to flush the
			/// 	outgoing direction of the communication channel.
			virtual bool flush (const bool& read, const bool& write) = 0;

			/// Returns the name of the communication channel.
			/// @return Name of the communication channel.
			virtual const char* getName (void) const = 0;

			/// Returns the MPI address of the communication partner.
			/// @return MPI address of the communication partner.
			virtual const unsigned short int getMpiAddress (void) const = 0;
		};

	} // namespace com
} // namespace easycom

#endif // #ifndef _EASYCOM_COM_MPIINTF_H_
