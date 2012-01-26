//# FlagDataHandler.h: This file contains the implementation of the FlagDataHandler class.
//#
//#  CASA - Common Astronomy Software Applications (http://casa.nrao.edu/)
//#  Copyright (C) Associated Universities, Inc. Washington DC, USA 2011, All rights reserved.
//#  Copyright (C) European Southern Observatory, 2011, All rights reserved.
//#
//#  This library is free software; you can redistribute it and/or
//#  modify it under the terms of the GNU Lesser General Public
//#  License as published by the Free software Foundation; either
//#  version 2.1 of the License, or (at your option) any later version.
//#
//#  This library is distributed in the hope that it will be useful,
//#  but WITHOUT ANY WARRANTY, without even the implied warranty of
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//#  Lesser General Public License for more details.
//#
//#  You should have received a copy of the GNU Lesser General Public
//#  License along with this library; if not, write to the Free Software
//#  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
//#  MA 02111-1307  USA
//# $Id: $

#include <flagging/Flagging/FlagDataHandler.h>

namespace casa { //# NAMESPACE CASA - BEGIN

/////////////////////////////////////
/// FlagDataHandler implementation //
/////////////////////////////////////

// -----------------------------------------------------------------------
// Default constructor
// -----------------------------------------------------------------------
FlagDataHandler::FlagDataHandler(string msname, uShort iterationApproach, Double timeInterval):
		msname_p(msname), iterationApproach_p(iterationApproach)
{
	// Initialize logger
	logger_p = new LogIO(LogOrigin("FlagDataHandler",__FUNCTION__,WHERE));

	// Set time interval
	setTimeInterval(timeInterval);

	// Deactivate profiling by default
	profiling_p = false;

	// Check if async i/o is enabled (double check for ROVisibilityIteratorAsync and FlagDataHandler config)
	asyncio_enabled_p = false;
	AipsrcValue<Bool>::find (asyncio_enabled_p,"VisibilityIterator.async.enabled", true);
	if (asyncio_enabled_p)
	{
		// Check Flag Data Handler config
		Bool tmp = false;
		AipsrcValue<Bool>::find (tmp,"FlagDataHandler.asyncio", false);
		if (!tmp)
		{
			asyncio_enabled_p = false;
		}
	}

	// Check if slurp is enabled
	if (!asyncio_enabled_p)
	{
		slurp_p = true;
		AipsrcValue<Bool>::find (slurp_p,"FlagDataHandler.slurp", true);
	}
	else
	{
		slurp_p = false;
	}

	*logger_p << LogIO::NORMAL 
		  << " Asyncio activated: " << asyncio_enabled_p
		  << " Slurp activated: "<< slurp_p <<  LogIO::POST;


	// WARNING: By default the visibility iterator adds the following
	// default columns: ARRAY_ID and FIELD_ID,DATA_DESC_ID and TIME.
	// And they are needed for the correct operation of the VisibilityIterator
	// (it needs to know when any of these changes to be able to give
	// the correct coordinates with the data). If these columns are not
	// explicitly sorted on, they will be added BEFORE any others, with
	// unexpected iteration behaviour
	// See: MSIter::construct

	// WARNING: By specifying Time as a sort column we are not producing
	// chunks with only one time step, but just the other way around!
	// (all time steps are grouped in the same chunk). This is because TIME
	// is one of the 'implicit' sort columns, and as explained before it is
	// added BEFORE the ones specified by the user, resulting in chunks with
	// only one time step, thus invalidating the time interval effect.
	// See: MSIter.h

	// By default we map polarizations because they are needed for the per-chunk logs
	mapPolarizations_p = true;

	// Antenna pointing map is only needed by the Elevation agent and I have move it there
	mapAntennaPointing_p = false;

	// These mapping is for the Quack agent, but requires a complete MS iteration, so I put it here,
	// together with the full swap that I do to pre-compute the amount of memory necessary to run
	mapScanStartStop_p = false;
	mapScanStartStopFlagged_p = false;

	// Initialize Pre-Load columns
	preLoadColumns_p.clear();
	preLoadColumns_p.push_back(VisBufferComponents::FieldId);
	preLoadColumns_p.push_back(VisBufferComponents::SpW);
	preLoadColumns_p.push_back(VisBufferComponents::Scan);
	preLoadColumns_p.push_back(VisBufferComponents::ArrayId);
	preLoadColumns_p.push_back(VisBufferComponents::ObservationId);

	preLoadColumns_p.push_back(VisBufferComponents::NRow);
	preLoadColumns_p.push_back(VisBufferComponents::NChannel);
	preLoadColumns_p.push_back(VisBufferComponents::NCorr);

	// Set the iteration approach based on the agent
	setIterationApproach(iterationApproach);

	// Initialize selection ranges
	anySelection_p = false;
	timeSelection_p = String("");
	baselineSelection_p = String("");
	fieldSelection_p = String("");
	// NOTE (after Dic 2011 testing): As far as I know spw selection does not have to be *
	// (can be empty) and in fact applying a spw selection slows down the MSSelection class
	spwSelection_p = String("");
	uvwSelection_p = String("");
	polarizationSelection_p = String("");
	scanSelection_p = String("");
	arraySelection_p = String("");
	observationSelection_p = String("");
	scanIntentSelection_p = String("");

	// Initialize iteration parameters
	chunksInitialized_p = false;
	buffersInitialized_p = false;
	iteratorGenerated_p = false;
	stopIteration_p = false;
	maxChunkRows = 0;
	chunkNo = 0;
	bufferNo = 0;


	// Initialize stats
	stats_p = false;
	cubeAccessCounter_p = 0;
	chunkCounts_p = 0;
	msCounts_p = 0;

	// Set all the initialized pointers to NULL
	selectedMeasurementSet_p = NULL;
	measurementSetSelection_p = NULL;
	originalMeasurementSet_p = NULL;

	rwVisibilityIterator_p = NULL;
	roVisibilityIterator_p = NULL;
	visibilityBuffer_p = NULL;

	antennaNames_p = NULL;
	antennaDiameters_p = NULL;
	antennaPairMap_p = NULL;
	subIntegrationMap_p = NULL;
	corrProducts_p = NULL;
	polarizationMap_p = NULL;
	polarizationIndexMap_p = NULL;
	antennaPointingMap_p = NULL;
	scanStartStopMap_p = NULL;
	fieldNames_p = NULL;

	// Initialize FlagDataHanler-FlagAgents state
	flushFlags_p = false;
	flushFlagRow_p = false;

	return;
}


// -----------------------------------------------------------------------
// Default destructor
// -----------------------------------------------------------------------
FlagDataHandler::~FlagDataHandler()
{
	// Delete logger
	if (logger_p) delete logger_p;

	// Delete MS objects
	if (selectedMeasurementSet_p) delete selectedMeasurementSet_p;
	if (measurementSetSelection_p) delete measurementSetSelection_p;
	if (originalMeasurementSet_p) delete originalMeasurementSet_p;

	// Delete VisBuffers and iterators
	if (visibilityBuffer_p) delete visibilityBuffer_p;
	// ROVisIter is in fact RWVisIter
	if (rwVisibilityIterator_p) delete rwVisibilityIterator_p;

	// Delete mapping members
	if (antennaNames_p) delete antennaNames_p;
	if (antennaDiameters_p) delete antennaDiameters_p;
	if (antennaPairMap_p) delete antennaPairMap_p;
	if (subIntegrationMap_p) delete subIntegrationMap_p;
	if (corrProducts_p) delete corrProducts_p;
	if (polarizationMap_p) delete polarizationMap_p;
	if (polarizationIndexMap_p) delete polarizationIndexMap_p;
	if (antennaPointingMap_p) delete antennaPointingMap_p;
	if (scanStartStopMap_p) delete scanStartStopMap_p;
	if (fieldNames_p) delete fieldNames_p;

	return;
}

// -----------------------------------------------------------------------
// Set iteration Approach
// -----------------------------------------------------------------------
void
FlagDataHandler::setIterationApproach(uShort iterationApproach)
{
	logger_p->origin(LogOrigin("FlagDataHandler",__FUNCTION__,WHERE));

	iterationApproach_p = iterationApproach;

	switch (iterationApproach_p)
	{
		case COMPLETE_SCAN_MAPPED:
		{
			*logger_p << LogIO::NORMAL << " Iteration mode: COMPLETE_SCAN_MAPPED" << LogIO::POST;
			sortOrder_p = Block<int>(6);
			sortOrder_p[0] = MS::OBSERVATION_ID;
			sortOrder_p[1] = MS::ARRAY_ID;
			sortOrder_p[2] = MS::SCAN_NUMBER;
			sortOrder_p[3] = MS::FIELD_ID;
			sortOrder_p[4] = MS::DATA_DESC_ID;
			sortOrder_p[5] = MS::TIME;

			// NOTE: groupTimeSteps_p=false selects only one time step per buffer
			groupTimeSteps_p = true;
			mapAntennaPairs_p = true;
			mapSubIntegrations_p = true;
			setMapAntennaPairs(true);
			setMapSubIntegrations(true);
			break;
		}
		case COMPLETE_SCAN_MAP_SUB_INTEGRATIONS_ONLY:
		{
			*logger_p << LogIO::NORMAL << " Iteration mode: COMPLETE_SCAN_MAP_SUB_INTEGRATIONS_ONLY" << LogIO::POST;
			sortOrder_p = Block<int>(6);
			sortOrder_p[0] = MS::OBSERVATION_ID;
			sortOrder_p[1] = MS::ARRAY_ID;
			sortOrder_p[2] = MS::SCAN_NUMBER;
			sortOrder_p[3] = MS::FIELD_ID;
			sortOrder_p[4] = MS::DATA_DESC_ID;
			sortOrder_p[5] = MS::TIME;

			// NOTE: groupTimeSteps_p=false selects only one time step per buffer
			groupTimeSteps_p = true;
			mapAntennaPairs_p = false;
			mapSubIntegrations_p = true;
			setMapSubIntegrations(true);
			break;
		}
		case COMPLETE_SCAN_MAP_ANTENNA_PAIRS_ONLY:
		{
			*logger_p << LogIO::NORMAL << " Iteration mode: COMPLETE_SCAN_MAP_ANTENNA_PAIRS_ONLY" << LogIO::POST;
			sortOrder_p = Block<int>(6);
			sortOrder_p[0] = MS::OBSERVATION_ID;
			sortOrder_p[1] = MS::ARRAY_ID;
			sortOrder_p[2] = MS::SCAN_NUMBER;
			sortOrder_p[3] = MS::FIELD_ID;
			sortOrder_p[4] = MS::DATA_DESC_ID;
			sortOrder_p[5] = MS::TIME;

			// NOTE: groupTimeSteps_p=false selects only one time step per buffer
			groupTimeSteps_p = true;
			mapAntennaPairs_p = true;
			mapSubIntegrations_p = false;
			setMapAntennaPairs(true);
			break;
		}
		case COMPLETE_SCAN_UNMAPPED:
		{
			*logger_p << LogIO::NORMAL << " Iteration mode: COMPLETE_SCAN_UNMAPPED" << LogIO::POST;
			sortOrder_p = Block<int>(6);
			sortOrder_p[0] = MS::OBSERVATION_ID;
			sortOrder_p[1] = MS::ARRAY_ID;
			sortOrder_p[2] = MS::SCAN_NUMBER;
			sortOrder_p[3] = MS::FIELD_ID;
			sortOrder_p[4] = MS::DATA_DESC_ID;
			sortOrder_p[5] = MS::TIME;

			// NOTE: groupTimeSteps_p=false selects only one time step per buffer
			groupTimeSteps_p = true;
			mapAntennaPairs_p = false;
			mapSubIntegrations_p = false;
			break;
		}
		case COMBINE_SCANS_MAPPED:
		{
			*logger_p << LogIO::NORMAL << " Iteration mode: COMBINE_SCANS_MAPPED" << LogIO::POST;
			sortOrder_p = Block<int>(5);
			sortOrder_p[0] = MS::OBSERVATION_ID;
			sortOrder_p[1] = MS::ARRAY_ID;
			sortOrder_p[2] = MS::FIELD_ID;
			sortOrder_p[3] = MS::DATA_DESC_ID;
			sortOrder_p[4] = MS::TIME;

			// NOTE: groupTimeSteps_p=false selects only one time step per buffer
			groupTimeSteps_p = true;
			mapAntennaPairs_p = true;
			mapSubIntegrations_p = true;
			setMapAntennaPairs(true);
			setMapSubIntegrations(true);
			break;
		}
		case COMBINE_SCANS_MAP_SUB_INTEGRATIONS_ONLY:
		{
			*logger_p << LogIO::NORMAL << " Iteration mode: COMBINE_SCANS_MAP_SUB_INTEGRATIONS_ONLY" << LogIO::POST;
			sortOrder_p = Block<int>(5);
			sortOrder_p[0] = MS::OBSERVATION_ID;
			sortOrder_p[1] = MS::ARRAY_ID;
			sortOrder_p[2] = MS::FIELD_ID;
			sortOrder_p[3] = MS::DATA_DESC_ID;
			sortOrder_p[4] = MS::TIME;

			// NOTE: groupTimeSteps_p=false selects only one time step per buffer
			groupTimeSteps_p = true;
			mapAntennaPairs_p = false;
			mapSubIntegrations_p = true;
			setMapSubIntegrations(true);
			break;
		}
		case COMBINE_SCANS_MAP_ANTENNA_PAIRS_ONLY:
		{
			*logger_p << LogIO::NORMAL << " Iteration mode: COMBINE_SCANS_MAP_ANTENNA_PAIRS_ONLY" << LogIO::POST;
			sortOrder_p = Block<int>(5);
			sortOrder_p[0] = MS::OBSERVATION_ID;
			sortOrder_p[1] = MS::ARRAY_ID;
			sortOrder_p[2] = MS::FIELD_ID;
			sortOrder_p[3] = MS::DATA_DESC_ID;
			sortOrder_p[4] = MS::TIME;

			// NOTE: groupTimeSteps_p=false selects only one time step per buffer
			groupTimeSteps_p = true;
			mapAntennaPairs_p = true;
			mapSubIntegrations_p = false;
			setMapAntennaPairs(true);
			break;
		}
		case COMBINE_SCANS_UNMAPPED:
		{
			*logger_p << LogIO::NORMAL << " Iteration mode: COMBINE_SCANS_UNMAPPED" << LogIO::POST;
			sortOrder_p = Block<int>(5);
			sortOrder_p[0] = MS::OBSERVATION_ID;
			sortOrder_p[1] = MS::ARRAY_ID;
			sortOrder_p[2] = MS::SCAN_NUMBER;
			sortOrder_p[3] = MS::DATA_DESC_ID;
			sortOrder_p[4] = MS::TIME;

			// NOTE: groupTimeSteps_p=false selects only one time step per buffer
			groupTimeSteps_p = true;
			mapAntennaPairs_p = false;
			mapSubIntegrations_p = false;
			break;
		}
		case ANTENNA_PAIR:
		{
			*logger_p << LogIO::NORMAL << " Iteration mode: ANTENNA_PAIR" << LogIO::POST;
			sortOrder_p = Block<int>(8);
			sortOrder_p[0] = MS::OBSERVATION_ID;
			sortOrder_p[1] = MS::ARRAY_ID;
			sortOrder_p[2] = MS::SCAN_NUMBER;
			sortOrder_p[3] = MS::FIELD_ID;
			sortOrder_p[4] = MS::DATA_DESC_ID;
			// NOTE: As requested by Urvashi, in this way we produce chunks
			// per antenna pair with all the time steps grouped instead
			// of chunks with entire integrations for a given time step.
			sortOrder_p[5] = MS::ANTENNA1;
			sortOrder_p[6] = MS::ANTENNA2;
			sortOrder_p[7] = MS::TIME;

			// NOTE: groupTimeSteps_p=true groups all time steps together in one buffer.
			groupTimeSteps_p = true;
			mapAntennaPairs_p = false;
			mapSubIntegrations_p = false;
			break;
		}
		case  SUB_INTEGRATION:
		{
			*logger_p << LogIO::NORMAL << " Iteration mode: SUB_INTEGRATION" << LogIO::POST;
			sortOrder_p = Block<int>(6);
			sortOrder_p[0] = MS::OBSERVATION_ID;
			sortOrder_p[1] = MS::ARRAY_ID;
			sortOrder_p[2] = MS::SCAN_NUMBER;
			sortOrder_p[3] = MS::FIELD_ID;
			sortOrder_p[4] = MS::DATA_DESC_ID;
			sortOrder_p[5] = MS::TIME;

			// NOTE: groupTimeSteps_p=false selects only one time step per buffer
			groupTimeSteps_p = false;
			mapAntennaPairs_p = false;
			mapSubIntegrations_p = false;
			break;
		}
		case  ARRAY_FIELD:
		{
			*logger_p << LogIO::NORMAL << " Iteration mode: ARRAY_FIELD" << LogIO::POST;
			sortOrder_p = Block<int>(4);
			sortOrder_p[0] = MS::ARRAY_ID;
			sortOrder_p[1] = MS::FIELD_ID;
			sortOrder_p[2] = MS::DATA_DESC_ID;
			sortOrder_p[3] = MS::TIME;

			// NOTE: groupTimeSteps_p=false selects only one time step per buffer
			groupTimeSteps_p = false;
			mapAntennaPairs_p = false;
			mapSubIntegrations_p = false;
			break;
		}
		default:
		{
			*logger_p << LogIO::NORMAL << " Iteration mode: COMPLETE_SCAN_UNMAPPED" << LogIO::POST;
			sortOrder_p = Block<int>(6);
			sortOrder_p[0] = MS::OBSERVATION_ID;
			sortOrder_p[1] = MS::ARRAY_ID;
			sortOrder_p[2] = MS::SCAN_NUMBER;
			sortOrder_p[3] = MS::FIELD_ID;
			sortOrder_p[4] = MS::DATA_DESC_ID;
			sortOrder_p[5] = MS::TIME;

			// NOTE: groupTimeSteps_p=false selects only one time step per buffer
			groupTimeSteps_p = true;
			mapAntennaPairs_p = false;
			mapSubIntegrations_p = false;
			break;
		}
	}

	return;
}


// -----------------------------------------------------------------------
// Open Measurement Set
// -----------------------------------------------------------------------
bool
FlagDataHandler::open()
{
	logger_p->origin(LogOrigin("FlagDataHandler",__FUNCTION__,WHERE));

	if (originalMeasurementSet_p) delete originalMeasurementSet_p;
	originalMeasurementSet_p = new MeasurementSet(msname_p,Table::Update);

	// Activate Memory Resident Sub-tables for everything but Pointing, Syscal and History
	originalMeasurementSet_p->setMemoryResidentSubtables (MrsEligibility::defaultEligible());

	// Read antenna names and diameters from Antenna table
	ROMSAntennaColumns *antennaSubTable = new ROMSAntennaColumns(originalMeasurementSet_p->antenna());
	antennaNames_p = new Vector<String>(antennaSubTable->name().getColumn());
	antennaDiameters_p = new Vector<Double>(antennaSubTable->dishDiameter().getColumn());

	// Read field names
	ROMSFieldColumns *fieldSubTable = new ROMSFieldColumns(originalMeasurementSet_p->field());
	fieldNames_p = new Vector<String>(fieldSubTable->name().getColumn());

	// Read polarizations
	ROMSPolarizationColumns *polarizationSubTable = new ROMSPolarizationColumns(originalMeasurementSet_p->polarization());
	ROArrayColumn<Int> corrTypeColum = polarizationSubTable->corrType();
	corrProducts_p = new std::vector<String>();
	for (uInt polRow_idx=0;polRow_idx<corrTypeColum.nrow();polRow_idx++)
	{
		Array<Int> polRow = corrTypeColum(polRow_idx);
		for (uInt corr_i=0;corr_i<polRow.size();corr_i++)
		{
			switch (polRow(IPosition(1,corr_i)))
			{
				case Stokes::I:
				{
					*logger_p << LogIO::DEBUG1 << " Correlation product I found, which should correspond to ALMA WVR data - skipping" << LogIO::POST;
					// corrProducts_p->push_back("I");
					break;
				}
				case Stokes::Q:
				{
					*logger_p << LogIO::DEBUG1 << " Correlation product Q found" << LogIO::POST;
					corrProducts_p->push_back("Q");
					break;
				}
				case Stokes::U:
				{
					*logger_p << LogIO::DEBUG1 << " Correlation product U found" << LogIO::POST;
					corrProducts_p->push_back("U");
					break;
				}
				case Stokes::V:
				{
					*logger_p << LogIO::DEBUG1 << " Correlation product V found" << LogIO::POST;
					corrProducts_p->push_back("V");
					break;
				}
				case Stokes::XX:
				{
					*logger_p << LogIO::DEBUG1 << " Correlation product XX found" << LogIO::POST;
					corrProducts_p->push_back("XX");
					break;
				}
				case Stokes::YY:
				{
					*logger_p << LogIO::DEBUG1 << " Correlation product YY found" << LogIO::POST;
					corrProducts_p->push_back("YY");
					break;
				}
				case Stokes::XY:
				{
					*logger_p << LogIO::DEBUG1 << " Correlation product XY found" << LogIO::POST;
					corrProducts_p->push_back("XY");
					break;
				}
				case Stokes::YX:
				{
					*logger_p << LogIO::DEBUG1 << " Correlation product YX found" << LogIO::POST;
					corrProducts_p->push_back("YX");
					break;
				}
				case Stokes::RR:
				{
					*logger_p << LogIO::DEBUG1 << " Correlation product RR found" << LogIO::POST;
					corrProducts_p->push_back("RR");
					break;
				}
				case Stokes::LL:
				{
					*logger_p << LogIO::DEBUG1 << " Correlation product LL found" << LogIO::POST;
					corrProducts_p->push_back("LL");
					break;
				}
				case Stokes::RL:
				{
					*logger_p << LogIO::DEBUG1 << " Correlation product RL found" << LogIO::POST;
					corrProducts_p->push_back("RL");
					break;
				}
				case Stokes::LR:
				{
					*logger_p << LogIO::DEBUG1 << " Correlation product LR found" << LogIO::POST;
					corrProducts_p->push_back("LR");
					break;
				}
				default:
				{
					*logger_p << LogIO::WARN << " Correlation product unknown found: " << polRow(IPosition(1,corr_i)) << LogIO::POST;
					break;
				}
			}
		}
	}

	return true;
}


// -----------------------------------------------------------------------
// Close Measurement Set
// -----------------------------------------------------------------------
bool
FlagDataHandler::close()
{
	logger_p->origin(LogOrigin("FlagDataHandler",__FUNCTION__,WHERE));

	if (selectedMeasurementSet_p)
	{
		// Flush and unlock MS
		selectedMeasurementSet_p->flush();
		selectedMeasurementSet_p->relinquishAutoLocks(True);
		selectedMeasurementSet_p->unlock();

		// Post stats
		if (stats_p)
		{
			*logger_p << LogIO::NORMAL << " Total Flag Cube accesses: " <<  cubeAccessCounter_p << LogIO::POST;
		}
	}

	return true;
}


// -----------------------------------------------------------------------
// Set Data Selection parameters
// -----------------------------------------------------------------------
bool
FlagDataHandler::setDataSelection(Record record)
{
	logger_p->origin(LogOrigin("FlagDataHandler",__FUNCTION__,WHERE));

	int exists;

	exists = record.fieldNumber ("array");
	if (exists >= 0)
	{
		anySelection_p = true;
		record.get (record.fieldNumber ("array"), arraySelection_p);
		*logger_p << LogIO::NORMAL << " array selection is " << arraySelection_p << LogIO::POST;
	}
	else
	{
		arraySelection_p = String("");
		*logger_p << LogIO::DEBUG1 << " no array selection" << LogIO::POST;
	}

	exists = record.fieldNumber ("field");
	if (exists >= 0)
	{
		anySelection_p = true;
		record.get (record.fieldNumber ("field"), fieldSelection_p);
		*logger_p << LogIO::NORMAL << " field selection is " << fieldSelection_p << LogIO::POST;
	}
	else
	{
		fieldSelection_p = String("");
		*logger_p << LogIO::DEBUG1 << " no field selection" << LogIO::POST;
	}

	exists = record.fieldNumber ("scan");
	if (exists >= 0)
	{
		anySelection_p = true;
		record.get (record.fieldNumber ("scan"), scanSelection_p);
		*logger_p << LogIO::NORMAL << " scan selection is " << scanSelection_p << LogIO::POST;
	}
	else
	{
		scanSelection_p = String("");
		*logger_p << LogIO::DEBUG1 << " no scan selection" << LogIO::POST;
	}

	exists = record.fieldNumber ("timerange");
	if (exists >= 0)
	{
		anySelection_p = true;
		record.get (record.fieldNumber ("timerange"), timeSelection_p);
		*logger_p << LogIO::NORMAL << " timerange selection is " << timeSelection_p << LogIO::POST;
	}
	else
	{
		timeSelection_p = String("");
		*logger_p << LogIO::DEBUG1 << " no timerange selection" << LogIO::POST;
	}

	exists = record.fieldNumber ("spw");
	if (exists >= 0)
	{
		anySelection_p = true;
		record.get (record.fieldNumber ("spw"), spwSelection_p);
		*logger_p << LogIO::NORMAL << " spw selection is " << spwSelection_p << LogIO::POST;
	}
	else
	{
		spwSelection_p = String("");
		*logger_p << LogIO::DEBUG1 << " no spw selection" << LogIO::POST;
	}

	exists = record.fieldNumber ("antenna");
	if (exists >= 0)
	{
		anySelection_p = true;
		record.get (record.fieldNumber ("antenna"), baselineSelection_p);
		*logger_p << LogIO::NORMAL << " antenna selection is " << baselineSelection_p << LogIO::POST;
	}
	else
	{
		baselineSelection_p = String("");
		*logger_p << LogIO::DEBUG1 << " no antenna selection" << LogIO::POST;
	}

	exists = record.fieldNumber ("uvrange");
	if (exists >= 0)
	{
		anySelection_p = true;
		record.get (record.fieldNumber ("uvrange"), uvwSelection_p);
		*logger_p << LogIO::NORMAL << " uvrange selection is " << uvwSelection_p << LogIO::POST;
	}
	else
	{
		uvwSelection_p = String("");
		*logger_p << LogIO::DEBUG1 << " no uvrange selection" << LogIO::POST;
	}

	exists = record.fieldNumber ("correlation");
	if (exists >= 0)
	{
		anySelection_p = true;
		record.get (record.fieldNumber ("correlation"), polarizationSelection_p);
		*logger_p << LogIO::NORMAL << " correlation selection is " << polarizationSelection_p << LogIO::POST;
	}
	else
	{
		polarizationSelection_p = String("");
		*logger_p << LogIO::DEBUG1 << " no correlation selection" << LogIO::POST;
	}

	exists = record.fieldNumber ("observation");
	if (exists >= 0)
	{
		anySelection_p = true;
		record.get (record.fieldNumber ("observation"), observationSelection_p);
		*logger_p << LogIO::NORMAL << " observation selection is " << observationSelection_p << LogIO::POST;
	}
	else
	{
		observationSelection_p = String("");
		*logger_p << LogIO::DEBUG1 << " no observation selection" << LogIO::POST;
	}

	exists = record.fieldNumber ("intent");
	if (exists >= 0)
	{
		anySelection_p = true;
		record.get (record.fieldNumber ("intent"), scanIntentSelection_p);
		*logger_p << LogIO::NORMAL << " scan intent selection is " << scanIntentSelection_p << LogIO::POST;
	}
	else
	{
		scanIntentSelection_p = String("");
		*logger_p << LogIO::DEBUG1 << " no scan intent selection" << LogIO::POST;
	}

	return true;
}

// -----------------------------------------------------------------------
// Set time interval
// -----------------------------------------------------------------------
void
FlagDataHandler::setTimeInterval(Double timeInterval)
{
	logger_p->origin(LogOrigin("FlagDataHandler",__FUNCTION__,WHERE));
	if (timeInterval >= 0)
	{
		timeInterval_p = timeInterval;
		*logger_p << LogIO::NORMAL << "Set time interval to " << timeInterval_p << "s"<<LogIO::POST;
	}
	else
	{
		*logger_p << LogIO::WARN << "Provided time interval is negative: " <<  timeInterval << LogIO::POST;
	}

	return;
}

// -----------------------------------------------------------------------
// Generate selected Measurement Set
// -----------------------------------------------------------------------
bool
FlagDataHandler::selectData()
{
	logger_p->origin(LogOrigin("FlagDataHandler",__FUNCTION__,WHERE));

	// Create Measurement Selection object
	const String dummyExpr = String("");
	if (measurementSetSelection_p) delete measurementSetSelection_p;
	measurementSetSelection_p = new MSSelection(
			*originalMeasurementSet_p,
			MSSelection::PARSE_NOW,
			(const String)timeSelection_p,
			(const String)baselineSelection_p,
			(const String)fieldSelection_p,
			(const String)spwSelection_p,
			(const String)uvwSelection_p,
			dummyExpr, // taqlExpr
			(const String)polarizationSelection_p,
			(const String)scanSelection_p,
			(const String)arraySelection_p,
			(const String)scanIntentSelection_p,
			(const String)observationSelection_p);

	// Apply Measurement Selection to a copy of the original Measurement Set
	MeasurementSet auxMeasurementSet = MeasurementSet(*originalMeasurementSet_p);
	measurementSetSelection_p->getSelectedMS(auxMeasurementSet, String(""));
	if (selectedMeasurementSet_p) delete selectedMeasurementSet_p;
	selectedMeasurementSet_p = new MeasurementSet(auxMeasurementSet);

	// Check if selected MS has rows...
	if (selectedMeasurementSet_p->nrow() == 0)
	{
		*logger_p << LogIO::WARN << "Selected Measurement Set doesn't have any rows " << LogIO::POST;
	}

	// More debugging information from MS-Selection
	if (!arraySelection_p.empty())
	{
		*logger_p << LogIO::NORMAL << " Selected array ids are " << measurementSetSelection_p->getSubArrayList() << LogIO::POST;
	}

	if (!observationSelection_p.empty())
	{
		*logger_p << LogIO::NORMAL << " Selected observation ids are " << measurementSetSelection_p->getObservationList() << LogIO::POST;
	}

	if (!fieldSelection_p.empty())
	{
		*logger_p << LogIO::NORMAL << " Selected field ids are " << measurementSetSelection_p->getFieldList() << LogIO::POST;
	}

	if (!scanSelection_p.empty())
	{
		*logger_p << LogIO::NORMAL << " Selected scan ids are " << measurementSetSelection_p->getScanList() << LogIO::POST;
	}

	if (!scanIntentSelection_p.empty())
	{
		*logger_p << LogIO::NORMAL << " Selected scan intent ids are " << measurementSetSelection_p->getStateObsModeList() << LogIO::POST;
	}

	if (!timeSelection_p.empty())
	{
		*logger_p << LogIO::NORMAL << " Selected time range is " << measurementSetSelection_p->getTimeList() << LogIO::POST;
	}

	if (!spwSelection_p.empty())
	{
		*logger_p << LogIO::NORMAL << " Selected spw-channels ids are " << measurementSetSelection_p->getChanList() << LogIO::POST;
	}

	if (!baselineSelection_p.empty())
	{
		*logger_p << LogIO::NORMAL << " Selected antenna1 ids are " << measurementSetSelection_p->getAntenna1List() << LogIO::POST;
		*logger_p << LogIO::NORMAL << " Selected antenna2 ids are " << measurementSetSelection_p->getAntenna2List() << LogIO::POST;
		*logger_p << LogIO::NORMAL << " Selected baselines are " << measurementSetSelection_p->getBaselineList() << LogIO::POST;
	}

	if (!uvwSelection_p.empty())
	{
		*logger_p << LogIO::NORMAL << " Selected uv range is " << measurementSetSelection_p->getUVList() << LogIO::POST;
	}

	if (!polarizationSelection_p.empty())
	{
		ostringstream polarizationListToPrint (ios::in | ios::out);
		polarizationListToPrint << measurementSetSelection_p->getPolMap();
		*logger_p << LogIO::NORMAL << " Selected correlation ids are " << polarizationListToPrint.str() << LogIO::POST;
	}

	return true;
}

// -----------------------------------------------------------------------
// Function to handled columns pre-load (to avoid problems with parallelism)
// -----------------------------------------------------------------------
void
FlagDataHandler::preLoadColumn(uInt column)
{
	logger_p->origin(LogOrigin("FlagDataHandler",__FUNCTION__,WHERE));

	if (std::find (preLoadColumns_p.begin(), preLoadColumns_p.end(), column) == preLoadColumns_p.end())
	{
		*logger_p << LogIO::DEBUG1 << " Adding column to list: " <<  column << LogIO::POST;
		preLoadColumns_p.push_back(column);
	}

	return;
}

void
FlagDataHandler::preFetchColumns()
{
	for (vector<uInt>::iterator iter=preLoadColumns_p.begin();iter!=preLoadColumns_p.end();iter++)
	{
		switch (*iter)
		{
			case VisBufferComponents::Ant1:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::Ant1);
				}
				else
				{
					visibilityBuffer_p->get()->antenna1();
				}
				break;
			}
			case VisBufferComponents::Ant2:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::Ant2);
				}
				else
				{
					visibilityBuffer_p->get()->antenna2();
				}
				break;
			}
			case VisBufferComponents::ArrayId:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::ArrayId);
				}
				else
				{
					visibilityBuffer_p->get()->arrayId();
				}
				break;
			}
			case VisBufferComponents::Channel:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::Channel);
				}
				else
				{
					visibilityBuffer_p->get()->channel();
				}
				break;
			}
			case VisBufferComponents::Cjones:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::Cjones);
				}
				else
				{
					visibilityBuffer_p->get()->CJones();
				}
				break;
			}
			case VisBufferComponents::CorrType:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::CorrType);
				}
				else
				{
					visibilityBuffer_p->get()->corrType();
				}
				break;
			}
			case VisBufferComponents::Corrected:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::Corrected);
				}
				else
				{
					visibilityBuffer_p->get()->correctedVisibility();
				}
				break;
			}
			case VisBufferComponents::CorrectedCube:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::CorrectedCube);
				}
				else
				{
					visibilityBuffer_p->get()->correctedVisCube();
				}
				break;
			}
			case VisBufferComponents::Direction1:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::Direction1);
				}
				else
				{
					visibilityBuffer_p->get()->direction1();
				}
				break;
			}
			case VisBufferComponents::Direction2:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::Direction2);
				}
				else
				{
					visibilityBuffer_p->get()->direction2();
				}
				break;
			}
			case VisBufferComponents::Exposure:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::Exposure);
				}
				else
				{
					visibilityBuffer_p->get()->exposure();
				}
				break;
			}
			case VisBufferComponents::Feed1:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::Feed1);
				}
				else
				{
					visibilityBuffer_p->get()->feed1();
				}
				break;
			}
			case VisBufferComponents::Feed1_pa:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::Feed1_pa);
				}
				else
				{
					visibilityBuffer_p->get()->feed1_pa();
				}
				break;
			}
			case VisBufferComponents::Feed2:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::Feed2);
				}
				else
				{
					visibilityBuffer_p->get()->feed2();
				}
				break;
			}
			case VisBufferComponents::Feed2_pa:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::Feed2_pa);
				}
				else
				{
					visibilityBuffer_p->get()->feed2_pa();
				}
				break;
			}
			case VisBufferComponents::FieldId:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::FieldId);
				}
				else
				{
					visibilityBuffer_p->get()->fieldId();
				}
				break;
			}
			case VisBufferComponents::Flag:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::Flag);
				}
				else
				{
					visibilityBuffer_p->get()->flag();
				}
				break;
			}
			case VisBufferComponents::FlagCategory:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::FlagCategory);
				}
				else
				{
					visibilityBuffer_p->get()->flagCategory();
				}
				break;
			}
			case VisBufferComponents::FlagCube:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::FlagCube);
				}
				else
				{
					visibilityBuffer_p->get()->flagCube();
				}
				break;
			}
			case VisBufferComponents::FlagRow:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::FlagRow);
				}
				else
				{
					visibilityBuffer_p->get()->flagRow();
				}
				break;
			}
			case VisBufferComponents::Freq:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::Freq);
				}
				else
				{
					visibilityBuffer_p->get()->frequency();
				}
				break;
			}
			case VisBufferComponents::ImagingWeight:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::ImagingWeight);
				}
				else
				{
					visibilityBuffer_p->get()->imagingWeight();
				}
				break;
			}
			case VisBufferComponents::Model:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::Model);
				}
				else
				{
					visibilityBuffer_p->get()->modelVisibility();
				}
				break;
			}
			case VisBufferComponents::ModelCube:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::ModelCube);
				}
				else
				{
					visibilityBuffer_p->get()->modelVisCube();
				}
				break;
			}
			case VisBufferComponents::NChannel:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::NChannel);
				}
				else
				{
					visibilityBuffer_p->get()->nChannel();
				}
				break;
			}
			case VisBufferComponents::NCorr:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::NCorr);
				}
				else
				{
					visibilityBuffer_p->get()->nCorr();
				}
				break;
			}
			case VisBufferComponents::NRow:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::NRow);
				}
				else
				{
					visibilityBuffer_p->get()->nRow();
				}
				break;
			}
			case VisBufferComponents::ObservationId:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::ObservationId);
				}
				else
				{
					visibilityBuffer_p->get()->observationId();
				}
				break;
			}
			case VisBufferComponents::Observed:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::Observed);
				}
				else
				{
					visibilityBuffer_p->get()->visibility();
				}
				break;
			}
			case VisBufferComponents::ObservedCube:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::ObservedCube);
				}
				else
				{
					visibilityBuffer_p->get()->visCube();
				}
				break;
			}
			case VisBufferComponents::PhaseCenter:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::PhaseCenter);
				}
				else
				{
					visibilityBuffer_p->get()->phaseCenter();
				}
				break;
			}
			case VisBufferComponents::PolFrame:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::PolFrame);
				}
				else
				{
					visibilityBuffer_p->get()->polFrame();
				}
				break;
			}
			case VisBufferComponents::ProcessorId:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::ProcessorId);
				}
				else
				{
					visibilityBuffer_p->get()->processorId();
				}
				break;
			}
			case VisBufferComponents::Scan:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::Scan);
				}
				else
				{
					visibilityBuffer_p->get()->scan();
				}
				break;
			}
			case VisBufferComponents::Sigma:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::Sigma);
				}
				else
				{
					visibilityBuffer_p->get()->sigma();
				}
				break;
			}
			case VisBufferComponents::SigmaMat:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::SigmaMat);
				}
				else
				{
					visibilityBuffer_p->get()->sigmaMat();
				}
				break;
			}
			case VisBufferComponents::SpW:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::SpW);
				}
				else
				{
					visibilityBuffer_p->get()->spectralWindow();
				}
				break;
			}
			case VisBufferComponents::StateId:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::StateId);
				}
				else
				{
					visibilityBuffer_p->get()->stateId();
				}
				break;
			}
			case VisBufferComponents::Time:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::Time);
				}
				else
				{
					visibilityBuffer_p->get()->time();
				}
				break;
			}
			case VisBufferComponents::TimeCentroid:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::TimeCentroid);
				}
				else
				{
					visibilityBuffer_p->get()->timeCentroid();
				}
				break;
			}
			case VisBufferComponents::TimeInterval:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::TimeInterval);
				}
				else
				{
					visibilityBuffer_p->get()->timeInterval();
				}
				break;
			}
			case VisBufferComponents::Weight:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::Weight);
				}
				else
				{
					visibilityBuffer_p->get()->weight();
				}
				break;
			}
			case VisBufferComponents::WeightMat:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::WeightMat);
				}
				else
				{
					visibilityBuffer_p->get()->weightMat();
				}
				break;
			}
			case VisBufferComponents::WeightSpectrum:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::WeightSpectrum);
				}
				else
				{
					visibilityBuffer_p->get()->weightSpectrum();
				}
				break;
			}
			case VisBufferComponents::Uvw:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::Uvw);
				}
				else
				{
					visibilityBuffer_p->get()->uvw();
				}
				break;
			}
			case VisBufferComponents::UvwMat:
			{
				if (asyncio_enabled_p)
				{
					prefetchColumns_p.insert(VisBufferComponents::UvwMat);
				}
				else
				{
					visibilityBuffer_p->get()->uvwMat();
				}
				break;
			}
		}
	}

	return;
}

