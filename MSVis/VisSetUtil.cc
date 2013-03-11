//# VisSetUtil.cc: VisSet Utilities
//# Copyright (C) 1996,1997,1998,1999,2001,2002
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
//# Correspondence concerning AIPS++ should be adressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//#
//# $Id$

#include <casa/aips.h>

#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/MatrixMath.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/Cube.h>
#include <casa/BasicMath/Math.h>
#include <casa/BasicSL/Complex.h>
#include <casa/BasicSL/Constants.h>
#include <casa/Utilities/Assert.h>

#include <tables/Tables/ArrColDesc.h>
#include <tables/Tables/ScaColDesc.h>
#include <tables/Tables/TiledDataStMan.h>
#include <tables/Tables/TiledShapeStMan.h>
#include <tables/Tables/StandardStMan.h>
#include <tables/Tables/TiledDataStManAccessor.h>
#include <tables/Tables/CompressComplex.h>
#include <tables/Tables/CompressFloat.h>


#include <ms/MeasurementSets/MSColumns.h>

#include <synthesis/MSVis/VisSet.h>
#include <synthesis/MSVis/VisBuffer.h>
#include <synthesis/MSVis/VisSetUtil.h>

#include <casa/Quanta/UnitMap.h>
#include <casa/Quanta/UnitVal.h>
#include <measures/Measures/Stokes.h>
#include <casa/Quanta/MVAngle.h>

#include <casa/Logging/LogIO.h>

#include <casa/iostream.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> 
// </summary>

// <reviewed reviewer="" date="" tests="tMEGI" demos="">

