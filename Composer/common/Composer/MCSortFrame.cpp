/*
 *
 *	Copyright 2006 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#include "MCSortFrame.h"
#include "Composer_UI.h"

#include "ADM_Access.h"


#if PROVERSION

void MCSortFrame::Draw( ADMDrawerRef environment )
{
	ASInt32 selectedItem = 0;
	
	// draw the outer frame...
	ADM_Access::item_suite()->DefaultDraw( native(), environment );

	ComposerUI* ui = GetComposerUI();
	if( !ui )
		return;

	ASRect ourBounds  = bounds();
	ASRect itemBounds = {0};
	
	// erase all the icon frames... (this way we don't have to maintain state)
	for( int i = PALETTE_ORDER_0; i < PALETTE_ORDER_0 + kNumSortIcons; i++ )
	{
		// draw frame -- expand it first
		itemBounds = ui->GetControl( i ).bounds();

		// we need to map this rectangle into our local coordinates as well...
		itemBounds.top    = itemBounds.top - ourBounds.top - 2;
		itemBounds.left   = itemBounds.left - ourBounds.left - 2;
		itemBounds.bottom = itemBounds.bottom - ourBounds.top;
		itemBounds.right  = itemBounds.right - ourBounds.left + 1;

		ADM_Access::drawing_suite()->SetADMColor( environment, kADMBackgroundColor );

		// draw the four lines
		ASPoint lineStart;
		ASPoint lineEnd;
		
		// top to bottom (left)
		lineStart.v = itemBounds.top;
		lineStart.h = itemBounds.left;
		lineEnd.h   = itemBounds.left;
		lineEnd.v   = itemBounds.bottom;
		ADM_Access::drawing_suite()->DrawLine( environment, &lineStart, &lineEnd );

		// top to bottom (right)
		lineStart.v = itemBounds.top;
		lineStart.h = itemBounds.right;
		lineEnd.h   = itemBounds.right;
		lineEnd.v   = itemBounds.bottom;
		ADM_Access::drawing_suite()->DrawLine( environment, &lineStart, &lineEnd );
		
		// left to right (top)
		lineStart.v = itemBounds.top;
		lineStart.h = itemBounds.left;
		lineEnd.h   = itemBounds.right;
		lineEnd.v   = itemBounds.top;
		ADM_Access::drawing_suite()->DrawLine( environment, &lineStart, &lineEnd );

		// left to right (bottom)
		lineStart.v = itemBounds.bottom;
		lineStart.h = itemBounds.left;
		lineEnd.h   = itemBounds.right;
		lineEnd.v   = itemBounds.bottom;
		ADM_Access::drawing_suite()->DrawLine( environment, &lineStart, &lineEnd );
	}
	
	
	// now determine which icon is selected
	selectedItem = PALETTE_ORDER_0 + GetComposerUI()->GetSortOrder();

	// assure correct index
	if( selectedItem < PALETTE_ORDER_0 || selectedItem > PALETTE_ORDER_5 )
		selectedItem = PALETTE_ORDER_0;
	
	// draw frame -- expand it first
	ASRect bounds = ui->GetControl( selectedItem ).bounds();

	// we need to map this rectangle into our local coordinates as well...
	bounds.top    = bounds.top - ourBounds.top - 2;
	bounds.left   = bounds.left - ourBounds.left - 2;
	bounds.bottom = bounds.bottom - ourBounds.top + 1;
	bounds.right  = bounds.right - ourBounds.left + 2;
	frame_draw( environment, bounds );
}

#endif // PROVERSION


// EOF
