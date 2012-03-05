//# Copyright (C) 2005
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


#include <casa/aips.h>
#include <casa/Arrays.h>
#include <measures/Measures.h>
#include <coordinates/Coordinates.h>
#include <casa/Exceptions/Error.h>
#include <images/Regions/WCBox.h>
#include <lattices/Lattices/LCRegion.h>
#include <lattices/Lattices/LCBox.h>
#include <lattices/Lattices/RegionType.h>
#include <tables/Tables/TableRecord.h>
#include <casa/BasicSL/String.h>
#include <casa/Utilities/Assert.h>

#include <display/QtPlotter/QtCanvas.qo.h>
#include <display/QtPlotter/QtProfile.qo.h>
#include <display/QtPlotter/QtProfilePrefs.qo.h>
#include <display/QtPlotter/QtMWCTools.qo.h>
#include <imageanalysis/ImageAnalysis/SpectralCollapser.h>
#include <imageanalysis/ImageAnalysis/SpectralFitter.h>
#include <images/Images/ImageAnalysis.h>
#include <images/Images/ImageUtilities.h>
#include <images/Images/PagedImage.h>
#include <images/Images/ImageFITSConverter.h>
#include <coordinates/Coordinates/QualityCoordinate.h>
#include <casa/Logging/LogFilter.h>
#include <casa/Logging/LogIO.h>
#include <casa/Logging/LogOrigin.h>
#include <images/Images/ImageUtilities.h>
#include <display/DisplayEvents/MWCPTRegion.h>
#include <display/Display/Options.h>

#include <graphics/X11/X_enter.h>
#include <QDir>
#include <QColor>
#include <QHash>
#include <QWidget>
#include <QMainWindow>
#include <QMouseEvent>
#include <cmath>
#include <QtGui>
#include <QComboBox>
#include <iostream>
#include <graphics/X11/X_exit.h>
#include <QMessageBox>

//#include <xmlcasa/version.h>

