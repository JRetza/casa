#include <display/region/Ellipse.h>
#include <display/Display/WorldCanvas.h>
#include <display/Display/PixelCanvas.h>

#include <display/Display/WorldCanvasHolder.h>
#include <display/Display/PanelDisplay.h>
#include <display/DisplayDatas/PrincipalAxesDD.h>
#include <images/Regions/WCEllipsoid.h>
#include <images/Images/SubImage.h>

#include <imageanalysis/Annotations/AnnEllipse.h>
#include <coordinates/Coordinates/CoordinateUtil.h>

namespace casa {
    namespace viewer {

	Ellipse::~Ellipse( ) { }


	AnnotationBase *Ellipse::annotation( ) const {

	    if ( wc_ == 0 || wc_->csMaster() == 0 ) return 0;

	    const CoordinateSystem &cs = wc_->coordinateSystem( );

	    double wx, wy;
	    try { linear_to_world( wc_, (blc_x + trc_x) / 2.0, (blc_y + trc_y) / 2.0, wx, wy ); } catch(...) { return 0; }
	    const Vector<String> &units = wc_->worldAxisUnits( );

	    Quantity qx( wx, units[0] );
	    Quantity qy( wy, units[1] );

	    Quantity minor, major, rot;
	    double wblc_x, wblc_y, wtrc_x, wtrc_y;
	    try { linear_to_world( wc_, blc_x, blc_y, trc_x, trc_y, wblc_x, wblc_y, wtrc_x, wtrc_y ); } catch(...) { return 0; }
	    // The position angle (rot) is the angle between north and the major axis of the
	    // ellipse, measured to the east (clockwise in standard astronomical coordinates
	    //  where the longitude increases with decreasing x).
	    if ( trc_y - blc_y > trc_x - blc_x ) {
		rot = Quantity(0,"deg");
		minor = Quantity( fabs(wtrc_x - wblc_x), units[0] );
		major = Quantity( fabs(wtrc_y - wblc_y), units[1] );
	    } else {
		rot = Quantity(90,"deg");
		major = Quantity( fabs(wtrc_x - wblc_x), units[0] );
		minor = Quantity( fabs(wtrc_y - wblc_y), units[1] );
	    }

	    const DisplayData *dd = wc_->displaylist().front();

	    Vector<Stokes::StokesTypes> stokes;
	    Int polaxis = CoordinateUtil::findStokesAxis(stokes, cs);

	    AnnEllipse *ellipse = 0;
	    try {
		std::vector<int> axes = dd->displayAxes( );
		IPosition shape(cs.nPixelAxes( ));
		for ( int i=0; i < shape.size( ); ++i )
		    shape(i) = dd->dataShape( )[axes[i]];
		ellipse = new AnnEllipse( qx, qy, major, minor, rot, cs, shape, stokes );
	    } catch ( AipsError &e ) {
		cerr << "Error encountered creating an AnnEllipse:" << endl;
		cerr << "\t\"" << e.getMesg( ) << "\"" << endl;
	    } catch ( ... ) {
		cerr << "Error encountered creating an AnnEllipse..." << endl;
	    }

	    return ellipse;
	}

	void Ellipse::fetch_region_details( RegionTypes &type, std::vector<std::pair<int,int> > &pixel_pts, 
					    std::vector<std::pair<double,double> > &world_pts ) const {

	    if ( wc_ == 0 || wc_->csMaster() == 0 ) return;

	    type = EllipseRegion;
	    RegionTypes x;
	    Rectangle::fetch_region_details( x, pixel_pts, world_pts );
	}