// <prerequisite>
// </prerequisite>
//
// <etymology>
// </etymology>
//
// <synopsis> 
// </synopsis> 
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// </motivation>
//
// <todo asof="">
// </todo>
// Calculate sensitivity
  void VisSetUtil::Sensitivity(VisSet &vs, Matrix<Double>& mssFreqSel,
			       Matrix<Int>& mssChanSel,
			       Quantity& pointsourcesens, 
			       Double& relativesens, 
			       Double& sumwt,
			       Vector<Vector<Int> >& nData,
			       Vector<Vector<Double> >& sumwtChan,
			       Vector<Vector<Double> >& sumwtsqChan,
			       Vector<Vector<Double> >& sumInverseVarianceChan)
{
  ROVisIter& vi(vs.iter());
  VisSetUtil::Sensitivity(vi, mssFreqSel, mssChanSel, pointsourcesens, relativesens, 
			  sumwt, nData, sumwtChan, sumwtsqChan, sumInverseVarianceChan);

}
void VisSetUtil::Sensitivity(ROVisIter &vi, Matrix<Double>& mssFreqSel,
			     Matrix<Int>& mssChanSel,
			     Quantity& pointsourcesens, 
			     Double& relativesens, 
			     Double& sumwt,
			     Vector<Vector<Int> >& nData,
			     Vector<Vector<Double> >& sumwtChan,
			     Vector<Vector<Double> >& sumwtsqChan,
			     Vector<Vector<Double> >& sumInverseVarianceChan)
{
  LogIO os(LogOrigin("VisSetUtil", "Sensitivity()", WHERE));
  
  sumwt=0.0;
  Double sumwtsq=0.0;
  Double sumInverseVariance=0.0;
  //  Vector<Vector<Double> > sumwtChan, sumwtsqChan, sumInverseVarianceChan;
  VisBuffer vb(vi);

  sumwtChan.resize(mssChanSel.shape()(0));
  sumwtsqChan.resize(mssChanSel.shape()(0));
  sumInverseVarianceChan.resize(mssChanSel.shape()(0));
  nData.resize(mssChanSel.shape()(0));
  for (Int spw=0; spw<mssChanSel.shape()(0); spw++)
    {
      Int nc=mssChanSel(spw,2)-mssChanSel(spw,1)+1;
      for (int c=0; c<nc; c++)
	{
	  sumwtChan(spw).resize(nc); sumwtChan(spw)=0.0;
	  sumwtsqChan(spw).resize(nc); sumwtsqChan(spw) = 0.0;
	  sumInverseVarianceChan(spw).resize(nc); sumInverseVarianceChan(spw)=0.0;
	  nData(spw).resize(nc); nData(spw)=0;
	}
    }
  // sumwtChan=0.0;
  // sumwtsqChan=0.0;
  // sumInverseVarianceChan=0.0;

  // Now iterate through the data
  for (vi.originChunks();vi.moreChunks();vi.nextChunk()) 
    {
      for (vi.origin();vi.more();vi++) 
	{
	  Int nRow=vb.nRow();
	  Int nChan=vb.nChannel();
	  Int spw=vb.spectralWindow();
	  Vector<Bool> rowFlags=vb.flagRow();
	  Matrix<Bool> flag = vb.flag();
	  
	  for (Int row=0; row<nRow; row++) 
	    {
	      // TBD: Should probably use weight() here, which updates with calibration
	      if (!rowFlags(row))
		{
		  Double variance=(vb.sigma()(row));
		  for (Int chn=0; chn<nChan; chn++) 
		    {
		      if(!flag(chn,row)&&variance>0.0) 
			{
			  sumwt+=vb.imagingWeight()(chn,row);
			  //sumwtsq+=square(vb.imagingWeight()(chn,row))*variance;
			  sumwtsq+=square(vb.imagingWeight()(chn,row));
			  sumInverseVariance+=1.0/variance;
			  
			  sumwtChan(spw)(chn) += vb.imagingWeight()(chn,row);
			  //sumwtsqChan(spw)(chn)+=square(vb.imagingWeight()(chn,row))*variance;
			  sumwtsqChan(spw)(chn)+=square(vb.imagingWeight()(chn,row));
			  
			  (nData(spw)(chn))++;
			}
		    }
		}
	    }
	}
    }

  // for (Int spw=0; spw<mssChanSel.shape()(0); spw++)
  //   {
  //     cerr << "Spw: " << spw << " " << sumwtChan(spw)  << " " 
  // 	   << sumwtsqChan(spw) <<  " "
  // 	   << sumInverseVarianceChan(spw) << endl;
  //   }


  if(sumwt==0.0) {
    os << "Cannot calculate sensitivity: sum of weights is zero" << endl
       << "Perhaps you need to weight the data" << LogIO::EXCEPTION;
  }
  if(sumInverseVariance==0.0) {
    os << "Cannot calculate sensitivity: sum of inverse variances is zero" << endl
       << "Perhaps you need to weight the data" << LogIO::EXCEPTION;
  }

  Double naturalsens=1.0/sqrt(sumInverseVariance);
  pointsourcesens=Quantity(sqrt(sumwtsq)/sumwt, "Jy");
  relativesens=sqrt(sumwtsq)/sumwt/naturalsens;
}
void VisSetUtil::HanningSmooth(VisSet &vs, const String& dataCol, const Bool& doFlagAndWeight)
{
  VisIter& vi(vs.iter());
  VisSetUtil::HanningSmooth(vi, dataCol, doFlagAndWeight);
}
void VisSetUtil::HanningSmooth(VisIter &vi, const String& dataCol, const Bool& doFlagAndWeight)
{
  LogIO os(LogOrigin("VisSetUtil", "HanningSmooth()"));

  VisBuffer vb(vi);
  Int row, chn, pol;

  for (vi.originChunks();vi.moreChunks();vi.nextChunk()) {
    if (vi.existsWeightSpectrum()) {
      for (vi.origin();vi.more();vi++) {

	Cube<Complex>& vc = ( dataCol=="data" ? vb.visCube() : vb.correctedVisCube());

	Cube<Bool>& fc= vb.flagCube();
	Cube<Float>& wc= vb.weightSpectrum();

	Int nRow=vb.nRow();
	Int nChan=vb.nChannel();
	if (nChan < 3) break;
	Int nPol=vi.visibilityShape()(0);
	Cube<Complex> smoothedData(nPol,nChan,nRow);
	Cube<Bool> newFlag(nPol,nChan,nRow);
	Cube<Float> newWeight(nPol,nChan,nRow);
	for (row=0; row<nRow; row++) {
	  for (pol=0; pol<nPol; pol++) {
	    ///Handle first channel and flag it
	    smoothedData(pol,0,row) = vc(pol,0,row)*0.5 + vc(pol,1,row)*0.5;
	    newWeight(pol,0,row) = 0.0;
	    newFlag(pol,0,row) = True;
	    for (chn=1; chn<nChan-1; chn++) {
	      smoothedData(pol,chn,row) =
		vc(pol,chn-1,row)*0.25 + vc(pol,chn,row)*0.50 +
		vc(pol,chn+1,row)*0.25;
	      if (wc(pol,chn-1,row) != 0 && wc(pol,chn,row) != 0
		  && wc(pol,chn+1,row) != 0) {
		newWeight(pol,chn,row) = 1.0 /
		  (1.0/(wc(pol,chn-1,row)*16.0) + 1.0/(wc(pol,chn,row)*4.0)
		   + 1.0/(wc(pol,chn+1,row)*16.0));
	      } else {
		newWeight(pol,chn,row) = 0.0;
	      }
	      newFlag(pol,chn,row) =
		fc(pol,chn-1,row)||fc(pol,chn,row)||fc(pol,chn+1,row);
	    }
	    //Handle last channel and flag it
	    smoothedData(pol,nChan-1,row) =
	      vc(pol,nChan-2,row)*0.5+vc(pol,nChan-1,row)*0.5;
	    newWeight(pol,nChan-1,row) = 0.0;
	    newFlag(pol,nChan-1,row) = True;  // flag last channel
	  }
	}

	if(dataCol=="data"){
	  if(doFlagAndWeight){
	    vi.setVisAndFlag(smoothedData,newFlag,VisibilityIterator::Observed);
	    vi.setWeightSpectrum(newWeight);
	  }
	  else{
	    vi.setVis(smoothedData,VisibilityIterator::Observed);
	  }	    
	}
	else{
	  if(doFlagAndWeight){
	    vi.setVisAndFlag(smoothedData,newFlag,VisibilityIterator::Corrected);
	    vi.setWeightSpectrum(newWeight);
	  }
	  else{
	    vi.setVis(smoothedData,VisibilityIterator::Corrected);
	  }
	}

      }
    } else {
      for (vi.origin();vi.more();vi++) {

	Cube<Complex>& vc = (dataCol=="data" ? vb.visCube() : vb.correctedVisCube());

	Cube<Bool>& fc= vb.flagCube();
	Matrix<Float>& wm = vb.weightMat();

	Int nRow=vb.nRow();
	Int nChan=vb.nChannel();
	if (nChan < 3) break;
	Int nPol=vi.visibilityShape()(0);
	Cube<Complex> smoothedData(nPol,nChan,nRow);
	Cube<Bool> newFlag(nPol,nChan,nRow);
	Matrix<Float> newWeight(nPol, nRow);
	for (row=0; row<nRow; row++) {
	  for (pol=0; pol<nPol; pol++) {
	    ///Handle first channel and flag it
	    smoothedData(pol,0,row) = vc(pol,0,row)*0.5 + vc(pol,1,row)*0.5;
	    newFlag(pol,0,row) = True;
	    ///Handle chan-independent weights
	    newWeight(pol, row) = 8.0*wm(pol, row)/3.0;
	    for (chn=1; chn<nChan-1; chn++) {
	      smoothedData(pol,chn,row) =
		vc(pol,chn-1,row)*0.25 + vc(pol,chn,row)*0.50 +
		vc(pol,chn+1,row)*0.25;
	      newFlag(pol,chn,row) =
		fc(pol,chn-1,row)||fc(pol,chn,row)||fc(pol,chn+1,row);
	    }
	    //Handle last channel and flag it
	    smoothedData(pol,nChan-1,row) =
	      vc(pol,nChan-2,row)*0.5+vc(pol,nChan-1,row)*0.5;
	    newFlag(pol,nChan-1,row) = True;  // flag last channel
	  }
	}

	if(dataCol=="data"){
	  if(doFlagAndWeight){
	    vi.setVisAndFlag(smoothedData,newFlag,VisibilityIterator::Observed);
	    vi.setWeightMat(newWeight);
	  }
	  else{
	    vi.setVis(smoothedData,VisibilityIterator::Observed);
	  }	    
	}
	else{
	  if(doFlagAndWeight){
	    vi.setVisAndFlag(smoothedData,newFlag,VisibilityIterator::Corrected);
	    vi.setWeightMat(newWeight);
	  }
	  else{
	    vi.setVis(smoothedData,VisibilityIterator::Corrected);
	  }
	}
      }
    }
  }
}

