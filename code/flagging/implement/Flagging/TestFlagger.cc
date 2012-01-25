///# TestFlagger.cc: this defines TestFlagger
//# Copyright (C) 2000,2001,2002
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
//# License for more details.
//#
//# You should have received a copy of the GNU Library General Public License
//# along with this library; if not, write to the Free Software Foundation,
//# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id$
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/ArrayLogical.h>
#include <casa/BasicSL/Complex.h>
#include <casa/Utilities/Regex.h>
#include <casa/OS/HostInfo.h>
#include <flagging/Flagging/TestFlagger.h>
#include <flagging/Flagging/FlagDataHandler.h>
#include <flagging/Flagging/FlagAgentBase.h>
#include <flagging/Flagging/FlagAgentSummary.h>
#include <tableplot/TablePlot/FlagVersion.h>
#include <casa/stdio.h>
#include <casa/math.h>
#include <stdarg.h>
#include <sstream>
#include <iostream>
#include <vector>

namespace casa {

const bool TestFlagger::dbg = false;


// TODO: add wrapping functions to existing fg.setdata(), fg.setclearflags(), etc?!


// -----------------------------------------------------------------------
// Default Constructor
// -----------------------------------------------------------------------
TestFlagger::TestFlagger ()
{
	fdh_p = NULL;
	summaryAgent_p = NULL;
	done();
}


TestFlagger::~TestFlagger ()
{
	done();
}

void
TestFlagger::done()
{
	if(fdh_p){
		delete fdh_p;
		fdh_p = NULL;
	}

	// Default values of parameters
	msname_p = "";
	iterationApproach_p = FlagDataHandler::COMPLETE_SCAN_UNMAPPED;
	timeInterval_p = 0.0;
	combinescans_p = false;
	spw_p = "";
	scan_p = "";
	field_p = "";
	antenna_p = "";
	timerange_p = "";
	correlation_p = "";
	intent_p = "";
	feed_p = "";
	array_p = "";
	uvrange_p = "";
	observation_p = "";

	max_p = 0.0;
	timeset_p = false;
	iterset_p = false;

	if (! dataselection_p.empty()) {
		Record temp;
		dataselection_p = temp;
	}

	if (! agentParams_p.empty()) {
		Record temp;
		agentParams_p = temp;
	}

	if(summaryAgent_p){
		summaryAgent_p = NULL;
	}

	mode_p = "";
	agents_config_list_p.clear();
	agents_list_p.clear();

	return;
}

// ---------------------------------------------------------------------
// TestFlagger::open
// Configure some parameters to the TestFlagger
// ---------------------------------------------------------------------
bool
TestFlagger::open(String msname, Double ntime)
{

	LogIO os(LogOrigin("TestFlagger", __FUNCTION__));

	if (msname.empty()) {
		os << LogIO::SEVERE << "No Measurement Set has been parsed"
				<< LogIO::POST;
		return False;
	}

	msname_p = msname;

	if (ntime)
		timeInterval_p = ntime;

	max_p = timeInterval_p;

	if(dbg){
		os << LogIO::NORMAL << "msname = " << msname_p << " ntime = " << timeInterval_p << LogIO::POST;
	}

	if(fdh_p) delete fdh_p;

	// create a FlagDataHandler object
	fdh_p = new FlagDataHandler(msname_p, iterationApproach_p, timeInterval_p);

	// Open the MS
	fdh_p->open();

	return true;
}

// ---------------------------------------------------------------------
// TestFlagger::selectData
// Parse parameters to the FlagDataHandler and select the data
// ---------------------------------------------------------------------
bool
TestFlagger::selectData(Record selrec)
{

	LogIO os(LogOrigin("TestFlagger", __FUNCTION__));
	if (dbg)
		os << LogIO::NORMAL << "Called from selectData(Record)" << LogIO::POST;


	if (! selrec.empty()) {

		dataselection_p = selrec;

		// Check if all the parameters are in the record. If not,
		// use the default values
		if (dataselection_p.isDefined("spw"))
			dataselection_p.get("spw", spw_p);
		if (dataselection_p.isDefined("scan"))
			dataselection_p.get("scan", scan_p);
		if (dataselection_p.isDefined("field"))
			dataselection_p.get("field", field_p);
		if (dataselection_p.isDefined("antenna"))
			dataselection_p.get("antenna", antenna_p);
		if (dataselection_p.isDefined("timerange"))
			dataselection_p.get("timerange", timerange_p);
		if (dataselection_p.isDefined("correlation"))
			dataselection_p.get("correlation", correlation_p);
		if (dataselection_p.isDefined("intent"))
			dataselection_p.get("intent", intent_p);
		if (dataselection_p.isDefined("feed"))
			dataselection_p.get("feed", feed_p);
		if (dataselection_p.isDefined("array"))
			dataselection_p.get("array", array_p);
		if (dataselection_p.isDefined("uvrange"))
			dataselection_p.get("uvrange", uvrange_p);
		if (dataselection_p.isDefined("observation"))
			dataselection_p.get("observation", observation_p);

	}

	bool ret_status = true;

	// Set the data selection
	ret_status = fdh_p->setDataSelection(dataselection_p);
	if (!ret_status) {
		os << LogIO::SEVERE << "Failed to set the data selection."
				<< LogIO::POST;
		return false;
	}

	// Select the data
	ret_status = fdh_p->selectData();
	if (!ret_status) {
		os << LogIO::SEVERE << "Failed to select the data."
				<< LogIO::POST;
		return false;
	}


	return true;
}


// ---------------------------------------------------------------------
// TestFlagger::selectData
// Parse parameters to the FlagDataHandler and select the data
// ---------------------------------------------------------------------
bool
TestFlagger::selectData(String field, String spw, String array,
						String feed, String scan, String antenna,
						String uvrange,  String timerange, String correlation,
						String intent, String observation)
{

	LogIO os(LogOrigin("TestFlagger", __FUNCTION__));

	if (dbg)
		os << LogIO::NORMAL << "Called from selectData(String....)" << LogIO::POST;

	// Create a record with the parameters
	Record selection = Record();

	selection.define("spw", spw);
	selection.define("scan", scan);
	selection.define("field", field);
	selection.define("antenna", antenna);
	selection.define("timerange", timerange);
	selection.define("correlation", correlation);
	selection.define("intent", intent);
	selection.define("feed", feed);
	selection.define("array", array);
	selection.define("uvrange", uvrange);
	selection.define("observation", observation);

	// Call the main selectData() method
	selectData(selection);

	return true;

}

// ---------------------------------------------------------------------
// DEPRECATED: TestFlagger::parseDataSelection
// Parse union of parameters to the FlagDataHandler
// ---------------------------------------------------------------------
bool
TestFlagger::parseDataSelection(Record selrec)
{

	LogIO os(LogOrigin("TestFlagger", __FUNCTION__));

	if(selrec.empty()) {
		return false;
	}

	dataselection_p = selrec;

	// Get the selection parameters
	dataselection_p.get("spw", spw_p);
	dataselection_p.get("scan", scan_p);
	dataselection_p.get("field", field_p);
	dataselection_p.get("antenna", antenna_p);
	dataselection_p.get("timerange", timerange_p);
	dataselection_p.get("correlation", correlation_p);
	dataselection_p.get("intent", intent_p);
	dataselection_p.get("feed", feed_p);
	dataselection_p.get("array", array_p);
	dataselection_p.get("uvrange", uvrange_p);
	dataselection_p.get("observation", observation_p);

	return true;
}

// ---------------------------------------------------------------------
// TestFlagger::parseAgentParameters
// Create a vector of agents and parameters
// Each input record contains data selection parameters
// and agent's specific parameters
// ---------------------------------------------------------------------
bool
TestFlagger::parseAgentParameters(Record agent_params)
{
	LogIO os(LogOrigin("TestFlagger", __FUNCTION__));

	String mode = "";
	String expression = "ABS ALL";
	String agent_name = "";

	// create a temporary vector of agents
	std::vector<Record> listOfAgents;


	if(agent_params.empty()){
		os << LogIO::SEVERE << "No agent's record has been provided."
				<< LogIO::POST;
		return false;
	}

	// Use the given Record of paramters
	agentParams_p = agent_params;

	if (! agentParams_p.isDefined("mode")) {
		os << LogIO::SEVERE << "No mode has been provided for the agent"
				<< LogIO::POST;
		return false;
	}

	// Get mode
	agentParams_p.get("mode", mode);

	// Name for the logging output
	if (! agentParams_p.isDefined("name"))
			agentParams_p.define("name", mode);


	agentParams_p.get("name", agent_name);

	// If there is a tfcrop or extend agent in the list,
	// get the maximum value of ntime and the combinescans parameter
	if (mode.compare("tfcrop") == 0 or mode.compare("extend") == 0) {
		Double ntime;
		if (agentParams_p.isDefined("ntime")){
			agentParams_p.get("ntime", ntime);
			getMax(ntime);
		}

		// Get the combinescans parameter. If any of them is True,
		// it will be True for the whole list
		Bool combine = false;
		if (agentParams_p.isDefined("combinescans"))
			agentParams_p.get("combinescans", combine);

		combinescans_p = combinescans_p || combine;

		os << LogIO::DEBUGGING << "max ntime="<<max_p<<" and combinescans="<< combinescans_p << LogIO::POST;

	}

	// Create one agent for each polarization
	if (mode.compare("tfcrop") == 0 or mode.compare("clip") == 0) {

		if (agentParams_p.isDefined("expression")) {

			agentParams_p.get("expression", expression);
		}
		else {
			agentParams_p.define("expression", expression);
		}

		// Is the expression polarization an ALL?
		if (isExpressionPolarizationAll(expression)) {

			// Get the complex unitary function (ABS, NORM, REAL, IMAG, ARG)
			String function = getExpressionFunction(expression);

			// Get all the polarizations in the MS
			std::vector<String> *allpol = fdh_p->corrProducts_p;

			for (size_t i=0; i < allpol->size(); i++){
				// compose the full expression
				String func = function;
				String pol = allpol->at(i);
				String exp = func.append(" ");
				exp = func.append(pol);

				// Save the record to a list of agents
				agentParams_p.define("expression", exp);
				String name = agent_name;
				name = name.append("_");
				name = name.append(pol);
				agentParams_p.define("name", name);
				listOfAgents.push_back(agentParams_p);
			}
		}
	}

	if (listOfAgents.size() > 0) {
		// add the agent(s) to the list
		for (size_t i=0; i < listOfAgents.size(); i++) {
			agents_config_list_p.push_back(listOfAgents.at(i));
		}
	}
	else {
		agents_config_list_p.push_back(agentParams_p);
	}

	if (dbg){
		for (size_t i=0; i < agents_config_list_p.size(); i++) {
			ostringstream os;
			agents_config_list_p.at(i).print(os);
			String str(os.str());
			cout << str << endl;
		}
	}

	return true;
}


// ---------------------------------------------------------------------
// DEPRECATED TestFlagger::initFlagDataHandler
// Initialize the FlagDataHandler
// ---------------------------------------------------------------------
bool
TestFlagger::initFlagDataHandler()
{

	bool ret_status = true;

	LogIO os(LogOrigin("TestFlagger", __FUNCTION__, WHERE));

	if (msname_p.empty()) {
		os << LogIO::SEVERE << "No Measurement Set has been parsed"
				<< LogIO::POST;
		return False;
	}

	if(fdh_p) delete fdh_p;

	// create a FlagDataHandler object
	fdh_p = new FlagDataHandler(msname_p, iterationApproach_p, timeInterval_p);

	// Open the MS
	fdh_p->open();

	if (dataselection_p.empty()) {
		return false;
	}

	// Set the data selection
	ret_status = fdh_p->setDataSelection(dataselection_p);
	if (!ret_status) {
		os << LogIO::SEVERE << "Failed to set the data selection."
				<< LogIO::POST;
		return false;
	}

	// Select the data
	ret_status = fdh_p->selectData();
	if (!ret_status) {
		os << LogIO::SEVERE << "Failed to select the data."
				<< LogIO::POST;
		return false;
	}


	return true;
}


// ---------------------------------------------------------------------
// TestFlagger::initAgents
// Initialize the Agents
// Call parseAgentParameters and selectData first
// ---------------------------------------------------------------------
bool
TestFlagger::initAgents()
{

    LogIO os(LogOrigin("TestFlagger",__FUNCTION__));

	if (agents_config_list_p.empty()){
		return false;
	}


	os<< LogIO::NORMAL<< "There are "<< agents_config_list_p.size()<< " agents in the list"<<LogIO::POST;

	// Check if list has a mixed state of apply and unapply parameters
	Bool mixed, apply0;
	size_t list_size = agents_config_list_p.size();
	if (list_size > 1){
		Record agent_rec = agents_config_list_p[0];
		agent_rec.get("apply", apply0);
		for (size_t j=1; j < list_size; j++)
		{
			Bool apply;
			agent_rec = agents_config_list_p[j];
			agent_rec.get("apply", apply);
			if (apply0 != apply){
				mixed = true;
				if (dbg) cout << "List has a mixed state"<<endl;
				break;
			}
			else {
				mixed = false;
			}
		}
	}
	else if (list_size == 1){
		mixed = false;
	}

	// Send the logging of the re-applying agents to the debug
	// as we are only interested in seeing the unapply action
	uChar loglevel = LogIO::DEBUGGING;

	// loop through the vector of agents
	for (size_t i=0; i < list_size; i++) {

		// get agent record
		Record agent_rec = agents_config_list_p[i];
		if (dbg){
			os<< LogIO::NORMAL<< "Record["<<i<<"].nfields()="<<agent_rec.nfields()<<LogIO::POST;
			ostringstream os;
			agent_rec.print(os);
			String str(os.str());
			cout << str << endl;

		}

		// TODO: should I check for fdh_p existence here?
		// Should it call initFlagDataHandler in case it doesn't exist?
		// call the factory method for each of the agent's records
		if(not fdh_p){
			os << LogIO::SEVERE << "FlagDataHandler has not been initialized."
					<< LogIO::POST;
			return false;
		}

		// Change the log level if apply=True in a mixed state
		Bool apply = true;;
		if (agent_rec.isDefined("apply")) {
			agent_rec.get("apply", apply);
		}

		if (mixed and apply){
			agent_rec.define("loglevel", loglevel);
		}

		// Get the mode
		String mode;
		agent_rec.get("mode", mode);

		// Set the new time interval only once
		if (!timeset_p and (mode.compare("tfcrop") == 0 or mode.compare("extend") == 0)) {
			fdh_p->setTimeInterval(max_p);
			timeset_p = true;
		}

		// Change the new iteration approach only once
		if (!iterset_p and (mode.compare("tfcrop") == 0 or mode.compare("extend") == 0)) {
			if (combinescans_p)
				fdh_p->setIterationApproach(FlagDataHandler::COMBINE_SCANS_MAP_ANTENNA_PAIRS_ONLY);
			else
				fdh_p->setIterationApproach(FlagDataHandler::COMPLETE_SCAN_MAP_ANTENNA_PAIRS_ONLY);

			iterset_p = true;
		}

		// TODO: Catch error, print a warning and continue to next agent.
		// Create this agent
		FlagAgentBase *fa = FlagAgentBase::create(fdh_p, agent_rec);

		// Get the summary agent to list the results later
		if (mode.compare("summary") == 0) {
			summaryAgent_p = (FlagAgentSummary *) fa;
		}

		// add the agent to the FlagAgentList
		agents_list_p.push_back(fa);

	}

	return true;
}


// ---------------------------------------------------------------------
// TestFlagger::run
// Run the agents
// It assumes that initAgents has been called first
// ---------------------------------------------------------------------
Record
TestFlagger::run(Bool writeflags, Bool sequential)
{

	LogIO os(LogOrigin("TestFlagger", __FUNCTION__));

	if (agents_list_p.empty()) {
		os << LogIO::SEVERE << "There is no agent to run in list"<< LogIO::POST;
		return Record();
	}


	// Use the maximum ntime of the list
	os << LogIO::DEBUGGING << "ntime for all agents will be "<< max_p << LogIO::POST;
	os << LogIO::DEBUGGING << "combinescans for all agents will be "<< combinescans_p << LogIO::POST;

	// Generate the iterators
	// It will iterate through the data to evaluate the necessary memory
	// and get the START and STOP values of the scans for the quack agent
	fdh_p->generateIterator();

	agents_list_p.start();
	if (dbg) cout << "size=" << agents_list_p.size()<<endl;

	// iterate over chunks
	while (fdh_p->nextChunk())
	{
		// iterate over visBuffers
		while (fdh_p->nextBuffer())
		{

			// Apply or unapply the flags, in sequential or in parallel
			agents_list_p.apply(sequential);

			// Flush flags to MS
			if (writeflags)
				fdh_p->flushFlags();
		}
		if (writeflags)
			agents_list_p.chunkSummary();
	}

	if (writeflags)
		agents_list_p.msSummary();

	agents_list_p.terminate();
	agents_list_p.join();

	// Get the record with the summary if there was any summary agent in the list
	Record summary_stats = Record();
	if (summaryAgent_p){
		summary_stats = summaryAgent_p->getResult();

/*		if(dbg){
			os << LogIO::NORMAL << "Get the summary results" << LogIO::POST;
			ostringstream os;
			summary_stats.print(os);
			String str(os.str());
			cout << str << endl;
		}*/
	}

	agents_list_p.clear();

	return summary_stats;
}

// ---------------------------------------------------------------------
// TestFlagger::isExpressionPolarizationAll
// Returns true if expression contains a polarization ALL
//
// ---------------------------------------------------------------------
bool
TestFlagger::isExpressionPolarizationAll(String expression)
{

	if (expression.find("ALL") == string::npos){
		return false;
	}

	return true;
}

// ---------------------------------------------------------------------
// TestFlagger::getExpressionFunction
// Get the unitary function of a polarization expression
// returns a String with the function name
//
// ---------------------------------------------------------------------
String
TestFlagger::getExpressionFunction(String expression)
{

	String func;

	// Parse complex unitary function
	if (expression.find("REAL") != string::npos)
	{
		func = "REAL";
	}
	else if (expression.find("IMAG") != string::npos)
	{
		func = "IMAG";
	}
	else if (expression.find("ARG") != string::npos)
	{
		func = "ARG";
	}
	else if (expression.find("ABS") != string::npos)
	{
		func = "ABS";
	}
	else if (expression.find("NORM") != string::npos)
	{
		func = "NORM";
	}
	else
	{
		return "";
	}

	return func;
}

void
TestFlagger::getMax(Double value)
{
	if (value > max_p)
		max_p = value;

	return;

}


// ---------------------------------------------------------------------
// TestFlagger::getFlagVersionList
// Get the flag versions list from the file FLAG_VERSION_LIST in the
// MS directory
//
// ---------------------------------------------------------------------
bool
TestFlagger::getFlagVersionList(Vector<String> &verlist)
{

	LogIO os(LogOrigin("TestFlagger", __FUNCTION__, WHERE));

	verlist.resize(0);
	Int num;

	FlagVersion fv(fdh_p->originalMeasurementSet_p->tableName(),"FLAG","FLAG_ROW");
	Vector<String> vlist = fv.getVersionList();

	num = verlist.nelements();
	verlist.resize( num + vlist.nelements() + 1, True );
	verlist[num] = String("\nMS : ") + fdh_p->originalMeasurementSet_p->tableName() + String("\n");

	for(Int j=0; j<(Int)vlist.nelements(); j++)
		verlist[num+j+1] = vlist[j];


	return true;
}


// ---------------------------------------------------------------------
// TestFlagger::printFlagSelection
// Get the flag versions list
//
// ---------------------------------------------------------------------
bool
TestFlagger::printFlagSelections()
{

	LogIO os(LogOrigin("TestFlagger", __FUNCTION__, WHERE));

	if (! agents_config_list_p.empty())
	{
//		os << "Current list of agents : " << agents_config_list_p << LogIO::POST;

		// TODO: loop through list
		// Duplicate the vector... ???
		for (size_t i=0; i < agents_config_list_p.size(); i++) {
			ostringstream out;
			Record agent_rec;
			agent_rec = agents_config_list_p.at(i);
			agent_rec.print(out);
			os << out.str() << LogIO::POST;
		}
		if (dbg)
			cout << "size of original list " << agents_config_list_p.size() << endl;

	}
	else os << " No current agents " << LogIO::POST;

	return true;
}


// ---------------------------------------------------------------------
// TestFlagger::saveFlagVersion
// Save the flag version
//
// ---------------------------------------------------------------------
bool
TestFlagger::saveFlagVersion(String versionname, String comment, String merge )
{
	LogIO os(LogOrigin("TestFlagger", __FUNCTION__, WHERE));

	FlagVersion fv(fdh_p->originalMeasurementSet_p->tableName(),"FLAG","FLAG_ROW");
	fv.saveFlagVersion(versionname, comment, merge);

	return true;
}


} //#end casa namespace
