//# PlotMSAction.cc: Actions on plotms that can be triggered.
//# Copyright (C) 2009
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
//# $Id: $
#include <plotms/Actions/PlotMSAction.h>

#include <casa/Logging/LogFilter.h>
#include <ms/MeasurementSets/MSSummary.h>
#include <plotms/Actions/PlotMSExportThread.qo.h>
#include <plotms/Gui/PlotMSPlotter.qo.h>
#include <plotms/GuiTabs/PlotMSFlaggingTab.qo.h>
#include <plotms/GuiTabs/PlotMSPlotTab.qo.h>
#include <plotms/PlotMS/PlotMS.h>
#include <plotms/Plots/PlotMSPlot.h>
#include <plotms/Plots/PlotMSPlotParameterGroups.h>
#include <plotms/Data/MSCache.h>
#include <plotms/Data/PlotMSIndexer.h>

#include <iomanip>

namespace casa {

//////////////////////////////
// PLOTMSACTION DEFINITIONS //
//////////////////////////////

// Static //

const String PlotMSAction::P_PLOT = "plot";
const String PlotMSAction::P_ON_OFF = "on_off";
const String PlotMSAction::P_AXES = "axes";
const String PlotMSAction::P_FILE = "file";
const String PlotMSAction::P_FORMAT = "format";
const String PlotMSAction::P_HIGHRES = "highres";
const String PlotMSAction::P_DPI = "dpi";
const String PlotMSAction::P_WIDTH = "width";
const String PlotMSAction::P_HEIGHT = "height";
const String PlotMSAction::P_INTERACTIVE = "interactive";


bool PlotMSAction::requires(Type type, const String& parameter) {
    switch(type) {
    case TOOL_MARK_REGIONS: 
    case TOOL_SUBTRACT_REGIONS: 
    case TOOL_ZOOM: 
    case TOOL_PAN:
    case TOOL_ANNOTATE_TEXT: 
    case TOOL_ANNOTATE_RECTANGLE: 
    case TRACKER_ENABLE_HOVER:
    case TRACKER_ENABLE_DISPLAY: 
    case HOLD_RELEASE_DRAWING:
        return parameter == P_ON_OFF;

    case CACHE_LOAD: case CACHE_RELEASE:
        return parameter == P_PLOT || parameter == P_AXES;

    case MS_SUMMARY: return parameter == P_PLOT;
        
    case PLOT_EXPORT:
        return parameter == P_PLOT || parameter == P_FILE;
    
    default: return false;
    }
}


// Constructors/Destructors //

PlotMSAction::PlotMSAction(Type type) : itsType_(type) { }

PlotMSAction::~PlotMSAction() { }


// Public Methods //

PlotMSAction::Type PlotMSAction::type() const { return itsType_; }

bool PlotMSAction::isValid() const {
	switch(itsType_) {
	case TOOL_MARK_REGIONS: 
	case TOOL_SUBTRACT_REGIONS: 
	case TOOL_ZOOM: 
	case TOOL_PAN:
	case TOOL_ANNOTATE_TEXT: 
	case TOOL_ANNOTATE_RECTANGLE: 
	case TRACKER_ENABLE_HOVER:
	case TRACKER_ENABLE_DISPLAY: 
	case HOLD_RELEASE_DRAWING:
		return isDefinedBool(P_ON_OFF);

	case CACHE_LOAD: case CACHE_RELEASE:
		return isDefinedPlot(P_PLOT) && valuePlot(P_PLOT) != NULL &&
		       isDefinedAxes(P_AXES) && valueAxes(P_AXES).size() > 0;

	case MS_SUMMARY:
	    return isDefinedPlot(P_PLOT) && valuePlot(P_PLOT) != NULL;
		       
	case PLOT_EXPORT:
		return isDefinedPlot(P_PLOT) && valuePlot(P_PLOT) != NULL &&
		       isDefinedString(P_FILE) && !valueString(P_FILE).empty();

	case SEL_FLAG: case SEL_UNFLAG: case SEL_LOCATE: case SEL_CLEAR_REGIONS:
	case ITER_FIRST: case ITER_PREV: case ITER_NEXT: case ITER_LAST:
	case STACK_BACK: case STACK_BASE: case STACK_FORWARD: case PLOT:
	case CLEAR_PLOTTER: case QUIT: case SEL_INFO:
		return true;

	default: return false;
	}
}

void PlotMSAction::setParameter(const String& parameter, PlotMSPlot* value) {
	itsPlotValues_[parameter] = value; }
void PlotMSAction::setParameter(const String& parameter, bool value) {
	itsBoolValues_[parameter] = value; }
void PlotMSAction::setParameter(const String& p, const vector<PMS::Axis>& v) {
	itsAxesValues_[p] = v; }
void PlotMSAction::setParameter(const String& parameter, const String& value) {
	itsStringValues_[parameter] = value; }
void PlotMSAction::setParameter(const String& parameter, int value) {
	itsIntValues_[parameter] = value; }





bool PlotMSAction::doAction(PlotMSApp* plotms) {
	itsDoActionResult_ = "";

	if(!isValid() || plotms == NULL) {
		itsDoActionResult_ = "Set parameters were not valid!";
		return false;
	}

	switch(itsType_) {
	
	case SEL_FLAG: 
	case SEL_UNFLAG: 
	case SEL_LOCATE: {
	    // Locate/Flag/Unflag on all visible canvases.
	    const vector<PlotMSPlot*>& plots = plotms->getPlotManager().plots();
	    vector<PlotCanvasPtr> visibleCanv = plotms->getPlotter()
                                            ->currentCanvases();
	    
	    // Get flagging parameters.
	    PlotMSFlagging flagging = plotms->getPlotter()->getFlaggingTab()
	                              ->getValue();

	    // Keep list of plots that have to be redrawn.
	    vector<PlotMSPlot*> redrawPlots;

	    PlotMSPlot* plot;
	    for(unsigned int i = 0; i < plots.size(); i++) {
	        plot = plots[i];
	        if(plot == NULL) continue;

	        // Get parameters.
	        PlotMSPlotParameters& params = plot->parameters();
	        PMS_PP_Cache* c = params.typedGroup<PMS_PP_Cache>();

		// Detect if we are showing flagged/unflagged points (for locate)
	        PMS_PP_Display* d = params.typedGroup<PMS_PP_Display>();
		Bool showUnflagged=(d->unflaggedSymbol()->symbol()!=PlotSymbol::NOSYMBOL);
		Bool showFlagged=(d->flaggedSymbol()->symbol()!=PlotSymbol::NOSYMBOL);

	        vector<PlotCanvasPtr> canv = plot->canvases();
	        for(unsigned int j = 0; j < canv.size(); j++) {
	            // Only apply to visible canvases.
	            bool visible = false;
	            for(unsigned int k= 0; !visible && k < visibleCanv.size(); k++)
	                if(canv[j] == visibleCanv[k]) visible = true;
	            if(!visible) continue;

	            // Get selected regions on that canvas.
	            vector<PlotRegion> regions= canv[j]->standardMouseTools()
                                            ->selectTool()->getSelectedRects();
	            if(regions.size() == 0) continue;
	            
	            // Actually do locate/flag/unflag...
	            PlotLogMessage* m = NULL;
	            try {
	                if(itsType_ == SEL_LOCATE) {
			  m = plot->cache().indexer(plot->iter()).locateRange(Vector<PlotRegion>(regions),
									      showUnflagged,showFlagged);
	                } else {
			  m = plot->cache().indexer(plot->iter()).flagRange(flagging,
									    Vector<PlotRegion>(regions),
									    itsType_ == SEL_FLAG);
	                }
	                
	            // ...and catch any reported errors.
	            } catch(AipsError& err) {
                    itsDoActionResult_ = "Error during ";
                    if(itsType_ == SEL_LOCATE) itsDoActionResult_ += "locate";
                    else if(itsType_==SEL_FLAG) itsDoActionResult_+="flagging";
                    else itsDoActionResult_ += "unflagging";
                    itsDoActionResult_ += ": " + err.getMesg();
                    return false;
	            } catch(...) {
                    itsDoActionResult_ = "Unknown error during ";
                    if(itsType_ == SEL_LOCATE) itsDoActionResult_ += "locate";
                    else if(itsType_==SEL_FLAG) itsDoActionResult_+="flagging";
                    else itsDoActionResult_ += "unflagging";
                    itsDoActionResult_ += "!";
                    return false;
	            }

	            // Log results.
                if(m != NULL) {
                    stringstream msg;
                    
                    // For multiple plots or canvases, add a note at the
                    // beginning to indicate which one this is.
                    if(plots.size() > 1 || canv.size() > 1) {
                        msg << "[";
                        
                        if(plots.size() > 1) msg << "Plot #" << i;
                        if(plots.size() > 1 && canv.size() > 1) msg << ", ";
                        if(canv.size() > 1) msg << "Canvas #" << j;
                        
                        msg << "]: ";
                    }
                    
                    // Append region values for x-axis.
                    msg << PMS::axis(c->xAxis()) << " in ";                    
                    for(unsigned int k = 0; k < regions.size(); k++) {
                        if(k > 0) msg << " or ";
                        msg << "[" << regions[k].left() << " "
                            << regions[k].right() << "]";
                    }
                    
                    // Append region values for y-axis.
                    msg << ", " << PMS::axis(c->yAxis()) << " in ";
                    for(unsigned int k = 0; k < regions.size(); k++) {
                        if(k > 0) msg << " or ";
                        msg << "[" << regions[k].bottom() << " "
                            << regions[k].top() << "]";
                    }
                    
                    // Append result as returned by cache.
                    msg << ":\n" << m->message();
                    m->message(msg.str(), true);
                    
                    // Log message.
                    plotms->getLogger()->postMessage(*m);
                    
                    delete m;
                    m = NULL;
                }
	            
	            // If this plot was flagged/unflagged, add it to the redraw
	            // list.
	            if(itsType_ == SEL_FLAG || itsType_ == SEL_UNFLAG)
	                redrawPlots.push_back(plot);
	        }
	        
	        // For a flag/unflag, need to tell the plots to redraw themselves,
	        // and clear selected regions.
	        if(itsType_ == SEL_FLAG || itsType_ == SEL_UNFLAG) {
	            bool hold = plotms->getPlotter()->allDrawingHeld();
	            if(!hold) plotms->getPlotter()->holdDrawing();
	            
	            for(unsigned int i = 0; i < redrawPlots.size(); i++) {
	                redrawPlots[i]->plotDataChanged();
	                
	                vector<PlotCanvasPtr> canv = plot->canvases();
	                for(unsigned int j = 0; j < canv.size(); j++) {
	                    // Only apply to visible canvases.
	                     bool visible = false;
	                     for(unsigned int k = 0;
	                         !visible && k < visibleCanv.size(); k++)
	                         if(canv[j] == visibleCanv[k]) visible = true;
	                     if(!visible) continue;
	                     
	                     canv[j]->standardMouseTools()->selectTool()
	                            ->clearSelectedRects();
	                }
	            }	            
	            
	            if(!hold) plotms->getPlotter()->releaseDrawing();
	        }
	    }
	    return true;
	}


	case SEL_CLEAR_REGIONS: {
		const vector<PlotMSPlot*>& plots = plotms->getPlotManager().plots();
		vector<PlotCanvasPtr> visibleCanv = plotms->getPlotter()
		                                    ->currentCanvases();
		vector<PlotCanvasPtr> canv;
		bool found;

		for(unsigned int i = 0; i < plots.size(); i++) {
			if(plots[i] == NULL) continue;
			canv = plots[i]->canvases();
			for(unsigned int j = 0; j < canv.size(); j++) {
				if(canv[j].null()) continue;

				// Only apply to visible canvases.
				found = false;
				for(unsigned int k = 0; !found && k < visibleCanv.size(); k++)
					if(canv[j] == visibleCanv[k]) found = true;
				if(!found) continue;

				canv[j]->standardMouseTools()->selectTool()
				       ->clearSelectedRects();
			}
		}

		return true;
	}


	case TOOL_MARK_REGIONS:	
	case TOOL_SUBTRACT_REGIONS:	
	case TOOL_ZOOM:	
	case TOOL_PAN:
	case TOOL_ANNOTATE_TEXT: 
	case TOOL_ANNOTATE_RECTANGLE: {
		
		bool on = valueBool(P_ON_OFF);
	    ToolCode toolcode;  
	    
	    toolcode=NONE_TOOL;
	    
	    // Set tool to enum value corresponding to desired tool,
	    // based on this Action's type (also an enum).
	    // Note that the SelectTool, as the entity holding the rectangles, doubles 
	    // for selection and for subtracting selections, distinguished by
	    // having separate enum values.
	    // 
	    if      (on && (itsType_ == TOOL_MARK_REGIONS))
	    	toolcode = SELECT_TOOL;
	    else if (on && (itsType_ == TOOL_SUBTRACT_REGIONS))
	    	toolcode = SUBTRACT_TOOL;
	    else if (on && (itsType_ == TOOL_ZOOM))
	    	toolcode = ZOOM_TOOL;
	    else if (on && (itsType_ == TOOL_PAN))
	    	toolcode = PAN_TOOL;
	    
	    bool useAnnotator = on && (itsType_ == TOOL_ANNOTATE_TEXT ||
	                        itsType_ == TOOL_ANNOTATE_RECTANGLE);
	    PlotMSAnnotator::Mode annotate = PlotMSAnnotator::TEXT;
	    if(on && itsType_ == TOOL_ANNOTATE_RECTANGLE)
	        annotate = PlotMSAnnotator::RECTANGLE;
	    
	    const vector<PlotMSPlot*>& plots = plotms->getPlotManager().plots();
	    vector<PlotCanvasPtr> canv;
	    for(unsigned int i = 0; i < plots.size(); i++) {
	    	if(plots[i] == NULL) continue;
	    	canv = plots[i]->canvases();
	    	for(unsigned int j = 0; j < canv.size(); j++) {
	    		if(canv[j].null()) continue;
	    		
	    		// Update standard mouse tools.
	    		canv[j]->standardMouseTools()->setActive(!useAnnotator);
	    		if(!useAnnotator)   {
					PlotStandardMouseToolGroupPtr x;
	    		    x=canv[j]->standardMouseTools();
	    		    x->setActiveTool(toolcode);
	    		}
	    	}
	    }
	    
	    // Update annotator.
	    PlotMSPlotter* plotter = plotms->getPlotter();
	    if(useAnnotator) plotter->getAnnotator().setDrawingMode(annotate);
	    plotter->getAnnotator().setActive(useAnnotator);

	    return true;
	}



	case TRACKER_ENABLE_HOVER:	
	case TRACKER_ENABLE_DISPLAY: {
	    PlotMSPlotter* plotter = plotms->getPlotter();
	    bool hover = itsType_ == TRACKER_ENABLE_HOVER ? valueBool(P_ON_OFF) :
                     plotter->actionIsChecked(TRACKER_ENABLE_HOVER),
           display = itsType_ == TRACKER_ENABLE_DISPLAY ? valueBool(P_ON_OFF) :
	                 plotter->actionIsChecked(TRACKER_ENABLE_DISPLAY);

   	    const vector<PlotMSPlot*>& plots = plotms->getPlotManager().plots();
   	    vector<PlotCanvasPtr> canv;
   	    for(unsigned int i = 0; i < plots.size(); i++) {
   	    	if(plots[i] == NULL) continue;
   	    	canv = plots[i]->canvases();
   	    	for(unsigned int j = 0; j < canv.size(); j++) {
   	    		if(canv[j].null()) continue;
   		    	canv[i]->standardMouseTools()->turnTracker(hover | display);
   		    	canv[i]->standardMouseTools()->turnTrackerDrawText(hover);
   	    	}
   	    }

	    return true;
	}

	case STACK_BACK: 
	case STACK_BASE: 
	case STACK_FORWARD: {
	    int delta = 0;
	    if(itsType_ == STACK_BACK)         delta = -1;
	    else if(itsType_ == STACK_FORWARD) delta = 1;
	    vector<PlotCanvasPtr> canv = plotms->getPlotter()->currentCanvases();
	    for(unsigned int i = 0; i < canv.size(); i++) {
	        if(canv[i].null()) continue;
	        canv[i]->axesStackMove(delta);
	    }
	    return true;
	}

	case CACHE_LOAD: 
	case CACHE_RELEASE: {
		PlotMSPlot* plot = valuePlot(P_PLOT);
		vector<PMS::Axis> axes = valueAxes(P_AXES);

	    PlotMSPlotParameters& params = plot->parameters();
	    PMS_PP_MSData* paramsData = params.typedGroup<PMS_PP_MSData>();
	    PMS_PP_Cache* paramsCache = params.typedGroup<PMS_PP_Cache>();
	    if(paramsData == NULL || paramsData->filename().empty()) {
	    	itsDoActionResult_ = "MS has not been loaded into the cache!";
	    	return false;
	    }
	    if(paramsCache == NULL) {
	        itsDoActionResult_ = "Cache parameters not available!  (Shouldn't "
	                             "happen.)";
	        return false;
	    }

	    PlotMSCacheBase& cache = plot->cache();
	    vector<PMS::Axis> a;

	    // Remove any duplicates or axes.  If loading, also make sure that the
	    // given axes are not already loaded.  If releasing, make sure that the
	    // axes are loaded.
	    vector<pair<PMS::Axis, unsigned int> > loaded = cache.loadedAxes();
	    bool valid;
	    for(unsigned int i = 0; i < axes.size(); i++) {
	        valid = true;
	        for(unsigned int j = 0; valid && j < a.size(); j++)
	            if(a[j] == axes[i]) valid = false;

	        if(valid) {
	            if(itsType_ == CACHE_LOAD) {
	                for(unsigned int j = 0; valid && j < loaded.size(); j++)
	                    if(loaded[j].first == axes[i]) valid = false;
	            } else {
	                valid = false;
	                for(unsigned int j = 0; !valid && j < loaded.size(); j++)
	                    if(loaded[j].first == axes[i]) valid = true;
	            }
	        }

	        if(valid) a.push_back(axes[i]);
	    }

	    // Make sure that currently used axes and/or meta-data isn't being
	    // released.
	    if(itsType_ == CACHE_RELEASE) {
	        stringstream ss;
	        ss << "The following axes could not be released because they are "
	              "currently in use:";
	        bool removed = false;
	        PMS::Axis x = paramsCache->xAxis(), y = paramsCache->yAxis();
	        for(int i = 0; i < (int)a.size(); i++) {
	            if(a[i]== x || a[i]== y || MSCache::axisIsMetaData(a[i])) {
	                if(removed) ss << ',';
	                ss << ' ' << PMS::axis(a[i]);
	                a.erase(a.begin() + i);
	                i--;
	                removed = true;
	            }
	        }
	        if(removed) {
	            ss << '.';
	            
	               plotms->getLogger()->postMessage(PMS::LOG_ORIGIN,
	                        PMS::LOG_ORIGIN_RELEASE_CACHE, ss.str(),
	                        PlotLogger::MSG_WARN);
	        }
	    }

	    if(a.size() > 0) {
	        PlotMSCacheThread* ct;
	        
	        if(itsType_ == CACHE_LOAD) {

		  //		  cout << "    calling PlotMSCacheThread from PlotMSAction" << endl;
	            ct = new PlotMSCacheThread(plot, &plot->cache(), a,
	                    vector<PMS::DataColumn>(a.size(), PMS::DEFAULT_DATACOLUMN),
	                    paramsData->filename(),paramsData->selection(),
			    paramsData->averaging(), paramsData->transformations(),
			    false,
	                    &PMS_PP_Cache::notifyWatchers, paramsCache);

	        } else {
	            ct = new PlotMSCacheThread(plot, a,
	                    &PMS_PP_Cache::notifyWatchers, paramsCache);
	        }
	        
	        plotms->getPlotter()->doThreadedOperation(ct);
	    }

	    return true;
	}
	
	case MS_SUMMARY: {
	    
	    bool success = false, reenableGlobal = false;
	    try {
	        // Get MS.
	        MeasurementSet ms;
	        
	        // Check if MS has already been opened.
		
		// Check if filename has been set but not plotted.
		PlotMSPlotParameters currentlySet = plotms->getPlotter()
		  ->getPlotTab()->currentlySetParameters();
		String filename = PMS_PP_RETCALL(currentlySet, PMS_PP_MSData,
						 filename, "");
		
		// If not, exit.
		if(filename.empty()) {
		  itsDoActionResult_ = "MS has not been opened/set yet!";
		  return false;
		}
	        
		ms= MeasurementSet(filename, TableLock(TableLock::AutoLocking),
				   Table::Old);
	        
	        // Set up MSSummary object.
	        MSSummary mss(ms);
	        
	        // Set up log objects.
	        LogSink sink(LogFilter(plotms->getLogger()->filterMinPriority()));
	        if(!plotms->getLogger()->usingGlobalSink()) {
	            LogSinkInterface* ic = plotms->getLogger()->localSinkCopy();
	            sink.localSink(ic);
	            
	            // Temporarily disable global log sink if we're not using it, since
	            // MSSummary posts to both (how annoying).
	            PlotLogger::disableGlobalSink();
	            reenableGlobal = true;
	        }
	        LogIO log(LogOrigin(PMS::LOG_ORIGIN,PMS::LOG_ORIGIN_SUMMARY),sink);
	        
	        // Log summary of the appropriate type and verbosity.
	        bool vb = plotms->getPlotter()->getPlotTab()->msSummaryVerbose();
	        switch(plotms->getPlotter()->getPlotTab()->msSummaryType()) {
	        case PMS::S_ALL:          mss.list(log, vb); break;
	        case PMS::S_WHERE:        mss.listWhere(log, vb); break;
	        case PMS::S_WHAT:         mss.listWhat(log, vb); break;
	        case PMS::S_HOW:          mss.listHow(log, vb); break;
	        case PMS::S_MAIN:         mss.listMain(log, vb); break;
	        case PMS::S_TABLES:       mss.listTables(log, vb); break;
	        case PMS::S_ANTENNA:      mss.listAntenna(log, vb); break;
	        case PMS::S_FEED:         mss.listFeed(log, vb); break;
	        case PMS::S_FIELD:        mss.listField(log, vb); break;
	        case PMS::S_OBSERVATION:  mss.listObservation(log, vb); break;
	        case PMS::S_HISTORY:      mss.listHistory(log); break;
	        case PMS::S_POLARIZATION: mss.listPolarization(log, vb); break;
	        case PMS::S_SOURCE:       mss.listSource(log, vb); break;
	        case PMS::S_SPW:          mss.listSpectralWindow(log, vb); break;
	        case PMS::S_SPW_POL:      mss.listSpectralAndPolInfo(log,vb);break;
	        case PMS::S_SYSCAL:       mss.listSysCal(log, vb); break;
	        case PMS::S_WEATHER:      mss.listWeather(log, vb); break;
	        }
	        success = true;
	        
	    } catch(AipsError x) {
	        itsDoActionResult_ = x.getMesg();
	    }
	    
	    // Cleanup.
	    if(reenableGlobal) PlotLogger::enableGlobalSink();
        return success;
	    
	}
	
	case PLOT: {
	    plotms->getPlotter()->getPlotTab()->plot();
	    return true;
	}

	case PLOT_EXPORT: {
	    const String& file = valueString(P_FILE);
	    String form = isDefinedString(P_FORMAT) ? valueString(P_FORMAT) : "";
	    bool ok;
	    PlotExportFormat::Type t = PlotExportFormat::exportFormat(form, &ok);
	    if(!ok) {
	        t = PlotExportFormat::typeForExtension(file, &ok);
	        if(!ok) {
	        	itsDoActionResult_ = "Invalid format extension for filename '"+
	        	                     file + "'!";
	            return false;
	        }
	    }

            if(t == PlotExportFormat::TEXT) {
                Record r;
                PlotMSAction action(PlotMSAction::SEL_INFO);
                bool success = action.doActionWithResponse(plotms, r);
                if(r.nfields() < 1) return success;
                // Write record data to file
                ofstream csv_file;
                csv_file.open(file.c_str());
                String xunit = r.asString("xaxis");
                String yunit = r.asString("yaxis");
                r.removeField("xaxis");
                r.removeField("yaxis");
                csv_file << "# x y chan scan field ant1 ant2 ant1name "
                         << "ant2name time freq spw corr offset currchunk irel"
                         << endl;
                csv_file << "# " << xunit << " " << yunit
                         << " None None None None None None None "
                         << "MJD(seconds) GHz None None None None None"
                         << endl;
                for(uInt _field = 0; _field < r.nfields(); ++_field) {
                    ostringstream fs;
                    fs << _field;
                    String field_str = fs.str();
                    Double x = r.subRecord(field_str).asDouble("x");
                    Double y = r.subRecord(field_str).asDouble("y");
                    Int chan = r.subRecord(field_str).asInt("chan");
                    Int scan = r.subRecord(field_str).asInt("scan");
                    Int field = r.subRecord(field_str).asInt("field");
                    Int ant1 = r.subRecord(field_str).asInt("ant1");
                    Int ant2 = r.subRecord(field_str).asInt("ant2");
                    String ant1name =
                        r.subRecord(field_str).asString("ant1name");
                    String ant2name =
                        r.subRecord(field_str).asString("ant2name");
                    //String time = r.subRecord(field_str).asString("time");
                    Double time = r.subRecord(field_str).asDouble("time");
                    Int spw = r.subRecord(field_str).asInt("spw");
                    Double freq = r.subRecord(field_str).asDouble("freq");
                    String corr = r.subRecord(field_str).asString("corr");
                    Int offset = r.subRecord(field_str).asInt("offset");
                    Int currchunk = r.subRecord(field_str).asInt("currchunk");
                    Int irel = r.subRecord(field_str).asInt("irel");
                    int precision = csv_file.precision();
                    if(xunit == "Time") {
                        csv_file << std::setprecision(3) << std::fixed
                                 << x << " ";
                        csv_file.unsetf(ios_base::fixed);
                        csv_file.precision(precision);
                    } else if(xunit == "Frequency") {
                        csv_file << std::setprecision(9) << std::fixed
                                 << x << " ";
                        csv_file.unsetf(ios_base::fixed);
                        csv_file.precision(precision);
                    } else {
                        csv_file << x << " ";
                    }
                    if(yunit == "Time") {
                        csv_file << std::setprecision(3) << std::fixed
                                 << y << " ";
                        csv_file.unsetf(ios_base::fixed);
                        csv_file.precision(precision);
                    } else if(yunit == "Frequency") {
                        csv_file << std::setprecision(9) << std::fixed
                                 << y << " ";
                        csv_file.unsetf(ios_base::fixed);
                        csv_file.precision(precision);
                    } else {
                        csv_file << y << " ";
                    }
                    csv_file << chan << " " << scan << " " << field << " "
                             << ant1 << " " << ant2 << " " << ant1name << " "
                             << ant2name << " ";
                    csv_file << std::setprecision(3) << std::fixed
                             << time << " ";
                    csv_file << std::setprecision(9) << std::fixed
                             << freq << " ";
                    csv_file.unsetf(ios_base::fixed);
                    csv_file.precision(precision);
                    csv_file << spw << " " << corr << " " << offset << " "
                             << currchunk << " " << irel << endl;
                }
                csv_file.close();
                return success;
            }

	    PlotExportFormat format(t, file);
	    format.resolution = isDefinedBool(P_HIGHRES) && valueBool(P_HIGHRES)
	    		? PlotExportFormat::HIGH : PlotExportFormat::SCREEN;
	    format.dpi = isDefinedInt(P_DPI) ? valueInt(P_DPI) : -1;
	    if(format.dpi <= 0) format.dpi = -1;
	    format.width = isDefinedInt(P_WIDTH) ? valueInt(P_WIDTH) : -1;
	    if(format.width <= 0) format.width = -1;
	    format.height = isDefinedInt(P_HEIGHT) ? valueInt(P_HEIGHT) : -1;
	    if(format.height <= 0) format.height = -1;
	    Bool interactive = ! (
	    	isDefinedBool(P_INTERACTIVE) && ! valueBool(P_INTERACTIVE)
	    );
	    
	    // Create Export Thread and start executing it
	    // Note: new object is deleted in PlotMSPlotter::currentThreadFinished() 
	    plotms->getPlotter()->doThreadedOperation(
	    	new PlotMSExportThread(valuePlot(P_PLOT), format, interactive)
	    );
	    
	    return true;
	}

	case HOLD_RELEASE_DRAWING: {
        PlotMSPlotter* plotter = plotms->getPlotter();
        bool hold = valueBool(P_ON_OFF);
        if(hold) plotter->holdDrawing();
        else     plotter->releaseDrawing();

        return true;
	}

	case CLEAR_PLOTTER:
		plotms->getPlotManager().clearPlotsAndCanvases();
		return true;

	case ITER_FIRST: {

	  //	  cout << "***You pressed the FIRST button***" << endl;
	  bool hold = plotms->getPlotter()->allDrawingHeld();
	  if(!hold) plotms->getPlotter()->holdDrawing();

	  if (plotms->getPlotManager().plots()[0]->firstIter())
	    plotms->getPlotManager().plots()[0]->plotDataChanged();
	  else
	    cout << "No more iterations." << endl;

	  if(!hold) plotms->getPlotter()->releaseDrawing();

	  //	  plotms->getPlotter()->getPlotTab()->plot();
	  return true;
	}
	case ITER_PREV: {

	  //	  cout << "***You pressed the PREV button***" << endl;
	  bool hold = plotms->getPlotter()->allDrawingHeld();
	  if(!hold) plotms->getPlotter()->holdDrawing();

	  if (plotms->getPlotManager().plots()[0]->prevIter())
	    plotms->getPlotManager().plots()[0]->plotDataChanged();
	  else
	    cout << "No more iterations." << endl;

	  if(!hold) plotms->getPlotter()->releaseDrawing();

	  //	  plotms->getPlotter()->getPlotTab()->plot();
	  return true;
	}
	case ITER_NEXT: {

	  //	  cout << "***You pressed the NEXT button***" << endl;
	  bool hold = plotms->getPlotter()->allDrawingHeld();
	  if(!hold) plotms->getPlotter()->holdDrawing();

	  if (plotms->getPlotManager().plots()[0]->nextIter())
	    plotms->getPlotManager().plots()[0]->plotDataChanged();
	  else
	    cout << "No more iterations." << endl;

	  if(!hold) plotms->getPlotter()->releaseDrawing();

	  //	  plotms->getPlotter()->getPlotTab()->plot();
	  return true;
	}
	case ITER_LAST: {

	  //	  cout << "***You pressed the LAST button***" << endl;
	  bool hold = plotms->getPlotter()->allDrawingHeld();
	  if(!hold) plotms->getPlotter()->holdDrawing();

	  if (plotms->getPlotManager().plots()[0]->lastIter())
	    plotms->getPlotManager().plots()[0]->plotDataChanged();
	  else
	    cout << "No more iterations." << endl;

	  if(!hold) plotms->getPlotter()->releaseDrawing();

	  //	  plotms->getPlotter()->getPlotTab()->plot();
	  return true;
	}

        case SEL_INFO: {
            Record retval;
            doActionWithResponse(plotms, retval);
            return true;
        }

	case QUIT:
	    //QApplication::setQuitOnLastWindowClosed(true);
		plotms->close();
		return true;

	// Unimplemented actions.
	default:
		itsDoActionResult_ = "Action type is currently unimplemented!";
		return false;
	}

	itsDoActionResult_ = "Unknown action type!";
	return false;
}

bool PlotMSAction::doActionWithResponse(PlotMSApp* plotms, Record &retval) {
    switch(itsType_) {
    case SEL_INFO: {
        // Locate/Flag/Unflag on all visible canvases.
        const vector<PlotMSPlot*>& plots = plotms->getPlotManager().plots();
        vector<PlotCanvasPtr> visibleCanv = plotms->getPlotter()->currentCanvases();

        // Get flagging parameters.
        PlotMSFlagging flagging = plotms->getPlotter()->getFlaggingTab()->getValue();

        // Keep list of plots that have to be redrawn.
        vector<PlotMSPlot*> redrawPlots;

        PlotMSPlot* plot;
        for(unsigned int i = 0; i < plots.size(); i++) {
            plot = plots[i];
            if(plot == NULL) continue;

            // Get parameters.
            PlotMSPlotParameters& params = plot->parameters();

            // Detect if we are showing flagged/unflagged points (for locate)
            PMS_PP_Display* d = params.typedGroup<PMS_PP_Display>();
            Bool showUnflagged=(d->unflaggedSymbol()->symbol()!=PlotSymbol::NOSYMBOL);
            Bool showFlagged=(d->flaggedSymbol()->symbol()!=PlotSymbol::NOSYMBOL);

            vector<PlotCanvasPtr> canv = plot->canvases();
            for(unsigned int j = 0; j < canv.size(); j++) {
                // Only apply to visible canvases.
                bool visible = false;
                for(unsigned int k= 0; !visible && k < visibleCanv.size(); k++)
                    if(canv[j] == visibleCanv[k]) visible = true;
                if(!visible) continue;

                // Get selected regions on that canvas.
                vector<PlotRegion> regions = canv[j]->standardMouseTools()
                    ->selectTool()->getSelectedRects();
                //if(regions.size() == 0) continue;
	            
                // Actually do locate/flag/unflag...
                try {
		  retval = plot->cache().indexer(plot->iter()).locateInfo(Vector<PlotRegion>(regions), showUnflagged, showFlagged);
									  
                    // int n = retval.nfields();
                    // for(uInt r = 0; r < d.nfields(); ++r) {
                    //     retval.defineRecord(n+r, d.subRecord(r));
                    // }
                    // retval.defineRecord(i, d);
	            // ...and catch any reported errors.
                } catch(AipsError& err) {
                    itsDoActionResult_ = "Error during info";
                    itsDoActionResult_ += ": " + err.getMesg();
                    return false;
                } catch(...) {
                    itsDoActionResult_ = "Unknown error during info!";
                    return false;
                }
            }
        }
    } // case SEL_INFO:
    default:
        break;
    } // switch(itsType_)
    return true;
}

const String& PlotMSAction::doActionResult() const{ return itsDoActionResult_;}


// Private Methods //

bool PlotMSAction::isDefinedPlot(const String& parameter) const {
	return itsPlotValues_.find(parameter) != itsPlotValues_.end(); }
bool PlotMSAction::isDefinedBool(const String& parameter) const {
	return itsBoolValues_.find(parameter) != itsBoolValues_.end(); }
bool PlotMSAction::isDefinedString(const String& parameter) const {
	return itsStringValues_.find(parameter) != itsStringValues_.end(); }
bool PlotMSAction::isDefinedInt(const String& parameter) const {
	return itsIntValues_.find(parameter) != itsIntValues_.end(); }
bool PlotMSAction::isDefinedAxes(const String& parameter) const {
	return itsAxesValues_.find(parameter) != itsAxesValues_.end(); }

PlotMSPlot* PlotMSAction::valuePlot(const String& parameter) {
	return itsPlotValues_[parameter]; }
const PlotMSPlot* PlotMSAction::valuePlot(const String& parameter) const {
	return const_cast<map<String, PlotMSPlot*>&>(itsPlotValues_)[parameter]; }
bool PlotMSAction::valueBool(const String& parameter) const {
	return const_cast<map<String, bool>&>(itsBoolValues_)[parameter]; }
const String& PlotMSAction::valueString(const String& parameter) const {
	return const_cast<map<String, String>&>(itsStringValues_)[parameter]; }
int PlotMSAction::valueInt(const String& parameter) const {
	return const_cast<map<String, int>&>(itsIntValues_)[parameter]; }
const vector<PMS::Axis>& PlotMSAction::valueAxes(const String& p) const {
	return const_cast<map<String, vector<PMS::Axis> >&>(itsAxesValues_)[p]; }

}
