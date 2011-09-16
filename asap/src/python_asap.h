//#---------------------------------------------------------------------------
//# python_asap.h: python module for single dish package asap
//#---------------------------------------------------------------------------
//# Copyright (C) 2004
//# ATNF
//#
//# This program is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by the Free
//# Software Foundation; either version 2 of the License, or (at your option)
//# any later version.
//#
//# This program is distributed in the hope that it will be useful, but
//# WITHOUT ANY WARRANTY; without even the implied warranty of
//# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
//# Public License for more details.
//#
//# You should have received a copy of the GNU General Public License along
//# with this program; if not, write to the Free Software Foundation, Inc.,
//# 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning this software should be addressed as follows:
//#        Internet email: Malte.Marquarding@csiro.au
//#        Postal address: Malte Marquarding,
//#                        Australia Telescope National Facility,
//#                        P.O. Box 76,
//#                        Epping, NSW, 2121,
//#                        AUSTRALIA
//#
//# $Id: python_asap.h 2163 2011-05-10 05:02:56Z MalteMarquarding $
//#---------------------------------------------------------------------------
#ifndef PYTHON_ASAP_H
#define PYTHON_ASAP_H

namespace asap {
  namespace python {
    void translate_ex(const casa::AipsError& e);
    void python_Scantable();
    void python_STFiller();
    void python_Filler();
    void python_MSFiller();
    void python_STSelector();
    void python_STMath();
    void python_Fitter();
    void python_STLineFinder();
    void python_STFitEntry();
    void python_STWriter();
    void python_MSWriter();
    void python_LineCatalog();
    void python_Logger();
    void python_LogSink();
    void python_STCoordinate();
    void python_STAtmosphere();
    void python_SrcType();

  } // python
} //asap

#endif
