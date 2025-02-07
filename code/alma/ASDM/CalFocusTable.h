
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
 * casacore::File CalFocusTable.h
 */
 
#ifndef CalFocusTable_CLASS
#define CalFocusTable_CLASS

#include <string>
#include <vector>
#include <map>



	
#include <ArrayTime.h>
	

	
#include <Temperature.h>
	

	
#include <Angle.h>
	

	
#include <Tag.h>
	

	
#include <Length.h>
	

	
#include <Frequency.h>
	




	

	

	

	
#include "CReceiverBand.h"
	

	

	
#include "CAtmPhaseCorrection.h"
	

	
#include "CFocusMethod.h"
	

	

	

	

	
#include "CPolarizationType.h"
	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	



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
//class asdm::CalFocusRow;

class ASDM;
class CalFocusRow;
/**
 * The CalFocusTable class is an Alma table.
 * <BR>
 * 
 * \par Role
 * Result of focus calibration performed on-line by TelCal.
 * <BR>
 
 * Generated from model's revision "-1", branch ""
 *
 * <TABLE BORDER="1">
 * <CAPTION> Attributes of CalFocus </CAPTION>
 * <TR BGCOLOR="#AAAAAA"> <TH> Name </TH> <TH> Type </TH> <TH> Expected shape  </TH> <TH> Comment </TH></TR>
 
 * <TR> <TH BGCOLOR="#CCCCCC" colspan="4" align="center"> Key </TD></TR>
	
 * <TR>
 		
 * <TD> antennaName </TD>
 		 
 * <TD> string</TD>
 * <TD> &nbsp; </TD>
 * <TD> &nbsp;the name of the antenna. </TD>
 * </TR>
	
 * <TR>
 		
 * <TD> receiverBand </TD>
 		 
 * <TD> ReceiverBandMod::ReceiverBand</TD>
 * <TD> &nbsp; </TD>
 * <TD> &nbsp;identifies the receiver band. </TD>
 * </TR>
	
 * <TR>
 		
 * <TD> calDataId </TD>
 		 
 * <TD> Tag</TD>
 * <TD> &nbsp; </TD>
 * <TD> &nbsp;refers to a unique row in CalData Table. </TD>
 * </TR>
	
 * <TR>
 		
 * <TD> calReductionId </TD>
 		 
 * <TD> Tag</TD>
 * <TD> &nbsp; </TD>
 * <TD> &nbsp;refers to a unique row in CalReduction Table. </TD>
 * </TR>
	


 * <TR> <TH BGCOLOR="#CCCCCC"  colspan="4" valign="center"> Value <br> (Mandatory) </TH></TR>
	
 * <TR>
 * <TD> startValidTime </TD> 
 * <TD> ArrayTime </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the start time of the result validity period. </TD>
 * </TR>
	
 * <TR>
 * <TD> endValidTime </TD> 
 * <TD> ArrayTime </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the end time of the result validity period. </TD>
 * </TR>
	
 * <TR>
 * <TD> ambientTemperature </TD> 
 * <TD> Temperature </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the ambient temperature. </TD>
 * </TR>
	
 * <TR>
 * <TD> atmPhaseCorrection </TD> 
 * <TD> AtmPhaseCorrectionMod::AtmPhaseCorrection </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;qualifies how the atmospheric phase correction has been applied. </TD>
 * </TR>
	
 * <TR>
 * <TD> focusMethod </TD> 
 * <TD> FocusMethodMod::FocusMethod </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;identifies the method used during the calibration. </TD>
 * </TR>
	
 * <TR>
 * <TD> frequencyRange </TD> 
 * <TD> vector<Frequency > </TD>
 * <TD>  2 </TD> 
 * <TD> &nbsp;the frequency range over which the result is valid. </TD>
 * </TR>
	
 * <TR>
 * <TD> pointingDirection </TD> 
 * <TD> vector<Angle > </TD>
 * <TD>  2 </TD> 
 * <TD> &nbsp;the antenna pointing direction (horizontal coordinates). </TD>
 * </TR>
	
 * <TR>
 * <TD> numReceptor (\f$N_{Rece}\f$)</TD> 
 * <TD> int </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the number of receptors. </TD>
 * </TR>
	
 * <TR>
 * <TD> polarizationTypes </TD> 
 * <TD> vector<PolarizationTypeMod::PolarizationType > </TD>
 * <TD>  numReceptor </TD> 
 * <TD> &nbsp;identifies the polarization types (one value per receptor). </TD>
 * </TR>
	
 * <TR>
 * <TD> wereFixed </TD> 
 * <TD> vector<bool > </TD>
 * <TD>  3 </TD> 
 * <TD> &nbsp;coordinates were fixed (true) or not fixed (false) (one value per individual coordinate). </TD>
 * </TR>
	
 * <TR>
 * <TD> offset </TD> 
 * <TD> vector<vector<Length > > </TD>
 * <TD>  numReceptor, 3 </TD> 
 * <TD> &nbsp;the measured focus offsets in X,Y,Z (one triple of values per receptor). </TD>
 * </TR>
	
 * <TR>
 * <TD> offsetError </TD> 
 * <TD> vector<vector<Length > > </TD>
 * <TD>  numReceptor, 3 </TD> 
 * <TD> &nbsp;the statistical uncertainties on measured focus offsets (one triple per receptor). </TD>
 * </TR>
	
 * <TR>
 * <TD> offsetWasTied </TD> 
 * <TD> vector<vector<bool > > </TD>
 * <TD>  numReceptor, 3 </TD> 
 * <TD> &nbsp;focus was tied (true) or not tied (false) (one value per receptor and focus individual coordinate). </TD>
 * </TR>
	
 * <TR>
 * <TD> reducedChiSquared </TD> 
 * <TD> vector<vector<double > > </TD>
 * <TD>  numReceptor, 3 </TD> 
 * <TD> &nbsp;a measure of the quality of the fit (one triple per receptor). </TD>
 * </TR>
	
 * <TR>
 * <TD> position </TD> 
 * <TD> vector<vector<Length > > </TD>
 * <TD>  numReceptor, 3 </TD> 
 * <TD> &nbsp;the absolute focus position in X,Y,Z (one triple of values per receptor). </TD>
 * </TR>
	


 * <TR> <TH BGCOLOR="#CCCCCC"  colspan="4" valign="center"> Value <br> (Optional) </TH></TR>
	
 * <TR>
 * <TD> polarizationsAveraged</TD> 
 * <TD> bool </TD>
 * <TD>  &nbsp; </TD>
 * <TD>&nbsp; Polarizations were averaged. </TD>
 * </TR>
	
 * <TR>
 * <TD> focusCurveWidth</TD> 
 * <TD> vector<vector<Length > > </TD>
 * <TD>  numReceptor, 3  </TD>
 * <TD>&nbsp; half power width of fitted focus curve (one triple per receptor). </TD>
 * </TR>
	
 * <TR>
 * <TD> focusCurveWidthError</TD> 
 * <TD> vector<vector<Length > > </TD>
 * <TD>  numReceptor, 3  </TD>
 * <TD>&nbsp; Uncertainty of the focus curve width. </TD>
 * </TR>
	
 * <TR>
 * <TD> focusCurveWasFixed</TD> 
 * <TD> vector<bool > </TD>
 * <TD>  3  </TD>
 * <TD>&nbsp; each coordinate of the focus curve width was set (true) or not set (false) to an assumed value. </TD>
 * </TR>
	
 * <TR>
 * <TD> offIntensity</TD> 
 * <TD> vector<Temperature > </TD>
 * <TD>  numReceptor  </TD>
 * <TD>&nbsp; the off intensity levels (one value per receptor). </TD>
 * </TR>
	
 * <TR>
 * <TD> offIntensityError</TD> 
 * <TD> vector<Temperature > </TD>
 * <TD>  numReceptor  </TD>
 * <TD>&nbsp; the uncertainties on the off intensity levels (one value per receptor). </TD>
 * </TR>
	
 * <TR>
 * <TD> offIntensityWasFixed</TD> 
 * <TD> bool </TD>
 * <TD>  &nbsp; </TD>
 * <TD>&nbsp; the off intensity level was fixed (true) or not fixed (false). </TD>
 * </TR>
	
 * <TR>
 * <TD> peakIntensity</TD> 
 * <TD> vector<Temperature > </TD>
 * <TD>  numReceptor  </TD>
 * <TD>&nbsp; the maximum intensities (one value per receptor). </TD>
 * </TR>
	
 * <TR>
 * <TD> peakIntensityError</TD> 
 * <TD> vector<Temperature > </TD>
 * <TD>  numReceptor  </TD>
 * <TD>&nbsp; the uncertainties on the maximum intensities (one value per receptor). </TD>
 * </TR>
	
 * <TR>
 * <TD> peakIntensityWasFixed</TD> 
 * <TD> bool </TD>
 * <TD>  &nbsp; </TD>
 * <TD>&nbsp; the maximum intensity was fixed (true) or not fixed (false). </TD>
 * </TR>
	
 * <TR>
 * <TD> astigmPlus</TD> 
 * <TD> vector<Length > </TD>
 * <TD>  numReceptor  </TD>
 * <TD>&nbsp; the astigmatism component with 0 degree symmetry axis. </TD>
 * </TR>
	
 * <TR>
 * <TD> astigmPlusError</TD> 
 * <TD> vector<Length > </TD>
 * <TD>  numReceptor  </TD>
 * <TD>&nbsp; the statistical error on astigmPlus </TD>
 * </TR>
	
 * <TR>
 * <TD> astigmMult</TD> 
 * <TD> vector<Length > </TD>
 * <TD>  numReceptor  </TD>
 * <TD>&nbsp; the astigmatism component with 45 degrees symmetry axis. </TD>
 * </TR>
	
 * <TR>
 * <TD> astigmMultError</TD> 
 * <TD> vector<Length > </TD>
 * <TD>  numReceptor  </TD>
 * <TD>&nbsp; the statistical error on astigmMult </TD>
 * </TR>
	
 * <TR>
 * <TD> illumOffset</TD> 
 * <TD> vector<vector<Length > > </TD>
 * <TD>  numReceptor, 2  </TD>
 * <TD>&nbsp; the illumination offset of the primary reflector expressed as a pair of values. </TD>
 * </TR>
	
 * <TR>
 * <TD> illumOffsetError</TD> 
 * <TD> vector<vector<Length > > </TD>
 * <TD>  numReceptor, 2  </TD>
 * <TD>&nbsp; the statistical error on illumOffset. </TD>
 * </TR>
	
 * <TR>
 * <TD> fitRMS</TD> 
 * <TD> vector<Length > </TD>
 * <TD>  numReceptor  </TD>
 * <TD>&nbsp; The RMS of the half path length after removing the best fit parabola. </TD>
 * </TR>
	

 * </TABLE>
 */
