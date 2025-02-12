/*
 * ALMA - Atacama Large Millimeter Array
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
 * casacore::File Float.h
 */
# ifndef Float_CLASS
# define Float_CLASS
#include <NumberFormatException.h>

#include <string>

using std::string;

namespace asdm {
/**
 * A collection of static methods to perform conversions
 * between strings and float values.
 */

    class Float {

public:
	/**
	 * Parse a string supposed to represent a double value and returns this value.
	 * @param s the string to parse
	 * @return a float.
	 * @throws NumberFormatException.
	 */
	static float parseFloat(const string &s) throw (NumberFormatException);

	/**
	 * Encode a float value into its string representation.
	 * @param d the float value to be encoded.
	 * @return the string representing the float value passed as parameter.
	 */
	static string toString(float);

  	/**	
	 * The maximum value for a float.
	 */
	static const float MAX_VALUE;

	/**
	 * The minimum value for a float.
	 */
    static const float MIN_VALUE;

};

} // End namespace asdm

#endif /* Float_CLASS */