// -----------------------------------------------------------------------
// Swap MS to check what is the maximum RAM memory needed
// -----------------------------------------------------------------------
void
FlagDataHandler::checkMaxMemory()
{
	logger_p->origin(LogOrigin("FlagDataHandler",__FUNCTION__,WHERE));

	double memoryNeeded = 0;
	double maxMemoryNeeded = 0;
	// visCube,flagCube
	double memoryPerVisFlagCubes = 65.0/(1024.0*1024.0);
	// ant1, ant2, corrType, feed1, feed2, fieldId, frequency, scan, spw, stateId, time, timeInterval, uvw
	double memoryPerRow = 32*15/(1024.0*1024.0);

	for (rwVisibilityIterator_p->originChunks(); rwVisibilityIterator_p->moreChunks();rwVisibilityIterator_p->nextChunk())
	{
		// Check if we have to group time steps
		if (groupTimeSteps_p)
		{
			rwVisibilityIterator_p->setRowBlocking(rwVisibilityIterator_p->nRowChunk());
			if (rwVisibilityIterator_p->nRowChunk() > maxChunkRows) maxChunkRows = rwVisibilityIterator_p->nRowChunk();
		}

		// Iterate over vis buffers
		for (rwVisibilityIterator_p->origin(); rwVisibilityIterator_p->more();(*rwVisibilityIterator_p)++)
		{
			// Check total amount of memory needed for visibilities
			memoryNeeded = memoryPerVisFlagCubes*(rwVisibilityIterator_p->visibilityShape().product());

			// Add up memory needed for the rest of the columns
			memoryNeeded += memoryPerRow*(rwVisibilityIterator_p->nRow());

			if (memoryNeeded > maxMemoryNeeded) maxMemoryNeeded = memoryNeeded;

			if (mapScanStartStop_p)
			{
				generateScanStartStopMap();
			}
		}
	}

	Int buffers = 1;
	double memoryFree = HostInfo::memoryFree( )/1024.0;
	double memoryUsed = 100*maxMemoryNeeded/memoryFree;
	if (asyncio_enabled_p)
	{
		AipsrcValue<Int>::find (buffers,"VisibilityIterator.async.nBuffers", 2);
		*logger_p << LogIO::NORMAL << " This process needs " << buffers << " (pre-fetched buffers in async mode) x " << maxMemoryNeeded << " MB for loading visibility buffers ("
				<< memoryUsed << "%) of available free memory (" << memoryFree << " MB)"<< LogIO::POST;
	}
	else
	{
		*logger_p << LogIO::NORMAL << " This process needs " << maxMemoryNeeded << " MB for loading visibility buffers ("
				<< memoryUsed << "%) of available free memory (" << memoryFree << " MB)"<< LogIO::POST;
	}

	if (buffers*maxMemoryNeeded > memoryFree*0.90)
	{
		if (asyncio_enabled_p)
		{
			*logger_p << LogIO::SEVERE << " This process would need to consume more than 90% ("
					<< buffers*maxMemoryNeeded << " MB) of the available memory (" << memoryFree
					<< " MB) for loading vis buffers, aborting. Consider reducing the time interval, or reducing the number of buffers pre-fetched by async I/O (" << buffers
					<< ") or even switch off async I/O." << LogIO::POST;
		}
		else
		{
			*logger_p << LogIO::SEVERE << " This process would need to consume more than 90% ("
					<< buffers*maxMemoryNeeded << " MB) of the available memory (" << memoryFree
					<< " MB) for loading vis buffers, aborting. Consider reducing the time interval."<< LogIO::POST;
		}

		throw(AipsError("FlagDataHandler::checkMaxMemory() Not enough memory to process"));
	}

	if (mapScanStartStop_p)
	{
		*logger_p << LogIO::NORMAL <<  " " << scanStartStopMap_p->size() <<" Scans found in MS" << LogIO::POST;
	}

	return;
}