class CalFocusTable : public Representable {
	friend class ASDM;

public:


	/**
	 * Return the list of field names that make up key key
	 * as an array of strings.
	 * @return a vector of string.
	 */	
	static const std::vector<std::string>& getKeyName();


	virtual ~CalFocusTable();
	
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
	 * to the schema defined for CalFocus (CalFocusTable.xsd).
	 *
	 * @returns a string containing the XML representation.
	 * @throws ConversionException
	 */
	std::string toXML()  ;

#ifndef WITHOUT_ACS
	// casacore::Conversion Methods
	/**
	 * Convert this table into a CalFocusTableIDL CORBA structure.
	 *
	 * @return a pointer to a CalFocusTableIDL
	 */
	asdmIDL::CalFocusTableIDL *toIDL() ;
	
	/**
	 * Fills the CORBA data structure passed in parameter
	 * with the content of this table.
	 *
	 * @param x a reference to the asdmIDL::CalFocusTableIDL to be populated
	 * with the content of this.
	 */
	 void toIDL(asdmIDL::CalFocusTableIDL& x) const;
	 
#endif

#ifndef WITHOUT_ACS
	/**
	 * Populate this table from the content of a CalFocusTableIDL Corba structure.
	 *
	 * @throws DuplicateKey Thrown if the method tries to add a row having a key that is already in the table.
	 * @throws ConversionException
	 */	
	void fromIDL(asdmIDL::CalFocusTableIDL x) ;
#endif
	
