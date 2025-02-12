
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
 * casacore::File CalPhaseRow.h
 */
 
#ifndef CalPhaseRow_CLASS
#define CalPhaseRow_CLASS

#include <vector>
#include <string>
#include <set>

#ifndef WITHOUT_ACS
#include <asdmIDLC.h>
#endif






	 
#include <ArrayTime.h>
	

	 
#include <Interval.h>
	

	 
#include <Angle.h>
	

	 
#include <Tag.h>
	

	 
#include <Length.h>
	

	 
#include <Frequency.h>
	




	
#include "CBasebandName.h"
	

	
#include "CReceiverBand.h"
	

	
#include "CAtmPhaseCorrection.h"
	

	

	

	

	

	

	

	

	

	

	

	

	

	
#include "CPolarizationType.h"
	

	

	

	



#include <ConversionException.h>
#include <NoSuchRow.h>
#include <IllegalAccessException.h>

#include <RowTransformer.h>
//#include <TableStreamReader.h>

/*\file CalPhase.h
    \brief Generated from model's revision "1.64", branch "HEAD"
*/

namespace asdm {

//class asdm::CalPhaseTable;


// class asdm::CalDataRow;
class CalDataRow;

// class asdm::CalReductionRow;
class CalReductionRow;
	

class CalPhaseRow;
typedef void (CalPhaseRow::*CalPhaseAttributeFromBin) (EndianIStream& eis);
typedef void (CalPhaseRow::*CalPhaseAttributeFromText) (const string& s);

/**
 * The CalPhaseRow class is a row of a CalPhaseTable.
 * 
 * Generated from model's revision "1.64", branch "HEAD"
 *
 */
class CalPhaseRow {
friend class asdm::CalPhaseTable;
friend class asdm::RowTransformer<CalPhaseRow>;
//friend class asdm::TableStreamReader<CalPhaseTable, CalPhaseRow>;

public:

	virtual ~CalPhaseRow();

