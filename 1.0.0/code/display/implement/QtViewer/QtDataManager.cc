//# QtDataManager.cc: Qt implementation of viewer data manager widget.
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
//# $Id$


#include <display/QtViewer/QtViewer.qo.h>
#include <display/QtViewer/QtDataManager.qo.h>
#include <display/QtViewer/QtDisplayPanelGui.qo.h>
#include <display/QtViewer/QtDisplayData.qo.h>
#include <tables/Tables/Table.h>
#include <tables/Tables/TableInfo.h>
#include <images/Images/FITSImgParser.h>
#include "fitsio.h"
#include <casa/BasicSL/String.h>
#include <casa/OS/File.h>
#include <casa/iostream.h>
#include <casa/fstream.h>
#include <casa/Exceptions/Error.h>
#include <display/QtViewer/QtDisplayPanel.qo.h>

#include <images/Images/PagedImage.h>		/*** needed for global imagePixelType( ) ***/
#include <sstream>

#include <casa/Utilities/GenSort.h>

#include <graphics/X11/X_enter.h>
#include <QDir>
#include <QMessageBox>
#include <QDirModel>
#include <QHash>
#include <QSettings>
#include <QTextStream>
#include <graphics/X11/X_exit.h>


namespace casa { //# NAMESPACE CASA - BEGIN



QtDataManager::QtDataManager(QtDisplayPanelGui* panel,
		const char *name,
		QWidget *parent ) :
		QWidget(parent),
		parent_(parent),
		panel_(panel),
		ms_selection(new Ui::QtDataMgrMsSelect),
		rc(viewer::getrc()) {
  
    setWindowTitle(name);
  
    setupUi(this);
    ms_selection->setupUi(ms_selection_scroll_widget);

    ifields.push_back(infofield_list_t::value_type(ibox11,itext11));
    ifields.push_back(infofield_list_t::value_type(ibox12,itext12));
    ifields.push_back(infofield_list_t::value_type(ibox21,itext21));
    ifields.push_back(infofield_list_t::value_type(ibox22,itext22));
    ifields.push_back(infofield_list_t::value_type(ibox31,itext31));
    ifields.push_back(infofield_list_t::value_type(ibox32,itext32));
    ifields.push_back(infofield_list_t::value_type(ibox41,itext41));
    ifields.push_back(infofield_list_t::value_type(ibox42,itext42));

#if defined(__APPLE__)
    QFont field_font( "Lucida Grande", 10 );
#else
    QFont field_font( "Sans Serif", 7 );
#endif
    for ( infofield_list_t::iterator it = ifields.begin( ); it != ifields.end( ); ++it ) {
	(*it).first->setTitle(" ");
	(*it).second->clear( );
	(*it).second->setFont( field_font );
    }

    std::string show_lel = rc.get("viewer." + panel_->rcid() + ".datamgr.show_lel");
    if ( show_lel != "true" && show_lel != "false" ) {
	rc.put( "viewer." + panel_->rcid() + ".datamgr.show_lel", "false" );
	show_lel = "false";
    }
    std::string leave_up = rc.get("viewer." + panel_->rcid() + ".datamgr.leave_up");
    if ( leave_up != "true" && leave_up != "false" ) {
	rc.put( "viewer." + panel_->rcid() + ".datamgr.leave_up", "true" );
	leave_up = "true";
    }
  
    // Tue Sep  6 12:17:21 EDT 2011
    // don't think people really use the lel entry, and while it's removal wasn't
    // required (with the addition of the ms selection to the "loading options"),
    // originally the lel expression was supposed to be replace with ms selection
    // options... so we'll remove it and see what happens <drs>...
    // Fri Nov 18 22:39:26 UTC 2011
    // some complaints make it reasonable to re-enable it as an option <drs>...
    if ( show_lel == "false" ) {
	lelGB_->hide( );
	showLEL_->setChecked( false );
    } else {
	showLEL_->setChecked( true );
    }


    connect(showLEL_, SIGNAL(clicked(bool)), SLOT(showlelButtonClicked(bool)));

  
    //updateButton_->setEnabled(false);	//#dk until this works.

    lelEdit_->setToolTip( "Enter an image expression, such as\n"
			  "'clean.im' - 'dirty.im'.  For details, see:\n"
			  "aips2.nrao.edu/docs/notes/223/223.html" );
  
  
    hideDisplayButtons();

    uiDataType_["Unknown"]          = UNKNOWN;
    uiDataType_["Image"]            = IMAGE;
    uiDataType_["Measurement Set"]  = MEASUREMENT_SET;
    uiDataType_["Sky Catalog"]      = SKY_CATALOG;
    uiDataType_["Directory"]        = DIRECTORY;
    uiDataType_["FITS Image"]       = IMAGE;
    uiDataType_["FITS Ext."]        = IMAGE;
    uiDataType_["Quality Ext."]     = QUALIMG;
    uiDataType_["Miriad Image"]     = IMAGE;
    uiDataType_["Gipsy"]            = IMAGE;
    uiDataType_["Restore File"]     = RESTORE;
    uiDataType_["CASA Region File"] = CASAREGION;
    uiDataType_["DS9 Region File"] = DS9REGION;
  
    dataType_[UNKNOWN]              = "unknown";
    dataType_[IMAGE]                = "image";
    dataType_[QUALIMG]              = "image";
    dataType_[MEASUREMENT_SET]      = "ms";
    dataType_[SKY_CATALOG]          = "skycatalog";
    dataType_[RESTORE]              = "restore";
    dataType_[CASAREGION]           = "casa region";
    dataType_[DS9REGION]           = "ds9 region";

    uiDisplayType_["raster image"]  = RASTER;
    uiDisplayType_["contour map"]   = CONTOUR;
    uiDisplayType_["vector map"]    = VECTOR;
    uiDisplayType_["marker map"]    = MARKER;
    uiDisplayType_["sky catalog"]   = SKY_CAT;
    uiDisplayType_["old window"]    = OLDPANEL;
    uiDisplayType_["new window"]    = NEWPANEL;
  
    displayType_["raster"]          = RASTER;
    displayType_["contour"]         = CONTOUR;
    displayType_["vector"]          = VECTOR;
    displayType_["marker"]          = MARKER;
    displayType_["skycatalog"]      = SKY_CAT;
    displayType_["oldpanel"]        = OLDPANEL;
    displayType_["newpanel"]        = NEWPANEL;
  
    leaveOpen_->setToolTip( "Uncheck to close this window after "
			    "data and display type selection.\n"
			    "Use 'Open' button/menu on Display Panel to show it again." );

    if ( leave_up == "true" ) {
	leaveOpen_->setChecked(true);
    } else {
	leaveOpen_->setChecked(false);
    }
    connect(leaveOpen_, SIGNAL(clicked(bool)), SLOT(leaveopenButtonClicked(bool)));

    dir_.setFilter( QDir::AllDirs | QDir::Files);
    dir_.setSorting(QDir::Name);

    //#dk  QSettings are often useful, but users
    //     want to begin in their cwd in this case....
    //
    // QSettings settings("NRAO", "casa");
    // QString lastDir = settings.value("lastDir", dir_.currentPath())
    //                          .toString();
    // //cout << "lastDir=" << lastDir.toStdString() << endl;
    // dir_.cd(lastDir);
    // dirLineEdit_->setText(lastDir);
  
    dirLineEdit_->setText(dir_.currentPath());
    panel_->selectedDMDir = dirLineEdit_->text().toStdString();
  
  
    buildDirTree();
  
  
    connect(rasterButton_,  SIGNAL(clicked()), SLOT(createButtonClicked()));
    connect(contourButton_, SIGNAL(clicked()), SLOT(createButtonClicked()));
    connect(vectorButton_,  SIGNAL(clicked()), SLOT(createButtonClicked()));
    connect(markerButton_,  SIGNAL(clicked()), SLOT(createButtonClicked()));
    connect(catalogButton_, SIGNAL(clicked()), SLOT(createButtonClicked()));
    connect(oldPanelButton_,  SIGNAL(clicked()), SLOT(restoreToOld_()));
    connect(newPanelButton_,  SIGNAL(clicked()), SLOT(restoreToNew_()));
    connect(updateButton_,  SIGNAL(clicked()), SLOT(buildDirTree()));
    connect(regionButton_, SIGNAL(clicked()), SLOT(load_regions_clicked()));

    //connect(registerCheck, SIGNAL(clicked()), 
    //      SLOT(registerClicked()));
  
    connect(dirLineEdit_,   SIGNAL(returnPressed()), SLOT(returnPressed()));
  
    connect(treeWidget_,    SIGNAL(itemSelectionChanged()),
			    SLOT(changeItemSelection()));
  
    connect(treeWidget_,    SIGNAL(itemClicked(QTreeWidgetItem*,int)),
			    SLOT(clickItem(QTreeWidgetItem*)));

    connect(treeWidget_, SIGNAL(itemExpanded(QTreeWidgetItem*)),
			 SLOT(expandItem(QTreeWidgetItem*)));
  
    connect(panel_, SIGNAL(createDDFailed(String, String, String, String)),
		    SLOT(showDDCreateError_(String)));
  
    connect(lelEdit_, SIGNAL(gotFocus(QFocusEvent*)),  SLOT(lelGotFocus_()));

}


QtDataManager::~QtDataManager(){
}


void QtDataManager::clickItem(QTreeWidgetItem* item){
	// make sure a directory was clicked
	if(item!=0 && item->text(1)=="Directory"){

		// get the text
		QString iText = item->text(0);

		// if there is a text go to that directory
		if (iText.size()>0){
			updateDirectory(item->text(0));}

		// if there is no text
		else if (iText.size()==0){

			// get the top-level index
			int index = treeWidget_->indexOfTopLevelItem (item );

			// go to "home" or "root"
			if (index == 0)
				updateDirectory(QDir::homePath());
			else if (index == 1)
				updateDirectory(QDir::rootPath());
		}
	}
}


static int findNumberOfFITSImageExt( QString path ) {
    fitsfile *fptr;
    int status = 0;
    fits_open_file( &fptr, path.toAscii( ).constData( ), READONLY, &status );
    if ( status != 0 ) {
	fits_report_error(stderr, status);
	return -1;
    }
    int number_hdus = 0;
    fits_get_num_hdus( fptr, &number_hdus, &status );
    if ( status != 0 ) {
	fits_report_error(stderr, status);
	fits_close_file( fptr, &status );
	return -1;
    }
    int number_images = 0;
    if ( number_hdus > 0 ) {
	int type = 0;
	fits_movabs_hdu( fptr, 1, &type, &status );
	if ( status != 0 ) {
	    fits_report_error(stderr, status);
	    fits_close_file( fptr, &status );
	    return -1;
	}
	if ( type == IMAGE_HDU ) ++number_images;
	for ( int i=2; i <= number_hdus; ++i ) {
	    type = 0;
	    fits_movrel_hdu(fptr, 1, &type, &status);
	    if ( status != 0 ) {
		fits_report_error(stderr, status);
		fits_close_file( fptr, &status );
		return -1;
	    }
	    if ( type == IMAGE_HDU ) ++number_images;
	}
    }

    fits_close_file( fptr, &status );
    if ( status != 0 ) {
	fits_report_error(stderr, status);
	return -1;
    }
    return number_images;
}


void QtDataManager::expandItem(QTreeWidgetItem* item) {
	if ( item->text(1) == "FITS Image" && item->childCount( ) == 1 ) {
		// check whether its a FITS image and prevent second generation children
		if(item!=0 && item->text(1)=="FITS Image" && !item->text(0).endsWith("]")){
			delete item->takeChild(0);

			QString path = dir_.path() + "/" +  item->text(0);

			// get a list of all extensions with data
			QStringList extList = analyseFITSImage(path);

			// if there is more than one extension
			if (extList.size()>1) {
				QTreeWidgetItem *childItem;
				int dType = uiDataType_[item->text(1)];

				// add the extensions as child items
				for (int j = 0; j < extList.size(); j+=2) {
					childItem = new QTreeWidgetItem(item);
					childItem->setText(0, extList.at(j));
					childItem->setText(1, extList.at(j+1));
					dType = uiDataType_[childItem->text(1)];
					childItem->setTextColor(1, getDirColor(dType));
				}
			}
			treeWidget_->resizeColumnToContents(0);
		}
	}
}


void QtDataManager::updateDirectory(QString str){
  QDir saved = dir_;
  if(!dir_.cd(str)) {
    QMessageBox::warning(this, tr("QtDataManager"),
    tr("No such directory:\n %1").arg(str));
    dir_ = saved;  }
  if(dir_.entryList().size() == 0) {
    QMessageBox::warning(this, tr("QtDataManager"),
    tr("Could not enter the directory:\n %1").arg(dir_.path()));
    dir_ = saved;  }
  dir_.makeAbsolute();
  dirLineEdit_->setText(dir_.cleanPath(dir_.path()));
  panel_->selectedDMDir = dirLineEdit_->text().toStdString();
  buildDirTree();
}


void QtDataManager::buildDirTree() {
   
  treeWidget_->clear();
  hideDisplayButtons();
  lelEdit_->deactivate();
  QStringList lbl;
  lbl << "Name" << "Type";
  treeWidget_->setColumnCount(2);
  treeWidget_->setHeaderLabels(lbl);

  QTreeWidgetItem *dirItem;
  dir_.makeAbsolute();
  QStringList entryList = dir_.entryList();

  // create and add an item for the home directory
  QString type = "Directory";
  int dType = uiDataType_[type];
  dirItem = new QTreeWidgetItem();
  dirItem->setIcon(0, QIcon(":/icons/home_folder.png"));
  dirItem->setToolTip(0, QString("Home directory"));
  dirItem->setText(1, type);
  dirItem->setTextColor(1, getDirColor(dType));
  treeWidget_->insertTopLevelItem (0, dirItem );

  // create and add an item for the root directory
  type = "Directory";
  dType = uiDataType_[type];
  dirItem = new QTreeWidgetItem();
  dirItem->setIcon(0, QIcon(":/icons/root_folder.png"));
  dirItem->setToolTip(0, QString("Root directory"));
  dirItem->setText(1, type);
  dirItem->setTextColor(1, getDirColor(dType));
  treeWidget_->insertTopLevelItem (1, dirItem );
  

  for (int i = 0; i < entryList.size(); i++) {
	  QString it = entryList.at(i);
	  if (it.compare(".") > 0) {
		  QString path = dir_.path() + "/" +  entryList.at(i);
		  type = panel_->viewer( )->fileType(path.toStdString()).chars();
		  dType = uiDataType_[type];
      //cout << "path=" << path.toStdString()
      //     << "type=" << type.toStdString() << " dType:"<<dType<<endl

		  if (dType!=UNKNOWN) {
			  dirItem = new QTreeWidgetItem(treeWidget_);
			  dirItem->setText(0, it);
			  dirItem->setText(1, type);
			  dirItem->setTextColor(1, getDirColor(dType));
			  if ( type == "FITS Image" && findNumberOfFITSImageExt( path ) > 1 ) {
				  QTreeWidgetItem *childItem = new QTreeWidgetItem(dirItem);
				  childItem->setText(0, "");
				  childItem->setText(1, "");
				  childItem->setTextColor(1, getDirColor(dType));
			  }
		  }
	  }
  }
	    
  // QSettings settings("NRAO", "casa");
  // //cout << "dir_.path()=" << dir_.path().toStdString() << endl;
  // settings.setValue("lastDir", dir_.path());

  treeWidget_->resizeColumnToContents(0);
  // treeWidget_->setColumnWidth(0, 200);   // (only suppotted in Qt 4.2+)

}


void QtDataManager::changeItemSelection(){
  QList<QTreeWidgetItem *> lst = treeWidget_->selectedItems();
  if (!lst.empty()) {
      lelEdit_->deactivate();
      QTreeWidgetItem *item = (QTreeWidgetItem*)(lst.at(0));
      showDisplayButtons(uiDataType_[item->text(1)],item->text(0));
      update_regrid_options( );
  }
}

