//AngularRate.h generated on 'Thu Feb 04 10:20:05 CET 2010'. Edit at your own risk.
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
 * casacore::File AngularRate.h
 */
#ifndef AngularRate_CLASS
#define AngularRate_CLASS
#include <vector>
#include <iostream>
#include <string>
using namespace std;
#ifndef WITHOUT_ACS
#include <asdmIDLTypesC.h>
using asdmIDLTypes::IDLAngularRate;
#endif
#include <StringTokenizer.h>
#include <NumberFormatException.h>
using asdm::StringTokenizer;
using asdm::NumberFormatException;
#include "EndianStream.h"
using asdm::EndianOSStream;
using asdm::EndianIStream;
namespace asdm {
class AngularRate;
AngularRate operator * ( double , const AngularRate & );
ostream & operator << ( ostream &, const AngularRate & );
istream & operator >> ( istream &, AngularRate &);
/**
 * The AngularRate class implements a quantity of AngularRate in radians per second..
 * 
 * @version 1.00 Jan. 7, 2005
 * @author Allen Farris
 * 
 * @version 1.1 Aug 8, 2006
 * @author Michel Caillat 
 * added toBin/fromBin methods.
 */
class AngularRate {
  /**
   * Overloading of multiplication operator.
   * @param d a value in double precision .
   * @param x a const reference to a AngularRate .
   * @return a AngularRate 
   */
  friend AngularRate operator * ( double d, const AngularRate & x );
  /**
   * Overloading of << to output the value an AngularRate on an ostream.
   * @param os a reference to the ostream to be written on.
   * @param x a const reference to a AngularRate.
   */
  friend ostream & operator << ( ostream & os, const AngularRate & x);
  /**
   * Overloading of >> to read an AngularRate from an istream.
   */
  friend istream & operator >> ( istream & is, AngularRate & x);
public:
	/**
	 * The nullary constructor (default).
	 */
	AngularRate();
	/**
	 * The copy constructor.
	 */
	AngularRate(const AngularRate &);
	/**
	 * A constructor from a string representation.
	 * The string passed in argument must be parsable into a double precision
	 * number to express the value in radian of the angle.
	 *
	 * @param s a string.
	 */
	AngularRate(const string &s);
#ifndef WITHOUT_ACS
	/**
	 *
	 * A constructor from a CORBA/IDL representation.
	 * 
	 * @param idlAngularRate a cons ref to an IDLAngularRate.
	 */
	AngularRate(const IDLAngularRate & idlAngularRate);
#endif
	/**
	 * A constructor from a value in double precision.
	 * The value passed in argument defines the value of the AngularRate in radian.
	 */
	AngularRate(double value);
	/**
	 * The destructor.
	 */
	virtual ~AngularRate();
	/**
	 * A static method equivalent to the constructor from a string.
	 * @param s a string?.
	 */
	static double fromString(const string& s);
	/**
	 * casacore::Conversion into string.
	 * The resulting string contains the representation of the value of this AngularRate.
	 *
	 * @return string
	 */
	static string toString(double);
	/**
	 * Parse the next (string) token of a StringTokenizer into an angle.
	 * @param st a reference to a StringTokenizer.
	 * @return an AngularRate.
	 */
	static AngularRate getAngularRate(StringTokenizer &st) throw(NumberFormatException);
			
	/**
	 * Write the binary representation of this to an EndianOSStream .
	 * @param eoss a reference to an EndianOSStream .
	 */		
	void toBin(EndianOSStream& eoss);
	/**
	 * Write the binary representation of a vector of AngularRate to a EndianOSStream.
	 * @param angle the vector of AngularRate to be written
	 * @param eoss the EndianOSStream to be written to
	 */
	static void toBin(const vector<AngularRate>& angle,  EndianOSStream& eoss);
	
	/**
	 * Write the binary representation of a vector of vector of AngularRate to a EndianOSStream.
	 * @param angle the vector of vector of AngularRate to be written
	 * @param eoss the EndianOSStream to be written to
	 */	
	static void toBin(const vector<vector<AngularRate> >& angle,  EndianOSStream& eoss);
	
	/**
	 * Write the binary representation of a vector of vector of vector of AngularRate to a EndianOSStream.
	 * @param angle the vector of vector of vector of AngularRate to be written
	 * @param eoss the EndianOSStream to be written to
	 */
	static void toBin(const vector<vector<vector<AngularRate> > >& angle,  EndianOSStream& eoss);
	/**
	 * Read the binary representation of an AngularRate from a EndianIStream
	 * and use the read value to set an  AngularRate.
	 * @param eis the EndianStream to be read
	 * @return an AngularRate
	 */
	static AngularRate fromBin(EndianIStream& eis);
	