// -----------------------------------------------------------------------
// Generate Visibility Iterator with a given sort order and time interval
// -----------------------------------------------------------------------
bool
FlagDataHandler::generateIterator()
{
	// First create and initialize RW iterator
	if (rwVisibilityIterator_p) delete rwVisibilityIterator_p;
	rwVisibilityIterator_p = new VisibilityIterator(*selectedMeasurementSet_p,sortOrder_p,true,timeInterval_p);

	// Set the table data manager (ISM and SSM) cache size to the full column size, for
	// the columns ANTENNA1, ANTENNA2, FEED1, FEED2, TIME, INTERVAL, FLAG_ROW, SCAN_NUMBER and UVW
	if (slurp_p) rwVisibilityIterator_p->slurp();

	// Apply channel selection (Notice that is not necessary to do this again with the RO iterator in sync mode)
	applyChannelSelection(rwVisibilityIterator_p);

	checkMaxMemory();

	// If async I/O is enabled we create an async RO iterator for reading and a conventional RW iterator for writing
	// Both iterators share a mutex which is resident in the VLAT data (Visibility Look Ahead thread Data Object)
	// With this configuration the Visibility Buffer is attached to the RO async iterator
	if (asyncio_enabled_p)
	{
		// Set preFetchColumns
		prefetchColumns_p = casa::asyncio::PrefetchColumns::prefetchColumns(VisBufferComponents::FlagCube,
																			VisBufferComponents::FlagRow,
																			VisBufferComponents::NRow,
																			VisBufferComponents::FieldId);
		preFetchColumns();


		// Then create and initialize RO Async iterator
		if (rwVisibilityIterator_p) delete rwVisibilityIterator_p;
		rwVisibilityIterator_p = new VisibilityIterator(&prefetchColumns_p,*selectedMeasurementSet_p,sortOrder_p,true,timeInterval_p);

		// Cast RW conventional iterator into RO conventional iterator
		if (roVisibilityIterator_p) delete roVisibilityIterator_p;
		roVisibilityIterator_p = (ROVisibilityIterator*)rwVisibilityIterator_p;

		// Set the table data manager (ISM and SSM) cache size to the full column size, for
		// the columns ANTENNA1, ANTENNA2, FEED1, FEED2, TIME, INTERVAL, FLAG_ROW, SCAN_NUMBER and UVW
		if (slurp_p) roVisibilityIterator_p->slurp();

		// Apply channel selection
		applyChannelSelection(roVisibilityIterator_p);

		// Set row blocking to a huge number
		*logger_p << LogIO::NORMAL <<  "Setting row blocking to maximum number of rows in all the chunks swapped: " << maxChunkRows << LogIO::POST;
		if (groupTimeSteps_p) roVisibilityIterator_p->setRowBlocking(maxChunkRows);

		// Attach Visibility Buffer to Visibility Iterator
		if (visibilityBuffer_p) delete visibilityBuffer_p;
		visibilityBuffer_p = new VisBufferAutoPtr(roVisibilityIterator_p);
	}
	else
	{
		// Cast RW conventional iterator into RO conventional iterator
		if (roVisibilityIterator_p) delete roVisibilityIterator_p;
		roVisibilityIterator_p = (ROVisibilityIterator*)rwVisibilityIterator_p;

		// Finally attach Visibility Buffer to RO conventional iterator
		if (visibilityBuffer_p) delete visibilityBuffer_p;
		visibilityBuffer_p = new VisBufferAutoPtr(roVisibilityIterator_p);
	}

	iteratorGenerated_p = true;

	return true;
}


