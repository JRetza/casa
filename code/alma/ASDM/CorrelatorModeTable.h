
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
 * casacore::File CorrelatorModeTable.h
 */
 
#ifndef CorrelatorModeTable_CLASS
#define CorrelatorModeTable_CLASS

#include <string>
#include <vector>
#include <map>



	
#include <Tag.h>
	




	

	

	
#include "CBasebandName.h"
	

	

	
#include "CAccumMode.h"
	

	

	

	
#include "CAxisName.h"
	

	
#include "CFilterMode.h"
	

	
#include "CCorrelatorName.h"
	



#include <ConversionException.h>
#include <DuplicateKey.h>
#include <UniquenessViolationException.h>
#include <NoSuchRow.h>
#include <DuplicateKey.h>


#ifndef WITHOUT_ACS
#include <asdmIDLC.h>
#endif

#include <Representable.h>

#include <pthread.h>

namespace asdm {

//class asdm::ASDM;
//class asdm::CorrelatorModeRow;

class ASDM;
class CorrelatorModeRow;
/**
 * The CorrelatorModeTable class is an Alma table.
 * <BR>
 * 
 * \par Role
 * Contains information on a Correlator processor.
 * <BR>
 
 * Generated from model's revision "1.64", branch "HEAD"
 *
 * <TABLE BORDER="1">
 * <CAPTION> Attributes of CorrelatorMode </CAPTION>
 * <TR BGCOLOR="#AAAAAA"> <TH> Name </TH> <TH> Type </TH> <TH> Expected shape  </TH> <TH> Comment </TH></TR>
 
 * <TR> <TH BGCOLOR="#CCCCCC" colspan="4" align="center"> Key </TD></TR>
	
 * <TR>
 		
 * <TD><I> correlatorModeId </I></TD>
 		 
 * <TD> Tag</TD>
 * <TD> &nbsp; </TD>
 * <TD> &nbsp;refers to a unique row in the table. </TD>
 * </TR>
	


 * <TR> <TH BGCOLOR="#CCCCCC"  colspan="4" valign="center"> Value <br> (Mandatory) </TH></TR>
	
 * <TR>
 * <TD> numBaseband </TD> 
 * <TD> int </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the number of basebands. </TD>
 * </TR>
	
 * <TR>
 * <TD> basebandNames </TD> 
 * <TD> vector<BasebandNameMod::BasebandName > </TD>
 * <TD>  numBaseband </TD> 
 * <TD> &nbsp;identifies the basebands (one value per basebands). </TD>
 * </TR>
	
 * <TR>
 * <TD> basebandConfig </TD> 
 * <TD> vector<int > </TD>
 * <TD>  numBaseband </TD> 
 * <TD> &nbsp;encodes the basebands configurations (one value per baseband). </TD>
 * </TR>
	
 * <TR>
 * <TD> accumMode </TD> 
 * <TD> AccumModeMod::AccumMode </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;identifies the accumulation mode.  </TD>
 * </TR>
	
 * <TR>
 * <TD> binMode </TD> 
 * <TD> int </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the binning mode. </TD>
 * </TR>
	
 * <TR>
 * <TD> numAxes </TD> 
 * <TD> int </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the number of axes in the binary data blocks. </TD>
 * </TR>
	
 * <TR>
 * <TD> axesOrderArray </TD> 
 * <TD> vector<AxisNameMod::AxisName > </TD>
 * <TD>  numAxes </TD> 
 * <TD> &nbsp;the order of axes in the binary data blocks. </TD>
 * </TR>
	
 * <TR>
 * <TD> filterMode </TD> 
 * <TD> vector<FilterModeMod::FilterMode > </TD>
 * <TD>  numBaseband </TD> 
 * <TD> &nbsp;identifies the filters modes (one value per baseband). </TD>
 * </TR>
	
 * <TR>
 * <TD> correlatorName </TD> 
 * <TD> CorrelatorNameMod::CorrelatorName </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;identifies the correlator's name. </TD>
 * </TR>
	


 * </TABLE>
 */
class CorrelatorModeTable : public Representable {
	friend class ASDM;

public:


