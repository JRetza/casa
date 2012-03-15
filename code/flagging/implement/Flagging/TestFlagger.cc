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
#include <casa/Exceptions/Error.h>
#include <flagging/Flagging/TestFlagger.h>
#include <casa/stdio.h>
#include <casa/math.h>
#include <stdarg.h>
#include <sstream>
#include <iostream>
#include <vector>

namespace casa {

const bool TestFlagger::dbg = false;


// -----------------------------------------------------------------------
// Default Constructor
// -----------------------------------------------------------------------
TestFlagger::TestFlagger ()
{
	fdh_p = NULL;
	summaryAgent_p = NULL;
	displayAgent_p = NULL;

	done();
}


// -----------------------------------------------------------------------
// Default Destructor
// -----------------------------------------------------------------------
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
	iterationApproach_p = FlagDataHandler::SUB_INTEGRATION;
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

	dataselection_p = Record();

/*
	if (! dataselection_p.empty()) {
		Record temp;
		dataselection_p = temp;
	}
*/

	if (! agentParams_p.empty()) {
		Record temp;
		agentParams_p = temp;
	}

	if(summaryAgent_p){
		summaryAgent_p = NULL;
	}

	if(displayAgent_p){
		displayAgent_p = NULL;
	}

	mode_p = "";
	agents_config_list_p.clear();
	agents_config_list_copy_p.clear();
	agents_list_p.clear();

	return;
}

// ---------------------------------------------------------------------
// TestFlagger::open
// Create a FlagDataHandler object based on the input type:
// MS or a calibration file. Open the MS or cal table and
// attach it to the tool.
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
	Table table(msname_p,TableLock(TableLock::AutoNoReadLocking));
	TableInfo& info = table.tableInfo();
	String type=info.type();
	table.flush();
	table.relinquishAutoLocks(True);
	table.unlock();
	os << LogIO::NORMAL << "Table type is " << type << LogIO::POST;

	// For a measurement set
	if (type == "Measurement Set")
	{
		fdh_p = new FlagMSHandler(msname_p, iterationApproach_p, timeInterval_p);
	}
	// For a calibration file
	else
	{
		fdh_p = new FlagCalTableHandler(msname_p, iterationApproach_p, timeInterval_p);
	}

	// Open the MS
	fdh_p->open();

	return true;
}

