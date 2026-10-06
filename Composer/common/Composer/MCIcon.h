/*
 *
 *	Copyright 2006 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#ifndef _H_MCIcon
#define _H_MCIcon

#include "Custom_Draw_Control.h"
#include "MCColor.h"


class MCIcon : public Custom_Draw_Control
{
	typedef Custom_Draw_Control	base;

	public:
		MCIcon( Dialog_Runner& dialog_runner, const ASInt32 control_id ) : base( dialog_runner, control_id ) {}
		virtual ~MCIcon() {}
		
	protected:
		// overrides
		virtual ASBoolean	HandleTracking( ADMTrackerRef tracker );
		virtual void		Draw( ADMDrawerRef environment );
};

#endif // !_H_MCIcon

// EOF
