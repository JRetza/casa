
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
 * File ScanTable.cpp
 */
#include <ConversionException.h>
#include <DuplicateKey.h>
#include <OutOfBoundsException.h>

using asdm::ConversionException;
using asdm::DuplicateKey;
using asdm::OutOfBoundsException;

#include <ASDM.h>
#include <ScanTable.h>
#include <ScanRow.h>
#include <Parser.h>

using asdm::ASDM;
using asdm::ScanTable;
using asdm::ScanRow;
using asdm::Parser;

#include <iostream>
#include <fstream>
#include <iterator>
#include <sstream>
#include <set>
#include <algorithm>
using namespace std;

#include <Misc.h>
using namespace asdm;

#include <libxml/parser.h>
#include <libxml/tree.h>

#include "boost/filesystem/operations.hpp"
#include <boost/algorithm/string.hpp>
using namespace boost;

namespace asdm {

	string ScanTable::itsName = "Scan";
	vector<string> ScanTable::attributesNames; 
	vector<string> ScanTable::attributesNamesInBin; 
	bool ScanTable::initAttributesNamesDone = ScanTable::initAttributesNames();
	

	/**
	 * The list of field names that make up key key.
	 * (Initialization is in the constructor.)
	 */
	vector<string> ScanTable::key;

	/**
	 * Return the list of field names that make up key key
	 * as an array of strings.
	 */	
	vector<string> ScanTable::getKeyName() {
		return key;
	}


	ScanTable::ScanTable(ASDM &c) : container(c) {

	
		key.push_back("execBlockId");
	
		key.push_back("scanNumber");
	


		// Define a default entity.
		entity.setEntityId(EntityId("uid://X0/X0/X0"));
		entity.setEntityIdEncrypted("na");
		entity.setEntityTypeName("ScanTable");
		entity.setEntityVersion("1");
		entity.setInstanceVersion("1");
		
		// Archive XML
		archiveAsBin = false;
		
		// File XML
		fileAsBin = false;
		
		// By default the table is considered as present in memory
		presentInMemory = true;
		
		// By default there is no load in progress
		loadInProgress = false;
	}
	
/**
 * A destructor for ScanTable.
 */
	ScanTable::~ScanTable() {
		for (unsigned int i = 0; i < privateRows.size(); i++) 
			delete(privateRows.at(i));
	}

	/**
	 * Container to which this table belongs.
	 */
	ASDM &ScanTable::getContainer() const {
		return container;
	}

	/**
	 * Return the number of rows in the table.
	 */
	unsigned int ScanTable::size() const {
		if (presentInMemory) 
			return privateRows.size();
		else
			return declaredSize;
	}
	
	/**
	 * Return the name of this table.
	 */
	string ScanTable::getName() const {
		return itsName;
	}
	
	/**
	 * Return the name of this table.
	 */
	string ScanTable::name() {
		return itsName;
	}
	
	/**
	 * Build the vector of attributes names.
	 */
	bool ScanTable::initAttributesNames() {

		attributesNames.push_back("execBlockId");

		attributesNames.push_back("scanNumber");


		attributesNames.push_back("startTime");

		attributesNames.push_back("endTime");

		attributesNames.push_back("numIntent");

		attributesNames.push_back("numSubscan");

		attributesNames.push_back("scanIntent");

		attributesNames.push_back("calDataType");

		attributesNames.push_back("calibrationOnLine");


		attributesNames.push_back("calibrationFunction");

		attributesNames.push_back("calibrationSet");

		attributesNames.push_back("calPattern");

		attributesNames.push_back("numField");

		attributesNames.push_back("fieldName");

		attributesNames.push_back("sourceName");


    
    	 
    	attributesNamesInBin.push_back("execBlockId") ; 
    	 
    	attributesNamesInBin.push_back("scanNumber") ; 
    	 
    	attributesNamesInBin.push_back("startTime") ; 
    	 
    	attributesNamesInBin.push_back("endTime") ; 
    	 
    	attributesNamesInBin.push_back("numIntent") ; 
    	 
    	attributesNamesInBin.push_back("numSubscan") ; 
    	 
    	attributesNamesInBin.push_back("scanIntent") ; 
    	 
    	attributesNamesInBin.push_back("calDataType") ; 
    	 
    	attributesNamesInBin.push_back("calibrationOnLine") ; 
    	
    	 
    	attributesNamesInBin.push_back("calibrationFunction") ; 
    	 
    	attributesNamesInBin.push_back("calibrationSet") ; 
    	 
    	attributesNamesInBin.push_back("calPattern") ; 
    	 
    	attributesNamesInBin.push_back("numField") ; 
    	 
    	attributesNamesInBin.push_back("fieldName") ; 
    	 
    	attributesNamesInBin.push_back("sourceName") ; 
    	
    
    	return true; 
	}
	

	const vector<string>& ScanTable::getAttributesNames() { return attributesNames; }
	
	const vector<string>& ScanTable::defaultAttributesNamesInBin() { return attributesNamesInBin; }