	/**
	 * Return the table to which this row belongs.
	 */
	CalPhaseTable &getTable() const;
	
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
  		
	
	
	


	
	// ===> Attribute receiverBand
	
	
	

	
 	/**
 	 * Get receiverBand.
 	 * @return receiverBand as ReceiverBandMod::ReceiverBand
 	 */
 	ReceiverBandMod::ReceiverBand getReceiverBand() const;
	
 
 	
 	
 	/**
 	 * Set receiverBand with the specified ReceiverBandMod::ReceiverBand.
 	 * @param receiverBand The ReceiverBandMod::ReceiverBand value to which receiverBand is to be set.
 	 
 		
 			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setReceiverBand (ReceiverBandMod::ReceiverBand receiverBand);
  		
	
	
	


	
	// ===> Attribute atmPhaseCorrection
	
	
	

	
 	/**
 	 * Get atmPhaseCorrection.
 	 * @return atmPhaseCorrection as AtmPhaseCorrectionMod::AtmPhaseCorrection
 	 */
 	AtmPhaseCorrectionMod::AtmPhaseCorrection getAtmPhaseCorrection() const;
	
 
 	
 	
 	/**
 	 * Set atmPhaseCorrection with the specified AtmPhaseCorrectionMod::AtmPhaseCorrection.
 	 * @param atmPhaseCorrection The AtmPhaseCorrectionMod::AtmPhaseCorrection value to which atmPhaseCorrection is to be set.
 	 
 		
 			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setAtmPhaseCorrection (AtmPhaseCorrectionMod::AtmPhaseCorrection atmPhaseCorrection);
  		
	
	
	


	
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
  		
	
	
	


	
	// ===> Attribute numBaseline
	
	
	

	
 	/**
 	 * Get numBaseline.
 	 * @return numBaseline as int
 	 */
 	int getNumBaseline() const;
	
 
 	
 	
 	/**
 	 * Set numBaseline with the specified int.
 	 * @param numBaseline The int value to which numBaseline is to be set.
 	 
 		
 			
 	 */
 	void setNumBaseline (int numBaseline);
  		
	
	
	


	
	// ===> Attribute numReceptor
	
	
	

	
 	/**
 	 * Get numReceptor.
 	 * @return numReceptor as int
 	 */
 	int getNumReceptor() const;
	
 
 	
 	
 	/**
 	 * Set numReceptor with the specified int.
 	 * @param numReceptor The int value to which numReceptor is to be set.
 	 
 		
 			
 	 */
 	void setNumReceptor (int numReceptor);
  		
	
	
	


	
	// ===> Attribute ampli
	
	
	

	
 	/**
 	 * Get ampli.
 	 * @return ampli as vector<vector<float > >
 	 */
 	vector<vector<float > > getAmpli() const;
	
 
 	
 	
 	/**
 	 * Set ampli with the specified vector<vector<float > >.
 	 * @param ampli The vector<vector<float > > value to which ampli is to be set.
 	 
 		
 			
 	 */
 	void setAmpli (vector<vector<float > > ampli);
  		
	
	
	


	
	// ===> Attribute antennaNames
	
	
	

	
 	/**
 	 * Get antennaNames.
 	 * @return antennaNames as vector<vector<string > >
 	 */
 	vector<vector<string > > getAntennaNames() const;
	
 
 	
 	
 	/**
 	 * Set antennaNames with the specified vector<vector<string > >.
 	 * @param antennaNames The vector<vector<string > > value to which antennaNames is to be set.
 	 
 		
 			
 	 */
 	void setAntennaNames (vector<vector<string > > antennaNames);
  		
	
	
	


	
	// ===> Attribute baselineLengths
	
	
	

	
 	/**
 	 * Get baselineLengths.
 	 * @return baselineLengths as vector<Length >
 	 */
 	vector<Length > getBaselineLengths() const;
	
 
 	
 	
 	/**
 	 * Set baselineLengths with the specified vector<Length >.
 	 * @param baselineLengths The vector<Length > value to which baselineLengths is to be set.
 	 
 		
 			
 	 */
 	void setBaselineLengths (vector<Length > baselineLengths);
  		
	
	
	


	
	// ===> Attribute decorrelationFactor
	
	
	

	
 	/**
 	 * Get decorrelationFactor.
 	 * @return decorrelationFactor as vector<vector<float > >
 	 */
 	vector<vector<float > > getDecorrelationFactor() const;
	
 
 	
 	
 	/**
 	 * Set decorrelationFactor with the specified vector<vector<float > >.
 	 * @param decorrelationFactor The vector<vector<float > > value to which decorrelationFactor is to be set.
 	 
 		
 			
 	 */
 	void setDecorrelationFactor (vector<vector<float > > decorrelationFactor);
  		
	
	
	


	
	// ===> Attribute direction
	
	
	

	
 	/**
 	 * Get direction.
 	 * @return direction as vector<Angle >
 	 */
 	vector<Angle > getDirection() const;
	
 
 	
 	
 	/**
 	 * Set direction with the specified vector<Angle >.
 	 * @param direction The vector<Angle > value to which direction is to be set.
 	 
 		
 			
 	 */
 	void setDirection (vector<Angle > direction);
  		
	
	
	


	
	// ===> Attribute frequencyRange
	
	
	

	
 	/**
 	 * Get frequencyRange.
 	 * @return frequencyRange as vector<Frequency >
 	 */
 	vector<Frequency > getFrequencyRange() const;
	
 
 	
 	
 	/**
 	 * Set frequencyRange with the specified vector<Frequency >.
 	 * @param frequencyRange The vector<Frequency > value to which frequencyRange is to be set.
 	 
 		
 			
 	 */
 	void setFrequencyRange (vector<Frequency > frequencyRange);
  		
	
	
	


	
	// ===> Attribute integrationTime
	
	
	

	
 	/**
 	 * Get integrationTime.
 	 * @return integrationTime as Interval
 	 */
 	Interval getIntegrationTime() const;
	
 
 	
 	
 	/**
 	 * Set integrationTime with the specified Interval.
 	 * @param integrationTime The Interval value to which integrationTime is to be set.
 	 
 		
 			
 	 */
 	void setIntegrationTime (Interval integrationTime);
  		
	
	
	


	
	// ===> Attribute phase
	
	
	

	
 	/**
 	 * Get phase.
 	 * @return phase as vector<vector<float > >
 	 */
 	vector<vector<float > > getPhase() const;
	
 
 	
 	
 	/**
 	 * Set phase with the specified vector<vector<float > >.
 	 * @param phase The vector<vector<float > > value to which phase is to be set.
 	 
 		
 			
 	 */
 	void setPhase (vector<vector<float > > phase);
  		
	
	
	


	
	// ===> Attribute polarizationTypes
	
	
	

	
 	/**
 	 * Get polarizationTypes.
 	 * @return polarizationTypes as vector<PolarizationTypeMod::PolarizationType >
 	 */
 	vector<PolarizationTypeMod::PolarizationType > getPolarizationTypes() const;
	
 
 	
 	
 	/**
 	 * Set polarizationTypes with the specified vector<PolarizationTypeMod::PolarizationType >.
 	 * @param polarizationTypes The vector<PolarizationTypeMod::PolarizationType > value to which polarizationTypes is to be set.
 	 
 		
 			
 	 */
 	void setPolarizationTypes (vector<PolarizationTypeMod::PolarizationType > polarizationTypes);
  		
	
	
	


	
	// ===> Attribute phaseRMS
	
	
	

	
 	/**
 	 * Get phaseRMS.
 	 * @return phaseRMS as vector<vector<float > >
 	 */
 	vector<vector<float > > getPhaseRMS() const;
	
 
 	
 	
 	/**
 	 * Set phaseRMS with the specified vector<vector<float > >.
 	 * @param phaseRMS The vector<vector<float > > value to which phaseRMS is to be set.
 	 
 		
 			
 	 */
 	void setPhaseRMS (vector<vector<float > > phaseRMS);
  		
	
	
	


	
	// ===> Attribute statPhaseRMS
	
	
	

	
 	/**
 	 * Get statPhaseRMS.
 	 * @return statPhaseRMS as vector<vector<float > >
 	 */
 	vector<vector<float > > getStatPhaseRMS() const;
	
 
 	
 	
 	/**
 	 * Set statPhaseRMS with the specified vector<vector<float > >.
 	 * @param statPhaseRMS The vector<vector<float > > value to which statPhaseRMS is to be set.
 	 
 		
 			
 	 */
 	void setStatPhaseRMS (vector<vector<float > > statPhaseRMS);
  		
	
	
	


	
	// ===> Attribute correctionValidity, which is optional
	
	
	
