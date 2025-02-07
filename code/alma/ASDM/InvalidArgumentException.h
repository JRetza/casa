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
 * casacore::File InvalidArgumentException.h
 */

#ifndef InvalidArgumentException_CLASS
#define InvalidArgumentException_CLASS

#include <string>
using std::string;

namespace asdm {

/**
 * The InvalidArgumentException class represents an exception when 
 * an attempt is made to access something outside of its defined bounds.
 */
class InvalidArgumentException {

public:
  /**
   * An empty CTOR.
   */
  InvalidArgumentException();

  /**
   * A CTOR with a message.
   */
  InvalidArgumentException(const string &s);

  /**
   * The DTOR.
   */
  ~InvalidArgumentException();

  /**
   * @return a text describing the exception.
   */
  string getMessage() const;

protected:
  
  string message;	

};
 
} // End namespace asdm

#endif /* InvalidArgumentException_CLASS */
