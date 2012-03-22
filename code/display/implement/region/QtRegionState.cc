#include <fcntl.h>
#include <unistd.h>
#include <QDebug>
#include <QFileInfo>
#include <display/region/QtRegion.qo.h>
#include <display/region/QtRegionState.qo.h>
#include <casadbus/types/nullptr.h>
#include <QFileDialog>

namespace casa {
    namespace viewer {

	QtRegionState::freestat_list *QtRegionState::freestats = 0;

	void QtRegionState::init( ) {
	    QString cat = categories->tabText(categories->currentIndex( ));
	    if ( cat == "stats" )
		emit statisticsVisible( true );
	    else if ( cat == "properties" ) {
		QString state = states->tabText(categories->currentIndex( ));
		if ( state == "coordinates" )
		    emit positionVisible( true );
	    }
	}

	QtRegionState::QtRegionState( const QString &n, QtRegion *r, QWidget *parent ) :
					QFrame(parent), selected_statistics(-1), region_(r) {
	    setupUi(this);

	    //setLineWidth(0);
	    setFrameShape(QFrame::NoFrame);

	    coordinate_angle_box->hide( );

	    if ( freestats == 0 )
		freestats = new freestat_list( );

	    text_position->setWrapping(true);

	    region_type->setText(QApplication::translate("QtRegionState", n.toAscii( ).constData( ), 0, QApplication::UnicodeUTF8));

	    csys_box->hide( );

	    // update line characteristics...
	    connect( line_color, SIGNAL(currentIndexChanged(int)), SLOT(state_change(int)) );
	    connect( line_style, SIGNAL(currentIndexChanged(int)), SLOT(state_change(int)) );
	    connect( line_width, SIGNAL(valueChanged(int)), SLOT(state_change(int)) );
	    connect( text_position, SIGNAL(valueChanged(int)), SLOT(state_change(int)) );
	    connect( text_color, SIGNAL(currentIndexChanged(int)), SLOT(state_change(int)) );
	    connect( font_name, SIGNAL(currentIndexChanged(int)), SLOT(state_change(int)) );
	    connect( font_size, SIGNAL(valueChanged(int)), SLOT(state_change(int)) );
	    connect( x_off, SIGNAL(valueChanged(int)), SLOT(state_change(int)) );
	    connect( y_off, SIGNAL(valueChanged(int)), SLOT(state_change(int)) );
	    connect( font_italic, SIGNAL(clicked(bool)), SLOT(state_change(bool)) );
	    connect( font_bold, SIGNAL(clicked(bool)), SLOT(state_change(bool)) );
	    connect( region_mark, SIGNAL(stateChanged(int)), SLOT(state_change(int)) );

	    connect( save_file_name_browse, SIGNAL(clicked(bool)), SLOT(save_browser(bool)) );
	    connect( load_file_name_browse, SIGNAL(clicked(bool)), SLOT(load_browser(bool)) );

	    connect( save_now, SIGNAL(clicked(bool)), SLOT(save_region(bool)) );
	    connect( save_file_type, SIGNAL(currentIndexChanged(const QString&)), SLOT(update_default_file_extension(const QString&)) );
	    connect( save_filename, SIGNAL(textChanged(const QString&)), SLOT(update_save_type(const QString &)) );

	    connect( load_now, SIGNAL(clicked(bool)), SLOT(load_regions(bool)) );
	    connect( load_filename, SIGNAL(textChanged(const QString&)), SLOT(update_load_type(const QString &)) );

	    int z_max = region_->numFrames( );
	    frame_min->setMaximum(z_max);
	    frame_max->setMaximum(z_max);
	    frame_max->setValue(z_max);

	    connect( frame_min, SIGNAL(valueChanged(int)), SLOT(frame_min_change(int)) );
	    connect( frame_max, SIGNAL(valueChanged(int)), SLOT(frame_max_change(int)) );

	    connect( text, SIGNAL(textChanged(const QString&)), SLOT(state_change(const QString&)) );

	    connect( categories, SIGNAL(currentChanged(int)), SLOT(category_change(int)) );
	    connect( states, SIGNAL(currentChanged(int)), SLOT(states_change(int)) );

	    connect( coordinate_system, SIGNAL(currentIndexChanged(const QString &)), SLOT(coordsys_change(const QString &)) );
	    connect( x_units, SIGNAL(currentIndexChanged(int)), SLOT(states_change(int)) );
	    connect( y_units, SIGNAL(currentIndexChanged(int)), SLOT(states_change(int)) );
	    connect( dim_units, SIGNAL(currentIndexChanged(int)), SLOT(states_change(int)) );
	    connect( coordinates_reset, SIGNAL(clicked(bool)), SLOT(coordinates_reset_event(bool)) );
	    connect( coordinates_apply, SIGNAL(clicked(bool)), SLOT(coordinates_apply_event(bool)) );


	    last_line_color = line_color->currentText( );
	    connect( line_color, SIGNAL(currentIndexChanged(const QString&)), SLOT(line_color_change(const QString&)) );
	}

