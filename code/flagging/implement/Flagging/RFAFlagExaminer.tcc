//# RFAFlagExaminer.cc: this defines RFAFlagExaminer
//# Copyright (C) 2000,2001,2002
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
//# $Id$
#include <casa/Exceptions/Error.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/ArrayLogical.h>
#include <casa/Arrays/MaskedArray.h>
#include <casa/Arrays/MaskArrMath.h>
#include <casa/Quanta/Quantum.h>
#include <casa/Quanta/MVTime.h>
#include <casa/Logging/LogIO.h>
#include <msvis/MSVis/VisibilityIterator.h>
#include <msvis/MSVis/VisBuffer.h>
#include <flagging/Flagging/RFAFlagExaminer.h>
#include <casa/stdio.h>

namespace casa { //# NAMESPACE CASA - BEGIN
  
    Bool dbg3 = False;
  
  // -----------------------------------------------------------------------
  // RFAFlagExaminer constructor
  // -----------------------------------------------------------------------
  RFAFlagExaminer::RFAFlagExaminer ( RFChunkStats &ch,const RecordInterface &parm ) : 
    RFASelector(ch, parm)//,RFDataMapper(parm.asArrayString(RF_EXPR),parm.asString(RF_COLUMN))
  {
    if(dbg3)  cout << __FILE__ << ":" << __func__ << "():" << __LINE__ << endl;
    //desc_str = String("flagexaminer");
    if(dbg3) cout<<"FlagExaminer constructor "<<endl;
    totalflags    = accumTotalFlags    = 0;
    totalcount    = accumTotalCount    = 0;
    totalrowflags = accumTotalRowFlags = 0;
    totalrowcount = accumTotalRowCount = 0;
    //parseParm(parm);

    os = LogIO(LogOrigin("RFAFlagExaminer", "RFAFlagExaminer", WHERE));
  }
  
  
  RFAFlagExaminer::~RFAFlagExaminer ()
  {
    if(dbg3)  cout << "FlgaExaminer destructor " << endl;
    //     char s[1024];
    //     sprintf(s,"Chunk %d (field %s, spw %d)",
    // 	    chunk.nchunk(),chunk.visIter().fieldName().chars(),chunk.visIter().spectralWindow());
    //     os << "---------------------------------------------------------------------" << LogIO::POST;
    //     os<<s<<LogIO::POST;
    
    //     sprintf(s,"%s, %d channels, %d time slots, %d baselines, %d rows\n",
    // 	    chunk.getCorrString().chars(),chunk.num(CHAN),chunk.num(TIME),
    // 	    chunk.num(IFR),chunk.num(ROW));
    //     os<<s<<LogIO::POST;
    
    //     os << "\n\n\nData Selection to examine : " << desc_str ;
    //     if(flag_everything) os << " all " ;
    //     os << LogIO::POST;
    
  }
  
  void RFAFlagExaminer::initialize()
  {
    if(dbg3)  cout << __FILE__ << ":" << __func__ << "():" << __LINE__ << endl;

    totalflags    = accumTotalFlags    = 0;
    totalcount    = accumTotalCount    = 0;
    totalrowflags = accumTotalRowFlags = 0;
    totalrowcount = accumTotalRowCount = 0;
    inTotalFlags =
	inTotalCount = 
	inTotalRowCount = 
	outTotalFlags =
	outTotalCount = 
	outTotalRowCount = 
	outTotalRowFlags = 0;
  }

