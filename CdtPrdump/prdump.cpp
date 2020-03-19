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
 *  $Log: $
 *
 */
//
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//  prdump
// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
#include <cstdlib>
#include <cstdio>
#include <string>
#include <iostream>
#include <fstream>
#include <cctype>

using namespace std;

#include "CompDiff.h"

// ----------------------------------------------------------------------------
//  Local Definitions
// ----------------------------------------------------------------------------
#if       defined( _MSC_VER )
#    pragma comment( exestr, "@(#) -*- prdump.cpp -*- $Header: $ " )
#endif // defined( _MSC_VER )

static char const * const pszWhat = "@(#) -*- prdump.cpp -*- $Header: $ ";

// ----------------------------------------------------------------------------
//  Local Prototypes
// ----------------------------------------------------------------------------
void           spitUsage( ostream &out );
char          *FormatOctet( char *iBuf, char *oBuf, int nChar, boolean bWide );
inline char   *FormatOffset( char *Offset, unsigned long nOffset );
inline boolean CheckForSwitch( char *Arg );
inline boolean IsItNarrow( char *Arg, boolean State );
inline void    EnvSnoop( char *Arg, char **envp );

// ----------------------------------------------------------------------------
//  Fnuctions
// ----------------------------------------------------------------------------
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
  //istream_withassign FTarget( cin );
  //ostream_withassign FOut( cout );
    //ostream_withassign FError( cerr );
  istream FTarget;
  ostream FOut;
    ostream FError;

  filebuf buffTarget;
    filebuf buffOut;
    filebuf buffError;

    boolean bWide = true;

    int    argcLocal   = 0;
    int    cbargvLocal = argc * sizeof( char * );
    char **argvLocal   = reinterpret_cast< char ** >( new ( char *[ argc ] ) );
    memset( argvLocal, 0, cbargvLocal );

    if ( argc > 1 )
    {
        for ( register int c = 0; c < argc; c++ )
        {
            if ( CheckForSwitch( argv[ c ] ) )
            {
                bWide = IsItNarrow( argv[ c ], bWide );
                EnvSnoop( argv[ c ], envp );
            }
            else
            {
                argvLocal[ argcLocal++ ] = argv[ c ];
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
        FError  = buffError.open( argvLocal[ 3 ], ios::out );
        bError  = true;

    case 3:
        FOut    = buffOut.open( argvLocal[ 2 ], ios::out );
        bOut    = true;

    case 2:
        FTarget = buffTarget.open( argvLocal[ 1 ], ios::in | ios::binary );
        bTarget = true;

    case 1:
    case 0:
        break;
    }
    if (!bTarget) {
      FTarget.rdbuf(cin);
    }
    if (!bOut) {
      FOut.rdbuf(cout);
    }
    if (!bError) {
      FError.rdbuf(cerr);
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

    memset( InBuff, 0, sizeof( InBuff ) );

    do
    {
        unsigned long nRecords;
        char         *InLine = InBuff;
        unsigned long rec;
        unsigned long nOffset;
        unsigned long nSpace = 0;
        unsigned long nSpaceCtl = cbLineW * 8 / cbLine;

        FTarget.read( InBuff, cbInBuff );
        cbIn = FTarget.gcount();

        nRecords = ( cbIn + cbLine - 1 ) / cbLine;

        for ( rec = 0, nOffset = 0; rec < nRecords; rec++, nOffset += cbLine, InLine += cbLine, nBuffOffset += cbLine )
        {
            char          Offset[ 12 ] = "";
            unsigned long current      = nOffset + cbLine;
            unsigned long nRecord      = current < cbIn ? cbLine : cbLine - ( current - cbIn );

            FOut << "  "
                 << FormatOffset( Offset, nBuffOffset )
                 << "  "
                 << FormatOctet( InLine, Line, nRecord, bWide )
                 << endl
                 ;

            if ( ++nSpace == nSpaceCtl )
            {
                nSpace = 0;
                FOut << '\n';
            }
        }
    } while ( cbIn == cbInBuff );

    delete [] Line;
    delete [] InBuff;

    switch ( argcLocal )
    {
    default:
    case 4:
        buffError.close();

    case 3:
        buffOut.close();

    case 2:
        buffTarget.close();

    case 1:
    case 0:
        break;
    }

    return 0;
}

// ----------------------------------------------------------------------------
// Function name    : spitUsage
// Description      :
// Return type      : void
// Argument         : ostream &out
// Exceptions       :
// ----------------------------------------------------------------------------
void spitUsage( ostream &out )
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
        << endl
        ;

    return;
}

// ----------------------------------------------------------------------------
// Function name    : *FormatOctet
// Description      :
// Return type      : char
// Argument         :  char *iBuf
// Argument         : char *oBuf
// Argument         : int nChar
// Argument         : boolean bWide
// Exceptions       :
// ----------------------------------------------------------------------------
char *FormatOctet( char *iBuf, char *oBuf, int nChar, boolean bWide )
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
    short register spacer1  = 0;
    short register spacer2  = 0;
    unsigned char  Text;
    unsigned char  Work[ 3 ];

    memset( oBuf, ' ', cbLine );

    for ( register int i = 0; i < nChar; i++ )
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
    sprintf( Offset, "%08X", nOffset );

    return Offset;
}

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
          spitUsage( cerr );

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
    char    Switch = *Arg;
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