// -----------------------------------------------------------------------
// Apply channel selection for asyn or normal iterator
// NOTE (first implementation): We always have to do this, even if there is no SPW:channel selection
// NOTE (after Dic 2011 testing): As far as I know spw selection does not have to be *, it can be empty,
// therefore this step will in practice do nothing , because the spw and channel lists are empty too.
// -----------------------------------------------------------------------
void
FlagDataHandler::applyChannelSelection(ROVisibilityIterator *roVisIter)
{
	// Apply channel selection (in row selection cannot be done with MSSelection)
	// NOTE: Each row of the Matrix has the following elements: SpwID StartCh StopCh Step
	Matrix<Int> spwchan = measurementSetSelection_p->getChanList();
	Vector<Int> spwlist = measurementSetSelection_p->getSpwList();
	Int spw,channelStart,channelStop,channelStep,channelWidth;
	for(uInt spw_i=0;spw_i<spwlist.nelements();spw_i++ )
	{
		// NOTE: selectChannel needs channelStart,channelWidth,channelStep
		spw = spwlist[spw_i];
		channelStart = spwchan(spw_i,1);
		channelStop = spwchan(spw_i,2);
		channelStep = spwchan(spw_i,3);
		channelWidth = channelStop-channelStart+1;
		roVisIter->selectChannel(1,channelStart,channelWidth,channelStep,spw);
	}

	return;
}


// -----------------------------------------------------------------------
// Move to next chunk
// -----------------------------------------------------------------------
bool
FlagDataHandler::nextChunk()
{
	logger_p->origin(LogOrigin("FlagDataHandler",__FUNCTION__,WHERE));

	msCounts_p += chunkCounts_p;
	chunkCounts_p = 0;
	bool moreChunks = false;
	if (stopIteration_p)
	{
		moreChunks = false;
	}
	else
	{
		if (!chunksInitialized_p)
		{
			if (!iteratorGenerated_p) generateIterator();
			roVisibilityIterator_p->originChunks();
			chunksInitialized_p = true;
			buffersInitialized_p = false;
			chunkNo++;
			bufferNo = 0;
			moreChunks = true;
		}
		else
		{
			roVisibilityIterator_p->nextChunk();

			if (roVisibilityIterator_p->moreChunks())
			{
				buffersInitialized_p = false;
				moreChunks = true;
				chunkNo++;
				bufferNo = 0;
			}
		}
	}

	if (!moreChunks)
	{
		*logger_p << LogIO::NORMAL << "==================================================================================== " << LogIO::POST;
	}

	return moreChunks;
}