	/**
	 * Return the list of field names that make up key key
	 * as an array of strings.
	 * @return a vector of string.
	 */	
	static const std::vector<std::string>& getKeyName();


	virtual ~CorrelatorModeTable();
	
	/**
	 * Return the container to which this table belongs.
	 *
	 * @return the ASDM containing this table.
	 */
	ASDM &getContainer() const;
	
	/**
	 * Return the number of rows in the table.
	 *
	 * @return the number of rows in an unsigned int.
	 */
	unsigned int size() const;
	
	/**
	 * Return the name of this table.
	 *
	 * This is a instance method of the class.
	 *
	 * @return the name of this table in a string.
	 */
	std::string getName() const;
	
	/**
	 * Return the name of this table.
	 *
	 * This is a static method of the class.
	 *
	 * @return the name of this table in a string.
	 */
	static std::string name() ;	
	
	/**
	 * Return the version information about this table.
	 *
	 */
	 std::string getVersion() const ;
	
	/**
	 * Return the names of the attributes of this table.
	 *
	 * @return a vector of string
	 */
	 static const std::vector<std::string>& getAttributesNames();

	/**
	 * Return the default sorted list of attributes names in the binary representation of the table.
	 *
	 * @return a const reference to a vector of string
	 */
	 static const std::vector<std::string>& defaultAttributesNamesInBin();
	 
	/**
	 * Return this table's Entity.
	 */
	Entity getEntity() const;

	/**
	 * Set this table's Entity.
	 * @param e An entity. 
	 */
	void setEntity(Entity e);
		
	/**
	 * Produces an XML representation conform
	 * to the schema defined for CorrelatorMode (CorrelatorModeTable.xsd).
	 *
	 * @returns a string containing the XML representation.
	 * @throws ConversionException
	 */
	std::string toXML()  ;

#ifndef WITHOUT_ACS
	// casacore::Conversion Methods
	/**
	 * Convert this table into a CorrelatorModeTableIDL CORBA structure.
	 *
	 * @return a pointer to a CorrelatorModeTableIDL
	 */
	asdmIDL::CorrelatorModeTableIDL *toIDL() ;
	
	/**
	 * Fills the CORBA data structure passed in parameter
	 * with the content of this table.
	 *
	 * @param x a reference to the asdmIDL::CorrelatorModeTableIDL to be populated
	 * with the content of this.
	 */
	 void toIDL(asdmIDL::CorrelatorModeTableIDL& x) const;
	 
#endif

#ifndef WITHOUT_ACS
	/**
	 * Populate this table from the content of a CorrelatorModeTableIDL Corba structure.
	 *
	 * @throws DuplicateKey Thrown if the method tries to add a row having a key that is already in the table.
	 * @throws ConversionException
	 */	
	void fromIDL(asdmIDL::CorrelatorModeTableIDL x) ;
#endif
	
	//
	// ====> Row creation.
	//
	
	/**
	 * Create a new row with default values.
	 * @return a pointer on a CorrelatorModeRow
	 */
	CorrelatorModeRow *newRow();
	
	
	/**
	 * Create a new row initialized to the specified values.
	 * @return a pointer on the created and initialized row.
	
 	 * @param numBaseband
	
 	 * @param basebandNames
	
 	 * @param basebandConfig
	
 	 * @param accumMode
	
 	 * @param binMode
	
 	 * @param numAxes
	
 	 * @param axesOrderArray
	
 	 * @param filterMode
	
 	 * @param correlatorName
	
     */
	CorrelatorModeRow *newRow(int numBaseband, vector<BasebandNameMod::BasebandName > basebandNames, vector<int > basebandConfig, AccumModeMod::AccumMode accumMode, int binMode, int numAxes, vector<AxisNameMod::AxisName > axesOrderArray, vector<FilterModeMod::FilterMode > filterMode, CorrelatorNameMod::CorrelatorName correlatorName);
	


