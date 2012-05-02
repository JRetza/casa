//# QtRegionDock.cc: dockable Qt implementation of viewer region management
//# with surrounding Gui functionality
//# Copyright (C) 2011
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


#include <fstream>
#include <iostream>
#include <display/region/QtRegionDock.qo.h>
#include <display/region/QtRegionState.qo.h>
#include <display/ds9/ds9writer.h>
#include <display/DisplayErrors.h>
#include <display/QtViewer/QtDisplayPanelGui.qo.h>

namespace casa {
    namespace viewer {
	QtRegionDock::QtRegionDock( QtDisplayPanelGui *d, QWidget* parent ) :
					QDockWidget(parent), Ui::QtRegionDock( ),
					dpg(d), current_dd(0), current_tab_state(-1,-1),
					dismissed(false) {
	    setupUi(this);

	    // there are two standard Qt, dismiss icons...
	    // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
	    // dismissRegion->setIcon(style()->standardIcon(QStyle::SP_DockWidgetCloseButton));
	    dismiss_region->setIcon(style()->standardIcon(QStyle::SP_DialogCloseButton));

	    // Qt Designer will not allow you to create an empty QStackedWidget (qt4.7)... all created
	    // stacked widgets seem to have two elements... here we remove that elements (if they
	    // exists) because we want the elements to appear as they are created by the user...
	    while ( regions->count( ) > 0 ) {
		QWidget *c = regions->currentWidget( );
		regions->removeWidget(c);
		delete c;
	    }

	    connect( regions, SIGNAL(currentChanged(int)), SLOT(stack_changed(int)) );
	    connect( regions, SIGNAL(widgetRemoved(int)), SLOT(stack_changed(int)) );

	    connect( region_scroll, SIGNAL(valueChanged(int)), SLOT(change_stack(int)) );

	    connect( dismiss_region, SIGNAL(clicked(bool)), SLOT(delete_current_region(bool)) );
	    connect( this, SIGNAL(visibilityChanged(bool)), SLOT(handle_visibility(bool)) );
	    connect( regions, SIGNAL(currentChanged(int)), SLOT(emit_region_stack_change(int)) );

	}
  
	QtRegionDock::~QtRegionDock() {  }

	// void QtRegionDock::showStats( const QString &stats ) { }

	void QtRegionDock::addRegion(QtRegionState *state,int index) {
	    if ( index >= 0 ) {
		regions->insertWidget( index, state );
	    } else {
		regions->addWidget(state);
	    }
	    regions->setCurrentWidget(state);
	    connect( state, SIGNAL(outputRegions(const QString &,const QString &,const QString&,const QString&)), SLOT(output_region_event(const QString &,const QString &,const QString&,const QString&)) );
	    connect( state, SIGNAL(loadRegions(bool&,const QString &,const QString &)), SIGNAL(loadRegions(bool&,const QString &,const QString&)) );
	    connect( this, SIGNAL(region_stack_change(QWidget*)), state, SLOT(stackChange(QWidget*)) );

	    // not needed if the dock starts out as visible (or in user control)
	    // if ( ! isVisible( ) && dismissed == false ) {
	    // 	show( );
	    // }
	}

	int QtRegionDock::indexOf(QtRegionState *state) const {
	    return regions->indexOf(state);
	}

	void QtRegionDock::removeRegion(QtRegionState *state) {
	    if ( regions->indexOf(state) != -1 ) {
		// disconnect signals from 'state' to this object...
		disconnect( state, 0, this, 0 );
	    }
	    regions->removeWidget(state);

	    if ( regions->count( ) <= 0 && isVisible( ) ) hide( );
	}

	void QtRegionDock::selectRegion(QtRegionState *state) {
	    regions->setCurrentWidget(state);
	    state->nowVisible( );
	}

	void QtRegionDock::dismiss( ) {
	    hide( );
	    dismissed = true;
	}

	void QtRegionDock::change_stack( int index ) {
	    int size = regions->count( );
	    if ( index >= 0 && index <= size - 1 )
		regions->setCurrentIndex( index );
	}