	//
	// ====> Row creation.
	//
	
	/**
	 * Create a new row with default values.
	 * @return a pointer on a CalFocusRow
	 */
	CalFocusRow *newRow();
	
	
	/**
	 * Create a new row initialized to the specified values.
	 * @return a pointer on the created and initialized row.
	
 	 * @param antennaName
	
 	 * @param receiverBand
	
 	 * @param calDataId
	
 	 * @param calReductionId
	
 	 * @param startValidTime
	
 	 * @param endValidTime
	
 	 * @param ambientTemperature
	
 	 * @param atmPhaseCorrection
	
 	 * @param focusMethod
	
 	 * @param frequencyRange
	
 	 * @param pointingDirection
	
 	 * @param numReceptor
	
 	 * @param polarizationTypes
	
 	 * @param wereFixed
	
 	 * @param offset
	
 	 * @param offsetError
	
 	 * @param offsetWasTied
	
 	 * @param reducedChiSquared
	
 	 * @param position
	
     */
	CalFocusRow *newRow(string antennaName, ReceiverBandMod::ReceiverBand receiverBand, Tag calDataId, Tag calReductionId, ArrayTime startValidTime, ArrayTime endValidTime, Temperature ambientTemperature, AtmPhaseCorrectionMod::AtmPhaseCorrection atmPhaseCorrection, FocusMethodMod::FocusMethod focusMethod, vector<Frequency > frequencyRange, vector<Angle > pointingDirection, int numReceptor, vector<PolarizationTypeMod::PolarizationType > polarizationTypes, vector<bool > wereFixed, vector<vector<Length > > offset, vector<vector<Length > > offsetError, vector<vector<bool > > offsetWasTied, vector<vector<double > > reducedChiSquared, vector<vector<Length > > position);
	


