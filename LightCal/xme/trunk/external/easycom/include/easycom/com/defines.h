#ifndef _COM_DEFINES_H_
#define _COM_DEFINES_H_

/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/
/*
 * Defines that help to correctly specify dllexport and dllimport on
 * Windows platforms.
 */
#ifdef WIN32
#  ifdef com_EXPORTS
#    define COM_EXPORT __declspec(dllexport)
#  else
#    define COM_EXPORT __declspec(dllimport)
#  endif //# ifdef com_EXPORTS
#else // #ifdef WIN32
#  define COM_EXPORT
#endif // #ifdef WIN32

#endif // #ifndef _COM_DEFINES_H_