	void Ellipse::drawRegion( bool selected ) {
	    if ( wc_ == 0 || wc_->csMaster() == 0 ) return;

	    PixelCanvas *pc = wc_->pixelCanvas();
	    if(pc==0) return;

	    double center_x, center_y;
	    regionCenter( center_x, center_y );

	    int x1, y1, x2, y2;
	    int cx, cy;
	    try { linear_to_screen( wc_, blc_x, blc_y, trc_x, trc_y, x1, y1, x2, y2 ); } catch(...) { return; }
	    try { linear_to_screen( wc_, center_x, center_y, cx, cy ); } catch(...) { return; }

	    pc->drawEllipse(cx, cy, cx - x1, cy - y1, 0.0, True, 1.0, 1.0);

	    if ( selected ) {

		// draw outline rectangle for resizing the ellipse...
		pushDrawingEnv(DotLine);
		pc->drawRectangle( x1, y1, x2, y2 );
		popDrawingEnv( );

		Int w = x2 - x1;
		Int h = y2 - y1;

		Int s = 0;		// handle size
		if (w>=35 && h>=35) s = 6;
		else if (w>=20 && h>=20) s = 4;
		else if (w>= 9 && h>= 9) s = 3;

		double xdx, ydy;
		try { screen_to_linear( wc_, x1 + s, y1 + s, xdx, ydy ); } catch(...) { return; }
		handle_delta_x = xdx - blc_x;
		handle_delta_y = ydy - blc_y;

		int hx0 = x1;
		int hx1 = x1 + s;
		int hx2 = x2 - s;
		int hx3 = x2;
		int hy0 = y1;
		int hy1 = y1 + s;
		int hy2 = y2 - s;
		int hy3 = y2;	// set handle coordinates
		if (s) {
		    pushDrawingEnv( Region::SolidLine);
		    if ( marked( ) ) {
			pc->drawRectangle(hx0, hy0 - 0, hx1 + 0, hy1 + 0);
			pc->drawRectangle(hx2, hy0 - 0, hx3 + 0, hy1 + 0);
			pc->drawRectangle(hx0, hy2 - 0, hx1 + 0, hy3 + 0);
			pc->drawRectangle(hx2, hy2 - 0, hx3 + 0, hy3 + 0);
		    } else {
			pc->drawFilledRectangle(hx0, hy0 - 0, hx1 + 0, hy1 + 0);
			pc->drawFilledRectangle(hx2, hy0 - 0, hx3 + 0, hy1 + 0);
			pc->drawFilledRectangle(hx0, hy2 - 0, hx1 + 0, hy3 + 0);
			pc->drawFilledRectangle(hx2, hy2 - 0, hx3 + 0, hy3 + 0);
		    }
		  popDrawingEnv( );
		}

	    }

	}

	unsigned int Ellipse::mouseMovement( double x, double y, bool other_selected ) {
	    unsigned int result = 0;

	    if ( visible_ == false ) return result;

	    //if ( x >= blc_x && x <= trc_x && y >= blc_y && y <= trc_y ) {
	    if ( x > blc_x && x < trc_x && y > blc_y && y < trc_y ) {
		result |= MouseSelected;
		result |= MouseRefresh;
		selected_ = true;
		draw( other_selected );
		if ( other_selected == false ) {
		    // mark flag as this is the region (how to mix in other shapes)
		    // of interest for statistics updates...
		    selectedInCanvas( );
		}
	    } else if ( selected_ == true ) {
		selected_ = false;
		draw( other_selected );
		result |= MouseRefresh;
	    }
	    return result;
	}

