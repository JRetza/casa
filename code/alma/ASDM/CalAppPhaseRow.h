
/*
 * ALMA - Atacama Large Millimeter Array
 * (c) European Southern Observatory, 2002
 * (c) Associated Universities Inc., 2002
 * Copyright by ESO (in the framework of the ALMA collaboration),
 * Copyright by AUI (in the framework of the ALMA collaboration),
 * All rights reserved.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307  USA
 *
 * Warning!
 *  -------------------------------------------------------------------- 
 * | This is generated code!  Do not modify this file.                  |
 * | If you do, all changes will be lost when the file is re-generated. |
 *  --------------------------------------------------------------------
 *
 * casacore::File CalAppPhaseRow.h
 */
 
#ifndef CalAppPhaseRow_CLASS
#define CalAppPhaseRow_CLASS

#include <vector>
#include <string>
#include <set>

#ifndef WITHOUT_ACS
#include <asdmIDLC.h>
#endif






	 
#include <ArrayTime.h>
	

	 
#include <Tag.h>
	




	
#include "CBasebandName.h"
	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	



#include <ConversionException.h>
#include <NoSuchRow.h>
#include <IllegalAccessException.h>

#include <RowTransformer.h>
//#include <TableStreamReader.h>

/*\file CalAppPhase.h
    \brief Generated from model's revision "-1", branch ""
*/

namespace asdm {

//class asdm::CalAppPhaseTable;


// class asdm::CalDataRow;
class CalDataRow;

// class asdm::CalReductionRow;
class CalReductionRow;
	

class CalAppPhaseRow;
typedef void (CalAppPhaseRow::*CalAppPhaseAttributeFromBin) (EndianIStream& eis);
typedef void (CalAppPhaseRow::*CalAppPhaseAttributeFromText) (const string& s);

/**
 * The CalAppPhaseRow class is a row of a CalAppPhaseTable.
 * 
 * Generated from model's revision "-1", branch ""
 *
 */
class CalAppPhaseRow {
friend class asdm::CalAppPhaseTable;
friend class asdm::RowTransformer<CalAppPhaseRow>;
//friend class asdm::TableStreamReader<CalAppPhaseTable, CalAppPhaseRow>;

public:

	virtual ~CalAppPhaseRow();

	/**
	 * Return the table to which this row belongs.
	 */
	CalAppPhaseTable &getTable() const;
	
	/**
	 * Has this row been added to its table ?
	 * @return true if and only if it has been added.
	 */
	bool isAdded() const;
		
