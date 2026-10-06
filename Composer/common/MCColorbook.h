/*
 *
 *	Copyright 2006 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */


#ifndef _H_MCColorbook
#define _H_MCColorbook

//////////////////////////////////////////////////////////////////////////////////////////////
//
// Includes
//
//////////////////////////////////////////////////////////////////////////////////////////////


#include "DynaArray.h"
#include "MCColor.h"
#include "RVNameField.h"

#include <stdio.h>



//////////////////////////////////////////////////////////////////////////////////////////////
//
// Defines
//
//////////////////////////////////////////////////////////////////////////////////////////////

typedef struct tagColorEntry
{
	RVWideNameField name;		// real color name
	RVNameField		hash;		// name used for searching
	MCColor			color;		// color components
} MCColorEntry;


typedef struct tagColorIndex
{
	Int16	colorIndex;		// what index is this color in this group?
	Int16	colorGroup;		// what group does this color belong to...?
} MCColorbookIndex;


typedef CDynamicArray<MCColorEntry>    MCColorEntries;
typedef CDynamicArray<MCColorEntries*> MCColorGroups;


const Int32 kIndicatorHeight = 8;
const Int32 kIndicatorWidth  = 8;
const Int32	kButtonHeight	 = 12;


//////////////////////////////////////////////////////////////////////////////////////////////
//
// Class
//
//////////////////////////////////////////////////////////////////////////////////////////////


class MCColorbook
{
	public:
		MCColorbook();
		virtual ~MCColorbook();
		
		// call this before using the other methods...
		bool				ReadColorbook( ConstSpCharPtr colorBookPath, bool justReadTheHeader = false );
	
		// the drawing method - this draws all the swatches in that group
		void				DrawSwatches( ADMDrawerRef environment, ASRect* bounds );
		void				DrawGroups( ADMDrawerRef environment, ASRect* bounds );
	
		// find closest color in a color book
		MCColorbookIndex	FindClosestColor( MCColorRef colorToMatch );
		MCColorbookIndex	FindByName( ConstSpCharPtr hashName );	
		
		// get/set the current color selection
		void				SetSelection( MCColorbookIndex );
		MCColorbookIndex	GetSelection() { return mSelection; }
		void				SetGroup( Int16 index );
		Int16				GetGroup() { return mCurrentGroup; }
		
		// this doesn't change the selection either, that is up to you...
		MCColorbookIndex	PickSelection( ASRect* bounds, ASPoint* mousePoint );
		
		// pick a new group.  Note: this doesn't change the selection as well...
		MCColorbookIndex	PickGroup( ASRect* boundsRect, ASPoint* mousePoint );
		
		// get a color by index...
		MCColor*			GetColor( Int16 group, Int16 index );
		MCColor*			GetColor( MCColorbookIndex );
		RVWideNameField		GetColorName( Int16 group, Int16 index );
		RVWideNameField		GetColorName( MCColorbookIndex );
		
		// not in use...!!@
		Int32				GetLongestName() { return mLongestName; }
		
		// get the height of the element for the current group.  used to better position the arrows...
		Int32				GetElementHeight( ASRect* bounds, bool groups = true );
		SpFloat				GetElementHeightFloat( ASRect* bounds, bool groups = true );
		
		
		// counting accessors
		Int16				GetNumGroups();
		Int16				GetNumColorsInGroup( Int16 groupIndex );
		
		// find out what color space this book is in
		MCSpace				GetBookSpace() { return mBookSpace; }
		
		// get the name of this color book...  this gets used in the popup menu
		RVWideNameField		GetBookTitle() { return mBookTitle; }
		
		bool				IsValidIndex( MCColorbookIndex index ) { return !(index.colorGroup == -1 || index.colorIndex == -1); } 
		
	protected:
		void				DrawSwatch( ADMDrawerRef environment, ASRect* bounds, MCColorbookIndex index );

		MCColorEntry*		GetColorEntry( MCColorbookIndex );
		void				DisposeColors();
		
		// actual file reading code
		UInt8				ReadByte( SpFilePtr fileToRead );
		UInt16				ReadShort( SpFilePtr fileToRead );
		UInt32				ReadLong( SpFilePtr fileToRead );
		bool				SkipBytes( SpFilePtr fileToRead, UInt32 numBytesToSkip );
		bool				ReadColorName( SpFilePtr fileToRead, UInt16 nameLength, RVWideNamePtr colorName );
		bool				ReadColorDefinition( SpFilePtr fileToRead, ConstSpWCharPtr definitionToFind, RVWideNamePtr resultDef = NULL );
		bool				ReadColorEntries( SpFilePtr fileToRead, MCColorEntries*, UInt16 numColorsInGroup, UInt16 numPlanes );
		Int16				MapFormatToPlanes( Int16 );

	
	private:
		RVWideNameField		mBookTitle;
		RVWideNameField		mNamePrefix;
		RVWideNameField		mNamePostfix;
		
		Int32				mLongestName;	// !!@ not in use
		MCSpace				mBookSpace;
		
		Int16				mGroupColorIndex;
		Int16				mCurrentGroup;
		MCColorbookIndex	mSelection;
		MCColorGroups		mColorGroups;
};

#endif // !_H_MCColorbook

// EOF
