
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
 * casacore::File WVMCalRow.h
 */
 
#ifndef WVMCalRow_CLASS
#define WVMCalRow_CLASS

#include <vector>
#include <string>
#include <set>

#ifndef WITHOUT_ACS
#include <asdmIDLC.h>
#endif






	 
#include <Tag.h>
	

	 
#include <Temperature.h>
	

	 
#include <Frequency.h>
	

	 
#include <ArrayTimeInterval.h>
	




	

	
#include "CWVRMethod.h"
	

	

	

	

	

	

	



#include <ConversionException.h>
#include <NoSuchRow.h>
#include <IllegalAccessException.h>

#include <RowTransformer.h>
//#include <TableStreamReader.h>

/*\file WVMCal.h
    \brief Generated from model's revision "1.64", branch "HEAD"
*/

namespace asdm {

//class asdm::WVMCalTable;


// class asdm::SpectralWindowRow;
class SpectralWindowRow;

// class asdm::AntennaRow;
class AntennaRow;

// class asdm::AntennaRow;
class AntennaRow;
	

class WVMCalRow;
typedef void (WVMCalRow::*WVMCalAttributeFromBin) (EndianIStream& eis);
typedef void (WVMCalRow::*WVMCalAttributeFromText) (const string& s);

/**
 * The WVMCalRow class is a row of a WVMCalTable.
 * 
 * Generated from model's revision "1.64", branch "HEAD"
 *
 */
class WVMCalRow {
friend class asdm::WVMCalTable;
friend class asdm::RowTransformer<WVMCalRow>;
//friend class asdm::TableStreamReader<WVMCalTable, WVMCalRow>;

public:

	virtual ~WVMCalRow();

	/**
	 * Return the table to which this row belongs.
	 */
	WVMCalTable &getTable() const;
	
	/**
	 * Has this row been added to its table ?
	 * @return true if and only if it has been added.
	 */
	bool isAdded() const;
		
	////////////////////////////////
	// Intrinsic casacore::Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute timeInterval
	
	
	

	
 	/**
 	 * Get timeInterval.
 	 * @return timeInterval as ArrayTimeInterval
 	 */
 	ArrayTimeInterval getTimeInterval() const;
	
 
 	
 	
 	/**
 	 * Set timeInterval with the specified ArrayTimeInterval.
 	 * @param timeInterval The ArrayTimeInterval value to which timeInterval is to be set.
 	 
 		
 			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setTimeInterval (ArrayTimeInterval timeInterval);
  		
	
	
	


	
	// ===> Attribute wvrMethod
	
	
	

	
 	/**
 	 * Get wvrMethod.
 	 * @return wvrMethod as WVRMethodMod::WVRMethod
 	 */
 	WVRMethodMod::WVRMethod getWvrMethod() const;
	
 
 	
 	
 	/**
 	 * Set wvrMethod with the specified WVRMethodMod::WVRMethod.
 	 * @param wvrMethod The WVRMethodMod::WVRMethod value to which wvrMethod is to be set.
 	 
 		
 			
 	 */
 	void setWvrMethod (WVRMethodMod::WVRMethod wvrMethod);
  		
	
	
	


	
	// ===> Attribute polyFreqLimits
	
	
	

	
 	/**
 	 * Get polyFreqLimits.
 	 * @return polyFreqLimits as vector<Frequency >
 	 */
 	vector<Frequency > getPolyFreqLimits() const;
	
 
 	
 	
 	/**
 	 * Set polyFreqLimits with the specified vector<Frequency >.
 	 * @param polyFreqLimits The vector<Frequency > value to which polyFreqLimits is to be set.
 	 
 		
 			
 	 */
 	void setPolyFreqLimits (vector<Frequency > polyFreqLimits);
  		
	
	
	


	
	// ===> Attribute numInputAntenna
	
	
	

	
 	/**
 	 * Get numInputAntenna.
 	 * @return numInputAntenna as int
 	 */
 	int getNumInputAntenna() const;
	
 
 	
 	
 	/**
 	 * Set numInputAntenna with the specified int.
 	 * @param numInputAntenna The int value to which numInputAntenna is to be set.
 	 
 		
 			
 	 */
 	void setNumInputAntenna (int numInputAntenna);
  		
	
	
	


	
	// ===> Attribute numChan
	
	
	

	
 	/**
 	 * Get numChan.
 	 * @return numChan as int
 	 */
 	int getNumChan() const;
	
 
 	
 	
 	/**
 	 * Set numChan with the specified int.
 	 * @param numChan The int value to which numChan is to be set.
 	 
 		
 			
 	 */
 	void setNumChan (int numChan);
  		
	
	
	


	
	// ===> Attribute numPoly
	
	
	

	
 	/**
 	 * Get numPoly.
 	 * @return numPoly as int
 	 */
 	int getNumPoly() const;
	
 
 	
 	
 	/**
 	 * Set numPoly with the specified int.
 	 * @param numPoly The int value to which numPoly is to be set.
 	 
 		
 			
 	 */
 	void setNumPoly (int numPoly);
  		
	
	
	


	
	// ===> Attribute pathCoeff
	
	
	

	
 	/**
 	 * Get pathCoeff.
 	 * @return pathCoeff as vector<vector<vector<float > > >
 	 */
 	vector<vector<vector<float > > > getPathCoeff() const;
	
 
 	
 	
 	/**
 	 * Set pathCoeff with the specified vector<vector<vector<float > > >.
 	 * @param pathCoeff The vector<vector<vector<float > > > value to which pathCoeff is to be set.
 	 
 		
 			
 	 */
 	void setPathCoeff (vector<vector<vector<float > > > pathCoeff);
  		
	
	
	


	
	// ===> Attribute refTemp
	
	
	

	
 	/**
 	 * Get refTemp.
 	 * @return refTemp as vector<vector<Temperature > >
 	 */
 	vector<vector<Temperature > > getRefTemp() const;
	
 
 	
 	
 	/**
 	 * Set refTemp with the specified vector<vector<Temperature > >.
 	 * @param refTemp The vector<vector<Temperature > > value to which refTemp is to be set.
 	 
 		
 			
 	 */
 	void setRefTemp (vector<vector<Temperature > > refTemp);
  		
	
	
	


