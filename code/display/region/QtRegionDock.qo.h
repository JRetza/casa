//# QtRegionDock.qo.h: dockable Qt implementation of viewer region management
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
//# $Id$

#ifndef REGION_QTREGIONDOCK_H_
#define REGION_QTREGIONDOCK_H_
#include <iostream>
#include <display/region/QtRegionDock.ui.h>
#include <imageanalysis/Annotations/AnnRegion.h>
#include <imageanalysis/Annotations/RegionTextList.h>

namespace casa {

    class QtDisplayData;
    class QtDisplayPanelGui;

    namespace viewer {

	class QtRegionState;
	class ds9writer;

	class QtRegionDock : public QDockWidget, protected Ui::QtRegionDock {
	    Q_OBJECT
	    public:

		QtRegionDock( QtDisplayPanelGui *, QWidget* parent=0 );
		~QtRegionDock();

		void addRegion(QtRegionState*,int index = -1);
		int indexOf(QtRegionState*) const;
		void removeRegion(QtRegionState*);
		void selectRegion(QtRegionState*);

		/* QStackedWidget *regionStack( ) { return regions; } */

		/* void showStats( const QString &stats ); */

		std::pair<int,int> &tabState( ) { return current_tab_state; }
		std::map<std::string,int> &coordState( ) { return current_coord_state; }
		QString &saveDir( ) { return current_save_dir; }
		QString &loadDir( ) { return current_load_dir; }

		void dismiss( );

	    signals:
		void deleteRegion(QtRegionState*);
		void deleteAllRegions( );
		void saveRegions( std::list<QtRegionState*>, RegionTextList & );
		void saveRegions( std::list<QtRegionState*>, ds9writer & );
		void loadRegions( bool &handled, const QString &path, const QString &type );

		void region_stack_change(QWidget*);

	    public slots:
		void updateRegionState(QtDisplayData*);

	    private slots:
		void stack_changed(int);
		void change_stack(int);
		void delete_current_region(bool);
		void delete_all_regions(bool);
		void output_region_event(const QString &what, const QString &where, const QString &type, const QString &csys );
		void handle_visibility(bool);
		void emit_region_stack_change( int );

	    protected:
		void closeEvent ( QCloseEvent * event );

	    private:
		QtDisplayPanelGui *dpg;
		QtDisplayData *current_dd;
		std::pair<int,int> current_tab_state;
		std::map<std::string,int> current_coord_state;
		QString current_save_dir;
		QString current_load_dir;
		bool dismissed;

	};
    }
}

#endif