	QtRegionState::~QtRegionState( ) { }

	void QtRegionState::reset( const QString &n, QtRegion *r ) {
	    region_type->setText(QApplication::translate("QtRegionState", n.toAscii( ).constData( ), 0, QApplication::UnicodeUTF8));
	    region_ = r;

	    int z_max = region_->numFrames( );
	    frame_min->setMaximum(z_max);
	    frame_max->setMaximum(z_max);
	    frame_max->setValue(z_max);
	}

	void QtRegionState::updateStatistics( std::list<RegionInfo> *stats ) {
	    if ( stats == 0 || stats->size() == 0 ) return;

	    while ( stats->size() < statistics_group->count() ) {
		QtRegionStats *w = dynamic_cast<QtRegionStats*>(statistics_group->widget(0));
		if ( w == 0 ) throw internal_error( );
		statistics_group->removeWidget(w);
		freestats->push_back(w);
	    }
	    while ( stats->size() > statistics_group->count() ) {
		QtRegionStats *mystat;
		// BEGIN - critical section
		if ( freestats->size() > 0 ) {
		    mystat = freestats->back( );
		    freestats->pop_back( );
		    // END - critical section
		    mystat->reset( );
		} else {
		    mystat = new QtRegionStats( );
		}
		statistics_group->insertWidget(statistics_group->count( ),mystat);
	    }

	    int num = statistics_group->count( );
	    QtRegionStats *first = dynamic_cast<QtRegionStats*>(statistics_group->widget(0));
	    if ( first == 0 ) throw internal_error( );
	    std::list<RegionInfo>::iterator stat_iter = stats->begin();
	    if ( memory::nullptr.check(stat_iter->list( )) ) {
		// fprintf( stderr, "YESYES1YESYES1YESYES1YESYES1YESYES1YESYES1YESYES1YESYES1YESYES1YESYES1YESYES1YESYES1YESYES1YESYES1YESYES1YESYES1YESYES1YESYES1\n" );
	    } else {
		first->updateStatistics(*stat_iter);
	    }
	    if ( num < 2 ) return;

	    QtRegionStats *prev = first;

	    for ( int i=1; i < statistics_group->count() && ++stat_iter != stats->end(); ++i ) {
		QtRegionStats *cur = dynamic_cast<QtRegionStats*>(statistics_group->widget(i));
		if ( cur == 0 ) throw internal_error( );
		if ( memory::nullptr.check(stat_iter->list( )) ) {
		    // fprintf( stderr, "YESYES2YESYES2YESYES2YESYES2YESYES2YESYES2YESYES2YESYES2YESYES2YESYES2YESYES2YESYES2YESYES2YESYES2YESYES2YESYES2YESYES2YESYES2\n" );
		} else {
		    cur->updateStatistics(*stat_iter);
		}
		prev->setNext( statistics_group, cur );
		prev = cur;
	    }
	    prev->setNext( statistics_group, first );

	}

	void QtRegionState::clearStatistics( ) {
	    while ( statistics_group->count() > 0 ) {
		QtRegionStats *w = dynamic_cast<QtRegionStats*>(statistics_group->widget(0));
		if ( w == 0 ) throw internal_error( );
		statistics_group->removeWidget(w);
		freestats->push_back(w);
	    }
	}

