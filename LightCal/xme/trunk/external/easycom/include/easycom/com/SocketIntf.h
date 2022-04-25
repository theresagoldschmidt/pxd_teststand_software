#ifndef _EASYCOM_COM_SOCKETINTF_H_
#define _EASYCOM_COM_SOCKETINTF_H_

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "easycom/com/Com.h"

namespace easycom
{
	namespace com
	{
		/******************************************************************************/
		/***   Declaration of class SocketIntf                                      ***/
		/******************************************************************************/
		class SocketIntf
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

			/// Returns the host name the socket is bound to.
			/// @return Host name the socket is bound to.
			virtual const char* getHost (void) const = 0;

			/// Returns the port number the socket is bound to.
			/// @return Port number the socket is bound to.
			virtual unsigned short int getPort (void) const = 0;
		};

	} // namespace com
} // namespace easycom

#endif // #ifndef _EASYCOM_COM_SOCKETINTF_H_