	std::list<RegionInfo> *Ellipse::generate_dds_centers( bool skycomp ){
		std::list<RegionInfo> *region_centers = new std::list<RegionInfo>( );

		if( wc_==0 ) return region_centers;

		Int zindex = 0;
		if (wc_->restrictionBuffer()->exists("zIndex")) {
			wc_->restrictionBuffer()->getValue("zIndex", zindex);
		}

		double blcx, blcy, trcx, trcy;
		boundingRectangle( blcx, blcy, trcx, trcy );

		DisplayData *dd = 0;
		const std::list<DisplayData*> &dds = wc_->displaylist( );
		Vector<Double> lin(2), blc(2), center(2);

		lin(0) = blcx;
		lin(1) = blcy;
		if ( ! wc_->linToWorld(blc, lin)) return region_centers;

		double center_x, center_y;
		regionCenter( center_x, center_y );
		lin(0) = center_x;
		lin(1) = center_y;
		if ( ! wc_->linToWorld(center, lin)) return region_centers;

		std::string errMsg_;
		std::map<String,bool> processed;
		for ( std::list<DisplayData*>::const_iterator ddi=dds.begin(); ddi != dds.end(); ++ddi ) {
			dd = *ddi;

			PrincipalAxesDD* padd = dynamic_cast<PrincipalAxesDD*>(dd);
			if (padd==0) continue;

			try {
				if ( ! padd->conformsTo(*wc_) ) continue;

				ImageInterface<Float> *image = padd->imageinterface( );

				if ( image == 0 ) continue;

				String full_image_name = image->name(false);
				std::map<String,bool>::iterator repeat = processed.find(full_image_name);
				if (repeat != processed.end()) continue;
				processed.insert(std::map<String,bool>::value_type(full_image_name,true));

				Int nAxes = image->ndim( );
				IPosition shp = image->shape( );
				const CoordinateSystem &cs = image->coordinates( );

				int zIndex = padd->activeZIndex( );
				IPosition pos = padd->fixedPosition( );
				Vector<Int> dispAxes = padd->displayAxes( );

				if ( nAxes == 2 ) dispAxes.resize(2,True);

				if ( nAxes < 2 || Int(shp.nelements()) != nAxes ||
						Int(pos.nelements()) != nAxes ||
						anyLT(dispAxes,0) || anyGE(dispAxes,nAxes) )
					continue;

				if ( dispAxes.nelements() > 2u )
					pos[dispAxes[2]] = zIndex;

				dispAxes.resize(2,True);

				// select the visible layer in the third and all
				// hidden axes with a WCBox and a SubImage
				Quantum<Double> px0(0.,"pix");
				Vector<Quantum<Double> > blcq(nAxes,px0), trcq(nAxes,px0);
				for (Int ax = 0; ax < nAxes; ax++) {
					if ( ax == dispAxes[0] || ax == dispAxes[1]) {
						trcq[ax].setValue(shp[ax]-1);
					} else  {
						blcq[ax].setValue(pos[ax]);
						trcq[ax].setValue(pos[ax]);
					}
				}
				WCBox box(blcq, trcq, cs, Vector<Int>());
				ImageRegion     *imgbox = new ImageRegion(box);
			 	SubImage<Float> *boxImg = new SubImage<Float>(*image, *imgbox);

			 	// generate the WCEllipsoide
				//Quantum<Double> px0(0.,"pix");
				Vector<Quantum<Double> > centerq(2,px0), radiiq(2,px0);
				const Vector<String> &units = wc_->worldAxisUnits( );

				centerq[0].setValue(center[0]);
				centerq[0].setUnit(units[0]);
				centerq[1].setValue(center[1]);
				centerq[1].setUnit(units[1]);

				Quantum<Double> _blc_1_(blc[0],units[0]);
				radiiq[0] = centerq[0] - _blc_1_;
				radiiq[0].setValue(fabs(radiiq[0].getValue( )));

				Quantum<Double> _blc_2_(blc[1],units[1]);
				radiiq[1] = centerq[1] - _blc_2_;
				radiiq[1].setValue(fabs(radiiq[1].getValue( )));

				cout << "centerq: " << centerq << endl;
				cout << "radiiq:  " << radiiq << endl;
				// This is a 2D ellipse (which is the same sort of ellipse that is created via
				// the new annotaitons). I don't know how one creates an elliptical column (which
				// extends the 2D ellipse through all spectral channels) that is analogous to
				// what is done for rectangles... must consult the delphic oracle when the
				// need arises... <drs>
				WCEllipsoid ellipse( centerq, radiiq, IPosition(dispAxes), cs);
				ImageRegion *imageregion = new ImageRegion(ellipse);

				//region_centers->push_back(ImageRegionInfo(full_image_name,getLayerCenter(padd,image,*imageregion,skycomp)));
				region_centers->push_back(ImageRegionInfo(full_image_name,getLayerCenter(padd,boxImg,*imageregion,skycomp)));
				delete imgbox;
				delete imageregion;
				delete boxImg;
			} catch (const casa::AipsError& err) {
				errMsg_ = err.getMesg();
				fprintf( stderr, "Ellipse::generate_dds_centers( ): %s\n", errMsg_.c_str() );
				continue;
			} catch (...) {
				errMsg_ = "Unknown error converting region";
				fprintf( stderr, "Ellipse::generate_dds_centers( ): %s\n", errMsg_.c_str() );
				continue;
			}
		}
		return region_centers;
	}