	std::string QtRegionState::lineColor( ) const {
	    QString lc = line_color->currentText( );
	    return lc.toStdString( );
	}

	Region::LineStyle QtRegionState::lineStyle( ) const {
	    QString ls = line_style->currentText( );
	    if ( ls == "dashed" ) return Region::DashLine;
	    else if ( ls == "dotted" ) return Region::DotLine;
	    else return Region::SolidLine;
	}

	std::string QtRegionState::textColor( ) const {
	    QString tc = text_color->currentText( );
	    return tc.toStdString( );
	}

	std::string QtRegionState::textFont( ) const {
	    QString tfn = font_name->currentText( );
	    return tfn.toStdString( );
	}

	int QtRegionState::textFontStyle( ) const {
	    int result = 0;
	    if ( font_italic->isChecked( ) ) result = result | Region::ItalicText;
	    if ( font_bold->isChecked( ) ) result = result | Region::BoldText;
	    return result;
	}

	std::string QtRegionState::textValue( ) const {
	    QString txt = text->text( );
	    return txt.toStdString( );
	}

	Region::TextPosition QtRegionState::textPosition( ) const {
	    int pos = text_position->value( );
	    switch ( pos ) {
		case 1:
		    return Region::LeftText;
		case 2:
		    return Region::TopText;
		case 3:
		    return Region::RightText;
		default:
		    return Region::BottomText;
	    }
	}

	void QtRegionState::textPositionDelta( int &x, int &y ) const {
	    x = x_off->value( );
	    y = y_off->value( );
	}


	void QtRegionState::setTextValue( const std::string &l ) { text->setText( QString::fromStdString(l) ); }

	void QtRegionState::setTextFont( const std::string &f ) {
	    QString font( QString::fromStdString(f) );
	    for ( int i = 0; i < font_name->count( ); ++i ) {
		if ( ! font.compare( font_name->itemText(i), Qt::CaseInsensitive ) ) {
		    font_name->setCurrentIndex(i);
		    break;
		}
	    }
	}

	void QtRegionState::setTextFontSize( int s ) {
	    // limits specified in QtRegionState.ui
	    if ( s >= 7 && s <= 99 ) font_size->setValue(s);
	}

	void QtRegionState::setTextFontStyle( int s ) {
	    if ( s & Region::BoldText ) font_bold->setCheckState(Qt::Checked);
	    if ( s & Region::ItalicText ) font_italic->setCheckState(Qt::Checked);
	}

	void QtRegionState::setTextColor( const std::string &c ) {
	    QString color(QString::fromStdString(c));
	    for ( int i = 0; i < text_color->count( ); ++i ) {
		if ( ! color.compare( text_color->itemText(i), Qt::CaseInsensitive ) ) {
		    text_color->setCurrentIndex(i);
		    break;
		}
	    }
	}

	void QtRegionState::setLineColor( const std::string &c ) {
	    QString color(QString::fromStdString(c));
	    for ( int i = 0; i < line_color->count( ); ++i ) {
		if ( ! color.compare( line_color->itemText(i), Qt::CaseInsensitive ) ) {
		    line_color->setCurrentIndex(i);
		    break;
		}
	    }
	}

	void QtRegionState::setLineStyle( Region::LineStyle s ) {
	    switch ( s ) {
		case Region::SolidLine:
		    line_style->setCurrentIndex(0);
		    break;
		case Region::DashLine:
		    line_style->setCurrentIndex(1);
		    break;
		case Region::DotLine:
		    line_style->setCurrentIndex(2);
		    break;
	    }
	}


	int QtRegionState::zMin( ) const { return frame_min->value( ); }
	int QtRegionState::zMax( ) const { return frame_max->value( ); }
	int QtRegionState::numFrames( ) const { return region_->numFrames( ); }


	void QtRegionState::state_change( int ) { emit refreshCanvas( ); }
	void QtRegionState::state_change( bool ) { emit refreshCanvas( ); }
	void QtRegionState::state_change( const QString & ) { emit refreshCanvas( ); }

