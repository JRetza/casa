//# SpectralCollapser.cc: Implementation of class SpectralCollapser
//# Copyright (C) 1998,1999,2000,2001,2003
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
//# $Id: $

#include <imageanalysis/ImageAnalysis/SpectralFitter.h>
//#include <imageanalysis/ImageAnalysis/SpectralCollapser.h>
//#include <imageanalysis/ImageAnalysis/ImageCollapser.h>

#include <casa/Arrays/ArrayMath.h>
#include <casa/OS/Directory.h>
#include <casa/OS/RegularFile.h>
#include <casa/OS/SymLink.h>
#include <coordinates/Coordinates/SpectralCoordinate.h>
#include <images/Images/ImageFit1D.h>
#include <images/Images/ImageUtilities.h>
#include <images/Images/ImageMoments.h>
#include <images/Images/FITSImage.h>
#include <images/Images/FITSQualityImage.h>
#include <images/Images/MIRIADImage.h>
#include <images/Images/PagedImage.h>
#include <images/Images/SubImage.h>
#include <images/Images/TempImage.h>
#include <components/SpectralComponents/SpectralList.h>
#include <components/SpectralComponents/SpectralElement.h>
#include <components/SpectralComponents/ProfileFit1D.h>
#include <lattices/Lattices/LatticeUtilities.h>