    struct strip_chars {
	// should generalize to strip 'chars'...
	strip_chars( const char */*chars*/ ) { }
	strip_chars(const strip_chars &other) : str(other.str) { }
	operator std::string( ) const { return str; }
	void operator( )( const char &c ) { if ( c != '[' && c != ']' ) str += c; }
    private:
	std::string str;
    };

    struct max_ftor {
	max_ftor( ) : max(-FLT_MAX) { }
	max_ftor( const max_ftor &other ) : max(other.max) { }
	operator float( ) const { return max; }
	void operator( )( float f ) { if ( f > max ) max = f; }
    private:
	float max;
    };

    struct min_ftor {
	min_ftor( ) : min(FLT_MAX) { }
	min_ftor( const min_ftor &other ) : min(other.min) { }
	operator float( ) const { return min; }
	void operator( )( float f ) { if ( f < min ) min = f; }
    private:
	float min;
    };

  void QtDataManager::showDisplayButtons(int ddtp,const QString &name) {
  hideDisplayButtons();
  switch (ddtp) {
     case IMAGE :
        rasterButton_->show();
        contourButton_->show();
        vectorButton_->show();
        markerButton_->show();
	if ( treeWidget_->currentItem()->text(1) == "Image" )
		info_box->show();
	if ( ! name.isNull( ) ) {
	    std::string path = (dir_.path() + "/" + name).toStdString( );
	    if( imagePixelType(path) == TpFloat ) {
		fill_image_info( path );
	    }
// 	    else if(imagePixelType(path)==TpComplex) {
// 		PagedImage<Complex> image(path, TableLock::AutoNoReadLocking);
// 	    }

	}
        break;      
     case MEASUREMENT_SET :
        rasterButton_->show();
	ms_selection_box->show();
        break;
     case SKY_CATALOG:        
        catalogButton_->show();
        break;
     case RESTORE:        
        oldPanelButton_->show();
        newPanelButton_->show();
        break;
     case QUALIMG:
        rasterButton_->show();
        contourButton_->show();
        vectorButton_->show();
        markerButton_->show();
        break;
    case CASAREGION:
	if ( panel_->useNewRegions( ) ) {
	    if ( panel_->nDDs( ) == 0 )
		regionButton_->setDisabled(true);
	    else
		regionButton_->setDisabled(false);
	    regionButton_->show( );
	}
	break;
    case DS9REGION:
	if ( panel_->useNewRegions( ) ) {
	    if ( panel_->nDDs( ) == 0 )
		regionButton_->setDisabled(true);
	    else
		regionButton_->setDisabled(false);
	    regionButton_->show( );
	}
	break;
  }
}



QColor QtDataManager::getDirColor(int ddtp) {
    QColor clr;
    switch (ddtp) {
	case IMAGE:            clr = Qt::darkGreen;        break;
	case MEASUREMENT_SET:  clr = Qt::darkBlue;         break;
	case SKY_CATALOG:      clr = Qt::darkCyan;         break;
	case RESTORE:          clr = QColor(255,43,45);    break;
	case DIRECTORY:        clr = Qt::black;            break;
	case QUALIMG:          clr = Qt::darkRed;          break;
	case CASAREGION:       clr = Qt::darkYellow;       break;
	case DS9REGION:        clr = QColor(255,153,51);   break;
	case UNKNOWN: default: clr = Qt::darkMagenta;
    }
     
    return clr;
}


void QtDataManager::hideDisplayButtons(){
  rasterButton_->hide();
  contourButton_->hide();
  vectorButton_->hide();
  markerButton_->hide();
  catalogButton_->hide();
  oldPanelButton_->hide();
  newPanelButton_->hide();
  regionButton_->hide();
  ms_selection_box->hide();
  info_box->hide();
}


void QtDataManager::returnPressed(){
  QString str = dirLineEdit_->text();
  updateDirectory(str);
}



void QtDataManager::createButtonClicked() {

	QPushButton* button = dynamic_cast<QPushButton*>(sender());

	if(panel_==0 || button==0) return;

	String path, datatype, displaytype;

	displaytype = (displayType_.key(uiDisplayType_[button->text()])).toStdString();

	if(lelEdit_->isActive()) {

		// Display LEL expression.
		path = lelEdit_->text().trimmed().toStdString();
		datatype = "lel";

	} else if (treeWidget_->currentItem() > 0) {

		// Display selected file.
		path = (dir_.path() + "/" + treeWidget_->currentItem()->text(0)).toStdString();

		datatype = dataType_.value(uiDataType_[treeWidget_->currentItem()->text(1)]).toStdString();
	}

	if(path=="" || datatype=="" || displaytype=="") return;

	viewer::DisplayDataOptions ddo;
	if ( datatype == "ms" ) {
		if ( ms_selection->select_field->text( ) != "" )
			ddo.insert( "field", ms_selection->select_field->text( ).toStdString( ) );
		if ( ms_selection->select_spw->text( ) != "" )
			ddo.insert( "spw", ms_selection->select_spw->text( ).toStdString( ) );
		if ( ms_selection->select_time->text( ) != "" )
			ddo.insert( "time", ms_selection->select_time->text( ).toStdString( ) );
		if ( ms_selection->select_uvrange->text( ) != "" )
			ddo.insert( "uvrange", ms_selection->select_uvrange->text( ).toStdString( ) );
		if ( ms_selection->select_antenna->text( ) != "" )
			ddo.insert( "antenna", ms_selection->select_antenna->text( ).toStdString( ) );
		if ( ms_selection->select_scan->text( ) != "" )
			ddo.insert( "scan", ms_selection->select_scan->text( ).toStdString( ) );
		if ( ms_selection->select_corr->text( ) != "" )
			ddo.insert( "corr", ms_selection->select_corr->text( ).toStdString( ) );
		if ( ms_selection->select_array->text( ) != "" )
			ddo.insert( "array", ms_selection->select_array->text( ).toStdString( ) );
		if ( ms_selection->select_msexpr->text( ) != "" )
			ddo.insert( "msexpr", ms_selection->select_msexpr->text( ).toStdString( ) );
	}

	// pass along regridding information to QtDisplayPanelGui...
	if ( treeWidget_ != 0 && treeWidget_->currentItem( ) != 0 &&
	     dataType_.value(uiDataType_[treeWidget_->currentItem()->text(1)]) == "image" ) {
	    ddo.insert( "regrid", guimethod_to_iamethod(regrid_method->currentText( )) );
	}
	
	panel_->createDD( path, datatype, displaytype, True, ddo, image_properties );

	if(!leaveOpen_->isChecked()) close();  // (will hide dialog for now).
}


void QtDataManager::load_regions_clicked( ) {
    QPushButton* button = dynamic_cast<QPushButton*>(sender());

    if(panel_==0 || button==0) return;

    std::string path, datatype, displaytype;

    displaytype = (displayType_.key(uiDisplayType_[button->text()])).toStdString();

    if (treeWidget_->currentItem() > 0) {

	// Display selected file.
	path = (dir_.path() + "/" + treeWidget_->currentItem()->text(0)).toStdString();

	datatype = dataType_.value(uiDataType_[treeWidget_->currentItem()->text(1)]).toStdString();
    }

    if(path=="" || datatype=="" || displaytype=="") return;

    
    panel_->loadRegions( path, datatype, displaytype );

    if(!leaveOpen_->isChecked()) close();  // (will hide dialog for now).
}

//<drs> Duplicate code for this functionality in QtDBusViewerAdaptor::restore(...)
//      should try to find a way to make this available from QtViewer
void QtDataManager::restoreToOld_() {
  // Restore viewer state to existing panel.
  // Use the first empty panel, or if none, the first panel.
  
  List<QtDisplayPanelGui*> DPs = panel_->viewer()->openDPs();  
  ListIter<QtDisplayPanelGui*> dps(DPs);
  
  for(dps.toStart(); !dps.atEnd(); dps++) {
    QtDisplayPanelGui* dp = dps.getRight();
    if(dp->displayPanel()->registeredDDs().len()==0) {
      restoreTo_(dp->displayPanel());		// restore to first empty panel, if any...
      return;  }  }
  
  dps.toStart();
  if(!dps.atEnd()) {
    QtDisplayPanelGui* dp = dps.getRight();
    restoreTo_(dp->displayPanel());		// ...else, restore to first panel, if any...
    return;  }
     
  restoreToNew_();  }		// ...else, restore to a new panel.




//<drs> Duplicate code for this functionality in QtDBusViewerAdaptor::restore(...)
//      should try to find a way to make this available from QtViewer
void QtDataManager::restoreToNew_() {
  // Create new display panel, restore viewer state to it.
  panel_->viewer()->createDPG();
  
  List<QtDisplayPanelGui*> DPs = panel_->viewer()->openDPs();  
  if(DPs.len()>0) {				// (Safety: should be True)
    ListIter<QtDisplayPanelGui*> dps(DPs);
    dps.toEnd();
    dps--;					// Newly-created dp should be
    QtDisplayPanelGui* dp = dps.getRight();	// the last one on the list.
    
    restoreTo_(dp->displayPanel());  }  }


  
    
//<drs> Duplicate code for this functionality in QtDBusViewerAdaptor::restore(...)
//      should try to find a way to make this available from QtViewer
void QtDataManager::restoreTo_(QtDisplayPanel* dp) {
  // Restore viewer state to given panel.
  
  // Collect the name of the selected restore file.
  String filename = ( dir_.path() + "/" +
	              treeWidget_->currentItem()->text(0) ).toStdString();

  Bool ok = dp->restorePanelState(filename);
  
  if(ok && !leaveOpen_->isChecked()) close();  }
	// (will hide open dialog for now, if requested).

    
  
void QtDataManager::lelGotFocus_() {
  treeWidget_->clearSelection();
  info_box->hide( );
  showDisplayButtons(IMAGE);  }
  


void QtDataManager::showDDCreateError_(String errMsg) {
  // For now, just send to cerr.  (To do: put this on a status line).
  cerr<<endl<<errMsg<<endl;  }


QStringList QtDataManager::analyseFITSImage(QString path){
	 QString qdelim="<delim>";
	 QString qualMark="<qualimg>";
	 QString fitsMark="<fitsimg>";

	 QStringList typedExtlist;

	 // create a parser object and get the String information on
    // the extensions with data
    FITSImgParser fip = FITSImgParser(String(path.toStdString()));
    String extstring = fip.get_extlist_string("<delim>", "<qualimg>", "<fitsimg>");

    // convert the String to a QString;
    // split into a list of QStrings
    QString qextstring = QString(extstring.c_str());
    QStringList extlist = qextstring.split(qdelim, QString::SkipEmptyParts);

    // create a list with the type plus
    // the extension name
    for (int j = 0; j < extlist.size(); j++) {
   	 QString ext = extlist.at(j);
   	 if (ext.contains(qualMark)){
   		 typedExtlist << ext.remove(qualMark);
   		 typedExtlist << "Quality Ext.";
   	 }
   	 else if (ext.contains(fitsMark)){
   		 typedExtlist << ext.remove(fitsMark);
   		 typedExtlist << "FITS Ext.";
   	 }
   	 else {
   		 typedExtlist << ext;
   	 }
    }

    // return the QString list
    return typedExtlist;
}

Bool QtDataManager::isQualImg(const QString &/*extexpr*/){
	return True;
}

// finally, this string is examined by LatticeSlice1D<T>::stringToMethod,
// which only looks at the first letter...
std::string QtDataManager::guimethod_to_iamethod( const QString &type ) {
    if ( type == "bicubic" ) return "C";
    if ( type == "bilinear" ) return "L";
    if ( type == "nearest" ) return "N";
    return "";
}