void VisSetUtil::addScrCols(MeasurementSet& ms, Bool addModel, Bool addCorr, 
			    Bool alsoinit, Bool compress) {

  // Add but DO NOT INITIALIZE calibration set (comprising a set of CORRECTED_DATA 
  // and MODEL_DATA columns) to the MeasurementSet.

  // Sense if anything is to be done
  addModel=addModel && !(ms.tableDesc().isColumn("MODEL_DATA"));
  addCorr=addCorr && !(ms.tableDesc().isColumn("CORRECTED_DATA"));

  // Escape (silently) without doing anything if nothing
  // to be done
  if (!addModel && !addCorr) 
    return;
  else {
    // Remove SORTED_TABLE and continue
    // This is needed because old SORTED_TABLE won't see
    //   the added column(s)
    if (ms.keywordSet().isDefined("SORT_COLUMNS")) 
      ms.rwKeywordSet().removeField("SORT_COLUMNS");
    if (ms.keywordSet().isDefined("SORTED_TABLE")) 
      ms.rwKeywordSet().removeField("SORTED_TABLE");
  }

  // If we are adding the MODEL_DATA column, make it
  //  the exclusive origin for model visibilities by
  //  deleting any OTF model keywords
  if (addModel)
    VisSetUtil::remOTFModel(ms);


  // Form log message
  String addMessage("Adding ");
  if (addModel) {
    addMessage+="MODEL_DATA ";
    if (addCorr) addMessage+="and ";
  }
  if (addCorr) addMessage+="CORRECTED_DATA ";
  addMessage+="column(s).";
  LogSink logSink;
  LogMessage message(addMessage,LogOrigin("VisSetUtil","addScrCols"));
  logSink.post(message);
    
    {
      // Define a column accessor to the observed data
      ROTableColumn* data;
      const bool data_is_float = ms.tableDesc().isColumn(MS::columnName(MS::FLOAT_DATA));
      if (data_is_float) {
	data = new ROArrayColumn<Float> (ms, MS::columnName(MS::FLOAT_DATA));
      } else {
	data = new ROArrayColumn<Complex> (ms, MS::columnName(MS::DATA));
      };
      
      // Check if the data column is tiled and, if so, the
      // smallest tile shape used.
      TableDesc td = ms.actualTableDesc();
      const ColumnDesc& cdesc = td[data->columnDesc().name()];
      String dataManType = cdesc.dataManagerType();
      String dataManGroup = cdesc.dataManagerGroup();
      
      IPosition dataTileShape;
      Bool tiled = (dataManType.contains("Tiled"));
      Bool simpleTiling = False;
      
      
      if (tiled) {
	ROTiledStManAccessor tsm(ms, dataManGroup);
	uInt nHyper = tsm.nhypercubes();
	// Find smallest tile shape
	Int lowestProduct=INT_MAX,highestProduct=-INT_MAX;
	Int lowestId=0, highestId=0;
	for (uInt id=0; id < nHyper; id++) {
	  Int product = tsm.getTileShape(id).product();
	  if (product > 0 && (product < lowestProduct)) {
	    lowestProduct = product;
	    lowestId = id;
	  };
	  if (product > 0 && (product > highestProduct)) {
	    highestProduct = product;
	    highestId = id;
	  };
	};
	
	// 2010Oct07 (gmoellen) We will try using
	//  maximum volumne intput tile shape, as this 
	//  improves things drastically for ALMA data with
	//  an enormous range of nchan (e.g., 3840+:1), and
	//  will have no impact on data with a single shape
	//	    dataTileShape = tsm.getTileShape(lowestId);
	dataTileShape = tsm.getTileShape(highestId);
	simpleTiling = (dataTileShape.nelements() == 3);
	
      };
      
      if (!tiled || !simpleTiling) {
	// Untiled, or tiled at a higher than expected dimensionality
	// Use a canonical tile shape of 1 MB size

	MSSpWindowColumns msspwcol(ms.spectralWindow());
	Int maxNchan = max (msspwcol.numChan().getColumn());
	Int tileSize = maxNchan/10 + 1;
	Int nCorr = data->shape(0)(0);
	dataTileShape = IPosition(3, nCorr, tileSize, 131072/nCorr/tileSize + 1);
      };
      
      delete data;

      if(addModel){
	// Add the MODEL_DATA column
	TableDesc tdModel, tdModelComp, tdModelScale;
	CompressComplex* ccModel=NULL;
	String colModel=MS::columnName(MS::MODEL_DATA);
	
	tdModel.addColumn(ArrayColumnDesc<Complex>(colModel,"model data", 2));
	td.addColumn(ArrayColumnDesc<Complex>(colModel,"model data", 2));
	IPosition modelTileShape = dataTileShape;
	if (compress) {
	  tdModelComp.addColumn(ArrayColumnDesc<Int>(colModel+"_COMPRESSED",
						     "model data compressed",2));
	  tdModelScale.addColumn(ScalarColumnDesc<Float>(colModel+"_SCALE"));
	  tdModelScale.addColumn(ScalarColumnDesc<Float>(colModel+"_OFFSET"));
	  ccModel = new CompressComplex(colModel, colModel+"_COMPRESSED",
					colModel+"_SCALE", colModel+"_OFFSET", True);
	
	  StandardStMan modelScaleStMan("ModelScaleOffset");
	  ms.addColumn(tdModelScale, modelScaleStMan);
	
	
	  TiledShapeStMan modelCompStMan("ModelCompTiled", modelTileShape);
	  ms.addColumn(tdModelComp, modelCompStMan);
	  ms.addColumn(tdModel, *ccModel);
	
	} else {
	  MeasurementSet::addColumnToDesc(tdModel, MeasurementSet::MODEL_DATA, 2);
	  //MeasurementSet::addColumnToDesc(td, MeasurementSet::MODEL_DATA, 2);
	  TiledShapeStMan modelStMan("ModelTiled", modelTileShape);
	  //String hcolName=String("Tiled")+String("MODEL_DATA");
	  //tdModel.defineHypercolumn(hcolName,3,
	  //			     stringToVector(colModel));
	  //td.defineHypercolumn(hcolName,3,
	  //		     stringToVector(colModel));
	  ms.addColumn(tdModel, modelStMan);
	};
	if (ccModel) delete ccModel;
      }
      if (addCorr) {
	// Add the CORRECTED_DATA column
	TableDesc tdCorr, tdCorrComp, tdCorrScale;
	CompressComplex* ccCorr=NULL;
	String colCorr=MS::columnName(MS::CORRECTED_DATA);
	
	tdCorr.addColumn(ArrayColumnDesc<Complex>(colCorr,"corrected data", 2));
	IPosition corrTileShape = dataTileShape;
	if (compress) {
	  tdCorrComp.addColumn(ArrayColumnDesc<Int>(colCorr+"_COMPRESSED",
						    "corrected data compressed",2));
	  tdCorrScale.addColumn(ScalarColumnDesc<Float>(colCorr+"_SCALE"));
	  tdCorrScale.addColumn(ScalarColumnDesc<Float>(colCorr+"_OFFSET"));
	  ccCorr = new CompressComplex(colCorr, colCorr+"_COMPRESSED",
				       colCorr+"_SCALE", colCorr+"_OFFSET", True);
	  
	  StandardStMan corrScaleStMan("CorrScaleOffset");
	  ms.addColumn(tdCorrScale, corrScaleStMan);
	  
	  TiledShapeStMan corrCompStMan("CorrectedCompTiled", corrTileShape);
	  ms.addColumn(tdCorrComp, corrCompStMan);
	  ms.addColumn(tdCorr, *ccCorr);
	  
	} else {
	  TiledShapeStMan corrStMan("CorrectedTiled", corrTileShape);
	  ms.addColumn(tdCorr, corrStMan);
	};
	//MeasurementSet::addColumnToDesc(td, MeasurementSet::CORRECTED_DATA, 2);
	
	
	if (ccCorr) delete ccCorr;
      }
      ms.flush();
	
    }
    
    if (alsoinit)
      // Initialize only what we added
      VisSetUtil::initScrCols(ms,addModel,addCorr);

  return;


}