	////////////////////////////////
	// Intrinsic casacore::Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute basebandName
	
	
	

	
 	/**
 	 * Get basebandName.
 	 * @return basebandName as BasebandNameMod::BasebandName
 	 */
 	BasebandNameMod::BasebandName getBasebandName() const;
	
 
 	
 	
 	/**
 	 * Set basebandName with the specified BasebandNameMod::BasebandName.
 	 * @param basebandName The BasebandNameMod::BasebandName value to which basebandName is to be set.
 	 
 		
 			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setBasebandName (BasebandNameMod::BasebandName basebandName);
  		
	
	
	


	
	// ===> Attribute scanNumber
	
	
	

	
 	/**
 	 * Get scanNumber.
 	 * @return scanNumber as int
 	 */
 	int getScanNumber() const;
	
 
 	
 	
 	/**
 	 * Set scanNumber with the specified int.
 	 * @param scanNumber The int value to which scanNumber is to be set.
 	 
 		
 			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setScanNumber (int scanNumber);
  		
	
	
	


	
	// ===> Attribute startValidTime
	
	
	

	
 	/**
 	 * Get startValidTime.
 	 * @return startValidTime as ArrayTime
 	 */
 	ArrayTime getStartValidTime() const;
	
 
 	
 	
 	/**
 	 * Set startValidTime with the specified ArrayTime.
 	 * @param startValidTime The ArrayTime value to which startValidTime is to be set.
 	 
 		
 			
 	 */
 	void setStartValidTime (ArrayTime startValidTime);
  		
	
	
	


	
	// ===> Attribute endValidTime
	
	
	

	
 	/**
 	 * Get endValidTime.
 	 * @return endValidTime as ArrayTime
 	 */
 	ArrayTime getEndValidTime() const;
	
 
 	
 	
 	/**
 	 * Set endValidTime with the specified ArrayTime.
 	 * @param endValidTime The ArrayTime value to which endValidTime is to be set.
 	 
 		
 			
 	 */
 	void setEndValidTime (ArrayTime endValidTime);
  		
	
	
	


	
	// ===> Attribute adjustTime
	
	
	

	
 	/**
 	 * Get adjustTime.
 	 * @return adjustTime as ArrayTime
 	 */
 	ArrayTime getAdjustTime() const;
	
 
 	
 	
 	/**
 	 * Set adjustTime with the specified ArrayTime.
 	 * @param adjustTime The ArrayTime value to which adjustTime is to be set.
 	 
 		
 			
 	 */
 	void setAdjustTime (ArrayTime adjustTime);
  		
	
	
	


	
	// ===> Attribute adjustToken
	
	
	

	
 	/**
 	 * Get adjustToken.
 	 * @return adjustToken as string
 	 */
 	string getAdjustToken() const;
	
 
 	
 	
 	/**
 	 * Set adjustToken with the specified string.
 	 * @param adjustToken The string value to which adjustToken is to be set.
 	 
 		
 			
 	 */
 	void setAdjustToken (string adjustToken);
  		
	
	
	


	
	// ===> Attribute phasingMode
	
	
	

	
 	/**
 	 * Get phasingMode.
 	 * @return phasingMode as string
 	 */
 	string getPhasingMode() const;
	
 
 	
 	
 	/**
 	 * Set phasingMode with the specified string.
 	 * @param phasingMode The string value to which phasingMode is to be set.
 	 
 		
 			
 	 */
 	void setPhasingMode (string phasingMode);
  		
	
	
	


	
	// ===> Attribute numPhasedAntennas
	
	
	

	
 	/**
 	 * Get numPhasedAntennas.
 	 * @return numPhasedAntennas as int
 	 */
 	int getNumPhasedAntennas() const;
	
 
 	
 	
 	/**
 	 * Set numPhasedAntennas with the specified int.
 	 * @param numPhasedAntennas The int value to which numPhasedAntennas is to be set.
 	 
 		
 			
 	 */
 	void setNumPhasedAntennas (int numPhasedAntennas);
  		
	
	
	


	
	// ===> Attribute phasedAntennas
	
	
	

	
 	/**
 	 * Get phasedAntennas.
 	 * @return phasedAntennas as vector<string >
 	 */
 	vector<string > getPhasedAntennas() const;
	
 
 	
 	
 	/**
 	 * Set phasedAntennas with the specified vector<string >.
 	 * @param phasedAntennas The vector<string > value to which phasedAntennas is to be set.
 	 
 		
 			
 	 */
 	void setPhasedAntennas (vector<string > phasedAntennas);
  		
	
	
	


	
	// ===> Attribute refAntennaIndex
	
	
	

	
 	/**
 	 * Get refAntennaIndex.
 	 * @return refAntennaIndex as int
 	 */
 	int getRefAntennaIndex() const;
	
 
 	
 	
 	/**
 	 * Set refAntennaIndex with the specified int.
 	 * @param refAntennaIndex The int value to which refAntennaIndex is to be set.
 	 
 		
 			
 	 */
 	void setRefAntennaIndex (int refAntennaIndex);
  		
	
	
	


	
	// ===> Attribute candRefAntennaIndex
	
	
	

	
 	/**
 	 * Get candRefAntennaIndex.
 	 * @return candRefAntennaIndex as int
 	 */
 	int getCandRefAntennaIndex() const;
	
 
 	
 	
 	/**
 	 * Set candRefAntennaIndex with the specified int.
 	 * @param candRefAntennaIndex The int value to which candRefAntennaIndex is to be set.
 	 
 		
 			
 	 */
 	void setCandRefAntennaIndex (int candRefAntennaIndex);
  		
	
	
	


	
	// ===> Attribute phasePacking
	
	
	

	
 	/**
 	 * Get phasePacking.
 	 * @return phasePacking as string
 	 */
 	string getPhasePacking() const;
	
 
 	
 	
 	/**
 	 * Set phasePacking with the specified string.
 	 * @param phasePacking The string value to which phasePacking is to be set.
 	 
 		
 			
 	 */
 	void setPhasePacking (string phasePacking);
  		
	
	
	


	
	// ===> Attribute numReceptors
	
	
	

	
 	/**
 	 * Get numReceptors.
 	 * @return numReceptors as int
 	 */
 	int getNumReceptors() const;
	
 
 	
 	
 	/**
 	 * Set numReceptors with the specified int.
 	 * @param numReceptors The int value to which numReceptors is to be set.
 	 
 		
 			
 	 */
 	void setNumReceptors (int numReceptors);
  		
	
	
	


	
	// ===> Attribute numChannels
	
	
	

	
 	/**
 	 * Get numChannels.
 	 * @return numChannels as int
 	 */
 	int getNumChannels() const;
	
 
 	
 	
 	/**
 	 * Set numChannels with the specified int.
 	 * @param numChannels The int value to which numChannels is to be set.
 	 
 		
 			
 	 */
 	void setNumChannels (int numChannels);
  		
	
	
	


	
	// ===> Attribute numPhaseValues
	
	
	

	
 	/**
 	 * Get numPhaseValues.
 	 * @return numPhaseValues as int
 	 */
 	int getNumPhaseValues() const;
	
 
 	
 	
 	/**
 	 * Set numPhaseValues with the specified int.
 	 * @param numPhaseValues The int value to which numPhaseValues is to be set.
 	 
 		
 			
 	 */
 	void setNumPhaseValues (int numPhaseValues);
  		
	
	
	


	
	// ===> Attribute phaseValues
	
	
	

	
 	/**
 	 * Get phaseValues.
 	 * @return phaseValues as vector<float >
 	 */
 	vector<float > getPhaseValues() const;
	
 
 	
 	
 	/**
 	 * Set phaseValues with the specified vector<float >.
 	 * @param phaseValues The vector<float > value to which phaseValues is to be set.
 	 
 		
 			
 	 */
 	void setPhaseValues (vector<float > phaseValues);
  		
	
	
	


	
	// ===> Attribute numCompare
	
	
	

	
 	/**
 	 * Get numCompare.
 	 * @return numCompare as int
 	 */
 	int getNumCompare() const;
	
 
 	
 	
 	/**
 	 * Set numCompare with the specified int.
 	 * @param numCompare The int value to which numCompare is to be set.
 	 
 		
 			
 	 */
 	void setNumCompare (int numCompare);
  		
	
	
	


	
	// ===> Attribute numEfficiencies
	
	
	

	
 	/**
 	 * Get numEfficiencies.
 	 * @return numEfficiencies as int
 	 */
 	int getNumEfficiencies() const;
	
 
 	
 	
 	/**
 	 * Set numEfficiencies with the specified int.
 	 * @param numEfficiencies The int value to which numEfficiencies is to be set.
 	 
 		
 			
 	 */
 	void setNumEfficiencies (int numEfficiencies);
  		
	
	
	


	
	// ===> Attribute compareArray
	
	
	

	
 	/**
 	 * Get compareArray.
 	 * @return compareArray as vector<string >
 	 */
 	vector<string > getCompareArray() const;
	
 
 	
 	
 	/**
 	 * Set compareArray with the specified vector<string >.
 	 * @param compareArray The vector<string > value to which compareArray is to be set.
 	 
 		
 			
 	 */
 	void setCompareArray (vector<string > compareArray);
  		
	
	
	


	
	// ===> Attribute efficiencyIndices
	
	
	

	
 	/**
 	 * Get efficiencyIndices.
 	 * @return efficiencyIndices as vector<int >
 	 */
 	vector<int > getEfficiencyIndices() const;
	
 
 	
 	
 	/**
 	 * Set efficiencyIndices with the specified vector<int >.
 	 * @param efficiencyIndices The vector<int > value to which efficiencyIndices is to be set.
 	 
 		
 			
 	 */
 	void setEfficiencyIndices (vector<int > efficiencyIndices);
  		
	
	
	


	
	// ===> Attribute efficiencies
	
	
	

	
 	/**
 	 * Get efficiencies.
 	 * @return efficiencies as vector<vector<float > >
 	 */
 	vector<vector<float > > getEfficiencies() const;
	
 
 	
 	
 	/**
 	 * Set efficiencies with the specified vector<vector<float > >.
 	 * @param efficiencies The vector<vector<float > > value to which efficiencies is to be set.
 	 
 		
 			
 	 */
 	void setEfficiencies (vector<vector<float > > efficiencies);
  		
	
	
	


	
	// ===> Attribute quality
	
	
	

	
 	/**
 	 * Get quality.
 	 * @return quality as vector<float >
 	 */
 	vector<float > getQuality() const;
	
 
 	
 	
 	/**
 	 * Set quality with the specified vector<float >.
 	 * @param quality The vector<float > value to which quality is to be set.
 	 
 		
 			
 	 */
 	void setQuality (vector<float > quality);
  		
	
	
	


	
	// ===> Attribute phasedSumAntenna
	
	
	

	
 	/**
 	 * Get phasedSumAntenna.
 	 * @return phasedSumAntenna as string
 	 */
 	string getPhasedSumAntenna() const;
	
 
 	
 	
 	/**
 	 * Set phasedSumAntenna with the specified string.
 	 * @param phasedSumAntenna The string value to which phasedSumAntenna is to be set.
 	 
 		
 			
 	 */
 	void setPhasedSumAntenna (string phasedSumAntenna);
  		
	
	
	


	
	// ===> Attribute typeSupports, which is optional
	
	
	
