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

#include "RangeControlsWidget.qo.h"
#include <QDoubleValidator>

namespace casa {

RangeControlsWidget::RangeControlsWidget(QWidget *parent)
    : QWidget(parent) {
	ui.setupUi(this);

	minMaxValidator = new QDoubleValidator( std::numeric_limits<double>::min(),
		std::numeric_limits<double>::max(), 10, this );
	ui.minLineEdit->setValidator( minMaxValidator );
	ui.maxLineEdit->setValidator( minMaxValidator );
	connect( ui.minLineEdit, SIGNAL(textChanged(const QString&)), this, SIGNAL(minMaxChanged()));
	connect( ui.maxLineEdit, SIGNAL(textChanged(const QString&)), this, SIGNAL(minMaxChanged()));
	connect( ui.clearRangeButton, SIGNAL(clicked()), this, SLOT(clearRange()));
}

void RangeControlsWidget::setRange( double min, double max ){
	ui.minLineEdit->setText( QString::number( min ));
	ui.maxLineEdit->setText( QString::number( max ));
}

void RangeControlsWidget::setRangeLimits( double min, double max ){
	minMaxValidator->setBottom( min );
	minMaxValidator->setTop( max );
}

void RangeControlsWidget::clearRange(){
	ui.minLineEdit->setText( "" );
	ui.maxLineEdit->setText( "" );
	emit rangeCleared();
}

pair<double,double> RangeControlsWidget::getMinMaxValues() const {
	QString minValueStr = ui.minLineEdit->text();
	QString maxValueStr = ui.maxLineEdit->text();
	double minValue = minValueStr.toDouble();
	double maxValue = maxValueStr.toDouble();
	if ( minValue > maxValue ){
		double tmp = minValue;
		minValue = maxValue;
		maxValue = tmp;
	}
	pair<double,double> maxMinValues(minValue,maxValue);
	return maxMinValues;
}

void RangeControlsWidget::setDataLimits( std::vector<float> values ){
	double min = numeric_limits<float>::max();
	double max = numeric_limits<float>::min();
	for ( int i = 0; i < static_cast<int>(values.size()); i++ ){
		if ( min > values[i] ){
			min = values[i];
		}
		if ( max > values[i] ){
			max = values[i];
		}
	}
	ui.dataMinLineEdit->setText( QString::number( min ) );
	ui.dataMaxLineEdit->setText( QString::number( max ) );
}

RangeControlsWidget::~RangeControlsWidget(){

}
}