  void RFAFlagExaminer::finalize()
  {
    if(dbg3)  cout << __FILE__ << ":" << __func__ << "():" << __LINE__ << endl;

    return;
    // because the following seems to be old code...

    Double ffrac=0.0,rffrac=0.0;
    if(accumTotalCount) ffrac = accumTotalFlags*100.0/accumTotalCount;
    if(accumTotalRowCount) rffrac = accumTotalRowFlags*100.0/accumTotalRowCount;
    
    os << "Field = " << chunk.visBuf().fieldId() << " , Spw Id : " << chunk.visBuf().spectralWindow() << endl;
    //     os << accumTotalRowFlags << " out of " << accumTotalRowCount << " (" << rffrac << "%) rows are flagged." 
    //        << endl;
    os << inTotalRowFlags << " out of " << inTotalRowCount << " (" << rffrac << "%) rows are flagged." 
       << endl;
    //    os << accumTotalFlags << " out of " << accumTotalCount << " (" << ffrac << "%) data points are flagged." << endl;
    //     os << inTotalFlags << " " << inTotalCount << " " << inTotalRowFlags << " " << inTotalRowCount << endl;
    //     os << outTotalFlags << " " << outTotalCount << " " << outTotalRowFlags << " " << outTotalRowCount << endl;

    os << inTotalFlags << " out of " << inTotalCount << " (" << ffrac << "%) data points are flagged." << endl;
    os << "Fraction of data flagged in this run = " << (outTotalFlags-inTotalFlags)*100.0/inTotalFlags << "%"
       << " ("<<outTotalFlags-inTotalFlags<< " new flags)." << endl;
    os << "Fraction of rows flagged in this run = " 
       << (outTotalRowFlags-inTotalRowFlags)*100.0/inTotalRowFlags<<"%" 
       << " ("<<outTotalRowFlags-inTotalRowFlags<< " new row-flags).";

    os << LogIO::POST;
  }
  // -----------------------------------------------------------------------
  // processRow
  // Raises/clears flags for a single row, depending on current selection
  // -----------------------------------------------------------------------
  void RFAFlagExaminer::processRow(uInt ifr, uInt it)
  {
      // called often... if(dbg3)  cout << __FILE__ << ":" << __func__ << "():" << __LINE__ << endl;
      
      return;
  }
  
  
  // -----------------------------------------------------------------------
  // startFlag
  // -----------------------------------------------------------------------
  void RFAFlagExaminer::startFlag ()
  {
    if(dbg3)  cout << __FILE__ << ":" << __func__ << "():" << __LINE__ << endl;

    totalflags    = accumTotalFlags    = 0;
    totalcount    = accumTotalCount   = 0;
    totalrowflags = accumTotalRowFlags = 0;
    totalrowcount = accumTotalRowCount = 0;
    
    inTotalFlags = 
	inTotalCount = 
	inTotalRowCount =
	outTotalFlags = 
	outTotalCount =
	outTotalRowCount = 
	outTotalRowFlags = 0;

    RFAFlagCubeBase::startFlag();

    return;
  }
  
  void RFAFlagExaminer::initializeIter (uInt it) 
  {
      //    totalflags = totalcount = totalrowflags = totalrowcount = 0;
      if(dbg3)  cout << __FILE__ << ":" << __func__ << "():" << __LINE__ << endl;

      for(unsigned ii=0;
	  ii<chunk.visBuf().flagRow().nelements();
	  ii++)
	  if (chunk.visBuf().flagRow()(ii)) {
	      inTotalRowFlags++;
	  }
      
      inTotalRowCount += chunk.visBuf().flagRow().nelements();
      
      for(Int ii=0;
	  ii<chunk.visBuf().flag().shape()(0);
	  ii++)
	  for(Int jj=0;
	      jj<chunk.visBuf().flag().shape()(1);
	      jj++)
	      if (chunk.visBuf().flag()(ii,jj)) inTotalFlags++;
      
//     iterFlag(it);
//     inTotalFlags += totalflags;
//     inTotalCount += totalcount;
//     inTotalRowFlags += totalrowflags;
//     inTotalRowCount += totalrowcount;
  }
  
  void RFAFlagExaminer::finalizeIter (uInt it) 
  {
      if(dbg3)  cout << __FILE__ << ":" << __func__ << "():" << __LINE__ << endl;

    outTotalRowCount += chunk.visBuf().flagRow().nelements();

    for (unsigned ii = 0;
	 ii < chunk.visBuf().flagRow().nelements();
	 ii++)
	if (chunk.visBuf().flagRow()(ii)) {
	    outTotalRowFlags++;
	}

    for (Int ii=0;
	 ii<chunk.visBuf().flag().shape()(0);
	 ii++) {

	outTotalCount += chunk.visBuf().flag().shape()(1);
	
	for (Int jj=0;
	     jj<chunk.visBuf().flag().shape()(1);
	     jj++) {
	    if (chunk.visBuf().flag()(ii,jj)) outTotalFlags++;
	}
    }

//     totalflags = totalcount = totalrowflags = totalrowcount = 0;
//     iterFlag(it, False);
//     outTotalFlags += totalflags;
//     outTotalCount += totalcount;
//     outTotalRowFlags += totalrowflags;
//     outTotalRowCount += totalrowcount;
    //    cerr << outTotalRowCount << " " << inTotalRowCount << " " << outTotalRowCount-inTotalRowCount << endl;
  }
  
