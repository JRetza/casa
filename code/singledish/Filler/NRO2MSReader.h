/*
 * NROReader.h
 *
 *  Created on: May 9, 2016
 *      Author: wataru kawasaki
 */

#ifndef SINGLEDISH_FILLER_NRO2MSREADER_H_
#define SINGLEDISH_FILLER_NRO2MSREADER_H_

#define STRING2CHAR(s) const_cast<char *>((s).c_str())

#include <measures/Measures/Stokes.h>

#include <singledish/Filler/ReaderInterface.h>
#include <singledish/Filler/NROData.h>
#include <string>
#include <memory>

using namespace std;

namespace casa { //# NAMESPACE CASA - BEGIN
  
constexpr double kDay2Sec = 86400.0;
constexpr double kSec2Day = 1.0 / kDay2Sec;

class NRO2MSReader: public ReaderInterface {
public:
  NRO2MSReader(std::string const &scantable_name);
  virtual ~NRO2MSReader();

  // get number of rows
  virtual size_t getNumberOfRows();

  virtual casacore::MDirection::Types getDirectionFrame();
  
  virtual casacore::Bool isFloatData() const {
    return true;
  }

  // to get OBSERVATION table
  virtual casacore::Bool getObservationRow(sdfiller::ObservationRecord &record) {
    POST_START;
    casacore::Bool return_value = (*this.*get_observation_row_)(record);
    POST_END;
    return return_value;
  }

  // to get ANTENNA table
  virtual casacore::Bool getAntennaRow(sdfiller::AntennaRecord &record) {
    POST_START;
    casacore::Bool return_value = (*this.*get_antenna_row_)(record);
    POST_END;
    return return_value;
  }

  // to get PROCESSOR table
  virtual casacore::Bool getProcessorRow(sdfiller::ProcessorRecord &record) {
    POST_START;
    casacore::Bool return_value = (*this.*get_processor_row_)(record);
    POST_END;
    return return_value;
  }

  // to get SOURCE table
  virtual casacore::Bool getSourceRow(sdfiller::SourceRecord &record) {
    POST_START;
    casacore::Bool return_value = (*this.*get_source_row_)(record);
    POST_END;
    return return_value;
  }

  // to get FIELD table
  virtual casacore::Bool getFieldRow(sdfiller::FieldRecord &record) {
    POST_START;
    casacore::Bool return_value = (*this.*get_field_row_)(record);
    POST_END;
    return return_value;
  }

  // to get SOURCE table
  virtual casacore::Bool getSpectralWindowRow(sdfiller::SpectralWindowRecord &record) {
    POST_START;
    casacore::Bool return_value = (*this.*get_spw_row_)(record);
    POST_END;
    return return_value;
  }

  // for DataAccumulator
  virtual casacore::Bool getData(size_t irow, sdfiller::DataRecord &record);

  virtual int getNROArraySize() {
    return obs_header_.ARYNM0; //obs_header_.NBEAM * obs_header_.NPOL * obs_header_.NSPWIN;
  }
  virtual int getNRONumBeam() {
    return obs_header_.NBEAM;
  }
  virtual int getNRONumPol() {
    return obs_header_.NPOL;
  }
  virtual int getNRONumSpw() {
    return obs_header_.NSPWIN;
  }

  virtual int getNROArrayBeamId(int array_id) {
//	  assert(array_id >= 0 && array_id < getNROArraySize());
    return array_mapper_[array_id].getBeamId();
  }
  virtual casacore::Stokes::StokesTypes getNROArrayPol(int array_id) {
//	  assert(array_id >= 0 && array_id < getNROArraySize());
    return array_mapper_[array_id].getPol();
  }
  virtual int getNROArraySpwId(int array_id) {
//	  assert(array_id >= 0 && array_id < getNROArraySize());
    return array_mapper_[array_id].getSpwId();
  }

protected:
  void initializeSpecific();
  void finalizeSpecific();

private:
  FILE *fp_;
  sdfiller::NRODataObsHeader obs_header_;
  void readObsHeader();
  void readScanData(int const irow, sdfiller::NRODataScanData &data);
  void checkEndian();
  template<typename T>
  void convertEndian(T &value) {
    char volatile *first = reinterpret_cast<char volatile *>(&value) ;
    char volatile *last = first + sizeof(T) ;
    std::reverse(first, last) ;
  }

  bool same_endian_;

  template<typename T>
  void readHeader(T &v) {
    if ((int)fread(&v, 1, sizeof(T), fp_) != sizeof(T)) {
      cout << "read failed." << endl;
    }
    if (!same_endian_) {
      convertEndian(v);
    }
  }