// -----------------------------------------------------------------------
// Move to next buffer
// -----------------------------------------------------------------------
bool
FlagDataHandler::nextBuffer()
{
	bool moreBuffers = false;
	if (stopIteration_p)
	{
		moreBuffers = false;
	}
	else
	{
		if (!buffersInitialized_p)
		{
			// Group all the time stamps in one single buffer
			// NOTE: Otherwise we have to iterate over Visibility Buffers
			// that contain all the rows with the same time step.
			if ((groupTimeSteps_p) and (!asyncio_enabled_p))
			{
				Int nRowChunk = roVisibilityIterator_p->nRowChunk();
				roVisibilityIterator_p->setRowBlocking(nRowChunk);
			}
			roVisibilityIterator_p->origin();
			buffersInitialized_p = true;

			if (!asyncio_enabled_p) preFetchColumns();
			if (mapAntennaPairs_p) generateAntennaPairMap();
			if (mapSubIntegrations_p) generateSubIntegrationMap();
			if (mapPolarizations_p) generatePolarizationsMap();
			if (mapAntennaPointing_p) generateAntennaPointingMap();
			moreBuffers = true;
			flushFlags_p = false;
			flushFlagRow_p = false;
			bufferNo++;
		}
		else
		{
			// WARNING: ++ operator is defined for VisibilityIterator class ("advance" function)
			// but if you define a VisibilityIterator pointer, then  ++ operator does not call
			// the advance function but increments the pointers.
			(*roVisibilityIterator_p)++;

			// WARNING: We iterate and afterwards check if the iterator is valid
			if (roVisibilityIterator_p->more())
			{
				if (!asyncio_enabled_p) preFetchColumns();
				if (mapAntennaPairs_p) generateAntennaPairMap();
				if (mapSubIntegrations_p) generateSubIntegrationMap();
				if (mapPolarizations_p) generatePolarizationsMap();
				if (mapAntennaPointing_p) generateAntennaPointingMap();
				moreBuffers = true;
				flushFlags_p = false;
				flushFlagRow_p = false;
				bufferNo++;
			}
		}
	}

	// Set new common flag cube
	if (moreBuffers)
	{
		logger_p->origin(LogOrigin("FlagDataHandler",__FUNCTION__,WHERE));

		// Get flag  (WARNING: We have to modify the shape of the cube before re-assigning it)
		Cube<Bool> curentFlagCube= visibilityBuffer_p->get()->flagCube();
		modifiedFlagCube_p.resize(curentFlagCube.shape());
		modifiedFlagCube_p = curentFlagCube;
		originalFlagCube_p.resize(curentFlagCube.shape());
		originalFlagCube_p = curentFlagCube;

		// Get flag row (WARNING: We have to modify the shape of the cube before re-assigning it)
		Vector<Bool> curentflagRow= visibilityBuffer_p->get()->flagRow();
		modifiedFlagRow_p.resize(curentflagRow.shape());
		modifiedFlagRow_p = curentflagRow;
		originalFlagRow_p.resize(curentflagRow.shape());
		originalFlagRow_p = curentflagRow;

		// Compute total number of flags per buffer to be used for generating the agents stats
		chunkCounts_p += curentFlagCube.shape().product();

		// Print chunk characteristics
		if (bufferNo == 1)
		{
			String corrs = "[ ";
			for (uInt corr_i=0;corr_i<visibilityBuffer_p->get()->nCorr();corr_i++)
			{
				corrs += (*polarizationIndexMap_p)[corr_i] + " ";
			}
			corrs += "]";

			*logger_p << LogIO::NORMAL << 
			  "------------------------------------------------------------------------------------ " << LogIO::POST;
			*logger_p << "Chunk = " << chunkNo <<
					", Observation = " << visibilityBuffer_p->get()->observationId()[0] <<
					", Array = " << visibilityBuffer_p->get()->arrayId() <<
					", Scan = " << visibilityBuffer_p->get()->scan0() <<
					", Field = " << visibilityBuffer_p->get()->fieldId() << " (" << fieldNames_p->operator()(visibilityBuffer_p->get()->fieldId()) << ")"
					", Spw = " << visibilityBuffer_p->get()->spectralWindow() <<
					", Channels = " << visibilityBuffer_p->get()->nChannel() <<
					", Corrs = " << corrs <<
					", Total Rows = " << visibilityBuffer_p->get()->nRowChunk() << LogIO::POST;
		}
	}

	return moreBuffers;
}


// -----------------------------------------------------------------------
// Flush flags to MS
// -----------------------------------------------------------------------
bool
FlagDataHandler::flushFlags()
{
	if (flushFlags_p)
	{
		rwVisibilityIterator_p->setFlag(modifiedFlagCube_p);
		flushFlags_p = false;
	}

	if (flushFlagRow_p)
	{
		rwVisibilityIterator_p->setFlagRow(modifiedFlagRow_p);
		flushFlagRow_p = false;
	}

	return true;
}


// -----------------------------------------------------------------------
// As requested by Urvashi R.V. provide access to the original and modified flag cubes
// -----------------------------------------------------------------------
Cube<Bool> *
FlagDataHandler::getModifiedFlagCube()
{
	return &modifiedFlagCube_p;
}

Cube<Bool> *
FlagDataHandler::getOriginalFlagCube()
{
	return &originalFlagCube_p;
}

Vector<Bool> *
FlagDataHandler::getModifiedFlagRow()
{
	return &modifiedFlagRow_p;
}

Vector<Bool> *
FlagDataHandler::getOriginalFlagRow()
{
	return &originalFlagRow_p;
}

// -----------------------------------------------------------------------
// Mapping functions as requested by Urvashi
// -----------------------------------------------------------------------
void
FlagDataHandler::generateAntennaPairMap()
{
	logger_p->origin(LogOrigin("FlagDataHandler",__FUNCTION__,WHERE));

	// Free previous map and create a new one
	if (antennaPairMap_p) delete antennaPairMap_p;
	antennaPairMap_p = new antennaPairMap();

	// Retrieve antenna vectors
	Vector<Int> antenna1Vector = visibilityBuffer_p->get()->antenna1();
	Vector<Int> antenna2Vector = visibilityBuffer_p->get()->antenna2();

	// Fill map
	Int ant1_i,ant2_i;
	uInt nRows = antenna1Vector.size();
	for (uInt row_idx=0;row_idx<nRows;row_idx++)
	{
		ant1_i = antenna1Vector[row_idx];
		ant2_i = antenna2Vector[row_idx];
		if (antennaPairMap_p->find(std::make_pair(ant1_i,ant2_i)) == antennaPairMap_p->end())
		{
			std::vector<uInt> newPair;
			newPair.push_back(row_idx);
			(*antennaPairMap_p)[std::make_pair(ant1_i,ant2_i)] = newPair;
		}
		else
		{
			(*antennaPairMap_p)[std::make_pair(ant1_i,ant2_i)].push_back(row_idx);
		}
	}
	*logger_p << LogIO::DEBUG1 <<  " " << antennaPairMap_p->size() <<" Antenna pairs found in current buffer" << LogIO::POST;

	return;
}


void
FlagDataHandler::generateSubIntegrationMap()
{
	logger_p->origin(LogOrigin("FlagDataHandler",__FUNCTION__,WHERE));

	// Free previous map and create a new one
	if (subIntegrationMap_p) delete subIntegrationMap_p;
	subIntegrationMap_p = new subIntegrationMap();

	// Retrieve antenna vectors
	Vector<Double> timeVector = visibilityBuffer_p->get()->time();

	// Fill map
	uInt nRows = timeVector.size();
	for (uInt row_idx=0;row_idx<nRows;row_idx++)
	{
		if (subIntegrationMap_p->find(timeVector[row_idx]) == subIntegrationMap_p->end())
		{
			std::vector<uInt> newSubIntegration;
			newSubIntegration.push_back(row_idx);
			(*subIntegrationMap_p)[timeVector[row_idx]] = newSubIntegration;
		}
		else
		{
			(*subIntegrationMap_p)[timeVector[row_idx]].push_back(row_idx);
		}
	}
	*logger_p << LogIO::NORMAL <<  " " << subIntegrationMap_p->size() <<" Sub-Integrations (time steps) found in current buffer" << LogIO::POST;

	return;
}


void
FlagDataHandler::generatePolarizationsMap()
{
	logger_p->origin(LogOrigin("FlagDataHandler",__FUNCTION__,WHERE));

	// Free previous map and create a new one
	if (polarizationMap_p) delete polarizationMap_p;
	polarizationMap_p = new polarizationMap();
	if (polarizationIndexMap_p) delete polarizationIndexMap_p;
	polarizationIndexMap_p = new polarizationIndexMap();

	uShort pos = 0;
	Vector<Int> corrTypes = visibilityBuffer_p->get()->corrType();
	*logger_p << LogIO::DEBUG1 << " Correlation type: " <<  corrTypes << LogIO::POST;

	for (Vector<Int>::iterator iter = corrTypes.begin(); iter != corrTypes.end();iter++)
	{
		switch (*iter)
		{
			case Stokes::I:
			{
				*logger_p << LogIO::DEBUG1 << " The " << pos << " th correlation is I" << LogIO::POST;
				(*polarizationMap_p)[Stokes::I] = pos;
				(*polarizationIndexMap_p)[pos] = "I";
				break;
			}
			case Stokes::Q:
			{
				*logger_p << LogIO::DEBUG1 << " The " << pos << " th correlation is Q" << LogIO::POST;
				(*polarizationMap_p)[Stokes::Q] = pos;
				(*polarizationIndexMap_p)[pos] = "Q";
				break;
			}
			case Stokes::U:
			{
				*logger_p << LogIO::DEBUG1 << " The " << pos << " th correlation is U" << LogIO::POST;
				(*polarizationMap_p)[Stokes::U] = pos;
				(*polarizationIndexMap_p)[pos] = "U";
				break;
			}
			case Stokes::V:
			{
				*logger_p << LogIO::DEBUG1 << " The " << pos << " th correlation is V" << LogIO::POST;
				(*polarizationMap_p)[Stokes::V] = pos;
				(*polarizationIndexMap_p)[pos] = "V";
				break;
			}
			case Stokes::XX:
			{
				*logger_p << LogIO::DEBUG1 << " The " << pos << " th correlation is XX" << LogIO::POST;
				(*polarizationMap_p)[Stokes::XX] = pos;
				(*polarizationIndexMap_p)[pos] = "XX";
				break;
			}
			case Stokes::YY:
			{
				*logger_p << LogIO::DEBUG1 << " The " << pos << " th correlation is YY" << LogIO::POST;
				(*polarizationMap_p)[Stokes::YY] = pos;
				(*polarizationIndexMap_p)[pos] = "YY";
				break;
			}
			case Stokes::XY:
			{
				*logger_p << LogIO::DEBUG1 << " The " << pos << " th correlation is XY" << LogIO::POST;
				(*polarizationMap_p)[Stokes::XY] = pos;
				(*polarizationIndexMap_p)[pos] = "XY";
				break;
			}
			case Stokes::YX:
			{
				*logger_p << LogIO::DEBUG1 << " The " << pos << " th correlation is YX" << LogIO::POST;
				(*polarizationMap_p)[Stokes::YX] = pos;
				(*polarizationIndexMap_p)[pos] = "YX";
				break;
			}
			case Stokes::RR:
			{
				*logger_p << LogIO::DEBUG1 << " The " << pos << " th correlation is RR" << LogIO::POST;
				(*polarizationMap_p)[Stokes::RR] = pos;
				(*polarizationIndexMap_p)[pos] = "RR";
				break;
			}
			case Stokes::LL:
			{
				*logger_p << LogIO::DEBUG1 << " The " << pos << " th correlation is LL" << LogIO::POST;
				(*polarizationMap_p)[Stokes::LL] = pos;
				(*polarizationIndexMap_p)[pos] = "LL";
				break;
			}
			case Stokes::RL:
			{
				*logger_p << LogIO::DEBUG1 << " The " << pos << " th correlation is RL" << LogIO::POST;
				(*polarizationMap_p)[Stokes::RL] = pos;
				(*polarizationIndexMap_p)[pos] = "RL";
				break;
			}
			case Stokes::LR:
			{
				*logger_p << LogIO::DEBUG1 << " The " << pos << " th correlation is LR" << LogIO::POST;
				(*polarizationMap_p)[Stokes::LR] = pos;
				(*polarizationIndexMap_p)[pos] = "LR";
				break;
			}
			default:
			{
				*logger_p << LogIO::WARN << " The " << pos << " th correlation is unknown: " << *iter << LogIO::POST;
				break;
			}
		}
		pos++;
	}

	for (polarizationMap::iterator iter =polarizationMap_p->begin();iter != polarizationMap_p->end();iter++)
	{
		*logger_p << LogIO::DEBUG1 << " Polarization map key: " << iter->first << " value: " << iter->second << LogIO::POST;
	}

	return;
}

