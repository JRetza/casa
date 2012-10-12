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
#include "AnimatorHolder.qo.h"
#include <QDebug>

namespace casa {

const bool AnimatorHolder::BLINK_MODE = false;
const bool AnimatorHolder::NORMAL_MODE = true;

AnimatorHolder::AnimatorHolder(QWidget *parent)
    : QWidget(parent),
      animatorChannel( NULL ), animatorImage( NULL ),
      selectedColor( Qt::white)
{
	ui.setupUi(this);

	QPalette pal( palette());
	backgroundColor = pal.color( QPalette::Background );

	//Initialize the channel and image animators so they
	//are ready to go.
	initChannel();
	initImage();

	//Initially there are no images so we should not show
	//the animator.
	removeImageGroupBox();
	removeChannelGroupBox();

	previousMode = CHANNEL_MODE;
}

int AnimatorHolder::getAnimationCount() const {
	int count = 0;
	QLayout* layoutBase = this->layout();
	int channelIndex = layoutBase->indexOf( ui.channelGroupBox );
	int imageIndex = layoutBase->indexOf( ui.imageGroupBox );
	if ( channelIndex >= 0 ){
		count++;
	}
	if ( imageIndex >= 0 ){
		count++;
	}
	return count;
}

void AnimatorHolder::setHeightFixed() {
	int boxCount = getAnimationCount();
	const int BASE_HEIGHT = 83;
	int height = 20 + BASE_HEIGHT * boxCount;
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	setFixedHeight( height );
}


void AnimatorHolder::addChannelGroupBox(){
	QLayout* layout = this->layout();
	int channelGroupIndex = layout->indexOf( ui.channelGroupBox );
	if ( channelGroupIndex == -1 ){
		ui.channelGroupBox->setParent( this );
		//We want the channel group box to always be first
		//so we may need to remove the image group box and put it
		//in later if it is there.
		bool removedImage = removeImageGroupBox();
		layout->addWidget( ui.channelGroupBox );
		if ( removedImage ){
			addImageGroupBox();
		}
		setHeightFixed();
	}
}

void AnimatorHolder::addImageGroupBox(){
	QLayout* layout = this->layout();
	int imageGroupIndex = layout->indexOf( ui.imageGroupBox );
	if ( imageGroupIndex == -1 ){
		ui.imageGroupBox->setParent( this );
		layout->addWidget( ui.imageGroupBox );
		setHeightFixed();
	}
}

void AnimatorHolder::removeChannelGroupBox(){
	QLayout* layout = this->layout();
	int channelGroupIndex = layout->indexOf( ui.channelGroupBox );
	if ( channelGroupIndex >= 0 ){
		layout->removeWidget( ui.channelGroupBox );
		ui.channelGroupBox->setParent( NULL );
		setHeightFixed();
	}
}

bool AnimatorHolder::removeImageGroupBox(){
	QLayout* layout = this->layout();
	int imageGroupIndex = layout->indexOf( ui.imageGroupBox );
	bool removed = false;
	if ( imageGroupIndex >= 0 ){
		layout->removeWidget( ui.imageGroupBox );
		ui.imageGroupBox->setParent( NULL );
		setHeightFixed();
		removed = true;
	}
	return removed;
}

void AnimatorHolder::initChannel(){
	if ( animatorChannel == NULL ){
		animatorChannel = new AnimatorWidget( ui.channelGroupBox );
		animatorChannel->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed );
		animatorChannel->setModeEnabled( false );
		QHBoxLayout* layoutChannel = new QHBoxLayout();
		layoutChannel->setContentsMargins( 2,2,2,2 );
		layoutChannel->addWidget( animatorChannel );
		ui.channelGroupBox->setLayout( layoutChannel );
		ui.channelGroupBox->setAutoFillBackground( true );
		connect( ui.channelGroupBox, SIGNAL(clicked()), this, SLOT(modeChange()));

		connect(animatorChannel, SIGNAL(goTo(int)), this, SLOT(goToChannel(int)));
		connect(animatorChannel, SIGNAL(frameNumberEdited(int)), this, SLOT(frameNumberEditedChannel(int)));
		connect(animatorChannel, SIGNAL(setRate(int)), this, SLOT(setRateChannel(int)));
		connect(animatorChannel, SIGNAL(toStart()),  this, SLOT(toStartChannel()));
		connect(animatorChannel, SIGNAL(revStep()),  this, SLOT(revStepChannel()));
		connect(animatorChannel, SIGNAL(revPlay()), this, SLOT(revPlayChannel()));
		connect(animatorChannel, SIGNAL(stop()), this, SLOT(stopChannel()));
		connect(animatorChannel, SIGNAL(fwdPlay()), this, SLOT(fwdPlayChannel()));
		connect(animatorChannel, SIGNAL(fwdStep()), this, SLOT(fwdStepChannel()));
		connect(animatorChannel, SIGNAL(toEnd()), this, SLOT(toEndChannel()));
	}
	ui.channelGroupBox->setVisible( true );
}