	/**
	 * Return this table's Entity.
	 */
	Entity ScanTable::getEntity() const {
		return entity;
	}

	/**
	 * Set this table's Entity.
	 */
	void ScanTable::setEntity(Entity e) {
		this->entity = e; 
	}
	
	//
	// ====> Row creation.
	//
	
	/**
	 * Create a new row.
	 */
	ScanRow *ScanTable::newRow() {
		return new ScanRow (*this);
	}
	

	/**
	 * Create a new row initialized to the specified values.
	 * @return a pointer on the created and initialized row.
	
 	 * @param execBlockId 
	
 	 * @param scanNumber 
	
 	 * @param startTime 
	
 	 * @param endTime 
	
 	 * @param numIntent 
	
 	 * @param numSubscan 
	
 	 * @param scanIntent 
	
 	 * @param calDataType 
	
 	 * @param calibrationOnLine 
	
     */
	ScanRow* ScanTable::newRow(Tag execBlockId, int scanNumber, ArrayTime startTime, ArrayTime endTime, int numIntent, int numSubscan, vector<ScanIntentMod::ScanIntent > scanIntent, vector<CalDataOriginMod::CalDataOrigin > calDataType, vector<bool > calibrationOnLine){
		ScanRow *row = new ScanRow(*this);
			
		row->setExecBlockId(execBlockId);
			
		row->setScanNumber(scanNumber);
			
		row->setStartTime(startTime);
			
		row->setEndTime(endTime);
			
		row->setNumIntent(numIntent);
			
		row->setNumSubscan(numSubscan);
			
		row->setScanIntent(scanIntent);
			
		row->setCalDataType(calDataType);
			
		row->setCalibrationOnLine(calibrationOnLine);
	
		return row;		
	}	
	


ScanRow* ScanTable::newRow(ScanRow* row) {
	return new ScanRow(*this, *row);
}

	//
	// Append a row to its table.
	//

	
	 
	/**
	 * Add a row.
	 * @throws DuplicateKey Thrown if the new row has a key that is already in the table.
	 * @param x A pointer to the row to be added.
	 * @return x
	 */
	ScanRow* ScanTable::add(ScanRow* x) {
		
		if (getRowByKey(
						x->getExecBlockId()
						,
						x->getScanNumber()
						))
			//throw DuplicateKey(x.getExecBlockId() + "|" + x.getScanNumber(),"Scan");
			throw DuplicateKey("Duplicate key exception in ","ScanTable");
		
		row.push_back(x);
		privateRows.push_back(x);
		x->isAdded(true);
		return x;
	}

	
		
	void ScanTable::addWithoutCheckingUnique(ScanRow * x) {
		if (getRowByKey(
						x->getExecBlockId()
						,
						x->getScanNumber()
						) != (ScanRow *) 0) 
			throw DuplicateKey("Dupicate key exception in ", "ScanTable");
		row.push_back(x);
		privateRows.push_back(x);
		x->isAdded(true);
	}




	// 
	// A private method to append a row to its table, used by input conversion
	// methods, with row uniqueness.
	//

	
	/**
	 * If this table has an autoincrementable attribute then check if *x verifies the rule of uniqueness and throw exception if not.
	 * Check if *x verifies the key uniqueness rule and throw an exception if not.
	 * Append x to its table.
	 * @param x a pointer on the row to be appended.
	 * @returns a pointer on x.
	 * @throws DuplicateKey
	 
	 */
	ScanRow*  ScanTable::checkAndAdd(ScanRow* x)  {
		
		
		if (getRowByKey(
	
			x->getExecBlockId()
	,
			x->getScanNumber()
			
		)) throw DuplicateKey("Duplicate key exception in ", "ScanTable");
		
		row.push_back(x);
		privateRows.push_back(x);
		x->isAdded(true);
		return x;	
	}	



	//
	// A private method to brutally append a row to its table, without checking for row uniqueness.
	//

	void ScanTable::append(ScanRow *x) {
		privateRows.push_back(x);
		x->isAdded(true);
	}





	 vector<ScanRow *> ScanTable::get() {
	 	checkPresenceInMemory();
	    return privateRows;
	 }
	 