  template<typename T>
  void readHeader(T *v, size_t numArray) {
    for (size_t i = 0; i < numArray; ++i) {
      readHeader<T>(v[i]);
    }
  }

  void readHeader(string &v, size_t strLength) {
    v.resize(strLength);
    if (fread(STRING2CHAR(v), 1, strLength, fp_) != strLength) {
      cout << "read failed." << endl;
    }
    v.resize(strlen(v.c_str())); // remove trailing null characters
  }

  void readHeader(string *v, size_t strLength, size_t numArray) {
    for (size_t i = 0; i < numArray; ++i) {
      readHeader(v[i], strLength);
    }
  }

  struct NROArrayData {
	  int beam_id=-1;
	  casacore::Stokes::StokesTypes stokes_type = casacore::Stokes::Undefined;
	  string pol_name="";
	  int spw_id=-1;
	  void set(int16_t const arr_data, string const *pol_data) {
		  // indices in NOSTAR data are 1-base
		  if (arr_data < 1101) {
			  throw "An attempt to set invalid ARRAY information to NROArrayData\n";
		  }
		  beam_id = static_cast<int>(arr_data/1000) - 1;
		  int pol_id = static_cast<int>((arr_data % 1000)/100) - 1;
		  spw_id = static_cast<int>(arr_data % 100) -1;
		  pol_name = pol_data[pol_id];
		  stokes_type = casacore::Stokes::type(pol_name);
		  if (stokes_type == casacore::Stokes::Undefined) {
			  throw "Got unsupported polarization type\n";
		  }
	  }
	  int getBeamId() const {
		  if (beam_id < 0) throw "Array data is not set yet\n";
		  return beam_id;}
	  casacore::Stokes::StokesTypes getPol() const {
		  if (stokes_type == casacore::Stokes::Undefined) throw "Array data is not set yet\n";
		  return stokes_type;}
	  int getSpwId() const {
		  if (spw_id < 0) throw "Array data is not set yet\n";
		  return spw_id;}
	  string getPolName() const {
		  if (pol_name.size() == 0) throw "Array data is not set yet\n";
		  return pol_name;}
  };

  std::vector<NROArrayData> array_mapper_;

  void constructArrayTable();
  bool checkScanArray(string const scan_array, NROArrayData const *header_array);

  int beam_id_counter_;
  int source_spw_id_counter_;
  int spw_id_counter_;
  casacore::Vector<casacore::Double> time_range_sec_;
  int const len_obs_header_ = 15136;
  double getMJD(string const &time);
  double getIntMiddleTimeSec(sdfiller::NRODataScanData const &data);
  double getIntStartTimeSec(int const scanno);
  double getIntEndTimeSec(int const scanno);
  void getFullTimeRange();
  double getMiddleOfTimeRangeSec();

  casacore::Double const posx_ = -3.8710235e6;
  casacore::Double const posy_ =  3.4281068e6;
  casacore::Double const posz_ =  3.7240395e6;

  double getRestFrequency(int const spwno);
  string convertVRefName(string const &vref0);
  void shiftFrequency(string const &vdef,
                      double const v,
		      std::vector<double> &freqs);

  std::vector<double> getSpectrum(int const irow, sdfiller::NRODataScanData const &data);
//  casacore::Int getPolNo(string const &rx);

  casacore::Bool (NRO2MSReader::*get_antenna_row_)(sdfiller::AntennaRecord &);
  casacore::Bool (NRO2MSReader::*get_field_row_)(sdfiller::FieldRecord &);
  casacore::Bool (NRO2MSReader::*get_observation_row_)(sdfiller::ObservationRecord &);
  casacore::Bool (NRO2MSReader::*get_processor_row_)(sdfiller::ProcessorRecord &);
  casacore::Bool (NRO2MSReader::*get_source_row_)(sdfiller::SourceRecord &);
  casacore::Bool (NRO2MSReader::*get_spw_row_)(sdfiller::SpectralWindowRecord &);

  casacore::Bool getAntennaRowImpl(sdfiller::AntennaRecord &record);
  casacore::Bool getFieldRowImpl(sdfiller::FieldRecord &record);
  casacore::Bool getObservationRowImpl(sdfiller::ObservationRecord &record);
  casacore::Bool getProcessorRowImpl(sdfiller::ProcessorRecord &record);
  casacore::Bool getSourceRowImpl(sdfiller::SourceRecord &record);
  casacore::Bool getSpectralWindowRowImpl(sdfiller::SpectralWindowRecord &record);

  template<class _Record>
  casacore::Bool noMoreRowImpl(_Record &) {
    POST_START;POST_END;
    return false;
  }
};

} //# NAMESPACE CASA - END

#endif /* SINGLEDISH_FILLER_NRO2MSREADER_H_ */