	/**
	 * The attribute typeSupports is optional. Return true if this attribute exists.
	 * @return true if and only if the typeSupports attribute exists. 
	 */
	bool isTypeSupportsExists() const;
	

	
 	/**
 	 * Get typeSupports, which is optional.
 	 * @return typeSupports as string
 	 * @throws IllegalAccessException If typeSupports does not exist.
 	 */
 	string getTypeSupports() const;
	
 
 	
 	
 	/**
 	 * Set typeSupports with the specified string.
 	 * @param typeSupports The string value to which typeSupports is to be set.
 	 
 		
 	 */
 	void setTypeSupports (string typeSupports);
		
	
	
	
	/**
	 * Mark typeSupports, which is an optional field, as non-existent.
	 */
	void clearTypeSupports ();
	


	
	// ===> Attribute numSupports, which is optional
	
	
	
	/**
	 * The attribute numSupports is optional. Return true if this attribute exists.
	 * @return true if and only if the numSupports attribute exists. 
	 */
	bool isNumSupportsExists() const;
	

	
 	/**
 	 * Get numSupports, which is optional.
 	 * @return numSupports as int
 	 * @throws IllegalAccessException If numSupports does not exist.
 	 */
 	int getNumSupports() const;
	
 
 	
 	
 	/**
 	 * Set numSupports with the specified int.
 	 * @param numSupports The int value to which numSupports is to be set.
 	 
 		
 	 */
 	void setNumSupports (int numSupports);
		
	
	
	
	/**
	 * Mark numSupports, which is an optional field, as non-existent.
	 */
	void clearNumSupports ();
	


	
	// ===> Attribute phaseSupports, which is optional
	
	
	
