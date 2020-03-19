// ----------------------------------------------------------------------------
//  Compiler differences
// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------

#ifndef COMPDIFF_HEADER
#define COMPDIFF_HEADER

#if defined(__WATCOM_CPLUSPLUS__)
// --- Watcom C++ -------------------------------------------------------------

#include <iostream>
typedef bool boolean;
typedef istream istream_withassign;
typedef ostream ostream_withassign;

#include <exception>
typedef string __exString;

#elif defined(_MSC_VER)
// --- Microsoft Visual C++ ---------------------------------------------------

#   if _MSC_VER > 1000
// ------ version 4.1 and up --------------------------------------------------
typedef bool boolean;
#include <exception>
#   else
// ------ version 4.0 ---------------------------------------------------------
enum boolean
{
    true = ( 1 == 1 ),
    false = !true
};
typedef boolean bool;

#include <stdexcpt.h>
typedef exception runtime_error;
#   endif

#else
// ---Others ------------------------------------------------------------------
typedef bool boolean;
#include <exception>

typedef std::string __exString;

#endif  // Compiler differences

#endif // COMPDIFF_HEADER


// ----------------------------------------------------------------------------
//  End of header
// ----------------------------------------------------------------------------

