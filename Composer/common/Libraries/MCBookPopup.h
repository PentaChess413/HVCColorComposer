/*
 *
 *	Copyright 2006 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#ifndef _H_MCBookPopup
#define _H_MCBookPopup

#include "Custom_Draw_Control.h"
#include "MCColorLibraries.h"


class MCBookPopup : public Custom_Draw_Control
{
	typedef Custom_Draw_Control	base;

	public:
		MCBookPopup( Dialog_Runner& dialog_runner, const ASInt32 control_id, MCColorLibraries& );
		virtual ~MCBookPopup();

	protected:
		virtual void		HandleNotify( ADMNotifierRef notifier );
		virtual void		Draw( ADMDrawerRef ) {};
		
		ASInt32				GetSelection();
	
	private:
		MCColorLibraries&	mBooks;
};

#endif


// EOF
