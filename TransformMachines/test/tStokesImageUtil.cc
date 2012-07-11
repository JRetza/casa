//# tVisModelData.cc: Tests the Synthesis model data serving
//# Copyright (C) 2011
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This program is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by the Free
//# Software Foundation; either version 2 of the License, or (at your option)
//# any later version.
//#
//# This program is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//# more details.
//#
//# You should have received a copy of the GNU General Public License along
//# with this program; if not, write to the Free Software Foundation, Inc.,
//# 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id$

/*
#include <casa/Arrays/ArrayMath.h>
#include <components/ComponentModels/ComponentList.h>
#include <components/ComponentModels/ComponentShape.h>
#include <components/ComponentModels/Flux.h>
#include <tables/Tables/ExprNode.h>
#include <measures/Measures/MeasTable.h>

#include <synthesis/TransformMachines/VisModelData.h>
#include <synthesis/TransformMachines/FTMachine.h>
#include <synthesis/TransformMachines/GridFT.h>
#include <synthesis/MSVis/VisibilityIterator.h>
#include <synthesis/MSVis/VisBuffer.h>
#include <casa/OS/Timer.h>
*/
#include <casa/OS/EnvVar.h>
#include <images/Images/FITSImage.h>
#include <synthesis/TransformMachines/StokesImageUtil.h>


#include <casa/namespace.h>

int main(int argc, char **argv) {
	try {
		String casapath = EnvironmentVariable::get("CASAPATH");
		String *parts = new String[2];
		split(casapath, parts, 2, String(" "));
		String datadir = parts[0] + "/data/regression/unittest/synthesis/";
		delete [] parts;
		FITSImage gaussianModel(datadir + "gaussian_model.fits");
		Vector<Float> beam(3, 0);
		AlwaysAssert(
			StokesImageUtil::FitGaussianPSF(gaussianModel, beam),
			AipsError
		);
		AlwaysAssert(beam[0] == 2.5, AipsError);
		AlwaysAssert(beam[1] == 1.25, AipsError);
		AlwaysAssert(near(beam[2], 57.2958), AipsError);
	}
	catch (AipsError x) {
		cout << x.getMesg() << endl;
		cout << "FAIL" << endl;
		return 1;
	}
	cout << "OK" << endl;
	return 0;



}