	////////////////////////////////
	// Extrinsic casacore::Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute antennaId
	
	
	

	
 	/**
 	 * Get antennaId.
 	 * @return antennaId as Tag
 	 */
 	Tag getAntennaId() const;
	
 
 	
 	
 	/**
 	 * Set antennaId with the specified Tag.
 	 * @param antennaId The Tag value to which antennaId is to be set.
 	 
 		
 			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setAntennaId (Tag antennaId);
  		
	
	
	


	
	// ===> Attribute inputAntennaId
	
	
	

	
 	/**
 	 * Get inputAntennaId.
 	 * @return inputAntennaId as vector<Tag> 
 	 */
 	vector<Tag>  getInputAntennaId() const;
	
 
 	
 	
 	/**
 	 * Set inputAntennaId with the specified vector<Tag> .
 	 * @param inputAntennaId The vector<Tag>  value to which inputAntennaId is to be set.
 	 
 		
 			
 	 */
 	void setInputAntennaId (vector<Tag>  inputAntennaId);
  		
	
	
	


	
	// ===> Attribute spectralWindowId
	
	
	

	
 	/**
 	 * Get spectralWindowId.
 	 * @return spectralWindowId as Tag
 	 */
 	Tag getSpectralWindowId() const;
	
 
 	
 	
 	/**
 	 * Set spectralWindowId with the specified Tag.
 	 * @param spectralWindowId The Tag value to which spectralWindowId is to be set.
 	 
 		
 			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setSpectralWindowId (Tag spectralWindowId);
  		
	
	
	


	///////////
	// Links //
	///////////
	
	

	
		
	/**
	 * spectralWindowId pointer to the row in the SpectralWindow table having SpectralWindow.spectralWindowId == spectralWindowId
	 * @return a SpectralWindowRow*
	 * 
	 
	 */
	 SpectralWindowRow* getSpectralWindowUsingSpectralWindowId();
	 

	

	

	
		
	/**
	 * antennaId pointer to the row in the Antenna table having Antenna.antennaId == antennaId
	 * @return a AntennaRow*
	 * 
	 
	 */
	 AntennaRow* getAntennaUsingAntennaId();
	 

	

	
 		