	 const vector<ScanRow *>& ScanTable::get() const {
	 	const_cast<ScanTable&>(*this).checkPresenceInMemory();	
	    return privateRows;
	 }	 
	 	




	

	
/*
 ** Returns a ScanRow* given a key.
 ** @return a pointer to the row having the key whose values are passed as parameters, or 0 if
 ** no row exists for that key.
 **
 */
 	ScanRow* ScanTable::getRowByKey(Tag execBlockId, int scanNumber)  {
 	checkPresenceInMemory();
	ScanRow* aRow = 0;
	for (unsigned int i = 0; i < privateRows.size(); i++) {
		aRow = row.at(i);
		
			
				if (aRow->execBlockId != execBlockId) continue;
			
		
			
				if (aRow->scanNumber != scanNumber) continue;
			
		
		return aRow;
	}
	return 0;		
}
	

	
/**
 * Look up the table for a row whose all attributes 
 * are equal to the corresponding parameters of the method.
 * @return a pointer on this row if any, 0 otherwise.
 *
			
 * @param execBlockId.
 	 		
 * @param scanNumber.
 	 		
 * @param startTime.
 	 		
 * @param endTime.
 	 		
 * @param numIntent.
 	 		
 * @param numSubscan.
 	 		
 * @param scanIntent.
 	 		
 * @param calDataType.
 	 		
 * @param calibrationOnLine.
 	 		 
 */
ScanRow* ScanTable::lookup(Tag execBlockId, int scanNumber, ArrayTime startTime, ArrayTime endTime, int numIntent, int numSubscan, vector<ScanIntentMod::ScanIntent > scanIntent, vector<CalDataOriginMod::CalDataOrigin > calDataType, vector<bool > calibrationOnLine) {
		ScanRow* aRow;
		for (unsigned int i = 0; i < privateRows.size(); i++) {
			aRow = privateRows.at(i); 
			if (aRow->compareNoAutoInc(execBlockId, scanNumber, startTime, endTime, numIntent, numSubscan, scanIntent, calDataType, calibrationOnLine)) return aRow;
		}			
		return 0;	
} 
	
 	 	

	



#ifndef WITHOUT_ACS
	using asdmIDL::ScanTableIDL;
#endif

#ifndef WITHOUT_ACS
	// Conversion Methods

	ScanTableIDL *ScanTable::toIDL() {
		ScanTableIDL *x = new ScanTableIDL ();
		unsigned int nrow = size();
		x->row.length(nrow);
		vector<ScanRow*> v = get();
		for (unsigned int i = 0; i < nrow; ++i) {
			x->row[i] = *(v[i]->toIDL());
		}
		return x;
	}
#endif
	
#ifndef WITHOUT_ACS
	void ScanTable::fromIDL(ScanTableIDL x) {
		unsigned int nrow = x.row.length();
		for (unsigned int i = 0; i < nrow; ++i) {
			ScanRow *tmp = newRow();
			tmp->setFromIDL(x.row[i]);
			// checkAndAdd(tmp);
			add(tmp);
		}
	}
#endif

	
	string ScanTable::toXML()  {
		string buf;

		buf.append("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?> ");
		buf.append("<ScanTable xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:scn=\"http://Alma/XASDM/ScanTable\" xsi:schemaLocation=\"http://Alma/XASDM/ScanTable http://almaobservatory.org/XML/XASDM/3/ScanTable.xsd\" schemaVersion=\"3\" schemaRevision=\"1.61\">\n");
	
		buf.append(entity.toXML());
		string s = container.getEntity().toXML();
		// Change the "Entity" tag to "ContainerEntity".
		buf.append("<Container" + s.substr(1,s.length() - 1)+" ");
		vector<ScanRow*> v = get();
		for (unsigned int i = 0; i < v.size(); ++i) {
			try {
				buf.append(v[i]->toXML());
			} catch (NoSuchRow e) {
			}
			buf.append("  ");
		}		
		buf.append("</ScanTable> ");
		return buf;
	}

	
	string ScanTable::getVersion() const {
		return version;
	}
	