  // -----------------------------------------------------------------------
  // iterFlag
  // -----------------------------------------------------------------------
  void RFAFlagExaminer::iterFlag(uInt it)
      //Bool resetFlags
  {
    if(dbg3)  cout << __FILE__ << ":" << __func__ << "():" << __LINE__ << endl;

    // Set the flags and count them up.
    //    if (resetFlags) 
    RFAFlagCubeBase::iterFlag(it);
    
    // count if within specific timeslots
    const Vector<Double> &times( chunk.visBuf().time() );
    Double t0 = times(it);
    
    Bool flagall = True;
    
    if (sel_time.ncolumn()) {

	if( anyEQ(sel_timerng.row(0) <= t0 && 
		  sel_timerng.row(1) >= t0, True) )
	    flagall = True;
	else flagall = False;
    }
    
    if (flagall) {

	// More counting and fill up final display variables.
	const Vector<Int> &ifrs( chunk.ifrNums() );
	const Vector<Int> &feeds( chunk.feedNums() );
	const Vector<casa::RigidVector<casa::Double, 3> >&uvw( chunk.visBuf().uvw() );
	// Vector<Vector<Double> > &uvw=NULL;//( chunk.visIter.uvw(uvw) );
	//chunk.visIter().uvw(uvw);
	Double uvdist=0.0;
	
	// loop over rows
	for (uInt i=0; i < ifrs.nelements(); i++) {
	    Bool inrange=False;
	    
	    uvdist = sqrt( uvw(i)(0)*uvw(i)(0) + uvw(i)(1)*uvw(i)(1) );
	    
	    for( uInt j=0; j<sel_uvrange.ncolumn(); j++) {
		if( uvdist >= sel_uvrange(0, j) &&
		    uvdist <= sel_uvrange(1, j) ) 
		    
		    inrange |= True;
	    }

	    if( (!sel_ifr.nelements() || sel_ifr(ifrs(i))) && 
		(!sel_feed.nelements() || sel_feed(feeds(i))) &&
		(!sel_uvrange.nelements() || inrange ) )
	      {
		// Operate on the chosen row.
		// Collect counts.
		
		//cout << "selected row for " << ifrs(i) << "," << it << endl;
		
		  if(chunk.nfIfrTime(ifrs(i),it) == chunk.num(CORR)*chunk.num(CHAN))
		      totalrowflags++;
		  totalrowcount++;
		
		for( uInt ich=0; ich<chunk.num(CHAN); ich++ ) {
		    if(!flagchan.nelements() || flagchan[ich])
			{
			    totalflags += chunk.nfChanIfrTime(ich,ifrs(i),it);
			    totalcount += chunk.num(CORR);
			}
		}
	      }
          }
      }// end of flagall
    
    accumTotalFlags    += totalflags;
    accumTotalCount    += totalcount;
    accumTotalRowFlags += totalrowflags;
    accumTotalRowCount += totalrowcount;
    
    return;
  }
  // -----------------------------------------------------------------------
  // endFlag
  // -----------------------------------------------------------------------
  void RFAFlagExaminer::endFlag ()
  {
    if(dbg3)  cout << __FILE__ << ":" << __func__ << "():" << __LINE__ << endl;
    
    // os << "\nChunk : " << chunk.nchunk() << " : [ Field Id : " << chunk.visBuf().fieldId() << " , Spw Id : " << chunk.visBuf().spectralWindow() << " ]" << endl;
    // cout << chunk.getCorrString() << " , " << chunk.num(CHAN) << " channels, " << chunk.num(TIME) << " time slots, " << chunk.num(IFR) << "(" << chunk.num(ROW)/chunk.num(TIME) << ") baselines, " << chunk.num(ROW) << " rows" << LogIO::POST;
    
    char s[1024];
    sprintf(s,"Chunk %d (field %s, spw %d)",
     	    chunk.nchunk(),chunk.visIter().fieldName().chars(),chunk.visIter().spectralWindow());
    os << "---------------------------------------------------------------------" << LogIO::POST;
    os<<s<<LogIO::POST;
    
    sprintf(s, "%s, %d channel%s, %d time slots, %d baselines, %d rows\n",
     	    chunk.getCorrString().chars(),
	    chunk.num(CHAN),
	    chunk.num(CHAN) == 1 ? "" : "s",
	    chunk.num(TIME),
     	    chunk.num(IFR),
	    chunk.num(ROW));
    os << s << LogIO::POST;
    
    /*
      Int n,n0;
      char s[200];
      // % of rows flagged
      n  = sum(chunk.nrfIfr());
      n0 = chunk.num(ROW);
      sprintf(s,"\n%d (%0.2f%%) rows are flagged (all baselines/times/chans/corrs for [field=%d,spw=%d]).",n,n*100.0/n0,chunk.visBuf().fieldId(),chunk.visBuf().spectralWindow());
      os<<s<<LogIO::POST;
    
      // % of data points flagged
      n  = sum(chunk.nfIfrTime());
      n0 = chunk.num(ROW)*chunk.num(CHAN)*chunk.num(CORR);
      sprintf(s,"%d of %d (%0.2f%%) data points are flagged (all baselines/times/chans/corrs for [field=%d,spw=%d]).",n,n0,n*100.0/n0,chunk.visBuf().fieldId(),chunk.visBuf().spectralWindow());
      os<<s<<LogIO::POST;
    */
    
    os << "\n\n\nData Selection to examine : " << desc_str ;
    if(flag_everything) os << " all " ;
    os << LogIO::POST;
    
    Double ffrac=0.0,rffrac=0.0;
    if(totalcount) ffrac = totalflags*100.0/totalcount;
    if(totalrowcount) rffrac = totalrowflags*100.0/totalrowcount;

    os << totalrowflags << " out of " << totalrowcount <<
	" (" << rffrac << "%) rows are flagged." <<
	LogIO::POST;

    os << totalflags << " out of " << totalcount <<
	" (" << ffrac << "%) data points are flagged.\n\n" <<
	LogIO::POST;
    

    
    if (0) {
	rffrac = outTotalRowFlags * 100.0 / outTotalRowCount;
	ffrac = outTotalFlags * 100.0 / outTotalCount;
	os <<
	    outTotalRowFlags << " out of " <<
	    outTotalRowCount << " (" << rffrac << "%) rows are flagged." <<
	    LogIO::POST;
	os << outTotalFlags << " out of " << 
	    outTotalCount << " (" << ffrac << "%) data points are flagged.\n\n" << 
	    LogIO::POST;
    }
    os << "---------------------------------------------------------------------" << LogIO::POST;
    
    return;
  }
  
#if 0
  String RFAFlagExaminer::getDesc ()
  {
    return desc_str+" "+RFAFlagCubeBase::getDesc();
  }
  