void AnimatorHolder::initImage(){
	if ( animatorImage == NULL ){
		animatorImage = new AnimatorWidget( ui.imageGroupBox );
		animatorImage->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed );
		animatorImage->setModeEnabled( false );
		QHBoxLayout* layoutImage = new QHBoxLayout();
		layoutImage->setContentsMargins( 2,2,2,2 );
		layoutImage->addWidget( animatorImage );
		ui.imageGroupBox->setLayout( layoutImage );
		ui.imageGroupBox->setAutoFillBackground( true );

		connect( ui.imageGroupBox, SIGNAL(clicked()), this, SLOT(modeChange()));

		connect(animatorImage, SIGNAL(goTo(int)), this, SLOT(goToImage(int)));
		connect(animatorImage, SIGNAL(frameNumberEdited(int)), this, SLOT(frameNumberEditedImage(int)));
		connect(animatorImage,  SIGNAL(setRate(int)), this, SLOT(setRateImage(int)));
		connect(animatorImage, SIGNAL(toStart()), this, SLOT(toStartImage()));
		connect(animatorImage, SIGNAL(revStep()), this, SLOT(revStepImage()));
		connect(animatorImage, SIGNAL(revPlay()), this, SLOT(revPlayImage()));
		connect(animatorImage, SIGNAL(stop()), this, SLOT(stopImage()));
		connect(animatorImage, SIGNAL(fwdPlay()), this, SLOT(fwdPlayImage()));
		connect(animatorImage, SIGNAL(fwdStep()), this, SLOT(fwdStepImage()));
		connect(animatorImage, SIGNAL(toEnd()), this, SLOT(toEndImage()));
	}
	ui.imageGroupBox->setVisible( true );
}



//-----------------------------------------------------------------------
//                    Setters
//-----------------------------------------------------------------------

void AnimatorHolder::setModeEnabled( int imageCount ){
	int animationCount = getAnimationCount();
	bool modeChanged = false;

	if ( imageCount <= 0 ){
		if ( animationCount != 0 ){
			modeChanged = true;
			removeImageGroupBox();
			removeChannelGroupBox();
			animatorImage->setModeEnabled( false );
			animatorChannel->setModeEnabled( false );
		}
	}
	else if ( imageCount == 1 ){
		bool imageEnabled = ui.imageGroupBox->isCheckable();
		if ( imageEnabled ){
			modeChanged = true;
			removeImageGroupBox();
		}
		addRemoveChannelAnimatorBasedOnFrameCount();
	}
	else {
		addImageGroupBox();
		bool imageEnabled = ui.imageGroupBox->isCheckable();
		//Image mode is coming up after having been unavailable
		if ( !imageEnabled ){
			modeChanged = true;
			animatorImage->setModeEnabled( true );
			ui.imageGroupBox->setCheckable( true );
			ui.imageGroupBox->setChecked( false );
		}
		addRemoveChannelAnimatorBasedOnFrameCount();
	}
	if ( modeChanged ){
		modeChange();
	}
}

void AnimatorHolder::addRemoveChannelAnimatorBasedOnFrameCount(){
	if ( animatorChannel->getFrameCount() > 1 ){
			addChannelGroupBox();
			animatorChannel->setModeEnabled( true );
			ui.channelGroupBox->setCheckable( true );
			changePalette( ui.channelGroupBox, selectedColor );
	}
	else {
		removeChannelGroupBox();
	}
}