	void QtRegionState::line_color_change(const QString &s ) {
	    if ( text_color->currentText() == last_line_color &&
		 text_color->itemText(line_color->currentIndex()) == s ) {
		text_color->setCurrentIndex(line_color->currentIndex());
	    }
	    last_line_color = s;
	}

	void QtRegionState::update_save_type(const QString &txt) {
	    QFileInfo fi(txt);
	    if ( fi.suffix( ) == "reg" )
		save_file_type->setCurrentIndex(1);
	    else if ( fi.suffix( ) == "crtf" )
		save_file_type->setCurrentIndex(0);
	}

	void QtRegionState::update_load_type(const QString &txt) {
	    QFileInfo fi(txt);
	    if ( fi.exists( ) ) {
		const int buffer_size = 1024;
		char buffer[buffer_size];
		FILE *fh = fopen( txt.toAscii( ).constData( ), "r" );
		if ( fh ) {
		    if (fgets( buffer, buffer_size-1, fh )) {
			const char ds9header[] = "# Region file format: DS9";
			const char casaheader[] = "#CRTFv0 CASA Region Text Format";
			if ( strncmp(ds9header, buffer, sizeof(ds9header)-1) == 0 ) {
			    load_file_type->setCurrentIndex(1);
			} else if ( strncmp(casaheader, buffer, sizeof(casaheader)-1) == 0 ) {
			    load_file_type->setCurrentIndex(0);
			}
		    }
		    fclose(fh);
		}
	    }
	  
	}

	void QtRegionState::load_regions( bool ) {
	    QString path = load_filename->text( );
	    if ( path == "" ) {
		load_filename->setPlaceholderText(QApplication::translate("QtRegionState", "please enter a file name or use 'browse' button", 0, QApplication::UnicodeUTF8));
		load_now->setFocus(Qt::OtherFocusReason);
		return;
	    }

	    QFileInfo fi(path);
	    if ( ! fi.exists( ) ) {
		char *buf = (char*) malloc((strlen(path.toAscii( ).constData( )) + 50) * sizeof(char));
		sprintf( buf, "file '%s' does not exist", path.toAscii( ).constData( ) );
		load_filename->clear( );
		load_filename->setPlaceholderText(QApplication::translate("QtRegionState", buf, 0, QApplication::UnicodeUTF8));
		load_now->setFocus(Qt::OtherFocusReason);
		free(buf);
		return;	      
	    } else {
		int fd = open( path.toAscii( ).constData( ), O_RDONLY );
		if ( fd == -1 ) {
		    char *buf = (char*) malloc((strlen(path.toAscii( ).constData( )) + 50) * sizeof(char));
		    sprintf( buf, "could not read %s", path.toAscii( ).constData( ) );
		    load_filename->clear( );
		    load_filename->setPlaceholderText(QApplication::translate("QtRegionState", buf, 0, QApplication::UnicodeUTF8));
		    load_now->setFocus(Qt::OtherFocusReason);
		    free(buf);
		    return;
		} else {
		    ::close(fd);
		}
	    }

	    bool handled = false;
	    emit loadRegions( handled, path, load_file_type->currentText( ) );
	}

	static const char casa_ext[] = ".crtf";
	static const char ds9_ext[] = ".reg";
	static const char *default_ext = casa_ext;
	void QtRegionState::update_default_file_extension(const QString &txt) {
	    if ( txt.compare("CASA region file") == 0 ) {
		default_ext = casa_ext;
		csys_box->hide( );
	    } else if ( txt.compare("DS9 region file") == 0 ) {
		default_ext = ds9_ext;
		csys_box->show( );
	    }
	}

	QString QtRegionState::default_extension( const QString &base ) {
	    if ( base.contains('.') ) return base;
	    else return base + QString(default_ext);
	}

