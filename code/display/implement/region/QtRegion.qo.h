//# QtRegion.h: base class for statistical regions
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


#ifndef REGION_QTREGION_H_
#define REGION_QTREGION_H_

#include <display/region/QtRegionState.qo.h>
#include <display/region/Region.h>
#include <casa/BasicSL/String.h>
#include <list>

namespace casa {

    class AnnRegion;

    namespace viewer {

	class QtRegionDock;
	class QtRegionSource;
	class QtRegionState;

	// Key points:
	//    <ul>
	//        <li> regions are produced by a factory to permit the creation of gui specific regions </li>
	//    </ul>
	class QtRegion : public QObject {
	    Q_OBJECT
	    public:
		QtRegion( const QString &nme, QtRegionSource *factory );
		virtual ~QtRegion( );

		const std::string name( ) const { return name_.toStdString( ); }

		std::string lineColor( ) const { return mystate->lineColor( ); }
		int lineWidth( ) const { return mystate->lineWidth( ); }
		Region::LineStyle lineStyle( ) const { return mystate->lineStyle( ); }

		std::string textColor( ) const { return mystate->textColor( ); }
		std::string textFont( ) const { return mystate->textFont( ); }
		int textFontSize( ) const { return mystate->textFontSize( ); }
		int textFontStyle( ) const { return mystate->textFontStyle( ); }
		std::string textValue( ) const { return mystate->textValue( ); }
		Region::TextPosition textPosition( ) const { return mystate->textPosition( ); }
		void textPositionDelta( int &x, int &y ) const { return mystate->textPositionDelta( x, y ); }

		int numFrames( ) const;
		void zRange( int &x, int &y ) const;
		virtual int zIndex( ) const = 0;
		virtual bool regionVisible( ) const = 0;

		virtual void regionCenter( double &x, double &y ) const = 0;

		virtual void refresh( ) = 0;
		virtual AnnRegion *annotation( ) const = 0;

		// indicates that the user has selected this rectangle...
		void selectedInCanvas( );

		// indicates that region movement requires the update of state information...
		void updateStateInfo( bool region_modified );
		void clearStatistics( );

		virtual void getCoordinatesAndUnits( Region::Coord &c, Region::Units &u ) const = 0;
		virtual void getPositionString( std::string &x, std::string &y, std::string &angle,
						Region::Coord coord = Region::DefaultCoord,
						Region::Units units = Region::DefaultUnits ) const = 0;
		virtual void movePosition( const std::string &x, const std::string &y,
					   const std::string &coord, const std::string &units ) = 0;

	    public slots:
		/* void name( const QString &newname ); */
		/* void color( const QString &newcolor ); */
	    signals:
		/* void updated( ); */
		/* void deleted( const QtRegion * ); */

	    protected slots:
		void refresh_canvas_event( );
		void refresh_statistics_event( bool );
		void refresh_position_event( bool );
		void position_move_event( const QString &x, const QString &y, const QString &coord, const QString &units );
		void refresh_zrange_event(int,int);
		void revoke_region(QtRegionState*);
		void output(std::list<QtRegionState*>,std::ostream&);

	    protected:
		virtual Region::StatisticsList *generate_statistics_list( ) = 0;
		// At the base of this inheritance hierarchy is a class that uses
		// multiple inheritance. We are QtRegion is one base class, and we
		// need to be able to retrieve our peer (the non-GUI dependent)
		// Region class pointer...
		virtual Region *fetch_my_region( ) = 0;

		bool statistics_visible;
		bool statistics_update_needed;
		bool position_visible;
		bool position_update_needed;

		QtRegionSource *source_;
		QtRegionDock *dock_;
		QtRegionState *mystate;
		typedef std::list<QtRegionState*> freestate_list;
		static freestate_list *freestates;
		QString name_;
		QString color_;
	    private:
		bool z_index_within_range;
	};
    }
}

#endif
