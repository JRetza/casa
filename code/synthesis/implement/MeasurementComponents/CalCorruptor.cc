//# CalCorruptor.cc: Corruptions for simulated data
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

#include <synthesis/MeasurementComponents/StandardVisCal.h>

#include <casa/Logging/LogMessage.h>
#include <casa/Logging/LogSink.h>

#define PRTLEV 3

namespace casa { //# NAMESPACE CASA - BEGIN


CalCorruptor::CalCorruptor(const Int nSim) : 
  nSim_(nSim),
  times_initialized_(False),
  prtlev_(PRTLEV),
  curr_slot_(-1),
  curr_spw_(-1), nSpw_(0),
  curr_ant_(-1), nAnt_(0) 
{}

CalCorruptor::~CalCorruptor() {}

Complex CalCorruptor::simPar(const VisIter& vi,VisCal::Type type, Int ipar){  
  // per par (e.g. for D,G, nPar=2, could have diff gains for diff polns)
  if (prtlev()>2) cout << "   Corruptor::simPar("<<VisCal::nameOfType(type)<<")" << endl;  

  if (type==VisCal::D) {
    return amp();
 
 } else {	
    // Every CalCorruptor needs to have its own checks - maybe put D here?
    throw(AipsError("This Corruptor doesn't yet support simulation of this VisCal type"));
  }
}

void CalCorruptor::setEvenSlots(const Double& dt) {
  // set slots to constant intervals dt
  Int nslots = Int( (stopTime()-startTime())/dt + 1 );
  if (nslots<=0) throw(AipsError("logic problem Corruptor::setEvenSlots called before start/stopTime set"));

  nSim()=nslots;
  slot_times().resize(nSim(),False);
  for (Int i=0;i<nSim();i++) 
    slot_time(i) = startTime() + (Double(i)+0.5) * dt;   
  times_initialized()=True;
  
  curr_slot()=0;      
  curr_time()=slot_time();  
}



Complex ANoiseCorruptor::simPar(const VisIter& vi, VisCal::Type type,Int ipar) {
  if (type==VisCal::ANoise) {
    return Complex((*nDist_p)()*amp(),(*nDist_p)()*amp());
  } else throw(AipsError("unknown VC type "+VisCal::nameOfType(type)+" in AnoiseCorruptor::simPar"));
}


ANoiseCorruptor::ANoiseCorruptor(): CalCorruptor(1) {};

ANoiseCorruptor::~ANoiseCorruptor() {};






// and also produce MfM and Topac corruptions


AtmosCorruptor::AtmosCorruptor() : 
  CalCorruptor(1),  // parent
  mean_pwv_(-1.)
{}

AtmosCorruptor::AtmosCorruptor(const Int nSim) : 
  CalCorruptor(nSim),  // parent
  mean_pwv_(-1.)
{}

AtmosCorruptor::~AtmosCorruptor() {
  if (prtlev()>2) cout << "AtmosCorruptor::~AtmosCorruptor()" << endl;
}



Vector<Float>* AtmosCorruptor::pwv() { 
  if (currAnt()<=pwv_p.nelements())
    return pwv_p[currAnt()];
  else
    return NULL;
};
 
Float& AtmosCorruptor::pwv(const Int islot) { 
  if (currAnt()<=pwv_p.nelements())
    return (*pwv_p[currAnt()])(islot);
  else
    throw(AipsError("AtmosCorruptor internal error accessing delay()"));;
};





Complex AtmosCorruptor::simPar(const VisIter& vi, VisCal::Type type,Int ipar){

  LogIO os(LogOrigin("AtmCorr", "simPar("+VisCal::nameOfType(type)+")", WHERE));
  if (prtlev()>4) cout << "  Atm::simPar("<<VisCal::nameOfType(type)<<") ipar=" <<ipar<< endl;
  
#ifndef RI_DEBUG
  try {
#endif
    if (type==VisCal::T) {

      if(mode()=="test" or mode()=="1d")
	return cphase(focusChan()); 
      
      else if (mode()=="2d") {
	// RI TODO Atmcorr:simPar  modify x,y by tan(zenith angle)*(layer altitude)
	Int ix(Int( antx()(currAnt()) ));
	Int iy(Int( anty()(currAnt()) ));
	if (prtlev()>5) 
	  cout << " getting gain for antenna ix,iy = " << ix << "," << iy << endl;  
	return cphase(ix,iy,focusChan()); // init all elts of gain vector with scalar 
      } else 
	throw(AipsError("AtmosCorruptor: unknown corruptor mode "+mode()));
    } else if (type==VisCal::M) {
      Float airmass1(1.0),airmass2(1.0),A(1.0),tsys(0.),tau(0.);
      Double factor(1.0);
      Float el1(1.5708), el2(1.5708);

      if (mode() == "tsys") {
	Double tint = vi.msColumns().exposure()(0);  
	Int iSpW = vi.spectralWindow();
	Double deltaNu = 
	  vi.msColumns().spectralWindow().totalBandwidth()(iSpW) / 
	  Float(vi.msColumns().spectralWindow().numChan()(iSpW));	    
	factor = amp() / sqrt( 2 * deltaNu * tint ) ;
	
	// RI verify accuracy of how refTime set in SVC
	Vector<MDirection> antazel(vi.azel(curr_time()));
	
	el1 = antazel(currAnt()).getAngle("rad").getValue()(1);
	el2 = antazel(currAnt2()).getAngle("rad").getValue()(1);
	
	if (el1 > 0.0 && el2 > 0.0) {
	  airmass1 = 1.0/ sin(el1);
	  airmass2 = 1.0/ sin(el2);
	} else {
	  airmass1 = 1.0;
	  airmass2 = 1.0;
	}
	
	if (freqDepPar()) {
	  // if tau0 is set, tauscale = tau0/tau(band center) else scale=1
	  tau = opac(focusChan());
	}	else {
	  // if tau0 is set, tauscale=tau0, else exception was thrown in init
	  tau = 1.;
	}
	// user could be overriding the tau scale, but keep the
	// ATM freq dependence - see atmcorruptor::initialize
	tau *= tauscale();
	
	A = exp(tau *0.5*(airmass1+airmass2));	      
	// this is tsys above atmosphere
	tsys = tsys0() + A*tsys1();
	
	return Complex( antDiams(currAnt())*antDiams(currAnt2()) /factor /tsys);

      } else return Complex( 1./amp() ); // for constant amp MfM
      
    } else {
      throw(AipsError("AtmosCorruptor: unknown VisCal type "+VisCal::nameOfType(type)));
    }
    
#ifndef RI_DEBUG    
  } catch (AipsError x) {
    os << LogIO::SEVERE << "Caught exception: " << x.getMesg() << LogIO::POST;
  } 
#endif
}


 
void AtmosCorruptor::initAtm() {

#ifndef CASA_STANDALONE
  atm::Temperature  T( 270.0,"K" );   // Ground temperature
  atm::Pressure     P( 560.0,"mb");   // Ground Pressure
  atm::Humidity     H(  20,"%" );     // Ground Relative Humidity (indication)
  atm::Length       Alt(  5000,"m" ); // Altitude of the site 
  atm::Length       WVL(   2.0,"km"); // Water vapor scale height

  // RI TODO get Alt etc from observatory info in Simulator

  double TLR = -5.6;     // Tropospheric lapse rate (must be in K/km)
  atm::Length  topAtm(  48.0,"km");   // Upper atm. boundary for calculations
  atm::Pressure Pstep(  10.0,"mb");   // Primary pressure step
  double PstepFact = 1.2; // Pressure step ratio between two consecutive layers
  atm::Atmospheretype atmType = atm::tropical;

  itsatm = new atm::AtmProfile(Alt, P, T, TLR, 
			       H, WVL, Pstep, PstepFact, 
			       topAtm, atmType);

  if (nSpw()<=0)
    throw(AipsError("AtmosCorruptor::initAtm called before spw setup."));

  // first SpW:
  double fRes(fWidth()[0]/fnChan()[0]);
  itsSpecGrid = new atm::SpectralGrid(fnChan()[0],0, 
				      atm::Frequency(fRefFreq()[0],"Hz"),
				      atm::Frequency(fRes,"Hz"));
  // any more?
  for (Int ispw=1;ispw<nSpw();ispw++) {
    fRes = fWidth()[ispw]/fnChan()[ispw];
    itsSpecGrid->add(fnChan()[ispw],0,
		     atm::Frequency(fRefFreq()[ispw],"Hz"),
		     atm::Frequency(fRes,"Hz"));
  }

  itsRIP = new atm::RefractiveIndexProfile(*itsSpecGrid,*itsatm);
  
  if (prtlev()>2) cout << "AtmosCorruptor::getDispersiveWetPathLength = " 
		       << itsRIP->getDispersiveWetPathLength().get("micron") 
		       << " microns at " 
		       << fRefFreq()[0]/1e9 << " GHz" << endl;

  //  itsSkyStatus = new atm::SkyStatus(*itsRIP);

#endif
}




void AtmosCorruptor::initialize() {
  // for testing only

  mode()="test";
  if (!times_initialized())
    throw(AipsError("logic error in AtmCorr::init(Seed,Beta,scale) - slot times not initialized."));

  initAtm();
  pwv_p.resize(nAnt(),False,True);
  for (Int ia=0;ia<nAnt();++ia) {
    pwv_p[ia] = new Vector<Float>(nSim());
    // not really pwv, but this is a test mode
    for (Int i=0;i<nSim();++i) 
      (*(pwv_p[ia]))(i) = (Float(i)/Float(nSim()) + Float(ia)/Float(nAnt()))*mean_pwv()*10;  
  }

  if (prtlev()>2) cout << "AtmosCorruptor::init [test]" << endl;
}



// this one is for the M - maybe we should just make one Corruptor and 
// pass the VisCal::Type to it - the concept of the corruptor taking a VC
// instead of being a member of it.

void AtmosCorruptor::initialize(const VisIter& vi, const Record& simpar) {

  LogIO os(LogOrigin("AtmosCorr", "init()", WHERE));
  
  // start with amp defined as something
  amp()=1.0;
  if (simpar.isDefined("amplitude")) amp()=simpar.asFloat("amplitude");
  
  // simple means just multiply by amp()
  mode()="simple"; 
  if (simpar.isDefined("mode")) mode()=simpar.asString("mode");  

  if (mode()=="simple") {
    freqDepPar()=False; 
    if (prtlev()>2) cout << "AtmosCorruptor::init [simple scale by " << amp() << "]" << endl;

  } else {
        
    antDiams = vi.msColumns().antenna().dishDiameter().getColumn();
    
    // use ATM but no time dependence of atm - e.g. Tf [Tsys scaling, also Mf]
    if (freqDepPar()) initAtm();
    
    // RI todo AtmCorr::initialize catch other modes?  
    // if (mode()=="tsys") {

    // go with ATM straight up:
    tauscale() = 1.0;
    // user can override the ATM tau0 thusly 
    if (simpar.isDefined("tau0")) {
      tauscale()=simpar.asFloat("tau0");
      // (if tau0 is not set, scale is 1.)    
      // find tau in center of band for current ATM parameters
      if (freqDepPar()) tauscale()/=opac(nChan()/2);
      // if freqDep then opac will be called in simPar and multiplied by tauscale
    } else {
      if (freqDepPar()) throw(AipsError("Must define tau0 if not using ATM to scale Tsys"));
    }
    
    // modified from mm::simPar:  
    // Tsys = Tsys0 + Tsys1*exp(+tau)
    tsys0() = simpar.asFloat("tcmb") - simpar.asFloat("spillefficiency")*simpar.asFloat("tatmos");
    tsys1() = simpar.asFloat("spillefficiency")*simpar.asFloat("tatmos") + 
      (1-simpar.asFloat("spillefficiency"))*simpar.asFloat("tground") +
      simpar.asFloat("trx");
    
    // conversion to Jy, when divided by D1D2
    amp() = 4 * C::sqrt2 * 1.38062e-16 * 1e23 * 1e-4 / 		
      ( simpar.asFloat("antefficiency") * 
	simpar.asFloat("correfficiency") * C::pi );

    os << LogIO::NORMAL 
       << "Tsys = " << tsys0() << " + exp(" << tauscale() << ") * " 
       << tsys1() << " => " << tsys0()+exp(tauscale())*tsys1() 
       << " [freqDepPar="<<freqDepPar()<<"]"<< LogIO::POST;    
    if (prtlev()>1) 
      cout << "AtmosCorruptor::init "
	   << "Tsys = " << tsys0() << " + exp(" << tauscale() << ") * " 
	   << tsys1() << " => " << tsys0()+exp(tauscale())*tsys1() 
	   << " [freqDepPar="<<freqDepPar() << "]"<< endl;    
  }
}



// opacity - for screens, we'll need other versions of this like 
// the different cphase calls that multiply wetopacity by fluctuation in pwv
Float AtmosCorruptor::opac(const Int ichan) {
  Float opac = itsRIP->getDryOpacity(currSpw(),ichan).get() + 
    itsRIP->getWetOpacity(currSpw(),ichan).get()  ;
  return opac;
}






void AtmosCorruptor::initialize(const Int Seed, const Float Beta, const Float scale) {
  // individual delays for each antenna

  initAtm();

  mode()="1d";
  // assumes nSim already changed to be even intervals in time by the caller - 
  // i.e. if the solTimes are not even, this has been dealt with.
  if (!times_initialized())
    throw(AipsError("logic error in AtmCorr::init(Seed,Beta,scale) - slot times not initialized."));

  fBM* myfbm = new fBM(nSim());
  pwv_p.resize(nAnt(),False,True);
  for (Int iant=0;iant<nAnt();++iant){
    pwv_p[iant] = new Vector<Float>(nSim());
    myfbm->initialize(Seed+iant,Beta); // (re)initialize
    *(pwv_p[iant]) = myfbm->data(); // iAnt()=iant; delay() = myfbm->data();
    Float pmean = mean(*(pwv_p[iant]));
    Float rms = sqrt(mean( (*(pwv_p[iant])-pmean)*(*(pwv_p[iant])-pmean) ));
    if (prtlev()>3 and currAnt()<2) {
      cout << "RMS fBM fluctuation for antenna " << iant 
	   << " = " << rms << " ( " << pmean << " ; beta = " << Beta << " ) " << endl;      
    }
    // scale is set above to delta/meanpwv
    // Float lscale = log(scale)/rms;
    for (Int islot=0;islot<nSim();++islot)
      (*(pwv_p[iant]))[islot] = (*(pwv_p[iant]))[islot]*scale/rms;  
    if (prtlev()>2 and currAnt()<5) {
      Float pmean = mean(*(pwv_p[iant]));
      Float rms = sqrt(mean( (*(pwv_p[iant])-pmean)*(*(pwv_p[iant])-pmean) ));
      cout << "RMS fractional fluctuation for antenna " << iant 
	   << " = " << rms << " ( " << pmean << " ) " 
	// << " lscale = " << lscale 
	   << endl;      
    }
    currAnt()=iant;
  }

  if (prtlev()>2) cout << "AtmosCorruptor::init [1d]" << endl;

}



  
  void AtmosCorruptor::initialize(const Int Seed, const Float Beta, const Float scale, const ROMSAntennaColumns& antcols) {
  // 2d delay screen
  LogIO os(LogOrigin("AtmCorr", "init(Seed,Beta,Scale,AntCols)", WHERE));
  initAtm();

  mode()="2d";

  if (!times_initialized())
    throw(AipsError("logic error in AtmCorr::init(Seed,Beta,scale) - slot times not initialized."));
  
   // figure out where the antennas are, for blowing a phase screen over them
  // and how big they are, to set the pixel scale of the screen
  Float mindiam = min(antcols.dishDiameter().getColumn()); // units? dDQuant()?
  pixsize() = 0.5*mindiam; // RI_TODO temp compensate for lack of screen interpolation
  nAnt()=antcols.nrow();
  antx().resize(nAnt());
  anty().resize(nAnt());
  MVPosition ant;
  for (Int i=0;i<nAnt();i++) {	
    ant = antcols.positionMeas()(i).getValue();
    // have to convert to ENU or WGS84
    // ant = MPosition::Convert(ant,MPosition::WGS84)().getValue();
    // RI_TODO do this projection properly
    antx()[i] = ant.getLong()*6371000.;
    anty()[i] = ant.getLat()*6371000.; // m
  }     
  // from SDTableIterator
  //// but this expects ITRF XYZ, so make a Position and convert
  //obsPos = MPosition(Quantity(siteElev_.asdouble(thisRow_), "m"),
  //			 Quantity(siteLong_.asdouble(thisRow_), "deg"),
  //			 Quantity(siteLat_.asdouble(thisRow_), "deg"),
  //			 MPosition::WGS84);
  //obsPos = MPosition::Convert(obsPos, MPosition::ITRF)();
  Float meanlat=mean(anty())/6371000.;
  antx()=antx()*cos(meanlat);
  if (prtlev()>5) 
    cout << antx() << endl << anty() << endl;
  Int buffer(2); // # pix border
  //antx()=antx()-mean(antx());
  //anty()=anty()-mean(anty());
  antx()=antx()-min(antx());
  anty()=anty()-min(anty());
  antx()=antx()/pixsize();
  anty()=anty()/pixsize();
  if (prtlev()>4) 
    cout << antx() << endl << anty() << endl;

  Int ysize(Int(ceil(max(anty())+buffer)));

  const Float tracklength = stopTime()-startTime();    
  const Float blowlength = windspeed()*tracklength*1.05; // 5% margin
  if (prtlev()>3) 
    cout << "blowlength: " << blowlength << " track time = " << tracklength << endl;
  
  Int xsize(Int(ceil(max(antx())+buffer+blowlength/pixsize()))); 

  if (prtlev()>3) 
    cout << "xy screen size = " << xsize << "," << ysize << 
      " pixels (" << pixsize() << "m)" << endl;
  // if the array is too elongated, FFT sometimes gets upset;
  if (Float(xsize)/Float(ysize)>5) ysize=xsize/5;
  
  if (prtlev()>2) 
    cout << "creating new fBM of size " << xsize << "," << ysize << " (may take a few minutes) ... " << endl;
  os << LogIO::POST << "creating new fBM of size " << xsize << "," << ysize << " (may take a few minutes) ... " << LogIO::POST;

  fBM* myfbm = new fBM(xsize,ysize);
  screen_p = new Matrix<Float>(xsize,ysize);
  myfbm->initialize(Seed,Beta); 
  *screen_p=myfbm->data();
  if (prtlev()>3) 
    cout << " fBM created" << endl;

  Float pmean = mean(*screen_p);
  Float rms = sqrt(mean( ((*screen_p)-pmean)*((*screen_p)-pmean) ));
  // if (prtlev()>4) cout << (*screen_p)[10] << endl;
  if (prtlev()>3 and currAnt()<2) {
    cout << "RMS screen fluctuation " 
	 << " = " << rms << " ( " << pmean << " ; beta = " << Beta << " ) " << endl;
  }
  // scale is set above to delta/meanpwv
  *screen_p = myfbm->data() * scale/rms;

  if (prtlev()>2) cout << "AtmosCorruptor::init [2d]" << endl;

}




Complex AtmosCorruptor::cphase(const Int ix, const Int iy, const Int ichan) {
  // this is the complex phase gain for a T
  // expects pixel positions in screen - already converted using the pixscale
  // of the screen, and modified for off-zenith pointing

  AlwaysAssert(mode()=="2d",AipsError);
  Float delay;
  ostringstream o; 
 
  if (curr_slot()>=0 and curr_slot()<nSim()) {
    // blow
    Int blown(Int(floor( (slot_time(curr_slot())-slot_time(0)) *
			 windspeed()/pixsize() ))); 
    if (prtlev()>4 and currAnt()<2) cout << "blown " << blown << endl;

    if ((ix+blown)>(screen_p->shape())[0]) {
      o << "Delay screen blown out of range (" << ix << "+" 
	<< blown << "," << iy << ") (" << screen_p->shape() << ")" << endl;
      throw(AipsError(o));
    }
    // RI TODO Tcorr::gain  interpolate screen!
    Float deltapwv = (*screen_p)(ix+blown,iy);
    delay = itsRIP->getDispersiveWetPhaseDelay(currSpw(),ichan).get("rad") 
      * deltapwv / 57.2958; // convert from deg to rad
    return Complex(cos(delay),sin(delay));
  } else {    
    o << "atmosCorruptor::cphase: slot " << curr_slot() << "out of range!" <<endl;
    throw(AipsError(o));
    return Complex(1.);
  }
}


Complex AtmosCorruptor::cphase(const Int ichan) {
  AlwaysAssert(mode()=="1d" or mode()=="test",AipsError);
  Float delay;
  
  if (curr_slot()>=0 and curr_slot()<nSim()) {
    // if this gets used in the future, 
    // be careful about using freq if not freqDepPar()
    // Float freq = fRefFreq()[currSpw()] + 
    //   Float(ichan) * (fWidth()[currSpw()]/Float(fnChan()[currSpw()]));
    
    if (currAnt()<=pwv_p.nelements()) {
      Float deltapwv = (*pwv_p[currAnt()])(curr_slot());
      delay = itsRIP->getDispersiveWetPhaseDelay(currSpw(),ichan).get("rad") 
	* deltapwv / 57.2958; // convert from deg to rad
      if (prtlev()>5) 
	cout << itsRIP->getDispersiveWetPhaseDelay(0,ichan).get("rad") << " "
	     << itsRIP->getDispersiveWetPhaseDelay(1,ichan).get("rad") << " "
	     << itsRIP->getDispersiveWetPhaseDelay(2,ichan).get("rad") << " "
	     << itsRIP->getDispersiveWetPhaseDelay(3,ichan).get("rad") << endl;
    } else
      throw(AipsError("AtmosCorruptor internal error accessing pwv()"));  
    return Complex(cos(delay),sin(delay));
  } else {
    cout << "AtmosCorruptor::cphase: slot " << curr_slot() << "out of range!" <<endl;
    return Complex(1.);
  }
}











//###################################  fractional brownian motion


fBM::fBM(uInt i1) :    
  initialized_(False)
{ data_ = new Vector<Float>(i1); };

fBM::fBM(uInt i1, uInt i2) :
  initialized_(False)
{ data_ = new Matrix<Float>(i1,i2); };

fBM::fBM(uInt i1, uInt i2, uInt i3) :
  initialized_(False)
{ data_ = new Cube<Float>(i1,i2,i3); };

void fBM::initialize(const Int seed, const Float beta) {
  
  MLCG rndGen_p(seed,seed);
  Normal nDist_p(&rndGen_p, 0.0, 1.0); // sigma=1.
  Uniform uDist_p(&rndGen_p, 0.0, 1.0);
  IPosition s = data_->shape();
  uInt ndim = s.nelements();
  Float amp,phase,pi=3.14159265358979;
  uInt i0,j0;
  
  // FFTServer<Float,Complex> server;
  // This class assumes that a Complex array is stored as
  // pairs of floating point numbers, with no intervening gaps, 
  // and with the real component first ie., 
  // <src>[re0,im0,re1,im1, ...]</src>. This means that the 
  // following type casts work,
  // <srcblock>
  // S * complexPtr;
  // T * realPtr = (T * ) complexPtr;
  // </srcblock>

  Int stemp(s(0));
  if (ndim>1)
    stemp=s(1);

  IPosition size(1,s(0));
  IPosition size2(2,s(0),stemp);
  // takes a lot of thread thrashing to resize the server but I can't
  // figure a great way around the scope issues to just define a 2d one
  // right off the bat
  FFTServer<Float,Complex> server(size);
  
  Vector<Complex> F(s(0)/2);
  Vector<Float> G; // size zero to let FFTServer calc right size  

  Matrix<Complex> F2(s(0)/2,stemp/2);
  Matrix<Float> G2;
  
  // FFTServer C,R assumes that the input is hermitian and only has 
  // half of the elements in each direction
  
  switch(ndim) {
  case 1:
    // beta = 1+2H = 5-2D
    for (uInt i=0; i<s(0)/2-1; i++) {
      // data_->operator()(IPosition(1,i))=5.;
      phase = 2.*pi*uDist_p(); 
      // RI TODO is this assuming the origin is at 0,0 in which case 
      // we should be using FFTServer::fft0 ? 
      amp = pow(Float(i+1), -0.5*beta) * nDist_p();
      F(i)=Complex(amp*cos(phase),amp*sin(phase));
      // F(s(0)-i)=Complex(amp*cos(phase),-amp*sin(phase));
    }
    server.fft(G,F,False);  // complex to real Xform
    // G comes out twice length of F 
    for (uInt i=0; i<s(0); i++)
      data_->operator()(IPosition(1,i)) = G(i); // there has to be a better way with strides or something.
    //    cout << endl << F(2) << " -> " << G(2) << " -> " 
    //	 << data_->operator()(IPosition(1,2)) << endl;
    break;
  case 2:
    // beta = 1+2H = 7-2D
    server.resize(size2);
    // RI_TODO fBM::init make sure hermitian calc is correct - fftw only doubles first axis.
    // F2.resize(s(0)/2,s(1)/2);
    F2.resize(s(0)/2,s(1));
    for (uInt i=0; i<s(0)/2; i++)
      // for (uInt j=0; j<s(1)/2; j++) {
      for (uInt j=0; j<s(1); j++) {
	phase = 2.*pi*uDist_p(); 	  
	// RI TODO is this assuming the origin is at 0,0 in which case 
	// we should be using FFTServer::fft0 ? 
	if (i!=0 or j!=0) {
	  Float ij2 = sqrt(Float(i)*Float(i) + Float(j)*Float(j));
	  // RI_TODO still something not quite right with exponent
	  // amp = pow(ij2, -0.25*(beta+1)) * nDist_p();
	  amp = pow(ij2, -0.5*(beta+0.5) ) * nDist_p();
	} else {
	  amp = 0.;
	}
	F2(i,j)=Complex(amp*cos(phase),amp*sin(phase));
	// if (i==0) {

	//   i0=0;
	// } else {
	//   i0=s(0)-i;
	// }
	// do we need this ourselves in the second dimension?
	// if (j==0) {
	//   j0=0;
	// } else {
	//   j0=s(1)-j;
	// }
	// F2(i0,j0)=Complex(amp*cos(phase),-amp*sin(phase));
      }
    // The complex to real transform does not check that the
    // imaginary component of the values where u=0 are zero
    F2(s(0)/2,0).imag()=0.;
    F2(0,s(1)/2).imag()=0.;
    // cout << endl;
    F2(s(0)/2,s(1)/2).imag()=0.;
    // for (uInt i=0; i<s(0)/2; i++)
    // 	for (uInt j=0; j<s(1)/2; j++) {
    // 	  phase = 2.*pi*uDist_p();
    // 	  amp = pow(Double(i)*Double(i) + Double(j)*Double(j), 
    // 		    -0.25*(beta+1)) * nDist_p();
    // 	  F2(i,s(1)-j) = Complex(amp*cos(phase),amp*sin(phase));
    // 	  F2(s(0)-i,j) = Complex(amp*cos(phase),-amp*sin(phase));
    // 	}
    server.fft(G2,F2,False);  // complex to real Xform
    // G2 comes out sized s(0),s(1)/2 i.e. only doubles the first dim.
    // cout << G2.shape() << endl;  
    // there has to be a better way
    for (uInt i=0; i<s(0); i++)
      for (uInt j=0; j<s(1); j++) 
	data_->operator()(IPosition(2,i,j)) = G2(i,j);       
    break;
  case 3:
    // beta = 1+2H = 9-2D
    throw(AipsError("no 3d fractional Brownian motion yet."));
    for (uInt i=0; i<s(0); i++)
      for (uInt j=0; j<s(1); j++)
	for (uInt k=0; j<s(3); k++)
	  data_->operator()(IPosition(3,i,j,k))=5.;     
  }
};










//###################################  

Complex GJonesCorruptor::simPar(const VisIter& vi,VisCal::Type type,Int ipar) {    
  if (type==VisCal::G || type==VisCal::B) {
      return gain(ipar,focusChan());
  } else  throw(AipsError("GCorruptor: incompatible VisCal type "+type));
}


GJonesCorruptor::GJonesCorruptor(const Int nSim) : 
  CalCorruptor(nSim),  // parent
  tsys_(0.0)
{}

GJonesCorruptor::~GJonesCorruptor() {
  if (prtlev()>2) cout << "GCorruptor::~GCorruptor()" << endl;
}


Matrix<Complex>* GJonesCorruptor::drift() { 
  if (currAnt()<=drift_p.nelements())
    return drift_p[currAnt()];
  else
    return NULL;
};
 

void GJonesCorruptor::initialize() {
  // for testing only
  if (prtlev()>2) cout << "GCorruptor::init [test]" << endl;
}

void GJonesCorruptor::initialize(const Int Seed, const Float Beta, const Float scale) {
  // individual delays for each antenna

  fBM* myfbm = new fBM(nSim());
  drift_p.resize(nAnt(),False,True);
  for (Int iant=0;iant<nAnt();++iant) {
    drift_p[iant] = new Matrix<Complex>(nPar(),nSim());
    for (Int icorr=0;icorr<nPar();++icorr){
      myfbm->initialize(Seed+iant+icorr,Beta); // (re)initialize
      Float pmean = mean(myfbm->data());
      Float rms = sqrt(mean( ((myfbm->data())-pmean)*((myfbm->data())-pmean) ));
      if (prtlev()>3 and currAnt()<2) {
	cout << "RMS fBM fluctuation for antenna " << iant 
	     << " = " << rms << " ( " << pmean << " ; beta = " << Beta << " ) " << endl;      
      }
      // amp
      Vector<Float> amp = (myfbm->data()) * scale/rms;
      for (Int i=0;i<nSim();++i)  {
	(*(drift_p[iant]))(icorr,i) = Complex(1.+amp[i],0);
      }
      
      // phase
      myfbm->initialize((Seed+iant+icorr)*100,Beta); // (re)initialize
      pmean = mean(myfbm->data());
      rms = sqrt(mean( ((myfbm->data())-pmean)*((myfbm->data())-pmean) ));
      Vector<Float> angle = (myfbm->data()) * scale/rms * 3.141592; // *2 ?
      for (Int i=0;i<nSim();++i)  {
	(*(drift_p[iant]))(icorr,i) *= exp(Complex(0,angle[i]));
      }

      currAnt()=iant;
    }
  }

  if (prtlev()>2) cout << "GCorruptor::init" << endl;

}


Complex GJonesCorruptor::gain(const Int icorr,const Int ichan) {
  if (curr_slot()>=0 and curr_slot()<nSim() and icorr>=0 and icorr<nPar()) {    
    if (currAnt()>drift_p.nelements())
      throw(AipsError("GJonesCorruptor internal error accessing drift()"));  
    Complex delta = (*drift_p[currAnt()])(icorr,curr_slot());    
    return delta;
  } else {
    cout << "GCorruptor::gain: slot " << curr_slot() << "out of range!" <<endl;
    return Complex(1.);
  }
}






} //casa namespace
