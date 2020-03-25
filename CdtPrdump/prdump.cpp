//============================================================================
// Name        : CdtPrdump.cpp
// Author      : "Alan Sampson" <alansamps@gmail.com>
// Version     :
// Copyright   : (C) Copyright Alan Sampson, 2010.  All rights reserved.
// Description : Format and print binary files in dump format.
//============================================================================
//
// ----------------------------------------------------------------------------
//  Maintenance History
// ----------------------------------------------------------------------------
//  Date        | User  | Reference         | Description
// -------------+-------+-------------------+----------------------------------
//  1997/10/30  | asamp |                   | Created
//              |       |                   |
// -------------+-------+-------------------+----------------------------------
//
/*
 *  MARK: - Change Log
 *  $Log: $
 *    2020-03-22 - 001 - Improve method used to redirect output streams to files
 *
 *  MARK: - References.
 *  @see: https://stackoverflow.com/questions/10150468/how-to-redirect-cin-and-cout-to-files
 *
 *
 */
//
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//  prdump
// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <cctype>

#include "CompDiff.h"

//  MARK: - Definitions.
// ----------------------------------------------------------------------------
//  Local Definitions
// ----------------------------------------------------------------------------
#if defined( _MSC_VER )
# pragma comment( exestr, "@(#) -*- prdump.cpp -*- $Header: $ " )
#endif // defined( _MSC_VER )

static char const * const __unused pszWhat = "@(#) -*- prdump.cpp -*- $Header: $ ";

// ----------------------------------------------------------------------------
//  Local Prototypes
// ----------------------------------------------------------------------------
void           spitUsage( std::ostream &out );
char          *FormatOctet( char *iBuf, char *oBuf, size_t nChar, boolean bWide );
inline char   *FormatOffset( char *Offset, unsigned long nOffset );
inline boolean CheckForSwitch( char *Arg );
inline boolean IsItNarrow( char *Arg, boolean State );
inline void    EnvSnoop( char *Arg, char **envp );

