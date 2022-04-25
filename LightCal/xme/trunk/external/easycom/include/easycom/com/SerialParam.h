#ifndef _EASYCOM_COM_SERIALPARAM_H_
#define _EASYCOM_COM_SERIALPARAM_H_

/******************************************************************************/
/***   Enumerations                                                         ***/
/******************************************************************************/
// The enumerations defined here are not in the easycom::com namespace, because
// some work derived from easycom has a C language interface and hence would
// not compile if namespaces were used.

/// Baud rate enumeration.
enum baudRate_t
{
	BAUDRATE_110BPS     =  0, ///< 110 bps.
	BAUDRATE_300BPS     =  1, ///< 300 bps.
	BAUDRATE_600BPS     =  2, ///< 600 bps.
	BAUDRATE_1200BPS    =  3, ///< 1,200 bps.
	BAUDRATE_2400BPS    =  4, ///< 2,400 bps.
	BAUDRATE_4800BPS    =  5, ///< 4,800 bps.
	BAUDRATE_9600BPS    =  6, ///< 9,600 bps.
#ifdef WIN32
	BAUDRATE_14400BPS   =  7, ///< 14,400 bps. Only available on Windows.
#endif // #ifdef WIN32
	BAUDRATE_19200BPS   =  8, ///< 19,200 bps.
	BAUDRATE_38400BPS   =  9, ///< 38,400 bps.
	BAUDRATE_57600BPS   = 10, ///< 57,600 bps.
	BAUDRATE_115200BPS  = 11, ///< 115,200 bps.
#ifdef WIN32
	BAUDRATE_128000BPS  = 12, ///< 128,000 bps. Only available on Windows.
	BAUDRATE_256000BPS  = 13  ///< 256,000 bps. Only available on Windows.
#else // #ifdef WIN32
	BAUDRATE_230400BPS  = 14, ///< 230,400 bps. Not available on Windows.
	BAUDRATE_460800BPS  = 15, ///< 460,800 bps. Not available on Windows.
	BAUDRATE_500000BPS  = 16, ///< 500,000 bps. Not available on Windows.
	BAUDRATE_576000BPS  = 17, ///< 576,000 bps. Not available on Windows.
	BAUDRATE_921600BPS  = 18, ///< 921,600 bps. Not available on Windows.
	BAUDRATE_1000000BPS = 19, ///< 1,000,000 bps. Not available on Windows.
	BAUDRATE_1152000BPS = 20, ///< 1,152,000 bps. Not available on Windows.
	BAUDRATE_1500000BPS = 21, ///< 1,500,000 bps. Not available on Windows.
	BAUDRATE_2000000BPS = 22, ///< 2,000,000 bps. Not available on Windows.
	BAUDRATE_2500000BPS = 23, ///< 2,500,000 bps. Not available on Windows.
	BAUDRATE_3000000BPS = 24, ///< 3,000,000 bps. Not available on Windows.
	BAUDRATE_3500000BPS = 25, ///< 3,500,000 bps. Not available on Windows.
	BAUDRATE_4000000BPS = 26  ///< 4,000,000 bps. Not available on Windows.
#endif // #ifdef WIN32
};

/// Data bits enumeration.
enum dataBits_t
{
	DATABITS_5BITS = 0, ///< 5 data bits.
	DATABITS_6BITS = 1, ///< 6 data bits.
	DATABITS_7BITS = 2, ///< 7 data bits.
	DATABITS_8BITS = 3  ///< 8 data bits.
};

/// Flow control enumeration.
enum flowControl_t
{
	FLOWCONTROL_OFF     = 0, ///< No flow control.
	FLOWCONTROL_RTSCTS  = 1, ///< Hardware flow control (RTS/CTS).
	FLOWCONTROL_XONXOFF = 2  ///< Software flow control (XON/XOFF).
};

/// Parity mode enumeration.
enum parityMode_t
{
	PARITY_EVENPARITY = 0, ///< Even parity.
	PARITY_NOPARITY   = 1, ///< No parity.
	PARITY_ODDPARITY  = 2  ///< Odd parity.
};

/// Stop bits enumeration.
enum stopBits_t
{
	STOPBITS_1BIT  = 0, /// 1 stop bit.
	STOPBITS_2BITS = 1  /// 2 stop bits.
};

#endif // #ifndef _EASYCOM_COM_SERIALPARAM_H_
