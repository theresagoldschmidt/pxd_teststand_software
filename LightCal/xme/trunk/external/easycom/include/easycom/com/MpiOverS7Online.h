#ifndef _EASYCOM_COM_MPIOVERS7ONLINE_H_
#define _EASYCOM_COM_MPIOVERS7ONLINE_H_

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "easycom/com/Mpi.h"
#include "easycom/com/MpiOverS7OnlineIntf.h"

namespace easycom
{
	namespace com
	{
		/******************************************************************************/
		/***   Forward declarations                                                 ***/
		/******************************************************************************/
		class MpiOverS7OnlineImpl;

		/******************************************************************************/
		/***   Declaration of class MpiOverS7Online                                 ***/
		/******************************************************************************/
		class MpiOverS7Online : public Mpi
		                      , public MpiOverS7OnlineIntf
		{
		public:
			// Constructors / Destructors //

			/// Virtual destructor.
			virtual ~MpiOverS7Online();

			// Methods //

			/// Creates a new MPI communication channel.
			/// @param mpiAddress MPI address of communication partner.
			static ComPtr create (const unsigned short int& mpiAddress);

			/// Tries to opens the communication channel.
			virtual void open (void);

		protected:
			// Constructors / Destructors //

			/// Forbidden constructor.
			/// Use static create() method of a derived class instead.
			MpiOverS7Online (const unsigned short int& mpiAddress);
		};

	} // namespace com
} // namespace easycom

#endif // #ifndef _EASYCOM_COM_MPIOVERS7ONLINE_H_