namespace casa {
SpectralFitter::SpectralFitter():
	_log(new LogIO()), _resultMsg(""){
	_setUp();
}

SpectralFitter::~SpectralFitter() {
	delete _log;
}

Bool SpectralFitter::fit(const Vector<Float> &spcVals,
		const Vector<Float> &yVals, const Vector<Float> &eVals,
		const Float startVal, const Float endVal,
		const Bool fitGauss, const Bool fitPoly,
		const uInt nPoly, String &msg) {

	*_log << LogOrigin("SpectralFitter", "fit", WHERE);

	if (spcVals.size() < 1) {
		msg = String("No spectral values provided!");
		*_log << LogIO::WARN << msg << LogIO::POST;
		return False;
	}

	Bool ascending = True;
	if (spcVals(spcVals.size() - 1) < spcVals(0))
		ascending = False;

	uInt startIndex, endIndex;
	if (ascending) {
		if (endVal < spcVals(0)) {
			msg = String("Start value: ") + String::toString(endVal) + String(
					" is smaller than all spectral values!");
			*_log << LogIO::WARN << msg << LogIO::POST;
			return False;
		}
		if (startVal > spcVals(spcVals.size() - 1)) {
			msg = String("End value: ") + String::toString(startVal) + String(
					" is larger than all spectral values!");
			*_log << LogIO::WARN << msg << LogIO::POST;
			return False;
		}
		startIndex = 0;
		while (spcVals(startIndex) < startVal)
			startIndex++;

		endIndex = spcVals.size() - 1;
		while (spcVals(endIndex) > endVal)
			endIndex--;
	} else {
		if (endVal < spcVals(spcVals.size() - 1)) {
			msg = String("Start value: ") + String::toString(endVal) + String(
					" is smaller than all spectral values!");
			*_log << LogIO::WARN << msg << LogIO::POST;
			return False;
		}
		if (startVal > spcVals(0)) {
			msg = String("End value: ") + String::toString(startVal) + String(
					" is larger than all spectral values!");
			*_log << LogIO::WARN << msg << LogIO::POST;
			return False;
		}
		startIndex = 0;
		while (spcVals(startIndex) > endVal)
			startIndex++;

		endIndex = spcVals.size() - 1;
		while (spcVals(endIndex) < startVal)
			endIndex--;
	}

	// prepare the fit images
	Vector<Bool> maskVals;
	Vector<Double> weightVals;
	if (!_prepareData(spcVals, eVals, startIndex, endIndex, maskVals, weightVals)){
		msg = String("The error array contains values <0.0!");
		*_log << LogIO::WARN << msg << LogIO::POST;
		return False;
	}

	//cout <<"spcVals: " << spcVals << endl;
	//cout <<"maskVals: " << maskVals << endl;
	//cout <<"start: " << startIndex << " end: " << endIndex << endl;

	// make sure that something can be done
	if ((endIndex-startIndex) + 1 < 2){
		msg = String("Only one data value selected. Can not fit anything.");
		*_log << LogIO::WARN << msg << LogIO::POST;
		return False;
	}
	else if (fitGauss && ((endIndex-startIndex) + 1 < 3)){
		msg = String("Only two data value selected. Can not fit a Gaussian.");
		*_log << LogIO::WARN << msg << LogIO::POST;
		return False;
	}

	// convert the input values to Double
	Vector<Double> dspcVals(spcVals.size()), dyVals(yVals.size());
	//for (uInt index=0; index<spcVals.size(); index++)
	//	dspcVals(index) = Double(spcVals(index));
	//for (uInt index=0; index<spcVals.size(); index++)
	//	dyVals(index) = Double(yVals(index));
	//convertArray(Array<T> &to,	const Array<U> &from);
	convertArray(dspcVals,	spcVals);
	convertArray(dyVals,	yVals);


	// store start and end values
	_startVal   = startVal;
	_endVal     = endVal;
	_startIndex = startIndex;
	_endIndex   = endIndex;

	// set data, weights and status
	_fit.clearList();
	if (weightVals.size()>0){
		_fit.setData (dspcVals, dyVals, maskVals, weightVals);
	}
	else {
		_fit.setData (dspcVals, dyVals, maskVals);
	}
	_fitStatus=SpectralFitter::UNKNOWN;

	// set the estimated elements
	SpectralList elemList;
	_prepareElems(fitGauss, fitPoly, nPoly, dspcVals, dyVals, elemList);
	_fit.setElements(elemList);
	_report(_fit.getList(False), *_log);

	// do the fit
	Bool ok(False);
	try {
		ok = _fit.fit();
	} catch (AipsError x) {
		msg = x.getMesg();
		*_log << LogIO::WARN << msg << LogIO::POST;
		return False;
	}
	if (ok){
		_fitStatus=SpectralFitter::SUCCESS;
	}
	else{
		_fitStatus=SpectralFitter::FAILED;
	   msg = "Fitter did not converge in " + String::toString(_fit.getNumberIterations()) + " iterations";
	  *_log << LogIO::NORMAL  << msg << LogIO::POST;
	   return False;
	}

	return True;
}

void SpectralFitter::getFit(const Vector<Float> &spcVals, Vector<Float> &spcFit, Vector<Float> &yFit) const{
	Vector<Double> tmp;

	// re-size all vectors
	spcFit.resize(_endIndex-_startIndex+1);
	yFit.resize(_endIndex-_startIndex+1);
	tmp.resize(_endIndex-_startIndex+1);

	// extract the range of the independent coordinate
	spcFit = spcVals(IPosition(1, _startIndex), IPosition(1, _endIndex));

	// extract the range of the dependent coordinate
	tmp    = (getFit())(IPosition(1, _startIndex), IPosition(1, _endIndex));

	// change to Float
	//for (uInt index=0; index<tmp.size(); index++)
	//	yFit(index)=Float(tmp(index));
	convertArray(yFit, tmp);
}

void SpectralFitter::report() const{
	String msg;
	report(*_log);
}

String SpectralFitter::report(LogIO &os) const{
	String resultMsg("");
	SpectralList list = _fit.getList(True);

	switch (_fitStatus){
	case SpectralFitter::SUCCESS:
		os << LogIO::NORMAL << " " << LogIO::POST;
		os << LogIO::NORMAL << "Successful fit!" << LogIO::POST;
		os << LogIO::NORMAL << "No. of iterations: " << String::toString(_fit.getNumberIterations()) << LogIO::POST;
		os << LogIO::NORMAL << "Chi-square:       " << String::toString(_fit.getChiSquared())       << LogIO::POST;
		// report the spectral elements
		resultMsg  = _report(list, os);
		//resultMsg += " No. of iterations: "+ String::toString(_fit.getNumberIterations()) + ", chi-square: "+String::toString(_fit.getChiSquared());
		//resultMsg += " Chisqu.: "+String::toString(_fit.getChiSquared()) + " " + String::toString(_fit.getNumberIterations()) + " Iter.";

		break;
	case SpectralFitter::FAILED:
		resultMsg = "Fit did not converge in " + String::toString(_fit.getNumberIterations()) + " iterations!";
		os << LogIO::NORMAL << " " << LogIO::POST;
		os << LogIO::NORMAL << resultMsg << LogIO::POST;
		break;
	default:
		resultMsg = "The fit is in an undefined state!";
		os << LogIO::NORMAL << " " << LogIO::POST;
		os << LogIO::NORMAL << resultMsg << LogIO::POST;
	}

	return resultMsg;
}

void SpectralFitter::_setUp() {
	*_log << LogOrigin("SpectralFitter", "setUp");

	// setup the fitter and the status
	_fit = ProfileFit1D<Double>();
	_fitStatus=SpectralFitter::UNKNOWN;
}

Bool SpectralFitter::_prepareData(const Vector<Float> &xVals, const Vector<Float> &eVals,
		const Int &startIndex, const Int &endIndex, Vector<Bool> &maskVals, Vector<Double> &weightVals) const {

	// create the mask
	maskVals.resize(xVals.size());
	maskVals = False;
	maskVals(IPosition(1, startIndex), IPosition(1, endIndex)) = True;

	// if possible, compute the weights
	if (eVals.size()>0){
		weightVals.resize(xVals.size());
		weightVals=0.0;
		Vector<Double> one(eVals.size(), 1.0);
		Vector<Double> deVals(eVals.size(), 0.0);
		//for (uInt index=0; index<eVals.size(); index++)
		//	deVals(index) = eVals(index);
		convertArray(deVals, eVals);
		if (min(eVals(IPosition(1, startIndex), IPosition(1, endIndex)))<0.0)
			return False;
		weightVals(IPosition(1, startIndex), IPosition(1, endIndex)) = one(IPosition(1, startIndex), IPosition(1, endIndex)) / deVals(IPosition(1, startIndex), IPosition(1, endIndex));
	}

	return True;
}

/*
Bool SpectralFitter::_prepareElems(const Bool fitGauss, const Vector<Double> gPars, const Bool fitPoly,
		const uInt nPoly, const Vector<Double> pPars, SpectralList& list){

	// add a Gaussian
	if (fitGauss){
		if (gPars.size()>0)
			list.add(GaussianSpectralElement(gPars));
		else
			list.add(GaussianSpectralElement());
	}

	// add a polynomial
	if (fitPoly) {
		if (pPars.size()>0)
			list.add(PolynomialSpectralElement(pPars));
		else
			list.add(PolynomialSpectralElement(nPoly));
	}

	return True;
}
*/

Bool SpectralFitter::_prepareElems(const Bool fitGauss, const Bool fitPoly, const uInt nPoly, Vector<Double> &xVals,
		Vector<Double> &yVals, SpectralList& list){
	Int nQuart=max(1,Int((_endIndex-_startIndex)/4));

	Double leftYVal(0.0), rightYVal(0.0);
	Double leftXVal(0.0), rightXVal(0.0);
	for (uInt index=_startIndex; index < (_startIndex+nQuart); index++){
		leftXVal += xVals(index);
		leftYVal += yVals(index);
	}
	leftXVal /= Double(nQuart);
	leftYVal /= Double(nQuart);

	for (uInt index=_endIndex; index > (_endIndex-nQuart); index--){
		rightXVal += xVals(index);
		rightYVal += yVals(index);
	}
	rightXVal /= Double(nQuart);
	rightYVal /= Double(nQuart);

	//cout << " leftXVal: " << leftXVal << " rightXVal: " << rightXVal << endl;
	//cout << " leftYVal: " << leftYVal << " rightYVal: " << rightYVal << endl;
	// swap the right and left values
	if (xVals(_startIndex)>xVals(_endIndex)){
		//cout << "Swapping...." << endl;
		Double tmp;
		tmp       = leftXVal;
		leftXVal  = rightXVal;
		rightXVal = tmp;

		tmp       = leftYVal;
		leftYVal  = rightYVal;
		rightYVal = tmp;
	}

	//cout << " leftXVal: " << leftXVal << " rightXVal: " << rightXVal << endl;
	//cout << " leftYVal: " << leftYVal << " rightYVal: " << rightYVal << endl;

	// add a polynomial
	if (fitPoly) {
		if (nPoly==0){
			Vector<Double> pPar(1, 0.5*(rightYVal+leftYVal));
			list.add(PolynomialSpectralElement(pPar));
		}
		else if (nPoly==1){
			Vector<Double> pPar(2, 0.0);
			pPar(1) = (rightYVal-leftYVal) / (rightXVal-leftXVal);
			pPar(0) = rightYVal - pPar(1)*rightXVal;
			list.add(PolynomialSpectralElement(pPar));
		}
	}

	// add a Gaussian
	if (fitGauss){
		//Vector<Double> gPar(3, 0.0);
		Double gAmp(0.0), gCentre(0.0), gSigma(0.0);
		// integrate over the data
		// integrate over the estimated polynomial
		Double curveIntegral(0.0), polyIntegral(0.0), averDisp(0.0);
		for (uInt index=_startIndex; index < (_endIndex+1); index++)
			curveIntegral += yVals(index);
		polyIntegral   = 0.5*(rightYVal+leftYVal)*Double(_endIndex-_startIndex+1);
		averDisp = fabs(xVals(_endIndex) - xVals(_startIndex)) /  Double(_endIndex-_startIndex);
		//cout << " curveIntegral: " << curveIntegral << " polyIntegral: " << polyIntegral << " averDisp: " << averDisp << endl;

		// make an estimate for the sigma (FWHM ~1/4 of x-range);
		// get the amplitude estimate from the integral and the sigma;
		// the centre estimate is set to the middle of the x-range;
		//gPar(2) = (xVals(_startIndex+nQuart)-xVals(_endIndex-nQuart))/(2.0*GaussianSpectralElement::SigmaToFWHM);
		//if (gPar(2))
		//	gPar(2) *= -1.0;
		//gPar(0) = (curveIntegral-polyIntegral)/(gPar(2)*sqrt(C::pi));
		//gPar(1) = xVals(_startIndex) + (xVals(_endIndex) - xVals(_startIndex)) / 2.0;
		gSigma = (xVals(_startIndex+nQuart)-xVals(_endIndex-nQuart))/(2.0*GaussianSpectralElement::SigmaToFWHM);
		if (gSigma<0.0)
			gSigma *= -1.0;
		gAmp = averDisp*(curveIntegral-polyIntegral)/(gSigma*sqrt(C::pi));
		gCentre = xVals(_startIndex) + (xVals(_endIndex) - xVals(_startIndex)) / 2.0;
		//cout << " gAmp: " << gAmp << " gCentre: " << gCentre << " gSigma: " << gSigma<<endl;

		list.add(GaussianSpectralElement(gAmp, gCentre, gSigma));
	}

	return True;
}

/*
void SpectralFitter::_report(const ProfileFit1D<Double> &fit, const Bool print, String &report) const{
	const SpectralList list = fit.getList(True);
	String spTypeStr;
	Vector<Double> params, errors;
	Double gaussArea(0.0), gaussCent(0.0), centVal(0.0);


	switch (_fitStatus){
	case SpectralFitter::SUCCESS:
		report += "The fit was successful!\n";
		report += "No. of iterations:  " + String::toString(fit.getNumberIterations()) + "\n";
		report += "Chi-squared:        " + String::toString(fit.getChiSquared())  + "\n";
		//report += "Wavelength shift: " + String::toString(_spcShift)  + "\n";

		for (uInt index=0; index < list.nelements(); index++){

			SpectralElement::Types spType = list[index]->getType();
			spTypeStr = list[index]->fromType(spType);
			list[index]->get(params);
			list[index]->getError(errors);

			switch (spType){
			case SpectralElement::GAUSSIAN:
				gaussArea = params(0)*params(2)*1.7724538509055;
				gaussCent = params(1);
				report += "Element No. " + String::toString(index) + ": " + spTypeStr+ "\n";
				//report += "Amplitude: " + String::toString(params(0)) + " centre: " + String::toString(params(1)+_spcShift) + " sigma: " + String::toString(params(2)) + " FWHM: " + String::toString(params(2)*GaussianSpectralElement::SigmaToFWHM) + "\n";
				report += "Amplitude: " + String::toString(params(0)) + " centre: " + String::toString(params(1)) + " sigma: " + String::toString(params(2)) + " FWHM: " + String::toString(params(2)*GaussianSpectralElement::SigmaToFWHM) + "\n";
				report += "Area: " + String::toString(gaussArea) + "\n";
				//report += "  parameters: " + String::toString(params) + "\n";
				report += "  errors:     " + String::toString(errors) + "\n";
				break;
			case SpectralElement::POLYNOMIAL:
				centVal = (*list[index])(gaussCent);
				report += "Element No. " + String::toString(index) + ": " + spTypeStr+ "\n";
				report += "  parameters: " + String::toString(params) + "\n";
				report += "  errors:     " + String::toString(errors) + "\n";
				report += "  gausscent value: " + String::toString(centVal) + "\n";
				break;
			default:
				report += "Element No. " + String::toString(index) + ": " + spTypeStr+ "\n";
				report += "  parameters: " + String::toString(params) + "\n";
				report += "  errors:     " + String::toString(errors) + "\n";
				break;
			}
			}
		break;
	case SpectralFitter::FAILED:
		report += "Fit die not converge in " + String::toString(fit.getNumberIterations()) + " iterations!\n";
		break;
	default:
		report += "The fit is not in a defined state!\n";
	}
	if (print)
		cout << report;
}
*/
String SpectralFitter::_report(const SpectralList &list, LogIO &os) const{
	ostringstream sstream;

	String spTypeStr;
	Vector<Double> params, errors;
	Double gaussAmpV(0.0), gaussCentV(0.0), gaussSigmaV(0.0), gaussFWHMV(0.0);
	Double gaussAmpE(0.0), gaussCentE(0.0), gaussSigmaE(0.0), gaussFWHME(0.0);
	Double gaussAreaV(0.0), gaussAreaE(0.0);
	Double polyOffsetV(0.0), polySlopeV(0.0);
	Double polyOffsetE(0.0), polySlopeE(0.0);
	Int gaussIndex(-1), polyIndex(-1);

	// go over all elements
	for (uInt index=0; index < list.nelements(); index++){

		// report element type and get the parameters/errors
		SpectralElement::Types spType = list[index]->getType();
		spTypeStr = list[index]->fromType(spType);
		//returnMsg += spTypeStr;
		//os << LogIO::NORMAL << "Element No. " << String::toString(index) << ": " << spTypeStr << LogIO::POST;
		list[index]->get(params);
		list[index]->getError(errors);

		switch (spType){

		// extract and report the Gaussian parameters
		case SpectralElement::GAUSSIAN:
			gaussIndex  = index;
			gaussAmpV   = params(0);
			gaussCentV  = params(1);
			gaussSigmaV = params(2);
			gaussFWHMV  = gaussSigmaV * GaussianSpectralElement::SigmaToFWHM;
			gaussAreaV  = gaussAmpV * gaussSigmaV * sqrt(C::pi);

			gaussAmpE   = errors(0);
			gaussCentE  = errors(1);
			gaussSigmaE = errors(2);
			gaussFWHME  = gaussSigmaE * GaussianSpectralElement::SigmaToFWHM;
			gaussAreaE  = sqrt(C::pi) * sqrt(gaussAmpV*gaussAmpV*gaussSigmaE*gaussSigmaE + gaussSigmaV*gaussSigmaV*gaussAmpE*gaussAmpE);

			os << LogIO::NORMAL << "  Amplitude: " << String::toString(gaussAmpV) << "+-" << gaussAmpE << " centre: " << String::toString(gaussCentV) << "+-" << gaussCentE << " FWHM: " << String::toString(gaussFWHMV) << "+-" << gaussFWHME<< LogIO::POST;
			os << LogIO::NORMAL << "  Gaussian area: " << String::toString(gaussAreaV) <<"+-"<< gaussAreaE << LogIO::POST;
			//returnMsg += " Cent.: " + String::toString(gaussCentV) + " FWHM: " + String::toString(gaussFWHMV) + "  Ampl.: " + String::toString(gaussAmpV);
			sstream << " Cent.: " << setiosflags(ios::scientific) << setprecision(6) << gaussCentV << " FWHM: " << setprecision(4) << gaussFWHMV << "  Ampl.: " << setprecision(3) << gaussAmpV;
			break;

		// extract and report the polynomial parameters
		case SpectralElement::POLYNOMIAL:
			polyIndex  = index;
			polyOffsetV = params(0);
			polyOffsetE = errors(0);
			if (params.size()>1){
				polySlopeV = params(1);
				polySlopeE = errors(2);
			}
			os << LogIO::NORMAL << "  Offset: " << String::toString(polyOffsetV) << "+-"<< String::toString(polyOffsetE) <<LogIO::POST;
			//returnMsg += "  Offs.: " + String::toString(polyOffsetV);
			sstream << "  Offs.: " << setiosflags(ios::scientific) << setprecision(3) << polyOffsetV;
			if (params.size()>1){
				os << LogIO::NORMAL << "  Slope:  " << String::toString(polySlopeV) << "+-"<< String::toString(polySlopeE) <<LogIO::POST;
				sstream << "  Slope:  " << setiosflags(ios::scientific) << setprecision(3) << polySlopeV;
				//returnMsg += "  Slope:  " + String::toString(polySlopeV);
			}
			break;

		// report the parameters
		default:
			os << LogIO::NORMAL << "  parameters: " << String::toString(params) << LogIO::POST;
			os << LogIO::NORMAL << "  errors:     " << String::toString(errors) << LogIO::POST;
			//returnMsg += "  Params:  " + String::toString(params);
			sstream << "  Params:  " << params;
			break;
		}
	}

	// if possible, compute and report the equivalent width
	if (gaussIndex > -1 && polyIndex >- 1){
		Double centVal = (*list[polyIndex])(gaussCentV);
		if (centVal==0.0){
			sstream << LogIO::NORMAL << "  Continuum is 0.0 - can not compute equivalent width!" << LogIO::POST;
		}
		else{
			os << LogIO::NORMAL << "Can compute equivalent width" << LogIO::POST;
			os << LogIO::NORMAL << "  Continuum value: " << String::toString(centVal) << LogIO::POST;
			os << LogIO::NORMAL << "  --> Equivalent width: " << String::toString(-1.0*gaussAreaV/centVal) << LogIO::POST;
			sstream << " Equ.Width: " << setiosflags(ios::scientific) << setprecision(4) << -1.0*gaussAreaV/centVal;
			//returnMsg += " Equ.Width: "+ String::toString(-1.0*gaussAreaV/centVal);
		}
	}
	return String(sstream);
}
}