	void QtRegionState::save_region( bool ) {
	    QString path = save_filename->text( );
	    if ( path == "" ) {
		save_filename->setPlaceholderText(QApplication::translate("QtRegionState", "please enter a file name or use 'browse' button", 0, QApplication::UnicodeUTF8));
		save_now->setFocus(Qt::OtherFocusReason);
		return;
	    }

	    QString name = default_extension(path);

	    bool do_unlink = false;
	    int fd = open( name.toAscii( ).constData( ), O_WRONLY | O_APPEND );
	    if ( fd == -1 ) {
		fd = open( name.toAscii( ).constData( ), O_WRONLY | O_APPEND | O_CREAT, 0644 );
		if ( fd != -1 ) do_unlink = true;
	    }
	    if ( fd == -1 ) {
		char *buf = (char*) malloc((strlen(name.toAscii( ).constData( )) + 50) * sizeof(char));
		sprintf( buf, "unable to write to %s", name.toAscii( ).constData( ) );
		save_filename->clear( );
		save_filename->setPlaceholderText(QApplication::translate("QtRegionState", buf, 0, QApplication::UnicodeUTF8));
		save_now->setFocus(Qt::OtherFocusReason);
		free(buf);
		return;
	    } else {
		::close(fd);
		if ( do_unlink ) unlink(name.toAscii( ).constData( ));
	    }

	    QString what( save_current_region->isChecked( ) ? "current" :
			  save_marked_regions->isChecked( ) ? "marked" : "all" );
	    emit outputRegions( what, name, save_file_type->currentText( ), save_csys_type->currentText( ) );
	}

	void QtRegionState::category_change( int ) {
	    QString cat = categories->tabText(categories->currentIndex( ));
	    if ( cat == "stats" )
		emit statisticsVisible( true );
	    else
		emit statisticsVisible( false );
	}

	void QtRegionState::states_change( int ) {
	    // coordinates tab selected or not...
	    QString state = states->tabText(categories->currentIndex( ));
	    if ( state == "coordinates" )
		emit positionVisible( true );
	    else
		emit positionVisible( false );
	}

	void QtRegionState::coordsys_change( const QString &text ) {
	    // pixels are unitless...
	    if ( text == "pixel" ) {
		x_units->setDisabled(true);
		y_units->setDisabled(true);
		dim_units->setDisabled(true);
	    } else {
		x_units->setDisabled(false);
		y_units->setDisabled(false);
		dim_units->setDisabled(false);
	    }
	    // coordinates tab selected or not...
	    QString state = states->tabText(categories->currentIndex( ));
	    if ( state == "coordinates" )
		emit positionVisible( true );
	    else
		emit positionVisible( false );
	}

	void QtRegionState::coordinates_reset_event(bool) {
	    emit positionVisible(true);
	}

	void QtRegionState::coordinates_apply_event(bool) {
	    emit positionMove( center_x->displayText( ), center_y->displayText( ), coordinate_system->currentText( ),
			       x_units->currentText( ), y_units->currentText( ), bounding_width->displayText( ),
			       bounding_height->displayText( ), QString::fromStdString(bounding_index_to_string(dim_units->currentIndex( ))) );
	}

	void QtRegionState::frame_min_change( int v ) {
	    frame_max->setMinimum(v);
	    emit zRange( v, frame_max->value( ) );
	}
	void QtRegionState::frame_max_change( int v ) {
	    frame_min->setMaximum(v);
	    emit zRange( frame_min->value( ), v );
	}

	// invoked from QtRegionDock...
	void QtRegionState::justExposed( ) {
	    QString cat = categories->tabText(categories->currentIndex( ));
	    if ( cat == "stats" )
		emit statisticsVisible( true );
	    else
		emit statisticsVisible( false );
	}

	std::string QtRegionState::bounding_index_to_string( int index ) const {
	    switch ( index ) {
		case 0: return("rad");
		case 1: return("deg");
		case 2: return("arcsec");
		case 3: return("arcmin");
		case 4: return("pixel");
		default: return("rad");
	    }
	    return("rad");
	}