	void ScanTable::fromXML(string& tableInXML)  {
		//
		// Look for a version information in the schemaVersion of the XML
		//
		xmlDoc *doc;
		doc = xmlReadMemory(tableInXML.data(), tableInXML.size(), "XMLTableHeader.xml", NULL, XML_PARSE_NOBLANKS);
		if ( doc == NULL )
			throw ConversionException("Failed to parse the xmlHeader into a DOM structure.", "Scan");
		
		xmlNode* root_element = xmlDocGetRootElement(doc);
   		if ( root_element == NULL || root_element->type != XML_ELEMENT_NODE )
      		throw ConversionException("Failed to retrieve the root element in the DOM structure.", "Scan");
      		
      	xmlChar * propValue = xmlGetProp(root_element, (const xmlChar *) "schemaVersion");
      	if ( propValue != 0 ) {
      		version = string( (const char*) propValue);
      		xmlFree(propValue);   		
      	}
      		     							
		Parser xml(tableInXML);
		if (!xml.isStr("<ScanTable")) 
			error();
		// cout << "Parsing a ScanTable" << endl;
		string s = xml.getElement("<Entity","/>");
		if (s.length() == 0) 
			error();
		Entity e;
		e.setFromXML(s);
		if (e.getEntityTypeName() != "ScanTable")
			error();
		setEntity(e);
		// Skip the container's entity; but, it has to be there.
		s = xml.getElement("<ContainerEntity","/>");
		if (s.length() == 0) 
			error();

		// Get each row in the table.
		s = xml.getElementContent("<row>","</row>");
		ScanRow *row;
		if (getContainer().checkRowUniqueness()) {
			try {
				while (s.length() != 0) {
					row = newRow();
					row->setFromXML(s);
					checkAndAdd(row);
					s = xml.getElementContent("<row>","</row>");
				}
				
			}
			catch (DuplicateKey e1) {
				throw ConversionException(e1.getMessage(),"ScanTable");
			} 
			catch (UniquenessViolationException e1) {
				throw ConversionException(e1.getMessage(),"ScanTable");	
			}
			catch (...) {
				// cout << "Unexpected error in ScanTable::checkAndAdd called from ScanTable::fromXML " << endl;
			}
		}
		else {
			try {
				while (s.length() != 0) {
					row = newRow();
					row->setFromXML(s);
					addWithoutCheckingUnique(row);
					s = xml.getElementContent("<row>","</row>");
				}
			}
			catch (DuplicateKey e1) {
				throw ConversionException(e1.getMessage(),"ScanTable");
			} 
			catch (...) {
				// cout << "Unexpected error in ScanTable::addWithoutCheckingUnique called from ScanTable::fromXML " << endl;
			}
		}				
				
				
		if (!xml.isStr("</ScanTable>")) 
		error();
			
		archiveAsBin = false;
		fileAsBin = false;
		
	}

	
	void ScanTable::error()  {
		throw ConversionException("Invalid xml document","Scan");
	}
	
	
	string ScanTable::MIMEXMLPart(const asdm::ByteOrder* byteOrder) {
		string UID = getEntity().getEntityId().toString();
		string withoutUID = UID.substr(6);
		string containerUID = getContainer().getEntity().getEntityId().toString();
		ostringstream oss;
		oss << "<?xml version='1.0'  encoding='ISO-8859-1'?>";
		oss << "\n";
		oss << "<ScanTable xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:scn=\"http://Alma/XASDM/ScanTable\" xsi:schemaLocation=\"http://Alma/XASDM/ScanTable http://almaobservatory.org/XML/XASDM/3/ScanTable.xsd\" schemaVersion=\"3\" schemaRevision=\"1.61\">\n";
		oss<< "<Entity entityId='"<<UID<<"' entityIdEncrypted='na' entityTypeName='ScanTable' schemaVersion='1' documentVersion='1'/>\n";
		oss<< "<ContainerEntity entityId='"<<containerUID<<"' entityIdEncrypted='na' entityTypeName='ASDM' schemaVersion='1' documentVersion='1'/>\n";
		oss << "<BulkStoreRef file_id='"<<withoutUID<<"' byteOrder='"<<byteOrder->toString()<<"' />\n";
		oss << "<Attributes>\n";

		oss << "<execBlockId/>\n"; 
		oss << "<scanNumber/>\n"; 
		oss << "<startTime/>\n"; 
		oss << "<endTime/>\n"; 
		oss << "<numIntent/>\n"; 
		oss << "<numSubscan/>\n"; 
		oss << "<scanIntent/>\n"; 
		oss << "<calDataType/>\n"; 
		oss << "<calibrationOnLine/>\n"; 

		oss << "<calibrationFunction/>\n"; 
		oss << "<calibrationSet/>\n"; 
		oss << "<calPattern/>\n"; 
		oss << "<numField/>\n"; 
		oss << "<fieldName/>\n"; 
		oss << "<sourceName/>\n"; 
		oss << "</Attributes>\n";		
		oss << "</ScanTable>\n";

		return oss.str();				
	}
	