// ---------------------------------------------------------------------
// TestFlagger::selectData
// Get a record with data selection parameters and
// Parse it to the FlagDataHandler to select the data
// ---------------------------------------------------------------------
bool
TestFlagger::selectData(Record selrec)
{

	LogIO os(LogOrigin("TestFlagger", __FUNCTION__));
	if (dbg)
		os << LogIO::NORMAL << "Called from selectData(Record)" << LogIO::POST;

	if (! fdh_p){
		os << LogIO::SEVERE << "There is no MS attached. Please run tf.open first." << LogIO::POST;
		return false;
	}


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
// Create a record with data selection parameters.
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
// TestFlagger::parseAgentParameters
// Create a vector of agents and parameters
// Each input record contains data selection parameters
// and agent's specific parameters
// ---------------------------------------------------------------------
bool
TestFlagger::parseAgentParameters(Record agent_params)
{
	LogIO os(LogOrigin("TestFlagger", __FUNCTION__));

	if (! fdh_p){
		os << LogIO::SEVERE << "There is no MS attached. Please run tf.open first." << LogIO::POST;
		return false;
	}

	// Default values for some parameters
	String mode = "";
	String agent_name = "";
	Bool apply = true;

	// create a temporary vector of agents
	std::vector<Record> listOfAgents;


	if(agent_params.empty()){
		os << LogIO::SEVERE << "No agent's record has been provided."
				<< LogIO::POST;
		return false;
	}

	// Use the given Record of parameters
	agentParams_p = agent_params;

	if (! agentParams_p.isDefined("mode")) {
		os << LogIO::WARN << "No mode has been provided"
				<< LogIO::POST;
		return false;
	}

	// Get mode
	agentParams_p.get("mode", mode);

	// Validate mode against known modes
	if (! isModeValid(mode)){
		os << LogIO::WARN << "Mode "<< mode << " is not valid or doesn't exist"
				<< LogIO::POST;
		return false;
	}


	// Name for the logging output
	if (! agentParams_p.isDefined("name")){
		agent_name = mode;
		agent_name.capitalize();
		agentParams_p.define("name", agent_name);

	}

	agentParams_p.get("name", agent_name);

	// Enforce a default value for the apply parameter
	if (! agentParams_p.isDefined("apply")){
		agentParams_p.define("apply", apply);
	}

	// If there is a tfcrop, extend or rflag agent in the list,
	// get the maximum value of ntime and the combinescans parameter
	if (mode.compare("tfcrop") == 0 or mode.compare("extend") == 0 or mode.compare("rflag") == 0) {
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

		os << LogIO::DEBUGGING << "max ntime="<<max_p<<" and combinescans="<<
				combinescans_p << LogIO::POST;

	}

	// Activate async i/o if tfcrop/clip/rflag is present
	if (mode.compare("tfcrop") == 0 or mode.compare("clip") == 0 or mode.compare("rflag") == 0)
	{
		fdh_p->enableAsyncIO(true);
	}

	// Make correlation always uppercase
	// Default for all modes
	String correlation = "";
	if (agentParams_p.isDefined("correlation")) {

		agentParams_p.get("correlation", correlation);
		correlation.upcase();
		agentParams_p.define("correlation", correlation);
	}

	// Create one agent for each polarization
	if (mode.compare("tfcrop") == 0) {

		if (not agentParams_p.isDefined("correlation")) {
			// Default for tfcrop
			correlation = "ABS_ALL";
			agentParams_p.define("correlation", correlation);
		}
		if (dbg){
			cout << "mode="<<mode<<" correlation="<<correlation<<endl;
		}

		// Is the expression polarization an ALL?
		if (isExpressionPolarizationAll(correlation)) {

			// Get the complex unitary function (ABS, NORM, REAL, IMAG, ARG)
			String function = getExpressionFunction(correlation);

			// Get all the polarizations in the MS
			std::vector<String> *allpol = fdh_p->corrProducts_p;

			for (size_t i=0; i < allpol->size(); i++){
				// compose the full expression
				String func = function;
				String pol = allpol->at(i);
				String exp = func.append(" ");
				exp = func.append(pol);

				// Save the record to a list of agents
				agentParams_p.define("correlation", exp);
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

	// Keep a copy of the vector of Agents parameters
	agents_config_list_copy_p = agents_config_list_p;

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

	if (! fdh_p){
		os << LogIO::SEVERE << "There is no MS attached. Please run tf.open first." << LogIO::POST;
		return false;
	}

	if (dataselection_p.empty()){
		os << LogIO::SEVERE << "There is no MS Selection available." << LogIO::POST;
		return false;
	}

	if (agents_config_list_p.empty()){
		return false;
	}

	os<< LogIO::DEBUGGING<< "There are initially "<< agents_config_list_p.size()<<
			" agents in the list"<<LogIO::POST;

	// Check if list has a mixed state of apply and unapply parameters
	// If the list contains apply=True and False, the apply=True list will
	// be hidden in the debug.
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
				os << LogIO::DEBUGGING << "List has a mixed state"<<LogIO::POST;
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
	// as we are only interested in seeing the unapply action (tflagcmd)
	uChar loglevel = LogIO::DEBUGGING;
	Bool retstate = true;

		// Loop through the vector of agents
		for (size_t i=0; i < list_size; i++) {

			// Get agent record
			Record agent_rec = agents_config_list_p[i];
			if (dbg){
				os<< LogIO::NORMAL<< "Record["<<i<<"].nfields()="<<agent_rec.nfields()<<LogIO::POST;
				ostringstream os;
				agent_rec.print(os);
				String str(os.str());
				cout << str << endl;

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
			if (!timeset_p and (mode.compare("tfcrop") == 0 or mode.compare("extend") == 0 or
					mode.compare("rflag") == 0)) {
				fdh_p->setTimeInterval(max_p);
				timeset_p = true;
			}

			// Change the new iteration approach only once
			if (!iterset_p and (mode.compare("tfcrop") == 0 or mode.compare("extend") == 0
				or mode.compare("rflag") == 0 or mode.compare("display") == 0)) {
				if (combinescans_p)
					fdh_p->setIterationApproach(FlagDataHandler::COMBINE_SCANS_MAP_ANTENNA_PAIRS_ONLY);
				else
					fdh_p->setIterationApproach(FlagDataHandler::COMPLETE_SCAN_MAP_ANTENNA_PAIRS_ONLY);

				iterset_p = true;
			}

			FlagAgentBase *fa=NULL;

			try
			{
				// Create this agent
				FlagAgentBase *tfa = FlagAgentBase::create(fdh_p, agent_rec);
				fa = tfa;

			}
			catch(AipsError x)
			{
			  fa=NULL;
			  // Send out a useful message, and stop adding agents to the list.
			  // All valid agents before the problematic one, will remain in agents_list_p
			  // A subsequent call to initAgents() will add to the list.
			  ostringstream oss;
			  agent_rec.print(oss);
			  String recstr(oss.str());
			  while(recstr.contains('\n'))
			    {
			      recstr = recstr.replace(recstr.index('\n'),1,", ");
			    }

			  os << LogIO::SEVERE << "Error in creating agent : " << x.getMesg() << endl
					  << "Input parameters : " << recstr << LogIO::POST;
			  retstate=false;
			  break;

			}

				if (fa == NULL){
					String name;
					agent_rec.get("name",name);
					os << LogIO::WARN << "Agent "<< name<< " is NULL. Skipping it."<<LogIO::POST;
					continue;
				}

				// Get the last summary agent to list the results back to the task
				if (mode.compare("summary") == 0) {
					summaryAgent_p = (FlagAgentSummary *) fa;
				}

				// Get the display agent.
				if (mode.compare("display") == 0){
					displayAgent_p = (FlagAgentDisplay *) fa;
				}

				// Add the agent to the FlagAgentList
				agents_list_p.push_back(fa);


		}
		os << LogIO::NORMAL << "There are "<< agents_list_p.size()<<" valid agents in list"<<
				LogIO::POST;

	// Clear the list so that this method cannot be called twice
	agents_config_list_p.clear();

	return retstate;
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

	if (! fdh_p){
		os << LogIO::SEVERE << "There is no MS attached. Please run tf.open first." << LogIO::POST;
		return Record();
	}

	if (agents_list_p.empty()) {
		os << LogIO::SEVERE << "There is no agent to run in list"<< LogIO::POST;
		return Record();
	}


	// Use the maximum ntime of the list
	os << LogIO::DEBUGGING << "ntime for all agents will be "<< max_p << LogIO::POST;
	os << LogIO::DEBUGGING << "combinescans for all agents will be "<< combinescans_p << LogIO::POST;

	// Report to return
	FlagReport combinedReport;


	// Generate the iterators
	// It will iterate through the data to evaluate the necessary memory
	// and get the START and STOP values of the scans for the quack agent
	fdh_p->generateIterator();

	agents_list_p.start();
	os << LogIO::DEBUGGING << "Size of agent's list is " << agents_list_p.size()<< LogIO::POST;

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

		// Print the chunk summary stats
		agents_list_p.chunkSummary();
	}

	// Print the MS summary stats
	agents_list_p.msSummary();
	if (writeflags)
		os << LogIO::NORMAL << "=> " << "Writing flags to the MS" << LogIO::POST;
	else
		os << LogIO::NORMAL << "=> " << "Flags are not written to the MS (writeflags = False)"
			<< LogIO::POST;

	agents_list_p.terminate();
	agents_list_p.join();

	// Gather the display reports from all agents
//	FlagReport combinedReport = agents_list_p.gatherReports();
	combinedReport = agents_list_p.gatherReports();

	// Send reports to display agent
	if (displayAgent_p)
		displayAgent_p->displayReports(combinedReport);

	// Get the record with the summary if there was any summary agent in the list
//	Record summary_stats = Record();
//	if (summaryAgent_p){
//		summary_stats = summaryAgent_p->getResult();

//	}

	agents_list_p.clear();

//	return summary_stats;
	return combinedReport;
}

// ---------------------------------------------------------------------
// TestFlagger::isExpressionPolarizationAll
// Returns true if expression contains a polarization ALL
//
// ---------------------------------------------------------------------
bool
TestFlagger::isExpressionPolarizationAll(String correlation)
{

	if (correlation.find("ALL") == string::npos){
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

// ---------------------------------------------------------------------
// TestFlagger::getMax
// Get the maximum between two values and
// assign it to the max_p class member.
// ---------------------------------------------------------------------
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

	try
	{

		if (! fdh_p){
			os << LogIO::SEVERE << "There is no MS attached. Please run tf.open first." << LogIO::POST;
			return false;
		}

		verlist.resize(0);
		Int num;

		FlagVersion fv(fdh_p->getTableName(),"FLAG","FLAG_ROW");
		Vector<String> vlist = fv.getVersionList();

		num = verlist.nelements();
		verlist.resize( num + vlist.nelements() + 1, True );
		verlist[num] = String("\nMS : ") + fdh_p->getTableName() + String("\n");

		for(Int j=0; j<(Int)vlist.nelements(); j++)
			verlist[num+j+1] = vlist[j];
	}
	catch (AipsError x)
	{
		os << LogIO::SEVERE << "Could not get Flag Version List : " << x.getMesg() << LogIO::POST;
		return False;
	}

	return true;
}


// ---------------------------------------------------------------------
// TestFlagger::printFlagSelection
// Print the flag versions list
//
// ---------------------------------------------------------------------
bool
TestFlagger::printFlagSelections()
{

	LogIO os(LogOrigin("TestFlagger", __FUNCTION__, WHERE));

	// Use the copy of the agent records list because the original
	// was deallocated in the init() method.
	if (! agents_config_list_copy_p.empty())
	{
		os << "Current list of agents : "  << LogIO::POST;

		// TODO: loop through list
		// Duplicate the vector... ???
		for (size_t i=0; i < agents_config_list_copy_p.size(); i++) {
			ostringstream out;
			Record agent_rec;
			agent_rec = agents_config_list_copy_p.at(i);
			agent_rec.print(out);
			os << out.str() << LogIO::POST;
		}
		if (dbg)
			cout << "Size of original list " << agents_config_list_copy_p.size() << endl;

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

	if (! fdh_p){
		os << LogIO::SEVERE << "There is no MS attached. Please run tf.open first." << LogIO::POST;
		return false;
	}

	FlagVersion fv(fdh_p->getTableName(),"FLAG","FLAG_ROW");
	fv.saveFlagVersion(versionname, comment, merge);

	return true;
}

// ---------------------------------------------------------------------
// TestFlagger::isModeValid
// Check if mode is valid.
// Return False if not in the list
// ---------------------------------------------------------------------
bool
TestFlagger::isModeValid(String mode)
{
	bool ret;

	if (mode.compare("manual") == 0 or mode.compare("clip") == 0 or
			mode.compare("quack") == 0 or mode.compare("shadow") == 0 or
			mode.compare("elevation") == 0 or mode.compare("tfcrop") == 0 or
			mode.compare("extend") == 0 or mode.compare("rflag") == 0 or
			mode.compare("unflag") == 0 or mode.compare("summary") == 0
			or mode.compare("display") == 0) {

		ret = true;
	}
	else
		ret = false;

	return ret;
}

// ---------------------------------------------------------------------
// TestFlagger::parseManualParameters
// Parse data selection parameters and specific manual parameters
//
// ---------------------------------------------------------------------
bool
TestFlagger::parseManualParameters(String field, String spw, String array,
								   String feed, String scan, String antenna,
								   String uvrange,  String timerange, String correlation,
								   String intent, String observation, Bool apply)
{

	LogIO os(LogOrigin("TestFlagger", __FUNCTION__));

	// Default values for some parameters
	String mode = "manual";
	String agent_name = "Manual";

	// Create a record with the parameters
	Record agent_record = Record();

	agent_record.define("mode", mode);
	agent_record.define("spw", spw);
	agent_record.define("scan", scan);
	agent_record.define("field", field);
	agent_record.define("antenna", antenna);
	agent_record.define("timerange", timerange);
	agent_record.define("correlation", correlation);
	agent_record.define("intent", intent);
	agent_record.define("feed", feed);
	agent_record.define("array", array);
	agent_record.define("uvrange", uvrange);
	agent_record.define("observation", observation);
	agent_record.define("apply", apply);
	agent_record.define("name", agent_name);

	// Call the main method
	parseAgentParameters(agent_record);

	return true;
}


// ---------------------------------------------------------------------
// TestFlagger::parseClipParameters
// Parse data selection parameters and specific clip parameters
//
// ---------------------------------------------------------------------
bool
TestFlagger::parseClipParameters(String field, String spw, String array, String feed, String scan,
   	    String antenna, String uvrange, String timerange, String correlation,
   	    String intent, String observation, String datacolumn,
   	    Vector<Double> clipminmax, Bool clipoutside, Bool channelavg, Bool clipzeros, Bool apply)
{

	LogIO os(LogOrigin("TestFlagger", __FUNCTION__));

	// Default values for some parameters
	String mode = "clip";
	String agent_name = "Clip";

	// Create a record with the parameters
	Record agent_record = Record();

	agent_record.define("mode", mode);
	agent_record.define("spw", spw);
	agent_record.define("scan", scan);
	agent_record.define("field", field);
	agent_record.define("antenna", antenna);
	agent_record.define("timerange", timerange);
	agent_record.define("correlation", correlation);
	agent_record.define("intent", intent);
	agent_record.define("feed", feed);
	agent_record.define("array", array);
	agent_record.define("uvrange", uvrange);
	agent_record.define("observation", observation);
	agent_record.define("apply", apply);
	agent_record.define("name", agent_name);

	agent_record.define("datacolumn", datacolumn);
	agent_record.define("clipminmax", clipminmax);
	agent_record.define("clipoutside", clipoutside);
	agent_record.define("channelavg", channelavg);
	agent_record.define("clipzeros", clipzeros);


	// Call the main method
	parseAgentParameters(agent_record);

	return true;

}

// ---------------------------------------------------------------------
// TestFlagger::parseQuackParameters
// Parse data selection parameters and specific quack parameters
//
// ---------------------------------------------------------------------
bool
TestFlagger::parseQuackParameters(String field, String spw, String array, String feed, String scan,
   	    String antenna, String uvrange, String timerange, String correlation,
   	    String intent, String observation, String quackmode, Double quackinterval,
   	    Bool quackincrement, Bool apply)
{

	LogIO os(LogOrigin("TestFlagger", __FUNCTION__));

	// Default values for some parameters
	String mode = "quack";
	String agent_name = "Quack";

	// Create a record with the parameters
	Record agent_record = Record();

	agent_record.define("mode", mode);
	agent_record.define("spw", spw);
	agent_record.define("scan", scan);
	agent_record.define("field", field);
	agent_record.define("antenna", antenna);
	agent_record.define("timerange", timerange);
	agent_record.define("correlation", correlation);
	agent_record.define("intent", intent);
	agent_record.define("feed", feed);
	agent_record.define("array", array);
	agent_record.define("uvrange", uvrange);
	agent_record.define("observation", observation);
	agent_record.define("apply", apply);
	agent_record.define("name", agent_name);

	agent_record.define("quackmode", quackmode);
	agent_record.define("quackinterval", quackinterval);
	agent_record.define("quackincrement", quackincrement);

	// Call the main method
	parseAgentParameters(agent_record);

	return true;

}

// ---------------------------------------------------------------------
// TestFlagger::parseElevationParameters
// Parse data selection parameters and specific elevation parameters
//
// ---------------------------------------------------------------------
bool
TestFlagger::parseElevationParameters(String field, String spw, String array, String feed, String scan,
   	    String antenna, String uvrange, String timerange,String correlation,
   	    String intent, String observation, Double lowerlimit, Double upperlimit, Bool apply)
{

	LogIO os(LogOrigin("TestFlagger", __FUNCTION__));

	// Default values for some parameters
	String mode = "elevation";
	String agent_name = "Elevation";

	// Create a record with the parameters
	Record agent_record = Record();

	agent_record.define("mode", mode);
	agent_record.define("spw", spw);
	agent_record.define("scan", scan);
	agent_record.define("field", field);
	agent_record.define("antenna", antenna);
	agent_record.define("timerange", timerange);
	agent_record.define("correlation", correlation);
	agent_record.define("intent", intent);
	agent_record.define("feed", feed);
	agent_record.define("array", array);
	agent_record.define("uvrange", uvrange);
	agent_record.define("observation", observation);
	agent_record.define("apply", apply);
	agent_record.define("name", agent_name);

	agent_record.define("lowerlimit", lowerlimit);
	agent_record.define("upperlimit", upperlimit);

	// Call the main method
	parseAgentParameters(agent_record);

	return true;

}

// ---------------------------------------------------------------------
// TestFlagger::parseTfcropParameters
// Parse data selection parameters and specific tfcrop parameters
//
// ---------------------------------------------------------------------
bool
TestFlagger::parseTfcropParameters(String field, String spw, String array, String feed, String scan,
   	    String antenna, String uvrange, String timerange, String correlation,
   	    String intent, String observation, Double ntime, Bool combinescans,
   	    String datacolumn, Double timecutoff, Double freqcutoff, String timefit,
   	    String freqfit, Int maxnpieces, String flagdimension, String usewindowstats, Int halfwin,
   	    Bool apply)
{

	LogIO os(LogOrigin("TestFlagger", __FUNCTION__));

	// Default values for some parameters
	String mode = "tfcrop";
	String agent_name = "Tfcrop";

	// Create a record with the parameters
	Record agent_record = Record();

	agent_record.define("mode", mode);
	agent_record.define("spw", spw);
	agent_record.define("scan", scan);
	agent_record.define("field", field);
	agent_record.define("antenna", antenna);
	agent_record.define("timerange", timerange);
	agent_record.define("correlation", correlation);
	agent_record.define("intent", intent);
	agent_record.define("feed", feed);
	agent_record.define("array", array);
	agent_record.define("uvrange", uvrange);
	agent_record.define("observation", observation);
	agent_record.define("apply", apply);
	agent_record.define("name", agent_name);

	agent_record.define("ntime", ntime);
	agent_record.define("combinescans", combinescans);
	agent_record.define("datacolumn", datacolumn);
	agent_record.define("timecutoff", timecutoff);
	agent_record.define("freqcutoff", freqcutoff);
	agent_record.define("timefit", timefit);
	agent_record.define("freqfit", freqfit);
	agent_record.define("maxnpieces", maxnpieces);
	agent_record.define("flagdimension", flagdimension);
	agent_record.define("usewindowstats", usewindowstats);
	agent_record.define("halfwin", halfwin);
	agent_record.define("apply", apply);

	// Call the main method
	parseAgentParameters(agent_record);

	return true;

}

// ---------------------------------------------------------------------
// TestFlagger::parseExtendParameters
// Parse data selection parameters and specific extend parameters
//
// ---------------------------------------------------------------------
bool
TestFlagger::parseExtendParameters(String field, String spw, String array, String feed, String scan,
   	    String antenna, String uvrange, String timerange, String correlation, String intent,
   	    String observation, Double ntime, Bool combinescans, Bool extendpols, Double growtime,
   	    Double growfreq, Bool growaround, Bool flagneartime, Bool flagnearfreq, Bool apply)
{

	LogIO os(LogOrigin("TestFlagger", __FUNCTION__));

	// Default values for some parameters
	String mode = "extend";
	String agent_name = "Extend";

	// Create a record with the parameters
	Record agent_record = Record();

	agent_record.define("mode", mode);
	agent_record.define("spw", spw);
	agent_record.define("scan", scan);
	agent_record.define("field", field);
	agent_record.define("antenna", antenna);
	agent_record.define("timerange", timerange);
	agent_record.define("correlation", correlation);
	agent_record.define("intent", intent);
	agent_record.define("feed", feed);
	agent_record.define("array", array);
	agent_record.define("uvrange", uvrange);
	agent_record.define("observation", observation);
	agent_record.define("apply", apply);
	agent_record.define("name", agent_name);

	agent_record.define("ntime", ntime);
	agent_record.define("combinescans", combinescans);
	agent_record.define("extendpols", extendpols);
	agent_record.define("growtime", growtime);
	agent_record.define("growfreq", growfreq);
	agent_record.define("growaround", growaround);
	agent_record.define("flagneartime", flagneartime);
	agent_record.define("flagnearfreq", flagnearfreq);
	agent_record.define("apply", apply);

	// Call the main method
	parseAgentParameters(agent_record);

	return true;

}



// ---------------------------------------------------------------------
// TestFlagger::parseSummaryParameters
// Parse data selection parameters and specific summary parameters
//
// ---------------------------------------------------------------------
bool
TestFlagger::parseSummaryParameters(String field, String spw, String array,
								   String feed, String scan, String antenna,
								   String uvrange,  String timerange, String correlation,
								   String intent, String observation,
								   Bool spwchan, Bool spwcorr, Bool basecnt)
{

	LogIO os(LogOrigin("TestFlagger", __FUNCTION__));

	// Default values for some parameters
	String mode = "summary";
	String agent_name = "Summary";

	// Create a record with the parameters
	Record agent_record = Record();

	agent_record.define("mode", mode);
	agent_record.define("spw", spw);
	agent_record.define("scan", scan);
	agent_record.define("field", field);
	agent_record.define("antenna", antenna);
	agent_record.define("timerange", timerange);
	agent_record.define("correlation", correlation);
	agent_record.define("intent", intent);
	agent_record.define("feed", feed);
	agent_record.define("array", array);
	agent_record.define("uvrange", uvrange);
	agent_record.define("observation", observation);
	agent_record.define("name", agent_name);

	agent_record.define("spwchan", spwchan);
	agent_record.define("spwcorr", spwcorr);
	agent_record.define("basecnt", basecnt);

	// Call the main method
	parseAgentParameters(agent_record);

	return true;
}



} //#end casa namespace