	void QtRegionState::getCoordinatesAndUnits( Region::Coord &c, Region::Units &xu, Region::Units &yu, std::string &bounding_units ) const {
	    switch ( coordinate_system->currentIndex( ) ) {
		case 0: c = Region::J2000; break;
		case 1: c = Region::B1950; break;
		case 2: c = Region::Galactic; break;
		case 3: c = Region::SuperGalactic; break;
		case 4: c = Region::Ecliptic; break;
		default: c = Region::J2000; break;
	    }
	    switch ( x_units->currentIndex( ) ) {
		case 0: xu = Region::Radians; break;
		case 1: xu = Region::Degrees; break;
		case 2: xu = Region::Sexagesimal; break;
		case 3: xu = Region::Pixel; break;
		default: xu = Region::Radians; break;
	    }
	    switch ( y_units->currentIndex( ) ) {
		case 0: yu = Region::Radians; break;
		case 1: yu = Region::Degrees; break;
		case 2: yu = Region::Sexagesimal; break;
		case 3: yu = Region::Pixel; break;
		default: yu = Region::Radians; break;
	    }

	    bounding_units = bounding_index_to_string(dim_units->currentIndex( ));

	}

	void QtRegionState::setCoordinatesAndUnits( Region::Coord c, Region::Units xu, Region::Units yu, const std::string &bounding_units ) {
	    switch( c ) {
		case Region::J2000: coordinate_system->setCurrentIndex( 0 ); break;
		case Region::B1950: coordinate_system->setCurrentIndex( 1 ); break;
		case Region::Galactic: coordinate_system->setCurrentIndex( 2 ); break;
		case Region::SuperGalactic: coordinate_system->setCurrentIndex( 3 ); break;
		case Region::Ecliptic: coordinate_system->setCurrentIndex( 4 ); break;
		default: coordinate_system->setCurrentIndex( 0 ); break;
	    }
	    switch ( xu ) {
		case Region::Radians: x_units->setCurrentIndex( 0 ); break;
		case Region::Degrees: x_units->setCurrentIndex( 1 ); break;
		case Region::Sexagesimal: x_units->setCurrentIndex( 2 ); break;
		case Region::Pixel: x_units->setCurrentIndex( 3 ); break;
		default: x_units->setCurrentIndex( 0 ); break;
	    }
	    switch ( yu ) {
		case Region::Radians: y_units->setCurrentIndex( 0 ); break;
		case Region::Degrees: y_units->setCurrentIndex( 1 ); break;
		case Region::Sexagesimal: y_units->setCurrentIndex( 2 ); break;
		case Region::Pixel: y_units->setCurrentIndex( 3 ); break;
		default: y_units->setCurrentIndex( 0 ); break;
	    }

	    if ( bounding_units == "deg" )
		dim_units->setCurrentIndex(1);
	    else if ( bounding_units == "arcsec" )
		dim_units->setCurrentIndex(2);
	    else if ( bounding_units == "arcmin" )
		dim_units->setCurrentIndex(3);
	    else if ( bounding_units == "pixel" )
		dim_units->setCurrentIndex(4);
	    else 
		dim_units->setCurrentIndex(0);
	}

	void QtRegionState::updatePosition( const QString &x, const QString &y, const QString &angle, const QString &width, const QString &height ) {
	    center_x->setText(x);
	    center_y->setText(y);
	    center_angle->setText(angle);
	    bounding_width->setText(width);
	    bounding_height->setText(height);
	}

	void QtRegionState::noOutputNotify( ) {
	    save_filename->clear( );
	    save_filename->setPlaceholderText(QApplication::translate("QtRegionState", "no regions were selected for output...", 0, QApplication::UnicodeUTF8));
	    save_now->setFocus(Qt::OtherFocusReason);
	}

	void QtRegionState::save_browser(bool) {
	    QString file = QFileDialog::getOpenFileName( this, "Save region file...", last_save_directory, QString("Region files (*") + casa_ext + " *" + ds9_ext + ")" );
	    if ( ! file.isEmpty() )
		save_filename->setText(file);
	}

	void QtRegionState::load_browser(bool) {
	    QString file = QFileDialog::getOpenFileName( this, "Load region file...", last_load_directory, QString("Region files (*") + casa_ext + " *" + ds9_ext + ")" );
	    if ( ! file.isEmpty() )
		load_filename->setText(file);
	}

    }
}

