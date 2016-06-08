/*
 * SDDoubleCircleGainCal.h
 *
 *  Created on: May 31, 2016
 *      Author: nakazato
 */

#ifndef SYNTHESIS_MEASUREMENTCOMPONENTS_SDDOUBLECIRCLEGAINCALIMPL_H_
#define SYNTHESIS_MEASUREMENTCOMPONENTS_SDDOUBLECIRCLEGAINCALIMPL_H_

#include <casacore/casa/aipstype.h>
#include <casacore/casa/Arrays/Cube.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Logging/LogIO.h>

namespace casa { // namespace casa START

class SDDoubleCircleGainCalImpl {
public:
  SDDoubleCircleGainCalImpl();
  virtual ~SDDoubleCircleGainCalImpl();

  // getter
  casacore::Double getCentralRegion() const {
    return central_region_;
  }
  casacore::Bool isSmoothingActive() const {
    return do_smooth_;
  }
  casacore::Int getSmoothingSize() const {
    return smooth_size_;
  }
  casacore::Double getObservingFrequency() const {
    return observing_frequency_;
  }
  casacore::Double getAntennaDiameter() const {
    return antenna_diameter_;
  }
  casacore::Double getPrimaryBeamSize() const;casacore::Int getDefaultSmoothingSize() const;

  // setter
  void setCentralRegion(casacore::Double value) {
    central_region_ = value;
  }
  void setSmoothing(casacore::Int size) {
    do_smooth_ = True;
    smooth_size_ = size;
  }
  void unsetSmoothing() {
    do_smooth_ = False;
    smooth_size_ = -1;
  }
  void setObservingFrequency(casacore::Double value) {
    observing_frequency_ = value;
  }
  void setAntennaDiameter(casacore::Double value) {
    antenna_diameter_ = value;
  }

  // gain calibration
  // based on Stephen White's IDL script
  void calibrate(casacore::Cube<casacore::Float> const &data,
  casacore::Vector<casacore::Double> const &time,
  casacore::Matrix<casacore::Double> const &direction,
  casacore::Vector<casacore::Double> &gain_time,
  casacore::Cube<casacore::Float> &gain);
  // subspecies that take into account flag (False: valid, True: invalid)
  void calibrate(casacore::Cube<casacore::Float> const &data,
  casacore::Cube<casacore::Bool> const &flag,
  casacore::Vector<casacore::Double> const &time,
  casacore::Matrix<casacore::Double> const &direction,
  casacore::Vector<casacore::Double> &gain_time,
  casacore::Cube<casacore::Float> &gain,
  casacore::Cube<casacore::Bool> &gain_flag);
  // apply gain factor
//  void apply(casacore::Vector<casacore::Double> const &gain_time,
//      casacore::Cube<casacore::Float> const &gain,
//      casacore::Vector<casacore::Double> const &time,
//      casacore::Cube<casacore::Float> &data);

private:
  // radius of the central region [arcsec]
  casacore::Double central_region_;

  // flag for smoothing
  casacore::Bool do_smooth_;

  // smoothing size [ch]
  casacore::Int smooth_size_;

  // parameter for primary beam size determination
  // observing frequency [Hz]
  casacore::Double observing_frequency_;
  // antenna diameter [m]
  casacore::Double antenna_diameter_;

  // logger
  casacore::LogIO logger_;

  // get radius of the central region
  casacore::Double getRadius();

  // get effective smoothing size
  casacore::Int getEffectiveSmoothingSize();

  // find data within radius
  void findDataWithinRadius(casacore::Double const radius,
      casacore::Vector<casacore::Double> const &time,
      casacore::Cube<casacore::Float> const &data,
      casacore::Matrix<casacore::Double> const &direction,
      casacore::Vector<casacore::Double> &gain_time,
      casacore::Cube<casacore::Float> &gain);
};

} // namespace casa END

#endif /* SYNTHESIS_MEASUREMENTCOMPONENTS_SDDOUBLECIRCLEGAINCALIMPL_H_ */
