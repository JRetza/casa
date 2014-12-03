
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
 * File DelayModelTable.cpp
 */
#include <ConversionException.h>
#include <DuplicateKey.h>
#include <OutOfBoundsException.h>

using asdm::ConversionException;
using asdm::DuplicateKey;
using asdm::OutOfBoundsException;

#include <ASDM.h>
#include <DelayModelTable.h>
#include <DelayModelRow.h>
#include <Parser.h>

using asdm::ASDM;
using asdm::DelayModelTable;
using asdm::DelayModelRow;
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
	// The name of the entity we will store in this table.
	static string entityNameOfDelayModel = "DelayModel";
	
	// An array of string containing the names of the columns of this table.
	// The array is filled in the order : key, required value, optional value.
	//
	static string attributesNamesOfDelayModel_a[] = {
		
			"antennaId"
		,
			"spectralWindowId"
		,
			"timeInterval"
		
		
			, "numPoly"
		
			, "phaseDelay"
		
			, "phaseDelayRate"
		
			, "groupDelay"
		
			, "groupDelayRate"
		
			, "fieldId"
				
		
			, "timeOrigin"
		
			, "atmosphericGroupDelay"
		
			, "atmosphericGroupDelayRate"
		
			, "geometricDelay"
		
			, "geometricDelayRate"
		
			, "numLO"
		
			, "LOOffset"
		
			, "LOOffsetRate"
		
			, "dispersiveDelay"
		
			, "dispersiveDelayRate"
		
			, "atmosphericDryDelay"
		
			, "atmosphericWetDelay"
		
			, "padDelay"
		
			, "antennaDelay"
		
			, "numReceptor"
		
			, "polarizationType"
		
			, "electronicDelay"
		
			, "electronicDelayRate"
		
			, "receiverDelay"
		
			, "IFDelay"
		
			, "LODelay"
		
			, "crossPolarizationDelay"
				
	};
	
	// A vector of string whose content is a copy of the strings in the array above.
	//
	static vector<string> attributesNamesOfDelayModel_v (attributesNamesOfDelayModel_a, attributesNamesOfDelayModel_a + sizeof(attributesNamesOfDelayModel_a) / sizeof(attributesNamesOfDelayModel_a[0]));

	// An array of string containing the names of the columns of this table.
	// The array is filled in the order where the names would be read by default in the XML header of a file containing
	// the table exported in binary mode.
	//	
	static string attributesNamesInBinOfDelayModel_a[] = {
    
    	 "antennaId" , "spectralWindowId" , "timeInterval" , "numPoly" , "phaseDelay" , "phaseDelayRate" , "groupDelay" , "groupDelayRate" , "fieldId" 
    	,
    	 "timeOrigin" , "atmosphericGroupDelay" , "atmosphericGroupDelayRate" , "geometricDelay" , "geometricDelayRate" , "numLO" , "LOOffset" , "LOOffsetRate" , "dispersiveDelay" , "dispersiveDelayRate" , "atmosphericDryDelay" , "atmosphericWetDelay" , "padDelay" , "antennaDelay" , "numReceptor" , "polarizationType" , "electronicDelay" , "electronicDelayRate" , "receiverDelay" , "IFDelay" , "LODelay" , "crossPolarizationDelay" 
    
	};
	        			
	// A vector of string whose content is a copy of the strings in the array above.
	//
	static vector<string> attributesNamesInBinOfDelayModel_v(attributesNamesInBinOfDelayModel_a, attributesNamesInBinOfDelayModel_a + sizeof(attributesNamesInBinOfDelayModel_a) / sizeof(attributesNamesInBinOfDelayModel_a[0]));		
	

	// The array of attributes (or column) names that make up key key.
	//
	string keyOfDelayModel_a[] = {
	
		"antennaId"
	,
		"spectralWindowId"
	,
		"timeInterval"
		 
	};
	 
	// A vector of strings which are copies of those stored in the array above.
	vector<string> keyOfDelayModel_v(keyOfDelayModel_a, keyOfDelayModel_a + sizeof(keyOfDelayModel_a) / sizeof(keyOfDelayModel_a[0]));

	/**
	 * Return the list of field names that make up key key
	 * as a const reference to a vector of strings.
	 */	
	const vector<string>& DelayModelTable::getKeyName() {
		return keyOfDelayModel_v;
	}


	DelayModelTable::DelayModelTable(ASDM &c) : container(c) {

		// Define a default entity.
		entity.setEntityId(EntityId("uid://X0/X0/X0"));
		entity.setEntityIdEncrypted("na");
		entity.setEntityTypeName("DelayModelTable");
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
 * A destructor for DelayModelTable.
 */
	DelayModelTable::~DelayModelTable() {
		for (unsigned int i = 0; i < privateRows.size(); i++) 
			delete(privateRows.at(i));
	}

	/**
	 * Container to which this table belongs.
	 */
	ASDM &DelayModelTable::getContainer() const {
		return container;
	}

	/**
	 * Return the number of rows in the table.
	 */
	unsigned int DelayModelTable::size() const {
		if (presentInMemory) 
			return privateRows.size();
		else
			return declaredSize;
	}
	
	/**
	 * Return the name of this table.
	 */
	string DelayModelTable::getName() const {
		return entityNameOfDelayModel;
	}
	
	/**
	 * Return the name of this table.
	 */
	string DelayModelTable::name() {
		return entityNameOfDelayModel;
	}
	
	/**
	 * Return the the names of the attributes (or columns) of this table.
	 */
	const vector<string>& DelayModelTable::getAttributesNames() { return attributesNamesOfDelayModel_v; }
	
	/**
	 * Return the the names of the attributes (or columns) of this table as they appear by default
	 * in an binary export of this table.
	 */
	const vector<string>& DelayModelTable::defaultAttributesNamesInBin() { return attributesNamesInBinOfDelayModel_v; }

	/**
	 * Return this table's Entity.
	 */
	Entity DelayModelTable::getEntity() const {
		return entity;
	}

	/**
	 * Set this table's Entity.
	 */
	void DelayModelTable::setEntity(Entity e) {
		this->entity = e; 
	}
	
	//
	// ====> Row creation.
	//
	
	/**
	 * Create a new row.
	 */
	DelayModelRow *DelayModelTable::newRow() {
		return new DelayModelRow (*this);
	}
	

	/**
	 * Create a new row initialized to the specified values.
	 * @return a pointer on the created and initialized row.
	
 	 * @param antennaId 
	
 	 * @param spectralWindowId 
	
 	 * @param timeInterval 
	
 	 * @param numPoly 
	
 	 * @param phaseDelay 
	
 	 * @param phaseDelayRate 
	
 	 * @param groupDelay 
	
 	 * @param groupDelayRate 
	
 	 * @param fieldId 
	
     */
	DelayModelRow* DelayModelTable::newRow(Tag antennaId, Tag spectralWindowId, ArrayTimeInterval timeInterval, int numPoly, vector<double > phaseDelay, vector<double > phaseDelayRate, vector<double > groupDelay, vector<double > groupDelayRate, Tag fieldId){
		DelayModelRow *row = new DelayModelRow(*this);
			
		row->setAntennaId(antennaId);
			
		row->setSpectralWindowId(spectralWindowId);
			
		row->setTimeInterval(timeInterval);
			
		row->setNumPoly(numPoly);
			
		row->setPhaseDelay(phaseDelay);
			
		row->setPhaseDelayRate(phaseDelayRate);
			
		row->setGroupDelay(groupDelay);
			
		row->setGroupDelayRate(groupDelayRate);
			
		row->setFieldId(fieldId);
	
		return row;		
	}	
	


DelayModelRow* DelayModelTable::newRow(DelayModelRow* row) {
	return new DelayModelRow(*this, *row);
}

	//
	// Append a row to its table.
	//

	
		
	
		
		
	/** 
	 * Returns a string built by concatenating the ascii representation of the
	 * parameters values suffixed with a "_" character.
	 */
	 string DelayModelTable::Key(Tag antennaId, Tag spectralWindowId) {
	 	ostringstream ostrstr;
	 		ostrstr  
			
				<<  antennaId.toString()  << "_"
			
				<<  spectralWindowId.toString()  << "_"
			
			;
		return ostrstr.str();	 	
	 }
	 
			
			
	DelayModelRow* DelayModelTable::add(DelayModelRow* x) {
		ArrayTime startTime = x->getTimeInterval().getStart();

		/*
	 	 * Is there already a context for this combination of not temporal 
	 	 * attributes ?
	 	 */
		string k = Key(
						x->getAntennaId()
					   ,
						x->getSpectralWindowId()
					   );
 
		if (context.find(k) == context.end()) { 
			// There is not yet a context ...
			// Create and initialize an entry in the context map for this combination....
			TIME_ROWS v;
			context[k] = v;			
		}
		
		return insertByStartTime(x, context[k]);
	}	
			
		
	
		
	void DelayModelTable::addWithoutCheckingUnique(DelayModelRow * x) {
		DelayModelRow * dummy = checkAndAdd(x, true); // We require the check for uniqueness to be skipped.
		                                           // by passing true in the second parameter
		                                           // whose value by default is false.
                if (false) cout << (unsigned long long) dummy;
	}
	

	


	// 
	// A private method to append a row to its table, used by input conversion
	// methods, with row uniqueness.
	//

	
	
		
		
			
			
			
			
	DelayModelRow*  DelayModelTable::checkAndAdd(DelayModelRow* x, bool ) {
		string keystr = Key( 
						x->getAntennaId() 
					   , 
						x->getSpectralWindowId() 
					   ); 
		if (context.find(keystr) == context.end()) {
			vector<DelayModelRow *> v;
			context[keystr] = v;
		}
		
		vector<DelayModelRow*>& found = context.find(keystr)->second;
		return insertByStartTime(x, found);			
	}
			
					
		



	//
	// A private method to brutally append a row to its table, without checking for row uniqueness.
	//

	void DelayModelTable::append(DelayModelRow *x) {
		privateRows.push_back(x);
		x->isAdded(true);
	}





	 vector<DelayModelRow *> DelayModelTable::get() {
	 	checkPresenceInMemory();
	    return privateRows;
	 }
	 
	 const vector<DelayModelRow *>& DelayModelTable::get() const {
	 	const_cast<DelayModelTable&>(*this).checkPresenceInMemory();	
	    return privateRows;
	 }	 
	 	




	

	
	
		
	 vector<DelayModelRow *> *DelayModelTable::getByContext(Tag antennaId, Tag spectralWindowId) {
	 	//if (getContainer().checkRowUniqueness() == false)
	 		//throw IllegalAccessException ("The method 'getByContext' can't be called because the dataset has been built without checking the row uniqueness.", "DelayModelTable");

	 	checkPresenceInMemory();
	  	string k = Key(antennaId, spectralWindowId);
 
	    if (context.find(k) == context.end()) return 0;
 	   else return &(context[k]);		
	}		
		
	


	
		
		
			
			
			
/*
 ** Returns a DelayModelRow* given a key.
 ** @return a pointer to the row having the key whose values are passed as parameters, or 0 if
 ** no row exists for that key.
 **
 */
 				
				
	DelayModelRow* DelayModelTable::getRowByKey(Tag antennaId, Tag spectralWindowId, ArrayTimeInterval timeInterval)  {
		checkPresenceInMemory();
 		string keystr = Key(antennaId, spectralWindowId);
 		vector<DelayModelRow *> row;
 		
 		if ( context.find(keystr)  == context.end()) return 0;
 		
 		row = context[keystr];
 		
 		// Is the vector empty...impossible in principle !
 		if (row.size() == 0) return 0;
 		
 		// Only one element in the vector
 		if (row.size() == 1) {
 			DelayModelRow* r = row.at(0);
 			if ( r->getTimeInterval().contains(timeInterval.getStart()))
 				return r;
 			else
 				return 0;
 		}
 		
 		// Optimizations
 		DelayModelRow* last = row.at(row.size()-1);
 		if (timeInterval.getStart().get() >= (last->getTimeInterval().getStart().get()+last->getTimeInterval().getDuration().get())) return 0;
 		
 		DelayModelRow* first = row.at(0);
 		if (timeInterval.getStart().get() < first->getTimeInterval().getStart().get()) return 0;
 		
 		
 		// More than one row 
 		// Let's use a dichotomy method for the general case..	
 		int k0 = 0;
 		int k1 = row.size() - 1;
 		DelayModelRow* r = 0;
 		while (k0!=k1) {
 		
 			// Is the start time contained in the time interval of row #k0
 			r = row.at(k0);
 			if (r->getTimeInterval().contains(timeInterval.getStart())) return r;
 			
 			// Is the start contained in the time interval of row #k1
 			r = row.at(k1);
			if (r->getTimeInterval().contains(timeInterval.getStart())) return r;
			
			// Are the rows #k0 and #k1 consecutive
			// Then we know for sure that there is no row containing the start of timeInterval
			if (k1==(k0+1)) return 0;
			
			// Proceed to the next step of dichotomy.
			r = row.at((k0+k1)/2);
			if ( timeInterval.getStart().get() <= r->getTimeInterval().getStart().get())
				k1 = (k0 + k1) / 2;
			else
				k0 = (k0 + k1) / 2;
		}
		return 0;	
	}
							
			
		
		
		
	



#ifndef WITHOUT_ACS
	using asdmIDL::DelayModelTableIDL;
#endif

#ifndef WITHOUT_ACS
	// Conversion Methods

	DelayModelTableIDL *DelayModelTable::toIDL() {
		DelayModelTableIDL *x = new DelayModelTableIDL ();
		unsigned int nrow = size();
		x->row.length(nrow);
		vector<DelayModelRow*> v = get();
		for (unsigned int i = 0; i < nrow; ++i) {
			//x->row[i] = *(v[i]->toIDL());
			v[i]->toIDL(x->row[i]);
		}
		return x;
	}
	
	void DelayModelTable::toIDL(asdmIDL::DelayModelTableIDL& x) const {
		unsigned int nrow = size();
		x.row.length(nrow);
		vector<DelayModelRow*> v = get();
		for (unsigned int i = 0; i < nrow; ++i) {
			v[i]->toIDL(x.row[i]);
		}
	}	
#endif
	
#ifndef WITHOUT_ACS
	void DelayModelTable::fromIDL(DelayModelTableIDL x) {
		unsigned int nrow = x.row.length();
		for (unsigned int i = 0; i < nrow; ++i) {
			DelayModelRow *tmp = newRow();
			tmp->setFromIDL(x.row[i]);
			// checkAndAdd(tmp);
			add(tmp);
		}
	}	
#endif

	
	string DelayModelTable::toXML()  {
		string buf;

		buf.append("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?> ");
		buf.append("<DelayModelTable xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:dlymdl=\"http://Alma/XASDM/DelayModelTable\" xsi:schemaLocation=\"http://Alma/XASDM/DelayModelTable http://almaobservatory.org/XML/XASDM/3/DelayModelTable.xsd\" schemaVersion=\"3\" schemaRevision=\"-1\">\n");
	
		buf.append(entity.toXML());
		string s = container.getEntity().toXML();
		// Change the "Entity" tag to "ContainerEntity".
		buf.append("<Container" + s.substr(1,s.length() - 1)+" ");
		vector<DelayModelRow*> v = get();
		for (unsigned int i = 0; i < v.size(); ++i) {
			try {
				buf.append(v[i]->toXML());
			} catch (NoSuchRow e) {
			}
			buf.append("  ");
		}		
		buf.append("</DelayModelTable> ");
		return buf;
	}

	
	string DelayModelTable::getVersion() const {
		return version;
	}
	

	void DelayModelTable::fromXML(string& tableInXML)  {
		//
		// Look for a version information in the schemaVersion of the XML
		//
		xmlDoc *doc;
		#if LIBXML_VERSION >= 20703
doc = xmlReadMemory(tableInXML.data(), tableInXML.size(), "XMLTableHeader.xml", NULL, XML_PARSE_NOBLANKS|XML_PARSE_HUGE);
#else
doc = xmlReadMemory(tableInXML.data(), tableInXML.size(), "XMLTableHeader.xml", NULL, XML_PARSE_NOBLANKS);
#endif

		if ( doc == NULL )
			throw ConversionException("Failed to parse the xmlHeader into a DOM structure.", "DelayModel");
		
		xmlNode* root_element = xmlDocGetRootElement(doc);
   		if ( root_element == NULL || root_element->type != XML_ELEMENT_NODE )
      		throw ConversionException("Failed to retrieve the root element in the DOM structure.", "DelayModel");
      		
      	xmlChar * propValue = xmlGetProp(root_element, (const xmlChar *) "schemaVersion");
      	if ( propValue != 0 ) {
      		version = string( (const char*) propValue);
      		xmlFree(propValue);   		
      	}
      		     							
		Parser xml(tableInXML);
		if (!xml.isStr("<DelayModelTable")) 
			error();
		// cout << "Parsing a DelayModelTable" << endl;
		string s = xml.getElement("<Entity","/>");
		if (s.length() == 0) 
			error();
		Entity e;
		e.setFromXML(s);
		if (e.getEntityTypeName() != "DelayModelTable")
			error();
		setEntity(e);
		// Skip the container's entity; but, it has to be there.
		s = xml.getElement("<ContainerEntity","/>");
		if (s.length() == 0) 
			error();

		// Get each row in the table.
		s = xml.getElementContent("<row>","</row>");
		DelayModelRow *row;
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
				throw ConversionException(e1.getMessage(),"DelayModelTable");
			} 
			catch (UniquenessViolationException e1) {
				throw ConversionException(e1.getMessage(),"DelayModelTable");	
			}
			catch (...) {
				// cout << "Unexpected error in DelayModelTable::checkAndAdd called from DelayModelTable::fromXML " << endl;
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
				throw ConversionException(e1.getMessage(),"DelayModelTable");
			} 
			catch (...) {
				// cout << "Unexpected error in DelayModelTable::addWithoutCheckingUnique called from DelayModelTable::fromXML " << endl;
			}
		}				
				
				
		if (!xml.isStr("</DelayModelTable>")) 
		error();
			
		archiveAsBin = false;
		fileAsBin = false;
		
	}

	
	void DelayModelTable::error()  {
		throw ConversionException("Invalid xml document","DelayModel");
	}
	
	
	string DelayModelTable::MIMEXMLPart(const asdm::ByteOrder* byteOrder) {
		string UID = getEntity().getEntityId().toString();
		string withoutUID = UID.substr(6);
		string containerUID = getContainer().getEntity().getEntityId().toString();
		ostringstream oss;
		oss << "<?xml version='1.0'  encoding='ISO-8859-1'?>";
		oss << "\n";
		oss << "<DelayModelTable xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:dlymdl=\"http://Alma/XASDM/DelayModelTable\" xsi:schemaLocation=\"http://Alma/XASDM/DelayModelTable http://almaobservatory.org/XML/XASDM/3/DelayModelTable.xsd\" schemaVersion=\"3\" schemaRevision=\"-1\">\n";
		oss<< "<Entity entityId='"<<UID<<"' entityIdEncrypted='na' entityTypeName='DelayModelTable' schemaVersion='1' documentVersion='1'/>\n";
		oss<< "<ContainerEntity entityId='"<<containerUID<<"' entityIdEncrypted='na' entityTypeName='ASDM' schemaVersion='1' documentVersion='1'/>\n";
		oss << "<BulkStoreRef file_id='"<<withoutUID<<"' byteOrder='"<<byteOrder->toString()<<"' />\n";
		oss << "<Attributes>\n";

		oss << "<antennaId/>\n"; 
		oss << "<spectralWindowId/>\n"; 
		oss << "<timeInterval/>\n"; 
		oss << "<numPoly/>\n"; 
		oss << "<phaseDelay/>\n"; 
		oss << "<phaseDelayRate/>\n"; 
		oss << "<groupDelay/>\n"; 
		oss << "<groupDelayRate/>\n"; 
		oss << "<fieldId/>\n"; 

		oss << "<timeOrigin/>\n"; 
		oss << "<atmosphericGroupDelay/>\n"; 
		oss << "<atmosphericGroupDelayRate/>\n"; 
		oss << "<geometricDelay/>\n"; 
		oss << "<geometricDelayRate/>\n"; 
		oss << "<numLO/>\n"; 
		oss << "<LOOffset/>\n"; 
		oss << "<LOOffsetRate/>\n"; 
		oss << "<dispersiveDelay/>\n"; 
		oss << "<dispersiveDelayRate/>\n"; 
		oss << "<atmosphericDryDelay/>\n"; 
		oss << "<atmosphericWetDelay/>\n"; 
		oss << "<padDelay/>\n"; 
		oss << "<antennaDelay/>\n"; 
		oss << "<numReceptor/>\n"; 
		oss << "<polarizationType/>\n"; 
		oss << "<electronicDelay/>\n"; 
		oss << "<electronicDelayRate/>\n"; 
		oss << "<receiverDelay/>\n"; 
		oss << "<IFDelay/>\n"; 
		oss << "<LODelay/>\n"; 
		oss << "<crossPolarizationDelay/>\n"; 
		oss << "</Attributes>\n";		
		oss << "</DelayModelTable>\n";

		return oss.str();				
	}
	
	string DelayModelTable::toMIME(const asdm::ByteOrder* byteOrder) {
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

	
	void DelayModelTable::setFromMIME(const string & mimeMsg) {
    string xmlPartMIMEHeader = "Content-ID: <header.xml>\n\n";
    
    string binPartMIMEHeader = "--MIME_boundary\nContent-Type: binary/octet-stream\nContent-ID: <content.bin>\n\n";
    
    // Detect the XML header.
    string::size_type loc0 = mimeMsg.find(xmlPartMIMEHeader, 0);
    if ( loc0 == string::npos) {
      // let's try with CRLFs
      xmlPartMIMEHeader = "Content-ID: <header.xml>\r\n\r\n";
      loc0 = mimeMsg.find(xmlPartMIMEHeader, 0);
      if  ( loc0 == string::npos ) 
	      throw ConversionException("Failed to detect the beginning of the XML header", "DelayModel");
    }

    loc0 += xmlPartMIMEHeader.size();
    
    // Look for the string announcing the binary part.
    string::size_type loc1 = mimeMsg.find( binPartMIMEHeader, loc0 );
    
    if ( loc1 == string::npos ) {
      throw ConversionException("Failed to detect the beginning of the binary part", "DelayModel");
    }
    
    //
    // Extract the xmlHeader and analyze it to find out what is the byte order and the sequence
    // of attribute names.
    //
    string xmlHeader = mimeMsg.substr(loc0, loc1-loc0);
    xmlDoc *doc;
    doc = xmlReadMemory(xmlHeader.data(), xmlHeader.size(), "BinaryTableHeader.xml", NULL, XML_PARSE_NOBLANKS);
    if ( doc == NULL ) 
      throw ConversionException("Failed to parse the xmlHeader into a DOM structure.", "DelayModel");
    
   // This vector will be filled by the names of  all the attributes of the table
   // in the order in which they are expected to be found in the binary representation.
   //
    vector<string> attributesSeq;
      
    xmlNode* root_element = xmlDocGetRootElement(doc);
    if ( root_element == NULL || root_element->type != XML_ELEMENT_NODE )
      throw ConversionException("Failed to parse the xmlHeader into a DOM structure.", "DelayModel");
    
    const ByteOrder* byteOrder=0;
    if ( string("ASDMBinaryTable").compare((const char*) root_element->name) == 0) {
      // Then it's an "old fashioned" MIME file for tables.
      // Just try to deserialize it with Big_Endian for the bytes ordering.
      byteOrder = asdm::ByteOrder::Big_Endian;
      
 	 //
    // Let's consider a  default order for the sequence of attributes.
    //
    
    	 
    attributesSeq.push_back("antennaId") ; 
    	 
    attributesSeq.push_back("spectralWindowId") ; 
    	 
    attributesSeq.push_back("timeInterval") ; 
    	 
    attributesSeq.push_back("numPoly") ; 
    	 
    attributesSeq.push_back("phaseDelay") ; 
    	 
    attributesSeq.push_back("phaseDelayRate") ; 
    	 
    attributesSeq.push_back("groupDelay") ; 
    	 
    attributesSeq.push_back("groupDelayRate") ; 
    	 
    attributesSeq.push_back("fieldId") ; 
    	
    	 
    attributesSeq.push_back("timeOrigin") ; 
    	 
    attributesSeq.push_back("atmosphericGroupDelay") ; 
    	 
    attributesSeq.push_back("atmosphericGroupDelayRate") ; 
    	 
    attributesSeq.push_back("geometricDelay") ; 
    	 
    attributesSeq.push_back("geometricDelayRate") ; 
    	 
    attributesSeq.push_back("numLO") ; 
    	 
    attributesSeq.push_back("LOOffset") ; 
    	 
    attributesSeq.push_back("LOOffsetRate") ; 
    	 
    attributesSeq.push_back("dispersiveDelay") ; 
    	 
    attributesSeq.push_back("dispersiveDelayRate") ; 
    	 
    attributesSeq.push_back("atmosphericDryDelay") ; 
    	 
    attributesSeq.push_back("atmosphericWetDelay") ; 
    	 
    attributesSeq.push_back("padDelay") ; 
    	 
    attributesSeq.push_back("antennaDelay") ; 
    	 
    attributesSeq.push_back("numReceptor") ; 
    	 
    attributesSeq.push_back("polarizationType") ; 
    	 
    attributesSeq.push_back("electronicDelay") ; 
    	 
    attributesSeq.push_back("electronicDelayRate") ; 
    	 
    attributesSeq.push_back("receiverDelay") ; 
    	 
    attributesSeq.push_back("IFDelay") ; 
    	 
    attributesSeq.push_back("LODelay") ; 
    	 
    attributesSeq.push_back("crossPolarizationDelay") ; 
    	
     
    
    
    // And decide that it has version == "2"
    version = "2";         
     }
    else if (string("DelayModelTable").compare((const char*) root_element->name) == 0) {
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
      	throw ConversionException ("Could not find the element '/DelayModelTable/BulkStoreRef'. Invalid XML header '"+ xmlHeader + "'.", "DelayModel");
      	
      // We found BulkStoreRef, now look for its attribute byteOrder.
      _xmlAttr* byteOrderAttr = 0;
      for (struct _xmlAttr* attr = bulkStoreRef->properties; attr; attr = attr->next) 
	  if (string("byteOrder").compare((const char*) attr->name) == 0) {
	   byteOrderAttr = attr;
	   break;
	 }
      
      if (byteOrderAttr == 0) 
	     throw ConversionException("Could not find the element '/DelayModelTable/BulkStoreRef/@byteOrder'. Invalid XML header '" + xmlHeader +"'.", "DelayModel");
      
      string byteOrderValue = string((const char*) byteOrderAttr->children->content);
      if (!(byteOrder = asdm::ByteOrder::fromString(byteOrderValue)))
		throw ConversionException("No valid value retrieved for the element '/DelayModelTable/BulkStoreRef/@byteOrder'. Invalid XML header '" + xmlHeader + "'.", "DelayModel");
		
	 //
	 // 2nd) Look for the Attributes element and grab the names of the elements it contains.
	 //
	 xmlNode* attributes = bulkStoreRef->next;
     if ( attributes == 0 || (attributes->type != XML_ELEMENT_NODE)  || (string("Attributes").compare((const char*) attributes->name) != 0))	 
       	throw ConversionException ("Could not find the element '/DelayModelTable/Attributes'. Invalid XML header '"+ xmlHeader + "'.", "DelayModel");
 
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
				DelayModelRow* aRow = DelayModelRow::fromBin((EndianIStream&) eiss, *this, attributesSeq);
				checkAndAdd(aRow);
      		}
    	}
    	catch (DuplicateKey e) {
      		throw ConversionException("Error while writing binary data , the message was "
				+ e.getMessage(), "DelayModel");
    	}
    	catch (TagFormatException e) {
     		 throw ConversionException("Error while reading binary data , the message was "
				+ e.getMessage(), "DelayModel");
    	}
    }
    else {
 		for (uint32_t i = 0; i < this->declaredSize; i++) {
			DelayModelRow* aRow = DelayModelRow::fromBin((EndianIStream&) eiss, *this, attributesSeq);
			append(aRow);
      	}   	
    }
    archiveAsBin = true;
    fileAsBin = true;
	}
	
	void DelayModelTable::setUnknownAttributeBinaryReader(const string& attributeName, BinaryAttributeReaderFunctor* barFctr) {
		//
		// Is this attribute really unknown ?
		//
		for (vector<string>::const_iterator iter = attributesNamesOfDelayModel_v.begin(); iter != attributesNamesOfDelayModel_v.end(); iter++) {
			if ((*iter).compare(attributeName) == 0) 
				throw ConversionException("the attribute '"+attributeName+"' is known you can't override the way it's read in the MIME binary file containing the table.", "DelayModel"); 
		}
		
		// Ok then register the functor to activate when an unknown attribute is met during the reading of a binary table?
		unknownAttributes2Functors[attributeName] = barFctr;
	}
	
	BinaryAttributeReaderFunctor* DelayModelTable::getUnknownAttributeBinaryReader(const string& attributeName) const {
		map<string, BinaryAttributeReaderFunctor*>::const_iterator iter = unknownAttributes2Functors.find(attributeName);
		return (iter == unknownAttributes2Functors.end()) ? 0 : iter->second;
	}

	
	void DelayModelTable::toFile(string directory) {
		if (!directoryExists(directory.c_str()) &&
			!createPath(directory.c_str())) {
			throw ConversionException("Could not create directory " , directory);
		}

		string fileName = directory + "/DelayModel.xml";
		ofstream tableout(fileName.c_str(),ios::out|ios::trunc);
		if (tableout.rdstate() == ostream::failbit)
			throw ConversionException("Could not open file " + fileName + " to write ", "DelayModel");
		if (fileAsBin) 
			tableout << MIMEXMLPart();
		else
			tableout << toXML() << endl;
		tableout.close();
		if (tableout.rdstate() == ostream::failbit)
			throw ConversionException("Could not close file " + fileName, "DelayModel");

		if (fileAsBin) {
			// write the bin serialized
			string fileName = directory + "/DelayModel.bin";
			ofstream tableout(fileName.c_str(),ios::out|ios::trunc);
			if (tableout.rdstate() == ostream::failbit)
				throw ConversionException("Could not open file " + fileName + " to write ", "DelayModel");
			tableout << toMIME() << endl;
			tableout.close();
			if (tableout.rdstate() == ostream::failbit)
				throw ConversionException("Could not close file " + fileName, "DelayModel");
		}
	}

	
	void DelayModelTable::setFromFile(const string& directory) {		
    if (boost::filesystem::exists(boost::filesystem::path(uniqSlashes(directory + "/DelayModel.xml"))))
      setFromXMLFile(directory);
    else if (boost::filesystem::exists(boost::filesystem::path(uniqSlashes(directory + "/DelayModel.bin"))))
      setFromMIMEFile(directory);
    else
      throw ConversionException("No file found for the DelayModel table", "DelayModel");
	}			

	
  void DelayModelTable::setFromMIMEFile(const string& directory) {
    string tablePath ;
    
    tablePath = directory + "/DelayModel.bin";
    ifstream tablefile(tablePath.c_str(), ios::in|ios::binary);
    if (!tablefile.is_open()) { 
      throw ConversionException("Could not open file " + tablePath, "DelayModel");
    }
    // Read in a stringstream.
    stringstream ss; ss << tablefile.rdbuf();
    
    if (tablefile.rdstate() == istream::failbit || tablefile.rdstate() == istream::badbit) {
      throw ConversionException("Error reading file " + tablePath,"DelayModel");
    }
    
    // And close.
    tablefile.close();
    if (tablefile.rdstate() == istream::failbit)
      throw ConversionException("Could not close file " + tablePath,"DelayModel");
    
    setFromMIME(ss.str());
  }	
