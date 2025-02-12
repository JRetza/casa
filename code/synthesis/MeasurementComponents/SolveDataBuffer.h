//# SolveDataBuffer.h: A container for data and residuals for solving
//# Copyright (C) 2008
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
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id: VisBuffer.h,v 19.14 2006/02/28 04:48:58 mvoronko Exp $

#ifndef SYNTHESIS_SOLVEDATABUFFER_H
#define SYNTHESIS_SOLVEDATABUFFER_H

#include <casa/aips.h>
#include <msvis/MSVis/VisBuffer2.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Cube.h>
#include <casa/Arrays/Matrix.h>
namespace casa { //# NAMESPACE CASA - BEGIN

//#forward

//<summary>SolveDataBuffer is a container for VisBuffer2 data and related residual and differentiation results related to generic calibration solving </summary>
//
// <use visibility=export>
//
// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class="VisBuffer">VisBuffer</linkto>
// </prerequisite>
//
// <etymology>
// SolveDataBuffer is a Buffer for Solving Data
// </etymology>
//
//<synopsis>
// 
// 
//</synopsis>

//<todo>
// <li> write todo list
//</todo>



class SolveDataBuffer 
{
public:
  // Generic ctor
  SolveDataBuffer();

  // Create from a VisBuffer2
  SolveDataBuffer(const vi::VisBuffer2& vb);

  // Copy construct
  // current iteration (or reattach).
  SolveDataBuffer(const SolveDataBuffer& sdb);

  // Destructor 
  ~SolveDataBuffer(); 

  // Assignment
  SolveDataBuffer& operator=(const SolveDataBuffer& sdb);

  // Does SDB contain finite weight?
  bool Ok();

  // Divide corrected by model
  //  NB: disable for now, may not be needed...
  //void divideCorrByModel();

  // Apply amp-only or phase-only to data
  void enforceAPonData(const casacore::String& apmode);

  // Zero flagged weights, and optionally the cross-hand weights
  void enforceSolveWeights(const casacore::Bool pHandOnly);

  // Set the focus channel
  //  (forms references to focus-channel flag/data/model)
  void setFocusChan(const casacore::Int focusChan=-1);

  // Size/init/finalize the residuals workspaces
  void sizeResiduals(const casacore::Int& nPar,const casacore::Int& nDiff);
  void initResidWithModel();
  void finalizeResiduals();

  // Delete the workspaces
  void cleanUp();


  // VB2-like data access methods (mostly const)
  casacore::Int nRows() const { return vb_->nRows(); };
  const casacore::Vector<int>& observationId() const { return vb_->observationId(); };
  const casacore::Vector<casacore::Int>& arrayId() const { return vb_->arrayId(); };
  const casacore::Vector<casacore::Int>& antenna1() const { return vb_->antenna1(); };
  const casacore::Vector<casacore::Int>& antenna2() const { return vb_->antenna2(); };
  const casacore::Vector<casacore::Int>& dataDescriptionIds() const { return vb_->dataDescriptionIds(); };
  const casacore::Vector<casacore::Int>& spectralWindow() const { return vb_->spectralWindows(); };
  const casacore::Vector<casacore::Int>& scan() const { return vb_->scan(); };
  const casacore::Vector<casacore::Double>& time() const { return vb_->time(); };
  const casacore::Vector<casacore::Int>& fieldId() const { return vb_->fieldId(); };
  casacore::Int nChannels() const { return vb_->nChannels(); };
  const casacore::Vector<casacore::Double>& freqs() const { return freqs_; };
  casacore::Int nCorrelations() const { return vb_->nCorrelations(); };
  const casacore::Cube<casacore::Complex>& visCubeModel() const { return vb_->visCubeModel(); };
  const casacore::Cube<casacore::Complex>& visCubeCorrected() const { return vb_->visCubeCorrected(); };

  // The feedPa
  const casacore::Vector<casacore::Float>& feedPa() const { return feedPa_; };

  // These are not const, because we will generally amend them:
  casacore::Vector<casacore::Bool>& flagRow() {fR_.reference(vb_->flagRow());return fR_;};
  casacore::Cube<casacore::Bool>& flagCube() {fC_.reference(vb_->flagCube());return fC_;};
  casacore::Cube<casacore::Float>& weightSpectrum() {wS_.reference(vb_->weightSpectrum());return wS_;};

  // <group>