void AnimatorHolder::setFrameInformation( bool mode, int frm, int len ){
	if ( previousMode == CHANNEL_IMAGES_MODE && mode==NORMAL_MODE){
		//The number of frames should match channels
		//since it is scrolling through channels
		int frameCount = animatorChannel->getFrameCount();
		animatorChannel->setFrameInformation( frm, frameCount );
	}
	else if ( mode == NORMAL_MODE ){
		animatorChannel->setFrameInformation( frm, len );
		addRemoveChannelAnimatorBasedOnFrameCount();
	}
	else {
		animatorImage->setFrameInformation( frm, len );
	}
}


void AnimatorHolder::setRateInformation( bool mode, int minr, int maxr, int rate ){
	if ( previousMode != CHANNEL_IMAGES_MODE ){
		if ( mode == NORMAL_MODE ){
			animatorChannel->setRateInformation( minr, maxr, rate );
		}
		else {
			animatorImage->setRateInformation( minr, maxr, rate );
		}
	}
}

void AnimatorHolder::setPlaying( bool mode, int play ){
	if ( mode == BLINK_MODE ){
		if ( previousMode != CHANNEL_IMAGES_MODE ){
			animatorImage->setPlaying( play );
		}
	}
	else {
		animatorChannel->setPlaying( play );
	}
}

//--------------------------------------------------------------------------
//                       Accessors
//--------------------------------------------------------------------------

int AnimatorHolder::getRate( bool mode ) const {
	int rate = -1;
	if ( mode == NORMAL_MODE ){
		rate = animatorChannel->getRate();
	}
	else {
		rate = animatorImage->getRate();
	}
	return rate;
}


//-------------------------------------------------------------------------
//                        Signal/Slot
//-------------------------------------------------------------------------

//Channels

void AnimatorHolder::goToChannel(int frame){
	stopImagePlay();
	if ( previousMode == CHANNEL_MODE ){
		emit goTo( frame );
	}
	else {
		emit channelSelect( frame );
	}
}
void AnimatorHolder::setRateChannel(int rate){
	stopImagePlay();
	emit setRate( rate );
}
void AnimatorHolder::frameNumberEditedChannel( int frame ){
	stopImagePlay();
	if ( previousMode == CHANNEL_MODE ){
		emit frameNumberEdited( frame );
	}
	else {
		emit channelSelect( frame );
	}
}
void AnimatorHolder::toStartChannel(){
	stopImagePlay();
	if ( previousMode == CHANNEL_MODE ){
		emit toStart();
	}
	else {
		emit channelSelect(0);
	}
}
void AnimatorHolder::revStepChannel(){
	stopImagePlay();
	if ( previousMode == CHANNEL_MODE ){
		emit revStep();
	}
	else {
		int currentFrame = this->animatorChannel->getFrame();
		currentFrame = currentFrame-1;
		if ( currentFrame < 0 ){
			int frameCount = animatorChannel->getFrameCount();
			currentFrame = frameCount;
		}
		emit channelSelect( currentFrame );
	}
}
void AnimatorHolder::revPlayChannel(){
	stopImagePlay();
	if ( previousMode == CHANNEL_MODE ){
		emit revPlay();
	}
	else {
		animatorChannel->setPlaying( -1 );
		emitMovieChannels( false );

	}

}
void AnimatorHolder::fwdPlayChannel(){
	stopImagePlay();
	if ( previousMode == CHANNEL_MODE ){
		emit fwdPlay();
	}
	else {
		animatorChannel->setPlaying( 1 );
		emitMovieChannels( true );

	}
}
void AnimatorHolder::stopChannel(){
	stopImagePlay();
	if ( previousMode == CHANNEL_MODE ){
		emit stop();
	}
	else {
		animatorChannel->setPlaying( 0 );
		emit stopMovie();
	}
}
void AnimatorHolder::fwdStepChannel(){
	stopImagePlay();
	if ( previousMode == CHANNEL_MODE ){
		emit fwdStep();
	}
	else {
		int currentFrame = animatorChannel->getFrame();
		currentFrame++;
		emit channelSelect( currentFrame );
	}
}
void AnimatorHolder::toEndChannel(){
	stopImagePlay();
	if ( previousMode == CHANNEL_MODE ){
		emit toEnd();
	}
	else {
		int frameCount = this->animatorChannel->getFrameCount();
		emit channelSelect( frameCount );
	}
}

//Images