void
FlagDataHandler::generateAntennaPointingMap()
{
	logger_p->origin(LogOrigin("FlagDataHandler",__FUNCTION__,WHERE));

	// Free previous map and create a new one
	if (antennaPointingMap_p) delete antennaPointingMap_p;
	antennaPointingMap_p = new antennaPointingMap();

	Vector<Double> time = visibilityBuffer_p->get()->time();
	uInt nRows = time.size();
	antennaPointingMap_p->reserve(nRows);
	for (uInt row_i=0;row_i<nRows;row_i++)
	{
		Vector<MDirection> azimuth_elevation = visibilityBuffer_p->get()->azel(time[row_i]);
		Int ant1 = visibilityBuffer_p->get()->antenna1()[row_i];
		Int ant2 = visibilityBuffer_p->get()->antenna1()[row_i];

	    double antenna1_elevation = azimuth_elevation[ant1].getAngle("deg").getValue()[1];
	    double antenna2_elevation = azimuth_elevation[ant2].getAngle("deg").getValue()[1];

	    vector<Double> item(2);
	    item[0] = antenna1_elevation;
	    item[1] = antenna2_elevation;
	    antennaPointingMap_p->push_back(item);
	}

	*logger_p << LogIO::NORMAL << " Generated antenna pointing map with "
			<< antennaPointingMap_p->size() << " elements" << LogIO::POST;

	return;
}

void
FlagDataHandler::generateScanStartStopMap()
{

	Int scan;
	Double start,stop;
	Vector<Int> scans;
	Vector<Double> times;

	Cube<Bool> flags;
	uInt scanStartRow;
	uInt scanStopRow;
	uInt ncorrs,nchannels,nrows;
	Bool stopSearch;

	if (scanStartStopMap_p == NULL) scanStartStopMap_p = new scanStartStopMap();

	scans = rwVisibilityIterator_p->scan(scans);
	times = rwVisibilityIterator_p->time(times);

	// Check if anything is flagged in this buffer
	scanStartRow = 0;
	scanStopRow = times.size()-1;
	if (mapScanStartStopFlagged_p)
	{
		flags = rwVisibilityIterator_p->flag(flags);
		IPosition shape = flags.shape();
		ncorrs = shape[0];
		nchannels = shape[1];
		nrows = shape[2];

		// Look for effective scan start
		stopSearch = False;
		for (uInt row_i=0;row_i<nrows;row_i++)
		{
			if (stopSearch) break;

			for (uInt channel_i=0;channel_i<nchannels;channel_i++)
			{
				if (stopSearch) break;

				for (uInt corr_i=0;corr_i<ncorrs;corr_i++)
				{
					if (stopSearch) break;

					if (!flags(corr_i,channel_i,row_i))
					{
						scanStartRow = row_i;
						stopSearch = True;
					}
				}
			}
		}

		// If none of the rows were un-flagged we don't continue checking from the end
		// As a consequence of this some scans may not be present in the map, and have
		// to be skipped in the flagging process because they are already flagged.
		if (!stopSearch) return;

		// Look for effective scan stop
		stopSearch = False;
		for (uInt row_i=0;row_i<nrows;row_i++)
		{
			if (stopSearch) break;

			for (uInt channel_i=0;channel_i<nchannels;channel_i++)
			{
				if (stopSearch) break;

				for (uInt corr_i=0;corr_i<ncorrs;corr_i++)
				{
					if (stopSearch) break;

					if (!flags(corr_i,channel_i,nrows-1-row_i))
					{
						scanStopRow = nrows-1-row_i;
						stopSearch = True;
					}
				}
			}
		}
	}

	// Check scan start/stop times
	scan = scans[0];
	start = times[scanStartRow];
	stop = times[scanStopRow];

	if (scanStartStopMap_p->find(scan) == scanStartStopMap_p->end())
	{
		(*scanStartStopMap_p)[scan].push_back(start);
		(*scanStartStopMap_p)[scan].push_back(stop);
	}
	else
	{
		// Check if we have a better start time
		if ((*scanStartStopMap_p)[scan][0] > start)
		{
			(*scanStartStopMap_p)[scan][0] = start;
		}
		// Check if we have a better stop time
		if ((*scanStartStopMap_p)[scan][1] < stop)
		{
			(*scanStartStopMap_p)[scan][1] = stop;
		}
	}

	return;
}

// -----------------------------------------------------------------------
// Functions to switch on/off mapping functions
// -----------------------------------------------------------------------
void
FlagDataHandler::setMapAntennaPairs(bool activated)
{
	mapAntennaPairs_p=activated;
	// Pre-Load antenna1, antenna2
	preLoadColumn(VisBufferComponents::Ant1);
	preLoadColumn(VisBufferComponents::Ant2);
}

void
FlagDataHandler::setMapSubIntegrations(bool activated)
{
	mapSubIntegrations_p=activated;
	// Pre-Load time
	preLoadColumn(VisBufferComponents::Time);
}

void
FlagDataHandler::setMapPolarizations(bool activated)
{
	mapPolarizations_p=activated;
	// Pre-Load corrType
	preLoadColumn(VisBufferComponents::CorrType);
}

void
FlagDataHandler::setMapAntennaPointing(bool activated)
{
	// Antenna Pointing mapping is now done by the first
	// Elevation agent, and stored as static variable
	// mapAntennaPointing_p=activated;

	// Pre-Load time, antenna1 and antenna2
	// Azel is derived and this only restriction
	// is that it can be access by 1 thread only
	preLoadColumn(VisBufferComponents::Time);
	preLoadColumn(VisBufferComponents::Ant1);
	preLoadColumn(VisBufferComponents::Ant2);

}

void
FlagDataHandler::setScanStartStopMap(bool activated)
{
	mapScanStartStop_p=activated;
	// Pre-Load scan and time
	preLoadColumn(VisBufferComponents::Scan);
	preLoadColumn(VisBufferComponents::Time);
}

void
FlagDataHandler::setScanStartStopFlaggedMap(bool activated)
{
	mapScanStartStopFlagged_p=activated;
	// Pre-Load scan and time
	preLoadColumn(VisBufferComponents::Scan);
	preLoadColumn(VisBufferComponents::Time);
}

CubeView<Bool> *
FlagDataHandler::getFlagsView(Int antenna1, Int antenna2)
{
	std::vector<uInt> *rows = &((*antennaPairMap_p)[std::make_pair(antenna1,antenna2)]);
	CubeView<Bool> * cube= new CubeView<Bool>(&modifiedFlagCube_p,rows);
	return cube;
}

CubeView<Bool> *
FlagDataHandler::getFlagsView(Double timestep)
{
	std::vector<uInt> *rows = &((*subIntegrationMap_p)[timestep]);
	CubeView<Bool> * cube= new CubeView<Bool>(&modifiedFlagCube_p,rows);
	return cube;
}

CubeView<Complex> *
FlagDataHandler::getVisibilitiesView(Int antenna1, Int antenna2)
{
	std::vector<uInt> *rows = &((*antennaPairMap_p)[std::make_pair(antenna1,antenna2)]);
	CubeView<Complex> * cube= new CubeView<Complex>(&(visibilityBuffer_p->get()->visCube()),rows);
	return cube;
}

CubeView<Complex> *
FlagDataHandler::getVisibilitiesView(Double timestep)
{
	std::vector<uInt> *rows = &((*subIntegrationMap_p)[timestep]);
	CubeView<Complex> * cube= new CubeView<Complex>(&(visibilityBuffer_p->get()->visCube()),rows);
	return cube;
}

// -----------------------------------------------------------------------
// Dummy function to simulate processing
// -----------------------------------------------------------------------
uShort
FlagDataHandler::processBuffer(bool write, uShort rotateMode, uShort rotateViews)
{
	logger_p->origin(LogOrigin("FlagDataHandler",__FUNCTION__,WHERE));

	stats_p = true;

	antennaPairMapIterator 	myAntennaPairMapIterator;
	Double timestep;
	subIntegrationMapIterator mySubIntegrationMapIterator;
	std::pair<Int,Int> antennaPair;
	uShort processView = rotateViews;
	IPosition flagCubeShape;
	CubeView<Bool> *flagCubeView;
	switch (rotateMode)
	{
		case 0:
			flagCubeView = new CubeView<Bool>(&modifiedFlagCube_p);
			fillBuffer(*flagCubeView,write,0);
			delete flagCubeView;
		break;

		case 1:
			for (myAntennaPairMapIterator=antennaPairMap_p->begin(); myAntennaPairMapIterator != antennaPairMap_p->end(); ++myAntennaPairMapIterator) {
				antennaPair = myAntennaPairMapIterator->first;
				flagCubeView = getFlagsView(antennaPair.first,antennaPair.second);
				flagCubeShape = flagCubeView->shape();
				*logger_p 	<< LogIO::NORMAL << "FlagDataHandler::" << __FUNCTION__
							<< " Flag cube for (" <<  antennaPair.first << "," << antennaPair.second << ") has shape ["
						    << flagCubeShape(0) << "," <<  flagCubeShape(1) << "," << flagCubeShape(2) << "]" << LogIO::POST;

				if (rotateViews == 0)
				{
					fillBuffer(*flagCubeView,write,0);
				}
				else
				{
					if (processView == 1)
					{
						fillBuffer(*flagCubeView,write,1);
						processView = 2;
					}
					else
					{
						fillBuffer(*flagCubeView,write,2);
						processView = 1;
					}
				}

				delete flagCubeView;
			}
		break;

		case 2:
			for (mySubIntegrationMapIterator=subIntegrationMap_p->begin(); mySubIntegrationMapIterator != subIntegrationMap_p->end(); ++mySubIntegrationMapIterator) {
				timestep = mySubIntegrationMapIterator->first;
				flagCubeView = getFlagsView(timestep);
				flagCubeShape = flagCubeView->shape();
				*logger_p 	<< LogIO::NORMAL << "FlagDataHandler::" << __FUNCTION__
							<< " Flag cube for (" <<  timestep << ") has shape ["
						    << flagCubeShape(0) << "," <<  flagCubeShape(1) << "," << flagCubeShape(2) << "]" << LogIO::POST;

				if (rotateViews == 0)
				{
					fillBuffer(*flagCubeView,write,0);
				}
				else
				{
					if (processView == 1)
					{
						fillBuffer(*flagCubeView,write,1);
						processView = 2;
					}
					else
					{
						fillBuffer(*flagCubeView,write,2);
						processView = 1;
					}
				}

				delete flagCubeView;
			}
		break;

		default:
		break;
	}

	return processView;
}

void
FlagDataHandler::fillBuffer(CubeView<Bool> &flagCube,bool write, uShort processBuffer)
{
	logger_p->origin(LogOrigin("FlagDataHandler",__FUNCTION__,WHERE));

	bool processCondition = false;
	if (processBuffer == 0)
	{
		if (groupTimeSteps_p)
		{
			if (chunkNo % 2 == 0)
			{
				processCondition = true;
			}
			else
			{
				processCondition = false;
			}
		}
		else
		{
			if (bufferNo % 2 == 0)
			{
				processCondition = true;
			}
			else
			{
				processCondition = false;
			}
		}
	}
	else
	{
		if (processBuffer == 1)
		{
			processCondition = true;
		}
		else
		{
			processCondition = false;
		}
	}

	IPosition flagCubeShape = flagCube.shape();
	uInt nPolarizations = flagCubeShape(0);
	uInt nChannels = flagCubeShape(1);
	uInt nRows = flagCubeShape(2);
	bool flag;
	for (uInt row_k=0;row_k<nRows;row_k++) {
		for (uInt chan_j=0;chan_j<nChannels;chan_j++) {
			for (uInt pol_i=0;pol_i<nPolarizations;pol_i++) {
				// Flag each other chunks (i.e. even chunks)
				if (write)
				{
					if (processCondition) {
						flagCube(pol_i,chan_j,row_k) = True;
						cubeAccessCounter_p++;
					} else {
						flagCube(pol_i,chan_j,row_k) = False;
						cubeAccessCounter_p++;
					}
				}
				else
				{
					flag = flagCube(pol_i,chan_j,row_k);
					cubeAccessCounter_p++;

					if (processCondition) {
						if (flag != True)
						{
							*logger_p 	<< LogIO::SEVERE << "FlagDataHandler::" << __FUNCTION__
										<<" Wrong flag (False instead of True) in chunk " << chunkNo
										<< " buffer " << bufferNo << " polarization " << pol_i
										<< " channel " << chan_j << " row " << row_k << LogIO::POST;
						}
					} else {
						if (flag != False)
						{
							*logger_p 	<< LogIO::SEVERE << "FlagDataHandler::" << __FUNCTION__
										<<" Wrong flag (True instead of False) in chunk " << chunkNo
										<< " buffer " << bufferNo << " polarization " << pol_i
										<< " channel " << chan_j << " row " << row_k << LogIO::POST;
						}
					}
				}
			}
		}
	}

	return;
}

