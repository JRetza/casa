//# ChannelAverageTVI.h: This file contains the interface definition of the MSTransformManager class.
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

#ifndef ChannelAverageTVI_H_
#define ChannelAverageTVI_H_

// Base class
#include <mstransform/TVI/FreqAxisTVI.h>


namespace casa { //# NAMESPACE CASA - BEGIN

namespace vi { //# NAMESPACE VI - BEGIN

//////////////////////////////////////////////////////////////////////////
// ChannelAverageTVI class
//////////////////////////////////////////////////////////////////////////

class ChannelAverageTVI : public FreqAxisTVI
{

public:

	ChannelAverageTVI(	ViImplementation2 * inputVii,
						const Record &configuration);

    void flag(Cube<Bool>& flagCube) const;
    void floatData (Cube<Float> & vis) const;
    void visibilityObserved (Cube<Complex> & vis) const;
    void visibilityCorrected (Cube<Complex> & vis) const;
    void visibilityModel (Cube<Complex> & vis) const;
    void weightSpectrum(Cube<Float> &weightSp) const;
    void sigmaSpectrum (Cube<Float> &sigmaSp) const;

    Vector<Double> getFrequencies (	Double time, Int frameOfReference,
    								Int spectralWindowId, Int msId) const;

    void writeFlag (const Cube<Bool> & flag);

protected:

    void propagateChanAvgFlags (const Cube<Bool> &avgFlagCube, Cube<Bool> &expandedFlagCube);
    Bool parseConfiguration(const Record &configuration);
    void initialize();

	Vector<Int> chanbin_p;
	mutable map<Int,uInt > spwChanbinMap_p; // Must be accessed from const methods
};

//////////////////////////////////////////////////////////////////////////
// ChannelAverageTVIFactory class
//////////////////////////////////////////////////////////////////////////

class ChannelAverageTVIFactory : public ViFactory
{

public:

	ChannelAverageTVIFactory(Record &configuration,ViImplementation2 *inputVII);

protected:

	vi::ViImplementation2 * createVi (VisibilityIterator2 *) const;
	vi::ViImplementation2 * createVi () const;

	Record configuration_p;
	ViImplementation2 *inputVii_p;;
};

//////////////////////////////////////////////////////////////////////////
// ChannelAverageTransformEngine class
//////////////////////////////////////////////////////////////////////////


template<class T> class ChannelAverageKernel; // Forward declaration

template<class T> class ChannelAverageTransformEngine : public FreqAxisTransformEngine2<T>
{

	using FreqAxisTransformEngine2<T>::inputData_p;
	using FreqAxisTransformEngine2<T>::outputData_p;

public:

	ChannelAverageTransformEngine	(	ChannelAverageKernel<T> *kernel,
										DataCubeMap *inputData,
										DataCubeMap *outputData,
										uInt width);

	void transform();

protected:

	uInt width_p;
	// This member has to be a pointer, otherwise there
	// are compile time problems due to the fact that
	// it is a pure virtual class.
	ChannelAverageKernel<T> *chanAvgKernel_p;
};

//////////////////////////////////////////////////////////////////////////
// ChannelAverageKernel class
//////////////////////////////////////////////////////////////////////////

template<class T> class ChannelAverageKernel
{

public:

	virtual void kernel(DataCubeMap *inputData,
						DataCubeMap *outputData,
						uInt startInputPos,
						uInt outputPos,
						uInt width) = 0;
};

//////////////////////////////////////////////////////////////////////////
// PlainChannelAverageKernel class
//////////////////////////////////////////////////////////////////////////

template<class T> class PlainChannelAverageKernel : public ChannelAverageKernel<T>
{

public:

	void kernel(	DataCubeMap *inputData,
					DataCubeMap *outputData,
					uInt startInputPos,
					uInt outputPos,
					uInt width);

};

//////////////////////////////////////////////////////////////////////////
// WeightedChannelAverageKernel class
//////////////////////////////////////////////////////////////////////////

template<class T> class WeightedChannelAverageKernel : public ChannelAverageKernel<T>
{

public:

	void kernel(DataCubeMap *inputData,
				DataCubeMap *outputData,
				uInt startInputPos,
				uInt outputPos,
				uInt width);
};

//////////////////////////////////////////////////////////////////////////
// LogicalANDKernel class
//////////////////////////////////////////////////////////////////////////

template<class T> class LogicalANDKernel : public ChannelAverageKernel<T>
{

public:

	void kernel(DataCubeMap *inputData,
				DataCubeMap *outputData,
				uInt startInputPos,
				uInt outputPos,
				uInt width);
};

//////////////////////////////////////////////////////////////////////////
// ChannelAccumulationKernel class
//////////////////////////////////////////////////////////////////////////

template<class T> class ChannelAccumulationKernel : public ChannelAverageKernel<T>
{

public:

	void kernel(DataCubeMap *inputData,
				DataCubeMap *outputData,
				uInt startInputPos,
				uInt outputPos,
				uInt width);
};

} //# NAMESPACE VI - END

} //# NAMESPACE CASA - END

#endif /* ChannelAverageTVI_H_ */

