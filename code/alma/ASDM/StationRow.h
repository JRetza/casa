
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
 * casacore::File StationRow.h
 */
 
#ifndef StationRow_CLASS
#define StationRow_CLASS

#include <vector>
#include <string>
#include <set>

#ifndef WITHOUT_ACS
#include <asdmIDLC.h>
#endif






	 
#include <ArrayTime.h>
	

	 
#include <Tag.h>
	

	 
#include <Length.h>
	




	

	

	

	
#include "CStationType.h"
	

	



#include <ConversionException.h>
#include <NoSuchRow.h>
#include <IllegalAccessException.h>

#include <RowTransformer.h>
//#include <TableStreamReader.h>

/*\file Station.h
    \brief Generated from model's revision "1.64", branch "HEAD"
*/

namespace asdm {

//class asdm::StationTable;

	

class StationRow;
typedef void (StationRow::*StationAttributeFromBin) (EndianIStream& eis);
typedef void (StationRow::*StationAttributeFromText) (const string& s);

/**
 * The StationRow class is a row of a StationTable.
 * 
 * Generated from model's revision "1.64", branch "HEAD"
 *
 */
class StationRow {
friend class asdm::StationTable;
friend class asdm::RowTransformer<StationRow>;
//friend class asdm::TableStreamReader<StationTable, StationRow>;

public:

	virtual ~StationRow();

	/**
	 * Return the table to which this row belongs.
	 */
	StationTable &getTable() const;
	
	/**
	 * Has this row been added to its table ?
	 * @return true if and only if it has been added.
	 */
	bool isAdded() const;
		
	////////////////////////////////
	// Intrinsic casacore::Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute stationId
	
	
	

	
 	/**
 	 * Get stationId.
 	 * @return stationId as Tag
 	 */
 	Tag getStationId() const;
	
 
 	
 	
	
	


	
	// ===> Attribute name
	
	
	

	
 	/**
 	 * Get name.
 	 * @return name as string
 	 */
 	string getName() const;
	
 
 	
 	
 	/**
 	 * Set name with the specified string.
 	 * @param name The string value to which name is to be set.
 	 
 		
 			
 	 */
 	void setName (string name);
  		
	
	
	


	
	// ===> Attribute position
	
	
	

	
 	/**
 	 * Get position.
 	 * @return position as vector<Length >
 	 */
 	vector<Length > getPosition() const;
	
 
 	
 	
 	/**
 	 * Set position with the specified vector<Length >.
 	 * @param position The vector<Length > value to which position is to be set.
 	 
 		
 			
 	 */
 	void setPosition (vector<Length > position);
  		
	
	
	


	
	// ===> Attribute type
	
	
	

	
 	/**
 	 * Get type.
 	 * @return type as StationTypeMod::StationType
 	 */
 	StationTypeMod::StationType getType() const;
	
 
 	
 	
 	/**
 	 * Set type with the specified StationTypeMod::StationType.
 	 * @param type The StationTypeMod::StationType value to which type is to be set.
 	 
 		
 			
 	 */
 	void setType (StationTypeMod::StationType type);
  		
	
	
	


	
	// ===> Attribute time, which is optional
	
	
	
	/**
	 * The attribute time is optional. Return true if this attribute exists.
	 * @return true if and only if the time attribute exists. 
	 */
	bool isTimeExists() const;
	

	
 	/**
 	 * Get time, which is optional.
 	 * @return time as ArrayTime
 	 * @throws IllegalAccessException If time does not exist.
 	 */
 	ArrayTime getTime() const;
	
 
 	
 	
 	/**
 	 * Set time with the specified ArrayTime.
 	 * @param time The ArrayTime value to which time is to be set.
 	 
 		
 	 */
 	void setTime (ArrayTime time);
		
	
	
	
	/**
	 * Mark time, which is an optional field, as non-existent.
	 */
	void clearTime ();
	


	////////////////////////////////
	// Extrinsic casacore::Table Attributes //
	////////////////////////////////
	