	/**
	 * The attribute correctionValidity is optional. Return true if this attribute exists.
	 * @return true if and only if the correctionValidity attribute exists. 
	 */
	bool isCorrectionValidityExists() const;
	

	
 	/**
 	 * Get correctionValidity, which is optional.
 	 * @return correctionValidity as vector<bool >
 	 * @throws IllegalAccessException If correctionValidity does not exist.
 	 */
 	vector<bool > getCorrectionValidity() const;
	
 
 	
 	
 	/**
 	 * Set correctionValidity with the specified vector<bool >.
 	 * @param correctionValidity The vector<bool > value to which correctionValidity is to be set.
 	 
 		
 	 */
 	void setCorrectionValidity (vector<bool > correctionValidity);
		
	
	
	
	/**
	 * Mark correctionValidity, which is an optional field, as non-existent.
	 */
	void clearCorrectionValidity ();
	


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
	 * Compare each mandatory attribute except the autoincrementable one of this CalPhaseRow with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param basebandName
	    
	 * @param receiverBand
	    
	 * @param atmPhaseCorrection
	    
	 * @param calDataId
	    
	 * @param calReductionId
	    
	 * @param startValidTime
	    
	 * @param endValidTime
	    
	 * @param numBaseline
	    
	 * @param numReceptor
	    
	 * @param ampli
	    
	 * @param antennaNames
	    
	 * @param baselineLengths
	    
	 * @param decorrelationFactor
	    
	 * @param direction
	    
	 * @param frequencyRange
	    
	 * @param integrationTime
	    
	 * @param phase
	    
	 * @param polarizationTypes
	    
	 * @param phaseRMS
	    
	 * @param statPhaseRMS
	    
	 */ 
	bool compareNoAutoInc(BasebandNameMod::BasebandName basebandName, ReceiverBandMod::ReceiverBand receiverBand, AtmPhaseCorrectionMod::AtmPhaseCorrection atmPhaseCorrection, Tag calDataId, Tag calReductionId, ArrayTime startValidTime, ArrayTime endValidTime, int numBaseline, int numReceptor, vector<vector<float > > ampli, vector<vector<string > > antennaNames, vector<Length > baselineLengths, vector<vector<float > > decorrelationFactor, vector<Angle > direction, vector<Frequency > frequencyRange, Interval integrationTime, vector<vector<float > > phase, vector<PolarizationTypeMod::PolarizationType > polarizationTypes, vector<vector<float > > phaseRMS, vector<vector<float > > statPhaseRMS);
	
	

	
	/**
	 * Compare each mandatory value (i.e. not in the key) attribute  with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param startValidTime
	    
	 * @param endValidTime
	    
	 * @param numBaseline
	    
	 * @param numReceptor
	    
	 * @param ampli
	    
	 * @param antennaNames
	    
	 * @param baselineLengths
	    
	 * @param decorrelationFactor
	    
	 * @param direction
	    
	 * @param frequencyRange
	    
	 * @param integrationTime
	    
	 * @param phase
	    
	 * @param polarizationTypes
	    
	 * @param phaseRMS
	    
	 * @param statPhaseRMS
	    
	 */ 
	bool compareRequiredValue(ArrayTime startValidTime, ArrayTime endValidTime, int numBaseline, int numReceptor, vector<vector<float > > ampli, vector<vector<string > > antennaNames, vector<Length > baselineLengths, vector<vector<float > > decorrelationFactor, vector<Angle > direction, vector<Frequency > frequencyRange, Interval integrationTime, vector<vector<float > > phase, vector<PolarizationTypeMod::PolarizationType > polarizationTypes, vector<vector<float > > phaseRMS, vector<vector<float > > statPhaseRMS); 
		 
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *
	 * @param x a pointer on the CalPhaseRow whose required attributes of the value part 
	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool equalByRequiredValue(CalPhaseRow* x) ;
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a CalPhaseRowIDL struct.
	 */
	asdmIDL::CalPhaseRowIDL *toIDL() const;
	