	/**
	 * Create a new row using a copy constructor mechanism.
	 * 
	 * The method creates a new CalFocusRow owned by this. Each attribute of the created row 
	 * is a (deep) copy of the corresponding attribute of row. The method does not add 
	 * the created row to this, its simply parents it to this, a call to the add method
	 * has to be done in order to get the row added (very likely after having modified
	 * some of its attributes).
	 * If row is null then the method returns a new CalFocusRow with default values for its attributes. 
	 *
	 * @param row the row which is to be copied.
	 */
	 CalFocusRow *newRow(CalFocusRow *row); 

	//
	// ====> Append a row to its table.
	//
 
	
	/**
	 * Add a row.
	 * @param x a pointer to the CalFocusRow to be added.
	 *
	 * @return a pointer to a CalFocusRow. If the table contains a CalFocusRow whose attributes (key and mandatory values) are equal to x ones
	 * then returns a pointer on that CalFocusRow, otherwise returns x.
	 *
	 * @throw DuplicateKey { thrown when the table contains a CalFocusRow with a key equal to the x one but having
	 * and a value section different from x one }
	 *
	
	 */
	CalFocusRow* add(CalFocusRow* x) ; 

 



	//
	// ====> Methods returning rows.
	//
		
	/**
	 * Get a collection of pointers on the rows of the table.
	 * @return Alls rows in a vector of pointers of CalFocusRow. The elements of this vector are stored in the order 
	 * in which they have been added to the CalFocusTable.
	 */
	std::vector<CalFocusRow *> get() ;
	
	/**
	 * Get a const reference on the collection of rows pointers internally hold by the table.
	 * @return A const reference of a vector of pointers of CalFocusRow. The elements of this vector are stored in the order 
	 * in which they have been added to the CalFocusTable.
	 *
	 */
	 const std::vector<CalFocusRow *>& get() const ;
	


 
	
	/**
 	 * Returns a CalFocusRow* given a key.
 	 * @return a pointer to the row having the key whose values are passed as parameters, or 0 if
 	 * no row exists for that key.
	
	 * @param antennaName
	
	 * @param receiverBand
	
	 * @param calDataId
	
	 * @param calReductionId
	
 	 *
	 */
 	CalFocusRow* getRowByKey(string antennaName, ReceiverBandMod::ReceiverBand receiverBand, Tag calDataId, Tag calReductionId);

 	 	