/* 
  void DelayModelTable::openMIMEFile (const string& directory) {
  		
  	// Open the file.
  	string tablePath ;
    tablePath = directory + "/DelayModel.bin";
    ifstream tablefile(tablePath.c_str(), ios::in|ios::binary);
    if (!tablefile.is_open())
      throw ConversionException("Could not open file " + tablePath, "DelayModel");
      
	// Locate the xmlPartMIMEHeader.
    string xmlPartMIMEHeader = "CONTENT-ID: <HEADER.XML>\n\n";
    CharComparator comparator;
    istreambuf_iterator<char> BEGIN(tablefile.rdbuf());
    istreambuf_iterator<char> END;
    istreambuf_iterator<char> it = search(BEGIN, END, xmlPartMIMEHeader.begin(), xmlPartMIMEHeader.end(), comparator);
    if (it == END) 
    	throw ConversionException("failed to detect the beginning of the XML header", "DelayModel");
    
    // Locate the binaryPartMIMEHeader while accumulating the characters of the xml header.	
    string binPartMIMEHeader = "--MIME_BOUNDARY\nCONTENT-TYPE: BINARY/OCTET-STREAM\nCONTENT-ID: <CONTENT.BIN>\n\n";
    string xmlHeader;
   	CharCompAccumulator compaccumulator(&xmlHeader, 100000);
   	++it;
   	it = search(it, END, binPartMIMEHeader.begin(), binPartMIMEHeader.end(), compaccumulator);
   	if (it == END) 
   		throw ConversionException("failed to detect the beginning of the binary part", "DelayModel");
   	
	cout << xmlHeader << endl;
	//
	// We have the xmlHeader , let's parse it.
	//
	xmlDoc *doc;
    doc = xmlReadMemory(xmlHeader.data(), xmlHeader.size(), "BinaryTableHeader.xml", NULL, XML_PARSE_NOBLANKS);
    if ( doc == NULL ) 
      throw ConversionException("Failed to parse the xmlHeader into a DOM structure.", "DelayModel");
    
   // This vector will be filled by the names of  all the attributes of the table
   // in the order in which they are expected to be found in the binary representation.
   //
    vector<string> attributesSeq(attributesNamesInBinOfDelayModel_v);
      
    xmlNode* root_element = xmlDocGetRootElement(doc);
    if ( root_element == NULL || root_element->type != XML_ELEMENT_NODE )
      throw ConversionException("Failed to parse the xmlHeader into a DOM structure.", "DelayModel");
    
    const ByteOrder* byteOrder=0;
    if ( string("ASDMBinaryTable").compare((const char*) root_element->name) == 0) {
      // Then it's an "old fashioned" MIME file for tables.
      // Just try to deserialize it with Big_Endian for the bytes ordering.
      byteOrder = asdm::ByteOrder::Big_Endian;
        
      // And decide that it has version == "2"
    version = "2";         
     }
    else if (string("DelayModelTable").compare((const char*) root_element->name) == 0) {
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
      	throw ConversionException ("Could not find the element '/DelayModelTable/BulkStoreRef'. Invalid XML header '"+ xmlHeader + "'.", "DelayModel");
      	
      // We found BulkStoreRef, now look for its attribute byteOrder.
      _xmlAttr* byteOrderAttr = 0;
      for (struct _xmlAttr* attr = bulkStoreRef->properties; attr; attr = attr->next) 
	  if (string("byteOrder").compare((const char*) attr->name) == 0) {
	   byteOrderAttr = attr;
	   break;
	 }
      
      if (byteOrderAttr == 0) 
	     throw ConversionException("Could not find the element '/DelayModelTable/BulkStoreRef/@byteOrder'. Invalid XML header '" + xmlHeader +"'.", "DelayModel");
      
      string byteOrderValue = string((const char*) byteOrderAttr->children->content);
      if (!(byteOrder = asdm::ByteOrder::fromString(byteOrderValue)))
		throw ConversionException("No valid value retrieved for the element '/DelayModelTable/BulkStoreRef/@byteOrder'. Invalid XML header '" + xmlHeader + "'.", "DelayModel");
		
	 //
	 // 2nd) Look for the Attributes element and grab the names of the elements it contains.
	 //
	 xmlNode* attributes = bulkStoreRef->next;
     if ( attributes == 0 || (attributes->type != XML_ELEMENT_NODE)  || (string("Attributes").compare((const char*) attributes->name) != 0))	 
       	throw ConversionException ("Could not find the element '/DelayModelTable/Attributes'. Invalid XML header '"+ xmlHeader + "'.", "DelayModel");
 
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

	
void DelayModelTable::setFromXMLFile(const string& directory) {
    string tablePath ;
    
    tablePath = directory + "/DelayModel.xml";
    
    /*
    ifstream tablefile(tablePath.c_str(), ios::in|ios::binary);
    if (!tablefile.is_open()) { 
      throw ConversionException("Could not open file " + tablePath, "DelayModel");
    }
      // Read in a stringstream.
    stringstream ss;
    ss << tablefile.rdbuf();
    
    if  (tablefile.rdstate() == istream::failbit || tablefile.rdstate() == istream::badbit) {
      throw ConversionException("Error reading file '" + tablePath + "'", "DelayModel");
    }
    
    // And close
    tablefile.close();
    if (tablefile.rdstate() == istream::failbit)
      throw ConversionException("Could not close file '" + tablePath + "'", "DelayModel");

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
    	throw ConversionException("Caugth an exception whose message is '" + e.getMessage() + "'.", "DelayModel");
    }
    
    if (xmlDocument.find("<BulkStoreRef") != string::npos)
      setFromMIMEFile(directory);
    else
      fromXML(xmlDocument);
  }

	

	

			
	
		
		
	/**
	 * Insert a DelayModelRow* in a vector of DelayModelRow* so that it's ordered by ascending start time.
	 *
	 * @param DelayModelRow* x . The pointer to be inserted.
	 * @param vector <DelayModelRow*>& row. A reference to the vector where to insert x.
	 *
	 */
	 DelayModelRow* DelayModelTable::insertByStartTime(DelayModelRow* x, vector<DelayModelRow*>& row) {
				
		vector <DelayModelRow*>::iterator theIterator;
		
		ArrayTime start = x->timeInterval.getStart();

    	// Is the row vector empty ?
    	if (row.size() == 0) {
    		row.push_back(x);
    		privateRows.push_back(x);
    		x->isAdded(true);
    		return x;
    	}
    	
    	// Optimization for the case of insertion by ascending time.
    	DelayModelRow* last = *(row.end()-1);
        
    	if ( start > last->timeInterval.getStart() ) {
 	    	//
	    	// Modify the duration of last if and only if the start time of x
	    	// is located strictly before the end time of last.
	    	//
	  		if ( start < (last->timeInterval.getStart() + last->timeInterval.getDuration()))   		
    			last->timeInterval.setDuration(start - last->timeInterval.getStart());
    		row.push_back(x);
    		privateRows.push_back(x);
    		x->isAdded(true);
    		return x;
    	}
    	
    	// Optimization for the case of insertion by descending time.
    	DelayModelRow* first = *(row.begin());
        
    	if ( start < first->timeInterval.getStart() ) {
			//
	  		// Modify the duration of x if and only if the start time of first
	  		// is located strictly before the end time of x.
	  		//
	  		if ( first->timeInterval.getStart() < (start + x->timeInterval.getDuration()) )	  		
    			x->timeInterval.setDuration(first->timeInterval.getStart() - start);
    		row.insert(row.begin(), x);
    		privateRows.push_back(x);
    		x->isAdded(true);
    		return x;
    	}
    	
    	// Case where x has to be inserted inside row; let's use a dichotomy
    	// method to find the insertion index.
		unsigned int k0 = 0;
		unsigned int k1 = row.size() - 1;
	
		while (k0 != (k1 - 1)) {
			if (start == row[k0]->timeInterval.getStart()) {
				if (row[k0]->equalByRequiredValue(x))
					return row[k0];
				else
					throw DuplicateKey("DuplicateKey exception in ", "DelayModelTable");	
			}
			else if (start == row[k1]->timeInterval.getStart()) {
				if (row[k1]->equalByRequiredValue(x))
					return row[k1];
				else
					throw DuplicateKey("DuplicateKey exception in ", "DelayModelTable");	
			}
			else {
				if (start <= row[(k0+k1)/2]->timeInterval.getStart())
					k1 = (k0 + k1) / 2;
				else
					k0 = (k0 + k1) / 2;				
			} 	
		}

		if (start == row[k0]->timeInterval.getStart()) {
			if (row[k0]->equalByRequiredValue(x))
				return row[k0];
			else
				throw DuplicateKey("DuplicateKey exception in ", "DelayModelTable");	
		}
		else if (start == row[k1]->timeInterval.getStart()) {
			if (row[k1]->equalByRequiredValue(x))
				return row[k1];
			else
				throw DuplicateKey("DuplicateKey exception in ", "DelayModelTable");	
		}	

		row[k0]->timeInterval.setDuration(start-row[k0]->timeInterval.getStart());
		x->timeInterval.setDuration(row[k0+1]->timeInterval.getStart() - start);
		row.insert(row.begin()+(k0+1), x);
		privateRows.push_back(x);
   		x->isAdded(true);
		return x;   
    } 
    	
	
	

	
} // End namespace asdm
 