namespace casa { 

QtProfile::~QtProfile()
{
}

QtProfile::QtProfile(ImageInterface<Float>* img, const char *name, QWidget *parent, std::string rcstr)
        :QMainWindow(parent),
         //pc(0),
         //te(0),
         analysis(0), image(img), collapser(0), fitter(0), over(0),
         coordinate("world"), coordinateType(""),xaxisUnit(""),ctypeUnit(""),
         cSysRval(""), fileName(name), position(""), yUnit(""),
         yUnitPrefix(""), xpos(""), ypos(""), cube(0),
         npoints(0), npoints_old(0), stateMProf(2), stateRel(0),
         lastPX(Vector<Double>()), lastPY(Vector<Double>()),
         lastWX(Vector<Double>()), lastWY(Vector<Double>()),
         z_xval(Vector<Float>()), z_yval(Vector<Float>()),
         z_eval(Vector<Float>()), region(""), rc(viewer::getrc()), rcid_(rcstr),
         itsPlotType(QtProfile::PMEAN), itsLog(new LogIO()), ordersOfM_(0)
{
    setupUi(this);
    initPlotterResource();

    setWindowTitle(QString("Spectral Profile - ").append(name));
    setBackgroundRole(QPalette::Dark);

    fillPlotTypes();
    connect(plotMode, SIGNAL(currentIndexChanged(const QString &)),
            this, SLOT(changePlotType(const QString &)));
    connect(errorMode, SIGNAL(currentIndexChanged(const QString &)),
            this, SLOT(changeErrorType(const QString &)));

    connect(collapseType, SIGNAL(currentIndexChanged(const QString &)),
            this, SLOT(changeCollapseType(const QString &)));
    connect(collapseError, SIGNAL(currentIndexChanged(const QString &)),
            this, SLOT(changeCollapseError(const QString &)));
    //changeCollapseType(collapseType->currentText());
    //changeCollapseError(collapseError->currentText());
    changeCollapseType();
    changeCollapseError();

    QPalette pal = pixelCanvas->palette();
    pal.setColor(QPalette::Background, Qt::white);
    pixelCanvas->setPalette(pal);

    // read the preferred ctype from casarc
    QString pref_ctype = QString(rc.get("viewer." + rcid() + ".freqcoord.type").c_str());
    if (pref_ctype.size()>0){
      // change to the preferred ctype
    	int ctypeindex= ctype->findText(pref_ctype);
    	if (ctypeindex > -1)
    		ctype->setCurrentIndex(ctypeindex);
    }

    ctypeUnit = String(ctype->currentText().toStdString());
    getcoordTypeUnit(ctypeUnit, coordinateType, xaxisUnit);
    collapseUnits->setText(QString("<font color='black'>[")+QString(xaxisUnit.c_str())+QString("]</font>"));
    fitUnits->setText(QString("<font color='black'>[")+QString(xaxisUnit.c_str())+QString("]</font>"));

    // get reference frame info for freq axis label
    MFrequency::Types freqtype = determineRefFrame(img);
    spcRefFrame = String(MFrequency::showType(freqtype));
    Int frameindex=spcRef->findText(QString(spcRefFrame.c_str()));
    spcRef->setCurrentIndex(frameindex);

    connect(ctype, SIGNAL(currentIndexChanged(const QString &)),
            this, SLOT(changeCoordinateType(const QString &)));
    connect(spcRef, SIGNAL(currentIndexChanged(const QString &)),
            this, SLOT(changeFrame(const QString &)));

    QSettings settings("CASA", "Viewer");
    QString pName = settings.value("Print/printer").toString();

    connect(pixelCanvas, SIGNAL(xRangeChanged(float, float)), this, SLOT(setCollapseRange(float, float)));

    QValidator *validator = new QDoubleValidator(-1.0e-32, 1.0e+32,10,this);
    startValue->setValidator(validator);
    startValue->setMaximumWidth(100);
    endValue->setValidator(validator);
    endValue->setMaximumWidth(100);
    fromLabel->setMinimumWidth(40);
    fromLabel->setMargin(3);
    fromLabel->setAlignment((Qt::Alignment)(Qt::AlignRight|Qt::AlignVCenter));
    toLabel->setMinimumWidth(30);
    toLabel->setMargin(3);
    toLabel->setAlignment((Qt::Alignment)(Qt::AlignRight|Qt::AlignVCenter));
    //collapseUnits->setMinimumWidth(45);
    collapseUnits->setMargin(3);
    fitUnits->setMargin(3);
    connect(collapse, SIGNAL(clicked()),
            this, SLOT(doImgCollapse()));

    startValueFit->setValidator(validator);
    startValueFit->setMaximumWidth(100);
    endValueFit->setValidator(validator);
    endValueFit->setMaximumWidth(100);
    connect(fit, SIGNAL(clicked()),
            this, SLOT(doLineFit()));
    connect(clean, SIGNAL(clicked()),
            this, SLOT(plotMainCurve()));


    pixelCanvas->setTitle("");
    pixelCanvas->setWelcome("assign a mouse button to\n"
                   "'crosshair' or 'rectangle' or 'polygon'\n"
                   "click/press+drag the assigned button on\n"
                   "the image to get a spectral profile");

    QString lbl = ctype->currentText();
    pixelCanvas->setXLabel(lbl, 12, 2, "Helvetica [Cronyx]");

    yUnit = QString(img->units().getName().chars());
    pixelCanvas->setYLabel("("+yUnitPrefix+yUnit+")", 12, 2, "Helvetica [Cronyx]");

    pixelCanvas->setAutoScaleX(true);
    pixelCanvas->setAutoScaleY(true);

    connect(actionZoomIn, SIGNAL(triggered()), this, SLOT(zoomIn()));
    connect(actionZoomOut, SIGNAL(triggered()), this, SLOT(zoomOut()));
    connect(actionZoomNeutral, SIGNAL(triggered()), this, SLOT(zoomNeutral()));
    connect(actionPrint, SIGNAL(triggered()), this, SLOT(print()));
    connect(actionSaveGraphic, SIGNAL(triggered()), this, SLOT(saveGraphic()));
    connect(actionExport, SIGNAL(triggered()), this, SLOT(exportProfile()));
    connect(actionMoveLeft, SIGNAL(triggered()), this, SLOT(left()));
    connect(actionMoveRight, SIGNAL(triggered()), this, SLOT(right()));
    connect(actionMoveUp, SIGNAL(triggered()), this, SLOT(up()));
    connect(actionMoveDown, SIGNAL(triggered()), this, SLOT(down()));
    connect(actionPreferences, SIGNAL(triggered()), this, SLOT(preferences()));

    try{
   	 analysis  = new ImageAnalysis(img);
   	 collapser = new SpectralCollapser(img, String(viewer::options.temporaryPath( )));
   	 //Float min, max;
   	 //collapser->getSpecMinMax(min, max);
   	 fitter    = new SpectralFitter();
    }
    catch (AipsError x){
   	 String message = "Error when starting the profiler:\n" + x.getMesg();
   	 *itsLog << LogIO::WARN << message << LogIO::POST;
    }
}

MFrequency::Types QtProfile::determineRefFrame(ImageInterface<Float>* img, bool check_native_frame )
{
  MFrequency::Types freqtype;

  CoordinateSystem cSys=img->coordinates();
  Int specAx=cSys.findCoordinate(Coordinate::SPECTRAL);

  if ( specAx < 0 ) {
    QMessageBox::information( this, "No spectral axis...",
			      "Sorry, could not find a spectral axis for this image...",
			      QMessageBox::Ok);
    return MFrequency::DEFAULT;
  }

  SpectralCoordinate specCoor=cSys.spectralCoordinate(specAx);
  MFrequency::Types tfreqtype;
  MEpoch tepoch;
  MPosition tposition;
  MDirection tdirection;
  specCoor.getReferenceConversion(tfreqtype, tepoch, tposition, tdirection);
  freqtype = specCoor.frequencySystem(False); // false means: get the native type

  if( check_native_frame && tfreqtype != freqtype ){ // there is an active conversion layer
	  // ask user if he/she wants to change to native frame
	  String title = "Change display reference frame?";
	  String message = "Native reference frame is " + MFrequency::showType(freqtype)
	  + ",\n display frame is " + MFrequency::showType(tfreqtype) + ".\n"
	  + "Change display frame permanently to " + MFrequency::showType(freqtype) + "?\n"
	  + "(Needs write access to image.)";
	  if(QMessageBox::question(this, title.c_str(), message.c_str(),
			  QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes){
		  // user wants to change
		  try {
			  // set the reference conversion to the native type, effectively switching it off
			  if(!specCoor.setReferenceConversion(freqtype, tepoch, tposition, tdirection)
					  || !cSys.replaceCoordinate(specCoor, specAx)
					  || !img->setCoordinateInfo(cSys)){

				  img->coordinates().spectralCoordinate(specAx).getReferenceConversion(tfreqtype, tepoch, tposition, tdirection);
				  title = "Failure";
				  message = "casaviewer: Error setting reference frame conversion to native frame ("
						  + MFrequency::showType(freqtype) + ")\nWill use " + MFrequency::showType(tfreqtype) + " instead";
				  QMessageBox::warning(this, title.c_str(), message.c_str(),
						  QMessageBox::Ok, QMessageBox::NoButton);
				  freqtype = tfreqtype;
			  }
		  } catch (AipsError x) {
			  title = "Failure";
			  message = "Error when trying to change display reference frame:\n" + x.getMesg();
			  QMessageBox::warning(this, title.c_str(), message.c_str(),
					  QMessageBox::Ok, QMessageBox::NoButton);
			  freqtype = tfreqtype;
		  }
	  }
	  else{ // user does not want to change
		  freqtype = tfreqtype;
	  }
  } // end if there is a conv layer

  return freqtype;

}
void QtProfile::zoomOut()
{
    pixelCanvas->zoomOut();
}

void QtProfile::zoomIn()
{
   pixelCanvas->zoomIn();
}

void QtProfile::zoomNeutral()
{
   pixelCanvas->zoomNeutral();
}

void QtProfile::print()
{
#ifndef QT_NO_PRINTER
    QPrinter printer(QPrinter::ScreenResolution);
    //printer.setFullPage(true);
    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    if (dlg->exec() == QDialog::Accepted) {
       QSettings settings("CASA", "Viewer");
       settings.setValue("Print/printer", printer.printerName());
       printIt(&printer);
    }
    delete dlg;
#endif
}
void QtProfile::printExp()
{
#ifndef QT_NO_PRINTER
    QPrinter printer(QPrinter::ScreenResolution);
    QSettings settings("CASA", "Viewer");
    printer.setPrinterName(settings.value("Print/printer").toString());
    printIt(&printer);
#endif
}

void QtProfile::saveGraphic()
{
    QString dflt = fileName + position + ".png";

    QString fn = QFileDialog::getSaveFileName(this,
       tr("Save as..."), QString(dflt), tr("(*.png);;(*.pdf);;(*.xpm);;(*.jpg);;(*.ppm);;(*.jpeg)"));

    if (fn.isEmpty())
        return ;

    QString ext = fn.section('.', -1);
    if (ext == "xpm" || ext == "jpg" || ext == "png" ||
        ext == "xbm" || ext == "ppm" || ext == "jpeg")
        ;
    else
        fn.append(".png");

    QFile file(fn);
    if (!file.open(QFile::WriteOnly))
        return ;

    pixelCanvas->graph()->save(fn);
    return ;
}
void QtProfile::saveExp()
{
    //qDebug() << "fileName:" << fileName;
    QFile file(fileName.append(position).append(".png"));
    if (!file.open(QFile::WriteOnly))
        return ;
    //qDebug() << "open ok";

    pixelCanvas->graph()->save(fileName, "PNG");
    //qDebug() << "save ok";
    return ;
}

void QtProfile::exportProfile()
{
    QString fn = QFileDialog::getSaveFileName(this,
       tr("Export profile"),
       QString(), tr( "FITS files (*.fits);; Text files (*.txt, *.plt)"));

    if (fn.isEmpty())
        return ;

    QString ext = fn.section('.', -1);
    bool ok;
    if (ext =="fits")
   	 ok = exportFITSSpectrum(fn);
    else {
    if (ext != "txt" && ext != "plt")
        fn.append(".txt");
		 ok = exportASCIISpectrum(fn);
    }
}

void QtProfile::left()
{
   QApplication::sendEvent(pixelCanvas,
       new QKeyEvent(QEvent::KeyPress, Qt::Key_Left, 0, 0));
}

void QtProfile::right()
{
   QApplication::sendEvent(pixelCanvas,
       new QKeyEvent(QEvent::KeyPress, Qt::Key_Right, 0, 0));
}

void QtProfile::up()
{
   QApplication::sendEvent(pixelCanvas,
       new QKeyEvent(QEvent::KeyPress, Qt::Key_Up, 0, 0));
}

void QtProfile::down()
{
   QApplication::sendEvent(pixelCanvas,
       new QKeyEvent(QEvent::KeyPress, Qt::Key_Down, 0, 0));
}

void QtProfile::preferences()
{
	QtProfilePrefs	*profilePrefs = new QtProfilePrefs(this,pixelCanvas->getAutoScaleX(), pixelCanvas->getAutoScaleY(), pixelCanvas->getShowGrid(),stateMProf, stateRel);
	connect(profilePrefs, SIGNAL(currentPrefs(int, int, int, int, int)),
			this, SLOT(setPreferences(int, int, int, int, int)));
	profilePrefs->showNormal();
}

void QtProfile::setPreferences(int inAutoX, int inAutoY, int showGrid, int inMProf, int inRel){
	bool update=false;
	if ((lastPX.nelements() > 0) && ((inMProf!=stateMProf) || (inRel!=stateRel)))
		update=true;
	pixelCanvas->setAutoScaleX(inAutoX);
	pixelCanvas->setAutoScaleY(inAutoY);
	pixelCanvas->setShowGrid(showGrid);
	stateMProf=inMProf;
	stateRel  = inRel;
	if (update)
		wcChanged(coordinate, lastPX, lastPY, lastWX, lastWY, UNKNPROF);
}

void QtProfile::setPlotError(int st)
{
	pixelCanvas->setPlotError(st);
}

void QtProfile::changeCoordinate(const QString &text) {

  coordinate = String(text.toStdString());

  //Double x1, y1;
  //getProfileRange(x1, y1, coordinate.chars());
  //qDebug() << "coordinate:" << QString(coordinate.chars())
  //           << "profile Range:" << x1 << y1;

  emit coordinateChange(coordinate);
}


void QtProfile::changeFrame(const QString &text) {
	//cout << "In change frame with input: " << text.toStdString() <<" coordinateType: " << coordinateType.c_str()<< endl;
	spcRefFrame=String(text.toStdString());
	//changeCoordinateType(QString(coordinateType.c_str()));
	changeCoordinateType(QString(ctypeUnit.c_str()));
}

void QtProfile::changeCoordinateType(const QString &text) {
    //coordinateType = String(text.toStdString());
    //qDebug() << "coordinate:" << text;
    //Double x1, y1;
    //getProfileRange(x1, y1, coordinate.chars());
    //qDebug() << "coordinate:" << QString(coordinate.chars())
    //           << "profile Range:" << x1 << y1;
    xpos = "";
    ypos = "";
    position = QString("");
    profileStatus->showMessage(position);
    pixelCanvas->clearCurve();


    ctypeUnit = String(text.toStdString());
    getcoordTypeUnit(ctypeUnit, coordinateType, xaxisUnit);
    collapseUnits->setText(QString("<font color='black'>[")+QString(xaxisUnit.c_str())+QString("]</font>"));
    fitUnits->setText(QString("<font color='black'>[")+QString(xaxisUnit.c_str())+QString("]</font>"));

    QString lbl = text;
    pixelCanvas->setXLabel(lbl, 12, 2, "Helvetica [Cronyx]");

    pixelCanvas->setPlotSettings(QtPlotSettings());

    //cout << "put to rc.viewer: " << text.toStdString() << endl;
    rc.put( "viewer." + rcid() + ".freqcoord.type", text.toStdString());

    if(lastPX.nelements() > 0){ // update display with new coord type
   	 wcChanged(coordinate, lastPX, lastPY, lastWX, lastWY, UNKNPROF);
    }

}

void QtProfile::closeEvent (QCloseEvent *) {
   //qDebug() << "closeEvent";
  lastPX.resize(0);
  lastPY.resize(0);
  lastWX.resize(0);
  lastWY.resize(0);
  z_xval.resize(0);
  z_yval.resize(0);
  z_eval.resize(0);
  emit hideProfile();
}

void QtProfile::resetProfile(ImageInterface<Float>* img, const char *name)
{
	image = img;

	try {
		if (analysis)
			delete analysis;
		analysis = new ImageAnalysis(img);

		if (collapser)
			delete collapser;
		collapser = new SpectralCollapser(img, String(QDir::tempPath().toStdString()));

		if (fitter)
			delete fitter;
		fitter = new SpectralFitter();
	}
	catch (AipsError x){
		String message = "Error when re-setting the profiler:\n" + x.getMesg();
		*itsLog << LogIO::WARN << message << LogIO::POST;
	}

	fileName = name;
	setWindowTitle(QString("Spectral Profile - ").append(name));

	// re-set the images
	// that are overplotted
	if (over) {
		delete over;
		over = 0;
		over = new QHash<QString, ImageAnalysis*>();
	}

	// adjust the error box
	fillPlotTypes();

	// adjust the collapse type
	changeCollapseType();

	// read the preferred ctype from casarc
	QString pref_ctype = QString(rc.get("viewer." + rcid() + ".freqcoord.type").c_str());
	if (pref_ctype.size()>0){
		// change to the preferred ctype
		int ctypeindex= ctype->findText(pref_ctype);
		if (ctypeindex > -1)
			ctype->setCurrentIndex(ctypeindex);
	}

	ctypeUnit = String(ctype->currentText().toStdString());
	getcoordTypeUnit(ctypeUnit, coordinateType, xaxisUnit);
	collapseUnits->setText(QString("<font color='black'>[")+QString(xaxisUnit.c_str())+QString("]</font>"));
	fitUnits->setText(QString("<font color='black'>[")+QString(xaxisUnit.c_str())+QString("]</font>"));

	QString lbl = ctype->currentText();
	pixelCanvas->setXLabel(lbl, 12, 2, "Helvetica [Cronyx]");


	// get reference frame info for frequency axis label
	MFrequency::Types freqtype = determineRefFrame(img);
	spcRefFrame = String(MFrequency::showType(freqtype));
	Int frameindex=spcRef->findText(QString(spcRefFrame.c_str()));
	spcRef->setCurrentIndex(frameindex);

	yUnit = QString(img->units().getName().chars());
	yUnitPrefix = "";
	pixelCanvas->setYLabel("("+yUnitPrefix+yUnit+")", 12, 2, "Helvetica [Cronyx]");

	xpos = "";
	ypos = "";
	lastPX.resize(0);
	lastPY.resize(0);
	lastWX.resize(0);
	lastWY.resize(0);
	position = QString("");
	profileStatus->showMessage(position);
	pixelCanvas->clearCurve();
}

void QtProfile::wcChanged( const String c,
			   const Vector<Double> px, const Vector<Double> py,
			   const Vector<Double> wx, const Vector<Double> wy,
			   const ProfileType ptype)
{
	if (!isVisible()) return;
    if (!analysis) return;
    *itsLog << LogOrigin("QtProfile", "wcChanged");

    //cout << "px: " << px << " py: " << py << endl;
    //cout << "wx: " << wx << " wy: " << wy << endl;

    if (cube == 0) {
       pixelCanvas->setWelcome("No profile available "
                   "for the given data \nor\n"
                   "No profile available for the "
                   "display axes orientation");
       pixelCanvas->clearCurve();
      return;
    }

    last_event_cs.resize(c.size());
    last_event_cs = c;
    last_event_px.resize(px.size());
    last_event_px = px;
    last_event_py.resize(py.size());
    last_event_py = py;
    last_event_wx.resize(wx.size());
    last_event_wx = wx;
    last_event_wy.resize(wy.size());
    last_event_wy = wy;

	 npoints = wx.size();
    if (npoints_old==0 && wx.size()>0){
   	 npoints_old = wx.size();
		 changePlotType(plotMode->currentText());
    }
    else{
   	 if (npoints==1 & npoints_old!=1){
   		 npoints_old = npoints;
   		 changePlotType(plotMode->currentText());
   	 }
   	 else if (npoints!=1 & npoints_old==1){
     		 npoints_old = npoints;
   		 changePlotType(plotMode->currentText());
   	 }
    }

    if (c != coordinate) {
   	 coordinate = c.chars();
   	 //if (coordinate == "world")
   	//	 chk->setCurrentIndex(0);
   	// else
   	//	 chk->setCurrentIndex(1);
    }

    Int ns;
    px.shape(ns);

    Vector<Double> pxv(ns);
    Vector<Double> pyv(ns);
    Vector<Double> wxv(ns);
    Vector<Double> wyv(ns);

    if (cube == -1){
   	 pxv.assign(py);
   	 pyv.assign(px);
   	 wxv.assign(wy);
   	 wyv.assign(wx);
    }
    else{
   	 pxv.assign(px);
   	 pyv.assign(py);
   	 wxv.assign(wx);
   	 wyv.assign(wy);
    }

    if (ns < 1) return;

    switch (ptype)
    {
       case SINGPROF:
    	   pixelCanvas->setTitle("Single Point Profile");
    	   region = "Point";
    	   break;
       case RECTPROF:
    	   pixelCanvas->setTitle("Rectangle Region Profile");
           region = "Rect";
           break;
       case ELLPROF:
    	   pixelCanvas->setTitle("Elliptical Region Profile");
           region = "Ellipse";
           break;
       case POLYPROF:
           pixelCanvas->setTitle("Polygon Region Profile");
           region = "Poly";
           break;
       case UNKNPROF:
    	   break;
       default:
           pixelCanvas->setTitle("");
           region = "";
    }
    pixelCanvas->setWelcome("");

	 Double xmean, ymean, minval, maxval;
    if (coordinate == "world") {
   	 if (wxv.size()==1){
          xmean =  wxv[0];
          ymean =  wyv[0];
   	 }
   	 else if (wxv.size()==2){
          xmean =  0.5*(wxv[0]+wxv[1]);
          ymean =  0.5*(wyv[0]+wyv[1]);
   	 }
   	 else {
          minMax(minval, maxval, wxv);
          xmean =  0.5*(minval + maxval);
          minMax(minval, maxval, wyv);
          ymean =  0.5*(minval + maxval);
   	 }
   	 //xpos, ypos and position only used for display
       xpos = QString::number(floor(xmean+0.5));
       ypos = QString::number(floor(ymean+0.5));

       position = getRaDec(xmean, ymean);
    }
    else {
   	 if (pxv.size()==1){
   		 xmean = pxv[0];
   		 ymean = pyv[0];
   	 }
   	 else if (pxv.size()==1){
   		 xmean = 0.5*(pxv[0]+pxv[1]);
   		 ymean = 0.5*(pyv[0]+pyv[0]);
   	 }
   	 else{
          minMax(minval, maxval, pxv);
          xmean =  0.5*(minval + maxval);
          minMax(minval, maxval, pyv);
          ymean =  0.5*(minval + maxval);
   	 }

       //xpos, ypos and position only used for display
       xpos = QString::number(floor(xmean+0.5));
       ypos = QString::number(floor(ymean+0.5));
       position = QString("[%1, %2]").arg(xpos).arg(ypos);
    }
    profileStatus->showMessage(position);

    //Get Profile Flux density v/s coordinateType
    Bool ok = False;
    switch (itsPlotType)
    {
    case QtProfile::PMEAN:
   	 ok=analysis->getFreqProfile( wxv, wyv, z_xval, z_yval,
   			 "world", coordinateType, 0, 0, 0, xaxisUnit, spcRefFrame,
   			 (Int)QtProfile::MEAN, 0, cSysRval);
   	 break;
    case QtProfile::PMEDIAN:
   	 ok=analysis->getFreqProfile( wxv, wyv, z_xval, z_yval,
   			 "world", coordinateType, 0, 0, 0, xaxisUnit, spcRefFrame,
   			 (Int)QtProfile::MEDIAN, 0, cSysRval);
   	 break;
    case QtProfile::PSUM:
		 ok=analysis->getFreqProfile( wxv, wyv, z_xval, z_yval,
				 "world", coordinateType, 0, 0, 0, xaxisUnit, spcRefFrame,
				 (Int)QtProfile::SUM, 0, cSysRval);
   	 break;
    case QtProfile::PFLUX:
		 ok=analysis->getFreqProfile( wxv, wyv, z_xval, z_yval,
				 "world", coordinateType, 0, 0, 0, xaxisUnit, spcRefFrame,
				 (Int)QtProfile::FLUX, 0, cSysRval);
   	 break;
    //case QtProfile::PVRMSE:
	//	 ok=analysis->getFreqProfile( wxv, wyv, z_xval, z_yval,
	//			 "world", coordinateType, 0, 0, 0, xaxisUnit, spcRefFrame,
	//			 (Int)QtProfile::RMSE, 0);
   //	 break;
    default:
   	 ok=analysis->getFreqProfile( wxv, wyv, z_xval, z_yval,
   			 "world", coordinateType, 0, 0, 0, xaxisUnit, spcRefFrame,
   			 (Int)QtProfile::MEAN, 0, cSysRval);
   	 break;
    }
    if (!ok) {
   	 // change to notify user of error...
    	*itsLog << LogIO::WARN << "Can not generate the frequency profile!" << LogIO::POST;
    	return;
    }

    // get the coordinate system
    CoordinateSystem cSys = image->coordinates();
    switch (itsErrorType)
    {
    case QtProfile::PNOERROR:
   	 if (z_eval.size()> 0)
   		 z_eval.resize(0);
   	 break;
    case QtProfile::PERMSE:
   	 if (wxv.size()<2){
   		 *itsLog << LogIO::NORMAL << "Can not do the plot request, only one point!" << LogIO::POST;
      	 if (z_eval.size()> 0)
      		 z_eval.resize(0);
   	 }
   	 else {
   		 switch (itsPlotType)
   		 {
   		 case QtProfile::PSUM:
   	   		 *itsLog << LogIO::NORMAL << "Plotting RMSE as error of SUM makes no sense!" << LogIO::POST;
   	      	 if (z_eval.size()> 0)
   	      		 z_eval.resize(0);
   	   		 break;
   		 //case QtProfile::PVRMSE:
   	   //		 cout << "Plotting RMSE over RMSE makes no sense!" << endl;
   	    //  	 if (z_eval.size()> 0)
   	    //  		 z_eval.resize(0);
   	   //		 break;
   		 default:
	   		 ok=analysis->getFreqProfile( wxv, wyv, z_xval, z_eval,
	   				 "world", coordinateType, 0, 0, 0, xaxisUnit, spcRefFrame,
	   				 (Int)QtProfile::RMSE, 0, cSysRval);
	   		 break;
   		 }
   	 }
		 break;
    case QtProfile::PPROPAG:
		if (cSys.qualityAxisNumber() <0){
			*itsLog << LogIO::NORMAL << "Can not do the plot request, no quality axis!" << LogIO::POST;
			if (z_eval.size()> 0)
				z_eval.resize(0);
		}
		else {
			switch (itsPlotType)
			{
			case QtProfile::PMEAN:
				ok=analysis->getFreqProfile( wxv, wyv, z_xval, z_eval,
						"world", coordinateType, 0, 0, 0, xaxisUnit, spcRefFrame,
						(Int)QtProfile::NSQRTSUM, 1, cSysRval);
				break;
			case QtProfile::PMEDIAN:
				*itsLog << LogIO::NORMAL << "Can not plot the error, NO propagation for median!" << LogIO::POST;
				if (z_eval.size()> 0)
					z_eval.resize(0);
				break;
			case QtProfile::PSUM:
				ok=analysis->getFreqProfile( wxv, wyv, z_xval, z_eval,
						"world", coordinateType, 0, 0, 0, xaxisUnit, spcRefFrame,
						(Int)QtProfile::SQRTSUM, 1, cSysRval);
				break;
			case QtProfile::PFLUX:
				ok=analysis->getFreqProfile( wxv, wyv, z_xval, z_eval,
						"world", coordinateType, 0, 0, 0, xaxisUnit, spcRefFrame,
						(Int)QtProfile::EFLUX, 1, cSysRval);
				break;
		    default:
		    	if (z_eval.size()> 0)
		    		z_eval.resize(0);
		    	break;
			}
		}
		break;
    default:
   	 if (z_eval.size()> 0)
   		 z_eval.resize(0);
   	 break;
    }
    //cout << "Lambda values: " << z_xval << endl;
    //cout << "Y values: " << z_yval << endl;

    if ( ! ok ) {
   	 // change to notify user of error...
    	*itsLog << LogIO::WARN << "Can not generate the frequency error profile!" << LogIO::POST;
    	return;
    }

    //cout << "x-values: " << z_xval << endl;
    //cout << "y-values: " << z_yval << endl;
    //cout << "e-values: " << z_eval << endl << endl;

    // scale for better display
    // max absolute display numbers should be between 0.1 and 100.0
    Double dmin = 0.1;
    Double dmax = 100.;
    Double ymin = min(z_yval);
    Double ymax = max(z_yval);
    ymax = max(abs(ymin), ymax);
    Double symax;
    Int ordersOfM = 0;

    symax = ymax;
    while(symax < dmin && ymax != 0.0){
   	 ordersOfM += 3;
   	 symax = ymax * pow(10.,ordersOfM);
    }
    while(symax > dmax && ymax != 0.0){
   	 ordersOfM -= 3;
   	 symax = ymax * pow(10.,ordersOfM);
    }

    if(ordersOfM!=0){
   	 // correct display y axis values
   	 for (uInt i = 0; i < z_yval.size(); i++) {
   		 z_yval(i) *= pow(10.,ordersOfM);
   	 }
   	 if (z_eval.size() > 0){
   		 for (uInt i = 0; i < z_eval.size(); i++) {
   			 z_eval(i) *= pow(10.,ordersOfM);
   		 }
   	 }
    }

    // store the scaling factor
    ordersOfM_=ordersOfM;

    if(ordersOfM!=0){
   	 // correct unit string
   	 if(yUnit.startsWith("(")||yUnit.startsWith("[")||
   			 yUnit.startsWith("\"")){
   		 // express factor as number
   		 ostringstream oss;
   		 oss << -ordersOfM;
   		 yUnitPrefix = "10E"+QString(oss.str().c_str())+" ";
   	 }
   	 else{
   		 // express factor as character
   		 switch(-ordersOfM){ // note the sign!
   		 case -9:
   			 yUnitPrefix = "p";
   			 break;
   		 case -6:
   			 yUnitPrefix = "u";
   			 break;
   		 case -3:
   			 yUnitPrefix = "m";
   			 break;
   		 case 3:
   			 yUnitPrefix = "k";
   			 break;
   		 case 6:
   			 yUnitPrefix = "M";
   			 break;
   		 case 9:
   			 yUnitPrefix = "M";
   			 break;
   		 default:
   			 ostringstream oss;
   			 oss << -ordersOfM;
   			 yUnitPrefix = "10E"+QString(oss.str().c_str())+" ";
   			 break;
   		 }
   	 }
    }
    else{ // no correction
   	 yUnitPrefix = "";
    }

    // remove the "/beam" in case of plotting flux
    if(itsPlotType==QtProfile::PFLUX){
      Int pos = yUnit.indexOf("/beam",0,Qt::CaseInsensitive);
      if(pos>-1){
	yUnit.remove(pos,5);
      }
    }

    pixelCanvas->setYLabel("("+yUnitPrefix+yUnit+")", 12, 2, "Helvetica [Cronyx]");

    // plot the graph

    pixelCanvas->clearData();
    pixelCanvas->plotPolyLine(z_xval, z_yval, z_eval, fileName);

    QHashIterator<QString, ImageAnalysis*> i(*over);
    while (i.hasNext() && stateMProf) {
   	 i.next();
   	 QString ky = i.key();
   	 ImageAnalysis* ana = i.value();
   	 Vector<Float> xval(100);
   	 Vector<Float> yval(100);

   	 switch (itsPlotType)
   	 {
   	 case QtProfile::PMEAN:
   		 ok=ana->getFreqProfile( wxv, wyv, xval, yval,
   				 "world", coordinateType, 0, 0, 0, xaxisUnit, spcRefFrame,
   				 (Int)QtProfile::MEAN, 0);
   		 break;
   	 case QtProfile::PMEDIAN:
   		 ok=ana->getFreqProfile( wxv, wyv, xval, yval,
   				 "world", coordinateType, 0, 0, 0, xaxisUnit, spcRefFrame,
   				 (Int)QtProfile::MEDIAN, 0);
   		 break;
   	 case QtProfile::PSUM:
   		 ok=ana->getFreqProfile( wxv, wyv, xval, yval,
   				 "world", coordinateType, 0, 0, 0, xaxisUnit, spcRefFrame,
   				 (Int)QtProfile::PSUM, 0);
   		 break;
   	 case QtProfile::PFLUX:
   		 ok=ana->getFreqProfile( wxv, wyv, xval, yval,
   				 "world", coordinateType, 0, 0, 0, xaxisUnit, spcRefFrame,
   				 (Int)QtProfile::PFLUX, 0);
   		 break;
   		 //case QtProfile::PVRMSE:
   		 //	ok=ana->getFreqProfile( wxv, wyv, xval, yval,
   		 //			"world", coordinateType, 0, 0, 0, xaxisUnit, spcRefFrame,
   		 //			(Int)QtProfile::RMSE, 0);
   		 // break;
   	 default:
   		 ok=ana->getFreqProfile( wxv, wyv, xval, yval,
   				 "world", coordinateType, 0, 0, 0, xaxisUnit, spcRefFrame,
   				 (Int)QtProfile::MEAN, 0);
   		 break;
   	 }

   	 if (ok) {
   		 if(ordersOfM!=0){
   			 // correct display y axis values
   			 for (uInt i = 0; i < yval.size(); i++) {
   				 yval(i) *= pow(10.,ordersOfM);
   			 }
   		 }
   		 Vector<Float> xRel(yval.size());
   		 Vector<Float> yRel(yval.size());
   		 Int count = 0;
   		 if (stateRel) {
   			 ky = ky+ "_rel.";
   			 for (uInt i = 0; i < yval.size(); i++) {
   				 uInt k = z_yval.size() - 1;
   				 //cout << xval(i) << " - " << yval(i) << endl;
   				 //cout << z_xval(0) << " + " << z_xval(k) << endl;
   				 if (coordinateType.contains("elocity")) {
   					 if (xval(i) < z_xval(0) && xval(i) >= z_xval(k)) {
   						 for (uInt j = 0; j < k; j++) {
   							 //cout << z_xval(j) << " + "
   							 //     << z_yval(j) << endl;
   							 if (xval(i) <= z_xval(j) &&
   									 xval(i) > z_xval(j + 1)) {
   								 float s = z_xval(j + 1) - z_xval(j);
   								 if (s != 0) {
   									 xRel(count) = xval(i);
   									 yRel(count)= yval(i) -
   											 (z_yval(j) + (xval(i) - z_xval(j)) / s *
   													 (z_yval(j + 1) - z_yval(j)));
   									 count++;
   									 //yval(i) -= (z_yval(j) + (xval(i) - z_xval(j)) / s *
   									 //           (z_yval(j + 1) - z_yval(j)));

   								 }
   								 break;
   							 }
   						 }
   					 }
   				 }
   				 else {
   					 if (xval(i) >= z_xval(0) && xval(i) < z_xval(k)) {
   						 for (uInt j = 0; j < k; j++) {
   							 if (xval(i) >= z_xval(j) && xval(i) < z_xval(j + 1)) {
   								 float s = z_xval(j + 1) - z_xval(j);
   								 if (s != 0) {
   									 xRel(count) = xval(i);
   									 yRel(count)= yval(i) -
   											 (z_yval(j) + (xval(i) - z_xval(j)) / s *
   													 (z_yval(j + 1) - z_yval(j)));
   									 count++;
   									 //yval(i) -= (z_yval(j) + (xval(i) - z_xval(j)) / s *
   									 //           (z_yval(j + 1) - z_yval(j)));
   								 }
   								 break;
   							 }
   						 }
   					 }
   				 }
   			 }
   			 xRel.resize(count, True);
   			 yRel.resize(count, True);
   			 pixelCanvas->addPolyLine(xRel, yRel, ky);
   		 }
   		 else {
   			 pixelCanvas->addPolyLine(xval, yval, ky);
   		 }
   	 }
    }

    lastWX.assign(wxv);
    lastWY.assign(wyv);
    lastPX.assign(pxv);
    lastPY.assign(pyv);

}


void QtProfile::changePlotType(const QString &text) {

	// store the plot type and set the class data
	rc.put( "viewer." + rcid() + ".plot.type", text.toStdString());
	stringToPlotType(text, itsPlotType);

	// get the coo-sys
	CoordinateSystem cSys = image->coordinates();
	yUnit = QString(image->units().getName().chars());

	switch (itsPlotType)
	{
	case QtProfile::PMEAN:
		if (npoints !=1 && errorMode->findText("rmse") < 0)
			errorMode->insertItem(1, "rmse");
		if (npoints ==1 && errorMode->findText("rmse") > -1)
			errorMode->removeItem(errorMode->findText("rmse"));
		if (cSys.qualityAxisNumber() > -1 && errorMode->findText("propagated") < 0)
			errorMode->insertItem(2, "propagated");
		break;
	case QtProfile::PMEDIAN:
		if (npoints !=1 && errorMode->findText("rmse") < 0)
			errorMode->insertItem(1, "rmse");
		if (npoints ==1 && errorMode->findText("rmse") > -1)
			errorMode->removeItem(errorMode->findText("rmse"));
		if (errorMode->findText("propagated") > -1)
			errorMode->removeItem(errorMode->findText("propagated"));
		break;
	case QtProfile::PSUM:
	case QtProfile::PFLUX:
		if (errorMode->findText("rmse") > -1)
			errorMode->removeItem(errorMode->findText("rmse"));
		if (cSys.qualityAxisNumber() > -1 && errorMode->findText("propagated") < 0)
			errorMode->insertItem(1, "propagated");
		break;
	//case QtProfile::PVRMSE:
	//	if (errorMode->findText("rmse") > -1)
	//		errorMode->removeItem(errorMode->findText("rmse"));
	//	if (errorMode->findText("propagated") > -1)
	//		errorMode->removeItem(errorMode->findText("propagated"));
	//	break;
	}

	redraw();
}

void QtProfile::changeErrorType(const QString &text) {
	rc.put( "viewer." + rcid() + ".error.type", text.toStdString());

	stringToErrorType(text, itsErrorType);

	redraw();
}

void QtProfile::changeCollapseType(QString text) {
	bool switchError(false);

	// if no type given means
	// initialization
	if (text.size()<1){
		switchError=true;

		// read and set a type from the rc-file
		text = QString(rc.get("viewer." + rcid() + ".collapse.type").c_str());
		if (text.size()>0){
			int index = collapseType->findText(text);
			if (index > -1)
				collapseType->setCurrentIndex(index);
		}
		else{
			// just use what's there
			text=collapseType->currentText();
		}
	}

	//set the class data
	SpectralCollapser::stringToCollapseType(String(text.toStdString ()), itsCollapseType);

	// get the coo-sys
	CoordinateSystem cSys = image->coordinates();

	// depending on the collapse type,
	// insert the allowed error types
	switch (itsCollapseType)
	{
	case SpectralCollapser::PMEAN:
		if (collapseError->findText("rmse") < 0)
			collapseError->insertItem(1, "rmse");
		if (cSys.qualityAxisNumber() > -1 && collapseError->findText("propagated") < 0)
			collapseError->insertItem(1, "propagated");
		if (cSys.qualityAxisNumber() < 0 && collapseError->findText("propagated") > -1)
			collapseError->removeItem(collapseError->findText("propagated"));
		break;
	case SpectralCollapser::PMEDIAN:
		if (collapseError->findText("rmse") < 0)
			collapseError->insertItem(1, "rmse");
		if (collapseError->findText("propagated") > -1)
			collapseError->removeItem(collapseError->findText("propagated"));
		break;
	case SpectralCollapser::PSUM:
		if (collapseError->findText("rmse") > -1)
			collapseError->removeItem(collapseError->findText("rmse"));
		if (cSys.qualityAxisNumber() > -1 && collapseError->findText("propagated") < 0)
			collapseError->insertItem(1, "propagated");
		if (cSys.qualityAxisNumber() < 0 && collapseError->findText("propagated") > -1)
			collapseError->removeItem(collapseError->findText("propagated"));
		break;
	}

	// store the collapse type in the rc-file
	rc.put( "viewer." + rcid() + ".collapse.type", text.toStdString());

	// if initialization
	if (switchError){

		// read the error type from the rc-file
		QString error(rc.get("viewer." + rcid() + ".collerror.type").c_str());
		if (error.size()>0){

			// if the error type does exist, which means
			// if it is allowed, set it
			int index = collapseError->findText(error);
			if (index > -1){
				collapseError->setCurrentIndex(index);
				SpectralCollapser::stringToCollapseError(String(error.toStdString ()), itsCollapseError);
			}
		}
	}
}

void QtProfile::changeCollapseError(QString text) {
	if (text.size()<1)
		text=collapseError->currentText();
	rc.put( "viewer." + rcid() + ".collerror.type", text.toStdString());
	SpectralCollapser::stringToCollapseError(String(text.toStdString ()), itsCollapseError);
}

void QtProfile::redraw( ) {
    wcChanged( last_event_cs, last_event_px, last_event_py, last_event_wx, last_event_wy, UNKNPROF);
}
void QtProfile::changeAxisOld(String xa, String ya, String za, std::vector<int> ) {
	//cout << "change axis=" << xa << " " << ya
	//     << " " << za << " cube=" << cube << endl;
	int cb = 0;
	if (xa.contains("Decl") && ya.contains("Right"))
		cb = -1;
	if (xa.contains("Right") && ya.contains("Decl"))
		cb = 1;
	if (xa.contains("atitu") && ya.contains("ongitu"))
		cb = -1;
	if (xa.contains("ongitu") && ya.contains("atitu"))
		cb = 1;
	if (!za.contains("Freq"))
		cb = 0;
	//if (cb != cube) {
	cube = cb;
	xpos = "";
	ypos = "";
	position = QString("");
	profileStatus->showMessage(position);
	if (cube == 0)
		pixelCanvas->setWelcome("No profile available "
				"for the given data \nor\n"
				"No profile available for the "
				"display axes orientation"
		);
	else
		pixelCanvas->setWelcome("assign a mouse button to\n"
				"'crosshair' or 'rectangle' or 'polygon'\n"
				"click/press+drag the assigned button on\n"
				"the image to get a spectral profile");

	pixelCanvas->clearCurve();
	//}

	//cout << "cube=" << cube << endl;

}
void QtProfile::changeAxis(String xa, String ya, String za, std::vector<int> ) {

	// the logic is as follows:
	// "za" (the z-axis") MUST be named "Frequency"
	// "xa" and "ya" must contain EITHER "Right" and "Declination"
	// OR "Latitude" and "Longitude".
	// The order is not important, and "cb=1" marks "normal" order
	// (xa=Ra/Lo, ya=Dec/La) while "cb=1" marks "not normal" order
	// (xa=Dec/La, ya=Ra/Lo).
	int cb = 0;
	if (xa.contains("Decl") && ya.contains("Right"))
		cb = -1;
	if (xa.contains("Right") && ya.contains("Decl"))
		cb = 1;
	if (xa.contains("atitu") && ya.contains("ongitu"))
		cb = -1;
	if (xa.contains("ongitu") && ya.contains("atitu"))
		cb = 1;
	if (!za.contains("Freq"))
		cb = 0;

	if (cb==0){
		// the current configuration can
		// NOT be plotted
		xpos = "";
		ypos = "";
		position = QString("");
		profileStatus->showMessage(position);
		pixelCanvas->setWelcome("No profile available "
				"for the given data \nor\n"
				"No profile available for the "
				"display axes orientation"
		);
		xpos = "";
		ypos = "";
		position = QString("");
		profileStatus->showMessage(position);
		pixelCanvas->clearCurve();
	}
	else {
		if (cb != cube){
			// either the profiler was intialized
			// or the axes have changed
			xpos = "";
			ypos = "";
			position = QString("");
			profileStatus->showMessage(position);
			pixelCanvas->setWelcome("assign a mouse button to\n"
					"'crosshair' or 'rectangle' or 'polygon'\n"
					"click/press+drag the assigned button on\n"
					"the image to get a spectral profile");
			pixelCanvas->clearCurve();
		}
	}

	// store the value
	cube = cb;
}

void QtProfile::changeSpectrum(String spcTypeUnit, String spcRval, String spcSys){
   *itsLog << LogOrigin("QtProfile", "changeSpectrum");

	// transform to QString
	QString  qSpcTypeUnit = QString(spcTypeUnit.c_str());
	QString  qSpcRval(spcRval.c_str());
	QString  qSpcSys(spcSys.c_str());

	if (qSpcSys != spcRef->currentText()){
		// if necessary, change the spectral frame
		int index = spcRef->findText(qSpcSys);
		if (index > -1)
			spcRef->setCurrentIndex(index);
	}
	if (qSpcTypeUnit != ctype->currentText()){
		// if necessary, change the unit and the spectral quantity
		int index = ctype->findText(qSpcTypeUnit);
		if (index > -1){
			ctype->setCurrentIndex(index);
		}
		else {
	   	 //
	    	*itsLog << LogIO::WARN << "Can not switch profile to spectral quantity and unit: \"" << qSpcTypeUnit.toStdString() << "\"!" << LogIO::POST;
		}
	}
	if (spcRval != cSysRval){
		// if necessary, change the rest freq./wavel.
		cSysRval = spcRval;
		//qDebug() << "New rest freq./wavel.: " <<  cSysRval.c_str();
	}
}

void QtProfile::doImgCollapse(){

	*itsLog << LogOrigin("QtProfile", "doImgCollapse");

   	// get the values
   	QString startStr = startValue->text();
	QString endStr   = endValue->text();
	String msg;

	// make sure the input is reasonable

	if (startStr.isEmpty()){
		msg = String("No start value specified!");
		*itsLog << LogIO::WARN << msg << LogIO::POST;
		profileStatus->showMessage(QString(msg.c_str()));
		return;
	}
	if (endStr.isEmpty()){
		msg = String("No end value specified!");
		*itsLog << LogIO::WARN << msg << LogIO::POST;
		profileStatus->showMessage(QString(msg.c_str()));
		return;
	}

	int pos=0;
	if (startValue->validator()->validate(startStr, pos) != QValidator::Acceptable){
		String startString(startStr.toStdString());
		msg = String("Start value not correct: ") + startString;
		*itsLog << LogIO::WARN << msg << LogIO::POST;
		profileStatus->showMessage(QString(msg.c_str()));
		return;
	}
	if (endValue->validator()->validate(endStr, pos) != QValidator::Acceptable){
		String endString(endStr.toStdString());
		msg = String("Start value not correct: ") + endString;
		*itsLog << LogIO::WARN << msg << LogIO::POST;
		profileStatus->showMessage(QString(msg.c_str()));
		return;
	}

	// convert input values to Float
	Float startVal=(Float)startStr.toFloat();
	Float endVal  =(Float)endStr.toFloat();

	String outname;
	Bool ok = collapser->collapse(z_xval, startVal, endVal, xaxisUnit, itsCollapseType, itsCollapseError, outname, msg);

	if (ok){
		*itsLog << msg << LogIO::POST;
		profileStatus->showMessage(QString(msg.c_str()));
	}
	else {
		msg = String("Problem collapsing the image: ") + msg;
		*itsLog << LogIO::WARN << msg << LogIO::POST;
		profileStatus->showMessage(QString(msg.c_str()));
		return;
	}

	emit showCollapsedImg(outname, "image", "raster", True, True);

	return;
}

void QtProfile::doLineFit(){

	*itsLog << LogOrigin("QtProfile", "doLineFit");

	// get the values
	QString startStr = startValueFit->text();
	QString endStr   = endValueFit->text();
	String  msg;

	// make sure the input is reasonable

	if (startStr.isEmpty()){
		msg = String("No start value specified!");
		*itsLog << LogIO::WARN << msg << LogIO::POST;
		profileStatus->showMessage(QString(msg.c_str()));
		return;
	}
	if (endStr.isEmpty()){
		msg = String("No end value specified!");
		*itsLog << LogIO::WARN << msg << LogIO::POST;
		profileStatus->showMessage(QString(msg.c_str()));
		return;
	}

	int pos=0;
	if (startValueFit->validator()->validate(startStr, pos) != QValidator::Acceptable){
		String startString(startStr.toStdString());
		msg = String("Start value not correct: ") + startString;
		*itsLog << LogIO::WARN << msg << LogIO::POST;
		profileStatus->showMessage(QString(msg.c_str()));
		return;
	}
	if (endValueFit->validator()->validate(endStr, pos) != QValidator::Acceptable){
		String endString(endStr.toStdString());
		msg = String("Start value not correct: ") + endString;
		*itsLog << LogIO::WARN << msg << LogIO::POST;
		profileStatus->showMessage(QString(msg.c_str()));
		return;
	}

	// convert input values to Float
	Float startVal=(Float)startStr.toFloat();
	Float endVal  =(Float)endStr.toFloat();

	// set the fitting modes
	Bool doFitGauss(False);
	Bool doFitPoly(False);
	if (fitGauss->currentText()==QString("gauss"))
		doFitGauss=True;
	//Int polyN = Int(fitPolyN->value());
	//if (polyN>-1)
	//	doFitPoly=True;
	Int polyN;
	if (fitPolyN->currentText()==QString("poly 0")){
		doFitPoly=True;
		polyN=0;
	}
	else if (fitPolyN->currentText()==QString("poly 1")){
		doFitPoly=True;
		polyN=1;
	}

	// make sure something should be fitted at all
	if (!doFitGauss && !doFitPoly){
		msg = String("There is nothing to fit!");
		*itsLog << LogIO::WARN << msg << LogIO::POST;
		profileStatus->showMessage(QString(msg.c_str()));
		return;
	}

	// do the fit
	Bool ok = fitter->fit(z_xval, z_yval, z_eval, startVal, endVal, doFitGauss, doFitPoly, polyN, msg);

	if (fitter->getStatus() == SpectralFitter::SUCCESS){
		// get the fit values
		Vector<Float> z_xfit, z_yfit;
		fitter->getFit(z_xval, z_xfit, z_yfit);

		// report problems
		if (z_yfit.size()<1){
			msg = String("There exist no fit values!");
			*itsLog << LogIO::WARN << msg << LogIO::POST;
			profileStatus->showMessage(QString(msg.c_str()));
			return;
		}

		// overplot the fit values
		QString fitName = fileName + "FIT" + startStr + "-" + endStr + QString(xaxisUnit.c_str());
		pixelCanvas->addPolyLine(z_xfit, z_yfit, fitName);
	}
	profileStatus->showMessage(QString((fitter->report(*itsLog)).c_str()));

	return;
}

void QtProfile::plotMainCurve(){
	pixelCanvas->clearData();
	pixelCanvas->plotPolyLine(z_xval, z_yval, z_eval, fileName);
}

void QtProfile::setCollapseRange(float xmin, float xmax){
	if (xmax < xmin){
		startValue->clear();
		endValue->clear();
		startValueFit->clear();
		endValueFit->clear();
	}
	else {
		QString startStr;
		QString endStr;
		startStr.setNum(xmin);
		endStr.setNum(xmax);
		startValue->setText(startStr);
		endValue->setText(endStr);
		startValueFit->setText(startStr);
		endValueFit->setText(endStr);
	}
}

void QtProfile::overplot(QHash<QString, ImageInterface<float>*> hash) {

	// re-set the images
	// that are overplotted
	if (over) {
		// TODO: put that in to avoid memory loss
		//QHashIterator<QString, ImageAnalysis*> ih(*over);
		//while (ih.hasNext()) {
		//	ih.next();
		//	//cout << ih.key() << ": " << ih.value() << endl;
		//	delete ih.value();
		//	qDebug() << "the frame: " << (QString)ih.key();
		//}
		delete over;
		over = 0;
	}

	over = new QHash<QString, ImageAnalysis*>();
	QHashIterator<QString, ImageInterface<float>*> i(hash);
	while (i.hasNext()) {
		i.next();
		//qDebug() << i.key() << ": " << i.value();
		QString ky = i.key();
		ImageAnalysis* ana = new ImageAnalysis(i.value());
		(*over)[ky] = ana;
	}
}


void QtProfile::newRegion( int id_, const QString &shape, const QString &name,
			   const QList<double> &world_x, const QList<double> &world_y,
			   const QList<int> &pixel_x, const QList<int> &pixel_y,
			   const QString &linecolor, const QString &text, const QString &font, int fontsize, int fontstyle ) {

    if (!isVisible()) return;
    if (!analysis) return;

    spectra_info_map[id_] = shape;
    String c("world");

    Vector<Double> px(pixel_x.size());
    Vector<Double> py(pixel_y.size());
    Vector<Double> wx(world_x.size());
    Vector<Double> wy(world_y.size());

    for ( int x=0; x < px.nelements(); ++x ) px[x] = pixel_x[x];
    for ( int x=0; x < py.nelements(); ++x ) py[x] = pixel_y[x];
    for ( int x=0; x < wx.nelements(); ++x ) wx[x] = world_x[x];
    for ( int x=0; x < wy.nelements(); ++x ) wy[x] = world_y[x];

    *itsLog << LogOrigin("QtProfile", "newRegion");

    if (cube == 0) {
	pixelCanvas->setWelcome( "No profile available "
			"for the given data \nor\n"
			"No profile available for the "
			"display axes orientation");
	pixelCanvas->clearCurve();
	return;
    }

    last_event_cs.resize(c.size());
    last_event_cs = c;
    last_event_px.resize(px.size());
    last_event_px = px;
    last_event_py.resize(py.size());
    last_event_py = py;
    last_event_wx.resize(wx.size());
    last_event_wx = wx;
    last_event_wy.resize(wy.size());
    last_event_wy = wy;

    npoints = wx.size();

    if (npoints_old==0 && wx.size()>0){
	npoints_old = wx.size();
	changePlotType(plotMode->currentText());
    } else {
	if (npoints==1 & npoints_old!=1){
	    npoints_old = npoints;
	    changePlotType(plotMode->currentText());
	} else if (npoints!=1 & npoints_old==1) {
	    npoints_old = npoints;
	    changePlotType(plotMode->currentText());
	}
    }

    if (c != coordinate) {
	coordinate = c.chars();
	//if (coordinate == "world")
	//    chk->setCurrentIndex(0);
	//else
	//    chk->setCurrentIndex(1);
    }

    Int ns;
    px.shape(ns);

    Vector<Double> pxv(ns);
    Vector<Double> pyv(ns);
    Vector<Double> wxv(ns);
    Vector<Double> wyv(ns);

    if (cube == -1){
	pxv.assign(py);
	pyv.assign(px);
	wxv.assign(wy);
	wyv.assign(wx);
    } else{
	pxv.assign(px);
	pyv.assign(py);
	wxv.assign(wx);
	wyv.assign(wy);
    }

    if (ns < 1) return;

    if ( shape == "point" ) {
	pixelCanvas->setTitle("Single Point Profile");
	region = "Point";
    } else if ( shape == "rectangle" ) {
	pixelCanvas->setTitle("Rectangle Region Profile");
	region = "Rect";
    } else if ( shape == "ellipse" ) {
	pixelCanvas->setTitle("Elliptical Region Profile");
	region = "Ellipse";
    } else if ( shape == "polygon" ) {
	pixelCanvas->setTitle("Polygon Region Profile");
	region = "Poly";
    } else {
	pixelCanvas->setTitle("");
	region = "";
    }
    pixelCanvas->setWelcome("");

	 Double xmean, ymean, minval, maxval;
    if (coordinate == "world") {
   	 if (wxv.size()==1){
          xmean =  wxv[0];
          ymean =  wyv[0];
   	 }
   	 else if (wxv.size()==2){
          xmean =  0.5*(wxv[0]+wxv[1]);
          ymean =  0.5*(wyv[0]+wyv[1]);
   	 }
   	 else {
          minMax(minval, maxval, wxv);
          xmean =  0.5*(minval + maxval);
          minMax(minval, maxval, wyv);
          ymean =  0.5*(minval + maxval);
   	 }
   	 //xpos, ypos and position only used for display
       xpos = QString::number(floor(xmean+0.5));
       ypos = QString::number(floor(ymean+0.5));

       position = getRaDec(xmean, ymean);
    }
    else {
   	 if (pxv.size()==1){
   		 xmean = pxv[0];
   		 ymean = pyv[0];
   	 }
   	 else if (pxv.size()==1){
   		 xmean = 0.5*(pxv[0]+pxv[1]);
   		 ymean = 0.5*(pyv[0]+pyv[0]);
   	 }
   	 else{
          minMax(minval, maxval, pxv);
          xmean =  0.5*(minval + maxval);
          minMax(minval, maxval, pyv);
          ymean =  0.5*(minval + maxval);
   	 }

       //xpos, ypos and position only used for display
       xpos = QString::number(floor(xmean+0.5));
       ypos = QString::number(floor(ymean+0.5));
       position = QString("[%1, %2]").arg(xpos).arg(ypos);
    }
    profileStatus->showMessage(position);

    //Get Profile Flux density v/s coordinateType
    Bool ok = False;
    switch (itsPlotType) {
	case QtProfile::PMEAN:
	    ok=analysis->getFreqProfile( wxv, wyv, z_xval, z_yval,
					 "world", coordinateType, 0, 0, 0, xaxisUnit, spcRefFrame,
					 (Int)QtProfile::MEAN, 0 );
	    break;
	case QtProfile::PMEDIAN:
	    ok=analysis->getFreqProfile( wxv, wyv, z_xval, z_yval,
					 "world", coordinateType, 0, 0, 0, xaxisUnit, spcRefFrame,
					 (Int)QtProfile::MEDIAN, 0);
	    break;
	case QtProfile::PSUM:
	    ok=analysis->getFreqProfile( wxv, wyv, z_xval, z_yval,
					 "world", coordinateType, 0, 0, 0, xaxisUnit, spcRefFrame,
					 (Int)QtProfile::SUM, 0);
	    break;
	case QtProfile::PFLUX:
	    ok=analysis->getFreqProfile( wxv, wyv, z_xval, z_yval,
					 "world", coordinateType, 0, 0, 0, xaxisUnit, spcRefFrame,
					 (Int)QtProfile::FLUX, 0);
	    break;
	//case QtProfile::PVRMSE:
	//	 ok=analysis->getFreqProfile( wxv, wyv, z_xval, z_yval,
	//			 "world", coordinateType, 0, 0, 0, xaxisUnit, spcRefFrame,
	//			 (Int)QtProfile::RMSE, 0);
	//	 break;
	default:
	    ok=analysis->getFreqProfile( wxv, wyv, z_xval, z_yval,
					 "world", coordinateType, 0, 0, 0, xaxisUnit, spcRefFrame,
					 (Int)QtProfile::MEAN, 0);
	    break;
    }

    if (!ok) {
   	 // change to notify user of error...
    	*itsLog << LogIO::WARN << "Can not generate the frequency profile!" << LogIO::POST;
    	return;
    }

    // get the coordinate system
    CoordinateSystem cSys = image->coordinates();
    switch (itsErrorType) {
	case QtProfile::PNOERROR:
	    if (z_eval.size()> 0)
		z_eval.resize(0);
	    break;
	case QtProfile::PERMSE:
	    if ( wxv.size() < 2 ) {
		*itsLog << LogIO::NORMAL << "Can not do the plot request, only one point!" << LogIO::POST;
		if (z_eval.size()> 0)
		z_eval.resize(0);
	    } else {
		switch (itsPlotType) {
		    case QtProfile::PSUM:
		    case QtProfile::PFLUX:
			*itsLog << LogIO::NORMAL << "Plotting RMSE as error of SUM makes no sense!" << LogIO::POST;
			if (z_eval.size()> 0)
			    z_eval.resize(0);
			break;
		    //case QtProfile::PVRMSE:
		    //		 cout << "Plotting RMSE over RMSE makes no sense!" << endl;
		    //  	 if (z_eval.size()> 0)
		    //  		 z_eval.resize(0);
		    //		 break;
		    default:
			ok=analysis->getFreqProfile( wxv, wyv, z_xval, z_eval,
						     "world", coordinateType, 0, 0, 0, xaxisUnit, spcRefFrame,
						     (Int)QtProfile::RMSE, 0);
			break;
		}
	    }
	    break;
	case QtProfile::PPROPAG:
	    if ( cSys.qualityAxisNumber() < 0 ) {
		*itsLog << LogIO::NORMAL << "Can not do the plot request, no quality axis!" << LogIO::POST;
		if (z_eval.size()> 0)
		    z_eval.resize(0);
	    } else {
		switch ( itsPlotType ) {
		    case QtProfile::PMEAN:
			ok=analysis->getFreqProfile( wxv, wyv, z_xval, z_eval,
						     "world", coordinateType, 0, 0, 0, xaxisUnit, spcRefFrame,
						     (Int)QtProfile::NSQRTSUM, 1);
			break;
		    case QtProfile::PMEDIAN:
			*itsLog << LogIO::NORMAL << "Can not plot the error, NO propagation for median!" << LogIO::POST;
			if (z_eval.size()> 0)
			    z_eval.resize(0);
			    break;
		    case QtProfile::PSUM:
			ok=analysis->getFreqProfile( wxv, wyv, z_xval, z_eval,
						     "world", coordinateType, 0, 0, 0, xaxisUnit, spcRefFrame,
						     (Int)QtProfile::SQRTSUM, 1);
			break;
		    case QtProfile::PFLUX:
			ok=analysis->getFreqProfile( wxv, wyv, z_xval, z_eval,
						     "world", coordinateType, 0, 0, 0, xaxisUnit, spcRefFrame,
						     (Int)QtProfile::EFLUX, 1);
			break;
		    default:
			if (z_eval.size()> 0)
			    z_eval.resize(0);
			break;
		}
	    }
	    break;
	default:
	    if (z_eval.size()> 0)
		z_eval.resize(0);
	    break;
    }

    if ( ! ok ) {
	// change to notify user of error...
	*itsLog << LogIO::WARN << "Can not generate the frequency error profile!" << LogIO::POST;
	return;
    }

    // scale for better display
    // max absolute display numbers should be between 0.1 and 100.0
    Double dmin = 0.1;
    Double dmax = 100.;
    Double ymin = min(z_yval);
    Double ymax = max(z_yval);
    ymax = max(abs(ymin), ymax);
    Double symax;
    Int ordersOfM = 0;

    symax = ymax;
    while(symax < dmin && ymax != 0.0){
	ordersOfM += 3;
	symax = ymax * pow(10.,ordersOfM);
    }
    while(symax > dmax && ymax != 0.0){
	ordersOfM -= 3;
	symax = ymax * pow(10.,ordersOfM);
    }

    if(ordersOfM!=0){
	// correct display y axis values
	for (uInt i = 0; i < z_yval.size(); i++) {
	    z_yval(i) *= pow(10.,ordersOfM);
	}
	if ( z_eval.size() > 0 ) {
	    for (uInt i = 0; i < z_eval.size(); i++) {
		z_eval(i) *= pow(10.,ordersOfM);
	    }
	}
    }

    // store the scaling factor
    ordersOfM_=ordersOfM;

    if(ordersOfM!=0){
	// correct unit string
	if( yUnit.startsWith("(") || yUnit.startsWith("[") || yUnit.startsWith("\"") ){
	    // express factor as number
	    ostringstream oss;
	    oss << -ordersOfM;
	    yUnitPrefix = "10E"+QString(oss.str().c_str())+" ";
	} else {
	    // express factor as character
	    switch(-ordersOfM){ // note the sign!
		case -9:
		    yUnitPrefix = "p";
		    break;
		case -6:
		    yUnitPrefix = "u";
		    break;
	    	case -3:
		    yUnitPrefix = "m";
		    break;
		case 3:
		    yUnitPrefix = "k";
		    break;
		case 6:
		    yUnitPrefix = "M";
		    break;
		case 9:
		    yUnitPrefix = "M";
		    break;
		default:
		    ostringstream oss;
		    oss << -ordersOfM;
		    yUnitPrefix = "10E"+QString(oss.str().c_str())+" ";
		    break;
	    }
	}
    } else{ // no correction
	yUnitPrefix = "";
    }

    pixelCanvas->setYLabel("("+yUnitPrefix+yUnit+")", 12, 2, "Helvetica [Cronyx]");

    // plot the graph

    pixelCanvas->clearData();
    pixelCanvas->plotPolyLine(z_xval, z_yval, z_eval, fileName);


    QHashIterator<QString, ImageAnalysis*> i(*over);
    while (i.hasNext() && stateMProf) {
	i.next();
	//qDebug() << i.key() << ": " << i.value();
	QString ky = i.key();
	ImageAnalysis* ana = i.value();
	Vector<Float> xval(100);
	Vector<Float> yval(100);

	switch (itsPlotType) {
	    case QtProfile::PMEAN:
		ok=ana->getFreqProfile( wxv, wyv, xval, yval,
					"world", coordinateType, 0, 0, 0, xaxisUnit, spcRefFrame,
					(Int)QtProfile::MEAN, 0);
		break;
	    case QtProfile::PMEDIAN:
		ok=ana->getFreqProfile( wxv, wyv, xval, yval,
					"world", coordinateType, 0, 0, 0, xaxisUnit, spcRefFrame,
					(Int)QtProfile::MEDIAN, 0);
		break;
	    case QtProfile::PSUM:
		ok=ana->getFreqProfile( wxv, wyv, xval, yval,
					"world", coordinateType, 0, 0, 0, xaxisUnit, spcRefFrame,
					(Int)QtProfile::PSUM, 0);
		break;
	    case QtProfile::PFLUX:
		ok=ana->getFreqProfile( wxv, wyv, xval, yval,
					"world", coordinateType, 0, 0, 0, xaxisUnit, spcRefFrame,
					(Int)QtProfile::PFLUX, 0);
		break;
	    //case QtProfile::PVRMSE:
	    //	ok=ana->getFreqProfile( wxv, wyv, xval, yval,
	    //			"world", coordinateType, 0, 0, 0, xaxisUnit, spcRefFrame,
	    //			(Int)QtProfile::RMSE, 0);
	    // break;
	    default:
		ok=ana->getFreqProfile( wxv, wyv, xval, yval,
					"world", coordinateType, 0, 0, 0, xaxisUnit, spcRefFrame,
					(Int)QtProfile::MEAN, 0);
		break;
	}

	if (ok) {
	    if( ordersOfM != 0 ) {
		// correct display y axis values
		for (uInt i = 0; i < yval.size(); i++) {
		    yval(i) *= pow(10.,ordersOfM);
		}
	    }
	    Vector<Float> xRel(yval.size());
	    Vector<Float> yRel(yval.size());
	    Int count = 0;
	    if (stateRel) {
	    	ky = ky+ "_rel.";
	    	for (uInt i = 0; i < yval.size(); i++) {
	    		uInt k = z_yval.size() - 1;
	    		//cout << xval(i) << " - " << yval(i) << endl;
	    		//cout << z_xval(0) << " + " << z_xval(k) << endl;
	    		if (coordinateType.contains("elocity")) {
	    			if (xval(i) < z_xval(0) && xval(i) >= z_xval(k)) {
	    				for (uInt j = 0; j < k; j++) {
	    					//cout << z_xval(j) << " + "
	    					//     << z_yval(j) << endl;
	    					if (xval(i) <= z_xval(j) && xval(i) > z_xval(j + 1)) {
	    						float s = z_xval(j + 1) - z_xval(j);
	    						if (s != 0) {
	    							xRel(count) = xval(i);
	    							yRel(count)= yval(i) - (z_yval(j) + (xval(i) - z_xval(j)) / s * (z_yval(j + 1) - z_yval(j)));
	    							count++;
	    							//yval(i) -= (z_yval(j) + (xval(i) - z_xval(j)) / s * (z_yval(j + 1) - z_yval(j)));
	    						}
	    						break;
	    					}
	    				}
	    			}
	    		} else {
	    			if (xval(i) >= z_xval(0) && xval(i) < z_xval(k)) {
	    				for (uInt j = 0; j < k; j++) {
	    					if (xval(i) >= z_xval(j) && xval(i) < z_xval(j + 1)) {
	    						float s = z_xval(j + 1) - z_xval(j);
	    						if (s != 0) {
	    							xRel(count) = xval(i);
	    							yRel(count)= yval(i) - (z_yval(j) + (xval(i) - z_xval(j)) / s * (z_yval(j + 1) - z_yval(j)));
	    							count++;
	    							//yval(i) -= (z_yval(j) + (xval(i) - z_xval(j)) / s * (z_yval(j + 1) - z_yval(j)));
	    						}
	    						break;
	    					}
	    				}
	    			}
	    		}
	    	}
	    	xRel.resize(count, True);
	    	yRel.resize(count, True);
	    	pixelCanvas->addPolyLine(xRel, yRel, ky);
	    } else {
	    	pixelCanvas->addPolyLine(xval, yval, ky);
	    }
	}
    }

    lastWX.assign(wxv);
    lastWY.assign(wyv);
    lastPX.assign(pxv);
    lastPY.assign(pyv);
}


void QtProfile::updateRegion( int id_, const QList<double> &world_x, const QList<double> &world_y,
			      const QList<int> &pixel_x, const QList<int> &pixel_y ) {


    if (!isVisible()) return;
    if (!analysis) return;

    SpectraInfoMap::iterator it = spectra_info_map.find(id_);
    if ( it == spectra_info_map.end( ) ) return;

    QString shape = it->second.shape( );

    String c("world");

    Vector<Double> px(pixel_x.size());
    Vector<Double> py(pixel_y.size());
    Vector<Double> wx(world_x.size());
    Vector<Double> wy(world_y.size());

    for ( int x=0; x < px.nelements(); ++x ) px[x] = pixel_x[x];
    for ( int x=0; x < py.nelements(); ++x ) py[x] = pixel_y[x];
    for ( int x=0; x < wx.nelements(); ++x ) wx[x] = world_x[x];
    for ( int x=0; x < wy.nelements(); ++x ) wy[x] = world_y[x];

    *itsLog << LogOrigin("QtProfile", "newRegion");

    if (cube == 0) {
	pixelCanvas->setWelcome( "No profile available "
			"for the given data \nor\n"
			"No profile available for the "
			"display axes orientation");
	pixelCanvas->clearCurve();
	return;
    }

    last_event_cs.resize(c.size());
    last_event_cs = c;
    last_event_px.resize(px.size());
    last_event_px = px;
    last_event_py.resize(py.size());
    last_event_py = py;
    last_event_wx.resize(wx.size());
    last_event_wx = wx;
    last_event_wy.resize(wy.size());
    last_event_wy = wy;

    npoints = wx.size();

    if (npoints_old==0 && wx.size()>0){
	npoints_old = wx.size();
	changePlotType(plotMode->currentText());
    } else {
	if (npoints==1 & npoints_old!=1){
	    npoints_old = npoints;
	    changePlotType(plotMode->currentText());
	} else if (npoints!=1 & npoints_old==1) {
	    npoints_old = npoints;
	    changePlotType(plotMode->currentText());
	}
    }

    if (c != coordinate) {
    	coordinate = c.chars();
	//if (coordinate == "world")
	//    chk->setCurrentIndex(0);
	//else
	//    chk->setCurrentIndex(1);
    }

    Int ns;
    px.shape(ns);

    Vector<Double> pxv(ns);
    Vector<Double> pyv(ns);
    Vector<Double> wxv(ns);
    Vector<Double> wyv(ns);

    if (cube == -1){
	pxv.assign(py);
	pyv.assign(px);
	wxv.assign(wy);
	wyv.assign(wx);
    } else{
	pxv.assign(px);
	pyv.assign(py);
	wxv.assign(wx);
	wyv.assign(wy);
    }

    if (ns < 1) return;

    if ( shape == "point" ) {
	pixelCanvas->setTitle("Single Point Profile");
	region = "Point";
    } else if ( shape == "rectangle" ) {
	pixelCanvas->setTitle("Rectangle Region Profile");
	region = "Rect";
    } else if ( shape == "ellipse" ) {
	pixelCanvas->setTitle("Elliptical Region Profile");
	region = "Ellipse";
    } else if ( shape == "polygon" ) {
	pixelCanvas->setTitle("Polygon Region Profile");
	region = "Poly";
    } else {
	pixelCanvas->setTitle("");
	region = "";
    }
    pixelCanvas->setWelcome("");

	 Double xmean, ymean, minval, maxval;
    if (coordinate == "world") {
   	 if (wxv.size()==1){
          xmean =  wxv[0];
          ymean =  wyv[0];
   	 }
   	 else if (wxv.size()==2){
          xmean =  0.5*(wxv[0]+wxv[1]);
          ymean =  0.5*(wyv[0]+wyv[1]);
   	 }
   	 else {
          minMax(minval, maxval, wxv);
          xmean =  0.5*(minval + maxval);
          minMax(minval, maxval, wyv);
          ymean =  0.5*(minval + maxval);
   	 }
   	 //xpos, ypos and position only used for display
       xpos = QString::number(floor(xmean+0.5));
       ypos = QString::number(floor(ymean+0.5));

       position = getRaDec(xmean, ymean);
    }
    else {
   	 if (pxv.size()==1){
   		 xmean = pxv[0];
   		 ymean = pyv[0];
   	 }
   	 else if (pxv.size()==1){
   		 xmean = 0.5*(pxv[0]+pxv[1]);
   		 ymean = 0.5*(pyv[0]+pyv[0]);
   	 }
   	 else{
          minMax(minval, maxval, pxv);
          xmean =  0.5*(minval + maxval);
          minMax(minval, maxval, pyv);
          ymean =  0.5*(minval + maxval);
   	 }

       //xpos, ypos and position only used for display
       xpos = QString::number(floor(xmean+0.5));
       ypos = QString::number(floor(ymean+0.5));
       position = QString("[%1, %2]").arg(xpos).arg(ypos);
    }
    profileStatus->showMessage(position);

    //Get Profile Flux density v/s coordinateType
    Bool ok = False;
    switch (itsPlotType) {
	case QtProfile::PMEAN:
	    ok=analysis->getFreqProfile( wxv, wyv, z_xval, z_yval,
					 "world", coordinateType, 0, 0, 0, xaxisUnit, spcRefFrame,
					 (Int)QtProfile::MEAN, 0 );
	    break;
	case QtProfile::PMEDIAN:
	    ok=analysis->getFreqProfile( wxv, wyv, z_xval, z_yval,
					 "world", coordinateType, 0, 0, 0, xaxisUnit, spcRefFrame,
					 (Int)QtProfile::MEDIAN, 0);
	    break;
	case QtProfile::PSUM:
	    ok=analysis->getFreqProfile( wxv, wyv, z_xval, z_yval,
					 "world", coordinateType, 0, 0, 0, xaxisUnit, spcRefFrame,
					 (Int)QtProfile::SUM, 0);
	    break;
	case QtProfile::PFLUX:
	    ok=analysis->getFreqProfile( wxv, wyv, z_xval, z_yval,
					 "world", coordinateType, 0, 0, 0, xaxisUnit, spcRefFrame,
					 (Int)QtProfile::FLUX, 0);
	    break;
	//case QtProfile::PVRMSE:
	//	 ok=analysis->getFreqProfile( wxv, wyv, z_xval, z_yval,
	//			 "world", coordinateType, 0, 0, 0, xaxisUnit, spcRefFrame,
	//			 (Int)QtProfile::RMSE, 0);
	//	 break;
	default:
	    ok=analysis->getFreqProfile( wxv, wyv, z_xval, z_yval,
					 "world", coordinateType, 0, 0, 0, xaxisUnit, spcRefFrame,
					 (Int)QtProfile::MEAN, 0);
	    break;
    }

    if (!ok) {
   	 // change to notify user of error...
    	*itsLog << LogIO::WARN << "Can not generate the frequency profile!" << LogIO::POST;
    	return;
    }

    // get the coordinate system
    CoordinateSystem cSys = image->coordinates();
    switch (itsErrorType) {
	case QtProfile::PNOERROR:
	    if (z_eval.size()> 0)
		z_eval.resize(0);
	    break;
	case QtProfile::PERMSE:
	    if ( wxv.size() < 2 ) {
		*itsLog << LogIO::NORMAL << "Can not do the plot request, only one point!" << LogIO::POST;
		if (z_eval.size()> 0)
		z_eval.resize(0);
	    } else {
		switch (itsPlotType) {
		    case QtProfile::PSUM:
			*itsLog << LogIO::NORMAL << "Plotting RMSE as error of SUM makes no sense!" << LogIO::POST;
			if (z_eval.size()> 0)
			    z_eval.resize(0);
			break;
		    //case QtProfile::PVRMSE:
		    //		 cout << "Plotting RMSE over RMSE makes no sense!" << endl;
		    //  	 if (z_eval.size()> 0)
		    //  		 z_eval.resize(0);
		    //		 break;
		    default:
			ok=analysis->getFreqProfile( wxv, wyv, z_xval, z_eval,
						     "world", coordinateType, 0, 0, 0, xaxisUnit, spcRefFrame,
						     (Int)QtProfile::RMSE, 0);
			break;
		}
	    }
	    break;
	case QtProfile::PPROPAG:
	    if ( cSys.qualityAxisNumber() < 0 ) {
		*itsLog << LogIO::NORMAL << "Can not do the plot request, no quality axis!" << LogIO::POST;
		if (z_eval.size()> 0)
		    z_eval.resize(0);
	    } else {
		switch ( itsPlotType ) {
		    case QtProfile::PMEAN:
			ok=analysis->getFreqProfile( wxv, wyv, z_xval, z_eval,
						     "world", coordinateType, 0, 0, 0, xaxisUnit, spcRefFrame,
						     (Int)QtProfile::NSQRTSUM, 1);
			break;
		    case QtProfile::PMEDIAN:
			*itsLog << LogIO::NORMAL << "Can not plot the error, NO propagation for median!" << LogIO::POST;
			if (z_eval.size()> 0)
			    z_eval.resize(0);
			    break;
		    case QtProfile::PSUM:
			ok=analysis->getFreqProfile( wxv, wyv, z_xval, z_eval,
						     "world", coordinateType, 0, 0, 0, xaxisUnit, spcRefFrame,
						     (Int)QtProfile::SQRTSUM, 1);
			break;
		    case QtProfile::PFLUX:
			ok=analysis->getFreqProfile( wxv, wyv, z_xval, z_eval,
						     "world", coordinateType, 0, 0, 0, xaxisUnit, spcRefFrame,
						     (Int)QtProfile::EFLUX, 1);
			break;
		    default:
			if (z_eval.size()> 0)
			    z_eval.resize(0);
			break;
		}
	    }
	    break;
	default:
	    if (z_eval.size()> 0)
		z_eval.resize(0);
	    break;
    }

    if ( ! ok ) {
	// change to notify user of error...
	*itsLog << LogIO::WARN << "Can not generate the frequency error profile!" << LogIO::POST;
	return;
    }

    // scale for better display
    // max absolute display numbers should be between 0.1 and 100.0
    Double dmin = 0.1;
    Double dmax = 100.;
    Double ymin = min(z_yval);
    Double ymax = max(z_yval);
    ymax = max(abs(ymin), ymax);
    Double symax;
    Int ordersOfM = 0;

    symax = ymax;
    while(symax < dmin && ymax != 0.0){
	ordersOfM += 3;
	symax = ymax * pow(10.,ordersOfM);
    }
    while(symax > dmax && ymax != 0.0){
	ordersOfM -= 3;
	symax = ymax * pow(10.,ordersOfM);
    }

    if(ordersOfM!=0){
	// correct display y axis values
	for (uInt i = 0; i < z_yval.size(); i++) {
	    z_yval(i) *= pow(10.,ordersOfM);
	}
	if ( z_eval.size() > 0 ) {
	    for (uInt i = 0; i < z_eval.size(); i++) {
		z_eval(i) *= pow(10.,ordersOfM);
	    }
	}
    }

    // store the scaling factor
    ordersOfM_=ordersOfM;

    if(ordersOfM!=0){
	// correct unit string
	if( yUnit.startsWith("(") || yUnit.startsWith("[") || yUnit.startsWith("\"") ){
	    // express factor as number
	    ostringstream oss;
	    oss << -ordersOfM;
	    yUnitPrefix = "10E"+QString(oss.str().c_str())+" ";
	} else {
	    // express factor as character
	    switch(-ordersOfM){ // note the sign!
		case -9:
		    yUnitPrefix = "p";
		    break;
		case -6:
		    yUnitPrefix = "u";
		    break;
	    	case -3:
		    yUnitPrefix = "m";
		    break;
		case 3:
		    yUnitPrefix = "k";
		    break;
		case 6:
		    yUnitPrefix = "M";
		    break;
		case 9:
		    yUnitPrefix = "M";
		    break;
		default:
		    ostringstream oss;
		    oss << -ordersOfM;
		    yUnitPrefix = "10E"+QString(oss.str().c_str())+" ";
		    break;
	    }
	}
    } else{ // no correction
	yUnitPrefix = "";
    }

    pixelCanvas->setYLabel("("+yUnitPrefix+yUnit+")", 12, 2, "Helvetica [Cronyx]");

    // plot the graph

    pixelCanvas->clearData();
    pixelCanvas->plotPolyLine(z_xval, z_yval, z_eval, fileName);


    QHashIterator<QString, ImageAnalysis*> i(*over);
    while (i.hasNext() && stateMProf) {
	i.next();
	//qDebug() << i.key() << ": " << i.value();
	QString ky = i.key();
	ImageAnalysis* ana = i.value();
	Vector<Float> xval(100);
	Vector<Float> yval(100);

	switch (itsPlotType) {
	    case QtProfile::PMEAN:
		ok=ana->getFreqProfile( wxv, wyv, xval, yval,
					"world", coordinateType, 0, 0, 0, xaxisUnit, spcRefFrame,
					(Int)QtProfile::MEAN, 0);
		break;
	    case QtProfile::PMEDIAN:
		ok=ana->getFreqProfile( wxv, wyv, xval, yval,
					"world", coordinateType, 0, 0, 0, xaxisUnit, spcRefFrame,
					(Int)QtProfile::MEDIAN, 0);
		break;
	    case QtProfile::PSUM:
		ok=ana->getFreqProfile( wxv, wyv, xval, yval,
					"world", coordinateType, 0, 0, 0, xaxisUnit, spcRefFrame,
					(Int)QtProfile::PSUM, 0);
		break;
	    case QtProfile::PFLUX:
		ok=ana->getFreqProfile( wxv, wyv, xval, yval,
					"world", coordinateType, 0, 0, 0, xaxisUnit, spcRefFrame,
					(Int)QtProfile::PFLUX, 0);
		break;
	    //case QtProfile::PVRMSE:
	    //	ok=ana->getFreqProfile( wxv, wyv, xval, yval,
	    //			"world", coordinateType, 0, 0, 0, xaxisUnit, spcRefFrame,
	    //			(Int)QtProfile::RMSE, 0);
	    // break;
	    default:
		ok=ana->getFreqProfile( wxv, wyv, xval, yval,
					"world", coordinateType, 0, 0, 0, xaxisUnit, spcRefFrame,
					(Int)QtProfile::MEAN, 0);
		break;
	}

	if (ok) {
	    if( ordersOfM != 0 ) {
		// correct display y axis values
		for (uInt i = 0; i < yval.size(); i++) {
		    yval(i) *= pow(10.,ordersOfM);
		}
	    }
	    Vector<Float> xRel(yval.size());
	    Vector<Float> yRel(yval.size());
	    Int count = 0;
	    if (stateRel) {
	    	ky = ky+ "_rel.";
	    	for (uInt i = 0; i < yval.size(); i++) {
	    		uInt k = z_yval.size() - 1;
	    		//cout << xval(i) << " - " << yval(i) << endl;
	    		//cout << z_xval(0) << " + " << z_xval(k) << endl;
	    		if (coordinateType.contains("elocity")) {
	    			if (xval(i) < z_xval(0) && xval(i) >= z_xval(k)) {
	    				for (uInt j = 0; j < k; j++) {
	    					//cout << z_xval(j) << " + "
	    					//     << z_yval(j) << endl;
	    					if (xval(i) <= z_xval(j) && xval(i) > z_xval(j + 1)) {
	    						float s = z_xval(j + 1) - z_xval(j);
	    						if (s != 0) {
	    							xRel(count) = xval(i);
	    							yRel(count)= yval(i) - (z_yval(j) + (xval(i) - z_xval(j)) / s * (z_yval(j + 1) - z_yval(j)));
	    							count++;
	    							//yval(i) -= (z_yval(j) + (xval(i) - z_xval(j)) / s * (z_yval(j + 1) - z_yval(j)));
	    						}
	    						break;
	    					}
	    				}
	    			}
	    		} else {
	    			if (xval(i) >= z_xval(0) && xval(i) < z_xval(k)) {
	    				for (uInt j = 0; j < k; j++) {
	    					if (xval(i) >= z_xval(j) && xval(i) < z_xval(j + 1)) {
	    						float s = z_xval(j + 1) - z_xval(j);
	    						if (s != 0) {
	    							xRel(count) = xval(i);
	    							yRel(count)= yval(i) - (z_yval(j) + (xval(i) - z_xval(j)) / s * (z_yval(j + 1) - z_yval(j)));
	    							count++;
	    							//yval(i) -= (z_yval(j) + (xval(i) - z_xval(j)) / s * (z_yval(j + 1) - z_yval(j)));
	    						}
	    						break;
	    					}
	    				}
	    			}
	    		}
	    	}
	    	xRel.resize(count, True);
	    	yRel.resize(count, True);
	    	pixelCanvas->addPolyLine(xRel, yRel, ky);
	    } else {
	    	pixelCanvas->addPolyLine(xval, yval, ky);
	    }
	}
    }

    lastWX.assign(wxv);
    lastWY.assign(wyv);
    lastPX.assign(pxv);
    lastPY.assign(pyv);
}


bool QtProfile::exportFITSSpectrum(QString &fn)
{
	// get the image coo and the spectral axis therein
   CoordinateSystem cSys = image->coordinates();
   Int wCoord = cSys.findCoordinate(Coordinate::SPECTRAL);
   Vector<Int> pCoord = cSys.pixelAxes(wCoord);

   if (wCoord <0){
   	// well, it REALLY should not get to here
   	QString msg="No spectral coordinate in image:\n" + QString((image->name(True)).c_str());
   	messageFromProfile(msg);
   	return false;
   }

   // get the spectral dimension and make some checks
   uInt nPoints=(image->shape())(pCoord(0));
   if (nPoints != z_yval.size()){
   	// well, this should not happen
   	QString msg="The dimension of the image and\nthe extracted profile do not match!";
   	messageFromProfile(msg);
   	return false;
   }
   else if (nPoints<1){
   	// well, this should not happen
   	QString msg="The extracted profile contains no points!";
   	messageFromProfile(msg);
   	return false;
   }

   // create a new coo and add the spectral one
   CoordinateSystem csysProfile = CoordinateSystem();
   csysProfile.addCoordinate(cSys.spectralCoordinate(wCoord));

   // if necessary, add a quality coordinate
   IPosition profDim;
   if (z_eval.size()>0){
   	Vector<Int> quality(2);
   	quality(0) = Quality::DATA;
   	quality(1) = Quality::ERROR;
   	QualityCoordinate qualAxis(quality);
   	csysProfile.addCoordinate(qualAxis);

   	profDim = IPosition(2,nPoints,2);
   }
   else{
   	profDim = IPosition(1,nPoints);
   }

   // create the temp-image
   TempImage<Float> profile(TiledShape(profDim),csysProfile);

   // scale the data and store the values in the
   // temp-image
	Float scaleFactor=pow(10.,ordersOfM_);
	IPosition posIndex;
   if (z_eval.size()>0){
   	Int qualCooPos, qualIndex;
   	posIndex = IPosition(2, 0);

   	// re-find the quality coordinate
   	qualCooPos = csysProfile.findCoordinate(Coordinate::QUALITY);
   	if (qualCooPos<0){
   		// this really should not happen
      	QString msg="Error finding the Quality coordinate!";
      	messageFromProfile(msg);
      	return false;
   	}

   	// get the pixel index of DATA
   	if (!(csysProfile.qualityCoordinate(qualCooPos)).toPixel(qualIndex, Quality::DATA)){
      	QString msg="Error finding the DATA index in quality coordinate!";
      	messageFromProfile(msg);
      	return false;
   	}
   	posIndex(1)=qualIndex;
   	for (uInt index=0; index<nPoints; index++){
   		posIndex(0)=index;
   		profile.putAt (z_yval(index)/scaleFactor, posIndex);
   		//cout << " " << z_yval(index)/scaleFactor << " : " << posIndex;
   	}


   	// get the pixel index of ERROR
   	if (!(csysProfile.qualityCoordinate(qualCooPos)).toPixel(qualIndex, Quality::ERROR)){
      	QString msg="Error finding the ERROR index in quality coordinate!";
      	messageFromProfile(msg);
      	return false;
   	}
   	posIndex(1)=qualIndex;
   	for (uInt index=0; index<nPoints; index++){
   		posIndex(0)=index;
   		profile.putAt (z_eval(index)/scaleFactor, posIndex);
   		//cout << " " << z_eval(index)/scaleFactor << " : " << posIndex;
   	}
   }
   else{
   	posIndex = IPosition(1);
   	for (uInt index=0; index<nPoints; index++){
   		posIndex(0)=index;
   		profile.putAt (z_yval(index)/scaleFactor, posIndex);
   		//cout << " " << z_yval(index)/scaleFactor;
   	}
   }

   // attach a mask to the temp-image
	Array<Bool> maskArray(profDim, True);
	ArrayLattice<Bool> maskLattice=ArrayLattice<Bool>(maskArray);
	profile.attachMask(maskLattice);

	// compile and set the miscInfo
	TableRecord miscInfo;
	miscInfo.define("inimage", image->name(True));
	miscInfo.setComment("inimage", "name input image");
	miscInfo.define("position", position.toStdString());
	miscInfo.setComment("position", "extraction position");
	miscInfo.define("proftype", (pixelCanvas->getTitle()).toStdString());
	miscInfo.setComment("proftype", "the profile type");
	miscInfo.define("plottype", (plotMode->currentText()).toStdString());
	miscInfo.setComment("plottype", "the plot type");
   if (z_eval.size()>0){
   	miscInfo.define("errtype", (errorMode->currentText()).toStdString());
   	miscInfo.setComment("errtype", "the error type");
   }
	profile.setMiscInfo(miscInfo);

   // thats the default values for the call "ImageFITSConverter::ImageToFITS"
	String error; uInt memoryInMB(64); Bool preferVelocity(True);
	Bool opticalVelocity(True); Int BITPIX(-32); Float minPix(1.0); Float maxPix(-1.0);
	Bool allowOverwrite(False); Bool degenerateLast(False); Bool verbose(True);
	Bool stokesLast(False); Bool preferWavelength(False); Bool preferAirWavelength(False);
	String origin("CASA Viewer / Spectral Profiler");
	allowOverwrite=True;
	String outFile(fn.toStdString());

	// find the "natural" flags for the spectral axis
	SpectralCoordinate::SpecType spcType = (cSys.spectralCoordinate(wCoord)).nativeType();
	switch (spcType) {
	case SpectralCoordinate::FREQ:
		preferVelocity      = False;
		opticalVelocity     = False;
		preferWavelength    = False;
		preferAirWavelength = False;
		break;
	case SpectralCoordinate::VRAD:
		preferVelocity      = True;
		opticalVelocity     = False;
		preferWavelength    = False;
		preferAirWavelength = False;
		break;
	case SpectralCoordinate::VOPT:
		preferVelocity      = True;
		opticalVelocity     = True;
		preferWavelength    = False;
		preferAirWavelength = False;
		break;
	case SpectralCoordinate::BETA:
		preferVelocity      = False;
		opticalVelocity     = False;
		preferWavelength    = False;
		preferAirWavelength = False;
		break;
	case SpectralCoordinate::WAVE:
		preferVelocity      = False;
		opticalVelocity     = False;
		preferWavelength    = True;
		preferAirWavelength = False;
		break;
	case SpectralCoordinate::AWAV:
		preferVelocity      = False;
		opticalVelocity     = False;
		preferWavelength    = True;
		preferAirWavelength = True;
		break;
	default:
		preferVelocity      = False;
		opticalVelocity     = False;
		preferWavelength    = False;
		preferAirWavelength = False;
	}

	try {
		// export the image to FITS
		ImageFITSConverter::ImageToFITS(
				error,
				profile,
				outFile,
				memoryInMB,
				preferVelocity,
				opticalVelocity,
				BITPIX,
				minPix,
				maxPix,
				allowOverwrite,
				degenerateLast,
				verbose,
				stokesLast,
				preferWavelength,
				preferAirWavelength,
				origin
		);
	} catch (AipsError x) {
		// catch an exception and report
   	QString msg="Error while exporting FITS:\n"+QString((x.getMesg()).c_str());
   	messageFromProfile(msg);
   	return false;
	}

	// check for any error indicated via the error-string
	if (error.size()>0){
   	QString msg="Error while exporting FITS:\n"+QString((error.c_str()));
   	messageFromProfile(msg);
   	return false;
	}

	return true;
}

bool QtProfile::exportASCIISpectrum(QString &fn)
	{
   QFile file(fn);
    if (!file.open(QFile::WriteOnly | QIODevice::Text))
        return false;
    QTextStream ts(&file);

    ts << "#title: Spectral profile - " << fileName << " "
       << region << "(" << position << ")\n";
    ts << "#coordintate: " << QString(coordinate.chars()) << "\n";
    ts << "#xLabel: " << QString(ctypeUnit.chars()) << "\n";
    ts << "#yLabel: " << "(" << yUnitPrefix << yUnit << ") "<< plotMode->currentText() << "\n";
    if (z_eval.size() > 0)
        ts << "#eLabel: " << "(" << yUnitPrefix << yUnit << ") " << errorMode->currentText() << "\n";

    ts.setRealNumberNotation(QTextStream::ScientificNotation);

    if (z_eval.size() > 0){
    	for (uInt i = 0; i < z_xval.size(); i++) {
    		ts << z_xval(i) << "    " << z_yval(i) << "    "  << z_eval(i) << "\n";
    	}
    }
    else {
    	for (uInt i = 0; i < z_xval.size(); i++) {
    		ts << z_xval(i) << "    " << z_yval(i) << "\n";
    	}
    }

    int i = pixelCanvas->getLineCount();
    for (int k = 1; k < i; k++) {
      ts << "\n";
      ts << "# " << pixelCanvas->getCurveName(k) << "\n";
      CurveData data = *(pixelCanvas->getCurveData(k));
      int j = data.size() / 2;
      for (int m = 0; m < j; m++) {
         ts << data[2 * m] << " " << data[2 * m + 1] << "\n";
      }
    }

    return true;
}

void QtProfile::messageFromProfile(QString &msg){
	//QMessageBox qMsg(this);
	//qMsg.setText(msg);
	//qMsg.exec();
	// critical
	QMessageBox::critical(this, "Error", msg);
}


void QtProfile::fillPlotTypes(){

	if (plotMode->count() <1 ){

		// fill the plot types
		plotMode->addItem("mean");
		plotMode->addItem("median");
		plotMode->addItem("sum");
		plotMode->addItem("flux");
		//plotMode->addItem("rmse");

		// read the preferred plot mode from casarc
		QString pref_plotMode = QString(rc.get("viewer." + rcid() + ".plot.type").c_str());
		if (pref_plotMode.size()>0){
			// change to the preferred plotMode
			int etypeindex= plotMode->findText(pref_plotMode);
			if (etypeindex > -1)
				plotMode->setCurrentIndex(etypeindex);
			else
				plotMode->setCurrentIndex(0);
		}
		stringToPlotType(plotMode->currentText(), itsPlotType);
	}

	// clean out the error box
	if (errorMode->count() > 0){
		if (errorMode->findText("propagated") > -1){
			errorMode->removeItem(errorMode->findText("propagated"));
		}
		if (errorMode->findText("rmse") > -1){
			errorMode->removeItem(errorMode->findText("rmse"));
		}
	}

	// get the coo-sys
	CoordinateSystem cSys = image->coordinates();

	// add the 'no error' option
	if (errorMode->findText("no error") < 0)
		errorMode->addItem("no error");

	switch (itsPlotType)
	{
	case QtProfile::PMEAN:
		if (npoints!=1)
			errorMode->addItem("rmse");
		if (cSys.qualityAxisNumber() > -1)
			errorMode->addItem("propagated");
		break;
	case QtProfile::PMEDIAN:
		if (npoints!=1)
			errorMode->addItem("rmse");
		break;
	case QtProfile::PSUM:
	case QtProfile::PFLUX:
		if (cSys.qualityAxisNumber() > -1)
			errorMode->addItem("propagated");
		break;
	//case QtProfile::PVRMSE:
	//	break;
	}

	// read the preferred error mode from casarc
   QString pref_errMode = QString(rc.get("viewer." + rcid() + ".error.type").c_str());
   if (pref_errMode.size()>0){
     // change to the preferred plotMode
   	int e_index= errorMode->findText(pref_errMode);
   	if (e_index > -1)
   		errorMode->setCurrentIndex(e_index);
   	else
   		errorMode->setCurrentIndex(0);
   }

   stringToErrorType(errorMode->currentText(), itsErrorType);
	rc.put( "viewer." + rcid() + ".error.type", errorMode->currentText().toStdString());
}

void QtProfile::stringToPlotType(const QString &text, QtProfile::PlotType &pType){
	*itsLog << LogOrigin("QtProfile", "stringToPlotType");

	if (!text.compare(QString("mean")))
		pType = QtProfile::PMEAN;
	else if (!text.compare(QString("median")))
		pType = QtProfile::PMEDIAN;
	else if (!text.compare(QString("sum")))
		pType = QtProfile::PSUM;
	else if (!text.compare(QString("flux")))
		pType = QtProfile::PFLUX;
	//else if (!text.compare(QString("rmse")))
	//	pType = QtProfile::PVRMSE;
	else
		*itsLog << LogIO::WARN << "The string: " << text.toStdString() << " does not correspond to a plot type!" << LogIO::POST;
}

void QtProfile::stringToErrorType(const QString &text, QtProfile::ErrorType &eType){
	*itsLog << LogOrigin("QtProfile", "stringToErrorType");

	if (!text.compare(QString("no error")))
		eType = QtProfile::PNOERROR;
	else if (!text.compare(QString("rmse")))
		eType = QtProfile::PERMSE;
	else if (!text.compare(QString("propagated")))
		eType = QtProfile::PPROPAG;
	else
		*itsLog << LogIO::WARN << "The string: " << text.toStdString() << " does not correspond to an error type!" << LogIO::POST;
}


void QtProfile::getcoordTypeUnit(String &ctypeUnitStr, String &cTypeStr, String &unitStr){
   // determine the coordinate type
   if (ctypeUnitStr.contains("air wavelength"))
   	cTypeStr = String("air wavelength");
   else if (ctypeUnitStr.contains("wavelength"))
   	cTypeStr = String("wavelength");
   else if (ctypeUnitStr.contains("radio velocity"))
   	cTypeStr = String("radio velocity");
   else if (ctypeUnitStr.contains("optical velocity"))
   	cTypeStr = String("optical velocity");
   else if (ctypeUnitStr.contains("frequency"))
   	cTypeStr = String("frequency");
   else
   	cTypeStr = String("channel");

   // determine the unit
   if (ctypeUnitStr.contains("[Hz]"))
   	unitStr = String("Hz");
   else if (ctypeUnitStr.contains("[MHz]"))
   	unitStr = String("MHz");
   else if (ctypeUnitStr.contains("[GHz]"))
   	unitStr = String("GHz");
   else if (ctypeUnitStr.contains("[m/s]"))
   	unitStr = String("m/s");
   else if (ctypeUnitStr.contains("[km/s]"))
   	unitStr = String("km/s");
   else if (ctypeUnitStr.contains("[mm]"))
   	unitStr = String("mm");
   else if (ctypeUnitStr.contains("[um]"))
   	unitStr = String("um");
   else if (ctypeUnitStr.contains("[nm]"))
   	unitStr = String("nm");
   else if (ctypeUnitStr.contains("[Angstrom]"))
   	unitStr = String("Angstrom");
   else
   	unitStr = String("");
}


void QtProfile::printIt(QPrinter* printer)
{
    QPainter painter(printer);
    QRect rect = painter.viewport();
    rect.adjust(72, 72, -72, -72);
    QPixmap *mp = pixelCanvas->graph();
    QSize size = mp->size();
    size.scale(rect.size(), Qt::KeepAspectRatio);
    painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
    painter.setWindow(mp->rect());
    painter.drawPixmap(0, 0, *mp);
    painter.end();
}

QString QtProfile::getRaDec(double x, double y) {
   QString raDec = "";
   int sign = (y > 0) ? 1 : -1;
   const double a = 572.95779513082;
   double rah, ram, decd, decm;
   double ras = x * 24 * a;
   double decs = sign * y * 360 * a;

   if (ras > 86400) ras = 0;
   if (decs > 1296000) decs = 0;

   int h, m, d, c;
   rah = ras / 3600;
   h = (int)floor(rah);
   ram = (rah - h) * 60;
   m = (int)floor(ram);
   ras = (ram - m) * 60;
   ras = (int)(1000 * ras) / 1000.;

   decd = decs / 3600;
   d = (int)floor(decd);
   decm = (decd - d) * 60;
   c = (int)floor(decm);
   decs = (decm - c) * 60;
   decs = (int)(1000 * decs) / 1000.;

   raDec.append((h < 10) ? "0" : "").append(QString().setNum(h)).append(":")
        .append((m < 10) ? "0" : "").append(QString().setNum(m)).append(":")
        .append((ras < 10) ? "0" : "").append(QString().setNum(ras))
        .append((sign > 0) ? "+" : "-")
        .append((d < 10) ? "0" : "").append(QString().setNum(d)).append("d")
        .append((c < 10) ? "0" : "").append(QString().setNum(c)).append("m")
        .append((decs < 10) ? "0" : "").append(QString().setNum(decs));

   return raDec;
}
}