	/**
	 * The attribute phaseSupports is optional. Return true if this attribute exists.
	 * @return true if and only if the phaseSupports attribute exists. 
	 */
	bool isPhaseSupportsExists() const;
	

	
 	/**
 	 * Get phaseSupports, which is optional.
 	 * @return phaseSupports as vector<float >
 	 * @throws IllegalAccessException If phaseSupports does not exist.
 	 */
 	vector<float > getPhaseSupports() const;
	
 
 	
 	
 	/**
 	 * Set phaseSupports with the specified vector<float >.
 	 * @param phaseSupports The vector<float > value to which phaseSupports is to be set.
 	 
 		
 	 */
 	void setPhaseSupports (vector<float > phaseSupports);
		
	
	
	
	/**
	 * Mark phaseSupports, which is an optional field, as non-existent.
	 */
	void clearPhaseSupports ();
	


	////////////////////////////////
	// Extrinsic casacore::Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute calDataId
	
	
	

	
 	/**
 	 * Get calDataId.
 	 * @return calDataId as Tag
 	 */
 	Tag getCalDataId() const;
	
 
 	
 	
 	/**
 	 * Set calDataId with the specified Tag.
 	 * @param calDataId The Tag value to which calDataId is to be set.
 	 
 		
 			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setCalDataId (Tag calDataId);
  		
	
	
	


	
	// ===> Attribute calReductionId
	
	
	

	
 	/**
 	 * Get calReductionId.
 	 * @return calReductionId as Tag
 	 */
 	Tag getCalReductionId() const;
	
 
 	
 	
 	/**
 	 * Set calReductionId with the specified Tag.
 	 * @param calReductionId The Tag value to which calReductionId is to be set.
 	 
 		
 			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setCalReductionId (Tag calReductionId);
  		
	
	
	


	///////////
	// Links //
	///////////
	
	

	
		
	/**
	 * calDataId pointer to the row in the CalData table having CalData.calDataId == calDataId
	 * @return a CalDataRow*
	 * 
	 
	 */
	 CalDataRow* getCalDataUsingCalDataId();
	 

	

	

	
		