	/**
	 * Define the content of a CalPhaseRowIDL struct from the values
	 * found in this row.
	 *
	 * @param x a reference to the CalPhaseRowIDL struct to be set.
	 *
	 */
	 void toIDL(asdmIDL::CalPhaseRowIDL& x) const;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct CalPhaseRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 * @throws ConversionException
	 */
	void setFromIDL (asdmIDL::CalPhaseRowIDL x) ;
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

	std::map<std::string, CalPhaseAttributeFromBin> fromBinMethods;
void basebandNameFromBin( EndianIStream& eis);
void receiverBandFromBin( EndianIStream& eis);
void atmPhaseCorrectionFromBin( EndianIStream& eis);
void calDataIdFromBin( EndianIStream& eis);
void calReductionIdFromBin( EndianIStream& eis);
void startValidTimeFromBin( EndianIStream& eis);
void endValidTimeFromBin( EndianIStream& eis);
void numBaselineFromBin( EndianIStream& eis);
void numReceptorFromBin( EndianIStream& eis);
void ampliFromBin( EndianIStream& eis);
void antennaNamesFromBin( EndianIStream& eis);
void baselineLengthsFromBin( EndianIStream& eis);
void decorrelationFactorFromBin( EndianIStream& eis);
void directionFromBin( EndianIStream& eis);
void frequencyRangeFromBin( EndianIStream& eis);
void integrationTimeFromBin( EndianIStream& eis);
void phaseFromBin( EndianIStream& eis);
void polarizationTypesFromBin( EndianIStream& eis);
void phaseRMSFromBin( EndianIStream& eis);
void statPhaseRMSFromBin( EndianIStream& eis);

void correctionValidityFromBin( EndianIStream& eis);


	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the CalPhaseTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.
	  */
	 static CalPhaseRow* fromBin(EndianIStream& eis, CalPhaseTable& table, const std::vector<std::string>& attributesSeq);	 
 
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
	CalPhaseTable &table;
	/**
	 * Whether this row has been added to the table or not.
	 */
	bool hasBeenAdded;

	// This method is used by the casacore::Table class when this row is added to the table.
	void isAdded(bool added);