  const RecordInterface & RFAFlagExaminer::getDefaults ()
  {
    static Record rec;
    // create record description on first entry
    if( !rec.nfields() )
      {
	rec = RFAFlagCubeBase::getDefaults();
	rec.removeField(RF_FIGNORE); // fignore is meaningless
	rec.define(RF_NAME,"flagexaminer"); 
	// TODO : use these selection params for "extend along"...
	rec.define(RF_SPWID,False); 
	rec.define(RF_FIELD,False);
	rec.define(RF_CHANS,False);
	rec.define(RF_CORR,False);
	rec.define(RF_BASELINE,False);
	rec.define(RF_TIMERANGE,False);
	rec.define(RF_TIMEDELTA,False);
	rec.define(RF_AUTOCORR,False);
	rec.define(RF_SCAN,False);
	rec.define(RF_ARRAY,False);
	rec.define(RF_FEED,False);
	rec.define(RF_UVRANGE,False);
	
	rec.setComment(RF_SPWID,"Restrict flagging to specific spectral windows (integers)");
	rec.setComment(RF_FIELD,"Restrict flagging to specific field IDs or field names (integers/strings)");
	rec.setComment(RF_CHANS,"Restrict flagging to specific channels (2,N array of integers)");
	rec.setComment(RF_CORR,"Restrict flagging to specific correlations (array of strings)");
	rec.setComment(RF_BASELINE,"Restrict flagging to specific baselines (array of strings, e.g., 'A1-A2','A1-*', or 2,N array of integers [[A1,A2],[B1,B2],...])");
	rec.setComment(RF_TIMERANGE,"Restrict flagging to specific time ranges (2,N array of strings or MJDs");
	rec.setComment(RF_TIMEDELTA,String("Time delta for ")+RF_CENTERTIME+", in seconds");
	rec.setComment(RF_AUTOCORR,"Flag autocorrelations (F/T)");
	rec.setComment(RF_SCAN,"Restrict flagging to specific scans (integers)");
	rec.setComment(RF_ARRAY,"Restrict flagging to specific array ids (integers)");
	rec.setComment(RF_FEED,"Restrict flagging to specific feeds (2,N array of integers)");
	rec.setComment(RF_UVRANGE,"Restrict flagging to specific uv-distance ranges in meters (2,N array of doubles )");
      }
    return rec;
  }
  