 void QtDataManager::showlelButtonClicked( bool clicked ) {
     if ( clicked ) {
	lelGB_->show( );
	rc.put( "viewer." + panel_->rcid() + ".datamgr.show_lel", "true" );
     } else {
	lelGB_->hide( );
	rc.put( "viewer." + panel_->rcid() + ".datamgr.show_lel", "false" );
     }
 }

 void QtDataManager::leaveopenButtonClicked( bool clicked ) {
     if ( clicked ) {
	rc.put( "viewer." + panel_->rcid() + ".datamgr.leave_up", "true" );
     } else {
	rc.put( "viewer." + panel_->rcid() + ".datamgr.leave_up", "false" );
     }
 }


    void QtDataManager::update_regrid_options( ) {

	// start out with the regrid combo-box hidden...
	regrid->hide( );

	// hide regrid option unless an image file is selected...
	if ( treeWidget_ == 0 || treeWidget_->currentItem( ) == 0 ||
	     dataType_.value(uiDataType_[treeWidget_->currentItem()->text(1)]) != "image" )
	    return;

	QtDisplayData *cdd = 0;
	if ( panel_->nDDs( ) == 0 || (cdd = panel_->dd( )) == 0 ) return;

	const viewer::ImageProperties &cproperties = cdd->imageProperties( );
	if ( cproperties.ok( ) == false || cproperties.hasSpectralAxis( ) == false ) return;

	Vector<double> controlling_velo_range = cproperties.veloRange( "km/s" );
	if ( controlling_velo_range.size( ) != 2 ) return;

	// at this point the already-loaded image has a spectral axis...
	// show the combo-box, but disable it...
	regrid->show( );
	regrid_method->setCurrentIndex(0);
	regrid->setDisabled(true);

	Vector<double> new_velo_range = image_properties.veloRange( "km/s" );
	if ( new_velo_range.size( ) != 2 ) return;

	GenSort<double>::sort(new_velo_range);
	GenSort<double>::sort(controlling_velo_range);
	if ( (new_velo_range[0] <= controlling_velo_range[0] && new_velo_range[1] >= controlling_velo_range[0]) ||
	     (new_velo_range[0] <= controlling_velo_range[1] && new_velo_range[1] >= controlling_velo_range[1]) ||
	     (new_velo_range[0] >= controlling_velo_range[0] && new_velo_range[1] <= controlling_velo_range[1]) ) {
	    regrid->setDisabled(false);
	}

    }

