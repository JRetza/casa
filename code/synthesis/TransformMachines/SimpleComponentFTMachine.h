//# SimpleComponentFTMachine.h: Definition for SimpleComponentFTMachine
//# Copyright (C) 1996,1997,1998,2000,2001
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
//# Correspondence concerning AIPS++ should be adressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//#
//# $Id$

#ifndef SYNTHESIS_SIMPLECOMPONENTFTMACHINE_H
#define SYNTHESIS_SIMPLECOMPONENTFTMACHINE_H

#include <synthesis/MSVis/VisBuffer.h>
#include <synthesis/TransformMachines/ComponentFTMachine.h>
#include <measures/Measures/MDirection.h>
#include <measures/Measures/MPosition.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Logging/LogIO.h>
#include <casa/Logging/LogSink.h>
#include <casa/Logging/LogMessage.h>


namespace casa { //# NAMESPACE CASA - BEGIN

// <summary>  does the simplest Fourier transform on SkyComponents </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class=ComponentFTMachine>ComponentFTMachine</linkto> class
//   <li> <linkto class=SkyEquation>SkyEquation</linkto> class
//   <li> <linkto class=VisBuffer>VisBuffer</linkto> module
// </prerequisite>
//
// <etymology>
// ComponentFTMachine is a Machine for Fourier Transforms of
// SkyComponents
// </etymology>
//
// <synopsis> 
// Does a simple transform of a sky component. The phase term
// is fully accurate but no smearing is included.
// </synopsis> 
//
// <example>
// </example>
//
// <motivation>
// Allow transformation of sky components.
//
// </motivation>
//
// <todo asof="97/10/01">
// </todo>


// Forward declarations
class ComponentList;


class SimpleComponentFTMachine : public ComponentFTMachine {
public:

  // Get actual coherence 
  virtual void get(VisBuffer& vb, const ComponentList& compList, Int row=-1);
  // Get actual coherence 
  virtual void get(VisBuffer& vb, SkyComponent& skycomponent, Int row=-1);

protected:
  void applyPhasor(const Int part, const Block<Int>& startrow, 
		   const Vector<uInt>& nRowp,  
		   const Vector<Double>& dphase,  const Vector<Double>& invLambda, 
		   const Int npol, const Int nchan, 
		   const Vector<Int>& corrType, 
		   const Cube<DComplex>& dVis, Complex*& modData);
  
  void predictVis(Complex*& modData, const Vector<Double>& invLambda, 
		  const Vector<Double>& frequency, 
		  const ComponentList& compList, 
		  ComponentType::Polarisation poltype, const Vector<Int>& corrType, 
		  const uInt startrow, const uInt nrows, 
		  const uInt nchan, const uInt npol, 
		  const Block<Matrix<Double> > & uvwcomp, 
		  const Block<Vector<Double> > & dphasecomp);

};

} //# NAMESPACE CASA - END

#endif