 	/**
 	 * Set inputAntennaId[i] with the specified Tag.
 	 * @param i The index in inputAntennaId where to set the Tag value.
 	 * @param inputAntennaId The Tag value to which inputAntennaId[i] is to be set. 
	 		
 	 * @throws IndexOutOfBoundsException
  	 */
  	void setInputAntennaId (int i, Tag inputAntennaId); 
 			
	

	
		 
/**
 * Append a Tag to inputAntennaId.
 * @param id the Tag to be appended to inputAntennaId
 */
 void addInputAntennaId(Tag id); 

/**
 * Append a vector of Tag to inputAntennaId.
 * @param id an array of Tag to be appended to inputAntennaId
 */
 void addInputAntennaId(const vector<Tag> & id); 
 

 /**
  * Returns the Tag stored in inputAntennaId at position i.
  * @param i the position in inputAntennaId where the Tag is retrieved.
  * @return the Tag stored at position i in inputAntennaId.
  */
 const Tag getInputAntennaId(int i);
 
 /**
  * Returns the AntennaRow linked to this row via the tag stored in inputAntennaId
  * at position i.
  * @param i the position in inputAntennaId.
  * @return a pointer on a AntennaRow whose key (a Tag) is equal to the Tag stored at position
  * i in the inputAntennaId. 
  */
 AntennaRow* getAntennaUsingInputAntennaId(int i); 
 
 /**
  * Returns the vector of AntennaRow* linked to this row via the Tags stored in inputAntennaId
  * @return an array of pointers on AntennaRow.
  */
 vector<AntennaRow *> getAntennasUsingInputAntennaId(); 
  

	

	
	
	
	/**
	 * Compare each mandatory attribute except the autoincrementable one of this WVMCalRow with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param antennaId
	    
	 * @param spectralWindowId
	    
	 * @param timeInterval
	    
	 * @param wvrMethod
	    
	 * @param polyFreqLimits
	    
	 * @param numInputAntenna
	    
	 * @param numChan
	    
	 * @param numPoly
	    
	 * @param pathCoeff
	    
	 * @param refTemp
	    
	 * @param inputAntennaId
	    
	 */ 
	bool compareNoAutoInc(Tag antennaId, Tag spectralWindowId, ArrayTimeInterval timeInterval, WVRMethodMod::WVRMethod wvrMethod, vector<Frequency > polyFreqLimits, int numInputAntenna, int numChan, int numPoly, vector<vector<vector<float > > > pathCoeff, vector<vector<Temperature > > refTemp, vector<Tag>  inputAntennaId);
	
	

	
	/**
	 * Compare each mandatory value (i.e. not in the key) attribute  with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param wvrMethod
	    
	 * @param polyFreqLimits
	    
	 * @param numInputAntenna
	    
	 * @param numChan
	    
	 * @param numPoly
	    
	 * @param pathCoeff
	    
	 * @param refTemp
	    
	 * @param inputAntennaId
	    
	 */ 
	bool compareRequiredValue(WVRMethodMod::WVRMethod wvrMethod, vector<Frequency > polyFreqLimits, int numInputAntenna, int numChan, int numPoly, vector<vector<vector<float > > > pathCoeff, vector<vector<Temperature > > refTemp, vector<Tag>  inputAntennaId); 
		 
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *
	 * @param x a pointer on the WVMCalRow whose required attributes of the value part 
	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool equalByRequiredValue(WVMCalRow* x) ;
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a WVMCalRowIDL struct.
	 */
	asdmIDL::WVMCalRowIDL *toIDL() const;
	
	/**
	 * Define the content of a WVMCalRowIDL struct from the values
	 * found in this row.
	 *
	 * @param x a reference to the WVMCalRowIDL struct to be set.
	 *
	 */
	 void toIDL(asdmIDL::WVMCalRowIDL& x) const;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct WVMCalRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 * @throws ConversionException
	 */
	void setFromIDL (asdmIDL::WVMCalRowIDL x) ;
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

	std::map<std::string, WVMCalAttributeFromBin> fromBinMethods;
void antennaIdFromBin( EndianIStream& eis);
void spectralWindowIdFromBin( EndianIStream& eis);
void timeIntervalFromBin( EndianIStream& eis);
void wvrMethodFromBin( EndianIStream& eis);
void polyFreqLimitsFromBin( EndianIStream& eis);
void numInputAntennaFromBin( EndianIStream& eis);
void numChanFromBin( EndianIStream& eis);
void numPolyFromBin( EndianIStream& eis);
void pathCoeffFromBin( EndianIStream& eis);
void refTempFromBin( EndianIStream& eis);
void inputAntennaIdFromBin( EndianIStream& eis);

	

	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the WVMCalTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.
	  */
	 static WVMCalRow* fromBin(EndianIStream& eis, WVMCalTable& table, const std::vector<std::string>& attributesSeq);	 
 
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
	WVMCalTable &table;
	/**
	 * Whether this row has been added to the table or not.
	 */
	bool hasBeenAdded;

