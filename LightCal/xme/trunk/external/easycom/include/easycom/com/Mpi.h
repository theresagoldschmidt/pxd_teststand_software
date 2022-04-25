#ifndef _EASYCOM_COM_MPI_H_
#define _EASYCOM_COM_MPI_H_

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "easycom/com/Com.h"
#include "easycom/com/MpiIntf.h"

namespace easycom
{
	namespace com
	{
		/******************************************************************************/
		/***   Forward declarations                                                 ***/
		/******************************************************************************/
		class MpiImpl;

		/******************************************************************************/
		/***   Declaration of class Socket                                          ***/
		/******************************************************************************/
		class Mpi : public Com
		          , public MpiIntf
		{
		public:
			// Constructors / Destructors //

			/// Virtual destructor.
			virtual ~Mpi();

			// Methods //

			/// Tries to opens the communication channel.
			virtual void open (void);

			/// Closes the communication channel.
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
			virtual size_t read (void* buf, const mpiArea_t& area, const size_t& db, const size_t& start, const size_t& count);

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
			virtual size_t write (const void* buf, const mpiArea_t& area, const size_t& db, const size_t& start, const size_t& count);

			/// Flushes the read and/or write direction of the
			/// communication channel.
			/// @param read Flag indicating whether to flush the
			/// 	incoming direction of the communication channel.
			/// @param write Flag indicating whether to flush the
			/// 	outgoing direction of the communication channel.
			virtual bool flush (const bool& read, const bool& write);

			/// Returns the name of the communication channel.
			/// @return Name of the communication channel.
			virtual const char* getName (void) const;

			/// Returns the MPI address of the communication partner.
			/// @return MPI address of the communication partner.
			virtual const unsigned short int getMpiAddress (void) const;

		protected:
			// Constructors / Destructors //

			/// Forbidden constructor.
			/// Use static create() method of a derived class instead.
			Mpi (const unsigned short int& mpiAddress);
		};

	} // namespace com
} // namespace easycom

#endif // #ifndef _EASYCOM_COM_MPI_H_