	/**
	 * calReductionId pointer to the row in the CalReduction table having CalReduction.calReductionId == calReductionId
	 * @return a CalReductionRow*
	 * 
	 
	 */
	 CalReductionRow* getCalReductionUsingCalReductionId();
	 

	

	
	
	
	/**
	 * Compare each mandatory attribute except the autoincrementable one of this CalAppPhaseRow with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param basebandName
	    
	 * @param scanNumber
	    
	 * @param calDataId
	    
	 * @param calReductionId
	    
	 * @param startValidTime
	    
	 * @param endValidTime
	    
	 * @param adjustTime
	    
	 * @param adjustToken
	    
	 * @param phasingMode
	    
	 * @param numPhasedAntennas
	    
	 * @param phasedAntennas
	    
	 * @param refAntennaIndex
	    
	 * @param candRefAntennaIndex
	    
	 * @param phasePacking
	    
	 * @param numReceptors
	    
	 * @param numChannels
	    
	 * @param numPhaseValues
	    
	 * @param phaseValues
	    
	 * @param numCompare
	    
	 * @param numEfficiencies
	    
	 * @param compareArray
	    
	 * @param efficiencyIndices
	    
	 * @param efficiencies
	    
	 * @param quality
	    
	 * @param phasedSumAntenna
	    
	 */ 
	bool compareNoAutoInc(BasebandNameMod::BasebandName basebandName, int scanNumber, Tag calDataId, Tag calReductionId, ArrayTime startValidTime, ArrayTime endValidTime, ArrayTime adjustTime, string adjustToken, string phasingMode, int numPhasedAntennas, vector<string > phasedAntennas, int refAntennaIndex, int candRefAntennaIndex, string phasePacking, int numReceptors, int numChannels, int numPhaseValues, vector<float > phaseValues, int numCompare, int numEfficiencies, vector<string > compareArray, vector<int > efficiencyIndices, vector<vector<float > > efficiencies, vector<float > quality, string phasedSumAntenna);
	
	

	
	/**
	 * Compare each mandatory value (i.e. not in the key) attribute  with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param startValidTime
	    
	 * @param endValidTime
	    
	 * @param adjustTime
	    
	 * @param adjustToken
	    
	 * @param phasingMode
	    
	 * @param numPhasedAntennas
	    
	 * @param phasedAntennas
	    
	 * @param refAntennaIndex
	    
	 * @param candRefAntennaIndex
	    
	 * @param phasePacking
	    
	 * @param numReceptors
	    
	 * @param numChannels
	    
	 * @param numPhaseValues
	    
	 * @param phaseValues
	    
	 * @param numCompare
	    
	 * @param numEfficiencies
	    
	 * @param compareArray
	    
	 * @param efficiencyIndices
	    
	 * @param efficiencies
	    
	 * @param quality
	    
	 * @param phasedSumAntenna
	    
	 */ 
	bool compareRequiredValue(ArrayTime startValidTime, ArrayTime endValidTime, ArrayTime adjustTime, string adjustToken, string phasingMode, int numPhasedAntennas, vector<string > phasedAntennas, int refAntennaIndex, int candRefAntennaIndex, string phasePacking, int numReceptors, int numChannels, int numPhaseValues, vector<float > phaseValues, int numCompare, int numEfficiencies, vector<string > compareArray, vector<int > efficiencyIndices, vector<vector<float > > efficiencies, vector<float > quality, string phasedSumAntenna); 
		 
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *
	 * @param x a pointer on the CalAppPhaseRow whose required attributes of the value part 
	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool equalByRequiredValue(CalAppPhaseRow* x) ;
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a CalAppPhaseRowIDL struct.
	 */
	asdmIDL::CalAppPhaseRowIDL *toIDL() const;
	
