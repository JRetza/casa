
// -----------------------------------------------------------------------------

/*

CalStatsDerived.h

Description:
------------
This header file contains definitions for the classes derived from CalStats.

Classes:
--------
CalStatsReal  - This class feeds real data to the CalStats base class.
CalStatsAmp   - This class converts complex data to amplitudes and initializes
                the CalStats base class.
CalStatsPhase - This class converts complex data to phases and initializes the
                CalStats base class.

Inhertited classes:
-------------------
CalStats - This class calculates statistics on CASA caltables.

Modification history:
---------------------
2011 Nov 15 - Nick Elias, NRAO
              Initial version.
2012 Jan 25 - Nick Elias, NRAO
              Logging capability added.  Error checking added.

*/

// -----------------------------------------------------------------------------
// Start of define macro to prevent multiple loading
// -----------------------------------------------------------------------------

#ifndef CAL_STATS_DERIVED_H
#define CAL_STATS_DERIVED_H

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#define _USE_MATH_DEFINES
#include <cmath>

#include <casa/Arrays/MaskedArray.h>

#include <calanalysis/CalAnalysis/CalStats.h>

// -----------------------------------------------------------------------------
// Start of casa namespace definitions
// -----------------------------------------------------------------------------

namespace casa {

// -----------------------------------------------------------------------------
// Start of CalStatsReal class definition
// -----------------------------------------------------------------------------

/*

CalStatsReal

Description:
------------
This class feeds real data to the CalStats base class.

Class public member functions:
------------------------------
CalStatsReal  - This class feeds real data to the CalStats base class.  It is
                primarily used for initial testing.
~CalStatsReal - This destructor deallocates the internal memory of an instance.

Modification history:
---------------------
2011 Dec 11 - Nick Elias, NRAO
              Initial version.  The public member functions are CalStatsReal()
              (generic) and ~CalStatsReal().

*/

// -----------------------------------------------------------------------------

class CalStatsReal : public CalStats {

  public:

    // Generic constructor
    CalStatsReal( const Cube<Double>& oValue, const Cube<Double>& oValueErr,
        const Cube<Bool>& oFlag, const Vector<String>& oFeed,
        const Vector<Double>& oFrequency, const Vector<Double>& oTime,
        const CalStats::AXIS& eAxisIterUser );

    // Destructor
    ~CalStatsReal( void );

};

// -----------------------------------------------------------------------------
// End of CalStatsReal class definition
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Start of CalStatsAmp class definition
// -----------------------------------------------------------------------------

/*

CalStatsAmp

Description:
------------
This class converts complex data to amplitudes and initializes the CalStats base
class.

Class public member functions:
------------------------------
CalStatsAmp  - This generic constructor converts complex data to amplitudes and
               initializes the CalStats base class.  It is primarily used for
               initial testing.
~CalStatsAmp - This destructor deallocates the internal memory of an instance.

Class public static member functions:
-------------------------------------
norm - This member function normalizes the amplitudes and their errors.

Modification history:
---------------------
2011 Nov 15 - Nick Elias, NRAO
              Initial version.  The public member functions are CalStatsAmp()
              (generic) and ~CalStatsAmp().  The static member function is
              norm().
2012 Feb 15 - Nick Elias, NRAO
              Value error input parameter changed from DComplex to Double.

*/

// -----------------------------------------------------------------------------

class CalStatsAmp : public CalStats {

  public:

    // Generic constructor
    CalStatsAmp( const Cube<DComplex>& oValue, const Cube<Double>& oValueErr,
        const Cube<Bool>& oFlag, const Vector<String>& oFeed,
        const Vector<Double>& oFrequency, const Vector<Double>& oTime,
        const CalStats::AXIS& eAxisIterUser, const Bool& bNorm );

    // Destructor
    ~CalStatsAmp( void );

    // Normalize member function
    static void norm( Cube<Double>& oAmp, Cube<Double>& oAmpErr,
        Cube<Bool>& oFlag );

};

// -----------------------------------------------------------------------------
// End of CalStatsAmp class definition
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Start of CalStatsPhase class definition
// -----------------------------------------------------------------------------

/*

CalStatsPhase

Description:
------------
This class converts complex data to phases and initializes the CalStats base
class.

CalStatsPhase public member functions:
--------------------------------------
CalStatsPhase  - This generic constructor converts complex data to amplitudes
                 and initializes the CalStats base class.  It is primarily used 
                 for initial testing.
~CalStatsPhase - This destructor deallocates the internal memory of an instance.

CalStatsPhase static member functions:
--------------------------------------
unwrap - This member function unwraps the phases.

Modification history:
---------------------
2011 Nov 15 - Nick Elias, NRAO
              Initial version.  The public member functions are CalStatsPhase()
              (generic) and ~CalStatsPhase().  The static member function is
              unwrap().
2012 Feb 15 - Nick Elias, NRAO
              Value error input parameter changed from DComplex to Double.

*/

// -----------------------------------------------------------------------------

class CalStatsPhase : public CalStats {

  public:

    // Generic constructor
    CalStatsPhase( const Cube<DComplex>& oValue, const Cube<Double>& oValueErr,
        const Cube<Bool>& oFlag, const Vector<String>& oFeed,
        const Vector<Double>& oFrequency, const Vector<Double>& oTime,
        const CalStats::AXIS& eAxisIterUser, const Bool& bUnwrap );

    // Destructor
    ~CalStatsPhase( void );

    // Unwrap member function
    static void unwrap( Cube<Double>& oPhase, const Vector<Double>& oFrequency,
        const Cube<Bool>& oFlag );

};

// -----------------------------------------------------------------------------
// End of CalStatsPhase class definition
// -----------------------------------------------------------------------------

};

// -----------------------------------------------------------------------------
// End of casa namespace
// -----------------------------------------------------------------------------

#endif

// -----------------------------------------------------------------------------
// End of define macro to prevent multiple loading
// -----------------------------------------------------------------------------