	/**
	 * Read the binary representation of  a vector of  AngularRate from an EndianIStream
	 * and use the read value to set a vector of  AngularRate.
	 * @param eis a reference to the EndianIStream to be read
	 * @return a vector of AngularRate
	 */	 
	 static vector<AngularRate> from1DBin(EndianIStream & eis);
	 
	/**
	 * Read the binary representation of  a vector of vector of AngularRate from an EndianIStream
	 * and use the read value to set a vector of  vector of AngularRate.
	 * @param eis the EndianIStream to be read
	 * @return a vector of vector of AngularRate
	 */	 
	 static vector<vector<AngularRate> > from2DBin(EndianIStream & eis);
	 
	/**
	 * Read the binary representation of  a vector of vector of vector of AngularRate from an EndianIStream
	 * and use the read value to set a vector of  vector of vector of AngularRate.
	 * @param eis the EndianIStream to be read
	 * @return a vector of vector of vector of AngularRate
	 */	 
	 static vector<vector<vector<AngularRate> > > from3DBin(EndianIStream & eis);	 
	 
	 /**
	  * An assignment operator AngularRate = AngularRate.
	  * @param x a const reference to an AngularRate.
	  */
	 AngularRate & operator = (const AngularRate & x);
	 
	 /**
	  * An assignment operator AngularRate = double.
	  * @param d a value in double precision.
	  */
	 AngularRate & operator = (const double d);
	 /**
	  * Operator increment and assign.
	  * @param x a const reference to an AngularRate.
	  */
	AngularRate & operator += (const AngularRate & x);
	/**
	 * Operator decrement and assign.
	 * @param x a const reference to an AngularRate.
	 */
	AngularRate & operator -= (const AngularRate & x);
	/**
	 * Operator multiply and assign.
	 * @param x a value in double precision.
	 */
	AngularRate & operator *= (const double x);
	/**
	 * Operator divide and assign.
	 * @param x a valye in double precision.
	 */
	AngularRate & operator /= (const double x);
	/**
	 * Addition operator.
	 * @param x a const reference to a AngularRate.
	 */
	AngularRate operator + (const AngularRate & x) const;
	/**
	 * Substraction operator.
	 * @param x a const reference to a AngularRate.
	 */
	AngularRate operator - (const AngularRate & x) const;
	/**
	 * Multiplication operator.
	 * @param x a value in double precision.
	 */
	AngularRate operator * (const double x) const;
	/**
	 * Division operator.
	 * @param d a value in double precision.
	 */
	AngularRate operator / (const double x) const;
	/**
	 * Comparison operator. Less-than.
	 * @param x a const reference to a AngularRate.
	 */
	bool operator < (const AngularRate & x) const;
	/**
	 * Comparison operator. Greater-than.
	 * @param x a const reference to a AngularRate.
	 */
	bool operator > (const AngularRate & x) const;
	/**
	 * Comparison operator. Less-than or equal.
	 * @param x a const reference to a AngularRate.
	 */	
	bool operator <= (const AngularRate & x) const;
	/**
	 * Comparison operator. Greater-than or equal.
	 * @param x a const reference to a AngularRate.
	 */
	bool operator >= (const AngularRate & x) const;
	/**
	 * Comparision operator. Equal-to.
	 * @param x a const reference to a AngularRate.
	 */
	bool operator == (const AngularRate & x) const;
	/** 
	 * Comparison method. Equality.
	 * @param x a const reference to a AngularRate.
	 */
	bool equals(const AngularRate & x) const;
	/**
	 * Comparison operator. Not-equal.
	 * @param x a const reference to a AngularRate.
	 */
	bool operator != (const AngularRate & x) const;
	/**
	 * Comparison method. Test nullity.
	 * @return a bool.
	 */
	bool isZero() const;
	/**
	 * casacore::Unary operator. Opposite.
	 */
	AngularRate operator - () const;
	/**
	 * casacore::Unary operator. casacore::Unary plus.
	 */
	AngularRate operator + () const;
	/**
	 * Converts into a string.
	 * @return a string containing the representation of a the value in double precision.
	 */
	string toString() const;
	/** 
	 * Idem toString.
	 */
	string toStringI() const;
	/**
	 * casacore::Conversion operator.
	 * Converts into a string.
	 */
	operator string () const;
	/**
	 * Return the double precision value of the AngularRate.
	 * @return double
	 */
	double get() const;
#ifndef WITHOUT_ACS
	/**
	 * Return the IDLAngularRate representation of the AngularRate.
	 * @return IDLAngularRate 
	 */
	IDLAngularRate toIDLAngularRate() const;
#endif
	/**
	 * Returns the abbreviated name of the unit implicitely associated to any AngularRate.
	 * @return string
	 */
	static string unit();
private:
	double value;
};
// AngularRate constructors
inline AngularRate::AngularRate() : value(0.0) {
}
inline AngularRate::AngularRate(const AngularRate &t) : value(t.value) {
}
#ifndef WITHOUT_ACS
inline AngularRate::AngularRate(const IDLAngularRate &l) : value(l.value) {
}
#endif
inline AngularRate::AngularRate(const string &s) : value(fromString(s)) {
}
inline AngularRate::AngularRate(double v) : value(v) {
}
// AngularRate destructor
inline AngularRate::~AngularRate() { }
// assignment operator
inline AngularRate & AngularRate::operator = ( const AngularRate &t ) {
	value = t.value;
	return *this;
}
// assignment operator
inline AngularRate & AngularRate::operator = ( const double v ) {
	value = v;
	return *this;
}
// assignment with arithmetic operators
inline AngularRate & AngularRate::operator += ( const AngularRate & t) {
	value += t.value;
	return *this;
}
inline AngularRate & AngularRate::operator -= ( const AngularRate & t) {
	value -= t.value;
	return *this;
}
inline AngularRate & AngularRate::operator *= ( const double n) {
	value *= n;
	return *this;
}
inline AngularRate & AngularRate::operator /= ( const double n) {
	value /= n;
	return *this;
}
// arithmetic functions
inline AngularRate AngularRate::operator + ( const AngularRate &t2 ) const {
	AngularRate tmp;
	tmp.value = value + t2.value;
	return tmp;
}
inline AngularRate AngularRate::operator - ( const AngularRate &t2 ) const {
	AngularRate tmp;
	tmp.value = value - t2.value;
	return tmp;
}
inline AngularRate AngularRate::operator * ( const double n) const {
	AngularRate tmp;
	tmp.value = value * n;
	return tmp;
}
inline AngularRate AngularRate::operator / ( const double n) const {
	AngularRate tmp;
	tmp.value = value / n;
	return tmp;
}
// comparison operators
inline bool AngularRate::operator < (const AngularRate & x) const {
	return (value < x.value);
}
inline bool AngularRate::operator > (const AngularRate & x) const {
	return (value > x.value);
}
inline bool AngularRate::operator <= (const AngularRate & x) const {
	return (value <= x.value);
}
inline bool AngularRate::operator >= (const AngularRate & x) const {
	return (value >= x.value);
}
inline bool AngularRate::equals(const AngularRate & x) const {
	return (value == x.value);
}
inline bool AngularRate::operator == (const AngularRate & x) const {
	return (value == x.value);
}
inline bool AngularRate::operator != (const AngularRate & x) const {
	return (value != x.value);
}
// unary - and + operators
inline AngularRate AngularRate::operator - () const {
	AngularRate tmp;
        tmp.value = -value;
	return tmp;
}
inline AngularRate AngularRate::operator + () const {
	AngularRate tmp;
    tmp.value = value;
	return tmp;
}
// casacore::Conversion functions
inline AngularRate::operator string () const {
	return toString();
}
inline string AngularRate::toString() const {
	return toString(value);
}
inline string AngularRate::toStringI() const {
	return toString(value);
}
inline double AngularRate::get() const {
	return value;
}
#ifndef WITHOUT_ACS
inline IDLAngularRate AngularRate::toIDLAngularRate() const {
	IDLAngularRate tmp;
	tmp.value = value;
	return tmp;
}
#endif
// Friend functions
inline AngularRate operator * ( double n, const AngularRate &x) {
	AngularRate tmp;
	tmp.value = x.value * n;
	return tmp;
}
inline ostream & operator << ( ostream &o, const AngularRate &x ) {
	o << x.value;
	return o;
}
inline istream & operator >> ( istream &i, AngularRate &x ) {
	i >> x.value;
	return i;
}
inline string AngularRate::unit() {
	return string ("rad/s");
}
} // End namespace asdm
#endif /* AngularRate_CLASS */