void VisSetUtil::remScrCols(MeasurementSet& ms, Bool remModel, Bool remCorr) {

  Vector<String> colNames(2);
  colNames(0)=MS::columnName(MS::MODEL_DATA);
  colNames(1)=MS::columnName(MS::CORRECTED_DATA);

  Vector<Bool> doCol(2);
  doCol(0)=remModel;
  doCol(1)=remCorr;


  for (uInt j=0; j<colNames.nelements(); j++) {
    if (doCol(j)) {
      if (ms.tableDesc().isColumn(colNames(j))) {
	ms.removeColumn(colNames(j));
      };
      if (ms.tableDesc().isColumn(colNames(j)+"_COMPRESSED")) {
	ms.removeColumn(colNames(j)+"_COMPRESSED");
      };
      if (ms.tableDesc().isColumn(colNames(j)+"_SCALE")) {
	ms.removeColumn(colNames(j)+"_SCALE");
      };
      if (ms.tableDesc().isColumn(colNames(j)+"_OFFSET")) {
	ms.removeColumn(colNames(j)+"_OFFSET");
      };
    };
  };


}
 
void VisSetUtil::remOTFModel(MeasurementSet& ms) {
 VisModelData::clearModel(ms);
}

void VisSetUtil::initScrCols(MeasurementSet& ms, Bool initModel, Bool initCorr) {

  // Sense if anything is to be done (relevant scr cols must be present)
  initModel=initModel && ms.tableDesc().isColumn("MODEL_DATA");
  initCorr=initCorr && ms.tableDesc().isColumn("CORRECTED_DATA");
  
  // Do nothing?
  if (!initModel && !initCorr) 
    return;

 /* Reconsider this trap?
  // Trap missing columns:
  if (initModel && !ms.tableDesc().isColumn("MODEL_DATA"))
    throw(AipsError("Cannot initialize MODEL_DATA if column is absent."));
  if (initCorr && !ms.tableDesc().isColumn("CORRECTED_DATA"))
    throw(AipsError("Cannot initialize CORRECTED_DATA if column is absent."));
 */

  // Create ordinary (un-row-selected) VisibilityIterator from the MS
  VisibilityIterator vi(ms,Block<Int>(),0.0);
  
  // Pass to VisibilityIterator-oriented method
  VisSetUtil::initScrCols(vi,initModel,initCorr);
}