	/**
	 * Define the content of a CalAppPhaseRowIDL struct from the values
	 * found in this row.
	 *
	 * @param x a reference to the CalAppPhaseRowIDL struct to be set.
	 *
	 */
	 void toIDL(asdmIDL::CalAppPhaseRowIDL& x) const;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct CalAppPhaseRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 * @throws ConversionException
	 */
	void setFromIDL (asdmIDL::CalAppPhaseRowIDL x) ;
#endif
	
	/**
	 * Return this row in the form of an XML string.
	 * @return The values of this row as an XML string.
	 */
	std::string toXML() const;

	/**
	 * Fill the values of this row from an XML string 
	 * that was produced by the toXML() method.
	 * @param rowDoc the XML string being used to set the values of this row.
	 * @throws ConversionException
	 */
	void setFromXML (std::string rowDoc) ;

	/// @cond DISPLAY_PRIVATE	
	////////////////////////////////////////////////////////////
	// binary-deserialization material from an EndianIStream  //
	////////////////////////////////////////////////////////////

	std::map<std::string, CalAppPhaseAttributeFromBin> fromBinMethods;
void basebandNameFromBin( EndianIStream& eis);
void scanNumberFromBin( EndianIStream& eis);
void calDataIdFromBin( EndianIStream& eis);
void calReductionIdFromBin( EndianIStream& eis);
void startValidTimeFromBin( EndianIStream& eis);
void endValidTimeFromBin( EndianIStream& eis);
void adjustTimeFromBin( EndianIStream& eis);
void adjustTokenFromBin( EndianIStream& eis);
void phasingModeFromBin( EndianIStream& eis);
void numPhasedAntennasFromBin( EndianIStream& eis);
void phasedAntennasFromBin( EndianIStream& eis);
void refAntennaIndexFromBin( EndianIStream& eis);
void candRefAntennaIndexFromBin( EndianIStream& eis);
void phasePackingFromBin( EndianIStream& eis);
void numReceptorsFromBin( EndianIStream& eis);
void numChannelsFromBin( EndianIStream& eis);
void numPhaseValuesFromBin( EndianIStream& eis);
void phaseValuesFromBin( EndianIStream& eis);
void numCompareFromBin( EndianIStream& eis);
void numEfficienciesFromBin( EndianIStream& eis);
void compareArrayFromBin( EndianIStream& eis);
void efficiencyIndicesFromBin( EndianIStream& eis);
void efficienciesFromBin( EndianIStream& eis);
void qualityFromBin( EndianIStream& eis);
void phasedSumAntennaFromBin( EndianIStream& eis);

void typeSupportsFromBin( EndianIStream& eis);
void numSupportsFromBin( EndianIStream& eis);
void phaseSupportsFromBin( EndianIStream& eis);


	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the CalAppPhaseTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.
	  */
	 static CalAppPhaseRow* fromBin(EndianIStream& eis, CalAppPhaseTable& table, const std::vector<std::string>& attributesSeq);	 
 
 	 /**
 	  * Parses a string t and assign the result of the parsing to the attribute of name attributeName.
 	  *
 	  * @param attributeName the name of the attribute whose value is going to be defined.
 	  * @param t the string to be parsed into a value given to the attribute of name attributeName.
 	  */
 	 void fromText(const std::string& attributeName, const std::string&  t);
     /// @endcond			

private:
	/**
	 * The table to which this row belongs.
	 */
	CalAppPhaseTable &table;
	/**
	 * Whether this row has been added to the table or not.
	 */
	bool hasBeenAdded;