	string ScanTable::toMIME(const asdm::ByteOrder* byteOrder) {
		EndianOSStream eoss(byteOrder);
		
		string UID = getEntity().getEntityId().toString();
		
		// The MIME Header
		eoss <<"MIME-Version: 1.0";
		eoss << "\n";
		eoss << "Content-Type: Multipart/Related; boundary='MIME_boundary'; type='text/xml'; start= '<header.xml>'";
		eoss <<"\n";
		eoss <<"Content-Description: Correlator";
		eoss <<"\n";
		eoss <<"alma-uid:" << UID;
		eoss <<"\n";
		eoss <<"\n";		
		
		// The MIME XML part header.
		eoss <<"--MIME_boundary";
		eoss <<"\n";
		eoss <<"Content-Type: text/xml; charset='ISO-8859-1'";
		eoss <<"\n";
		eoss <<"Content-Transfer-Encoding: 8bit";
		eoss <<"\n";
		eoss <<"Content-ID: <header.xml>";
		eoss <<"\n";
		eoss <<"\n";
		
		// The MIME XML part content.
		eoss << MIMEXMLPart(byteOrder);

		// The MIME binary part header
		eoss <<"--MIME_boundary";
		eoss <<"\n";
		eoss <<"Content-Type: binary/octet-stream";
		eoss <<"\n";
		eoss <<"Content-ID: <content.bin>";
		eoss <<"\n";
		eoss <<"\n";	
		
		// The MIME binary content
		entity.toBin(eoss);
		container.getEntity().toBin(eoss);
		eoss.writeInt((int) privateRows.size());
		for (unsigned int i = 0; i < privateRows.size(); i++) {
			privateRows.at(i)->toBin(eoss);	
		}
		
		// The closing MIME boundary
		eoss << "\n--MIME_boundary--";
		eoss << "\n";
		
		return eoss.str();	
	}

	
	void ScanTable::setFromMIME(const string & mimeMsg) {
    string xmlPartMIMEHeader = "Content-ID: <header.xml>\n\n";
    
    string binPartMIMEHeader = "--MIME_boundary\nContent-Type: binary/octet-stream\nContent-ID: <content.bin>\n\n";
    
    // Detect the XML header.
    string::size_type loc0 = mimeMsg.find(xmlPartMIMEHeader, 0);
    if ( loc0 == string::npos) {
      // let's try with CRLFs
      xmlPartMIMEHeader = "Content-ID: <header.xml>\r\n\r\n";
      loc0 = mimeMsg.find(xmlPartMIMEHeader, 0);
      if  ( loc0 == string::npos ) 
	      throw ConversionException("Failed to detect the beginning of the XML header", "Scan");
    }

    loc0 += xmlPartMIMEHeader.size();
    
    // Look for the string announcing the binary part.
    string::size_type loc1 = mimeMsg.find( binPartMIMEHeader, loc0 );
    
    if ( loc1 == string::npos ) {
      throw ConversionException("Failed to detect the beginning of the binary part", "Scan");
    }
    
    //
    // Extract the xmlHeader and analyze it to find out what is the byte order and the sequence
    // of attribute names.
    //
    string xmlHeader = mimeMsg.substr(loc0, loc1-loc0);
    xmlDoc *doc;
    doc = xmlReadMemory(xmlHeader.data(), xmlHeader.size(), "BinaryTableHeader.xml", NULL, XML_PARSE_NOBLANKS);
    if ( doc == NULL ) 
      throw ConversionException("Failed to parse the xmlHeader into a DOM structure.", "Scan");
    
   // This vector will be filled by the names of  all the attributes of the table
   // in the order in which they are expected to be found in the binary representation.
   //
    vector<string> attributesSeq;
      
    xmlNode* root_element = xmlDocGetRootElement(doc);
    if ( root_element == NULL || root_element->type != XML_ELEMENT_NODE )
      throw ConversionException("Failed to parse the xmlHeader into a DOM structure.", "Scan");
    
    const ByteOrder* byteOrder;
    if ( string("ASDMBinaryTable").compare((const char*) root_element->name) == 0) {
      // Then it's an "old fashioned" MIME file for tables.
      // Just try to deserialize it with Big_Endian for the bytes ordering.
      byteOrder = asdm::ByteOrder::Big_Endian;
      
 	 //
    // Let's consider a  default order for the sequence of attributes.
    //
    
    	 
    attributesSeq.push_back("execBlockId") ; 
    	 
    attributesSeq.push_back("scanNumber") ; 
    	 
    attributesSeq.push_back("startTime") ; 
    	 
    attributesSeq.push_back("endTime") ; 
    	 
    attributesSeq.push_back("numIntent") ; 
    	 
    attributesSeq.push_back("numSubscan") ; 
    	 
    attributesSeq.push_back("scanIntent") ; 
    	 
    attributesSeq.push_back("calDataType") ; 
    	 
    attributesSeq.push_back("calibrationOnLine") ; 
    	
    	 
    attributesSeq.push_back("calibrationFunction") ; 
    	 
    attributesSeq.push_back("calibrationSet") ; 
    	 
    attributesSeq.push_back("calPattern") ; 
    	 
    attributesSeq.push_back("numField") ; 
    	 
    attributesSeq.push_back("fieldName") ; 
    	 
    attributesSeq.push_back("sourceName") ; 
    	
     
    
    
    // And decide that it has version == "2"
    version = "2";         
     }
    else if (string("ScanTable").compare((const char*) root_element->name) == 0) {
      // It's a new (and correct) MIME file for tables.
      //
      // 1st )  Look for a BulkStoreRef element with an attribute byteOrder.
      //
      xmlNode* bulkStoreRef = 0;
      xmlNode* child = root_element->children;
      
      if (xmlHasProp(root_element, (const xmlChar*) "schemaVersion")) {
      	xmlChar * value = xmlGetProp(root_element, (const xmlChar *) "schemaVersion");
      	version = string ((const char *) value);
      	xmlFree(value);	
      }
      
      // Skip the two first children (Entity and ContainerEntity).
      bulkStoreRef = (child ==  0) ? 0 : ( (child->next) == 0 ? 0 : child->next->next );
      
      if ( bulkStoreRef == 0 || (bulkStoreRef->type != XML_ELEMENT_NODE)  || (string("BulkStoreRef").compare((const char*) bulkStoreRef->name) != 0))
      	throw ConversionException ("Could not find the element '/ScanTable/BulkStoreRef'. Invalid XML header '"+ xmlHeader + "'.", "Scan");
      	
      // We found BulkStoreRef, now look for its attribute byteOrder.
      _xmlAttr* byteOrderAttr = 0;
      for (struct _xmlAttr* attr = bulkStoreRef->properties; attr; attr = attr->next) 
	  if (string("byteOrder").compare((const char*) attr->name) == 0) {
	   byteOrderAttr = attr;
	   break;
	 }
      
      if (byteOrderAttr == 0) 
	     throw ConversionException("Could not find the element '/ScanTable/BulkStoreRef/@byteOrder'. Invalid XML header '" + xmlHeader +"'.", "Scan");
      
      string byteOrderValue = string((const char*) byteOrderAttr->children->content);
      if (!(byteOrder = asdm::ByteOrder::fromString(byteOrderValue)))
		throw ConversionException("No valid value retrieved for the element '/ScanTable/BulkStoreRef/@byteOrder'. Invalid XML header '" + xmlHeader + "'.", "Scan");
		
	 //
	 // 2nd) Look for the Attributes element and grab the names of the elements it contains.
	 //
	 xmlNode* attributes = bulkStoreRef->next;
     if ( attributes == 0 || (attributes->type != XML_ELEMENT_NODE)  || (string("Attributes").compare((const char*) attributes->name) != 0))	 
       	throw ConversionException ("Could not find the element '/ScanTable/Attributes'. Invalid XML header '"+ xmlHeader + "'.", "Scan");
 
 	xmlNode* childOfAttributes = attributes->children;
 	
 	while ( childOfAttributes != 0 && (childOfAttributes->type == XML_ELEMENT_NODE) ) {
 		attributesSeq.push_back(string((const char*) childOfAttributes->name));
 		childOfAttributes = childOfAttributes->next;
    }
    }
    // Create an EndianISStream from the substring containing the binary part.
    EndianISStream eiss(mimeMsg.substr(loc1+binPartMIMEHeader.size()), byteOrder);
    
    entity = Entity::fromBin((EndianIStream&) eiss);
    
    // We do nothing with that but we have to read it.
    Entity containerEntity = Entity::fromBin((EndianIStream&) eiss);

	// Let's read numRows but ignore it and rely on the value specified in the ASDM.xml file.    
    int numRows = ((EndianIStream&) eiss).readInt();
    if ((numRows != -1)                        // Then these are *not* data produced at the EVLA.
    	&& ((unsigned int) numRows != this->declaredSize )) { // Then the declared size (in ASDM.xml) is not equal to the one 
    	                                       // written into the binary representation of the table.
		cout << "The a number of rows ('" 
			 << numRows
			 << "') declared in the binary representation of the table is different from the one declared in ASDM.xml ('"
			 << this->declaredSize
			 << "'). I'll proceed with the value declared in ASDM.xml"
			 << endl;
    }                                           

	if (getContainer().checkRowUniqueness()) {
    	try {
      		for (uint32_t i = 0; i < this->declaredSize; i++) {
				ScanRow* aRow = ScanRow::fromBin((EndianIStream&) eiss, *this, attributesSeq);
				checkAndAdd(aRow);
      		}
    	}
    	catch (DuplicateKey e) {
      		throw ConversionException("Error while writing binary data , the message was "
				+ e.getMessage(), "Scan");
    	}
    	catch (TagFormatException e) {
     		 throw ConversionException("Error while reading binary data , the message was "
				+ e.getMessage(), "Scan");
    	}
    }
    else {
 		for (uint32_t i = 0; i < this->declaredSize; i++) {
			ScanRow* aRow = ScanRow::fromBin((EndianIStream&) eiss, *this, attributesSeq);
			append(aRow);
      	}   	
    }
    archiveAsBin = true;
    fileAsBin = true;
	}
	
