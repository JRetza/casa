//# PlotMSDBusApp.cc: Controller for plotms using DBus.
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
#include <plotms/PlotMS/PlotMSDBusApp.h>

#include <plotms/Actions/PlotMSAction.h>
#include <plotms/Gui/PlotMSPlotter.qo.h>
#include <plotms/GuiTabs/PlotMSFlaggingTab.qo.h>
#include <plotms/PlotMS/PlotMS.h>
#include <graphics/GenericPlotter/PlotOptions.h>

#include <plotms/Plots/PlotMSPlotParameterGroups.h>

#include <sys/types.h>
#include <unistd.h>


namespace casa {

///////////////////////////////
// PLOTMSDBUSAPP DEFINITIONS //
///////////////////////////////

// Static //

const String PlotMSDBusApp::APP_NAME = "casaplotms";
const String PlotMSDBusApp::APP_CASAPY_SWITCH = "--casapy";
const String PlotMSDBusApp::APP_LOGFILENAME_SWITCH = "--logfilename";
const String PlotMSDBusApp::APP_LOGFILTER_SWITCH = "--logfilter";

const String PlotMSDBusApp::PARAM_AVERAGING = "averaging";
const String PlotMSDBusApp::PARAM_AXIS_X = "xAxis";
const String PlotMSDBusApp::PARAM_AXIS_Y = "yAxis";
const String PlotMSDBusApp::PARAM_CLEARSELECTIONS = "clearSelections";
const String PlotMSDBusApp::PARAM_DATACOLUMN_X = "xDataColumn";
const String PlotMSDBusApp::PARAM_DATACOLUMN_Y = "yDataColumn";
const String PlotMSDBusApp::PARAM_FILENAME = "filename";
const String PlotMSDBusApp::PARAM_FLAGGING = "flagging";
const String PlotMSDBusApp::PARAM_HEIGHT = "height";
const String PlotMSDBusApp::PARAM_PLOTINDEX = "plotIndex";
const String PlotMSDBusApp::PARAM_PRIORITY = "priority";
const String PlotMSDBusApp::PARAM_SELECTION = "selection";
const String PlotMSDBusApp::PARAM_TRANSFORMATIONS = "transformations";
const String PlotMSDBusApp::PARAM_UPDATEIMMEDIATELY = "updateImmediately";
const String PlotMSDBusApp::PARAM_WIDTH = "width";

const String PlotMSDBusApp::PARAM_EXPORT_FILENAME = "exportfilename";
const String PlotMSDBusApp::PARAM_EXPORT_FORMAT = "exportformat";
const String PlotMSDBusApp::PARAM_EXPORT_HIGHRES = "exporthighres";
const String PlotMSDBusApp::PARAM_EXPORT_INTERACTIVE = "exportinteractive";
const String PlotMSDBusApp::PARAM_EXPORT_ASYNC = "exportasync";

const String PlotMSDBusApp::PARAM_COLORIZE = "colorize";
const String PlotMSDBusApp::PARAM_COLORAXIS = "coloraxis";
const String PlotMSDBusApp::PARAM_CANVASTITLE = "canvastitle";
const String PlotMSDBusApp::PARAM_XAXISLABEL = "xaxislabel";
const String PlotMSDBusApp::PARAM_YAXISLABEL = "yaxislabel";

const String PlotMSDBusApp::PARAM_SHOWMAJORGRID  = "showmajorgrid";
const String PlotMSDBusApp::PARAM_SHOWMINORGRID  = "showminorgrid";
const String PlotMSDBusApp::PARAM_MAJORCOLOR  = "majorcolor";
const String PlotMSDBusApp::PARAM_MINORCOLOR  = "minorcolor";
const String PlotMSDBusApp::PARAM_MAJORSTYLE = "majorstyle";
const String PlotMSDBusApp::PARAM_MINORSTYLE = "minorstyle";
const String PlotMSDBusApp::PARAM_MAJORWIDTH = "majorwidth";
const String PlotMSDBusApp::PARAM_MINORWIDTH = "minorwidth";


const String PlotMSDBusApp::METHOD_GETLOGPARAMS = "getLogParams";
const String PlotMSDBusApp::METHOD_SETLOGPARAMS = "setLogParams";

const String PlotMSDBusApp::METHOD_GETPLOTMSPARAMS = "getPlotMSParams";
const String PlotMSDBusApp::METHOD_SETPLOTMSPARAMS = "setPlotMSParams";
const String PlotMSDBusApp::METHOD_SETCACHEDIMAGESIZETOSCREENRES =
    "setCachedImageSizeToScreenResolution";

const String PlotMSDBusApp::METHOD_GETPLOTPARAMS = "getPlotParams";
const String PlotMSDBusApp::METHOD_SETPLOTPARAMS = "setPlotParams";

const String PlotMSDBusApp::METHOD_GETFLAGGING = "getFlagging";
const String PlotMSDBusApp::METHOD_SETFLAGGING = "setFlagging";

const String PlotMSDBusApp::METHOD_SHOW   = "show";
const String PlotMSDBusApp::METHOD_HIDE   = "hide";
const String PlotMSDBusApp::METHOD_UPDATE = "update";
const String PlotMSDBusApp::METHOD_QUIT   = "quit";

const String PlotMSDBusApp::METHOD_SAVE   = "save";
const String PlotMSDBusApp::METHOD_ISDRAWING   = "isDrawing";
const String PlotMSDBusApp::METHOD_ISCLOSED  = "isClosed";



/* 
 Determine line style enum from a given string.   
 In other parts of CASA, in the , this is done - somewhere....? 
 The reverse function, giving a string from the enum, is in casaqt/implement/QwtPlotter/QPOptions.h 
*/

static
PlotLine::Style  StyleFromString(String s)   {
    PlotLine::Style style = PlotLine::SOLID;  // default, if we can't decipher the given string
    if (PMS::strEq(s, "dot", true))        style=PlotLine::DOTTED; 
    else if (PMS::strEq(s, "dash", true))   style=PlotLine::DASHED; 
    else if (PMS::strEq(s, "noline", true))  style=PlotLine::NOLINE; 
    return style;
}
        



String PlotMSDBusApp::dbusName(pid_t pid) {
    return "plotms_" + String::toString(pid); }

const QString &PlotMSDBusApp::name( ) {
    static QString _name("plotms");
    return _name;
}

// Constructors/Destructors //

PlotMSDBusApp::PlotMSDBusApp(PlotMSApp& plotms) : itsPlotms_(plotms),
        itsUpdateFlag_(false) {
    // Register self as watcher.
    plotms.getPlotManager().addWatcher(this);
}

// Destructor.
PlotMSDBusApp::~PlotMSDBusApp() {
    itsPlotms_.getPlotManager().removeWatcher(this);
    if(dbusSelfIsRegistered()) dbusUnregisterSelf();
}




// Public Methods //

bool PlotMSDBusApp::connectToDBus( const QString & ) {
    bool res = dbusRegisterSelf(dbusName(getpid()));
    log(!res ? "Could not register!" :
        "Successfully registered with name " + dbusSelfRegisteredName() + "!");
    return res;
}




void PlotMSDBusApp::parametersHaveChanged(const PlotMSWatchedParameters& p,
        int updateFlag) {
    (void)updateFlag;
    if(&p == &itsPlotms_.getParameters()) {
        itsParams_ = dynamic_cast<const PlotMSParameters&>(p);

    } else {
        const vector<PlotMSPlotParameters*>& params =
            itsPlotms_.getPlotManager().plotParameters();
        unsigned int index = 0;
        for(; index < params.size(); index++) if(&p == params[index]) break;
        if(index >= itsPlotParams_.size()) return; // shouldn't happen
        itsPlotParams_[index] = *params[index];
    }
}




void PlotMSDBusApp::plotsChanged(const PlotMSPlotManager& manager) {
    const vector<PlotMSPlotParameters*>& p = manager.plotParameters();
    itsPlotParams_.resize(p.size(),
            PlotMSPlotParameters(itsPlotms_.getPlotter()->getFactory()));
    for(unsigned int i = 0; i < p.size(); i++) itsPlotParams_[i] = *p[i];
}




// Protected Methods //

void PlotMSDBusApp::dbusRunXmlMethod(
	const String& methodName, const Record& parameters, Record& retValue,
	const String& callerName, bool isAsync
) 
{
    // Common parameters: plot index.
    int index = -1;
    bool indexSet = parameters.isDefined(PARAM_PLOTINDEX) &&
                    (parameters.dataType(PARAM_PLOTINDEX) == TpInt ||
                     parameters.dataType(PARAM_PLOTINDEX) == TpUInt);
    if(indexSet) index = parameters.dataType(PARAM_PLOTINDEX) == TpUInt ?
                         (int)parameters.asuInt(PARAM_PLOTINDEX) :
                         parameters.asInt(PARAM_PLOTINDEX);
                         
    // Index valid for getter methods if 1) not async, 2) index parameter is
    // set, and 3) index is in bounds.
    bool indexValid= indexSet&& index>=0 && index<(int)itsPlotParams_.size() &&
                     !isAsync;
    
    // Common parameters: update immediately.
    bool updateImmediately = true;
    if(parameters.isDefined(PARAM_UPDATEIMMEDIATELY) &&
       parameters.dataType(PARAM_UPDATEIMMEDIATELY) == TpBool)
        updateImmediately = parameters.asBool(PARAM_UPDATEIMMEDIATELY);
    
    bool callError = false;
    
    if(methodName == METHOD_GETLOGPARAMS) {
        Record ret;
        ret.define(PARAM_FILENAME, itsPlotms_.getLogger()->sinkLocation());
        ret.define(PARAM_PRIORITY, LogMessage::toString(
                   itsPlotms_.getLogger()->filterMinPriority()));
        retValue.defineRecord(0, ret);
    }
    else if(methodName == METHOD_SETLOGPARAMS) {
        if(parameters.isDefined(PARAM_FILENAME) &&
           parameters.dataType(PARAM_FILENAME) == TpString)
            itsPlotms_.getLogger()->setSinkLocation(parameters.asString(
                                              PARAM_FILENAME));
        if(parameters.isDefined(PARAM_PRIORITY) &&
           parameters.dataType(PARAM_PRIORITY) == TpString) {
            String value = parameters.asString(PARAM_PRIORITY);
            bool found = false;
            LogMessage::Priority p = LogMessage::DEBUGGING;
            for(; p < LogMessage::SEVERE; p = LogMessage::Priority(p + 1)) {
                if(LogMessage::toString(p) == value) {
                    found = true;
                    break;
                }
            }
            if(found) itsPlotms_.getLogger()->setFilterMinPriority(p);
            else      callError = true;
        }
        
    } else if(methodName == METHOD_GETPLOTMSPARAMS) {
        Record ret;
        ret.define(PARAM_CLEARSELECTIONS,
                itsPlotms_.getParameters().clearSelectionsOnAxesChange());
        pair<int, int> ci = itsPlotms_.getParameters().cachedImageSize();
        ret.define(PARAM_WIDTH, ci.first);
        ret.define(PARAM_HEIGHT, ci.second);
        retValue.defineRecord(0, ret);
        
    } else if(methodName == METHOD_SETPLOTMSPARAMS) {
        if(parameters.isDefined(PARAM_CLEARSELECTIONS) &&
           parameters.dataType(PARAM_CLEARSELECTIONS) == TpBool)
            itsPlotms_.getParameters().setClearSelectionsOnAxesChange(
                    parameters.asBool(PARAM_CLEARSELECTIONS));
        pair<int, int> ci = itsPlotms_.getParameters().cachedImageSize();
        if(parameters.isDefined(PARAM_HEIGHT) &&
           (parameters.dataType(PARAM_HEIGHT) == TpInt ||
            parameters.dataType(PARAM_HEIGHT) == TpUInt))
            ci.second = parameters.dataType(PARAM_HEIGHT) == TpInt ?
                    parameters.asInt(PARAM_HEIGHT) :
                    parameters.asuInt(PARAM_HEIGHT);
        if(parameters.isDefined(PARAM_WIDTH) &&
           (parameters.dataType(PARAM_WIDTH) == TpInt ||
            parameters.dataType(PARAM_WIDTH) == TpUInt))
            ci.first = parameters.dataType(PARAM_WIDTH) == TpInt ?
                    parameters.asInt(PARAM_WIDTH) :
                    parameters.asuInt(PARAM_WIDTH);
        itsPlotms_.getParameters().setCachedImageSize(ci.first, ci.second);   
        
    } else if(methodName == METHOD_SETCACHEDIMAGESIZETOSCREENRES) {
        itsPlotms_.getParameters().setCachedImageSizeToResolution();
        
    } else if(methodName == METHOD_GETPLOTPARAMS) {
        if(indexValid) {
            const PlotMSPlotParameters& p = itsPlotParams_[index];
            const PMS_PP_MSData* d = p.typedGroup<PMS_PP_MSData>();
            const PMS_PP_Cache* c = p.typedGroup<PMS_PP_Cache>();
            const PMS_PP_Canvas* can = p.typedGroup<PMS_PP_Canvas>();            
            const PMS_PP_Display *disp = p.typedGroup<PMS_PP_Display>();
            
            Record ret;
            if(d != NULL) {
                ret.define(PARAM_FILENAME, d->filename());
                ret.defineRecord(PARAM_AVERAGING,
                        d->averaging().toRecord(true));
                ret.defineRecord(PARAM_SELECTION, d->selection().toRecord());
                ret.defineRecord(PARAM_TRANSFORMATIONS, d->transformations().toRecord());
            }
            
            if(c != NULL) {
                ret.define(PARAM_AXIS_X, PMS::axis(c->xAxis()));
                ret.define(PARAM_DATACOLUMN_X,
                        PMS::dataColumn(c->xDataColumn()));
                ret.define(PARAM_AXIS_Y, PMS::axis(c->yAxis()));
                ret.define(PARAM_DATACOLUMN_Y,
                        PMS::dataColumn(c->yDataColumn()));
            }
            
            if (disp!=NULL)  {
                ret.define(PARAM_COLORIZE, disp->colorizeFlag());
                PMS::Axis  ax = disp->colorizeAxis();
                ret.define(PARAM_COLORAXIS, PMS::Axis(ax));
            }

            if (can!=NULL)   {
                ret.define(PARAM_CANVASTITLE,  can->titleFormat().format);
                ret.define(PARAM_XAXISLABEL,  can->xLabelFormat().format);
                ret.define(PARAM_YAXISLABEL,  can->yLabelFormat().format);
                ret.define(PARAM_SHOWMAJORGRID,  can->gridMajorShown());
                ret.define(PARAM_SHOWMINORGRID,  can->gridMinorShown());
                PlotLinePtr majplp = can->gridMajorLine();
                PlotLinePtr minplp = can->gridMajorLine();
                ret.define(PARAM_MAJORWIDTH,   (int)majplp->width() );
                ret.define(PARAM_MAJORWIDTH,   (int)minplp->width() );
                //ret.define(PARAM_MAJORSTYLE,    string from Style enum - see casaqt/implement/QwtPlotter/QPOptions.h  /// (int)majplp->style() );
                // minor....
                //ret.define(PARAM_MAJORCOLOR,  ..... 
                // minor....
                
            }
            
            if(ret.nfields() != 0) retValue.defineRecord(0, ret);
        } else callError = true;
        
    } 
    else if (methodName == METHOD_SETPLOTPARAMS)  {
        bool resized = plotParameters(index);
        PlotMSPlotParameters& ppp = itsPlotParams_[index];

        PMS_PP_MSData* ppdata = ppp.typedGroup<PMS_PP_MSData>();
        if (ppdata == NULL) {
            ppp.setGroup<PMS_PP_MSData>();
            ppdata = ppp.typedGroup<PMS_PP_MSData>();
        }

        PMS_PP_Cache* ppcache = ppp.typedGroup<PMS_PP_Cache>();
        if (ppcache == NULL) {
            ppp.setGroup<PMS_PP_Cache>();
            ppcache = ppp.typedGroup<PMS_PP_Cache>();
        }

        PMS_PP_Display* ppdisp = ppp.typedGroup<PMS_PP_Display>();
        if (ppdisp == NULL) {
            ppp.setGroup<PMS_PP_Display>();
            ppdisp = ppp.typedGroup<PMS_PP_Display>();
        }

        PMS_PP_Canvas* ppcan = ppp.typedGroup<PMS_PP_Canvas>();
        if (ppcan == NULL) {
            ppp.setGroup<PMS_PP_Canvas>();
            ppcan = ppp.typedGroup<PMS_PP_Canvas>();
        }
        
        
        if(parameters.isDefined(PARAM_FILENAME) &&
           parameters.dataType(PARAM_FILENAME) == TpString)
            ppdata->setFilename(parameters.asString(PARAM_FILENAME));
        
        if(parameters.isDefined(PARAM_SELECTION) &&
           parameters.dataType(PARAM_SELECTION) == TpRecord) {
            PlotMSSelection sel = ppdata->selection();
            sel.fromRecord(parameters.asRecord(PARAM_SELECTION));
            ppdata->setSelection(sel);
        }
        
        if(parameters.isDefined(PARAM_AVERAGING) &&
           parameters.dataType(PARAM_AVERAGING) == TpRecord) {
            PlotMSAveraging avg = ppdata->averaging();
            avg.fromRecord(parameters.asRecord(PARAM_AVERAGING));
            ppdata->setAveraging(avg);
        }

        if(parameters.isDefined(PARAM_TRANSFORMATIONS) &&
           parameters.dataType(PARAM_TRANSFORMATIONS) == TpRecord) {
            PlotMSTransformations trans = ppdata->transformations();
            trans.fromRecord(parameters.asRecord(PARAM_TRANSFORMATIONS));
            ppdata->setTransformations(trans);
        }

        
        bool ok;
        PMS::Axis a;
        if(parameters.isDefined(PARAM_AXIS_X) &&
           parameters.dataType(PARAM_AXIS_X) == TpString) {
            a = PMS::axis(parameters.asString(PARAM_AXIS_X), &ok);
            if(ok) ppcache->setXAxis(a);
        }
        if(parameters.isDefined(PARAM_AXIS_Y) &&
           parameters.dataType(PARAM_AXIS_Y) == TpString) {
            a = PMS::axis(parameters.asString(PARAM_AXIS_Y), &ok);
            if(ok) ppcache->setYAxis(a);
        }
        
        PMS::DataColumn dc;
        if(parameters.isDefined(PARAM_DATACOLUMN_X) &&
           parameters.dataType(PARAM_DATACOLUMN_X) == TpString) {
            dc = PMS::dataColumn(parameters.asString(PARAM_DATACOLUMN_X), &ok);
            if(ok) ppcache->setXDataColumn(dc);
        }
        if(parameters.isDefined(PARAM_DATACOLUMN_Y) &&
           parameters.dataType(PARAM_DATACOLUMN_Y) == TpString) {
            dc = PMS::dataColumn(parameters.asString(PARAM_DATACOLUMN_Y), &ok);
            if(ok) ppcache->setYDataColumn(dc);
        }


        if(parameters.isDefined(PARAM_CANVASTITLE) &&
           parameters.dataType(PARAM_CANVASTITLE) == TpString)   {
            PlotMSLabelFormat f = ppcan->titleFormat();
            f.format =parameters.asString(PARAM_CANVASTITLE);
            ppcan->setTitleFormat(f);
        }

        if(parameters.isDefined(PARAM_XAXISLABEL) &&
           parameters.dataType(PARAM_XAXISLABEL) == TpString)   {
            PlotMSLabelFormat f = ppcan->xLabelFormat();
            f.format =parameters.asString(PARAM_XAXISLABEL);
            ppcan->setXLabelFormat(f);

        }

        if(parameters.isDefined(PARAM_YAXISLABEL) &&
           parameters.dataType(PARAM_YAXISLABEL) == TpString)   {
            PlotMSLabelFormat f = ppcan->yLabelFormat();
            f.format =parameters.asString(PARAM_YAXISLABEL);
            ppcan->setYLabelFormat(f);
        }


        if(parameters.isDefined(PARAM_COLORIZE) &&
           parameters.dataType(PARAM_COLORIZE) == TpBool)   {
            bool want = parameters.asBool(PARAM_COLORIZE);
            ppdisp->setColorize(want);
        }

        if(parameters.isDefined(PARAM_COLORAXIS) &&
           parameters.dataType(PARAM_COLORAXIS) == TpString)   {
            a = PMS::axis(parameters.asString(PARAM_COLORAXIS), &ok);
            if (ok)  ppdisp->setColorize(a);
        }

        
        if (parameters.isDefined(PARAM_SHOWMAJORGRID)  && 
           parameters.dataType(PARAM_SHOWMAJORGRID) == TpBool)   {
            bool want = parameters.asBool(PARAM_SHOWMAJORGRID);
            ppcan->showGridMajor(want);
        }
        
        if (parameters.isDefined(PARAM_SHOWMINORGRID)  && 
           parameters.dataType(PARAM_SHOWMINORGRID) == TpBool)   {
            bool want = parameters.asBool(PARAM_SHOWMINORGRID);
            ppcan->showGridMinor(want);
        }
        

        if (parameters.isDefined(PARAM_MAJORCOLOR)  && 
           parameters.dataType(PARAM_MAJORCOLOR) == TpString)   {
            String ccc = parameters.asString(PARAM_MAJORCOLOR);
            PlotLinePtr plp = ppcan->gridMajorLine() ;
            plp->setColor(ccc);
            ppcan->setGridMajorLine(plp);
        }
        
        if (parameters.isDefined(PARAM_MINORCOLOR)  && 
           parameters.dataType(PARAM_MINORCOLOR) == TpString)   {
            String ccc = parameters.asString(PARAM_MINORCOLOR);
            PlotLinePtr plp = ppcan->gridMinorLine() ;
            plp->setColor(ccc);
            ppcan->setGridMinorLine(plp);
        }
   
        if (parameters.isDefined(PARAM_MAJORSTYLE)  && 
           parameters.dataType(PARAM_MAJORSTYLE) == TpString)   {
            String s = parameters.asString(PARAM_MAJORSTYLE);
            PlotLinePtr plp = ppcan->gridMajorLine();
            PlotLine::Style  style = StyleFromString(s);
            plp->setStyle(style);
            ppcan->setGridMajorLine(plp);
        }
        
        if (parameters.isDefined(PARAM_MINORSTYLE)  && 
           parameters.dataType(PARAM_MINORSTYLE) == TpString)   {
            String s = parameters.asString(PARAM_MINORSTYLE);
            PlotLinePtr plp = ppcan->gridMinorLine();
            PlotLine::Style  style = StyleFromString(s);
            plp->setStyle(style);
            ppcan->setGridMinorLine(plp);
        }
        
        if (parameters.isDefined(PARAM_MAJORWIDTH)  && 
           parameters.dataType(PARAM_MAJORWIDTH) == TpInt)   {
            int w = parameters.asInt(PARAM_MAJORWIDTH);
            PlotLinePtr plp = ppcan->gridMajorLine();
            plp->setWidth(w);
            ppcan->setGridMajorLine(plp);
        }
        
        if (parameters.isDefined(PARAM_MINORWIDTH)  && 
           parameters.dataType(PARAM_MINORWIDTH) == TpInt)   {
            int w = parameters.asInt(PARAM_MINORWIDTH);
            PlotLinePtr plp = ppcan->gridMinorLine();
            plp->setWidth(w);
            ppcan->setGridMinorLine(plp);
        }
        
        
        
        
        if(updateImmediately && itsPlotms_.guiShown()) {
            if(resized) itsPlotms_.addSinglePlot(&ppp);
            else {
                PlotMSPlotParameters* sp =
                    itsPlotms_.getPlotManager().plotParameters(index);
                sp->holdNotification(this);
                *sp = ppp;
                sp->releaseNotification();
            }
        } else if(updateImmediately) itsUpdateFlag_ = true;
        
    } else if(methodName == METHOD_GETFLAGGING) {
        retValue.defineRecord(0, itsPlotms_.getPlotter()->getFlaggingTab()->
                                 getValue().toRecord(true));
        
    } else if(methodName == METHOD_SETFLAGGING) {
        PlotMSFlagging flag;
        flag.fromRecord(parameters);
        itsPlotms_.getPlotter()->getFlaggingTab()->setValue(flag);
        
    } else if(methodName == METHOD_SHOW || methodName == METHOD_HIDE) {
        itsPlotms_.showGUI(methodName == METHOD_SHOW);
        if(itsPlotms_.guiShown() && itsUpdateFlag_) {
        	update();
        }
    } else if(methodName == METHOD_UPDATE) {
        update();
    } else if(methodName == METHOD_QUIT) {
        PlotMSAction(PlotMSAction::QUIT).doAction(&itsPlotms_);
    }
    else if(methodName == METHOD_SAVE) {
    	update();
    	if (!_savePlot(parameters)) {
    		callError = true;
    	}
    }
    else if (methodName == METHOD_ISDRAWING) {
    	retValue.define(0, itsPlotms_.isDrawing());
    }
    else if (methodName == METHOD_ISCLOSED) {
    	retValue.define(0, itsPlotms_.isClosed());
    }
    else {
        log("Unknown method: " + methodName);
    }
    if(callError) log("Method " + methodName + " was called incorrectly.");
}




bool PlotMSDBusApp::_savePlot(const Record& parameters) {
	bool ok = true;
	String methodName = "_savePlot";
	PlotMSAction action(PlotMSAction::PLOT_EXPORT);
	String filename;
	if(parameters.isDefined(PARAM_EXPORT_FILENAME)) {
		filename = parameters.asString(PARAM_EXPORT_FILENAME);
		if (filename.empty()) {
			ok = false;
			log("Method " + methodName + ": file name not specified");
		}
	}
	else {
		ok = false;
		log("Method " + methodName + ": file name not defined ");
	}

	if (ok) {
		String format;
		PlotExportFormat::Type type;
		if (
			! parameters.isDefined(PARAM_EXPORT_FORMAT)
			|| (format = parameters.asString(PARAM_EXPORT_FORMAT)).empty()
		) {
			type = PlotExportFormat::typeForExtension(filename, &ok);
			if (!ok) {
				log(
					"Method " + methodName
					+ ": failed to save plot to file: unknown format from file name "
					+ filename
				);
			}
		}
		else {
			type = PlotExportFormat::exportFormat(format, &ok);
			if (! ok) {
				log(
					"Method " + methodName
					+ ": failed to save plot to file: unknown format " + format
				);
			}
		}
		if(ok) {
			PlotExportFormat format(type, filename);
			format.resolution = (
						parameters.isDefined(PARAM_EXPORT_HIGHRES)
						&& parameters.asBool(PARAM_EXPORT_HIGHRES)
					)
					? PlotExportFormat::HIGH : PlotExportFormat::SCREEN;
			bool interactive = ! (
				parameters.isDefined(PARAM_EXPORT_INTERACTIVE)
				&& ! parameters.asBool(PARAM_EXPORT_INTERACTIVE)
			);
			if (! (ok = itsPlotms_.save(format, interactive))) {
				log("Method " + methodName + ": failed to save plot to file ");
			}
		}
	}
	return ok;
}

void PlotMSDBusApp::dbusXmlReceived(const QtDBusXML& xml) {
    log("Received message:\n" + xml.toXMLString()); }


// Private Methods //

/*
bool plotms::exportPlot(const string& filename, const bool highResolution,
        const int dpi, const int width, const int height, const int plotIndex){
    //if(plotIndex < 0 || plotIndex >= (int)itsPlotParameters_.size() ||
    //   itsCurrentPlotMS_ == NULL) return false;

    //PlotMSAction action(PlotMSAction::PLOT_EXPORT);
    //action.setParameter(PlotMSAction::P_FILE, filename);
    //action.setParameter(PlotMSAction::P_HIGHRES, highResolution);
    //action.setParameter(PlotMSAction::P_DPI, dpi);
    //action.setParameter(PlotMSAction::P_WIDTH, width);
    //action.setParameter(PlotMSAction::P_HEIGHT, height);
    //action.setParameter(PlotMSAction::P_PLOT,
    //        itsCurrentPlotMS_->getPlotManager().plot(plotIndex));

    //return false;
}
*/


// Private Methods //

void PlotMSDBusApp::log(const String& m) {
    itsPlotms_.getLogger()->postMessage(PMS::LOG_ORIGIN, PMS::LOG_ORIGIN_DBUS,
            m, PMS::LOG_EVENT_DBUS);
}

bool PlotMSDBusApp::plotParameters(int& plotIndex) const {
    if(plotIndex < 0) plotIndex = 0;
    if((unsigned int)plotIndex > itsPlotParams_.size())
        plotIndex = itsPlotParams_.size();

    bool resized = false;
    if((unsigned int)plotIndex >= itsPlotParams_.size()) {
        resized = true;
        const_cast<PlotMSDBusApp*>(this)->itsPlotParams_.resize(plotIndex + 1,
                PlotMSPlotParameters(itsPlotms_.getPlotter()->getFactory()));
    }

    return resized;
}

void PlotMSDBusApp::update() {
	// single threaded here
    itsUpdateFlag_ = false;
    itsPlotms_.showGUI(true);
    unsigned int n = itsPlotms_.getPlotManager().plotParameters().size();
    
    // update plot parameters
    PlotMSPlotParameters* p;
    for(unsigned int i = 0; i < n; i++) {
        p = itsPlotms_.getPlotManager().plotParameters(i);
        if(p == NULL) continue;
        if(*p != itsPlotParams_[i]) {
            p->holdNotification(this);
            *p = itsPlotParams_[i];
            p->releaseNotification();
        }
    }
    
    // check for added plots
    if(itsPlotParams_.size() > n) {
        vector<PlotMSPlotParameters> v(itsPlotParams_.size() - n,
                PlotMSPlotParameters(itsPlotms_.getPlotter()->getFactory()));
        for(unsigned int i = 0; i < v.size(); i++)
            v[i] = itsPlotParams_[i + n];
        for(unsigned int i = 0; i < v.size(); i++)
            itsPlotms_.addSinglePlot(&v[i]);
    }
}

}