	// This method is used by the casacore::Table class when this row is added to the table.
	void isAdded(bool added);


	/**
	 * Create a CalAppPhaseRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	CalAppPhaseRow (CalAppPhaseTable &table);

	/**
	 * Create a CalAppPhaseRow using a copy constructor mechanism.
	 * <p>
	 * Given a CalAppPhaseRow row and a CalAppPhaseTable table, the method creates a new
	 * CalAppPhaseRow owned by table. Each attribute of the created row is a copy (deep)
	 * of the corresponding attribute of row. The method does not add the created
	 * row to its table, its simply parents it to table, a call to the add method
	 * has to be done in order to get the row added (very likely after having modified
	 * some of its attributes).
	 * If row is null then the method returns a row with default values for its attributes. 
	 *
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 * @param row  The row which is to be copied.
	 */
	 CalAppPhaseRow (CalAppPhaseTable &table, CalAppPhaseRow &row);
	 	
	////////////////////////////////
	// Intrinsic casacore::Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute basebandName
	
	

	BasebandNameMod::BasebandName basebandName;

	
	
 	

	
	// ===> Attribute scanNumber
	
	

	int scanNumber;

	
	
 	

	
	// ===> Attribute startValidTime
	
	

	ArrayTime startValidTime;

	
	
 	

	
	// ===> Attribute endValidTime
	
	

	ArrayTime endValidTime;

	
	
 	

	
	// ===> Attribute adjustTime
	
	

	ArrayTime adjustTime;

	
	
 	

	
	// ===> Attribute adjustToken
	
	

	string adjustToken;

	
	
 	

	
	// ===> Attribute phasingMode
	
	

	string phasingMode;

	
	
 	

	
	// ===> Attribute numPhasedAntennas
	
	

	int numPhasedAntennas;

	
	
 	

	
	// ===> Attribute phasedAntennas
	
	

	vector<string > phasedAntennas;

	
	
 	

	
	// ===> Attribute refAntennaIndex
	
	

	int refAntennaIndex;

	
	
 	

	
	// ===> Attribute candRefAntennaIndex
	
	

	int candRefAntennaIndex;

	
	
 	

	
	// ===> Attribute phasePacking
	
	

	string phasePacking;

	
	
 	

	
	// ===> Attribute numReceptors
	
	

	int numReceptors;

	
	
 	

	
	// ===> Attribute numChannels
	
	

	int numChannels;

	
	
 	

	
	// ===> Attribute numPhaseValues
	
	

	int numPhaseValues;

	
	
 	

	
	// ===> Attribute phaseValues
	
	

	vector<float > phaseValues;

	
	
 	

	
	// ===> Attribute numCompare
	
	

	int numCompare;

	
	
 	

	
	// ===> Attribute numEfficiencies
	
	

	int numEfficiencies;

	
	
 	

	
	// ===> Attribute compareArray
	
	

	vector<string > compareArray;

	
	
 	

	
	// ===> Attribute efficiencyIndices
	
	

	vector<int > efficiencyIndices;

	
	
 	

	
	// ===> Attribute efficiencies
	
	

	vector<vector<float > > efficiencies;

	
	
 	

	
	// ===> Attribute quality
	
	

	vector<float > quality;

	
	
 	

	
	// ===> Attribute phasedSumAntenna
	
	

	string phasedSumAntenna;

	
	
 	

	
	// ===> Attribute typeSupports, which is optional
	
	
	bool typeSupportsExists;
	

	string typeSupports;

	
	
 	

	
	// ===> Attribute numSupports, which is optional
	
	
	bool numSupportsExists;
	

	int numSupports;

	
	
 	

	
	// ===> Attribute phaseSupports, which is optional
	
	
	bool phaseSupportsExists;
	

	vector<float > phaseSupports;

	
	
 	

	////////////////////////////////
	// Extrinsic casacore::Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute calDataId
	
	

	Tag calDataId;

	
	
 	

	
	// ===> Attribute calReductionId
	
	

	Tag calReductionId;

	
	
 	