	void ScanTable::setUnknownAttributeBinaryReader(const string& attributeName, BinaryAttributeReaderFunctor* barFctr) {
		//
		// Is this attribute really unknown ?
		//
		for (vector<string>::const_iterator iter = attributesNames.begin(); iter != attributesNames.end(); iter++) {
			if ((*iter).compare(attributeName) == 0) 
				throw ConversionException("the attribute '"+attributeName+"' is known you can't override the way it's read in the MIME binary file containing the table.", "Scan"); 
		}
		
		// Ok then register the functor to activate when an unknown attribute is met during the reading of a binary table?
		unknownAttributes2Functors[attributeName] = barFctr;
	}
	
	BinaryAttributeReaderFunctor* ScanTable::getUnknownAttributeBinaryReader(const string& attributeName) const {
		map<string, BinaryAttributeReaderFunctor*>::const_iterator iter = unknownAttributes2Functors.find(attributeName);
		return (iter == unknownAttributes2Functors.end()) ? 0 : iter->second;
	}

	
	void ScanTable::toFile(string directory) {
		if (!directoryExists(directory.c_str()) &&
			!createPath(directory.c_str())) {
			throw ConversionException("Could not create directory " , directory);
		}

		string fileName = directory + "/Scan.xml";
		ofstream tableout(fileName.c_str(),ios::out|ios::trunc);
		if (tableout.rdstate() == ostream::failbit)
			throw ConversionException("Could not open file " + fileName + " to write ", "Scan");
		if (fileAsBin) 
			tableout << MIMEXMLPart();
		else
			tableout << toXML() << endl;
		tableout.close();
		if (tableout.rdstate() == ostream::failbit)
			throw ConversionException("Could not close file " + fileName, "Scan");

		if (fileAsBin) {
			// write the bin serialized
			string fileName = directory + "/Scan.bin";
			ofstream tableout(fileName.c_str(),ios::out|ios::trunc);
			if (tableout.rdstate() == ostream::failbit)
				throw ConversionException("Could not open file " + fileName + " to write ", "Scan");
			tableout << toMIME() << endl;
			tableout.close();
			if (tableout.rdstate() == ostream::failbit)
				throw ConversionException("Could not close file " + fileName, "Scan");
		}
	}

	
	void ScanTable::setFromFile(const string& directory) {		
    if (boost::filesystem::exists(boost::filesystem::path(uniqSlashes(directory + "/Scan.xml"))))
      setFromXMLFile(directory);
    else if (boost::filesystem::exists(boost::filesystem::path(uniqSlashes(directory + "/Scan.bin"))))
      setFromMIMEFile(directory);
    else
      throw ConversionException("No file found for the Scan table", "Scan");
	}			

	
  void ScanTable::setFromMIMEFile(const string& directory) {
    string tablePath ;
    
    tablePath = directory + "/Scan.bin";
    ifstream tablefile(tablePath.c_str(), ios::in|ios::binary);
    if (!tablefile.is_open()) { 
      throw ConversionException("Could not open file " + tablePath, "Scan");
    }
    // Read in a stringstream.
    stringstream ss; ss << tablefile.rdbuf();
    
    if (tablefile.rdstate() == istream::failbit || tablefile.rdstate() == istream::badbit) {
      throw ConversionException("Error reading file " + tablePath,"Scan");
    }
    
    // And close.
    tablefile.close();
    if (tablefile.rdstate() == istream::failbit)
      throw ConversionException("Could not close file " + tablePath,"Scan");
    
    setFromMIME(ss.str());
  }	
/* 
  void ScanTable::openMIMEFile (const string& directory) {
  		
  	// Open the file.
  	string tablePath ;
    tablePath = directory + "/Scan.bin";
    ifstream tablefile(tablePath.c_str(), ios::in|ios::binary);
    if (!tablefile.is_open())
      throw ConversionException("Could not open file " + tablePath, "Scan");
      
	// Locate the xmlPartMIMEHeader.
    string xmlPartMIMEHeader = "CONTENT-ID: <HEADER.XML>\n\n";
    CharComparator comparator;
    istreambuf_iterator<char> BEGIN(tablefile.rdbuf());
    istreambuf_iterator<char> END;
    istreambuf_iterator<char> it = search(BEGIN, END, xmlPartMIMEHeader.begin(), xmlPartMIMEHeader.end(), comparator);
    if (it == END) 
    	throw ConversionException("failed to detect the beginning of the XML header", "Scan");
    
    // Locate the binaryPartMIMEHeader while accumulating the characters of the xml header.	
    string binPartMIMEHeader = "--MIME_BOUNDARY\nCONTENT-TYPE: BINARY/OCTET-STREAM\nCONTENT-ID: <CONTENT.BIN>\n\n";
    string xmlHeader;
   	CharCompAccumulator compaccumulator(&xmlHeader, 100000);
   	++it;
   	it = search(it, END, binPartMIMEHeader.begin(), binPartMIMEHeader.end(), compaccumulator);
   	if (it == END) 
   		throw ConversionException("failed to detect the beginning of the binary part", "Scan");
   	
	cout << xmlHeader << endl;
	//
	// We have the xmlHeader , let's parse it.
	//
	xmlDoc *doc;
    doc = xmlReadMemory(xmlHeader.data(), xmlHeader.size(), "BinaryTableHeader.xml", NULL, XML_PARSE_NOBLANKS);
    if ( doc == NULL ) 
      throw ConversionException("Failed to parse the xmlHeader into a DOM structure.", "Scan");
    
   // This vector will be filled by the names of  all the attributes of the table
   // in the order in which they are expected to be found in the binary representation.
   //
    vector<string> attributesSeq(attributesNamesInBin);
      
    xmlNode* root_element = xmlDocGetRootElement(doc);
    if ( root_element == NULL || root_element->type != XML_ELEMENT_NODE )
      throw ConversionException("Failed to parse the xmlHeader into a DOM structure.", "Scan");
    
    const ByteOrder* byteOrder;
    if ( string("ASDMBinaryTable").compare((const char*) root_element->name) == 0) {
      // Then it's an "old fashioned" MIME file for tables.
      // Just try to deserialize it with Big_Endian for the bytes ordering.
      byteOrder = asdm::ByteOrder::Big_Endian;
        
      // And decide that it has version == "2"
    version = "2";         
     }
    else if (string("ScanTable").compare((const char*) root_element->name) == 0) {
      // It's a new (and correct) MIME file for tables.
      //
      // 1st )  Look for a BulkStoreRef element with an attribute byteOrder.
      //
      xmlNode* bulkStoreRef = 0;
      xmlNode* child = root_element->children;
      
      if (xmlHasProp(root_element, (const xmlChar*) "schemaVersion")) {
      	xmlChar * value = xmlGetProp(root_element, (const xmlChar *) "schemaVersion");
      	version = string ((const char *) value);
      	xmlFree(value);	
      }
      
      // Skip the two first children (Entity and ContainerEntity).
      bulkStoreRef = (child ==  0) ? 0 : ( (child->next) == 0 ? 0 : child->next->next );
      
      if ( bulkStoreRef == 0 || (bulkStoreRef->type != XML_ELEMENT_NODE)  || (string("BulkStoreRef").compare((const char*) bulkStoreRef->name) != 0))
      	throw ConversionException ("Could not find the element '/ScanTable/BulkStoreRef'. Invalid XML header '"+ xmlHeader + "'.", "Scan");
      	
      // We found BulkStoreRef, now look for its attribute byteOrder.
      _xmlAttr* byteOrderAttr = 0;
      for (struct _xmlAttr* attr = bulkStoreRef->properties; attr; attr = attr->next) 
	  if (string("byteOrder").compare((const char*) attr->name) == 0) {
	   byteOrderAttr = attr;
	   break;
	 }
      
      if (byteOrderAttr == 0) 
	     throw ConversionException("Could not find the element '/ScanTable/BulkStoreRef/@byteOrder'. Invalid XML header '" + xmlHeader +"'.", "Scan");
      
      string byteOrderValue = string((const char*) byteOrderAttr->children->content);
      if (!(byteOrder = asdm::ByteOrder::fromString(byteOrderValue)))
		throw ConversionException("No valid value retrieved for the element '/ScanTable/BulkStoreRef/@byteOrder'. Invalid XML header '" + xmlHeader + "'.", "Scan");
		
	 //
	 // 2nd) Look for the Attributes element and grab the names of the elements it contains.
	 //
	 xmlNode* attributes = bulkStoreRef->next;
     if ( attributes == 0 || (attributes->type != XML_ELEMENT_NODE)  || (string("Attributes").compare((const char*) attributes->name) != 0))	 
       	throw ConversionException ("Could not find the element '/ScanTable/Attributes'. Invalid XML header '"+ xmlHeader + "'.", "Scan");
 
 	xmlNode* childOfAttributes = attributes->children;
 	
 	while ( childOfAttributes != 0 && (childOfAttributes->type == XML_ELEMENT_NODE) ) {
 		attributesSeq.push_back(string((const char*) childOfAttributes->name));
 		childOfAttributes = childOfAttributes->next;
    }
    }
    // Create an EndianISStream from the substring containing the binary part.
    EndianIFStream eifs(&tablefile, byteOrder);
    
    entity = Entity::fromBin((EndianIStream &) eifs);
    
    // We do nothing with that but we have to read it.
    Entity containerEntity = Entity::fromBin((EndianIStream &) eifs);

	// Let's read numRows but ignore it and rely on the value specified in the ASDM.xml file.    
    int numRows = eifs.readInt();
    if ((numRows != -1)                        // Then these are *not* data produced at the EVLA.
    	&& ((unsigned int) numRows != this->declaredSize )) { // Then the declared size (in ASDM.xml) is not equal to the one 
    	                                       // written into the binary representation of the table.
		cout << "The a number of rows ('" 
			 << numRows
			 << "') declared in the binary representation of the table is different from the one declared in ASDM.xml ('"
			 << this->declaredSize
			 << "'). I'll proceed with the value declared in ASDM.xml"
			 << endl;
    }    
  } 
 */

	
void ScanTable::setFromXMLFile(const string& directory) {
    string tablePath ;
    
    tablePath = directory + "/Scan.xml";
    
    /*
    ifstream tablefile(tablePath.c_str(), ios::in|ios::binary);
    if (!tablefile.is_open()) { 
      throw ConversionException("Could not open file " + tablePath, "Scan");
    }
      // Read in a stringstream.
    stringstream ss;
    ss << tablefile.rdbuf();
    
    if  (tablefile.rdstate() == istream::failbit || tablefile.rdstate() == istream::badbit) {
      throw ConversionException("Error reading file '" + tablePath + "'", "Scan");
    }
    
    // And close
    tablefile.close();
    if (tablefile.rdstate() == istream::failbit)
      throw ConversionException("Could not close file '" + tablePath + "'", "Scan");

    // Let's make a string out of the stringstream content and empty the stringstream.
    string xmlDocument = ss.str(); ss.str("");
	
    // Let's make a very primitive check to decide
    // whether the XML content represents the table
    // or refers to it via a <BulkStoreRef element.
    */
    
    string xmlDocument;
    try {
    	xmlDocument = getContainer().getXSLTransformer()(tablePath);
    	if (getenv("ASDM_DEBUG")) cout << "About to read " << tablePath << endl;
    }
    catch (XSLTransformerException e) {
    	throw ConversionException("Caugth an exception whose message is '" + e.getMessage() + "'.", "Scan");
    }
    
    if (xmlDocument.find("<BulkStoreRef") != string::npos)
      setFromMIMEFile(directory);
    else
      fromXML(xmlDocument);
  }

	

	

			
	
	

	
} // End namespace asdm
 
