//# SIIterBot.h: This file contains the interface definition SIIterBot class
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

#ifndef SYNTHESIS_SIITERBOT
#define SYNTHESIS_SIITERBOT

// .casarc interface
#include <casa/System/AipsrcValue.h>

// System utilities (for profiling macros)
#include <casa/OS/HostInfo.h>
#include <sys/time.h>


// Boost Libraries for mutex and noncopyable semantics
#include <boost/thread/recursive_mutex.hpp>
#include <boost/utility.hpp>

// Include files for the DBus Service
#include <casadbus/interfaces/SynthImager.adaptor.h>
#include <casadbus/utilities/DBusBase.h>

#include<synthesis/MeasurementEquations/SISubIterBot.h>

/* Future Decl */
class casa::Record;

namespace casa { //# NAMESPACE CASA - BEGIN

  class SIIterBot : public SISubIterBot,
                    public edu::nrao::casa::SynthImager_adaptor,
                    public DBusService
  {
  public:
    SIIterBot(const std::string &serviceName);
    ~SIIterBot();
    
    /* Wait for an Interactive Clean Cycle */
    void waitForInteractiveInput(); 

    virtual bool majorCycleRequired(Float currentPeakResidual);

    void mergeSubIterBot(SISubIterBot& subIterBot);

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

    void changeInteractiveMode(const bool& interactiveEnabled);
    void changePauseFlag(const bool& pauseEnabled);
    void changeStopFlag(const bool& stopEnabled);

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

    Record getDetailsRecord(); 
    Record getSubIterBotRecord();

    /* ------ END Functions for runtime parameter modification -------*/

    /* Call these functions to keep track of cycles */

    /* Note:  Incrementing the Major cycle will reset the cycleIterDone */
    void incrementMajorCycleCount();

    Int getMajorCycleCount();

    /* This will calculate and set a new cycle threshold based
       on the Peak Residual and the current values of the PSF values.*/
    void updateCycleThreshold(Float PeakResidual);
    
    /* Values for the control of the cycle threshold */
    void setMaxPsfSidelobe( Float maxpsfsidelobe );
    Float getMaxPsfSidelobe();

    void setMaxPsfFraction(Float maxpsffraction );
    Float getMaxPsfFraction();

    void setMinPsfFraction(Float minpsffraction );
    Float getMinPsfFraction();

    void addSummaryMajor();

    /* Publish the current details from the Iterbot to all clients */
    void pushDetails();

    /* Publish the current summary from the Iterbot to all clients */
    void pushSummary();

    /* These are the fuctions we provide through the dbus interface */
    bool incrementController();
    bool decrementController();
    int  getNumberOfControllers();


    std::map<std::string,DBus::Variant> getDetails();
    DBus::Variant getSummary();
    
    void interactionComplete();
    void controlUpdate(const std::map<std::string, DBus::Variant>& parameters);
    //// START /// Functions for runtime parameter modification

    /* Methods to get the state of the iterbot as a Record*/

    /* This record has all of the fields associated with the detail record 
       but adds
       - summaryminor
       - summarymajor
    */
    Record getSummaryRecord();

  protected:

    Float itsMinPsfFraction;
    Float itsMaxPsfFraction;
    Float itsMaxPsfSidelobe;

    /* The number of Controllers Currently Connected */
    int    itsControllerCount;
    
    /*
      A condition variable used when we're waiting for interaction to 
      complete
    */
     bool                      interactionPending;
     bool                      updateNeeded;
     boost::condition_variable interactionCond; 
     boost::mutex              interactionMutex; 

    Int   itsMajorDone;

    /* Summary variables */
    Array<Int>    itsSummaryMajor;
  };
    
} //# NAMESPACE CASA - END

#endif /* FLAGDATAHANDLER_H_ */