	///////////
	// Links //
	///////////
	
	
	
	
	/**
	 * Compare each mandatory attribute except the autoincrementable one of this StationRow with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param name
	    
	 * @param position
	    
	 * @param type
	    
	 */ 
	bool compareNoAutoInc(string name, vector<Length > position, StationTypeMod::StationType type);
	
	

	
	/**
	 * Compare each mandatory value (i.e. not in the key) attribute  with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param name
	    
	 * @param position
	    
	 * @param type
	    
	 */ 
	bool compareRequiredValue(string name, vector<Length > position, StationTypeMod::StationType type); 
		 
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *
	 * @param x a pointer on the StationRow whose required attributes of the value part 
	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool equalByRequiredValue(StationRow* x) ;
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a StationRowIDL struct.
	 */
	asdmIDL::StationRowIDL *toIDL() const;
	
	/**
	 * Define the content of a StationRowIDL struct from the values
	 * found in this row.
	 *
	 * @param x a reference to the StationRowIDL struct to be set.
	 *
	 */
	 void toIDL(asdmIDL::StationRowIDL& x) const;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct StationRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 * @throws ConversionException
	 */
	void setFromIDL (asdmIDL::StationRowIDL x) ;
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

	std::map<std::string, StationAttributeFromBin> fromBinMethods;
void stationIdFromBin( EndianIStream& eis);
void nameFromBin( EndianIStream& eis);
void positionFromBin( EndianIStream& eis);
void typeFromBin( EndianIStream& eis);

void timeFromBin( EndianIStream& eis);


	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the StationTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.
	  */
	 static StationRow* fromBin(EndianIStream& eis, StationTable& table, const std::vector<std::string>& attributesSeq);	 
 
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
	StationTable &table;
	/**
	 * Whether this row has been added to the table or not.
	 */
	bool hasBeenAdded;

	// This method is used by the casacore::Table class when this row is added to the table.
	void isAdded(bool added);


	/**
	 * Create a StationRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	StationRow (StationTable &table);

	/**
	 * Create a StationRow using a copy constructor mechanism.
	 * <p>
	 * Given a StationRow row and a StationTable table, the method creates a new
	 * StationRow owned by table. Each attribute of the created row is a copy (deep)
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
	 StationRow (StationTable &table, StationRow &row);
	 	
	////////////////////////////////
	// Intrinsic casacore::Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute stationId
	
	

	Tag stationId;

	
	
 	
 	/**
 	 * Set stationId with the specified Tag value.
 	 * @param stationId The Tag value to which stationId is to be set.
		
 		
			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setStationId (Tag stationId);
  		
	

	
	// ===> Attribute name
	
	

	string name;

	
	
 	

	
	// ===> Attribute position
	
	

	vector<Length > position;

	
	
 	

	
	// ===> Attribute type
	
	

	StationTypeMod::StationType type;

	
	
 	

	
	// ===> Attribute time, which is optional
	
	
	bool timeExists;
	

	ArrayTime time;

	
	
 	

	////////////////////////////////
	// Extrinsic casacore::Table Attributes //
	////////////////////////////////
	
	///////////
	// Links //
	///////////
	
	
/*
	////////////////////////////////////////////////////////////
	// binary-deserialization material from an EndianIStream  //
	////////////////////////////////////////////////////////////
	std::map<std::string, StationAttributeFromBin> fromBinMethods;
void stationIdFromBin( EndianIStream& eis);
void nameFromBin( EndianIStream& eis);
void positionFromBin( EndianIStream& eis);
void typeFromBin( EndianIStream& eis);

void timeFromBin( EndianIStream& eis);

*/
	
	///////////////////////////////////
	// text-deserialization material //
	///////////////////////////////////
	std::map<std::string, StationAttributeFromText> fromTextMethods;
	
void stationIdFromText (const string & s);
	
	
void nameFromText (const string & s);
	
	
void positionFromText (const string & s);
	
	
void typeFromText (const string & s);
	

	
void timeFromText (const string & s);
	
	
	
	/**
	 * Serialize this into a stream of bytes written to an EndianOSStream.
	 * @param eoss the EndianOSStream to be written to
	 */
	 void toBin(EndianOSStream& eoss);
	 	 
	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the StationTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.

	 static StationRow* fromBin(EndianIStream& eis, StationTable& table, const std::vector<std::string>& attributesSeq);	 
		*/
};

} // End namespace asdm

#endif /* Station_CLASS */