	/**
	 * Create a new row using a copy constructor mechanism.
	 * 
	 * The method creates a new CorrelatorModeRow owned by this. Each attribute of the created row 
	 * is a (deep) copy of the corresponding attribute of row. The method does not add 
	 * the created row to this, its simply parents it to this, a call to the add method
	 * has to be done in order to get the row added (very likely after having modified
	 * some of its attributes).
	 * If row is null then the method returns a new CorrelatorModeRow with default values for its attributes. 
	 *
	 * @param row the row which is to be copied.
	 */
	 CorrelatorModeRow *newRow(CorrelatorModeRow *row); 

	//
	// ====> Append a row to its table.
	//

	
	
	
	/** 
	 * Add a row.
	 * If there table contains a row whose key's fields are equal
	 * to x's ones then return a pointer on this row (i.e. no actual insertion is performed) 
	 * otherwise add x to the table and return x.
	 * @param x . A pointer on the row to be added.
 	 * @returns a pointer to a CorrelatorModeRow.	 
	 */	 
	 
 	 CorrelatorModeRow* add(CorrelatorModeRow* x) ;



	//
	// ====> Methods returning rows.
	//
		
	/**
	 * Get a collection of pointers on the rows of the table.
	 * @return Alls rows in a vector of pointers of CorrelatorModeRow. The elements of this vector are stored in the order 
	 * in which they have been added to the CorrelatorModeTable.
	 */
	std::vector<CorrelatorModeRow *> get() ;
	
	/**
	 * Get a const reference on the collection of rows pointers internally hold by the table.
	 * @return A const reference of a vector of pointers of CorrelatorModeRow. The elements of this vector are stored in the order 
	 * in which they have been added to the CorrelatorModeTable.
	 *
	 */
	 const std::vector<CorrelatorModeRow *>& get() const ;
	


 
	
	/**
 	 * Returns a CorrelatorModeRow* given a key.
 	 * @return a pointer to the row having the key whose values are passed as parameters, or 0 if
 	 * no row exists for that key.
	
	 * @param correlatorModeId
	
 	 *
	 */
 	CorrelatorModeRow* getRowByKey(Tag correlatorModeId);

 	 	



	/**
 	 * Look up the table for a row whose all attributes  except the autoincrementable one 
 	 * are equal to the corresponding parameters of the method.
 	 * @return a pointer on this row if any, null otherwise.
 	 *
			
 	 * @param numBaseband
 	 		
 	 * @param basebandNames
 	 		
 	 * @param basebandConfig
 	 		
 	 * @param accumMode
 	 		
 	 * @param binMode
 	 		
 	 * @param numAxes
 	 		
 	 * @param axesOrderArray
 	 		
 	 * @param filterMode
 	 		
 	 * @param correlatorName
 	 		 
 	 */
	CorrelatorModeRow* lookup(int numBaseband, vector<BasebandNameMod::BasebandName > basebandNames, vector<int > basebandConfig, AccumModeMod::AccumMode accumMode, int binMode, int numAxes, vector<AxisNameMod::AxisName > axesOrderArray, vector<FilterModeMod::FilterMode > filterMode, CorrelatorNameMod::CorrelatorName correlatorName); 


	void setUnknownAttributeBinaryReader(const std::string& attributeName, BinaryAttributeReaderFunctor* barFctr);
	BinaryAttributeReaderFunctor* getUnknownAttributeBinaryReader(const std::string& attributeName) const;

private:

	/**
	 * Create a CorrelatorModeTable.
	 * <p>
	 * This constructor is private because only the
	 * container can create tables.  All tables must know the container
	 * to which they belong.
	 * @param container The container to which this table belongs.
	 */ 
	CorrelatorModeTable (ASDM & container);

	ASDM & container;
	
	bool archiveAsBin; // If true archive binary else archive XML
	bool fileAsBin ; // If true file binary else file XML	
	
	std::string version ; 
	
	Entity entity;
	

	// A map for the autoincrementation algorithm
	std::map<std::string,int>  noAutoIncIds;
	void autoIncrement(std::string key, CorrelatorModeRow* x);


