#ifndef _EASYCOM_COM_TCPSOCKET_H_
#define _EASYCOM_COM_TCPSOCKET_H_

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "easycom/com/TcpSocketIntf.h"
#include "easycom/com/Socket.h"

namespace easycom
{
	namespace com
	{
		/******************************************************************************/
		/***   Declaration of class TcpSocket                                       ***/
		/******************************************************************************/
		class TcpSocket : public Socket
		                , public TcpSocketIntf
		{
		public:
			// Constructors / Destructors //

			/// Virtual destructor.
			virtual ~TcpSocket();

			// Methods //

			/// Creates a new TCP communication channel.
			/// @param host Host name the socket is bound to.
			/// @param port Port number the socket is bound to.
			static ComPtr create (const char* host, const unsigned short int& port);

			/// Returns the name of the communication channel.
			/// @return Name of the communication channel.
			virtual const char* getName (void) const;

			/// Tries to opens the communication channel.
			virtual void open (void);

		private:
			// Constructors / Destructors //

			/// Forbidden constructor.
			/// Use static create() method instead.
			/// @param host Host name the socket is bound to.
			/// @param port Port number the socket is bound to.
			TcpSocket (const char* host, const unsigned short int& port);
		};

	} // namespace com
} // namespace easycom

#endif // #ifndef _EASYCOM_COM_TCPSOCKET_H_
