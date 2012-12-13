//# RegionManager.cc: framework independent class that provides 
//# functionality to tool of same name
//# Copyright (C) 2007
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

#include <imageanalysis/Regions/CasacRegionManager.h>

#include <casa/Containers/Record.h>
#include <casa/OS/File.h>
#include <images/Regions/ImageRegion.h>
#include <images/Regions/WCBox.h>
#include <lattices/Lattices/LCBox.h>
#include <measures/Measures/Stokes.h>
#include <tables/Tables/TableRecord.h>

#include <imageanalysis/Annotations/RegionTextList.h>

#include <casa/namespace.h>
#include <memory>

namespace casa { //# name space casa begins

const String CasacRegionManager::ALL = "ALL";

CasacRegionManager::CasacRegionManager() : RegionManager() {}

CasacRegionManager::CasacRegionManager(
		const CoordinateSystem& csys) : RegionManager(csys) {}

CasacRegionManager::~CasacRegionManager() {}

vector<uInt> CasacRegionManager::consolidateAndOrderRanges(
	const vector<uInt>& ranges
) {
	uInt arrSize = ranges.size()/2;
	uInt arrMin[arrSize];
	uInt arrMax[arrSize];
	for (uInt i=0; i<arrSize; i++) {
		arrMin[i] = ranges[2*i];
		arrMax[i] = ranges[2*i + 1];
	}
	Sort sort;
	sort.sortKey (arrMin, TpUInt);
	sort.sortKey (arrMax, TpUInt, 0, Sort::Descending);
	Vector<uInt> inxvec;
	vector<uInt> consol(0);
	sort.sort(inxvec, arrSize);
	for (uInt i=0; i<arrSize; i++) {
		uInt idx = inxvec(i);
		uInt size = consol.size();
		uInt min = arrMin[idx];
		uInt max = arrMax[idx];
		uInt lastMax = (i == 0) ? 0 : consol[size-1];
		if (i==0) {
			// consol.resize(2, True);
			// consol[0] = min;
			// consol[1] = max;
			consol.push_back(min);
			consol.push_back(max);
		}
		else if (
			// overlaps previous range, so extend
			(min < lastMax && max > lastMax)
			// or contiguous with previous range, so extend
			|| min == lastMax + 1
		) {
			// overwriting the end value, so do not resize
			consol[size-1] = max;
		}

		else if (min > lastMax + 1) {
			// non overlap of and not contiguous with previous range,
			// so create new end point pair

			consol.push_back(min);
			consol.push_back(max);

		}
	}
	return consol;
}

vector<uInt> CasacRegionManager::_setPolarizationRanges(
	String& specification, const String& firstStokes, const uInt nStokes,
	const StokesControl stokesControl
) const {
	LogOrigin origin("CasacRegionManager", __FUNCTION__);
	//const LogIO *myLog = _getLog();
	*_getLog() << origin;

	vector<uInt> ranges(0);
	if (! _getCsys()->hasPolarizationCoordinate()) {
		return ranges;
	}
	specification.trim();
	specification.upcase();
	if (specification == ALL) {
		ranges.push_back(0);
		ranges.push_back(nStokes - 1);
		return ranges;
	}
	if (specification.empty()) {
		// ranges.resize(2);
		// ranges[0] = 0;
		ranges.push_back(0);
		switch (stokesControl) {
		case USE_FIRST_STOKES:
			ranges.push_back(0);
			specification = firstStokes;
			break;
		case USE_ALL_STOKES:
			ranges.push_back(nStokes - 1);
			specification = ALL;
			break;
		default:
			// bug if we get here
			*_getLog() << "Logic error, unhandled stokes control" << LogIO::EXCEPTION;
		};
		return ranges;
	}
	// First split on commas and semi-colons.
	// in the past for polarization specification.

	Vector<String> parts = stringToVector(specification, Regex("[,;]"));
	Vector<String> polNames = Stokes::allNames(False);
	uInt nNames = polNames.size();
	Vector<uInt> nameLengths(nNames);
	for (uInt i=0; i<nNames; i++) {
		nameLengths[i] = polNames[i].length();
	}
	uInt *lengthData = nameLengths.data();

	Vector<uInt> idx(nNames);
	Sort sorter;
	sorter.sortKey(lengthData, TpUInt, 0, Sort::Descending);
	sorter.sort(idx, nNames);

	Vector<String> sortedNames(nNames);
	for (uInt i=0; i<nNames; i++) {
		sortedNames[i] = polNames[idx[i]];
		sortedNames[i].upcase();
	}

	for (uInt i=0; i<parts.size(); i++) {
		String part = parts[i];

		Vector<String>::iterator iter = sortedNames.begin();
		while (iter != sortedNames.end() && ! part.empty()) {
			if (part.startsWith(*iter)) {
				Int stokesPix = _getCsys()->stokesPixelNumber(*iter);
				if (stokesPix >= int(nStokes)) {
					stokesPix = -1;
				}
				if (stokesPix >= 0) {
					ranges.push_back(stokesPix);
					ranges.push_back(stokesPix);
					// consume the string
					part = part.substr(iter->length());
					if (! part.empty()) {
						// reset the iterator to start over at the beginning of the list for
						// the next specified polarization
						iter = sortedNames.begin();
					}
				}
				else {
					*_getLog() << "Polarization " << *iter << " specified in "
						<< parts[i] << " does not exist in the specified "
						<< "coordinate system for the specified number of "
						<< "polarization parameters" << LogIO::EXCEPTION;
				}
			}
			else {
				iter++;
			}
		}
		if (! part.empty()) {
			*_getLog() << "(Sub)String " << part << " in stokes specification part " << parts[i]
			    << " does not match a known polarization." << LogIO::EXCEPTION;
		}
	}
	return consolidateAndOrderRanges(ranges);
}

vector<Double> CasacRegionManager::_setBoxCorners(const String& box) const {
	Vector<String> boxParts = stringToVector(box);
	AlwaysAssert(boxParts.size() % 4 == 0, AipsError);
	vector<Double> corners(boxParts.size());
	for(uInt i=0; i<boxParts.size()/4; i++) {
		uInt index = 4*i;
		for (uInt j=0; j<4; j++) {
			boxParts[index + j].trim();
			if (! boxParts[index + j].matches(RXdouble)) {
				*_getLog() << "Box spec contains non numeric characters and so is invalid"
						<< LogIO::EXCEPTION;
			}
			corners[index + j] = String::toDouble(boxParts[index + j]);
		}
	}
	return corners;
}

Record CasacRegionManager::fromBCS(
		String& diagnostics, uInt& nSelectedChannels, String& stokes,
		const Record  * const &regionPtr, const String& regionName,
		const String& chans, const StokesControl stokesControl,
		const String& box, const IPosition& imShape, const String& imageName
) {
	LogOrigin origin("CasacRegionManager", __FUNCTION__);
	Record regionRecord;
	if (! box.empty()) {
		if (regionPtr != 0 || ! regionName.empty()) {
			*_getLog()
				<< "box, regionPtr, and/or regionName cannot be simultaneously specified"
				<< LogIO::EXCEPTION;
		}
		if (box.freq(",") % 4 != 3) {
			*_getLog() << "box not specified correctly" << LogIO::EXCEPTION;
		}
		regionRecord = _fromBCS(
				diagnostics, nSelectedChannels, stokes,
				chans, stokesControl, box, imShape
		).toRecord("");
		*_getLog() << origin;
		*_getLog() << LogIO::NORMAL << "Using specified box(es) "
			<< box << LogIO::POST;
	}
	else if (regionPtr != 0) {
		if (! (regionName.empty() && chans.empty() && stokes.empty())) {
			*_getLog()
				<< "regionPtr and regionName, chans, and/or stokes cannot "
				<< "be simultaneously specified."
				<< LogIO::EXCEPTION;
		}
		_setRegion(regionRecord, diagnostics, regionPtr);
		*_getLog() << origin;
		*_getLog() << LogIO::NORMAL << "Set region from supplied region record"
				<< LogIO::POST;
		stokes = _stokesFromRecord(regionRecord, stokesControl, imShape);
	}
	else if (! regionName.empty()) {
		if (! chans.empty() || ! stokes.empty()) {
			*_getLog()
			<< "regionName and chans and/or stokes cannot "
			<< "be specified simultaneously"
			<< LogIO::EXCEPTION;
		}
		_setRegion(regionRecord, diagnostics, regionName, imShape, imageName);
		*_getLog() << origin;
		*_getLog() << LogIO::NORMAL << diagnostics << LogIO::POST;
		stokes = _stokesFromRecord(regionRecord, stokesControl, imShape);
	}
	else {
		vector<uInt> chanEndPts, polEndPts;
		regionRecord = _fromBCS(
			diagnostics, nSelectedChannels, stokes,
			chans, stokesControl, box, imShape
		).toRecord("");
		*_getLog() << origin;
		*_getLog() << LogIO::NORMAL << "No directional region specified. Using full positional plane."
				<< LogIO::POST;
		if (_getCsys()->hasSpectralAxis()) {
			if (chans.empty()) {
				*_getLog() << LogIO::NORMAL << "Using all spectral channels."
					<< LogIO::POST;
			}
			else {
				*_getLog() << LogIO::NORMAL << "Using channel range(s) "
					<< _pairsToString(chanEndPts) << LogIO::POST;
			}
		}
		if (_getCsys()->hasPolarizationCoordinate()) {
			if (stokes.empty()) {
				switch (stokesControl) {
				case USE_ALL_STOKES:
					*_getLog() << LogIO::NORMAL << "Using all polarizations " << LogIO::POST;
					break;
				case USE_FIRST_STOKES:
					*_getLog() << LogIO::NORMAL << "polarization "
					<< _getCsys()->stokesAtPixel(0) << LogIO::POST;
					break;
				default:
					break;
				}
			}
			else {
				*_getLog() << LogIO::NORMAL << "Using polarizations " << stokes << LogIO::POST;
			}
		}
	}
    return regionRecord;
}

Record CasacRegionManager::regionFromString(
	const CoordinateSystem& csys, const String& regionStr,
	const String& imageName, const IPosition& imShape
) {
	CasacRegionManager mgr(csys);
	Record reg;
	String diag;
	mgr._setRegion(reg, diag, regionStr, imShape, imageName);
	return reg;
}

void CasacRegionManager::_setRegion(
		Record& regionRecord, String& diagnostics,
		const Record* regionPtr
)  {
	// region record pointer provided
	regionRecord = *(regionPtr->clone());
	// set stokes from the region record
	diagnostics = "used provided region record";
}

void CasacRegionManager::_setRegion(
	Record& regionRecord, String& diagnostics,
	const String& regionName, const IPosition& imShape,
	const String& imageName
) {
	if (regionName.empty() && imageName.empty()) {
		regionRecord = Record();
		diagnostics = "No region string";
		return;
	}
	// region name provided
	const static Regex image("(.*)+:(.*)+");
	const static Regex regionText(
		"^[[:space:]]*[[:alpha:]]+[[:space:]]*\\[(.*)+,(.*)+\\]"
	);

	File myFile(regionName);
	if (myFile.exists()) {
		if (! myFile.isReadable()) {
			*_getLog() << "File " + regionName + " exists but is not readable."
				<< LogIO::EXCEPTION;
		}
		try {
			std::auto_ptr<Record> rec(readImageFile(regionName, ""));
			regionRecord = *rec;
			diagnostics = "Region read from binary region file " + regionName;
			return;
		}
		catch(AipsError x) {
		}
		try {
			RegionTextList annList(regionName, *_getCsys(), imShape);
			regionRecord = annList.regionAsRecord();
			diagnostics = "Region read from region text file " + regionName;
		}
		catch (AipsError x) {
			*_getLog() << LogIO::SEVERE << regionName
				<< " is neither a valid binary region file, or a valid region text file."
                << LogIO::EXCEPTION;
        }
	}
	else if (regionName.contains(regionText)) {
		try {
			RegionTextList annList(*_getCsys(), regionName, imShape);
			regionRecord = annList.regionAsRecord();
			diagnostics = "Region read from text string " + regionName;
		}
		catch (AipsError x) {
			*_getLog() << x.getMesg() << LogIO::EXCEPTION;
		}
	}
	else if (regionName.matches(image) || ! imageName.empty()) {
		ImageRegion imRegion;
		String imagename, region;
		if (regionName.matches(image)) {
			String res[2];
			casa::split(regionName, res, 2, ":");
			imagename = res[0];
			region = res[1];
		}
		else {
			// imageName is not empty if we get here
			imagename = imageName;
			region = regionName;
		}
		try {
			Record *myRec = tableToRecord(imagename, region);
			if (Table::isReadable(imagename)) {
				if (myRec == 0) {
					*_getLog() << "Region " << region << " not found in image "
							<< imagename << LogIO::EXCEPTION;
				}
				regionRecord = *myRec;
				diagnostics = "Used region " + region + " from image "
						+ imagename + " table description";
			}
			else {
				*_getLog() << "Cannot read image " << imagename
						<< " to get region " << region << LogIO::EXCEPTION;
			}
		}
		catch (AipsError) {
			*_getLog() << "Unable to open region file or region table description "
					<< region << " in image " << imagename << LogIO::EXCEPTION;
		}
	}
	else {
		*_getLog() << "Unable to open region file or region table description "
			<< regionName << "." << endl
			<< "If it is supposed to be a text string its format is incorrect"
			<< LogIO::EXCEPTION;
	}
}

ImageRegion CasacRegionManager::_fromBCS(
	String& diagnostics, uInt& nSelectedChannels, String& stokes,
	const String& chans,
	const StokesControl stokesControl, const String& box,
	const IPosition& imShape
) const {
	Int specAxisNumber = _getCsys()->spectralAxisNumber();
	uInt nTotalChannels = specAxisNumber >= 0 ? imShape[specAxisNumber] : 0;
	vector<uInt> chanEndPts = setSpectralRanges(
		chans, nSelectedChannels, nTotalChannels
	);
    Int polAxisNumber = _getCsys()->polarizationAxisNumber();
	uInt nTotalPolarizations = polAxisNumber >= 0 ? imShape[polAxisNumber] : 0;
	String firstStokes = polAxisNumber >= 0 ? _getCsys()->stokesAtPixel(0) : "";
	vector<uInt> polEndPts = _setPolarizationRanges(
		stokes, firstStokes,
		nTotalPolarizations, stokesControl
	);
	vector<Double> boxCorners;
	if (box.empty()) {
		if (
			_getCsys()->hasDirectionCoordinate()
			|| _getCsys()->hasLinearCoordinate()
		) {
			Vector<Int> dirAxesNumbers;
			if (_getCsys()->hasDirectionCoordinate()) {
				dirAxesNumbers = _getCsys()->directionAxesNumbers();
			}
			else {
				dirAxesNumbers = _getCsys()->linearAxesNumbers();
			}
			Vector<Int> dirShape(2);
			dirShape[0] = imShape[dirAxesNumbers[0]];
			dirShape[1] = imShape[dirAxesNumbers[1]];
			boxCorners.resize(4);
			boxCorners[0] = 0;
			boxCorners[1] = 0;
			boxCorners[2] = dirShape[0] - 1;
			boxCorners[3] = dirShape[1] - 1;
		}
	}
	else {
		boxCorners = _setBoxCorners(box);
	}
	return _fromBCS(
		diagnostics, boxCorners,
		chanEndPts, polEndPts, imShape
	);
}

ImageRegion CasacRegionManager::_fromBCS(
		String& diagnostics, const vector<Double>& boxCorners,
		const vector<uInt>& chanEndPts, const vector<uInt>& polEndPts,
		const IPosition imShape
) const {
	LogOrigin origin("CasacRegionManager", __FUNCTION__);
	*_getLog() << origin;
	Vector<Double> blc(imShape.nelements(), 0);
	Vector<Double> trc(imShape.nelements(), 0);
	Vector<Int> directionAxisNumbers = _getCsys()->directionAxesNumbers();
	Vector<Int> linearAxisNumbers = _getCsys()->linearAxesNumbers();

	Int spectralAxisNumber = _getCsys()->spectralAxisNumber();
	Int polarizationAxisNumber = _getCsys()->polarizationAxisNumber();

	Vector<Double> xCorners(boxCorners.size()/2);
	Vector<Double> yCorners(xCorners.size());
	for (uInt i=0; i<xCorners.size(); i++) {
		Double x = boxCorners[2*i];
		Double y = boxCorners[2*i + 1];

		if (x < 0 || y < 0 ) {
			*_getLog() << "blc in box spec is less than 0" << LogIO::EXCEPTION;
		}
		if (_getCsys()->hasDirectionCoordinate()) {
			if (
				x >= imShape[directionAxisNumbers[0]]
				|| y >= imShape[directionAxisNumbers[1]]
			) {
				*_getLog() << "dAxisNum0=" <<directionAxisNumbers[0] <<" dAxisNum1="<<directionAxisNumbers[1];
				*_getLog() << "x="<<x<<" imShape[0]="<<imShape[directionAxisNumbers[0]]<< " y="<<y<<" imShape[1]="<<imShape[directionAxisNumbers[1]]<<LogIO::POST;
				*_getLog() << "trc in box spec is greater than or equal to number "
					<< "of direction coordinate pixels in the image" << LogIO::EXCEPTION;
			}
		}
		else if (
			_getCsys()->hasLinearCoordinate()
			&& (
				x >= imShape[linearAxisNumbers[0]]
				|| y >= imShape[linearAxisNumbers[1]]
			)
		) {
			*_getLog() << "trc in box spec is greater than or equal to number "
				<< "of linear coordinate pixels in the image" << LogIO::EXCEPTION;
		}
		xCorners[i] = x;
		yCorners[i] = y;
	}
	Vector<Double> polEndPtsDouble(polEndPts.size());
	for (uInt i=0; i<polEndPts.size(); i++) {
		polEndPtsDouble[i] = (Double)polEndPts[i];
	}

	Vector<Double> chanEndPtsDouble(chanEndPts.size());
	for (uInt i=0; i<chanEndPts.size(); i++) {
		chanEndPtsDouble[i] = (Double)chanEndPts[i];
	}
	uInt nRegions = 1;
	if (_getCsys()->hasDirectionCoordinate())  {
		nRegions *= boxCorners.size()/4;
	}
	if (_getCsys()->hasLinearCoordinate())  {
		nRegions *= boxCorners.size()/4;
	}
	if (_getCsys()->hasPolarizationCoordinate()) {
		nRegions *= polEndPts.size()/2;
	}
	if (_getCsys()->hasSpectralAxis()) {
		nRegions *= chanEndPts.size()/2;
	}
	Vector<Double> extXCorners(2*nRegions);
	Vector<Double> extYCorners(2*nRegions);
	Vector<Double> extPolEndPts(2*nRegions);
	Vector<Double> extChanEndPts(2*nRegions);

	uInt count = 0;
	for (uInt i=0; i<max(uInt(1), xCorners.size()/2); i++) {
		for (uInt j=0; j<max((uInt)1, polEndPts.size()/2); j++) {
			for (uInt k=0; k<max(uInt(1), chanEndPts.size()/2); k++) {
				if (
					_getCsys()->hasDirectionCoordinate()
					|| _getCsys()->hasLinearCoordinate()
				) {
					extXCorners[2*count] = xCorners[2*i];
					extXCorners[2*count + 1] = xCorners[2*i + 1];
					extYCorners[2*count] = yCorners[2*i];
					extYCorners[2*count + 1] = yCorners[2*i + 1];
				}
				if (_getCsys()->hasPolarizationCoordinate()) {
					extPolEndPts[2*count] = polEndPtsDouble[2*j];
					extPolEndPts[2*count + 1] = polEndPtsDouble[2*j + 1];
				}
				if (_getCsys()->hasSpectralAxis()) {
					extChanEndPts[2*count] = chanEndPtsDouble[2*k];
					extChanEndPts[2*count + 1] = chanEndPtsDouble[2*k + 1];
				}
				count++;
			}
		}
	}
	map<uInt, Vector<Double> > axisCornerMap;
	for (uInt i=0; i<nRegions; i++) {
		for (uInt axisNumber=0; axisNumber<_getCsys()->nPixelAxes(); axisNumber++) {
			if (
				(
					directionAxisNumbers.size() > 1
					&& (Int)axisNumber == directionAxisNumbers[0]
				)
				|| (
					! _getCsys()->hasDirectionCoordinate()
					&& linearAxisNumbers.size() > 1
					&& (Int)axisNumber == linearAxisNumbers[0]
				)
			) {
				axisCornerMap[axisNumber] = extXCorners;
			}
			else if (
				(
					directionAxisNumbers.size() > 1
					&& (Int)axisNumber == directionAxisNumbers[1]
				)
				|| (
					! _getCsys()->hasDirectionCoordinate()
					&& linearAxisNumbers.size() > 1
					&& (Int)axisNumber == linearAxisNumbers[1]
				)
			) {
				axisCornerMap[axisNumber] = extYCorners;
			}
			else if ((Int)axisNumber == spectralAxisNumber) {
				axisCornerMap[axisNumber] = extChanEndPts;
			}
			else if ((Int)axisNumber == polarizationAxisNumber) {
				axisCornerMap[axisNumber] = extPolEndPts;
			}
			else {
				Vector<Double> range(2, 0);
				range[1] = imShape[axisNumber] - 1;
				axisCornerMap[axisNumber] = range;
				/*
				*_getLog() << "Unhandled image axis number " << axisNumber
					<< LogIO::EXCEPTION;
				*/
			}
		}
	}
	ImageRegion imRegion;
	for (uInt i=0; i<nRegions; i++) {
		for (uInt axisNumber=0; axisNumber<_getCsys()->nPixelAxes(); axisNumber++) {
			blc(axisNumber) = axisCornerMap[axisNumber][2*i];
			trc(axisNumber) = axisCornerMap[axisNumber][2*i + 1];
		}
		LCBox lcBox(blc, trc, imShape);
		WCBox wcBox(lcBox, *_getCsys());
		ImageRegion thisRegion(wcBox);
		imRegion = (i == 0)
  			? thisRegion
  			: imRegion = *(doUnion(imRegion, thisRegion));
	}
	ostringstream os;
	os << "Used image region from " << endl;
	if (_getCsys()->hasDirectionCoordinate()) {
		os << "    position box corners: ";
		for (uInt i=0; i<boxCorners.size()/4; i++) {
			os << boxCorners[4*i] << ", " << boxCorners[4*i + 1]
			    << ", " << boxCorners[4*i + 2] << ", " << boxCorners[4*i + 3];
			if (i < boxCorners.size()/4 - 1) {
				os << "; ";
			}
		}
	}
	if (_getCsys()->hasSpectralAxis()) {
		os << "    spectral channel ranges: " << _pairsToString(chanEndPts);
	}
	if (_getCsys()->hasPolarizationCoordinate()) {
		os << "    polarization pixel ranges: " << _pairsToString(polEndPts);
	}
	diagnostics = os.str();
	return imRegion;
}

String CasacRegionManager::_pairsToString(const vector<uInt>& pairs) const {
	ostringstream os;
	uInt nPairs = pairs.size()/2;
	for (uInt i=0; i<nPairs; i++) {
		os << pairs[2*i] << " - " << pairs[2*i + 1];
		if (i < nPairs - 1) {
			os << "; ";
		}
	}
	return os.str();
}

String CasacRegionManager::_stokesFromRecord(
		const Record& region, const StokesControl stokesControl, const IPosition& shape
) const {
	// FIXME This implementation is incorrect for complex, recursive records
	String stokes = "";
	if(! _getCsys()->hasPolarizationCoordinate()) {
		return stokes;
	}
	Int polAxis = _getCsys()->polarizationAxisNumber();
	if (shape[polAxis] == 1) {
		// degenerate stokes axis
		return _getCsys()->stokesAtPixel(0);
	}
	uInt stokesBegin = 0;
	uInt stokesEnd = 0;
	ImageRegion *imreg = ImageRegion::fromRecord(region, "");
	Array<Float> blc, trc;
	Bool oneRelAccountedFor = False;
	if (imreg->isLCSlicer()) {
		blc = imreg->asLCSlicer().blc();
        if ((Int)blc.size() <= polAxis) {
            *_getLog() << LogIO::WARN << "Cannot determine stokes. "
               << "blc of input region does not include the polarization coordinate."
               << LogIO::POST;
            return stokes;
        }
		trc = imreg->asLCSlicer().trc();
        if ((Int)trc.size() <= polAxis) {
            *_getLog() << LogIO::WARN << "Cannot determine stokes. "
               << "trc of input region does not include the polarization coordinate."
               << LogIO::POST;
            return stokes;
        } 
		stokesBegin = (uInt)((Vector<Float>)blc)[polAxis];
		stokesEnd = (uInt)((Vector<Float>)trc)[polAxis];
		oneRelAccountedFor = True;
	}
	else if (
		RegionManager::isPixelRegion(
			*(ImageRegion::fromRecord(region, ""))
		)
	) {
		region.toArray("blc", blc);
		region.toArray("trc", trc);
		stokesBegin = (uInt)((Vector<Float>)blc)[polAxis];
		stokesEnd = (uInt)((Vector<Float>)trc)[polAxis];
	}
	else if (region.fieldNumber("x") >= 0 && region.fieldNumber("y") >= 0) {
		// world polygon
		oneRelAccountedFor = True;
		stokesBegin = 0;
		stokesEnd = stokesControl == USE_FIRST_STOKES
			? 0 : shape[polAxis];
	}
	else if (region.fieldNumber("blc") >= 0 && region.fieldNumber("blc") >= 0) {
		// world box
		Record blcRec = region.asRecord("blc");
		Record trcRec = region.asRecord("trc");
		String polField = "*" + String::toString(polAxis + 1);
		stokesBegin = blcRec.isDefined(polField)
			? (Int)blcRec.asRecord(
				String(polField)
			).asDouble("value")
			: 0;
		stokesEnd = trcRec.isDefined(polField)
			? (Int)blcRec.asRecord(
				String(polField)
			).asDouble("value")
			: stokesControl == USE_FIRST_STOKES
				? 0
				: shape[polAxis];
		if (! blcRec.isDefined(polField)) {
			oneRelAccountedFor = True;
		}
	}
	else {
		// FIXME not very nice, but until all can be implemented this will have to do
		*_getLog() << LogIO::WARN << "Stokes cannot be determined because this "
			<< "region type is not handled yet. But chances are very good this is no need to be alarmed."
			<< LogIO::POST;
		return stokes;
	}
	if (
		! oneRelAccountedFor
		&& region.isDefined("oneRel")
		&& region.asBool("oneRel")
	) {
		stokesBegin--;
		stokesEnd--;
	}
	for (uInt i=stokesBegin; i<=stokesEnd; i++) {
		stokes += _getCsys()->stokesAtPixel(i);
	}
	return stokes;
}

vector<uInt> CasacRegionManager::setSpectralRanges(
	String specification, uInt& nSelectedChannels, const uInt nChannels
) const {
	LogOrigin origin("CasacRegionManager", __FUNCTION__);
	*_getLog() << origin;

	vector<uInt> ranges(0);
	if (! _getCsys()->hasSpectralAxis()) {
		nSelectedChannels = 0;
		return ranges;
	}

	specification.trim();
	specification.upcase();

	if (specification.empty() || specification == ALL) {
		ranges.push_back(0);
		ranges.push_back(nChannels - 1);
		nSelectedChannels = nChannels;
		return ranges;
	}

	// First split on commas
	Vector<String> parts = stringToVector(specification, Regex("[,;]"));
	Regex regexuInt("^[0-9]+$");
	Regex regexRange("^[0-9]+[ \n\t\r\v\f]*~[ \n\t\r\v\f]*[0-9]+$");
	Regex regexLT("^<.*$");
	Regex regexLTEq("^<=.*$");
	Regex regexGT("^>.*$");
	Regex regexGTEq("^>=.*$");

	for (uInt i=0; i<parts.size(); i++) {
		parts[i].trim();
		uInt min = 0;
		uInt max = 0;
		if (parts[i].matches(regexuInt)) {
			// just one channel
			min = String::toInt(parts[i]);
			max = min;
		}
		else if(parts[i].matches(regexRange)) {
			// a range of channels
			Vector<String> values = stringToVector(parts[i], '~');
			if (values.size() != 2) {
				*_getLog() << "Incorrect specification for channel range "
					<< parts[i] << LogIO::EXCEPTION;
			}
			values[0].trim();
			values[1].trim();
			for(uInt j=0; j < 2; j++) {
				if (! values[j].matches(regexuInt)) {
					*_getLog() << "For channel specification " << values[j]
					    << " is not a non-negative integer in "
					    << parts[i] << LogIO::EXCEPTION;
				}
			}
			min = String::toInt(values[0]);
			max = String::toInt(values[1]);
		}
		else if (parts[i].matches(regexLT)) {
			String maxs = parts[i].matches(regexLTEq) ? parts[i].substr(2) : parts[i].substr(1);
			maxs.trim();
			if (! maxs.matches(regexuInt)) {
				*_getLog() << "In channel specification, " << maxs
					<< " is not a non-negative integer in " << parts[i]
					<< LogIO::EXCEPTION;
			}
			min = 0;
			max = String::toInt(maxs);
			if (! parts[i].matches(regexLTEq)) {
				if (max == 0) {
					*_getLog() << "In channel specification, max channel cannot "
						<< "be less than zero in " + parts[i];
				}
				else {
					max--;
				}
			}
		}
		else if (parts[i].matches(regexGT)) {
			String mins = parts[i].matches(regexGTEq)
         		? parts[i].substr(2)
         		: parts[i].substr(1);
			mins.trim();
			if (! mins.matches(regexuInt)) {
				*_getLog() << " In channel specification, " << mins
					<< " is not an integer in " << parts[i]
					<< LogIO::EXCEPTION;
			}
			max = nChannels - 1;
			min = String::toInt(mins);
			if(! parts[i].matches(regexGTEq)) {
				min++;
			}
			if (min > nChannels - 1) {
				*_getLog() << "Min channel cannot be greater than the (zero-based) number of channels ("
					<< nChannels - 1 << ") in the image" << LogIO::EXCEPTION;
			}
		}
		else {
			*_getLog() << "Invalid channel specification in " << parts[i]
			    << " of spec " << specification << LogIO::EXCEPTION;
		}
		if (min > max) {
			*_getLog() << "Min channel " << min << " cannot be greater than max channel "
				<< max << " in " << parts[i] << LogIO::EXCEPTION;
		}
		else if (max >= nChannels) {
			*_getLog() << "Zero-based max channel " << max
				<< " must be less than the total number of channels ("
				<< nChannels << ") in the channel specification " << parts[i] << LogIO::EXCEPTION;
		}
		ranges.push_back(min);
		ranges.push_back(max);
	}
	vector<uInt> consolidatedRanges = consolidateAndOrderRanges(ranges);
	nSelectedChannels = 0;
	for (uInt i=0; i<consolidatedRanges.size()/2; i++) {
		nSelectedChannels += consolidatedRanges[2*i + 1] - consolidatedRanges[2*i] + 1;
	}
	return consolidatedRanges;
}

} // end of  casa namespace
