//# SynthesisParSync.h: Gather and Scatter operations for parallel major cycles.
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2002,2003
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
//# License for more details.
//#
//# You should have received a copy of the GNU Library General Public License
//# along with this library; if not, write to the Free Software Foundation,
//# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#
//# $Id$

#ifndef SYNTHESIS_SYNTHESISPARSYNC_H
#define SYNTHESIS_SYNTHESISPARSYNC_H

#include <casa/aips.h>
#include <casa/OS/Timer.h>
#include <casa/Containers/Record.h>
#include <ms/MeasurementSets/MeasurementSet.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Quanta/Quantum.h>
#include <measures/Measures/MDirection.h>

#include<synthesis/MeasurementEquations/SDAlgorithmBase.h>
#include<synthesis/MeasurementEquations/SDAlgorithmHogbomClean.h>
#include<synthesis/MeasurementEquations/SDMaskHandler.h>
#include <synthesis/MeasurementEquations/SIMinorCycleController.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// Forward declarations
class MeasurementSet;
template<class T> class ImageInterface;

// <summary> Class that contains functions needed for imager </summary>

class SynthesisParSync 
{
 public:
  // Default constructor

  SynthesisParSync();
  ~SynthesisParSync();

  // Copy constructor and assignment operator

  // make all pure-inputs const
  void setupParSync(Record syncpars);

  // Gather all part images to the 'full' one
  void gatherResidual();

  // Copy out model to all pieces. Currently a No-Op.
  void scatterModel();

protected:

 // Normalize. This can later change to be more general, i.e. used for PB-correction too...
  void divideResidualByWeight();
  void divideModelByWeight();
 // Check if images exist on disk and are all the same shape
  Bool setupImagesOnDisk();
  Bool doImagesExist( String imagename );

  /////////////// Member Objects

  CountedPtr<SIImageStore> itsImages;
  Vector<CountedPtr<SIImageStore> > itsPartImages;

  IPosition itsImageShape;
  
  String itsImageName;
  Vector<String> itsPartImageNames;
  String itsStartingModelName;

};


} //# NAMESPACE CASA - END

#endif
