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
#ifndef CANVASCURVE_H_
#define CANVASCURVE_H_

#include <casa/aips.h>
#include <casa/Arrays/Vector.h>
#include <vector>
#include <utility>			/***for std::pair***/
#include <QColor>
#include <QString>

/**
 * Represents a curve drawn on the QtCanvas.
 */

namespace casa {

	typedef std::vector<double> CurveData;
	typedef std::vector<double> ErrorData;

	class CanvasCurve {
	public:
		CanvasCurve();
		CanvasCurve( CurveData curveData, ErrorData errorData,
		             QString legend, QColor curveColor, int curveType );
		QColor getColor() const;
		void setColor( QColor color );
		QString getLegend() const;
		void setLegend( const QString& legend );
		int getCurveType() const;
		CurveData getCurveData();
		Vector<float> getXValues() const;
		Vector<float> getYValues() const;
		CurveData getErrorData();

		QString getToolTip( double x, double y , const double X_ERROR,
		                    const double Y_ERROR, const QString& xUnit, const QString& yUnit ) const;
		void getMinMax(Double& xmin, Double& xmax, Double& ymin,
		               Double& ymax, bool plotError ) const;
		std::pair<double,double> getRangeFor(double xMin, double xMax, Bool& exists );
		void scaleYValues( const QString& oldDisplayUnits, const QString& yUnitDisplay, const QString& xUnits );
		double convertValue( double value, double freqValue, const QString& oldDisplayUnits, const QString& yUnitDisplay, const QString& xUnits);
		virtual ~CanvasCurve();

	private:
		Vector<float> getErrorValues() const;
		double getMaxY() const;
		double getMaxError() const;
		void storeData( const QString& oldUnits );
		void setYValues( const Vector<float>& yValues );
		void setErrorValues( const Vector<float>& errorValues );

		QColor curveColor;
		QString legend;
		CurveData curveData;
		ErrorData errorData;
		QString maxUnits;
		double maxValue;
		double maxErrorValue;
		int curveType;
	};

} /* namespace casa */
#endif /* CANVASCURVE_H_ */