  // Access functions
  //
  // Access to focus-channel slices of the flags, data, and model
  casacore::Cube<casacore::Bool>& infocusFlagCube() { return infocusFlagCube_p; };
  const casacore::Cube<casacore::Bool>& infocusFlagCube() const {return infocusFlagCube_p;};

  casacore::Cube<casacore::Complex>& infocusVisCube() { return infocusVisCube_p; };
  const casacore::Cube<casacore::Complex>& infocusVisCube() const {return infocusVisCube_p;};

  casacore::Cube<casacore::Float>& infocusWtSpec() { return infocusWtSpec_p; };
  const casacore::Cube<casacore::Float>& infocusWtSpec() const {return infocusWtSpec_p;};

  casacore::Cube<casacore::Complex>& infocusModelVisCube() { return infocusModelVisCube_p; };
  const casacore::Cube<casacore::Complex>& infocusModelVisCube() const {return infocusModelVisCube_p;};

  // Workspace for the residual visibilities
  casacore::Cube<casacore::Complex>& residuals() { return residuals_p; };
  const casacore::Cube<casacore::Complex>& residuals() const {return residuals_p;};

  // Workspace for flags of the residuals
  casacore::Cube<casacore::Bool>& residFlagCube() { return residFlagCube_p; };
  const casacore::Cube<casacore::Bool>& residFlagCube() const {return residFlagCube_p;};

  // Workspace for the differentiated residuals
  casacore::Array<casacore::Complex>& diffResiduals() { return diffResiduals_p; };
  const casacore::Array<casacore::Complex>& diffResiduals() const {return diffResiduals_p;};

  //</group>

protected:

  // Handle copy from the input VB2
  void initFromVB(const vi::VisBuffer2& vb);


private:

  // The underlying VisBuffer2
  vi::VisBuffer2* vb_;

  // The frequencies
  //  Currently, assumed uniform over rows
  casacore::Vector<double> freqs_;

  // The feedPa
  //  Currently, assumed uniform (per antenna) over rows
  casacore::Vector<float> feedPa_;

    // Array reference objects for things in the vb we need to mess with
  casacore::Vector<casacore::Bool> fR_;
  casacore::Cube<casacore::Bool> fC_;
  casacore::Cube<casacore::Float> wS_;
  

  // The current in-focus channel
  casacore::Int focusChan_p;

  // actual storage for the data
  casacore::Cube<casacore::Bool> infocusFlagCube_p;
  casacore::Cube<casacore::Float> infocusWtSpec_p;
  casacore::Cube<casacore::Complex> infocusVisCube_p;
  casacore::Cube<casacore::Complex> infocusModelVisCube_p;

  casacore::Cube<casacore::Complex> residuals_p;
  casacore::Cube<casacore::Bool> residFlagCube_p;
  casacore::Array<casacore::Complex> diffResiduals_p;
};


class SDBList 
{
public:

  // Construct empty list
  SDBList();

  // Destructor
  ~SDBList();

  // How many SDBs?
  casacore::Int nSDB() const { return nSDB_; };

  // Generate a new SDB from an input VB2
  void add(const vi::VisBuffer2& vb);

  // Access an SDB by index
  SolveDataBuffer& operator()(casacore::Int i);

  // Aggregate meta info
  int aggregateObsId() const;
  int aggregateScan() const;
  int aggregateSpw() const;
  int aggregateFld() const;
  double aggregateTime() const;

  // How man correlations
  //   Currently, this insists on uniformity over all SDBs
  int nCorrelations() const;


  // How many data chans?
  //   Currently, this insists on uniformity over all SDBs
  int nChannels() const;

  // The frequencies
  //   Currently, this insists on uniformity over all SDBs
  const casacore::Vector<double>& freqs() const;

  // Does the SDBList contain usable data?
  //  (at least one SDB, with non-zero net weight)
  bool Ok();

  // Aggregated methods
  void enforceAPonData(const casacore::String& apmode);
  void enforceSolveWeights(const casacore::Bool pHandOnly);
  void sizeResiduals(const casacore::Int& nPar, const casacore::Int& nDiff);
  void initResidWithModel();
  void finalizeResiduals();
  //  NB: disable for now, may not be needed...
  //  void divideCorrByModel();

private:

  // How many SDBs contained herein
  casacore::Int nSDB_;

  // Keep SDBs as a list of pointers
  casacore::PtrBlock<SolveDataBuffer*> SDB_;

};



} //# NAMESPACE CASA - END

#endif

