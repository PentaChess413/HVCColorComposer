/*
 *
 *	Copyright 2006 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#ifndef _H_MCColorSlider
#define _H_MCColorSlider

#include "Custom_Draw_Control.h"
#include "MCColorbook.h"

#if WIN32
#include <Process.h>
#endif

typedef enum
{
	kMC_MouseOutside = 0,
	kMC_MouseInTopButton,
	kMC_MouseInBottomButton,
	kMC_MouseInSlider	
} MCMouseLoc;


class MCColorSlider : public Custom_Draw_Control
{
	typedef Custom_Draw_Control	base;

	public:
		MCColorSlider( Dialog_Runner& dialog_runner, const ASInt32 control_id );
		virtual ~MCColorSlider();

	protected:
		virtual ASBoolean	HandleTracking( ADMTrackerRef tracker );
		virtual void		Draw( ADMDrawerRef environment );
		
		// these are called from HandleTracking
		bool				HandleButtonClick( ADMTrackerRef tracker, ASRect* buttonRect, bool downButton );
		bool				HandleSliderClick( ADMTrackerRef tracker, ASRect* sliderRect );

		// these are called from Draw()
		void				DrawArrow( ADMDrawerRef environment, ASRect* bounds, bool downArrow );
		void				DrawIndicators( ADMDrawerRef environment, ASRect* bounds, Int16 colorGroup );
		
		// used to select a new color
		void				SelectNewGroup( MCColorbookIndex );
		bool				SelectNextGroup( bool downButton );
		
		// where did the mouse land?
		MCMouseLoc			DetermineMouseLoc( ASPoint mousePt );
#if WIN32
		// timer routines for auto repeat
		static unsigned __stdcall	TimerThread( void* arg );
		HANDLE						mTimerHandle;
		uintptr_t					mTimerThread;
#endif

	private:
		MCMouseLoc		mMouseLoc;

		// we need state information otherwise we don't know which button we are pressing...
		bool			mDownButton;
};

#endif // !_H_MCColorSlider

// EOF
