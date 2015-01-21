#include <iostream>
#include <string>
#include <vector>

//#include <libsakura/sakura.h>
//#include <libsakura/config.h>

#include <casa/Logging/LogIO.h>
#include <casa/Logging/LogOrigin.h>
#include <casa/Utilities/Assert.h>
#include <casa/Arrays/ArrayMath.h>

#include <ms/MeasurementSets/MSSelectionTools.h>
#include <msvis/MSVis/VisibilityIterator2.h>
#include <msvis/MSVis/VisSetUtil.h>

#include <casa_sakura/SakuraUtils.h>
#include <singledish/SingleDish/SingleDishMS.h>

//---for measuring elapse time------------------------
#include <sys/time.h>
double gettimeofday_sec() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec + (double)tv.tv_usec*1.0e-6;
}
//----------------------------------------------------

#define _ORIGIN LogOrigin("SingleDishMS", __func__, WHERE)

namespace casa {

SingleDishMS::SingleDishMS()
  : msname_(""), ms_(0), mssel_(0)
{
  initialize();
}

SingleDishMS::SingleDishMS(string const& ms_name)
  : msname_(ms_name), ms_(0), mssel_(0)
{
  LogIO os(_ORIGIN);
  initialize();
  // Make a MeasurementSet object for the disk-base MeasurementSet
  String lems(ms_name);
  ms_ = new MeasurementSet(lems, TableLock(TableLock::AutoNoReadLocking), 
			    Table::Update);
  os << "Opened Measurement set " << name() << LogIO::POST;
  AlwaysAssert(ms_, AipsError);
}

SingleDishMS::SingleDishMS(MeasurementSet &ms)
  : msname_(""), ms_(0), mssel_(0)
{
  initialize();
  msname_ = static_cast<string>(ms.tableName());
  ms_ = new MeasurementSet(ms);
  AlwaysAssert(ms_, AipsError);
}

SingleDishMS::SingleDishMS(SingleDishMS const &other)
  : msname_(""), ms_(0)
{
  initialize();
  ms_ = new MeasurementSet(*other.ms_);
  if(other.mssel_) {
    mssel_ = new MeasurementSet(*other.mssel_);
  }
  msname_ = static_cast<string>(ms_->tableName());
}

SingleDishMS &SingleDishMS::operator=(SingleDishMS const &other)
{
  initialize();
  msname_ = "";
  if (ms_ && this != &other) {
    *ms_ = *(other.ms_);
  }
  if (mssel_ && this != &other && other.mssel_) {
    *mssel_ = *(other.mssel_);
  }
  msname_ = static_cast<string>(ms_->tableName());
  return *this;
}

SingleDishMS::~SingleDishMS()
{
  LogIO os(_ORIGIN);
  if (ms_) {
    os << "Closing Measurement set " << name() << LogIO::POST;
    ms_->relinquishAutoLocks();
    ms_->unlock();
    delete ms_;
  }
  ms_ = 0;
  if (mssel_) {
    mssel_->relinquishAutoLocks();
    mssel_->unlock();
    delete mssel_;
  }
  mssel_ = 0;
  msname_ = "";
}

void SingleDishMS::initialize()
{
  in_column_ = MS::UNDEFINED_COLUMN;
  out_column_ = MS::UNDEFINED_COLUMN;
}

bool SingleDishMS::close()
{
  LogIO os(_ORIGIN);
  os << "Closing MeasurementSet and detaching from SingleDishMS"
     << LogIO::POST;

  ms_->unlock();
  if(mssel_) {
    mssel_->unlock();
    delete mssel_;
    mssel_ = 0;
  }
  if(ms_) delete ms_; ms_ = 0;
  msname_ = "";

  return True;
}

////////////////////////////////////////////////////////////////////////
///// Common utility functions
////////////////////////////////////////////////////////////////////////
void SingleDishMS::check_ms()
{
  AlwaysAssert(ms_, AipsError);
}

void SingleDishMS::reset_selection()
{
  if (mssel_) {
    delete mssel_;
    mssel_=0;
  };
}

void SingleDishMS::set_selection(Record const &selection, bool const verbose)
{
  LogIO os(_ORIGIN);
  check_ms();
  reset_selection();
  selection_ = selection;

  //Parse selection
  bool any_selection(false);
  String timeExpr(""), antennaExpr(""), fieldExpr(""),
    spwExpr(""), uvDistExpr(""), taQLExpr(""), polnExpr(""),
    scanExpr(""), arrayExpr(""), intentExpr(""), obsExpr("");
  timeExpr = get_field_as_casa_string(selection,"timerange");
  antennaExpr = get_field_as_casa_string(selection,"baseline");
  fieldExpr = get_field_as_casa_string(selection,"field");
  spwExpr = get_field_as_casa_string(selection,"spw");
  uvDistExpr = get_field_as_casa_string(selection,"uvdist");
  taQLExpr = get_field_as_casa_string(selection,"taql");
  polnExpr = get_field_as_casa_string(selection,"correlation");
  scanExpr = get_field_as_casa_string(selection,"scan");
  arrayExpr = get_field_as_casa_string(selection,"array");
  intentExpr = get_field_as_casa_string(selection,"intent");
  obsExpr = get_field_as_casa_string(selection,"observation");
  //Now the actual selection.
  mssel_ = new MeasurementSet(*ms_);
  if (!mssSetData(*ms_,*mssel_,"",timeExpr,antennaExpr,fieldExpr,
		  spwExpr,uvDistExpr,taQLExpr,polnExpr,scanExpr,
		  arrayExpr,intentExpr,obsExpr)) { // no valid selection
    reset_selection();
    os << "Selection is reset." << LogIO::POST;
  } else if (verbose) {
    // selection Summary
    os << "[Selection Summary]" << LogIO::POST;
    if (obsExpr != "")
      {any_selection = true; os << "- Observation: " << obsExpr << LogIO::POST;}
    if (antennaExpr != "")
      {any_selection = true; os << "- Antenna: " << antennaExpr << LogIO::POST;}
    if (fieldExpr != "")
      {any_selection = true; os << "- Field: " << fieldExpr << LogIO::POST;}
    if (spwExpr != "")
      {any_selection = true; os << "- SPW: " << spwExpr << LogIO::POST;}
    if (polnExpr != "")
      {any_selection = true; os << "- Pol: " << polnExpr << LogIO::POST;}
    if (scanExpr != "")
      {any_selection = true; os << "- Scan: " << scanExpr << LogIO::POST;}
    if (timeExpr != "")
      {any_selection = true; os << "- Time: " << timeExpr << LogIO::POST;}
    if (intentExpr != "")
      {any_selection = true; os << "- Intent: " << intentExpr << LogIO::POST;}
    if (arrayExpr != "")
      {any_selection = true; os << "- Array: " << arrayExpr << LogIO::POST;}
    if (uvDistExpr != "")
      {any_selection = true; os << "- UVDist: " << uvDistExpr << LogIO::POST;}
    if (taQLExpr != "")
      {any_selection = true; os << "- TaQL: " << taQLExpr << LogIO::POST;}
    if (!any_selection)
      os << "RESET selection" << LogIO::POST;
  }
  if (mssel_!=0)
    os << "Selected nrows = " << mssel_->nrow()
       << " from " << ms_->nrow() << "rows" << LogIO::POST;
}

String SingleDishMS::get_field_as_casa_string(Record const &in_data,
					      string const &field_name)
{
  Int ifield;
  ifield = in_data.fieldNumber(String(field_name));
  if (ifield>-1) return in_data.asString(ifield);
  return "";
}

void SingleDishMS::prepare_for_process(string const& in_column_name,
				       string const& out_ms_name)
{
  LogIO os(_ORIGIN);
  // make sure MS is set
  check_ms();
  // define a column to read data from
  if (in_column_name == "float_data"){
    if (!set_column(MS::FLOAT_DATA, in_column_))
      throw(AipsError("Input MS does not have FLOAT_DATA column"));
    os << "Reading data from FLOAT_DATA column" << LogIO::POST;
  } else if (in_column_name == "corrected_data") {
    if (!set_column(MS::CORRECTED_DATA, in_column_))
      throw(AipsError("Input MS does not have CORRECTED_DATA column"));
    os << "Reading data from CORRECTED_DATA column" << LogIO::POST;
  } else if (in_column_name == "data") {
    if (!set_column(MS::DATA, in_column_))
      throw(AipsError("Input MS does not have DATA column"));
    os << "Reading data from DATA column" << LogIO::POST;
  } else if (in_column_name != "") {
    throw(AipsError("Invalid data column name"));
  }
  if (set_column(MS::FLOAT_DATA, in_column_)) {
    os << "Reading data from FLOAT_DATA column" << LogIO::POST;
  } else if (set_column(MS::DATA, in_column_)) {
    os << "Reading data from DATA column" << LogIO::POST;
  } else {
    throw(AipsError("Unable to find input data column in input MS"));
  }
  // define a column to save data to
  if (out_ms_name != "") { // Creating a new MS.
    if (in_column_ == MS::CORRECTED_DATA)
      out_column_ = MS::DATA;
    else // DATA or FLOAT_DATA
      out_column_ = in_column_;
    os << "Output is stored in a new MS" << LogIO::POST;
  } else { // Storing results to input MS
    out_column_ = MS::CORRECTED_DATA;
    os << "Output data to CORRECTED_DATA column" << LogIO::POST;
  }
  //  create output CORRECTED_DATA if not exists.
  if (out_column_==MS::CORRECTED_DATA &&
      !ms_->tableDesc().isColumn("CORRECTED_DATA")) {
    bool redo_selection(false);
    if (mssel_) {
      redo_selection = true;
      reset_selection();
    }
    Bool addModel(False), addScratch(True), alsoinit(True), compress(False);
    VisSetUtil::addScrCols(*ms_,addModel,addScratch,alsoinit,compress);

    if (redo_selection) set_selection(selection_,false);
  }
  // handle no selection
  if (mssel_==0)
    mssel_ = new MeasurementSet(*ms_);
}

bool SingleDishMS::set_column(MSMainEnums::PredefinedColumns const &in,
			      MSMainEnums::PredefinedColumns &out)
{
  if (ms_!=0 && ms_->tableDesc().isColumn(MS::columnName(in))) {
    out = in;
    return true;
  } else {
    out = MS::UNDEFINED_COLUMN;
    return false;
  }
}

void SingleDishMS::get_data_cube_float(vi::VisBuffer2 const &vb,
				       Cube<Float> &data_cube)
{
  if (in_column_==MS::FLOAT_DATA) {
    data_cube = vb.visCubeFloat();
  } else { //need to convert Complex cube to Float
    Cube<Complex> cdata_cube(data_cube.shape());
    if (in_column_==MS::DATA) {
      cdata_cube = vb.visCube();
    } else {//MS::CORRECTED_DATA
      cdata_cube = vb.visCubeCorrected();
    }
    // convert Complext to Float
    convertArrayC2F(data_cube, cdata_cube);
  }
}

void SingleDishMS::convertArrayC2F(Array<Float> &to,
				   Array<Complex> const &from)
{
    if (to.nelements() == 0 && from.nelements() == 0) {
        return;
    }
    if (to.shape() != from.shape()) {
        throw(ArrayConformanceError("Array shape differs"));
    }
    Array<Complex>::const_iterator endFrom = from.end();
    Array<Complex>::const_iterator iterFrom = from.begin();
    for (Array<Float>::iterator iterTo = to.begin();
	 iterFrom != endFrom;
	 ++iterFrom, ++iterTo) {
      *iterTo = iterFrom->real();
    }
}

std::vector<string> SingleDishMS::split_string(string const &s, 
					       char delim)
{
  std::vector<string> elems;
  string item;
  for (size_t i = 0; i < s.size(); ++i) {
    char ch = s.at(i);
    if (ch == delim) {
      if (!item.empty()) {
	elems.push_back(item);
      }
      item.clear();
    } else {
      item += ch;
    }
  }
  if (!item.empty()) {
    elems.push_back(item);
  }
  return elems;
}

void SingleDishMS::parse_spwch(string const &spwch, 
			       Vector<Int> &spw, 
			       Vector<size_t> &nchan, 
			       Vector<Vector<Bool> > &mask)
{
  std::vector<string> elems = split_string(spwch, ',');
  size_t length = elems.size();
  spw.resize(length);
  nchan.resize(length);
  mask.resize(length);
  Vector<Vector<size_t> > edge(length);

  for (size_t i = 0; i < length; ++i) {
    std::vector<string> elems_spw = split_string(elems[i], ':');
    std::istringstream iss0(elems_spw[0]);
    iss0 >> spw[i];
    std::istringstream iss1(elems_spw[1]);
    iss1 >> nchan[i];
    std::istringstream iss2(elems_spw[2]);
    string edges;
    iss2 >> edges;
    std::vector<string> elems_edge = split_string(edges, ';');
    size_t length_edge = elems_edge.size();
    edge[i].resize(length_edge);
    for (size_t j = 0; j < length_edge; ++j) {
      std::istringstream iss(elems_edge[j]);
      iss >> edge[i][j];
    }
    mask[i].resize(nchan[i]);
    for (size_t j = 0; j < nchan[i]; ++j) {
      mask[i][j] = False;
    }
    for (size_t j = 0; j < length_edge; j+=2) {
      for (size_t k = edge[i][j]; k <= edge[i][j+1]; ++k) {
	mask[i][k] = True;
      }
    }
  }
}

void SingleDishMS::create_baseline_contexts(LIBSAKURA_SYMBOL(BaselineType) const baseline_type, 
		 	 		    uint16_t order, 
					    Vector<size_t> const &nchan, 
					    Vector<size_t> ctx_indices, 
					    Vector<LIBSAKURA_SYMBOL(BaselineContext) *> &bl_contexts)
{
  std::vector<size_t> uniq_nchan;
  uniq_nchan.clear();
  ctx_indices.resize(nchan.nelements());
  for (size_t i = 0; i < nchan.nelements(); ++i) {
    size_t idx = 0;
    bool found = false;
    for (size_t j = 0; j < uniq_nchan.size(); ++j) {
      if (uniq_nchan[j] == nchan[i]) {
	idx = j;
	found = true;
	break;
      }
    }
    if (found) {
      ctx_indices[i] = idx;
    } else {
      uniq_nchan.push_back(nchan[i]);
      ctx_indices[i] = uniq_nchan.size() - 1;
    }
  }

  bl_contexts.resize(uniq_nchan.size());
  LIBSAKURA_SYMBOL(Status) status; 
  for (size_t i = 0; i < uniq_nchan.size(); ++i) {
    status = LIBSAKURA_SYMBOL(CreateBaselineContext)(baseline_type, 
						     static_cast<uint16_t>(order), 
						     uniq_nchan[i], 
						     &bl_contexts[i]);
    if (status != LIBSAKURA_SYMBOL(Status_kOK)) {
      std::cout << "   -- error occured in CreateBaselineContext()." << std::flush;
    }
  }
}

void SingleDishMS::destroy_baseline_contexts(Vector<LIBSAKURA_SYMBOL(BaselineContext) *> &bl_contexts)
{
  LIBSAKURA_SYMBOL(Status) status;
  for (size_t i = 0; i < bl_contexts.nelements(); ++i) {
    status = LIBSAKURA_SYMBOL(DestroyBaselineContext)(bl_contexts[i]);
    if (status != LIBSAKURA_SYMBOL(Status_kOK)) {
      std::cout << "   -- error occured in DestroyBaselineContext()." << std::flush;
    }
  }
}

void SingleDishMS::set_data_cube_float(vi::VisBuffer2 &vb,
				       Cube<Float> const &data_cube)
{
  if (out_column_==MS::FLOAT_DATA) {

    vb.setVisCubeFloat(data_cube);
  } else { //need to convert Float cube to Complex
    Cube<Complex> cdata_cube(data_cube.shape());
    convertArray(cdata_cube,data_cube);
    if (out_column_==MS::DATA) {
      vb.setVisCube(cdata_cube);
    } else {//MS::CORRECTED_DATA
      if (!ms_->tableDesc().isColumn("CORRECTED_DATA"))
	throw(AipsError("CORRECTED_DATA column unexpectedly absent. Cannot correct."));
      vb.setVisCubeCorrected(cdata_cube);
    }
  }
}

void SingleDishMS::get_spectrum_from_cube(Cube<Float> &data_cube,
					  size_t const row,
					  size_t const plane,
					  size_t const num_data,
					  float *out_data)
{
  for (size_t i=0; i < num_data; ++i) 
    out_data[i] = static_cast<float>(data_cube(plane, i, row));
}

void SingleDishMS::get_spectrum_from_cube(Cube<Float> &data_cube,
					  size_t const row,
					  size_t const plane,
					  size_t const num_data,
					  SakuraAlignedArray<float> &out_data)
{
  float *ptr = out_data.data;
  for (size_t i=0; i < num_data; ++i) 
    ptr[i] = static_cast<float>(data_cube(plane, i, row));
}

void SingleDishMS::set_spectrum_to_cube(Cube<Float> &data_cube,
					  size_t const row,
					  size_t const plane,
					  size_t const num_data,
					  float *in_data)
{
  for (size_t i=0; i < num_data; ++i) 
    data_cube(plane, i, row) = static_cast<Float>(in_data[i]);
}

void SingleDishMS::get_flag_cube(vi::VisBuffer2 const &vb,
				 Cube<Bool> &flag_cube)
{
  flag_cube = vb.flagCube();
}

void SingleDishMS::get_flag_from_cube(Cube<Bool> &flag_cube,
					  size_t const row,
					  size_t const plane,
					  size_t const num_flag,
					  SakuraAlignedArray<bool> &out_flag)
{
  bool *ptr = out_flag.data;
  for (size_t i=0; i < num_flag; ++i) 
    ptr[i] = static_cast<bool>(flag_cube(plane, i, row));
}

////////////////////////////////////////////////////////////////////////
///// Atcual processing functions
////////////////////////////////////////////////////////////////////////

void SingleDishMS::subtract_baseline(Vector<Bool> const &in_mask,
				     int const order, 
				     float const clip_threshold_sigma, 
				     int const num_fitting_max)
{
  LogIO os(_ORIGIN);
  os << "Fitting and subtracting polynomial baseline order = " << order << LogIO::POST;
  // in_ms = out_ms
  // in_column = [FLOAT_DATA|DATA] (auto-select), out_column=CORRECTED_DATA
  // no iteration is necessary for the processing.
  // procedure
  // 1. iterate over MS
  // 2. pick single spectrum from in_column (this is not actually necessary for simple scaling but for exibision purpose)
  // 3. fit a polynomial to each spectrum and subtract it
  // 4. put single spectrum (or a block of spectra) to out_column

  double tstart = gettimeofday_sec();

  prepare_for_process();
  Block<Int> columns(1);
  columns[0] = MS::DATA_DESC_ID;
  vi::SortColumns sc(columns,False);
  vi::VisibilityIterator2 vi(*mssel_,sc,True);
  vi::VisBuffer2 *vb = vi.getVisBuffer();
  LIBSAKURA_SYMBOL(Status) status;
  LIBSAKURA_SYMBOL(BaselineStatus) bl_status;

  for (vi.originChunks(); vi.moreChunks(); vi.nextChunk()) {
    for (vi.origin(); vi.more(); vi.next()) {
      size_t const num_chan = static_cast<size_t>(vb->nChannels());
      size_t const num_pol = static_cast<size_t>(vb->nCorrelations());
      size_t const num_row = static_cast<size_t>(vb->nRows());
      Cube<Float> data_chunk(num_pol,num_chan,num_row);
      Matrix<Float> data_row(num_pol,num_chan);
      SakuraAlignedArray<float> spec(num_chan);
      Cube<Bool> flag_chunk(num_pol,num_chan,num_row);
      Matrix<Bool> flag_row(num_pol,num_chan);
      SakuraAlignedArray<bool> mask(num_chan);
      // set the given channel mask into aligned mask
      /*
      for (size_t ichan=0; ichan < num_chan; ++ichan) {
	//mask.data[ichan] = true;
	mask.data[ichan] = in_mask[ichan];
      }
      */
      // create baseline context
      LIBSAKURA_SYMBOL(BaselineContext) *bl_context;
      status = 
	LIBSAKURA_SYMBOL(CreateBaselineContext)(LIBSAKURA_SYMBOL(BaselineType_kPolynomial), 
						static_cast<uint16_t>(order), 
						num_chan, 
						&bl_context);
      if (status != LIBSAKURA_SYMBOL(Status_kOK)) {
	std::cout << "   -- error occured in CreateBaselineContext()." << std::flush;
      }
      // get a data cube (npol*nchan*nrow) from VisBuffer
      get_data_cube_float(*vb, data_chunk);
      // get a flag cube (npol*nchan*nrow) from VisBuffer
      get_flag_cube(*vb, flag_chunk);
      // loop over MS rows
      for (size_t irow=0; irow < num_row; ++irow) {
	// loop over polarization
	for (size_t ipol=0; ipol < num_pol; ++ipol) {
	  // get a spectrum from data cube
	  get_spectrum_from_cube(data_chunk, irow, ipol, num_chan, spec);
	  // get a channel mask from data cube
	  // (note that mask used here is actually a flag)
	  get_flag_from_cube(flag_chunk, irow, ipol, num_chan, mask);
	  // convert flag to mask by taking logical NOT of flag
	  // and then operate logical AND with in_mask
	  for (size_t ichan=0; ichan < num_chan; ++ichan) {
	    mask.data[ichan] = in_mask[ichan] && (!(mask.data[ichan]));
	  }
	  // actual execution of single spectrum
	  status = 
	    LIBSAKURA_SYMBOL(SubtractBaselineFloat)(num_chan, spec.data, mask.data, bl_context, 
						    static_cast<uint16_t>(order), clip_threshold_sigma, num_fitting_max,
						    true, mask.data, spec.data, &bl_status);
	  if (status != LIBSAKURA_SYMBOL(Status_kOK)) {
	    //raise exception?
	    std::cout << "   -- error occured in SubtractBaselineFloat()." << std::flush;
	  }
	  // set back a spectrum to data cube
	  set_spectrum_to_cube(data_chunk, irow, ipol, num_chan, spec.data);
	} // end of polarization loop
      } // end of MS row loop
      // write back data cube to VisBuffer
      set_data_cube_float(*vb, data_chunk);
      vb->writeChangesBack();
      // destroy baseline context
      status =
        LIBSAKURA_SYMBOL(DestroyBaselineContext)(bl_context);
      if (status != LIBSAKURA_SYMBOL(Status_kOK)) {
	//raise exception?
	std::cout << "   -- error occured in DestroyBaselineContext()." << std::flush;
      }
    } // end of vi loop
  } // end of chunk loop

  double tend = gettimeofday_sec();
  std::cout << "Elapsed time = " << (tend - tstart) << " sec." << std::endl;
}

void SingleDishMS::subtract_baseline_new(string const &spwch,
				     int const order, 
				     float const clip_threshold_sigma, 
				     int const num_fitting_max)
{
  LogIO os(_ORIGIN);
  os << "Fitting and subtracting polynomial baseline order = " << order << LogIO::POST;
  // in_ms = out_ms
  // in_column = [FLOAT_DATA|DATA] (auto-select), out_column=CORRECTED_DATA
  // no iteration is necessary for the processing.
  // procedure
  // 1. iterate over MS
  // 2. pick single spectrum from in_column (this is not actually necessary for simple scaling but for exibision purpose)
  // 3. fit a polynomial to each spectrum and subtract it
  // 4. put single spectrum (or a block of spectra) to out_column

  double tstart = gettimeofday_sec();

  prepare_for_process();
  Block<Int> columns(1);
  columns[0] = MS::DATA_DESC_ID;
  vi::SortColumns sc(columns,False);
  vi::VisibilityIterator2 vi(*mssel_,sc,True);
  vi::VisBuffer2 *vb = vi.getVisBuffer();
  LIBSAKURA_SYMBOL(Status) status;
  LIBSAKURA_SYMBOL(BaselineStatus) bl_status;

  Vector<Int> spw;
  Vector<size_t> nchan;
  Vector<Vector<Bool> > in_mask;
  parse_spwch(spwch, spw, nchan, in_mask);
  Vector<size_t> ctx_indices;
  Vector<LIBSAKURA_SYMBOL(BaselineContext) *> bl_contexts;
  create_baseline_contexts(LIBSAKURA_SYMBOL(BaselineType_kPolynomial), 
			   static_cast<uint16_t>(order), 
			   nchan, ctx_indices, bl_contexts);

  for (vi.originChunks(); vi.moreChunks(); vi.nextChunk()) {
    for (vi.origin(); vi.more(); vi.next()) {
      Vector<Int> data_spw = vb->spectralWindows();
      size_t const num_chan = static_cast<size_t>(vb->nChannels());
      size_t const num_pol = static_cast<size_t>(vb->nCorrelations());
      size_t const num_row = static_cast<size_t>(vb->nRows());
      Cube<Float> data_chunk(num_pol,num_chan,num_row);
      SakuraAlignedArray<float> spec(num_chan);
      Cube<Bool> flag_chunk(num_pol,num_chan,num_row);
      SakuraAlignedArray<bool> mask(num_chan);

      // get a data cube (npol*nchan*nrow) from VisBuffer
      get_data_cube_float(*vb, data_chunk);
      // get a flag cube (npol*nchan*nrow) from VisBuffer
      get_flag_cube(*vb, flag_chunk);
      // loop over MS rows
      for (size_t irow=0; irow < num_row; ++irow) {
	size_t idx = 0;
	for (size_t ispw=0; ispw < spw.nelements(); ++ispw) {
	  if (data_spw[irow] == spw[ispw]) {
	    idx = ispw;
	    break;
	  }
	}
	assert(num_chan == nchan[idx]);

	// loop over polarization
	for (size_t ipol=0; ipol < num_pol; ++ipol) {
	  // get a spectrum from data cube
	  get_spectrum_from_cube(data_chunk, irow, ipol, num_chan, spec);
	  // get a channel mask from data cube
	  // (note that mask used here is actually a flag)
	  get_flag_from_cube(flag_chunk, irow, ipol, num_chan, mask);
	  // convert flag to mask by taking logical NOT of flag
	  // and then operate logical AND with in_mask
	  for (size_t ichan=0; ichan < num_chan; ++ichan) {
	    mask.data[ichan] = in_mask[idx][ichan] && (!(mask.data[ichan]));
	  }
	  // actual execution of single spectrum
	  status = 
	    LIBSAKURA_SYMBOL(SubtractBaselineFloat)(num_chan, 
						    spec.data, 
						    mask.data, 
						    bl_contexts[ctx_indices[idx]], 
						    static_cast<uint16_t>(order), 
						    clip_threshold_sigma, 
						    num_fitting_max,
						    true, 
						    mask.data, 
						    spec.data, 
						    &bl_status);
	  if (status != LIBSAKURA_SYMBOL(Status_kOK)) {
	    //raise exception?
	    std::cout << "   -- error occured in SubtractBaselineFloat()." << std::flush;
	  }
	  // set back a spectrum to data cube
	  set_spectrum_to_cube(data_chunk, irow, ipol, num_chan, spec.data);
	} // end of polarization loop
      } // end of MS row loop
      // write back data cube to VisBuffer
      set_data_cube_float(*vb, data_chunk);
      vb->writeChangesBack();
    } // end of vi loop
  } // end of chunk loop

  // destroy baselint contexts
  destroy_baseline_contexts(bl_contexts);

  double tend = gettimeofday_sec();
  std::cout << "Elapsed time = " << (tend - tstart) << " sec." << std::endl;
}

void SingleDishMS::scale(float const factor)
{
  LogIO os(_ORIGIN);
  os << "Multiplying scaling factor = " << factor << LogIO::POST;
  // in_ms = out_ms
  // in_column = [FLOAT_DATA|DATA] (auto-select), out_column=CORRECTED_DATA
  // no iteration is necessary for the processing.
  // procedure
  // 1. iterate over MS
  // 2. pick single spectrum from in_column (this is not actually necessary for simple scaling but for exibision purpose)
  // 3. multiply a scaling factor to each spectrum
  // 4. put single spectrum (or a block of spectra) to out_column
  prepare_for_process();
  Block<Int> columns(1);
  columns[0] = MS::DATA_DESC_ID;
  vi::SortColumns sc(columns,False);
  vi::VisibilityIterator2 vi(*mssel_,sc,True);
  vi::VisBuffer2 *vb = vi.getVisBuffer();

  for (vi.originChunks(); vi.moreChunks(); vi.nextChunk()) {
    for (vi.origin(); vi.more(); vi.next()) {
      size_t const num_chan = static_cast<size_t>(vb->nChannels());
      size_t const num_pol = static_cast<size_t>(vb->nCorrelations());
      size_t const num_row = static_cast<size_t>(vb->nRows());
      Cube<Float> data_chunk(num_pol,num_chan,num_row);
      Matrix<Float> data_row(num_pol,num_chan);
      float spectrum[num_chan];
      // get a data cube (npol*nchan*nrow) from VisBuffer
      get_data_cube_float(*vb, data_chunk);
      // loop over MS rows
      for (size_t irow=0; irow < num_row; ++irow) {
	// loop over polarization
	for (size_t ipol=0; ipol < num_pol; ++ipol) {
	  // get a spectrum from data cube
	  get_spectrum_from_cube(data_chunk, irow, ipol, num_chan, spectrum);

	  // actual execution of single spectrum
	  do_scale(factor, num_chan, spectrum);

	  // set back a spectrum to data cube
	  set_spectrum_to_cube(data_chunk, irow, ipol, num_chan, spectrum);
	} // end of polarization loop
      } // end of MS row loop
      // write back data cube to VisBuffer
      set_data_cube_float(*vb, data_chunk);
      vb->writeChangesBack();
    } // end of vi loop
  } // end of chunk loop
}

void SingleDishMS::do_scale(float const factor,
			    size_t const num_data, float *data)
{
  for (size_t i=0; i < num_data; ++i) 
    data[i] *= factor;
}

}  // End of casa namespace.