//  MARK: - Implementation.
// ----------------------------------------------------------------------------
//  Fnuctions
// ----------------------------------------------------------------------------
//  MARK: main()
// ----------------------------------------------------------------------------
// Function name    : main
// Description      :
// Return type      : int
// Argument         :  int argc
// Argument         : char **argv
// Argument         : char **envp
// Exceptions       :
// ----------------------------------------------------------------------------
int main( int argc, char **argv, char **envp )
{
  std::ifstream FTarget;
  std::ofstream FOut;
  std::ofstream FError;

  std::streambuf * cinbuf  = NULL;
  std::streambuf * coutbuf = NULL;
  std::streambuf * cerrbuf = NULL;

  boolean bWide = true;

  int    argcLocal   = 0;
  int    cbargvLocal = argc * sizeof( char * );
  char **argvLocal   = new char *[ argc ];
  memset( argvLocal, 0, cbargvLocal );

  if ( argc > 1 )
  {
    for ( size_t c_ = 0; c_ < argc; c_++)
    {
      if ( CheckForSwitch( argv[ c_ ] ) )
      {
        bWide = IsItNarrow( argv[ c_ ], bWide );
        EnvSnoop( argv[ c_ ], envp );
      }
      else
      {
        argvLocal[ argcLocal++ ] = argv[ c_ ];
      }
    }
  }

  bool bError(false);
  bool bOut(false);
  bool bTarget(false);

  switch ( argcLocal )
  {
    default:
    case 4:
      FError  = std::ofstream( argvLocal[ 3 ]);
      cerrbuf = std::cerr.rdbuf();      //  save system cerr buffer
      std::cerr.rdbuf(FError.rdbuf());  //  redirect cerr to file
      bError  = true;

    case 3:
      FOut    = std::ofstream( argvLocal[ 2 ]);
      coutbuf = std::cout.rdbuf();      //  save system cout buffer
      std::cout.rdbuf(FOut.rdbuf());    //  redirect cout to file
      bOut    = true;

    case 2:
      FTarget = std::ifstream( argvLocal[ 1 ]);
      cinbuf  = std::cin.rdbuf();       //  save system cin buffer
      std::cin.rdbuf(FTarget.rdbuf());  //  redirect cin from file
      bTarget = true;

    case 1:
    case 0:
      break;
  }

  delete [] argvLocal;

  unsigned long const cbInBuff  = 4096;
  unsigned long const cbLineLen = 120;
  unsigned long const cbLineW   = 32;
  unsigned long const cbLineN   = 16;
  unsigned long       cbIn;
  unsigned long       nBuffOffset = 0;
  unsigned long       cbLine = bWide ? cbLineW : cbLineN;

  char *Line   = new char[ cbLineLen ];
  char *InBuff = new char[ cbInBuff ];

  memset( InBuff, 0, cbInBuff );

  do
  {
    unsigned long nRecords;
    char         *InLine = InBuff;
    unsigned long rec;
    unsigned long nOffset;
    unsigned long nSpace = 0;
    unsigned long nSpaceCtl = cbLineW * 8 / cbLine;

    std::cin.read( InBuff, cbInBuff );
    cbIn = std::cin.gcount();

    nRecords = ( cbIn + cbLine - 1 ) / cbLine;

    for ( rec = 0, nOffset = 0;
          rec < nRecords;
          rec++, nOffset += cbLine, InLine += cbLine, nBuffOffset += cbLine )
    {
      char          Offset[ 12 ] = "";
      unsigned long current      = nOffset + cbLine;
      unsigned long nRecord      = current < cbIn ? cbLine : cbLine - ( current - cbIn );

      std::cout << "  "
                << FormatOffset( Offset, nBuffOffset )
                << "  "
                << FormatOctet( InLine, Line, nRecord, bWide )
                << std::endl
                ;

      if ( ++nSpace == nSpaceCtl )
      {
        nSpace = 0;
        std::cout << '\n';
      }
    }
  } while ( cbIn == cbInBuff );

  delete [] Line;
  delete [] InBuff;

  switch ( argcLocal )
  {
    default:
    case 4:
      std::cerr.rdbuf(cerrbuf);   //  reset cerr

    case 3:
      std::cout.rdbuf(coutbuf);   //  reset cout

    case 2:
      std::cin.rdbuf(cinbuf);     //  reset cin

    case 1:
    case 0:
      break;
  }

  return 0;
}

//  MARK: spitUsage()
// ----------------------------------------------------------------------------
// Function name    : spitUsage
// Description      :
// Return type      : void
// Argument         : ostream &out
// Exceptions       :
// ----------------------------------------------------------------------------
void spitUsage( std::ostream &out )
{
  out << "Usage:  prdump <switches> inFile outFile errorLog"
      << '\n'
      << "\twhere:  inFile is the file to read.  (default stdin.)"
      << '\n'
      << "\t        outFile is where the results are written.  (default stdout.)"
      << '\n'
      << "\t        errorLog is where the error messages are written.  (default stderr.)"
      << '\n'
      << '\n'
      << "\tswitches:"
      << '\n'
      << "\t\t-h\tHelp.  This message."
      << '\n'
      << "\t\t-?\tDitto."
      << '\n'
      << "\t\t-n\tPrint the report in narrow format."
      << '\n'
      << "\t\t-w\tPrint the report in wide format.  (default.)"
      << std::endl
      ;

  return;
}

