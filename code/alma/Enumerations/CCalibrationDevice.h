
#ifndef CCalibrationDevice_H
#define CCalibrationDevice_H

/*
 * ALMA - Atacama Large Millimeter casacore::Array
 * (c) European Southern Observatory, 2002
 * (c) Associated Universities Inc., 2002
 * Copyright by ESO (in the framework of the ALMA collaboration),
 * Copyright by AUI (in the framework of the ALMA collaboration),
 * All rights reserved.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307  USA
 * 
 * /////////////////////////////////////////////////////////////////
 * // WARNING!  DO NOT MODIFY THIS FILE!                          //
 * //  ---------------------------------------------------------  //
 * // | This is generated code!  Do not modify this file.       | //
 * // | Any changes will be lost when the file is re-generated. | //
 * //  ---------------------------------------------------------  //
 * /////////////////////////////////////////////////////////////////
 *
 * casacore::File CCalibrationDevice.h
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <iostream>
#include <string>
#include <vector>
/**
  * A namespace to encapsulate the CalibrationDevice enumeration.
  */
#ifndef WITHOUT_ACS
#include <almaEnumerations_IFC.h>
#else

// This part mimics the behaviour of 
namespace CalibrationDeviceMod
{
  //! CalibrationDevice.
  //! Devices that may be inserted in the optical path in front of the receiver.
  
  const char *const revision = "-1";
  const int version = 1;
  
  enum CalibrationDevice
  { 
    AMBIENT_LOAD /*!< An absorbing load at the ambient temperature. */
     ,
    COLD_LOAD /*!< A cooled absorbing load. */
     ,
    HOT_LOAD /*!< A heated absorbing load. */
     ,
    NOISE_TUBE_LOAD /*!< A noise tube. */
     ,
    QUARTER_WAVE_PLATE /*!< A transparent plate that introduces a 90-degree phase difference between othogonal polarizations. */
     ,
    SOLAR_FILTER /*!< An optical attenuator (to protect receiver from solar heat). */
     ,
    NONE /*!< No device, the receiver looks at the sky (through the telescope). */
     
  };
  typedef CalibrationDevice &CalibrationDevice_out;
} 
#endif

namespace CalibrationDeviceMod {
	std::ostream & operator << ( std::ostream & out, const CalibrationDevice& value);
	std::istream & operator >> ( std::istream & in , CalibrationDevice& value );
}

/** 
  * A helper class for the enumeration CalibrationDevice.
  * 
  */
class CCalibrationDevice {
  public:
 
	/**
	  * Enumerators as strings.
	  */  
	
	static const std::string& sAMBIENT_LOAD; /*!< A const string equal to "AMBIENT_LOAD".*/
	
	static const std::string& sCOLD_LOAD; /*!< A const string equal to "COLD_LOAD".*/
	
	static const std::string& sHOT_LOAD; /*!< A const string equal to "HOT_LOAD".*/
	
	static const std::string& sNOISE_TUBE_LOAD; /*!< A const string equal to "NOISE_TUBE_LOAD".*/
	
	static const std::string& sQUARTER_WAVE_PLATE; /*!< A const string equal to "QUARTER_WAVE_PLATE".*/
	
	static const std::string& sSOLAR_FILTER; /*!< A const string equal to "SOLAR_FILTER".*/
	
	static const std::string& sNONE; /*!< A const string equal to "NONE".*/
	

	/**
	  * Return the major version number as an int.
	  * @return an int.
	  */
	  static int version() ;
	  
	  
	  /**
	    * Return the revision as a string.
	    * @return a string
	    *
	    */
	  static std::string revision() ;
	  
	  
     /**
       * Return the number of enumerators declared in CalibrationDeviceMod::CalibrationDevice.
       * @return an unsigned int.
       */
       static unsigned int size() ;
       
       
    /**
      * Returns an enumerator as a string.
      * @param e an enumerator of CalibrationDeviceMod::CalibrationDevice.
      * @return a string.
      */
	static std::string name(const CalibrationDeviceMod::CalibrationDevice& e);
	
	/**
	  * Equivalent to the name method.
	  */
    static std::string toString(const CalibrationDeviceMod::CalibrationDevice& f) { return name(f); }

	/** 
	  * Returns vector of  all the enumerators as strings. 
	  * The strings are stored in the vector in the same order than the enumerators are declared in the enumeration. 
	  * @return a vector of string.
	  */
     static const std::vector<std::string> names();	 
    
   	
   	// Create a CalibrationDevice enumeration object by specifying its name.
   	static CalibrationDeviceMod::CalibrationDevice newCalibrationDevice(const std::string& name);
   	
   	/*! Return a CalibrationDevice's enumerator  given a string.
   	  * @param name the string representation of the enumerator.
   	 *  @return a CalibrationDeviceMod::CalibrationDevice's enumerator.
   	 *  @throws a string containing an error message if no enumerator could be found for this name.
   	 */
 	static CalibrationDeviceMod::CalibrationDevice literal(const std::string& name);
 	
    /*! Return a CalibrationDevice's enumerator given an unsigned int.
      * @param i the index of the enumerator in CalibrationDeviceMod::CalibrationDevice.
      * @return a CalibrationDeviceMod::CalibrationDevice's enumerator.
      * @throws a string containing an error message if no enumerator could be found for this integer.
      */
 	static CalibrationDeviceMod::CalibrationDevice from_int(unsigned int i);	
 	

  private:
    /* Not Implemented.  This is a pure static class. */
    CCalibrationDevice();
    CCalibrationDevice(const CCalibrationDevice&);
    CCalibrationDevice& operator=(const CCalibrationDevice&);
    
    static std::string badString(const std::string& name) ;
  	static std::string badInt(unsigned int i) ;
  	
};
 
#endif /*!CCalibrationDevice_H*/