	///////////
	// Links //
	///////////
	
	
		

	 

	

	
		

	 

	

	
/*
	////////////////////////////////////////////////////////////
	// binary-deserialization material from an EndianIStream  //
	////////////////////////////////////////////////////////////
	std::map<std::string, CalAppPhaseAttributeFromBin> fromBinMethods;
void basebandNameFromBin( EndianIStream& eis);
void scanNumberFromBin( EndianIStream& eis);
void calDataIdFromBin( EndianIStream& eis);
void calReductionIdFromBin( EndianIStream& eis);
void startValidTimeFromBin( EndianIStream& eis);
void endValidTimeFromBin( EndianIStream& eis);
void adjustTimeFromBin( EndianIStream& eis);
void adjustTokenFromBin( EndianIStream& eis);
void phasingModeFromBin( EndianIStream& eis);
void numPhasedAntennasFromBin( EndianIStream& eis);
void phasedAntennasFromBin( EndianIStream& eis);
void refAntennaIndexFromBin( EndianIStream& eis);
void candRefAntennaIndexFromBin( EndianIStream& eis);
void phasePackingFromBin( EndianIStream& eis);
void numReceptorsFromBin( EndianIStream& eis);
void numChannelsFromBin( EndianIStream& eis);
void numPhaseValuesFromBin( EndianIStream& eis);
void phaseValuesFromBin( EndianIStream& eis);
void numCompareFromBin( EndianIStream& eis);
void numEfficienciesFromBin( EndianIStream& eis);
void compareArrayFromBin( EndianIStream& eis);
void efficiencyIndicesFromBin( EndianIStream& eis);
void efficienciesFromBin( EndianIStream& eis);
void qualityFromBin( EndianIStream& eis);
void phasedSumAntennaFromBin( EndianIStream& eis);

void typeSupportsFromBin( EndianIStream& eis);
void numSupportsFromBin( EndianIStream& eis);
void phaseSupportsFromBin( EndianIStream& eis);

*/
	
	///////////////////////////////////
	// text-deserialization material //
	///////////////////////////////////
	std::map<std::string, CalAppPhaseAttributeFromText> fromTextMethods;
	
void basebandNameFromText (const string & s);
	
	
void scanNumberFromText (const string & s);
	
	
void calDataIdFromText (const string & s);
	
	
void calReductionIdFromText (const string & s);
	
	
void startValidTimeFromText (const string & s);
	
	
void endValidTimeFromText (const string & s);
	
	
void adjustTimeFromText (const string & s);
	
	
void adjustTokenFromText (const string & s);
	
	
void phasingModeFromText (const string & s);
	
	
void numPhasedAntennasFromText (const string & s);
	
	
void phasedAntennasFromText (const string & s);
	
	
void refAntennaIndexFromText (const string & s);
	
	
void candRefAntennaIndexFromText (const string & s);
	
	
void phasePackingFromText (const string & s);
	
	
void numReceptorsFromText (const string & s);
	
	
void numChannelsFromText (const string & s);
	
	
void numPhaseValuesFromText (const string & s);
	
	
void phaseValuesFromText (const string & s);
	
	
void numCompareFromText (const string & s);
	
	
void numEfficienciesFromText (const string & s);
	
	
void compareArrayFromText (const string & s);
	
	
void efficiencyIndicesFromText (const string & s);
	
	
void efficienciesFromText (const string & s);
	
	
void qualityFromText (const string & s);
	
	
void phasedSumAntennaFromText (const string & s);
	

	
void typeSupportsFromText (const string & s);
	
	
void numSupportsFromText (const string & s);
	
	
void phaseSupportsFromText (const string & s);
	
	
	
	/**
	 * Serialize this into a stream of bytes written to an EndianOSStream.
	 * @param eoss the EndianOSStream to be written to
	 */
	 void toBin(EndianOSStream& eoss);
	 	 
	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the CalAppPhaseTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.

	 static CalAppPhaseRow* fromBin(EndianIStream& eis, CalAppPhaseTable& table, const std::vector<std::string>& attributesSeq);	 
		*/
};

} // End namespace asdm

#endif /* CalAppPhase_CLASS */
