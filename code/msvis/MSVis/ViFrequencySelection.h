//# ViFrequencySelection.h: Step through the MeasurementEquation by visibility
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2002,2003
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
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
//# $Id: ViFrequencySelection.h,v 19.14 2006/02/28 04:48:58 mvoronko Exp $

#if ! defined (MSVIS_ViFrequencySelection_H_121116_1101)
#define MSVIS_ViFrequencySelection_H_121116_1101

#include <casa/aips.h>
#include <casa/BasicSL.h>
#include <casa/Arrays/Slicer.h>
#include <measures/Measures/MFrequency.h>


#include <set>
#include <vector>

using std::set;
using std::vector;

namespace casacore{

class MeasurementSet;
class MSSelection;
}

namespace casa { //# NAMESPACE CASA - BEGIN


namespace vi {

///////////////////////////////////////////////////////////////////
//
//  FrequencySelection class
//
// A frequency selection is a way to select the channels of interest from the
// data in a single MeasurementSet.  The user will provide one or more selections
// for the casacore::MS; each selection effectively specify the desired channel data in a
// specified spectral window.  If the user uses the FrequencySelectionChannel
// class then the selection simply selects a range of channels.  The other derived
// class is FrequencySelectionReferential which specifies a range of frequencies
// in a specified frame of reference (e.g., LSRK).  Unlike the other first method,
// the frame-related selection will not necessarily select the same channels across
// the entire MS.
//
// The frame of reference will either be one defined in the casacore::MFrequency::Types enum
// or one of the special "frames" defined in this class.

class FrequencySelection {

public:

    typedef enum {Unknown = -11, ByChannel = -10} SpecialFrames;

    virtual ~FrequencySelection (){}

    void addCorrelationSlices (const casacore::Vector <casacore::Vector <casacore::Slice> > & slices);
    virtual FrequencySelection * clone () const = 0;
    virtual casacore::Bool empty () const = 0;
    void filterByWindow (casacore::Int windowId = -1) const;
    casacore::Vector <casacore::Slice> getCorrelationSlices (casacore::Int polarizationId) const;
    casacore::Int getFrameOfReference () const;
    virtual set<int> getSelectedWindows () const = 0;
    virtual casacore::String toString () const = 0;

    static casacore::String frameName (casacore::Int referenceFrame);

//**********************************************************************
// Internal methods below this line
//**********************************************************************

protected:

    FrequencySelection (casacore::Int referenceFrame)
    : filterWindowId_p (-1),
      referenceFrame_p (referenceFrame) {}
    casacore::Int filterWindow() const;

private:

    casacore::Vector <casacore::Vector <casacore::Slice> > correlationSlices_p; // outer index is polarization id
    mutable casacore::Int filterWindowId_p;
    casacore::Int referenceFrame_p;
};

///////////////////////////////////////////////////////////////////
//
// FrequencySelectionUsingChannels class
//
// Selects sets of channels from a single MS.  The selection is created
// by specifying a sequence of channels in a single MS.  By adding multiple
// selections, the user can select any arbitrary collecton of channels.
//
// The order of the "add" operations are unimportant.

class FrequencySelectionUsingChannels : public FrequencySelection {

public:

    class Element {
    public:

        Element (casacore::Int spectralWindow = -1, casacore::Int firstChannel = -1, casacore::Int nChannels = -1, casacore::Int increment = 1)
        : firstChannel_p (firstChannel),
          increment_p (increment),
          nChannels_p (nChannels),
          spectralWindow_p (spectralWindow)
        {}

        casacore::Slice getSlice () const { return casacore::Slice (firstChannel_p, nChannels_p, increment_p);}

        casacore::Int firstChannel_p;
        casacore::Int increment_p;
        casacore::Int nChannels_p;
        casacore::Int spectralWindow_p;
    };

    typedef std::vector<Element> Elements;
    typedef Elements::const_iterator const_iterator;

    FrequencySelectionUsingChannels () : FrequencySelection (ByChannel) {}

