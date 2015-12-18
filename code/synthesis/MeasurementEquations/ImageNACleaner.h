//# ImageNACleaner.h: this defines Cleaner a class for doing deconvolution
//# Copyright (C) 2015
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by
//# the Free Software Foundation; either version 3 of the License, or (at your
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
//#
//# $Id: $

#ifndef SYNTHESIS_IMAGENACLEANER_H
#define SYNTHESIS_IMAGENACLEANER_H

//# Includes
#include <synthesis/MeasurementEquations/MatrixNACleaner.h>
#include <casa/Utilities/CountedPtr.h>
namespace casa { //# NAMESPACE CASA - BEGIN

//# Forward Declarations
template <class T> class Matrix;
template <class T> class ImageInterface;



// <summary>A class interfacing images to MatrixNACleaner </summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="tLatticeCleaner">
// </reviewed>

// <prerequisite>
//  <li> ImageInterface
//  <li> MatrixNACleaner
// </prerequisite>
//
// <etymology>

// The ImageCleaner class will use Non Amnesiac on Images.

// </etymology>
//
// <synopsis>
// This class will perform some kind  of Clean deconvolution
// on Lattices.
//
// </synopsis>
//
// <example>
// <srcblock>
// </srcblock> 
// </example>
//
// <motivation>
// </motivation>
//
// <thrown>
// <li> AipsError: if psf has more dimensions than the model. 
// </thrown>
//

class ImageNACleaner

{

 public:
  //Default
  ImageNACleaner();
  // Create a cleaner with a psf and dirty image
  ImageNACleaner(ImageInterface<Float>& psf, ImageInterface<Float>& dirty);
  //assignmemnt constructor
  ImageNACleaner(const ImageNACleaner& other);
  //assignment operator
  ImageNACleaner & operator=(const ImageNACleaner & other);

  //The destructor
  ~ImageNACleaner();
  
  // Update the dirty image only
  void setDirty(ImageInterface<Float> & dirty);
  // Change the psf image 
  void setPsf(ImageInterface<Float> & psf);

 
 
  // niter - number of iterations
  // gain - loop gain used in cleaning (a fraction of the maximum 
  //        subtracted at every iteration)
  // aThreshold - absolute threshold to stop iterations
  // masksupport is +-number of pixel around a peak to remember
  // memtype is memory function to use 0- no memory standard clean, 1 weak memory, 2 medium, 3 strong
  // numsigma: keep memory of position if peak is above this value 
  void setcontrol(const Int niter,
		  const Float gain, const Quantity& aThreshold,
		  const Int masksupp=3, const Int memtype=2, const Float numSigma=5.0);
  
  
  // return how many iterations we did do
  Int iteration() const ;
 
  
  // what iteration number to start on
  void startingIteration(const Int starting = 0);

  // Clean an image. 
  //return value gives you a hint of what's happening
  //  1 = converged
  //  0 = not converged but behaving normally
  // -1 = not converged and stopped on cleaning consecutive smallest scale
  // -2 = not converged and either large scale hit negative or diverging 
  // -3 = clean is diverging rather than converging 
  Int clean(ImageInterface<Float> & model, 
	    const Int niter, const Float gain, const Quantity& threshold, const Int masksupp=3,
	    const Int memType=2, const Float numsigma=5.0);
  // Set the mask
  // mask - input mask lattice
  // This is where the algorithm searched...the value of the mask shows the weight 
  //importance
  // code is exactly the same as before this parameter has been introduced.
  void setMask(ImageInterface<Float> & mask);
  //Max residual after last clean
  Float maxResidual() {return maxResidual_p;};

 private:
  //Helper function to setup some param
  Bool setupMatCleaner(const Int niter, const Float gain, 
		       const Quantity& threshold, const Int supp=3, const Int memType=2, const Float numsigma=5.0);
  MatrixNACleaner matClean_p;
  CountedPtr<ImageInterface<Float> > psf_p;
  CountedPtr<ImageInterface<Float> >dirty_p;
  CountedPtr<ImageInterface<Float> > mask_p;
  Int nPsfChan_p;
  Int nImChan_p;
  Int nPsfPol_p;
  Int nImPol_p;
  Int chanAxis_p;
  Int polAxis_p;
  Int nMaskChan_p;
  Int nMaskPol_p;
  Float maxResidual_p;

};

} //# NAMESPACE CASA - END

#endif