	/**
 	 * Look up the table for a row whose all attributes 
 	 * are equal to the corresponding parameters of the method.
 	 * @return a pointer on this row if any, null otherwise.
 	 *
			
 	 * @param antennaName
 	 		
 	 * @param receiverBand
 	 		
 	 * @param calDataId
 	 		
 	 * @param calReductionId
 	 		
 	 * @param startValidTime
 	 		
 	 * @param endValidTime
 	 		
 	 * @param ambientTemperature
 	 		
 	 * @param atmPhaseCorrection
 	 		
 	 * @param focusMethod
 	 		
 	 * @param frequencyRange
 	 		
 	 * @param pointingDirection
 	 		
 	 * @param numReceptor
 	 		
 	 * @param polarizationTypes
 	 		
 	 * @param wereFixed
 	 		
 	 * @param offset
 	 		
 	 * @param offsetError
 	 		
 	 * @param offsetWasTied
 	 		
 	 * @param reducedChiSquared
 	 		
 	 * @param position
 	 		 
 	 */
	CalFocusRow* lookup(string antennaName, ReceiverBandMod::ReceiverBand receiverBand, Tag calDataId, Tag calReductionId, ArrayTime startValidTime, ArrayTime endValidTime, Temperature ambientTemperature, AtmPhaseCorrectionMod::AtmPhaseCorrection atmPhaseCorrection, FocusMethodMod::FocusMethod focusMethod, vector<Frequency > frequencyRange, vector<Angle > pointingDirection, int numReceptor, vector<PolarizationTypeMod::PolarizationType > polarizationTypes, vector<bool > wereFixed, vector<vector<Length > > offset, vector<vector<Length > > offsetError, vector<vector<bool > > offsetWasTied, vector<vector<double > > reducedChiSquared, vector<vector<Length > > position); 


	void setUnknownAttributeBinaryReader(const std::string& attributeName, BinaryAttributeReaderFunctor* barFctr);
	BinaryAttributeReaderFunctor* getUnknownAttributeBinaryReader(const std::string& attributeName) const;

private:

	/**
	 * Create a CalFocusTable.
	 * <p>
	 * This constructor is private because only the
	 * container can create tables.  All tables must know the container
	 * to which they belong.
	 * @param container The container to which this table belongs.
	 */ 
	CalFocusTable (ASDM & container);

	ASDM & container;
	
	bool archiveAsBin; // If true archive binary else archive XML
	bool fileAsBin ; // If true file binary else file XML	
	
	std::string version ; 
	
	Entity entity;
	

	
	


	/**
	 * If this table has an autoincrementable attribute then check if *x verifies the rule of uniqueness and throw exception if not.
	 * Check if *x verifies the key uniqueness rule and throw an exception if not.
	 * Append x to its table.
	 * @throws DuplicateKey
	 
	 */
	CalFocusRow* checkAndAdd(CalFocusRow* x, bool skipCheckUniqueness=false) ;
	
	/**
	 * Brutally append an CalFocusRow x to the collection of rows already stored in this table. No uniqueness check is done !
	 *
	 * @param CalFocusRow* x a pointer onto the CalFocusRow to be appended.
	 */
	 void append(CalFocusRow* x) ;
	 
	/**
	 * Brutally append an CalFocusRow x to the collection of rows already stored in this table. No uniqueness check is done !
	 *
	 * @param CalFocusRow* x a pointer onto the CalFocusRow to be appended.
	 */
	 void addWithoutCheckingUnique(CalFocusRow* x) ;
	 
	 



// A data structure to store the pointers on the table's rows.

// In all cases we maintain a private vector of CalFocusRow s.
   std::vector<CalFocusRow * > privateRows;
   

			
	std::vector<CalFocusRow *> row;

	
	void error() ; //throw(ConversionException);

	
	/**
	 * Populate this table from the content of a XML document that is required to
	 * be conform to the XML schema defined for a CalFocus (CalFocusTable.xsd).
	 * @throws ConversionException
	 * 
	 */
	void fromXML(std::string& xmlDoc) ;
		
	std::map<std::string, BinaryAttributeReaderFunctor *> unknownAttributes2Functors;

	/**
	  * Private methods involved during the build of this table out of the content
	  * of file(s) containing an external representation of a CalFocus table.
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
	  * will be saved in a file "CalFocus.bin" or an XML representation (fileAsBin==false) will be saved in a file "CalFocus.xml".
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
	 * Reads and parses a file containing a representation of a CalFocusTable as those produced  by the toFile method.
	 * This table is populated with the result of the parsing.
	 * @param directory The name of the directory containing the file te be read and parsed.
	 * @throws ConversionException If any error occurs while reading the 
	 * files in the directory or parsing them.
	 *
	 */
	 void setFromFile(const std::string& directory);	
 
};

} // End namespace asdm

#endif /* CalFocusTable_CLASS */