	// This method is used by the casacore::Table class when this row is added to the table.
	void isAdded(bool added);


	/**
	 * Create a WVMCalRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	WVMCalRow (WVMCalTable &table);

	/**
	 * Create a WVMCalRow using a copy constructor mechanism.
	 * <p>
	 * Given a WVMCalRow row and a WVMCalTable table, the method creates a new
	 * WVMCalRow owned by table. Each attribute of the created row is a copy (deep)
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
	 WVMCalRow (WVMCalTable &table, WVMCalRow &row);
	 	
	////////////////////////////////
	// Intrinsic casacore::Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute timeInterval
	
	

	ArrayTimeInterval timeInterval;

	
	
 	

	
	// ===> Attribute wvrMethod
	
	

	WVRMethodMod::WVRMethod wvrMethod;

	
	
 	

	
	// ===> Attribute polyFreqLimits
	
	

	vector<Frequency > polyFreqLimits;

	
	
 	

	
	// ===> Attribute numInputAntenna
	
	

	int numInputAntenna;

	
	
 	

	
	// ===> Attribute numChan
	
	

	int numChan;

	
	
 	

	
	// ===> Attribute numPoly
	
	

	int numPoly;

	
	
 	

	
	// ===> Attribute pathCoeff
	
	

	vector<vector<vector<float > > > pathCoeff;

	
	
 	

	
	// ===> Attribute refTemp
	
	

	vector<vector<Temperature > > refTemp;

	
	
 	

	////////////////////////////////
	// Extrinsic casacore::Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute antennaId
	
	

	Tag antennaId;

	
	
 	

	
	// ===> Attribute inputAntennaId
	
	

	vector<Tag>  inputAntennaId;

	
	
 	

	
	// ===> Attribute spectralWindowId
	
	

	Tag spectralWindowId;

	
	
 	

	///////////
	// Links //
	///////////
	
	
		

	 

	

	
		

	 

	

	
		


	

	
/*
	////////////////////////////////////////////////////////////
	// binary-deserialization material from an EndianIStream  //
	////////////////////////////////////////////////////////////
	std::map<std::string, WVMCalAttributeFromBin> fromBinMethods;
void antennaIdFromBin( EndianIStream& eis);
void spectralWindowIdFromBin( EndianIStream& eis);
void timeIntervalFromBin( EndianIStream& eis);
void wvrMethodFromBin( EndianIStream& eis);
void polyFreqLimitsFromBin( EndianIStream& eis);
void numInputAntennaFromBin( EndianIStream& eis);
void numChanFromBin( EndianIStream& eis);
void numPolyFromBin( EndianIStream& eis);
void pathCoeffFromBin( EndianIStream& eis);
void refTempFromBin( EndianIStream& eis);
void inputAntennaIdFromBin( EndianIStream& eis);

	
*/
	
	///////////////////////////////////
	// text-deserialization material //
	///////////////////////////////////
	std::map<std::string, WVMCalAttributeFromText> fromTextMethods;
	
void antennaIdFromText (const string & s);
	
	
void spectralWindowIdFromText (const string & s);
	
	
void timeIntervalFromText (const string & s);
	
	
void wvrMethodFromText (const string & s);
	
	
void polyFreqLimitsFromText (const string & s);
	
	
void numInputAntennaFromText (const string & s);
	
	
void numChanFromText (const string & s);
	
	
void numPolyFromText (const string & s);
	
	
void pathCoeffFromText (const string & s);
	
	
void refTempFromText (const string & s);
	
	
void inputAntennaIdFromText (const string & s);
	

		
	
	/**
	 * Serialize this into a stream of bytes written to an EndianOSStream.
	 * @param eoss the EndianOSStream to be written to
	 */
	 void toBin(EndianOSStream& eoss);
	 	 
	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the WVMCalTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.

	 static WVMCalRow* fromBin(EndianIStream& eis, WVMCalTable& table, const std::vector<std::string>& attributesSeq);	 
		*/
};

} // End namespace asdm

#endif /* WVMCal_CLASS */