    void QtDataManager::fill_image_info( const std::string &path ) {

	for ( infofield_list_t::iterator it = ifields.begin( ); it != ifields.end( ); ++it ) {
	    (*it).first->hide( );
	}

	image_properties = path;
	if ( image_properties.ok( ) == false ) return;

	infofield_list_t::iterator it = ifields.begin( );
	(*it).first->show( );
	(*it).first->setTitle("shape");
	std::ostringstream buf;
	buf << image_properties.shape( );
	std::string shape_str = buf.str( );
	(*it).second->setText(QString::fromStdString(std::for_each(shape_str.begin(),shape_str.end(),strip_chars("[]"))));
	buf.str("");
	(*it).second->setCursorPosition(0);
	++it;

	std::vector<std::string> beamvec = image_properties.restoringBeamAsStr( );
	if ( beamvec.size( ) == 3 ) {
	    (*it).first->show( );
	    (*it).first->setTitle("restoring beam");
	    std::string beam = beamvec[0] + ", " + beamvec[1] + ", " + beamvec[2];
	    (*it).second->setText(QString::fromStdString(beam));
	    (*it).second->setCursorPosition(0);
	    ++it;
	} else {
	    ++it;	// align ra/dec & freq/velo (may need to be made smarter)
	}

	if ( image_properties.hasDirectionAxis( ) ) {
	    std::vector<std::string> ra_range = image_properties.raRangeAsStr( );
	    std::vector<std::string> dec_range = image_properties.decRangeAsStr( );
	    if ( ra_range.size( ) == 2 && dec_range.size( ) == 2 ) {
		(*it).first->show( );
		(*it).first->setTitle(QString::fromStdString(image_properties.directionType( )) + " right ascension" );
		std::string ra_str = ra_range[0] + ", " + ra_range[1];
		(*it).second->setText(QString::fromStdString(ra_str));
		(*it).second->setCursorPosition(0);
		++it;
		(*it).first->show( );
		(*it).first->setTitle(QString::fromStdString(image_properties.directionType( )) + " declination" );
		std::string dec_str = dec_range[0] + ", " + dec_range[1];
		(*it).second->setText(QString::fromStdString(dec_str));
		(*it).second->setCursorPosition(0);
		++it;
	    } else {
		(*it).first->show( );
		(*it).first->setTitle("direction type");
		(*it).second->setText(QString::fromStdString(image_properties.directionType( )));
		(*it).second->setCursorPosition(0);
		++it;
	    }

	}

	if ( image_properties.hasSpectralAxis( ) ) {
	    if ( image_properties.freqRange().size( ) == 2 ) {
		(*it).first->show( );
		(*it).first->setTitle("frequency range");
		buf.str("");
		buf << image_properties.freqRange()[0] << ", " << image_properties.freqRange()[1] << " " << image_properties.freqUnits( );
		(*it).second->setText(QString::fromStdString(buf.str( )));
		(*it).second->setCursorPosition(0);
		++it;
	    }

	    if ( image_properties.veloRange().size( ) == 2 ) {
		(*it).first->show( );
		(*it).first->setTitle("velocity range");
		buf.str("");
		buf << image_properties.veloRange()[0] << ", " << image_properties.veloRange()[1] << " km/s" ;
		(*it).second->setText(QString::fromStdString(buf.str( )));
		(*it).second->setCursorPosition(0);
		++it;
	    }
	}

    }

} //# NAMESPACE CASA - END