//  MARK: FormatOctet()
// ----------------------------------------------------------------------------
// Function name    : *FormatOctet
// Description      :
// Return type      : char
// Argument         : char *iBuf
// Argument         : char *oBuf
// Argument         : int nChar
// Argument         : boolean bWide
// Exceptions       :
// ----------------------------------------------------------------------------
char *FormatOctet( char *iBuf, char *oBuf, size_t nChar, boolean bWide )
{
  // Wide...
  // 0....+....1....+....2....+....3....+....4....+....5....+....6....+....7....+....8....+....9....+....0....+....1
  // 00010203 04050607 08090a0b 0c0d0e0f  10111213 14151617 18191a1b 1c1d1e1f    0123456789abcdef 0123456789abcdef

  // Narrow...
  // 0....+....1....+....2....+....3....+....4....+....5....+....6....+....7....+....8....+....9....+....0....+....1
  // 00010203 04050607 08090a0b 0c0d0e0f  0123456789abcdef

  unsigned int const cbLine = 110;

  unsigned int   nPOffset = bWide ? 76 : 37;
  unsigned char *iThis    = reinterpret_cast< unsigned char * >( iBuf );
  unsigned char *oThisX   = reinterpret_cast< unsigned char * >( oBuf );
  unsigned char *oThisP   = reinterpret_cast< unsigned char * >( oBuf + nPOffset );
  short spacer1           = 0;
  short spacer2           = 0;
  unsigned char Text;
  unsigned char Work[ 3 ];

  memset( oBuf, ' ', cbLine );

  for ( size_t i_ = 0; i_ < nChar; i_++ )
  {
    Text = *iThis++;

    sprintf( reinterpret_cast< char * >( Work ), "%02x", Text );
    memcpy( oThisX, Work, 2 );

    if ( isprint( Text ) || isgraph( Text ) )
    {
      *oThisP = Text;
    }
    else
    {
      *oThisP = '.';
    }

    oThisX += 2;
    oThisP++;

    if ( ++spacer1 == 4 )
    {
      spacer2++;
      spacer1 = 0;
      oThisX++;
    }

    if ( spacer2 == 4 )
    {
      spacer2 = 0;
      oThisX++;
    }
  }

  *oThisP = '\0';

  return oBuf;
}

//  MARK: FormatOffset()
// ----------------------------------------------------------------------------
// Function name    : *FormatOffset
// Description      :
// Return type      : char
// Argument         :  char *Offset
// Argument         : unsigned long nOffset
// Exceptions       :
// ----------------------------------------------------------------------------
inline char *FormatOffset( char *Offset, unsigned long nOffset )
{
  sprintf( Offset, "%08lX", nOffset );

  return Offset;
}

//  MARK: CheckForSwitch()
// ----------------------------------------------------------------------------
// Function name    : CheckForSwitch
// Description      :
// Return type      : boolean
// Argument         : char *Arg
// Exceptions       :
// ----------------------------------------------------------------------------
inline boolean CheckForSwitch( char *Arg )
{
  boolean Result;
  char    Switch = *Arg;
  char   *Val    = Arg;

  switch( Switch )
  {
    case '-':
    case '/':
      switch ( toupper( *( ++Val ) ) )
      {
        case '?':
        case 'H':
          spitUsage( std::cerr );

        case 'W':
        case 'N':
          Result = true;
          break;

        default:
          Result = false;
          break;
      }
      break;

  default:
    Result = false;
    break;
  }

  return Result;
}

//  MARK: IsItNarrow()
// ----------------------------------------------------------------------------
// Function name    : IsItNarrow
// Description      :
// Return type      : boolean
// Argument         :  char *Arg
// Argument         : boolean State
// Exceptions       :
// ----------------------------------------------------------------------------
inline boolean IsItNarrow( char *Arg, boolean State )
{
  boolean Result;
  char    __unused Switch = *Arg;
  char   *Val    = ++Arg;

  // State is set true by default.  A State of "false" implies a
  // -n switch has already been processed.
  if ( State )
  {
      switch ( toupper( *Val ) )
      {
        case 'N':
          Result = false;
          break;

        case 'W':
        default:
          Result = true;
          break;
      }
  }
  else
  {
    Result = State;
  }

  return Result;
}

//  MARK: EnvSnoop()
// ----------------------------------------------------------------------------
// Function name    : EnvSnoop
// Description      :
// Return type      : void
// Argument         :  char *Arg
// Argument         : char **envp
// Exceptions       :
// ----------------------------------------------------------------------------
inline void EnvSnoop( char *Arg, char **envp )
{
  char *Val    = ++Arg;
  char *EnvVar;

  switch ( toupper( *Val ) )
  {
    case 'E':
      while ( *envp != NULL )
      {
        EnvVar = *envp++;
      }
      break;

    default:
      break;
  }

  return;
}

// ----------------------------------------------------------------------------
//  End of module
// ----------------------------------------------------------------------------