  // -----------------------------------------------------------------------
  // parseParm
  // -----------------------------------------------------------------------
  bool RFAFlagExaminer::parseParm (const RecordInterface &parm)
  {
    char s[200];
    // parse input arguments: channels
    if( parseRange(sel_chan,parm,RF_CHANS) )
      {
	String sch;
	for( uInt i=0; i<sel_chan.ncolumn(); i++ )
	  {
	    sprintf(s,"%d:%d",sel_chan(0,i),sel_chan(1,i));
	    addString(sch,s,",");
	  }
	addString(desc_str,String(RF_CHANS)+"="+sch);
	sel_chan(sel_chan>=0) += -(Int)indexingBase();
      }
    
    // parse input arguments: correlations
    if( fieldType(parm,RF_CORR,TpString,TpArrayString))
      {
	String ss;
	Vector<String> scorr( parm.asArrayString(RF_CORR) );
	sel_corr.resize( scorr.nelements() );
	for( uInt i=0; i<scorr.nelements(); i++ )
	  {
	    sel_corr(i) = Stokes::type( scorr(i) );
	    if( sel_corr(i) == Stokes::Undefined )
	      os<<"Illegal correlation "<<scorr(i)<<endl<<LogIO::EXCEPTION;
	    addString(ss,scorr(i),",");
	  }
	addString(desc_str,String(RF_CORR)+"="+ss);
      }
    
    return True;
  }
  
  
  // -----------------------------------------------------------------------
  // reformRange
  // Reforms an array of 2N elements into a [2,N] matrix
  // -----------------------------------------------------------------------
  template<class T> Bool RFAFlagExaminer::reformRange( Matrix<T> &rng,const Array<T> &arr )
  {
    if( arr.ndim()>2 || (arr.nelements()%2) !=0 )
      return False;
    rng = arr.reform(IPosition(2,2,arr.nelements()/2));
    return True;
  }
  
  template<class T> Array<T> fieldToArray( const RecordInterface &parm,const String &id );
  
  /*
    template<> Array<Int> fieldToArray<Int>( const RecordInterface &parm,const String &id )
    { return parm.toArrayInt(id); }
    template<> Array<Double> fieldToArray<Double>( const RecordInterface &parm,const String &id )
    { return parm.toArrayDouble(id); }
    template<> Array<String> fieldToArray<String>( const RecordInterface &parm,const String &id )
    { return parm.toArrayString(id); }
  */
  
  
  // -----------------------------------------------------------------------
  // RFAFlagExaminer::parseRange
  // Returns a record field of 2N elements as a [2,N] matrix
  // -----------------------------------------------------------------------
  template<class T> Bool RFAFlagExaminer::parseRange( Matrix<T> &rng,const RecordInterface &parm,const String &id )
  {
    if( isFieldSet(parm,id) )
      {
	try 
	  {
	    Array<T> arr( fieldToArray<T>(parm,id) );
	    if( !reformRange(rng,arr) )
	      throw( AipsError("") );
	    return True;
	  } 
	catch( AipsError x ) 
	  {
	    os<<"Illegal \""<<id<<"\" array\n"<<LogIO::EXCEPTION;
	  }
      }
    return False;
  }
  
  
  // -----------------------------------------------------------------------
  // addString
  // Helper method to build up description strings
  // -----------------------------------------------------------------------
  void RFAFlagExaminer::addString( String &str,const String &s1,const char *sep )
  {
    if( str.length() )
      str += sep;
    str += s1;
  }
  
#endif
  
} //# NAMESPACE CASA - END