	/**
	 * If this table has an autoincrementable attribute then check if *x verifies the rule of uniqueness and throw exception if not.
	 * Check if *x verifies the key uniqueness rule and throw an exception if not.
	 * Append x to its table.
	 * @throws DuplicateKey
	 
	 * @throws UniquenessViolationException
	 
	 */
	CorrelatorModeRow* checkAndAdd(CorrelatorModeRow* x, bool skipCheckUniqueness=false) ;
	
	/**
	 * Brutally append an CorrelatorModeRow x to the collection of rows already stored in this table. No uniqueness check is done !
	 *
	 * @param CorrelatorModeRow* x a pointer onto the CorrelatorModeRow to be appended.
	 */
	 void append(CorrelatorModeRow* x) ;
	 
	/**
	 * Brutally append an CorrelatorModeRow x to the collection of rows already stored in this table. No uniqueness check is done !
	 *
	 * @param CorrelatorModeRow* x a pointer onto the CorrelatorModeRow to be appended.
	 */
	 void addWithoutCheckingUnique(CorrelatorModeRow* x) ;
	 
	 



// A data structure to store the pointers on the table's rows.

// In all cases we maintain a private vector of CorrelatorModeRow s.
   std::vector<CorrelatorModeRow * > privateRows;
   

			
	std::vector<CorrelatorModeRow *> row;

	
	void error() ; //throw(ConversionException);

	
	/**
	 * Populate this table from the content of a XML document that is required to
	 * be conform to the XML schema defined for a CorrelatorMode (CorrelatorModeTable.xsd).
	 * @throws ConversionException
	 * 
	 */
	void fromXML(std::string& xmlDoc) ;
		
	std::map<std::string, BinaryAttributeReaderFunctor *> unknownAttributes2Functors;

	/**
	  * Private methods involved during the build of this table out of the content
	  * of file(s) containing an external representation of a CorrelatorMode table.
	  */
	void setFromMIMEFile(const std::string& directory);
	/*
	void openMIMEFile(const std::string& directory);
	*/
	void setFromXMLFile(const std::string& directory);
	
		 /**
	 * Serialize this into a stream of bytes and encapsulates that stream into a MIME message.
	 * @returns a string containing the MIME message.
	 *
	 * @param byteOrder a const pointer to a static instance of the class ByteOrder.
	 * 
	 */
	std::string toMIME(const asdm::ByteOrder* byteOrder=asdm::ByteOrder::Machine_Endianity);
  
	
   /** 
     * Extracts the binary part of a MIME message and deserialize its content
	 * to fill this with the result of the deserialization. 
	 * @param mimeMsg the string containing the MIME message.
	 * @throws ConversionException
	 */
	 void setFromMIME(const std::string & mimeMsg);
	
	/**
	  * Private methods involved during the export of this table into disk file(s).
	  */
	std::string MIMEXMLPart(const asdm::ByteOrder* byteOrder=asdm::ByteOrder::Machine_Endianity);
	
	/**
	  * Stores a representation (binary or XML) of this table into a file.
	  *
	  * Depending on the boolean value of its private field fileAsBin a binary serialization  of this (fileAsBin==true)  
	  * will be saved in a file "CorrelatorMode.bin" or an XML representation (fileAsBin==false) will be saved in a file "CorrelatorMode.xml".
	  * The file is always written in a directory whose name is passed as a parameter.
	 * @param directory The name of directory  where the file containing the table's representation will be saved.
	  * 
	  */
	  void toFile(std::string directory);
	  
	  /**
	   * Load the table in memory if necessary.
	   */
	  bool loadInProgress;
	  void checkPresenceInMemory() {
		if (!presentInMemory && !loadInProgress) {
			loadInProgress = true;
			setFromFile(getContainer().getDirectory());
			presentInMemory = true;
			loadInProgress = false;
	  	}
	  }
	/**
	 * Reads and parses a file containing a representation of a CorrelatorModeTable as those produced  by the toFile method.
	 * This table is populated with the result of the parsing.
	 * @param directory The name of the directory containing the file te be read and parsed.
	 * @throws ConversionException If any error occurs while reading the 
	 * files in the directory or parsing them.
	 *
	 */
	 void setFromFile(const std::string& directory);	
 
};

} // End namespace asdm

#endif /* CorrelatorModeTable_CLASS */