void VisSetUtil::initScrCols(VisibilityIterator& vi, Bool initModel, Bool initCorr) {

  // Sense if anything is to be done (relevant scr cols must be present)
  initModel=initModel && vi.ms().tableDesc().isColumn("MODEL_DATA");
  initCorr=initCorr && vi.ms().tableDesc().isColumn("CORRECTED_DATA");
  
  // Do nothing?
  if (!initModel && !initCorr) 
    return;

 /*  Reconsider this trap?
  // Trap missing columns:
  if (initModel && !vi.ms().tableDesc().isColumn("MODEL_DATA"))
    throw(AipsError("Cannot initialize MODEL_DATA if column is absent."));
  if (initCorr && !vi.ms().tableDesc().isColumn("CORRECTED_DATA"))
    throw(AipsError("Cannot initialize CORRECTED_DATA if column is absent."));
 */

  // Form log message
  String initMessage("Initializing ");
  if (initModel) {
    initMessage+="MODEL_DATA to (unity)";
    if (initCorr) initMessage+=" and ";
  }
  if (initCorr) initMessage+="CORRECTED_DATA (to DATA)";
  initMessage+=".";
  LogSink logSink;
  LogMessage message(initMessage,LogOrigin("VisSetUtil","initScrCols"));
  logSink.post(message);

  Vector<Int> lastCorrType;
  Vector<Bool> zero;
  Int nRows(0);
  vi.setRowBlocking(10000);
  for (vi.originChunks(); vi.moreChunks(); vi.nextChunk()) {

    Vector<Int> corrType; vi.corrType(corrType);
    uInt nCorr = corrType.nelements();
    if (initModel) {
      // figure out which correlations to set to 1. and 0. for the model.
      if (nCorr != lastCorrType.nelements() ||
          !allEQ(corrType, lastCorrType)) {
	
        lastCorrType.resize(nCorr); 
        lastCorrType=corrType;
        zero.resize(nCorr);
	
        for (uInt i=0; i<nCorr; i++) 
          {
            zero[i]=(corrType[i]==Stokes::RL || corrType[i]==Stokes::LR ||
                     corrType[i]==Stokes::XY || corrType[i]==Stokes::YX);
          }
      }
    }
    for (vi.origin(); vi.more(); vi++) {
      nRows+=vi.nRow();

      Cube<Complex> data;
      vi.visibility(data, VisibilityIterator::Observed);
      if (initCorr) {
	// Read DATA and set CORRECTED_DATA
	vi.setVis(data,VisibilityIterator::Corrected);
      }
      if (initModel) {
	// Set MODEL_DATA
	data.set(1.0);
	if (ntrue(zero)>0) {
	  for (uInt i=0; i < nCorr; i++) {
	    if (zero[i]) data(Slice(i), Slice(), Slice()) = Complex(0.0,0.0);
	  }
	}
	vi.setVis(data,VisibilityIterator::Model);
      }
    }
  }
  vi.ms().relinquishAutoLocks();

  vi.originChunks();
  vi.setRowBlocking(0);

  ostringstream os;
  os << "Initialized " << nRows << " rows."; 
  message.message(os.str());
  logSink.post(message);

}  