/////////////////////////////////////
/// VisMapper implementation ////////
/////////////////////////////////////
VisMapper::VisMapper(String expression,polarizationMap *polMap,CubeView<Complex> *leftVis,CubeView<Complex> *rightVis)
{
	rightVis_p = NULL;
	leftVis_p = NULL;
	setParentCubes(leftVis,rightVis);
	setExpressionMapping(expression,polMap);
}

VisMapper::VisMapper(String expression,polarizationMap *polMap)
{
	rightVis_p = NULL;
	leftVis_p = NULL;
	setExpressionMapping(expression,polMap);
}

void
VisMapper::setParentCubes(CubeView<Complex> *leftVis,CubeView<Complex> *rightVis)
{
	if (rightVis_p != NULL) delete rightVis_p;
	if (leftVis_p != NULL) delete leftVis_p;

	leftVis_p = leftVis;
	IPosition leftVisSize = leftVis->shape(); // pol,chan,row
	reducedLength_p = IPosition(2);
	reducedLength_p(0) = leftVisSize(1); // chan
	reducedLength_p(1) = leftVisSize(2); // row
	reducedLength_p(2) = leftVisSize(0); // pol


	if (rightVis != NULL)
	{
		rightVis_p = rightVis;
		getVis_p = &VisMapper::diffVis;
	}
	else
	{
		rightVis_p = NULL;
		getVis_p = &VisMapper::leftVis;
	}
}

void
VisMapper::setExpressionMapping(String expression,polarizationMap *polMap)
{
	selectedCorrelations_p.clear();
	expression_p = expression;
	polMap_p = polMap;
	bool matchExpression = false;

	// Parse complex unitary function
	if (expression_p.find("REAL") != string::npos)
	{
		applyVisExpr_p = &VisMapper::real;
	}
	else if (expression_p.find("IMAG") != string::npos)
	{
		applyVisExpr_p = &VisMapper::imag;
	}
	else if (expression_p.find("ARG") != string::npos)
	{
		applyVisExpr_p = &VisMapper::arg;
	}
	else if (expression_p.find("ABS") != string::npos)
	{
		applyVisExpr_p = &VisMapper::abs;
	}
	else if (expression_p.find("NORM") != string::npos)
	{
		applyVisExpr_p = &VisMapper::norm;
	}
	else
	{
		applyVisExpr_p = &VisMapper::abs;
	}

	// Parse polarization
	if (expression_p.find("WVR") != string::npos)
	{
		if (polMap_p->find(Stokes::I) != polMap_p->end())
		{
			selectedCorrelations_p.push_back((*polMap_p)[Stokes::I]);
			getCorr_p = &VisMapper::stokes_i;
			matchExpression = true;
		}
		else
		{
			throw AipsError("Requested polarization parameter (WVR - I) not available");
		}
	}
	if (expression_p.find("XX") != string::npos)
	{
		if (polMap_p->find(Stokes::XX) != polMap_p->end())
		{
			selectedCorrelations_p.push_back((*polMap_p)[Stokes::XX]);
			getCorr_p = &VisMapper::linear_xx;
			matchExpression = true;
		}
		else
		{
			throw AipsError("Requested polarization parameter (XX) not available");
		}
	}
	if (expression_p.find("YY") != string::npos)
	{
		if (polMap_p->find(Stokes::YY) != polMap_p->end())
		{
			selectedCorrelations_p.push_back((*polMap_p)[Stokes::YY]);
			getCorr_p = &VisMapper::linear_yy;
			matchExpression = true;
		}
		else
		{
			throw AipsError("Requested polarization parameter (YY) not available");
		}
	}
	if (expression_p.find("XY") != string::npos)
	{
		if (polMap_p->find(Stokes::XY) != polMap_p->end())
		{
			selectedCorrelations_p.push_back((*polMap_p)[Stokes::XY]);
			getCorr_p = &VisMapper::linear_xy;
			matchExpression = true;
		}
		else
		{
			throw AipsError("Requested polarization parameter (XY) not available");
		}
	}
	if (expression_p.find("YX") != string::npos)
	{
		if (polMap_p->find(Stokes::YX) != polMap_p->end())
		{
			selectedCorrelations_p.push_back((*polMap_p)[Stokes::YX]);
			getCorr_p = &VisMapper::linear_yx;
			matchExpression = true;
		}
		else
		{
			throw AipsError("Requested polarization parameter (YX) not available");
		}
	}
	if (expression_p.find("RR") != string::npos)
	{
		if (polMap_p->find(Stokes::RR) != polMap_p->end())
		{
			selectedCorrelations_p.push_back((*polMap_p)[Stokes::RR]);
			getCorr_p = &VisMapper::circular_rr;
			matchExpression = true;
		}
		else
		{
			throw AipsError("Requested polarization parameter (RR) not available");
		}
	}
	if (expression_p.find("LL") != string::npos)
	{
		if (polMap_p->find(Stokes::LL) != polMap_p->end())
		{
			selectedCorrelations_p.push_back((*polMap_p)[Stokes::LL]);
			getCorr_p = &VisMapper::circular_ll;
			matchExpression = true;
		}
		else
		{
			throw AipsError("Requested polarization parameter (LL) not available");
		}
	}
	if (expression_p.find("LR") != string::npos)
	{
		if (polMap_p->find(Stokes::LR) != polMap_p->end())
		{
			selectedCorrelations_p.push_back((*polMap_p)[Stokes::LR]);
			getCorr_p = &VisMapper::circular_lr;
			matchExpression = true;
		}
		else
		{
			throw AipsError("Requested polarization parameter (LR) not available");
		}
	}
	if (expression_p.find("RL") != string::npos)
	{
		if (polMap_p->find(Stokes::RL) != polMap_p->end())
		{
			selectedCorrelations_p.push_back((*polMap_p)[Stokes::RL]);
			getCorr_p = &VisMapper::circular_rl;
			matchExpression = true;
		}
		else
		{
			throw AipsError("Requested polarization parameter (RL) not available");
		}
	}
	if (expression_p.find("I") != string::npos)
	{
		if (polMap_p->find(Stokes::I) != polMap_p->end())
		{
			selectedCorrelations_p.push_back((*polMap_p)[Stokes::I]);
			getCorr_p = &VisMapper::stokes_i;
			matchExpression = true;
		}
		else if ((polMap_p->find(Stokes::XX) != polMap_p->end()) and (polMap_p->find(Stokes::YY) != polMap_p->end()))
		{
			selectedCorrelations_p.push_back((*polMap_p)[Stokes::XX]);
			selectedCorrelations_p.push_back((*polMap_p)[Stokes::YY]);
			getCorr_p = &VisMapper::stokes_i_from_linear;
			matchExpression = true;
		}
		else if ((polMap_p->find(Stokes::RR) != polMap_p->end()) and (polMap_p->find(Stokes::LL) != polMap_p->end()))
		{
			selectedCorrelations_p.push_back((*polMap_p)[Stokes::RR]);
			selectedCorrelations_p.push_back((*polMap_p)[Stokes::LL]);
			getCorr_p = &VisMapper::stokes_i_from_circular;
			matchExpression = true;
		}
		else
		{
			throw AipsError("Requested Stokes parameter (I) cannot be computed from available polarizations");
		}
	}
	if (expression_p.find("Q") != string::npos)
	{
		if (polMap_p->find(Stokes::Q) != polMap_p->end())
		{
			selectedCorrelations_p.push_back((*polMap_p)[Stokes::Q]);
			getCorr_p = &VisMapper::stokes_q;
			matchExpression = true;
		}
		else if ((polMap_p->find(Stokes::XX) != polMap_p->end()) and (polMap_p->find(Stokes::YY) != polMap_p->end()))
		{
			selectedCorrelations_p.push_back((*polMap_p)[Stokes::XX]);
			selectedCorrelations_p.push_back((*polMap_p)[Stokes::YY]);
			getCorr_p = &VisMapper::stokes_q_from_linear;
			matchExpression = true;
		}
		else if ((polMap_p->find(Stokes::RL) != polMap_p->end()) and (polMap_p->find(Stokes::LR) != polMap_p->end()))
		{
			selectedCorrelations_p.push_back((*polMap_p)[Stokes::RL]);
			selectedCorrelations_p.push_back((*polMap_p)[Stokes::LR]);
			getCorr_p = &VisMapper::stokes_q_from_circular;
			matchExpression = true;
		}
		else
		{
			throw AipsError("Requested Stokes parameter (Q) cannot be computed from available polarizations");
		}
	}
	if (expression_p.find("U") != string::npos)
	{
		if (polMap_p->find(Stokes::U) != polMap_p->end())
		{
			selectedCorrelations_p.push_back((*polMap_p)[Stokes::U]);
			getCorr_p = &VisMapper::stokes_u;
			matchExpression = true;
		}
		else if ((polMap_p->find(Stokes::XY) != polMap_p->end()) and (polMap_p->find(Stokes::YX) != polMap_p->end()))
		{
			selectedCorrelations_p.push_back((*polMap_p)[Stokes::XY]);
			selectedCorrelations_p.push_back((*polMap_p)[Stokes::YX]);
			getCorr_p = &VisMapper::stokes_u_from_linear;
			matchExpression = true;
		}
		else if ((polMap_p->find(Stokes::RL) != polMap_p->end()) and (polMap_p->find(Stokes::LR) != polMap_p->end()))
		{
			selectedCorrelations_p.push_back((*polMap_p)[Stokes::RL]);
			selectedCorrelations_p.push_back((*polMap_p)[Stokes::LR]);
			getCorr_p = &VisMapper::stokes_u_from_circular;
			matchExpression = true;
		}
		else
		{
			throw AipsError("Requested Stokes parameter (U) cannot be computed from available polarizations");
		}
	}
	if ((expression_p.find("V") != string::npos) and (expression_p.find("WVR") == string::npos))
	{
		if (polMap_p->find(Stokes::V) != polMap_p->end())
		{
			selectedCorrelations_p.push_back((*polMap_p)[Stokes::V]);
			getCorr_p = &VisMapper::stokes_v;
			matchExpression = true;
		}
		else if ((polMap_p->find(Stokes::XY) != polMap_p->end()) and (polMap_p->find(Stokes::YX) != polMap_p->end()))
		{
			selectedCorrelations_p.push_back((*polMap_p)[Stokes::XY]);
			selectedCorrelations_p.push_back((*polMap_p)[Stokes::YX]);
			getCorr_p = &VisMapper::stokes_v_from_linear;
			matchExpression = true;
		}
		else if ((polMap_p->find(Stokes::RR) != polMap_p->end()) and (polMap_p->find(Stokes::LL) != polMap_p->end()))
		{
			selectedCorrelations_p.push_back((*polMap_p)[Stokes::RR]);
			selectedCorrelations_p.push_back((*polMap_p)[Stokes::LL]);
			getCorr_p = &VisMapper::stokes_v_from_circular;
			matchExpression = true;
		}
		else
		{
			throw AipsError("Requested Stokes parameter (V) cannot be computed from available polarizations");
		}
	}

	if (!matchExpression)
	{
		throw AipsError("Unknown polarization requested, (" + expression_p + ") supported types are: XX,YY,XY,YX,RR,LL,RL,LR,I,Q,U,V");
	}
}


VisMapper::~VisMapper()
{
	if (rightVis_p != NULL) delete rightVis_p;
	if (leftVis_p != NULL) delete leftVis_p;
}

Float
VisMapper::operator()(uInt chan, uInt row)
{
	Complex val = (*this.*getCorr_p)(chan,row);
	return (*this.*applyVisExpr_p)(val);
}

Float
VisMapper::operator()(uInt pol, uInt chan, uInt row)
{
	Complex val = (*this.*getVis_p)(pol,chan,row);
	return (*this.*applyVisExpr_p)(val);
}

Complex
VisMapper::leftVis(uInt pol, uInt chan, uInt row)
{
	return leftVis_p->operator()(pol,chan,row);
}

Complex
VisMapper::diffVis(uInt pol, uInt chan, uInt row)
{
	return leftVis_p->operator()(pol,chan,row)-rightVis_p->operator()(pol,chan,row);
}

Complex
VisMapper::stokes_i(uInt chan, uInt row)
{
	return (*this.*getVis_p)((*polMap_p)[Stokes::I],chan,row);
}

Complex
VisMapper::stokes_q(uInt chan, uInt row)
{
	return (*this.*getVis_p)((*polMap_p)[Stokes::Q],chan,row);
}

Complex
VisMapper::stokes_u(uInt chan, uInt row)
{
	return (*this.*getVis_p)((*polMap_p)[Stokes::U],chan,row);
}

Complex
VisMapper::stokes_v(uInt chan, uInt row)
{
	return (*this.*getVis_p)((*polMap_p)[Stokes::V],chan,row);
}

Complex
VisMapper::linear_xx(uInt chan, uInt row)
{
	return (*this.*getVis_p)((*polMap_p)[Stokes::XX],chan,row);
}