	std::list<RegionInfo> *Ellipse::generate_dds_statistics(  ) {
	    std::list<RegionInfo> *region_statistics = new std::list<RegionInfo>( );

	    if( wc_==0 ) return region_statistics;

	    Int zindex = 0;
	    if (wc_->restrictionBuffer()->exists("zIndex")) {
		wc_->restrictionBuffer()->getValue("zIndex", zindex);
	    }

	    double blcx, blcy, trcx, trcy;
	    boundingRectangle( blcx, blcy, trcx, trcy );

	    DisplayData *dd = 0;
	    const std::list<DisplayData*> &dds = wc_->displaylist( );
	    Vector<Double> lin(2), blc(2), center(2);

	    lin(0) = blcx;
	    lin(1) = blcy;
	    if ( ! wc_->linToWorld(blc, lin)) return region_statistics;

	    double center_x, center_y;
	    regionCenter( center_x, center_y );
	    lin(0) = center_x;
	    lin(1) = center_y;
	    if ( ! wc_->linToWorld(center, lin)) return region_statistics;

	    std::string errMsg_;
	    std::map<String,bool> processed;
	    for ( std::list<DisplayData*>::const_iterator ddi=dds.begin(); ddi != dds.end(); ++ddi ) {
		dd = *ddi;

		PrincipalAxesDD* padd = dynamic_cast<PrincipalAxesDD*>(dd);
		if (padd==0) continue;

		try {
		    if ( ! padd->conformsTo(*wc_) ) continue;

		    ImageInterface<Float> *image = padd->imageinterface( );

		    if ( image == 0 ) continue;

		    String full_image_name = image->name(false);
		    std::map<String,bool>::iterator repeat = processed.find(full_image_name);
		    if (repeat != processed.end()) continue;
		    processed.insert(std::map<String,bool>::value_type(full_image_name,true));

		    Int nAxes = image->ndim( );
		    IPosition shp = image->shape( );
		    const CoordinateSystem &cs = image->coordinates( );

		    int zIndex = padd->activeZIndex( );
		    IPosition pos = padd->fixedPosition( );
		    Vector<Int> dispAxes = padd->displayAxes( );

		    if ( nAxes == 2 ) dispAxes.resize(2,True);

		    if ( nAxes < 2 || Int(shp.nelements()) != nAxes ||
			 Int(pos.nelements()) != nAxes ||
			 anyLT(dispAxes,0) || anyGE(dispAxes,nAxes) )
			continue;

		    if ( dispAxes.nelements() > 2u )
			pos[dispAxes[2]] = zIndex;

		    dispAxes.resize(2,True);

		    // WCBox dummy;
		    Quantum<Double> px0(0.,"pix");
		    Vector<Quantum<Double> > centerq(2,px0), radiiq(2,px0);
		    const Vector<String> &units = wc_->worldAxisUnits( );

		    centerq[0].setValue(center[0]);
		    centerq[0].setUnit(units[0]);
		    centerq[1].setValue(center[1]);
		    centerq[1].setUnit(units[1]);

		    Quantum<Double> _blc_1_(blc[0],units[0]);
		    radiiq[0] = centerq[0] - _blc_1_;
		    radiiq[0].setValue(fabs(radiiq[0].getValue( )));

		    Quantum<Double> _blc_2_(blc[1],units[1]);
		    radiiq[1] = centerq[1] - _blc_2_;
		    radiiq[1].setValue(fabs(radiiq[1].getValue( )));

		    // This is a 2D ellipse (which is the same sort of ellipse that is created via
		    // the new annotaitons). I don't know how one creates an elliptical column (which
		    // extends the 2D ellipse through all spectral channels) that is analogous to
		    // what is done for rectangles... must consult the delphic oracle when the
		    // need arises... <drs>
		    WCEllipsoid ellipse( centerq, radiiq, IPosition(dispAxes), cs);
		    ImageRegion *imageregion = new ImageRegion(ellipse);

		    region_statistics->push_back(ImageRegionInfo(full_image_name,getLayerStats(padd,image,*imageregion)));
		    delete imageregion;

		} catch (const casa::AipsError& err) {
		    errMsg_ = err.getMesg();
		    continue;
		} catch (...) {
		    errMsg_ = "Unknown error converting region";
		    continue;
		}
	    }
	    return region_statistics;
	}

    }

}