void VisSetUtil::removeCalSet(MeasurementSet& ms, Bool removeModel) {
  // Remove an existing calibration set (comprising a set of CORRECTED_DATA 
  // and MODEL_DATA columns) from the MeasurementSet.

  //Remove model in header
  if(removeModel)
    VisSetUtil::remOTFModel(ms);

  VisSetUtil::remScrCols(ms,True,True);

}


void VisSetUtil::UVSub(VisSet &vs, Bool reverse)
{
  VisIter& vi(vs.iter());
  VisSetUtil::UVSub(vi, reverse);
}
void VisSetUtil::UVSub(VisIter &vi, Bool reverse)
{
  LogIO os(LogOrigin("VisSetUtil", "UVSub()"));


  VisBuffer vb(vi);

  Int row, chn, pol;

  for (vi.originChunks();vi.moreChunks();vi.nextChunk()) {
    for (vi.origin();vi.more();vi++) {

      Cube<Complex>& vc= vb.correctedVisCube();
      Cube<Complex>& mc= vb.modelVisCube();

      Int nRow=vb.nRow();
      Int nChan=vb.nChannel();
      Int nPol=vi.visibilityShape()(0);
      Cube<Complex> residualData(nPol,nChan,nRow);
      if (reverse) {
	for (row=0; row<nRow; row++) {
	  for (pol=0; pol<nPol; pol++) {
	    for (chn=0; chn<nChan; chn++) {
	      residualData(pol,chn,row) = vc(pol,chn,row)+mc(pol,chn ,row);
	    }
	  }
	}
      } else {
	for (row=0; row<nRow; row++) {
	  for (pol=0; pol<nPol; pol++) {
	    for (chn=0; chn<nChan; chn++) {
	      residualData(pol,chn,row) = vc(pol,chn,row)-mc(pol,chn ,row);
	    }
	  }
	}
      }
      vi.setVis(residualData,VisibilityIterator::Corrected);
    }
  }
}

} //# NAMESPACE CASA - END

