/*
 *
 *	Copyright 2006 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#include "MCColorSlider.h"


#include "Custom_UI.h"


const long long kRepeatDelayTimeInNanos = 5000000;
const long		kRepeatPeriodInMillis	= 80;


MCColorSlider::MCColorSlider( Dialog_Runner& dialog_runner, const ASInt32 control_id ) :
	base( dialog_runner, control_id ),
	mMouseLoc( kMC_MouseOutside )
{
#if WIN32
	mTimerThread = NULL;
	mTimerHandle = ::CreateWaitableTimer( NULL, FALSE, NULL );
#endif	
}


MCColorSlider::~MCColorSlider()
{
#if WIN32
	if( mTimerThread  )
		::CloseHandle( (HANDLE)mTimerThread );
	
	if( mTimerHandle )
		::CloseHandle( mTimerHandle );
#endif
}


void MCColorSlider::Draw( ADMDrawerRef environment )
{
	// get access to UI
	CustomUI* ui = GetLibrariesUI();
	if( !ui )
		return;
		
	// get color book
	MCColorbook* book = ui->GetColorbook();
	if( !book )
		return;

	ASRect controlBounds = bounds( true );

	// make room for the top and bottom of the frame
	controlBounds.top += 2;
	controlBounds.bottom -= 2;
	
	// erase sides of the slider
	ASRect eraseRect = controlBounds;
	eraseRect.right = eraseRect.left + kIndicatorWidth;
	ADM_Access::drawing_suite()->ClearRect( environment, &eraseRect );
	
	// erase right side
	eraseRect = controlBounds;
	eraseRect.left = eraseRect.right - kIndicatorWidth;
	ADM_Access::drawing_suite()->ClearRect( environment, &eraseRect );

	
	// well get the selection, we need to know the group...
	Int16 group = book->GetGroup();
	
	// pick first group if we get back an invalid selection
	if( group < 0 )
		group = 0;

	// draw the two arrows
	ASRect arrowRect = controlBounds;
	arrowRect.top += 1;
	arrowRect.bottom = arrowRect.top + kButtonHeight + 1;
	DrawArrow( environment, &arrowRect, false );
	
	// move the rectangle down...
	arrowRect.bottom = controlBounds.bottom - 1;
	arrowRect.top = arrowRect.bottom - kButtonHeight - 1;
	DrawArrow( environment, &arrowRect, true ); 

	// draw the indicator -- clip out the buttons...
	ASRect indicatorRect = controlBounds;
	indicatorRect.top    += kButtonHeight + 1;
	indicatorRect.bottom -= kButtonHeight + 1;
	DrawIndicators( environment, &indicatorRect, group );

	// frame the slider...
	ASRGBColor frame_color = {0};	// black
	ADM_Access::drawing_suite()->SetRGBColor( environment, &frame_color );

	ASRect frameRect  = controlBounds;
	frameRect.top    += kButtonHeight + 1;
	frameRect.left   += kIndicatorWidth + 1;
	frameRect.right  -= kIndicatorWidth + 1;
	frameRect.bottom -= kButtonHeight + 1;
	ADM_Access::drawing_suite()->DrawRect( environment, &frameRect );

	// draw the color groups
	ASRect insetRect = frameRect;
	++insetRect.top;
	++insetRect.left;
	--insetRect.right;
	--insetRect.bottom;
	book->DrawGroups( environment, &insetRect );
	
	// now draw last frame
	insetRect = controlBounds;
	insetRect.left  += kIndicatorWidth;
	insetRect.right -= kIndicatorWidth;
	ADM_Access::drawing_suite()->DrawSunkenRect( environment, &insetRect );
}

void MCColorSlider::DrawArrow( ADMDrawerRef environment, ASRect* bounds, bool downArrow )
{
	if( !bounds )
		return;
		
	ASRect	 controlBounds = *bounds;
	ADMColor arrowColor    = kADMBlackColor;

	// adjust controlBounds -- center control and account for indicators
	controlBounds.left  += kIndicatorWidth + 1;
	controlBounds.right -= kIndicatorWidth + 1;

	// erase inside of arrow...
	ADM_Access::drawing_suite()->ClearRect( environment, &controlBounds );

	// figure out whether or not to draw the arrow greyed out
	if( !IsEnabled() )
		arrowColor = kADMDisabledColor;
	
	ADM_Access::drawing_suite()->SetADMColor( environment, arrowColor );
	
	// figure out which way to draw...
	if( downArrow )
		ADM_Access::drawing_suite()->DrawDownArrow( environment, &controlBounds );
	else
		ADM_Access::drawing_suite()->DrawUpArrow( environment, &controlBounds );
		
	// now draw frame
	ASRGBColor frame_color = {0};	// black
	ADM_Access::drawing_suite()->SetRGBColor( environment, &frame_color );

	// draw outer frame this is also the frame for the button
	ADM_Access::drawing_suite()->DrawRect( environment, &controlBounds );

	// draw the rasied bevel frame for the button
	ASRect insetRect = controlBounds;
	++insetRect.top;
	++insetRect.left;
	--insetRect.bottom;
	--insetRect.right;
	ADM_Access::drawing_suite()->DrawRaisedRect( environment, &insetRect );
}


void MCColorSlider::DrawIndicators( ADMDrawerRef environment, ASRect* bounds, Int16 colorGroup )
{
	ASRect	 controlBounds = *bounds;
	ADMColor arrowColor    = kADMBlackColor;
	Int32    height		   = controlBounds.bottom - controlBounds.top;
	
	// we need access to the color book
	CustomUI* ui = GetLibrariesUI();
	if( !ui )
		return;
		
	// get color book
	MCColorbook* book = ui->GetColorbook();
	if( !book )
		return;
	
	// map color group to position on slider
	SpFloat ratio =  height / (SpFloat)book->GetNumGroups();
	Int32 mappedPosition = (Int32)(colorGroup * ratio); 
	
	// clamp value
	if( mappedPosition < 0 )
		mappedPosition = 0;

	if( mappedPosition >= height )
		mappedPosition = height - 1;
	
	// account for frame at top
 	Int32	halfHeight  = (Int32)((book->GetElementHeightFloat( bounds ) / 2.0f) + 0.5f);
	ASRect	draw_bounds = { controlBounds.left, (controlBounds.top - (kIndicatorHeight / 2)) + mappedPosition + halfHeight };

	// setup position on vertical for arrows
	draw_bounds.bottom = draw_bounds.top + kIndicatorHeight;
	
	// Draw the left indicator arrow
	draw_bounds.right = draw_bounds.left + kIndicatorWidth;
	ADM_Access::drawing_suite()->DrawRightArrow( environment, &draw_bounds );

	// Draw the right indicator arrow -- account for deep frame
	draw_bounds.right = controlBounds.right - 1;
	draw_bounds.left  = draw_bounds.right - kIndicatorWidth;
	ADM_Access::drawing_suite()->DrawLeftArrow( environment, &draw_bounds );
}


ASBoolean MCColorSlider::HandleTracking( ADMTrackerRef tracker )
{
	bool    clickHandled    = false;
	ASRect	sliderBounds    = bounds( true );
	ASRect	topButtonBounds = sliderBounds;
	ASRect	botButtonBounds = sliderBounds;
	
	// top button
	topButtonBounds.bottom = topButtonBounds.top + kButtonHeight;
	
	// bottom button
	botButtonBounds.top = botButtonBounds.bottom - kButtonHeight;

	// make room for the top and bottom buttons and the frame
	sliderBounds.top    += kButtonHeight + 1;
	sliderBounds.bottom -= kButtonHeight + 1;

	// get where the mouse is now...
	ASPoint	mousePt;
	ADM_Access::tracking_suite()->GetPoint( tracker, &mousePt );

	// keep UI state -- when the mouse button goes down we find out where we are
	ASBoolean mouseDown   = ADM_Access::tracking_suite()->TestAction( tracker, kADMButtonDownAction );
	MCMouseLoc currentLoc = DetermineMouseLoc( mousePt );
	
#if WIN32
	// if we got a mouse up, cancel any timer procs we may have...	
	if( mTimerHandle && ADM_Access::tracking_suite()->TestAction( tracker, kADMButtonUpAction ) )
	{
		// kill the timer
		::CancelWaitableTimer( mTimerHandle );
		
		// kill the thread
		::CloseHandle( (HANDLE)mTimerThread );
		mTimerThread = NULL;
	}
#endif

	// remember where we initially clicked in case the mouse leaves that location	
	if( mouseDown )
		mMouseLoc = currentLoc;
	
	// if we aren't still in the same place we clicked we should just bail
	if( mMouseLoc != currentLoc )
		return false;
	
	// if we are still here it's safe to pass on the event to our handlers.	
	switch( currentLoc )
	{
		case kMC_MouseInTopButton:
			clickHandled = HandleButtonClick( tracker, &topButtonBounds, false );
			break;
			
		case kMC_MouseInBottomButton:
			clickHandled = HandleButtonClick( tracker, &botButtonBounds, true );
			break;
			
		case kMC_MouseInSlider:
			clickHandled = HandleSliderClick( tracker, &sliderBounds );
			break;
	}
		
	return clickHandled;
}


MCMouseLoc MCColorSlider::DetermineMouseLoc( ASPoint mousePt )
{
	ASRect	sliderBounds    = bounds( true );
	ASRect	topButtonBounds = sliderBounds;
	ASRect	botButtonBounds = sliderBounds;
	
	// top button
	topButtonBounds.bottom = topButtonBounds.top + kButtonHeight;
	
	// bottom button
	botButtonBounds.top = botButtonBounds.bottom - kButtonHeight;

	// make room for the top and bottom buttons and the frame
	sliderBounds.top    += kButtonHeight + 1;
	sliderBounds.bottom -= kButtonHeight + 1;

	// determine where the mouse was clicked.
	if( topButtonBounds.top <= mousePt.v && topButtonBounds.left <= mousePt.h 
	&&	topButtonBounds.bottom >= mousePt.v && topButtonBounds.right >= mousePt.h )
	{
		return kMC_MouseInTopButton;
	}
	else if( botButtonBounds.top <= mousePt.v && botButtonBounds.left <= mousePt.h 
	&&	botButtonBounds.bottom >= mousePt.v && botButtonBounds.right >= mousePt.h )
	{
		return kMC_MouseInBottomButton;
	}
	else if( sliderBounds.top <= mousePt.v && sliderBounds.left <= mousePt.h 
	&&	sliderBounds.bottom >= mousePt.v && sliderBounds.right >= mousePt.h )
	{
		return kMC_MouseInSlider;
	}
	
	return kMC_MouseOutside;
}


// here is the game plan.  When the user clicks the mouse, we light off a timer (that we kill at mouse up). 
// this timer waits for a split second before it starts "auto-repeating" the click.
bool MCColorSlider::HandleButtonClick( ADMTrackerRef tracker, ASRect* buttonRect, bool downButton )
{
	ASBoolean mouseDown = ADM_Access::tracking_suite()->TestAction( tracker, kADMButtonDownAction );

	if( !mouseDown )
		return false;

	bool result = SelectNextGroup( downButton );

#if WIN32
	if( !mTimerHandle )
		return true;
	
	LARGE_INTEGER waitTime;
	
	waitTime.QuadPart = -kRepeatDelayTimeInNanos;	// must be negative for relative time (in nanoseconds)
	BOOL timerCreated = ::SetWaitableTimer( mTimerHandle, &waitTime, kRepeatPeriodInMillis, NULL, NULL, false );
	
	if( timerCreated )
	{
		// we need state information otherwise we don't know which button we are pressing...
		mDownButton = downButton;
	
		// this is the thread continuously repeats the action needed...
		mTimerThread = ::_beginthreadex( 0, 0, TimerThread, this, 0, 0 );
	}
#endif
	
	return result;
}


bool MCColorSlider::HandleSliderClick( ADMTrackerRef tracker, ASRect* sliderRect )
{
	ASBoolean mouseDown = ADM_Access::tracking_suite()->TestAction( tracker, kADMButtonDownAction );
	ASBoolean mouseDrag = ADM_Access::tracking_suite()->TestAction( tracker, kADMMouseMovedDownAction );

	if( !mouseDown && !mouseDrag )
		return false;

	// get access to UI
	CustomUI* ui = GetLibrariesUI();
	if( !ui )
		return false;
		
	// get color book
	MCColorbook* book = ui->GetColorbook();
	if( !book )
		return false;

	// ask the color book if we selected anything...
	MCColorbookIndex result = {0, -1};

	ASPoint	mousePt;
	ADM_Access::tracking_suite()->GetPoint( tracker, &mousePt );

	result = book->PickGroup( sliderRect, &mousePt );
	SelectNewGroup( result );

	return true;
}



void MCColorSlider::SelectNewGroup( MCColorbookIndex newIndex )
{
	// get access to UI
	CustomUI* ui = GetLibrariesUI();
	if( !ui )
		return;
		
	// get color book
	MCColorbook* book = ui->GetColorbook();
	if( !book )
		return;

	// see what the current book is
	Int16 oldGroup = book->GetGroup();

	// check for valid and not the same as the current selection
	if( book->IsValidIndex( newIndex ) && oldGroup != newIndex.colorGroup )
	{
		// pick that group in the color book...
		book->SetGroup( newIndex.colorGroup );
		
		// redraw the arrow position
		ui->InvalidateControl( CustomUI::COLOR_SLIDER );

		// redraw the list...
		ui->InvalidateControl( CustomUI::COLOR_LIST );
		
		// force everything to draw NOW
		ADM_Access::dialog_suite()->Update( ui->GetNative() );
	}
}


bool MCColorSlider::SelectNextGroup( bool downButton )
{
	// get access to UI
	CustomUI* ui = GetLibrariesUI();
	if( !ui )
		return false;
		
	// get color book
	MCColorbook* book = ui->GetColorbook();
	if( !book )
		return false;

	// see what the current book is
	Int16 oldGroup = book->GetGroup();

	// try to select the next or previous group...
	MCColorbookIndex result = {0, -1};
	
	if( downButton )
		result.colorGroup = oldGroup + 1;
	else	
		result.colorGroup = oldGroup - 1;
	
	// clamp
	if( result.colorGroup < 0 )
		result.colorGroup = 0;
	if( result.colorGroup >= book->GetNumGroups() )
		result.colorGroup = book->GetNumGroups() - 1;
		
	SelectNewGroup( result );
	
	return true;
}


#if WIN32
unsigned MCColorSlider::TimerThread( void* arg ) 
{
	MCColorSlider* me = (MCColorSlider*)arg;
	
	if( !me )
		return 0;
	
	// get the timer handle
	HANDLE timer = me->mTimerHandle;
	
	if( !timer )
		return 0;
		
	// go into a wait loop so that the timer always fires on time...
	while( 1 ) 
	{
		// wait for the timer
		::WaitForSingleObject( timer, INFINITE );
		
		// go ahead and increment slider by one- we pass which button was clicked
		me->SelectNextGroup( me->mDownButton );
	}
	
	return 0;
}
#endif // WIN32


// EOF