    void add (casacore::Int spectralWindow, casacore::Int firstChannel, casacore::Int nChannels, casacore::Int increment = 1);
    void add (const casacore::MSSelection & msSelection, const casacore::MeasurementSet * ms);
    const_iterator begin () const;
    FrequencySelection * clone () const;
    casacore::Bool empty () const;
    const_iterator end () const;
    casacore::Int getNChannels (casacore::Int spectralWindowId) const;
    set<int> getSelectedWindows () const;
    size_t size () const;
    casacore::String toString () const;

//**********************************************************************
// Internal methods below this line
//**********************************************************************

private:

    Elements elements_p;
    mutable Elements filtered_p;

};

///////////////////////////////////////////////////////////////////
//
// FrequencySelectionUsingFrame class
//
// Selects sets of channels from a single MS.  The selection is created
// by specifying a sequence of frame-related frequencies in a single MS.
// By adding multiple selections, the user can select any arbitrary collecton
// of channels.  The frequencies are related to the specified frame of reference
// and the actual selected can be a function of time as the telescope moves through
// space.

class FrequencySelectionUsingFrame : public FrequencySelection {

public:

    class Element {
    public:

        Element (casacore::Int spectralWindow = -1, casacore::Double beginFrequency = 0,
                 casacore::Double endFrequency = 0, casacore::Double increment = 0)
        : beginFrequency_p (beginFrequency),
          endFrequency_p (endFrequency),
          increment_p (increment),
          spectralWindow_p (spectralWindow)
          {}

        casacore::Double getBeginFrequency () const;
        casacore::Double getEndFrequency () const;

    private:

        friend class FrequencySelectionUsingFrame;

        casacore::Double beginFrequency_p;
        casacore::Double endFrequency_p;
        casacore::Double increment_p;
        casacore::Int spectralWindow_p;
    };

    typedef std::vector<Element> Elements;
    typedef Elements::const_iterator const_iterator;

    FrequencySelectionUsingFrame (casacore::MFrequency::Types frameOfReference);

    void add (casacore::Int spectralWindow, casacore::Double bottomFrequency, casacore::Double topFrequency);
    //void add (casacore::Int spectralWindow, casacore::Double bottomFrequency, casacore::Double topFrequency, casacore::Double increment);
    const_iterator begin () const;
    FrequencySelection * clone () const;
    casacore::Bool empty () const;
    const_iterator end () const;
    set<int> getSelectedWindows () const;
    casacore::String toString () const;

private:

    Elements elements_p;
    mutable Elements filtered_p;
};


///////////////////////////////////////////////////////////////////
//
//    FrequencySelections class
//
//    A FrequenceSelections object is a collection of FrequencySelection objects.
//    It is intended to allow the user to provide a frequency selection per
//    casacore::MS when the VisibilityIterator is sweeping multiple MSs.  All selections
//    included in the collection must have the same frame of reference; an
//    exception will be thrown when attempting to add a frame with a different
//    frame of reference.

class FrequencySelections {
public:


    FrequencySelections ();
    FrequencySelections (const FrequencySelections & other);
    ~FrequencySelections ();

    void add (const FrequencySelection & selection);
    FrequencySelections * clone () const;
    void filterToSpectralWindow (casacore::Int spectralWindowId);
    const FrequencySelection & get (casacore::Int msIndex) const;
    casacore::Int getFrameOfReference () const;
    casacore::Bool isSpectralWindowSelected (casacore::Int msIndex, casacore::Int spectralWindowId) const;
    casacore::Int size () const;

//**********************************************************************
// Internal methods below this line
//**********************************************************************


private:

    typedef std::set<pair<casacore::Int, casacore::Int> > SelectedWindows; // pair=(msIndex,spwId)

    const FrequencySelectionUsingChannels defaultSelection_p;
    mutable casacore::Int filterWindow_p;
    SelectedWindows selectedWindows_p;

    typedef std::vector<FrequencySelection *> Selections;
    Selections selections_p;
};



} // end namespace vi

} //# NAMESPACE CASA - END

#endif // ! defined (MSVIS_ViFrequencySelection_H_121116_1101)