Complex
VisMapper::linear_yy(uInt chan, uInt row)
{
	return (*this.*getVis_p)((*polMap_p)[Stokes::YY],chan,row);
}

Complex
VisMapper::linear_xy(uInt chan, uInt row)
{
	return (*this.*getVis_p)((*polMap_p)[Stokes::XY],chan,row);
}

Complex
VisMapper::linear_yx(uInt chan, uInt row)
{
	return (*this.*getVis_p)((*polMap_p)[Stokes::YX],chan,row);
}

Complex
VisMapper::circular_rr(uInt chan, uInt row)
{
	return (*this.*getVis_p)((*polMap_p)[Stokes::RR],chan,row);
}

Complex
VisMapper::circular_ll(uInt chan, uInt row)
{
	return (*this.*getVis_p)((*polMap_p)[Stokes::LL],chan,row);
}

Complex
VisMapper::circular_rl(uInt chan, uInt row)
{
	return (*this.*getVis_p)((*polMap_p)[Stokes::RL],chan,row);
}

Complex
VisMapper::circular_lr(uInt chan, uInt row)
{
	return (*this.*getVis_p)((*polMap_p)[Stokes::LR],chan,row);
}

Complex
VisMapper::stokes_i_from_linear(uInt chan, uInt row)
{
	// I = (XX+YY)/2
	return ((*this.*getVis_p)((*polMap_p)[Stokes::XX],chan,row) + (*this.*getVis_p)((*polMap_p)[Stokes::YY],chan,row))/2;
}

Complex
VisMapper::stokes_q_from_linear(uInt chan, uInt row)
{
	// Q = (XX-YY)/2
	return ((*this.*getVis_p)((*polMap_p)[Stokes::XX],chan,row) - (*this.*getVis_p)((*polMap_p)[Stokes::YY],chan,row))/2;
}

Complex
VisMapper::stokes_u_from_linear(uInt chan, uInt row)
{
	// U = (XY-YX)/2
	return ((*this.*getVis_p)((*polMap_p)[Stokes::XY],chan,row) - (*this.*getVis_p)((*polMap_p)[Stokes::YX],chan,row))/2;
}

Complex
VisMapper::stokes_v_from_linear(uInt chan, uInt row)
{
	// V = (XY-YX)/2i
	return ((*this.*getVis_p)((*polMap_p)[Stokes::XY],chan,row) - (*this.*getVis_p)((*polMap_p)[Stokes::YX],chan,row))/(2*ImaginaryUnit);
}

Complex
VisMapper::stokes_i_from_circular(uInt chan, uInt row)
{
	// I = (RR+LL)/2
	return ((*this.*getVis_p)((*polMap_p)[Stokes::RR],chan,row) + (*this.*getVis_p)((*polMap_p)[Stokes::LL],chan,row))/2;
}

Complex
VisMapper::stokes_q_from_circular(uInt chan, uInt row)
{
	// Q = (RL-LR)/2
	return ((*this.*getVis_p)((*polMap_p)[Stokes::RL],chan,row) - (*this.*getVis_p)((*polMap_p)[Stokes::LR],chan,row))/2;
}

Complex
VisMapper::stokes_u_from_circular(uInt chan, uInt row)
{
	// U = (RL-LR)/2i
	return ((*this.*getVis_p)((*polMap_p)[Stokes::RL],chan,row) - (*this.*getVis_p)((*polMap_p)[Stokes::LR],chan,row))/(2*ImaginaryUnit);
}

Complex
VisMapper::stokes_v_from_circular(uInt chan, uInt row)
{
	// V = (RR-LL)/2
	return ((*this.*getVis_p)((*polMap_p)[Stokes::RR],chan,row) - (*this.*getVis_p)((*polMap_p)[Stokes::LL],chan,row))/2;
}

//////////////////////////////////////
/// FlagMapper implementation ////////
//////////////////////////////////////
FlagMapper::FlagMapper(Bool flag, 	vector<uInt> selectedCorrelations,
									CubeView<Bool> *commonFlagsView,
									CubeView<Bool> *originalFlagsView,
									CubeView<Bool> *privateFlagsView,
									VectorView<Bool> *commonFlagRowView,
									VectorView<Bool> *originalFlagRowView,
									VectorView<Bool> *privateFlagRowView)
{
	commonFlagsView_p = NULL;
	originalFlagsView_p = NULL;
	privateFlagsView_p = NULL;
	commonFlagRowView_p = NULL;
	originalFlagRowView_p = NULL;
	privateFlagRowView_p = NULL;
	setParentCubes(commonFlagsView,originalFlagsView,privateFlagsView);
	setParentFlagRow(commonFlagRowView,originalFlagRowView,privateFlagRowView);
	setExpressionMapping(selectedCorrelations);
	nSelectedCorrelations_p = selectedCorrelations.size();
	flag_p = flag;
}

FlagMapper::FlagMapper(Bool flag, vector<uInt> selectedCorrelations)
{
	commonFlagsView_p = NULL;
	originalFlagsView_p = NULL;
	privateFlagsView_p = NULL;
	commonFlagRowView_p = NULL;
	originalFlagRowView_p = NULL;
	privateFlagRowView_p = NULL;
	setExpressionMapping(selectedCorrelations);
	nSelectedCorrelations_p = selectedCorrelations.size();
	flag_p = flag;
}

void
FlagMapper::setParentCubes(CubeView<Bool> *commonFlagsView,CubeView<Bool> *originalFlagsView,CubeView<Bool> *privateFlagsView)
{
	if (commonFlagsView_p != NULL) delete commonFlagsView_p;
	if (originalFlagsView_p != NULL) delete originalFlagsView_p;
	if (privateFlagsView_p != NULL) delete privateFlagsView_p;

	commonFlagsView_p = commonFlagsView;
	originalFlagsView_p = originalFlagsView;
	if (privateFlagsView != NULL)
	{
		privateFlagsView_p = privateFlagsView;
		applyFlag_p = &FlagMapper::applyPrivateFlags;
	}
	else
	{
		applyFlag_p = &FlagMapper::applyCommonFlags;
	}

	IPosition commonFlagCubeSize = commonFlagsView_p->shape(); // pol,chan,row
	reducedLength_p = IPosition(2);
	reducedLength_p(0) = commonFlagCubeSize(1); // chan
	reducedLength_p(1) = commonFlagCubeSize(2); // row
	reducedLength_p(2) = commonFlagCubeSize(0); // pol

	flagsPerRow_p = nSelectedCorrelations_p*reducedLength_p(0);
}

void
FlagMapper::setParentFlagRow(VectorView<Bool> *commonFlagRowView,VectorView<Bool> *originalFlagRowView,VectorView<Bool> *privateFlagRowView)
{
	if (commonFlagRowView_p != NULL) delete commonFlagRowView_p;
	if (originalFlagRowView_p != NULL) delete originalFlagRowView_p;
	if (privateFlagRowView_p != NULL) delete privateFlagRowView_p;

	commonFlagRowView_p = commonFlagRowView;
	originalFlagRowView_p = originalFlagRowView;
	if (privateFlagRowView_p != NULL)
	{
		privateFlagRowView_p = privateFlagRowView;
		applyFlagRow_p = &FlagMapper::applyCommonFlagRow;
	}
	else
	{
		applyFlagRow_p = &FlagMapper::applyPrivateFlagRow;
	}

	return;
}


void
FlagMapper::setExpressionMapping(vector<uInt> selectedCorrelations)
{
	selectedCorrelations_p = selectedCorrelations;
	if (selectedCorrelations_p.empty())
	{
		throw AipsError("None of the correlations is eligible for flagging");
	}
}

FlagMapper::~FlagMapper()
{
	if (commonFlagsView_p != NULL) delete commonFlagsView_p;
	if (originalFlagsView_p != NULL) delete originalFlagsView_p;
	if (privateFlagsView_p != NULL) delete privateFlagsView_p;
	if (commonFlagRowView_p != NULL) delete commonFlagRowView_p;
	if (originalFlagRowView_p != NULL) delete originalFlagRowView_p;
	if (privateFlagRowView_p != NULL) delete privateFlagRowView_p;
}

Bool
FlagMapper::getOriginalFlags(uInt channel, uInt row)
{
	Bool combinedFlag = False;
	for (vector<uInt>::iterator iter=selectedCorrelations_p.begin();iter!=selectedCorrelations_p.end();iter++)
	{
		combinedFlag = combinedFlag | originalFlagsView_p->operator ()(*iter,channel,row);
	}

	return combinedFlag;
}

Bool
FlagMapper::getModifiedFlags(uInt channel, uInt row)
{
	Bool combinedFlag = False;
	for (vector<uInt>::iterator iter=selectedCorrelations_p.begin();iter!=selectedCorrelations_p.end();iter++)
	{
		combinedFlag = combinedFlag | commonFlagsView_p->operator ()(*iter,channel,row);
	}

	return combinedFlag;
}

Bool
FlagMapper::getPrivateFlags(uInt channel, uInt row)
{
	Bool combinedFlag = False;
	for (vector<uInt>::iterator iter=selectedCorrelations_p.begin();iter!=selectedCorrelations_p.end();iter++)
	{
		combinedFlag = combinedFlag | privateFlagsView_p->operator ()(*iter,channel,row);
	}

	return combinedFlag;
}

Bool
FlagMapper::getOriginalFlags(uInt pol, uInt channel, uInt row)
{
	return originalFlagsView_p->operator ()(pol,channel,row);
}

Bool
FlagMapper::getModifiedFlags(uInt pol, uInt channel, uInt row)
{
	return commonFlagsView_p->operator ()(pol,channel,row);
}

Bool
FlagMapper::getPrivateFlags(uInt pol, uInt channel, uInt row)
{
	return privateFlagsView_p->operator ()(pol,channel,row);
}

Bool
FlagMapper::getOriginalFlagRow(uInt row)
{
	return originalFlagRowView_p->operator ()(row);
}

Bool
FlagMapper::getModifiedFlagRow(uInt row)
{
	return commonFlagRowView_p->operator ()(row);
}

Bool
FlagMapper::getPrivateFlagRow(uInt row)
{
	return privateFlagRowView_p->operator ()(row);
}

void
FlagMapper::setModifiedFlags(uInt pol, uInt channel, uInt row)
{
	commonFlagsView_p->operator ()(pol,channel,row) = flag_p;
}

void
FlagMapper::setPrivateFlags(uInt pol, uInt channel, uInt row)
{
	privateFlagsView_p->operator ()(pol,channel,row) = flag_p;
}

void
FlagMapper::applyFlag(uInt channel, uInt row)
{
	for (vector<uInt>::iterator iter=selectedCorrelations_p.begin();iter!=selectedCorrelations_p.end();iter++)
	{
		(*this.*applyFlag_p)(*iter,channel,row);
	}
}

void
FlagMapper::applyFlagRow(uInt row)
{
	// Flag cube
	for (uInt chan_i=0;chan_i<reducedLength_p(0);chan_i++)
	{
		for (vector<uInt>::iterator iter=selectedCorrelations_p.begin();iter!=selectedCorrelations_p.end();iter++)
		{
			(*this.*applyFlag_p)(*iter,chan_i,row);
		}
	}

	// Flag row
	(*this.*applyFlagRow_p)(row);
}

void
FlagMapper::applyCommonFlags(uInt pol, uInt channel, uInt row)
{
	// NOTE: Notice that the position is pol,channel,row, not the other way around
	commonFlagsView_p->operator()(pol,channel,row) = flag_p;
	return;
}

void
FlagMapper::applyPrivateFlags(uInt pol, uInt channel, uInt row)
{
	// NOTE: Notice that the position is pol,channel,row, not the other way around
	commonFlagsView_p->operator()(pol,channel,row) = flag_p;
	privateFlagsView_p->operator()(pol,channel,row) = flag_p;
	return;
}

void
FlagMapper::checkCommonFlags(uInt pol, uInt channel, uInt row)
{
	// NOTE: Notice that the position is pol,channel,row, not the other way around
	if (commonFlagsView_p->operator()(pol,channel,row) != flag_p)
	{
		cerr << "FlagMapper::" << __FUNCTION__ <<  " Flag missmatch at pol=" << pol << ",channel=" << channel << ",row=" << row << endl;
	}
	return;
}

void
FlagMapper::applyCommonFlagRow(uInt row)
{
	// NOTE: Notice that the position is pol,channel,row, not the other way around
	commonFlagRowView_p->operator()(row) = flag_p;
	return;
}

void
FlagMapper::applyPrivateFlagRow(uInt row)
{
	// NOTE: Notice that the position is pol,channel,row, not the other way around
	commonFlagRowView_p->operator()(row) = flag_p;
	originalFlagRowView_p->operator()(row) = flag_p;
	return;
}

} //# NAMESPACE CASA - END