void AnimatorHolder::goToImage(int frame){
	stopChannelPlay();
	emit goTo( frame );
}
void AnimatorHolder::setRateImage(int rate){
	stopChannelPlay();
	emit setRate( rate );
}

void AnimatorHolder::frameNumberEditedImage( int frame ){
	stopChannelPlay();
	emit frameNumberEdited( frame );
}

void AnimatorHolder::toStartImage(){
	stopChannelPlay();
	emit toStart();
}
void AnimatorHolder::revStepImage(){
	stopChannelPlay();
	emit revStep();
}
void AnimatorHolder::revPlayImage(){
	stopChannelPlay();
	emit revPlay();
}

void AnimatorHolder::fwdPlayImage(){
	stopChannelPlay();
	emit fwdPlay();
}
void AnimatorHolder::stopImage(){
	emit stop();
}
void AnimatorHolder::fwdStepImage(){
	stopChannelPlay();
	emit fwdStep();
}
void AnimatorHolder::toEndImage(){
	stopChannelPlay();
	emit toEnd();
}

void AnimatorHolder::emitMovieChannels( bool direction ){
	int currentFrame = animatorChannel->getFrame();
	int frameCount = animatorChannel->getFrameCount();
	emit movieChannels( currentFrame, direction, frameCount );
}

void AnimatorHolder::stopImagePlay(){
	if ( animatorImage != NULL ){
		if ( animatorImage->isPlaying()){
			emit stop();
			animatorImage->setPlaying( 0 );
		}
	}
}

void AnimatorHolder::stopChannelPlay(){
	if ( animatorChannel != NULL ){
		if ( animatorChannel->isPlaying()){
			emit stop();
			animatorChannel->setPlaying( 0 );
		}
	}
}

//----------------------------------------------------------------
//                 Change the mode between channel, image, and
//                 channel_image
//----------------------------------------------------------------

void AnimatorHolder::modeChanged( Mode mode ){
	if ( mode != previousMode ){
		int rate = 0;
		int frame = 0;
		bool channelMode = NORMAL_MODE;
		bool channelImages = false;
		if ( mode == IMAGE_MODE ){
			animatorChannel->setPlaying( false );
			rate = animatorImage->getRate();
			frame = animatorImage->getFrame();
			channelMode = BLINK_MODE;
		}
		else if ( mode == CHANNEL_MODE  ){
			if ( animatorImage != NULL ){
				animatorImage->setPlaying( false );
			}
			if ( animatorChannel != NULL ){
				rate = animatorChannel->getRate();
				frame = animatorChannel->getFrame();
			}
		}
		else if ( mode == CHANNEL_IMAGES_MODE ){
			channelImages = true;
			animatorImage->setPlaying( false );
			rate = animatorChannel->getRate();
			frame = animatorChannel->getFrame();
			channelMode = BLINK_MODE;
		}
		emit setMode( channelMode );
		emit setRate( rate );
		emit goTo( frame );
		previousMode = mode;
	}
}

void AnimatorHolder::modeChange(){
	bool channelMode = ui.channelGroupBox->isChecked();
	bool imageMode = ui.imageGroupBox->isChecked();
	Mode mode = END_MODE;
	if ( channelMode && !imageMode ){
		mode = CHANNEL_MODE;
		changePalette( ui.channelGroupBox, selectedColor );
		changePalette( ui.imageGroupBox, backgroundColor );
	}
	else if ( !channelMode && imageMode ){
		mode = IMAGE_MODE;
		changePalette( ui.imageGroupBox, selectedColor );
		changePalette( ui.channelGroupBox, backgroundColor );
	}
	else if ( channelMode && imageMode ){
		mode = CHANNEL_IMAGES_MODE;
		changePalette( ui.channelGroupBox, selectedColor );
		changePalette( ui.imageGroupBox, selectedColor );
	}
	else {
		changePalette( ui.channelGroupBox, backgroundColor );
		changePalette( ui.imageGroupBox, backgroundColor );
	}
	if ( mode != END_MODE ){
		modeChanged( mode );
	}
}


void AnimatorHolder::changePalette( QGroupBox* box, QColor color ){
	QPalette pal = box->palette();
	pal.setColor( QPalette::Background, color );
	box->setPalette( pal );
}


AnimatorHolder::~AnimatorHolder(){

}
}
