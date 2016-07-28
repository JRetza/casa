//# SIMapperCollection.h: Imager functionality sits here; 
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

#ifndef SYNTHESIS_SIMAPPERCOLLECTION_H
#define SYNTHESIS_SIMAPPERCOLLECTION_H

#include <casa/aips.h>
#include <casa/OS/Timer.h>
#include <casa/Containers/Record.h>
#include <ms/MeasurementSets/MeasurementSet.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Quanta/Quantum.h>
#include <measures/Measures/MDirection.h>

#include <synthesis/ImagerObjects/SIMapper.h>
#include <synthesis/ImagerObjects/SIMapperImageMosaic.h>

namespace casa { //# NAMESPACE CASA - BEGIN

class SIMapperCollection 
{
 public:
  // Default constructor

  SIMapperCollection();
  ~SIMapperCollection();

  void addMapper(CountedPtr <SIMapper> map);
  Int nMappers();
  Vector<String> getImageNames();

  /*  // No need for now.
      // Write this function only if we need to support multiple facets on outlier fields too.
      // In that case, hold a list of original ImageStores and a map to the list of Mappers
      //  to keep track of which original image points to which subset of the Mapper list.
      //  Also add code into "finalizeGrid" to copy only one PSF to the original. 
  void addMapper( Int mapperId, 
		  String mapperType,
		  uInt nTaylorTerms,
		  CountedPtr<FTMachine> ftm,
		  CountedPtr<FTMachine> iftm,
		  String imageName,
		  CoordinateSystem& cSys,
		  IPosition imShape, 
		  const Bool overwrite );
  */
  void initializeGrid(vi::VisBuffer2& vb, Bool dopsf=False,const Int mapperid=-1);
  void grid(vi::VisBuffer2& vb, Bool dopsf=False, refim::FTMachine::Type col=refim::FTMachine::CORRECTED,
	    const Int mapperid=-1);
  void finalizeGrid(vi::VisBuffer2& vb, Bool dopsf=False,const Int mapperid=-1);
  void initializeDegrid(vi::VisBuffer2& vb,const Int mapperid=-1);
  void degrid(vi::VisBuffer2& vb, Bool saveVirtualMod=False,const Int mapperid=-1);
  void saveVirtualModel(vi::VisBuffer2& vb);
  void finalizeDegrid(vi::VisBuffer2& vb,const Int mapperid=-1);
  void addPB(vi::VisBuffer2& vb, PBMath& pbMath);

  //////////// OLD VI/VB : Version that use old vi/vb can be removed the vi2/vb2 works
  void initializeGrid(VisBuffer& vb, Bool dopsf=False,const Int mapperid=-1);
  void grid(VisBuffer& vb, Bool dopsf=False, FTMachine::Type col=FTMachine::CORRECTED,
	    const Int mapperid=-1);
  void finalizeGrid(VisBuffer& vb, Bool dopsf=False,const Int mapperid=-1);
  void initializeDegrid(VisBuffer& vb,const Int mapperid=-1);
  void degrid(VisBuffer& vb, Bool saveVirtualMod=False,const Int mapperid=-1);
  void saveVirtualModel(VisBuffer& vb);
  void finalizeDegrid(VisBuffer& vb,const Int mapperid=-1);
  void addPB(VisBuffer& vb, PBMath& pbMath);
  ////////////////////////////////////////////////////
  Record getFTMRecord(Int mapperid);

  CountedPtr<SIImageStore> imageStore(const Int id=0);
  Bool releaseImageLocks();

  void checkOverlappingModels(String action); // action='blank' or 'restore'

  Bool anyNonZeroModels();

  const CountedPtr<FTMachine> getFTM(Int which, const Bool ift=True) 
  {
    AlwaysAssert(which>=0 && which<(Int)itsMappers.nelements(),AipsError);
    return itsMappers[which]->getFTM(ift);
  };
  const CountedPtr<refim::FTMachine>& getFTM2(Int which, const Bool ift=True) 
  {
    AlwaysAssert(which>=0 && which<(Int)itsMappers.nelements(),AipsError);
    return itsMappers[which]->getFTM2(ift);
  };

  
  void initPB();

  const CountedPtr<SIMapper> getMapper(Int which)
  {AlwaysAssert(which>=0 && which<(Int)itsMappers.nelements(),AipsError);
    return itsMappers[which];};

protected:

  ///////////////////// Member Objects

  Block<CountedPtr<SIMapper> >  itsMappers;
  Int oldMsId_p;

  Bool itsIsNonZeroModel;

};


} //# NAMESPACE CASA - END

#endif
