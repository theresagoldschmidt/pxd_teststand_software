#ifndef _EASYCOM_COM_MPIOVERSERIAL_H_
#define _EASYCOM_COM_MPIOVERSERIAL_H_

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "easycom/com/Mpi.h"
#include "easycom/com/MpiOverSerialIntf.h"

namespace easycom
{
	namespace com
	{
		/******************************************************************************/
		/***   Forward declarations                                                 ***/
		/******************************************************************************/
		class MpiOverSerialImpl;

		/******************************************************************************/
		/***   Declaration of class MpiOverSerial                                   ***/
		/******************************************************************************/
		class MpiOverSerial : public Mpi
		                    , public MpiOverSerialIntf
		{
		public:
			// Constructors / Destructors //

			/// Virtual destructor.
			virtual ~MpiOverSerial();

			// Methods //

			/// Creates a new MPI communication channel.
			/// @param filename Name of the serial interface (e.g., "COM1").
			/// @param mpiAddress MPI address of communication partner.
			static ComPtr create (const char* filename, const unsigned short int& mpiAddress);

			/// Returns the name of the communication channel.
			/// @return Name of the communication channel.
			virtual const char* getName (void) const;

			/// Tries to opens the communication channel.
			virtual void open (void);

			/// Returns the name of the serial interface (e.g., "COM1").
			/// @return Name of the serial interface.
			const char* getFilename (void) const;

		protected:
			// Constructors / Destructors //

			/// Forbidden constructor.
			/// Use static create() method of a derived class instead.
			MpiOverSerial (const char* filename, const unsigned short int& mpiAddress);
		};

	} // namespace com
} // namespace easycom

#endif // #ifndef _EASYCOM_COM_MPIOVERSERIAL_H_
