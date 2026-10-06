/*
 *
 *	Copyright 2005 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#ifndef Control_Runner_h
#define Control_Runner_h

#include "Control_Interface.h"

class ComposerUI;
class CustomUI;

class Control_Runner : public Control_Interface
{
	typedef Control_Interface	base;

	public:
		Control_Runner( Dialog_Runner& dialog_runner, const ASInt32 control_id );
		virtual	~Control_Runner() = 0;

		virtual void		Update() = 0;
		
		// shortcut routine to get at main UI
		ComposerUI*			GetComposerUI() const;
		CustomUI*			GetLibrariesUI() const;

		// utility function...
		static Control_Runner*	GetControlFromNative( ADMItemRef p_control );

	protected:
		virtual ASBoolean		HandleTracking( ADMTrackerRef tracker );
		virtual void			HandleNotify( ADMNotifierRef notifier );

		static ASAPI ASBoolean 	TrackingCallback( ADMItemRef p_control, ADMTrackerRef inTracker );
		static ASAPI void 		NotifyCallback( ADMItemRef p_control, ADMNotifierRef notifier );
		static ASAPI void 		DestroyCallback( ADMItemRef p_control );
};

#endif

// EOF
