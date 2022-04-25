#ifndef _EASYUTIL_EXCEPTION_EXCEPTION_H_
#define _EASYUTIL_EXCEPTION_EXCEPTION_H_

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include <exception>
#include <iostream>
#include <string>

namespace util {
	
	/******************************************************************************/
	/***   Declaration of class Exception                                       ***/
	/******************************************************************************/
	/// The Exception class is the base class of our exception hierarchy.
	class Exception : public std::exception
	{
	public:
		// Constructors / destructors //
		
		/// Constructs an exception with an empty text message.
		Exception (void);
		
		/// Constructs an exception with the given text message.
		/// @param message Exception message text.
		Exception (std::string message);
		
		/// Virtual destructor.
		virtual ~Exception (void) throw ();
		
		// Methods //
		
		/// Returns the text message associated with the exception.
		/// @return Text message associated with the exception.
		virtual const char* what (void) const throw ();
		
	protected:
		// Methods //
		
		/// Sets the text message associated with the exception.
		/// @param message New text message.
		void setMessage (const std::string& message);
		
	private:
		/// String stream that is used to format exception message
		std::string _message;
	};
	
} // namespace util

/******************************************************************************/
/***   Declaration of global functions                                      ***/
/******************************************************************************/
/// Writes an exception object to a stream.
/// @param o Output stream.
/// @param e Exception object to be printed.
std::ostream& operator<< (std::ostream& o, const std::exception& e);

#endif // #ifndef _EASYUTIL_EXCEPTION_EXCEPTION_H_
