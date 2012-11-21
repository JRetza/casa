//# FlagReport.h: This file contains the interface definition of the FlagReport class.
//#
//#  CASA - Common Astronomy Software Applications (http://casa.nrao.edu/)
//#  Copyright (C) Associated Universities, Inc. Washington DC, USA 2011, All rights reserved.
//#  Copyright (C) European Southern Observatory, 2011, All rights reserved.
//#
//#  This library is free software; you can redistribute it and/or
//#  modify it under the terms of the GNU Lesser General Public
//#  License as published by the Free software Foundation; either
//#  version 2.1 of the License, or (at your option) any later version.
//#
//#  This library is distributed in the hope that it will be useful,
//#  but WITHOUT ANY WARRANTY, without even the implied warranty of
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//#  Lesser General Public License for more details.
//#
//#  You should have received a copy of the GNU Lesser General Public
//#  License along with this library; if not, write to the Free Software
//#  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
//#  MA 02111-1307  USA
//# $Id: $

#ifndef SYNTHESIS_SILOOPCONTROLLER
#define SYNTHESIS_SILOOPCONTROLLER

// .casarc interface
#include <casa/System/AipsrcValue.h>

// Records interface
//#include <casa/Containers/Record.h>

// System utilities (for profiling macros)
#include <casa/OS/HostInfo.h>
#include <sys/time.h>


// Boost Libraries for mutex and noncopyable semantics
#include <boost/thread/recursive_mutex.hpp>
#include <boost/utility.hpp>

/* Future Decl */
class casa::Record;

namespace casa { //# NAMESPACE CASA - BEGIN
  
  class SIIterBot : boost::noncopyable 
  {
  public:
    SIIterBot();
    ~SIIterBot();
    
    /* Method to call to determine if we should wait for interactive
       updates from the user true if:
         - exceeded number of interactiveNiter 
         - exceeded interactiveThreshold
         - pause has been pressed
    */
    //bool interactiveRequired(Float currentPeakResidual);
    //void waitForInteractive();

    /* Method to call to determine if a major cycle is required, true if
         - We are done (see below)
         - exceeded number maxcycleniter
         - exceeded cycle threshold
    */
    bool majorCycleRequired(Float currentPeakResidual);

    /* Method to call to determine if we should stop the clean process 
       true if
         - stop flag has been set
         - exceeded Niter
         - exceeded threshold
    */
    bool cleanComplete(Float currentPeakResidual);

    /* ------ Begin functions for runtime parameter modification ------ */
    /* These are the control variables.  We can set them either through these
       method or through the Record interface */
    void changeNiter( Int niter );
    void changeCycleNiter( Int cycleniter );
    void changeInteractiveNiter(Int interactiveniter );

    void changeThreshold( Float threshold );
    void changeCycleThreshold( Float cyclethreshold );
    void changeInteractiveThreshold( Float cyclethreshold );

    void changeLoopGain(  Float loopgain );
    void changeCycleFactor( Float cyclefactor);

    void changeInteractiveMode(Bool interactiveEnabled);
    void changePauseFlag(Bool pauseEnabled);
    void changeStopFlag(Bool stopEnabled);

    /* As a convience the controls can also be updated from a Record.  
       The following fields are supported:
       - niter
       - cycleniter
       - interactiveniter
       - threshold
       - cyclethreshold
       - interactivethreshold
       - loopgain
       - cyclefactor
    */
    void setControlsFromRecord(Record &recordIn);

    /* Getter Methods for the control variables */
    Int getNiter();
    Int getCycleNiter();
    int getInteractiveNiter();

    Float getThreshold();
    Float getCycleThreshold();
    Float getInteractiveThreshold();
   
    Float getLoopGain();
    Float getCycleFactor();

    Bool getInteractiveMode();
    Bool getPauseFlag();
    Bool getStopFlag();


    /* ------ END Functions for runtime parameter modification -------*/

    /* Call these functions to keep track of cycles */

    /* Note:  Incrementing the Major cycle will reset the cycleIterDone */
    void incrementMajorCycleCount();

    void incrementMinorCycleCount();

    Int getMajorCycleCount();

    Int getRemainingNiter();
    Int getCompletedNiter();

    /* This will calculate and set a new cycle threshold based
       on the Peak Residual and the current values of the PSF values.*/
    void updateCycleThreshold(Float PeakResidual);

    /* This method resets the iteration counter for the cycle */
    void resetCycleIter();
    
    /* Values for the control of the cycle threshold */
    void setMaxPsfSidelobe( Float maxpsfsidelobe );
    Float getMaxPsfSidelobe();

    void setMaxPsfFraction(Float maxpsffraction );
    Float getMaxPsfFraction();

    void setMinPsfFraction(Float minpsffraction );
    Float getMinPsfFraction();

    /* Flag to note that the model has been updated */
    Bool getUpdatedModelFlag();
    void setUpdatedModelFlag(Bool updatedmodel);

    void addSummaryMinor(Int mapperid, Float model, Float peakresidual);
    void addSummaryMajor();

    //// START /// Functions for runtime parameter modification

    /* Methods to get the state of the iterbot as a Record*/
    /* This returns the following fields:
       * Controls
       - niter
       - cycleniter
       - interactiveniter
       - threshold
       - cyclethreshold
       - interactivethreshold
       - loopgain
       - cyclefactor
       - maxpsfsidelobe
       - maxpsffraction
       - minpsffraction
       
       * Status 
       - iterdone
       - cycleiterdone;
       - interactiveiterdone;
       - nmajordone;
    */
    Record getDetailsRecord(); 

    /* This record has all of the fields associated with the detail record 
       but adds
       - summaryminor
       - summarymajor
    */
    Record getSummaryRecord();

  protected:


  private:

    /* A recursive mutex which provides access control to the
       underlying state variables
    */
    boost::recursive_mutex recordMutex;

    /* These are the internal variables that we need for the control 
       note: ALL access to these should be through getter/setter mechanism
       so they can be protected by the mutex 
    */
    
    /* Control Variables */
    Int    itsNiter;
    Int    itsCycleNiter;
    Int    itsInteractiveNiter;

    Float itsThreshold;
    Float itsCycleThreshold;
    Float itsInteractiveThreshold;

    Float itsCycleFactor;
    Float itsLoopGain;

    Float itsMinPsfFraction;
    Float itsMaxPsfFraction;
    Float itsMaxPsfSidelobe;

    
    Bool  itsStopFlag;
    Bool  itsPauseFlag;
    Bool  itsInteractiveMode;

    /* Status Reporting Variables */
    Int   itsIterDone;
    Int   itsCycleIterDone;
    Int   itsInteractiveIterDone;
    Int   itsMajorDone;
    
    Bool   itsUpdatedModelFlag;

    /* Summary variables */
    Array<Double> itsSummaryMinor;
    Array<Int>    itsSummaryMajor;
  };
    
} //# NAMESPACE CASA - END

#endif /* FLAGDATAHANDLER_H_ */