	void QtRegionDock::updateRegionState( QtDisplayData *dd ) {
	    if ( dd == 0 && current_dd == 0 ) return;
	    if ( current_dd != 0 && dd == 0 )
		regions->hide( );
	    else if ( current_dd == 0 && dd != 0 )
		regions->show( );

	    for ( int i = 0; i < regions->count( ); ++i ) {
		QWidget *widget = regions->widget( i );
		QtRegionState *state = dynamic_cast<QtRegionState*>(widget);
		if ( state != 0 ) state->updateCoord( );
	    }

	    current_dd = dd;
	}

	void QtRegionDock::stack_changed( int index ) {

	    static int last_index = -1;
	    int size = regions->count( );
	    region_scroll->setMaximum( size > 0 ? size - 1 : 0 );
	    if ( size <= 0 ) {
		region_scroll->setEnabled(false);
		dismiss_region->setEnabled(false);
	    } else if ( size == 1 ) {
		region_scroll->setEnabled(false);
		dismiss_region->setEnabled(true);
	    } else {
		region_scroll->setEnabled(true);
		dismiss_region->setEnabled(true);
	    }
	    QWidget *current_widget = regions->currentWidget( );
	    if ( current_widget == 0 ) {
		// box is empty... e.g. after being deleted while updating
		region_scroll->setEnabled(false);
		dismiss_region->setEnabled(false);
		last_index = 0;
		return;
	    }

	    int current_index = regions->currentIndex( );
	    if ( current_index >= 0 )
		region_scroll->setValue(current_index);

	    QtRegionState *state = dynamic_cast<QtRegionState*>(current_widget);
	    if ( state == 0 )
		throw internal_error("region state corruption");
	    state->justExposed( );
	    last_index = index;
	}

	void QtRegionDock::delete_current_region(bool) {
	    emit deleteRegion(dynamic_cast<QtRegionState*>(regions->currentWidget( )));
	}

      void QtRegionDock::output_region_event( const QString &what, const QString &where, const QString &type, const QString &csys ) {
	    std::list<QtRegionState*> regionstate_list;
	    if ( what == "current" ) {
		// current region, only...
		QWidget *current_widget = regions->currentWidget( );
		QtRegionState *current = dynamic_cast<QtRegionState*>(current_widget);
		if ( current != 0 )
		    regionstate_list.push_back(current);
	    } else if ( what == "marked" ) {
		// all marked regions...
		for ( int i = 0; i < regions->count( ); ++i ) {
		    QWidget *widget = regions->widget( i );
		    QtRegionState *state = dynamic_cast<QtRegionState*>(widget);
		    if ( state != 0 && state->marked( ) )
			regionstate_list.push_back(state);
		}
	    } else {
		// ("all")... all regions...
		for ( int i = 0; i < regions->count( ); ++i ) {
		    QWidget *widget = regions->widget( i );
		    QtRegionState *state = dynamic_cast<QtRegionState*>(widget);
		    if ( state != 0 )
			regionstate_list.push_back(state);
		}
	    }

	    if ( regionstate_list.size( ) > 0 ) {
		if ( type == "CASA region file" ) {
		    AnnRegion::unitInit();
		    RegionTextList annotation_list;
		    try { emit saveRegions(regionstate_list,annotation_list); } catch (...) { return; }
		    ofstream sink;
		    sink.open(where.toAscii( ).constData( ));
		    annotation_list.print(sink);
		    sink.close( );
		} else if ( type == "DS9 region file" ) {
		    ds9writer writer(where.toAscii( ).constData( ),csys.toAscii( ).constData( ));
		    try { emit saveRegions(regionstate_list,writer); } catch (...) { return; }

		}
	    } else {
		QWidget *current_widget = regions->currentWidget( );
		QtRegionState *current = dynamic_cast<QtRegionState*>(current_widget);
		if ( current != 0 )
		    current->noOutputNotify( );
	    }
	}

	void QtRegionDock::handle_visibility( bool visible ) {
	    if ( visible && dismissed ) {
		dismissed = false;
		dpg->putrc( "visible.regiondock", "true" );
	    }
	}

	void QtRegionDock::emit_region_stack_change( int index ) {
	    emit region_stack_change(regions->widget(index));
	}

	void QtRegionDock::closeEvent ( QCloseEvent * event ) {
	    dismissed = true;
	    QDockWidget::closeEvent(event);
	    dpg->putrc( "visible.regiondock", "false" );
	}
    }
}