	/**
	 * Create a CalPhaseRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	CalPhaseRow (CalPhaseTable &table);

	/**
	 * Create a CalPhaseRow using a copy constructor mechanism.
	 * <p>
	 * Given a CalPhaseRow row and a CalPhaseTable table, the method creates a new
	 * CalPhaseRow owned by table. Each attribute of the created row is a copy (deep)
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
	 CalPhaseRow (CalPhaseTable &table, CalPhaseRow &row);
	 	
	////////////////////////////////
	// Intrinsic casacore::Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute basebandName
	
	

	BasebandNameMod::BasebandName basebandName;

	
	
 	

	
	// ===> Attribute receiverBand
	
	

	ReceiverBandMod::ReceiverBand receiverBand;

	
	
 	

	
	// ===> Attribute atmPhaseCorrection
	
	

	AtmPhaseCorrectionMod::AtmPhaseCorrection atmPhaseCorrection;

	
	
 	

	
	// ===> Attribute startValidTime
	
	

	ArrayTime startValidTime;

	
	
 	

	
	// ===> Attribute endValidTime
	
	

	ArrayTime endValidTime;

	
	
 	

	
	// ===> Attribute numBaseline
	
	

	int numBaseline;

	
	
 	

	
	// ===> Attribute numReceptor
	
	

	int numReceptor;

	
	
 	

	
	// ===> Attribute ampli
	
	

	vector<vector<float > > ampli;

	
	
 	

	
	// ===> Attribute antennaNames
	
	

	vector<vector<string > > antennaNames;

	
	
 	

	
	// ===> Attribute baselineLengths
	
	

	vector<Length > baselineLengths;

	
	
 	

	
	// ===> Attribute decorrelationFactor
	
	

	vector<vector<float > > decorrelationFactor;

	
	
 	

	
	// ===> Attribute direction
	
	

	vector<Angle > direction;

	
	
 	

	
	// ===> Attribute frequencyRange
	
	

	vector<Frequency > frequencyRange;

	
	
 	

	
	// ===> Attribute integrationTime
	
	

	Interval integrationTime;

	
	
 	

	
	// ===> Attribute phase
	
	

	vector<vector<float > > phase;

	
	
 	

	
	// ===> Attribute polarizationTypes
	
	

	vector<PolarizationTypeMod::PolarizationType > polarizationTypes;

	
	
 	

	
	// ===> Attribute phaseRMS
	
	

	vector<vector<float > > phaseRMS;

	
	
 	

	
	// ===> Attribute statPhaseRMS
	
	

	vector<vector<float > > statPhaseRMS;

	
	
 	

	
	// ===> Attribute correctionValidity, which is optional
	
	
	bool correctionValidityExists;
	

	vector<bool > correctionValidity;

	
	
 	

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
	std::map<std::string, CalPhaseAttributeFromBin> fromBinMethods;
void basebandNameFromBin( EndianIStream& eis);
void receiverBandFromBin( EndianIStream& eis);
void atmPhaseCorrectionFromBin( EndianIStream& eis);
void calDataIdFromBin( EndianIStream& eis);
void calReductionIdFromBin( EndianIStream& eis);
void startValidTimeFromBin( EndianIStream& eis);
void endValidTimeFromBin( EndianIStream& eis);
void numBaselineFromBin( EndianIStream& eis);
void numReceptorFromBin( EndianIStream& eis);
void ampliFromBin( EndianIStream& eis);
void antennaNamesFromBin( EndianIStream& eis);
void baselineLengthsFromBin( EndianIStream& eis);
void decorrelationFactorFromBin( EndianIStream& eis);
void directionFromBin( EndianIStream& eis);
void frequencyRangeFromBin( EndianIStream& eis);
void integrationTimeFromBin( EndianIStream& eis);
void phaseFromBin( EndianIStream& eis);
void polarizationTypesFromBin( EndianIStream& eis);
void phaseRMSFromBin( EndianIStream& eis);
void statPhaseRMSFromBin( EndianIStream& eis);

void correctionValidityFromBin( EndianIStream& eis);

*/
	
	///////////////////////////////////
	// text-deserialization material //
	///////////////////////////////////
	std::map<std::string, CalPhaseAttributeFromText> fromTextMethods;
	
void basebandNameFromText (const string & s);
	
	
void receiverBandFromText (const string & s);
	
	
void atmPhaseCorrectionFromText (const string & s);
	
	
void calDataIdFromText (const string & s);
	
	
void calReductionIdFromText (const string & s);
	
	
void startValidTimeFromText (const string & s);
	
	
void endValidTimeFromText (const string & s);
	
	
void numBaselineFromText (const string & s);
	
	
void numReceptorFromText (const string & s);
	
	
void ampliFromText (const string & s);
	
	
void antennaNamesFromText (const string & s);
	
	
void baselineLengthsFromText (const string & s);
	
	
void decorrelationFactorFromText (const string & s);
	
	
void directionFromText (const string & s);
	
	
void frequencyRangeFromText (const string & s);
	
	
void integrationTimeFromText (const string & s);
	
	
void phaseFromText (const string & s);
	
	
void polarizationTypesFromText (const string & s);
	
	
void phaseRMSFromText (const string & s);
	
	
void statPhaseRMSFromText (const string & s);
	

	
void correctionValidityFromText (const string & s);
	
	
	
	/**
	 * Serialize this into a stream of bytes written to an EndianOSStream.
	 * @param eoss the EndianOSStream to be written to
	 */
	 void toBin(EndianOSStream& eoss);
	 	 
	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the CalPhaseTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.

	 static CalPhaseRow* fromBin(EndianIStream& eis, CalPhaseTable& table, const std::vector<std::string>& attributesSeq);	 
		*/
};

} // End namespace asdm

#endif /* CalPhase_CLASS */
