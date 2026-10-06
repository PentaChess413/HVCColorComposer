/*
 *
 *	Copyright 2006 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#ifndef _H_MCSortFrame
#define _H_MCSortFrame

#include "Custom_Draw_Control.h"
#include "MCColor.h"


class MCSortFrame : public Custom_Draw_Control
{
	typedef Custom_Draw_Control	base;

	public:
		MCSortFrame( Dialog_Runner& dialog_runner, const ASInt32 control_id ) : base( dialog_runner, control_id ) {}
		virtual ~MCSortFrame() {}
		
	protected:
		// overrides
		virtual void Draw( ADMDrawerRef environment );
};

#endif // !_H_MCSortFrame

// EOF
