//# TransformVisDataHandler.h: This file contains the interface definition of the TransformVisDataHandler class.
//#
//#  CASA - Common Astronomy Software Applications (http://casa.nrao.edu/)
//#  Copyright (C) Associated Universities, Inc. Washington DC, USA 2011, All rights reserved.
//#  Copyright (C) European Southern Observatory, 2011, All rights reserved.
//#
//#  This library is free software; you can redistribute it and/or
//#  modify it under the terms of the GNU Lesser General Public
//#  License as published by the Free software Foundation; either
//#  version 2.1 of the License, or (at your option) any later version.
//#
//#  This library is distributed in the hope that it will be useful,
//#  but WITHOUT ANY WARRANTY, without even the implied warranty of
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//#  Lesser General Public License for more details.
//#
//#  You should have received a copy of the GNU Lesser General Public
//#  License along with this library; if not, write to the Free Software
//#  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
//#  MA 02111-1307  USA
//# $Id: $

#ifndef TransformVisDataHandler_H_
#define TransformVisDataHandler_H_

#include <casacore/casa/aipstype.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Containers/Record.h>
#include <casacore/casa/Logging/LogIO.h>

#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <synthesis/MSVis/SubMS.h>

#include <synthesis/MSVis/VisibilityIterator2.h>
#include <synthesis/MSVis/VisBuffer2.h>


namespace casa { //# NAMESPACE CASA - BEGIN


typedef map<MS::PredefinedColumns,MS::PredefinedColumns> dataColMap;


namespace tvf
{
	// Returns 1/sqrt(wt) or -1, depending on whether wt is positive..
	Double wtToSigma(Double wt);
}

class TransformVisDataHandler
{

public:

	TransformVisDataHandler();
	TransformVisDataHandler(Record configuration);

	~TransformVisDataHandler();

	void initialize();
	void configure(Record &configuration);

	void open();
	void setup();
	void close();

	vi::VisibilityIterator2 * getVisIter() {return visibilityIterator_p;}

	void fillOutputMs(vi::VisBuffer2 *vb);

protected:

	void checkFillFlagCategory();
	void checkFillWeightSpectrum();
	void checkDataColumnsToFill();
	void setIterationApproach();
	void generateIterator();

	void fillDataCols(vi::VisBuffer2 *vb,RefRows &rowRef);
	void fillAuxCols(vi::VisBuffer2 *vb,RefRows &rowRef);

	String inpMsName_p;
	String outMsName_p;

	SubMS *splitter_p;
	MeasurementSet *inputMs_p;
	MeasurementSet *selectedInputMs_p;
	MeasurementSet *outputMs_p;
	ROMSColumns *inputMsCols_p;
	MSColumns *outputMsCols_p;

	vi::VisibilityIterator2 *visibilityIterator_p;

	String arraySelection_p;
	String fieldSelection_p;
	String scanSelection_p;
	String timeSelection_p;
	String spwSelection_p;
	String baselineSelection_p;
	String uvwSelection_p;
	String polarizationSelection_p;
	String scanIntentSelection_p;
	String observationSelection_p;

	String colname_p;
	String combine_p;
	Double timeBin_p;
	Vector<Int> tileShape_p;
	Vector<Int> chanSpec_p;
	Block<Int> sortColumns_p;

	Bool fillFlagCategory_p;
	Bool fillWeightSpectrum_p;
	Bool correctedToData_p;
	dataColMap dataColMap_p;

	LogIO logger_p;
};

} //# NAMESPACE CASA - END

#endif /* TransformVisDataHandler_H_ */
