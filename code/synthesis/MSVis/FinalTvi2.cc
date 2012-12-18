#include <casa/aips.h>
#include <casa/Arrays.h>
#include <synthesis/MSVis/VisibilityIterator2.h>
#include <synthesis/MSVis/FinalTvi2.h>
#include <ms/MeasurementSets/MeasurementSet.h>
#include <tables/Tables/RefRows.h>
#include <synthesis/MSVis/UtilJ.h>
#include <synthesis/MSVis/VisBuffer2.h>

using namespace casa::utilj;

namespace casa {

namespace vi {

FinalTvi2::FinalTvi2 (ViImplementation2 * inputVi, VisibilityIterator2 * vi,
                      MeasurementSet & finalMs, Bool isWritable)
: TransformingVi2 (inputVi),
  columns_p (),
  columnsAttached_p (False),
  ms_p (finalMs)
{
    VisBufferOptions options = isWritable ? VbWritable : VbNoOptions;
    setVisBuffer (VisBuffer2::factory (vi, VbPlain, options));
}

FinalTvi2::~FinalTvi2 ()
{
}

void
FinalTvi2::configureNewSubchunk ()
{
    Vector<Int> channels = getChannels (0, 0); // args are ignored
    Int nChannels = channels.nelements();

    getVisBuffer()->configureNewSubchunk (0, // always the first MS
                                          ms_p.tableName(),
                                          False,
                                          isNewArrayId (),
                                          isNewFieldId (),
                                          isNewSpectralWindow (),
                                          getSubchunkId (),
                                          nRows(),
                                          nChannels,
                                          getVii()->nPolarizations(),
                                          getVii()->getCorrelations());
}


void
FinalTvi2::origin ()
{
    TransformingVi2::origin ();

    configureNewSubchunk ();
}

void
FinalTvi2::advance ()
{
    TransformingVi2::advance();

    configureNewSubchunk ();
}


void
FinalTvi2::writeBackChanges (VisBuffer2 * vb)
{
    // Extend the measurement set

    if (! columnsAttached_p){

        columns_p.attachColumns (ms_p, True);
        columnsAttached_p = True;
    }

    Bool wasFillable = vb->isFillable();
    vb->setFillable (True);

    Int firstRowAdded = ms_p.nrow();
    ms_p.addRow (vb->nRows());
    RefRows rows (firstRowAdded, ms_p.nrow() - 1);

    // Write out the key column values

    writeKeyValues (ms_p, rows);

    // Write out the data column values

    writeDataValues (ms_p, rows);

    // Write out the other column values

    writeMiscellaneousValues (ms_p, rows);

    vb->setFillable (wasFillable);
}

void
FinalTvi2::writeDataValues (MeasurementSet & /*ms*/, const RefRows & rows)
{
    // Write out the visibility data either complex or float

    if (columns_p.isFloatDataPresent()){
        columns_p.floatVis_p.putColumnCells (rows, getVisBuffer()->visCubeFloat());
    }
    else{
        columns_p.vis_p.putColumnCells (rows, getVisBuffer()->visCube());
    }

    // Write out the corrected and/or model visibilities if they are present

    if (! columns_p.corrVis_p.isNull()){
        columns_p.corrVis_p.putColumnCells (rows, getVisBuffer()->visCubeCorrected());
    }

    if (! columns_p.modelVis_p.isNull()){
        columns_p.modelVis_p.putColumnCells (rows, getVisBuffer()->visCubeModel());
    }

    columns_p.flag_p.putColumnCells (rows, getVisBuffer()->flagCube());
    columns_p.flagRow_p.putColumnCells (rows, getVisBuffer()->flagRow());

    columns_p.sigma_p.putColumnCells (rows, getVisBuffer()->sigmaMat());
    columns_p.weight_p.putColumnCells (rows, getVisBuffer()->weightMat());

    if (! columns_p.weightSpectrum_p.isNull()){
        columns_p.weightSpectrum_p.putColumnCells (rows, getVisBuffer()->weightSpectrum());
    }
}

void
FinalTvi2::writeKeyValues (MeasurementSet & /*ms*/, const RefRows & rows)
{
    columns_p.antenna1_p.putColumnCells (rows, getVisBuffer()->antenna1());

    columns_p.antenna2_p.putColumnCells (rows, getVisBuffer()->antenna2());

    columns_p.feed1_p.putColumnCells (rows, getVisBuffer()->feed1());

    columns_p.feed2_p.putColumnCells (rows, getVisBuffer()->feed2());

    Vector<Int> ddis (getVisBuffer()->nRows(), getVisBuffer()->dataDescriptionId ());
    columns_p.dataDescription_p.putColumnCells (rows, ddis);

    columns_p.processor_p.putColumnCells (rows, getVisBuffer()->processorId());

    columns_p.time_p.putColumnCells (rows, getVisBuffer()->time());

    columns_p.field_p.putColumnCells (rows, Vector<Int> (getVisBuffer()->nRows(), getVisBuffer()->fieldId()));
}

void
FinalTvi2::writeMiscellaneousValues (MeasurementSet & /*ms*/, const RefRows & rows)
{
    columns_p.timeInterval_p.putColumnCells (rows, getVisBuffer()->timeInterval());

    columns_p.exposure_p.putColumnCells (rows, getVisBuffer()->exposure());

    columns_p.timeCentroid_p.putColumnCells (rows, getVisBuffer()->timeCentroid());

    columns_p.scan_p.putColumnCells (rows, getVisBuffer()->scan());

    Vector<Int> arrayIds (getVisBuffer()->nRows(), getVisBuffer()->arrayId ());
    columns_p.array_p.putColumnCells (rows, arrayIds);

    columns_p.observation_p.putColumnCells (rows, getVisBuffer()->observationId());

    columns_p.state_p.putColumnCells (rows, getVisBuffer()->stateId());

    columns_p.uvw_p.putColumnCells (rows, getVisBuffer()->uvw ());
}

void
FinalTvi2::writeFlag (const Matrix<Bool> & /*flag*/)
{
    Throw ("Not Implemented");
}

void
FinalTvi2::writeFlag (const Cube<Bool> & /*flag*/)
{
    Throw ("Not Implemented");
}

void
FinalTvi2::writeFlagRow (const Vector<Bool> & /*rowflags*/)
{
    Throw ("Not Implemented");
}

void
FinalTvi2::writeFlagCategory(const Array<Bool>& /*fc*/)
{
    Throw ("Not Implemented");
}

void
FinalTvi2::writeVisCorrected (const Matrix<CStokesVector> & /*visibilityStokes*/)
{
    Throw ("Not Implemented");
}
void
FinalTvi2::writeVisModel (const Matrix<CStokesVector> & /*visibilityStokes*/)
{
    Throw ("Not Implemented");
}
void
FinalTvi2::writeVisObserved (const Matrix<CStokesVector> & /*visibilityStokes*/)
{
    Throw ("Not Implemented");
}

void
FinalTvi2::writeVisCorrected (const Cube<Complex> & /*vis*/)
{
    Throw ("Not Implemented");
}
void
FinalTvi2::writeVisModel (const Cube<Complex> & /*vis*/)
{
    Throw ("Not Implemented");
}
void
FinalTvi2::writeVisObserved (const Cube<Complex> & /*vis*/)
{
    Throw ("Not Implemented");
}

void
FinalTvi2::writeWeight (const Vector<Float> & /*wt*/)
{
    Throw ("Not Implemented");
}

void
FinalTvi2::writeWeightMat (const Matrix<Float> & /*wtmat*/)
{
    Throw ("Not Implemented");
}

void
FinalTvi2::writeWeightSpectrum (const Cube<Float> & /*wtsp*/)
{
    Throw ("Not Implemented");
}

void
FinalTvi2::writeSigma (const Vector<Float> & /*sig*/)
{
    Throw ("Not Implemented");
}

void
FinalTvi2::writeSigmaMat (const Matrix<Float> & /*sigmat*/)
{
    Throw ("Not Implemented");
}

void
FinalTvi2::writeModel(const RecordInterface& /*rec*/, Bool /*iscomponentlist*/,
                      Bool /*incremental*/)
{
    Throw ("Not Implemented");
}

} // end namespace vi

} //# NAMESPACE CASA - END



