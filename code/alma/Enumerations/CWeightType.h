
#ifndef CWeightType_H
#define CWeightType_H

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
 * casacore::File CWeightType.h
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <iostream>
#include <string>
#include <vector>
/**
  * A namespace to encapsulate the WeightType enumeration.
  */
#ifndef WITHOUT_ACS
#include <almaEnumerations_IFC.h>
#else

// This part mimics the behaviour of 
namespace WeightTypeMod
{
  //! WeightType.
  //! 
  
  const char *const revision = "-1";
  const int version = 1;
  
  enum WeightType
  { 
    K /*!< Based on System temperature. */
     ,
    JY /*!< Based on Flux (include antenna efficiency). */
     ,
    COUNT_WEIGHT /*!< Square-root of the number of samples (i.e. sqrt(bandwidth * time)) */
     
  };
  typedef WeightType &WeightType_out;
} 
#endif

namespace WeightTypeMod {
	std::ostream & operator << ( std::ostream & out, const WeightType& value);
	std::istream & operator >> ( std::istream & in , WeightType& value );
}

/** 
  * A helper class for the enumeration WeightType.
  * 
  */
class CWeightType {
  public:
 
	/**
	  * Enumerators as strings.
	  */  
	
	static const std::string& sK; /*!< A const string equal to "K".*/
	
	static const std::string& sJY; /*!< A const string equal to "JY".*/
	
	static const std::string& sCOUNT_WEIGHT; /*!< A const string equal to "COUNT_WEIGHT".*/
	

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
       * Return the number of enumerators declared in WeightTypeMod::WeightType.
       * @return an unsigned int.
       */
       static unsigned int size() ;
       
       
    /**
      * Returns an enumerator as a string.
      * @param e an enumerator of WeightTypeMod::WeightType.
      * @return a string.
      */
	static std::string name(const WeightTypeMod::WeightType& e);
	
	/**
	  * Equivalent to the name method.
	  */
    static std::string toString(const WeightTypeMod::WeightType& f) { return name(f); }

	/** 
	  * Returns vector of  all the enumerators as strings. 
	  * The strings are stored in the vector in the same order than the enumerators are declared in the enumeration. 
	  * @return a vector of string.
	  */
     static const std::vector<std::string> names();	 
    
   	
   	// Create a WeightType enumeration object by specifying its name.
   	static WeightTypeMod::WeightType newWeightType(const std::string& name);
   	
   	/*! Return a WeightType's enumerator  given a string.
   	  * @param name the string representation of the enumerator.
   	 *  @return a WeightTypeMod::WeightType's enumerator.
   	 *  @throws a string containing an error message if no enumerator could be found for this name.
   	 */
 	static WeightTypeMod::WeightType literal(const std::string& name);
 	
    /*! Return a WeightType's enumerator given an unsigned int.
      * @param i the index of the enumerator in WeightTypeMod::WeightType.
      * @return a WeightTypeMod::WeightType's enumerator.
      * @throws a string containing an error message if no enumerator could be found for this integer.
      */
 	static WeightTypeMod::WeightType from_int(unsigned int i);	
 	

  private:
    /* Not Implemented.  This is a pure static class. */
    CWeightType();
    CWeightType(const CWeightType&);
    CWeightType& operator=(const CWeightType&);
    
    static std::string badString(const std::string& name) ;
  	static std::string badInt(unsigned int i) ;
  	
};
 
#endif /*!CWeightType_H*/
