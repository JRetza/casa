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
 * casacore::File ComplexWrapper.h
 */

#ifndef Complex_CLASS
#define Complex_CLASS

#include <vector>
#include <complex>
#include <iostream>
#include <string>
using namespace std;
using std::complex;

#ifndef WITHOUT_ACS
#include <asdmIDLTypesC.h>
using asdmIDLTypes::IDLComplex;
#endif

#include <DoubleWrapper.h>
#include <StringTokenizer.h>
#include <NumberFormatException.h>
using asdm::Double;
using asdm::StringTokenizer;
using asdm::NumberFormatException;

#include "EndianStream.h"
using asdm::EndianOSStream;
using asdm::EndianIStream;

namespace asdm {

/**
 * The casacore::Complex class extends the casacore::Complex class in the C++ standard library.
 * 
 * @version 1.00 Jan. 7, 2005
 * @author Allen Farris
 * @version 1.1 Aug 8, 2006
 * @author Michel Caillat 
 * added toBin/fromBin methods.
 */

    class Complex : public std::complex<double> {

public:
	static Complex fromString(const string&) throw(NumberFormatException);
	static string toString(const Complex&);
	static Complex getComplex(StringTokenizer &t) throw(NumberFormatException);

	Complex();                              		// default constructor
	Complex(const Complex &);						// X const X& constructor
	Complex(const string &s);
#ifndef WITHOUT_ACS
	Complex(const IDLComplex &);
#endif
	Complex(double re, double im);

	double getReal() const;
	double getImg() const;
	void setReal(double re);
	void setImg(double im);

	bool isZero() const;
	bool equals(const Complex &) const;

	string toString() const;
#ifndef WITHOUT_ACS
	IDLComplex toIDLComplex() const;
#endif

	/**
	 * Write the binary representation of this to a EndianOSStream.
	 */		
	void toBin(EndianOSStream& eoss);

	/**
	 * Write the binary representation of a vector of Complex to a EndianOSStream.
	 * @param cmplx the vector of Complex to be written
	 * @param eoss the EndianOSStream to be written to
	 */
	static void toBin(const vector<Complex>& cmplx,  EndianOSStream& eoss);
	
	/**
	 * Write the binary representation of a vector of vector of Complex to a EndianOSStream.
	 * @param cmplx the vector of vector of Complex to be written
	 * @param eoss the EndianOSStream to be written to
	 */	
	static void toBin(const vector<vector<Complex> >& cmplx,  EndianOSStream& eoss);
	
	/**
	 * Write the binary representation of a vector of vector of vector of Complex to a EndianOSStream.
	 * @param cmplx the vector of vector of vector of Complex to be written
	 * @param eoss the EndianOSStream to be written to
	 */
	static void toBin(const vector<vector<vector<Complex> > >& cmplx,  EndianOSStream& eoss);

	/**
	 * Read the binary representation of an Complex from a EndianIStream
	 * and use the read value to set an  Complex.
	 * @param eis the EndianStream to be read
	 * @return an Complex
	 */
	static Complex fromBin(EndianIStream& eis);
	
	/**
	 * Read the binary representation of  a vector of  Complex from an EndianIStream
	 * and use the read value to set a vector of  Complex.
	 * @param dis the EndianIStream to be read
	 * @return a vector of Complex
	 */	 
	 static vector<Complex> from1DBin(EndianIStream & eis);
	 
	/**
	 * Read the binary representation of  a vector of vector of Complex from an EndianIStream
	 * and use the read value to set a vector of  vector of Complex.
	 * @param eiis the EndianIStream to be read
	 * @return a vector of vector of Complex
	 */	 
	 static vector<vector<Complex> > from2DBin(EndianIStream & eis);
	 
	/**
	 * Read the binary representation of  a vector of vector of vector of Complex from an EndianIStream
	 * and use the read value to set a vector of  vector of vector of Complex.
	 * @param eis the EndianIStream to be read
	 * @return a vector of vector of vector of Complex
	 */	 
	 static vector<vector<vector<Complex> > > from3DBin(EndianIStream & eis);

};

// Complex constructors
inline Complex::Complex() : std::complex<double>(0.0,0.0) {
}

inline Complex::Complex(const Complex &t) : std::complex<double>(t.real(),t.imag()) {
}

inline Complex::Complex(const string &s) : std::complex<double>(Complex::fromString(s)) {
}

#ifndef WITHOUT_ACS
inline Complex::Complex(const IDLComplex &l) : std::complex<double>(l.re,l.im) {
}
#endif

inline Complex::Complex(double r, double i) : std::complex<double>(r,i) {
}

inline double Complex::getReal() const {
	return real();
}

inline double Complex::getImg() const {
	return imag();
}

inline void Complex::setReal(double re) {
	*this = Complex(re,imag());
}

inline void Complex::setImg(double im) {
	*this = Complex(real(),im);
}

inline bool Complex::isZero() const {
	return real() == 0.0 && imag() == 0.0;
}

inline bool Complex::equals(const Complex &x) const {
	return real() == x.real() && imag() == x.imag();
}

#ifndef WITHOUT_ACS
inline IDLComplex Complex::toIDLComplex() const {
	IDLComplex x;
	x.re = getReal();
	x.im = getImg();
	return x;
}
#endif

inline string Complex::toString() const {
	return Double::toString(getReal()) + " " + Double::toString(getImg());
}

} // End namespace asdm

#endif /* Complex_CLASS */
