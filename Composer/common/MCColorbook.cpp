/*
 *
 *	Copyright 2006 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#include "PhotoShopSDK.h"

#include "MCColorbook.h"

#include "ADM_Access.h"
#include "SwapByte.h"


// NOTE: The color book files are written in big-endian format...

#if WIN32 // || MAC_ENDIANESS == LITTLE // !! Macs come in two different endians...  
#define MC_LITTLE_ENDIAN
#endif


//////////////////////////////////////////////////////////////////////////////////////////////
//
// Defines
//
//////////////////////////////////////////////////////////////////////////////////////////////

const size_t	kColorNameBuffer	= 1024;
const UInt16	kColorDefaultPlanes = 3;
const UInt16	kColorbookVersion	= 1;
const UInt32	kColorbookSignature = '8BCB';

const ASInt16	kGully		= 4;
const ASInt16	kTextHeight	= 12;

ConstSpWCharPtr kColorbookTitleTag	 = L"title";
ConstSpWCharPtr kColorbookPrefixTag	 = L"prefix";
ConstSpWCharPtr kColorbookPostfixTag = L"postfix";
ConstSpWCharPtr kColorbookDescTag	 = L"description";


//////////////////////////////////////////////////////////////////////////////////////////////
//
// Constructors
//
//////////////////////////////////////////////////////////////////////////////////////////////

MCColorbook::MCColorbook() : 
	mLongestName( 0 ),
	mGroupColorIndex( 0 ),
	mCurrentGroup( 0 ),
	mBookSpace( kLabSpace )
{
	mSelection.colorGroup = 0;
	mSelection.colorIndex = -1;
}


MCColorbook::~MCColorbook()
{
	DisposeColors();
}


//////////////////////////////////////////////////////////////////////////////////////////////
//
// Main routine...
//
//////////////////////////////////////////////////////////////////////////////////////////////

// Read a color book file
bool MCColorbook::ReadColorbook( ConstSpCharPtr colorBookPath, bool justReadTheHeader )
{
	MCColorEntries* colorGroup = NULL;
	Int16 numColors			= 0;			
	Int16 numColorsInGroup	= 0;			
	Int16 planes			= 0;										
	
	// sanity
	if( !colorBookPath )
		return false;
	
	// Open the file
	SpFilePtr colorFile = ::fopen( colorBookPath, "rb" );
	if( !colorFile  )
		return false;
	
	// check signature		
	if( ReadLong( colorFile ) != kColorbookSignature )
		goto closeFile;
	
	// now check version number...
	if( ReadShort( colorFile ) != kColorbookVersion )
		goto closeFile;					

	// clean out any existing data...
	DisposeColors();

	// Skip 2 bytes of unknown data
	SkipBytes( colorFile, sizeof( Int16 ) );
	
	// read the title
	if( !ReadColorDefinition( colorFile, kColorbookTitleTag, &mBookTitle ) )
		goto closeFile;

	// Read the name prefix
	if( !ReadColorDefinition( colorFile, kColorbookPrefixTag, &mNamePrefix ) )	
		goto closeFile;
	
	// Read the name postfix
	if( !ReadColorDefinition( colorFile, kColorbookPostfixTag, &mNamePostfix ) )		
		goto closeFile;

	// Skip the description
	if( !ReadColorDefinition( colorFile, kColorbookDescTag ) )
		goto closeFile;

	// Read the number of colors in the file
	numColors = ReadShort( colorFile );			
	
	// Read the number of colors in a group
	numColorsInGroup = ReadShort( colorFile );			
	
	// Default number of planes
	planes = kColorDefaultPlanes;										

	// Read the index for the color book slider color 
	mGroupColorIndex = ReadShort( colorFile );

	// Read the color format indicator
	planes = MapFormatToPlanes( ReadShort( colorFile ) );

	// Clear the max name -- not used either...
	mLongestName = 0;											
	
	// don't read all the colors unless told to...
	if( !justReadTheHeader )
	{
		for( ; numColors > 0; numColors -= numColorsInGroup )
		{
			// read all the color entries in this "block" or "group"
			colorGroup = new MCColorEntries( numColorsInGroup );
			if( !colorGroup )
				goto closeFile;

			if( ReadColorEntries( colorFile, colorGroup, numColorsInGroup, planes ) )
			{
				// break out of this loop if some sort of failure occurs...
				if( !colorGroup->GetCount() )
					goto closeFile;						

				// store the group in the group list
				mColorGroups << colorGroup;
			}		
		}
	}
	
	if( colorFile  )
		::fclose( colorFile );

	return true;

closeFile:
	if( colorFile  )
		::fclose( colorFile );
	
	delete colorGroup;
	return false;
}




Int16 MCColorbook::MapFormatToPlanes( Int16 format )
{
	// Update the dialog for the color format
	switch( format )									
	{
		case 2:
			mBookSpace = kCMYKSpace;
			return 4;

		case 7:
			mBookSpace = kLabSpace;
			return 3;

		// Can't handle other formats
		default:
			return 0;				
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
// File Reading code
//
//////////////////////////////////////////////////////////////////////////////////////////////

// Read a byte from the file
UInt8 MCColorbook::ReadByte( SpFilePtr fileToRead )
{
	UInt8 result = 0;

	::fread( &result, sizeof( result ), 1, fileToRead );

	return result;
}

// Read a UInt16 from the file
UInt16 MCColorbook::ReadShort( SpFilePtr fileToRead )
{
	UInt16 result = 0;

	::fread( &result, sizeof( result ), 1, fileToRead );

#ifdef MC_LITTLE_ENDIAN
	::SwapShort( result );
#endif
	
	return result;
}


// Read a UInt32 from the file
UInt32 MCColorbook::ReadLong( SpFilePtr fileToRead )
{
	UInt32 result = 0;

	::fread( &result, sizeof( result ), 1, fileToRead );

#ifdef MC_LITTLE_ENDIAN
	::SwapLong( result );
#endif
	
	return result;
}


bool MCColorbook::SkipBytes( SpFilePtr fileToRead, UInt32 numBytesToSkip )
{
	return !::fseek( fileToRead, numBytesToSkip, SEEK_CUR );
}


// Read a tagged string from the file, handling escape sequences
bool MCColorbook::ReadColorDefinition( SpFilePtr fileToRead, ConstSpWCharPtr definitionToFind, RVWideNamePtr resultDef )
{	
	// Skip the filler
	SkipBytes( fileToRead, sizeof( Int16 ) );									

	// Read the length
	UInt16 length = ReadShort( fileToRead );

	// use a stack based read buffer
	SpWChar readBuffer[kColorNameBuffer] = {0};
	
	// clamp length to prevent buffer overrun
	if( length > kColorNameBuffer )
		length = kColorNameBuffer;
	
	// read the definition
	if( ::fread( readBuffer, sizeof( SpWChar ), length, fileToRead ) != length )
		return false;
		
#ifdef MC_LITTLE_ENDIAN
	// byte swap loop
	for( int i = 0; i < length; i++ )
		::SwapShort( readBuffer[i] );
#endif

	// find the last slash in the read buffer
	SpWCharPtr slash = ::wcsrchr( readBuffer, L'/' );
	if( !slash )
		return false;
	
	// look for the equal sign while building the string... 
	SpWCharPtr equal = ::wcsrchr( readBuffer, L'=' );
	if( !equal )
		return false;
	
	// temporarily null terminate the string...
	SpWChar temp = *equal;
	*equal = NULL;
	
	// skip over the slash we found
	RVWideNameField foundTag( ++slash );
	
	// replace the equal sign
	*equal = temp;
		
	// compare the read buffer against the passed in definition to find
	if( RVWideNameField( definitionToFind ) != foundTag )
		return false;
	
	// now grab the remaining part of the string and return that...
	if( resultDef )
	{
		resultDef->Set( ++equal );
		resultDef->ReplaceEscapeCodes();
	}

	return true;
}


// Read a block of color information
bool MCColorbook::ReadColorEntries( SpFilePtr fileToRead, MCColorEntries* entries, UInt16 numColorsInGroup, UInt16 numPlanes )
{
	if( !entries )
		return false;

	while( !feof( fileToRead ) && numColorsInGroup-- > 0 )
	{
		MCColorEntry	colorEntry = {};
		Int16			nameLength;

		// Skip the filler
		SkipBytes( fileToRead, sizeof( Int16 ) );
		
		// Read the name length					
		nameLength = ReadShort( fileToRead );			
		
		// Skip an empty color entry
		if( nameLength <= 0 )
		{
			// skip empty hash name
			if( !SkipBytes( fileToRead, sizeof( Int8 ) * 6 ) )
				break;
			
			// skip empty components
			if( !SkipBytes( fileToRead, sizeof( Int8 ) * numPlanes ) )
				break;
			
			// process next entry
			continue;						
		}

		ReadColorName( fileToRead, nameLength, &colorEntry.name );

		// Record the length of the longest short name
		if( mLongestName < nameLength )
			mLongestName = nameLength;		

		// grab ASCII name for searching -- the hash name is 6 bytes
		::fread( colorEntry.hash.mName, sizeof( Int8 ), 6, fileToRead );
		
		short components[4] = {0};
		
		// Read the color information and stuff
		for( int i = 0; i < numPlanes; i++ )	
		{
			components[i] = ReadByte( fileToRead );
		}
		
		// set the display color
		colorEntry.color.SetColor( numPlanes == 4 ? kCMYKSpace : kLabSpace, components );
				
		// Add the color to the block data
		(*entries) << colorEntry;
	}
	
	return true;
}


// Read a wstring from the file, skipping a leading tag, ignoring escape sequences
bool MCColorbook::ReadColorName( SpFilePtr fileToRead, UInt16 length, RVWideNamePtr colorName )
{
	// use a stack based read buffer
	SpWChar readBuffer[kColorNameBuffer] = {0};
	
	// clamp length to prevent buffer overrun
	if( length > kColorNameBuffer )
		length = kColorNameBuffer;
	
	// read the definition
	if( ::fread( readBuffer, sizeof( SpWChar ), length, fileToRead ) != length )
		return false;
		
#ifdef MC_LITTLE_ENDIAN
	// byte swap loop
	for( int i = 0; i < length; i++ )
		::SwapShort( readBuffer[i] );
#endif

	// look for the equal sign while building the string... if we find one
	// we need to skip this string and grab the one after the equals sign.
	SpWCharPtr equal = ::wcsrchr( readBuffer, L'=' );
	SpWCharPtr color = NULL;
	if( equal )
		color = ++equal;
	else
		color = readBuffer;
			
	// now grab the remaining part of the string and return that...
	if( colorName && color )
		colorName->Set( color );

	return true;
}


void MCColorbook::DisposeColors()
{
	Int32 numGroups = mColorGroups.GetCount();
	if( !numGroups )
		return;
	
	MCColorEntries** entryArray = mColorGroups.Lock(0);
	
	if( entryArray )
	{
		// dump all the color entries
		for( int i = 0; i < numGroups; i++ )
		{
			delete entryArray[i];
		}
	}
	
	mColorGroups.Remove( 0, -1 ); // remove all elements...
}


//////////////////////////////////////////////////////////////////////////////////////////////
//
// Searching code
//
//////////////////////////////////////////////////////////////////////////////////////////////


// Find the color closest to the original color
MCColorbookIndex MCColorbook::FindClosestColor( MCColorRef inputColor )
{
	// start with furthest initial distance
	Int32			 distance     = LONG_MAX;		
	Int32			 bestDistance = LONG_MAX;		
	MCColorbookIndex bestMatch    = {-1,-1};
	
	// we use a temporary color to match the color spaces...
	MCColor			 colorToMatch;
	short			 components[4] = {0};
	inputColor.GetColor( mBookSpace, components );
	colorToMatch.SetColor( mBookSpace, components );
	
	// Search the color book for the closest color
	for( int i = 0; i < mColorGroups.GetCount(); i++ )
	{
		// search through every color in the group
		MCColorEntries* colors = mColorGroups[i];
		if( colors )
		{
			for( int j = 0; j < colors->GetCount(); j++ )
			{
				distance = colorToMatch.ColorDistance( const_cast<MCColor&>( (*colors)[j].color ) );
				
				// see if we found something closer
				if( distance < bestDistance )
				{
					// store state
					bestDistance = distance;
					bestMatch.colorIndex = j;
					bestMatch.colorGroup = i;
				}
			}
		}
	}	

	return bestMatch;
}



MCColorbookIndex MCColorbook::FindByName( ConstSpCharPtr hashName )
{
	MCColorbookIndex bestMatch = {-1,-1};
	
	// sanity
	if( !hashName )
		return bestMatch;
	

	// Search the color book for the closest color
	for( int i = 0; i < mColorGroups.GetCount(); i++ )
	{
		// search through every color in the group
		MCColorEntries* colors = mColorGroups[i];
		if( colors )
		{
			for( int j = 0; j < colors->GetCount(); j++ )
			{
				MCColorEntry* e = colors->Lock( j );
				if( e )
				{
					// see if we found it -- use partial match for now...
					SpCharPtr found = ::strstr( e->hash, hashName );
					if( found )
					{
						// store state
						bestMatch.colorIndex = j;
						bestMatch.colorGroup = i;
						return bestMatch;
					}
					colors->Unlock();
				}
			}
		}
	}	
	
	return bestMatch;
}


void MCColorbook::SetSelection( MCColorbookIndex index )
{
	mSelection = index;
	mCurrentGroup = mSelection.colorGroup;
}

void MCColorbook::SetGroup( Int16 index )
{
	mCurrentGroup = index;
}


Int32 MCColorbook::GetElementHeight( ASRect* bounds, bool groups )
{
	Int32 clientHeight = bounds->bottom - bounds->top;
	
	// calculate the height of each color for all the groups...
	if( groups )
		return (Int32)((clientHeight / (SpFloat)GetNumGroups()) + 0.5f);
	
	return (Int32)((clientHeight / (SpFloat)GetNumColorsInGroup( mCurrentGroup )) + 0.5f);
}


SpFloat MCColorbook::GetElementHeightFloat( ASRect* bounds, bool groups )
{
	Int32 clientHeight = bounds->bottom - bounds->top;
	
	// calculate the height of each color for all the groups...
	if( groups )
		return clientHeight / (SpFloat)GetNumGroups();
	
	return clientHeight / (SpFloat)GetNumColorsInGroup( mCurrentGroup );
}


MCColorbookIndex MCColorbook::PickSelection( ASRect* boundsRect, ASPoint* mousePoint )
{
	// start out with a bogus color index in case we fail...
	MCColorbookIndex sel = { -1, mCurrentGroup };
	
	if( !boundsRect || !mousePoint )
		return sel;

	// map the mouse point which is in local coordinates to an item in the current group
	MCColorEntries* e = mColorGroups[mCurrentGroup];
	if( !e )
		return sel;

	// we need the element height...
	Int32 elementHeight = (boundsRect->bottom - boundsRect->top) / e->GetCount();
	Int32 position = (mousePoint->v - boundsRect->top) / elementHeight;
	
	if( position >= e->GetCount() )
		position = e->GetCount() - 1;
	
	// select that item
	sel.colorIndex = (Int16)position;
	
	// let the caller know what the selection is...
	return sel;
}


MCColorbookIndex MCColorbook::PickGroup( ASRect* boundsRect, ASPoint* mousePoint )
{
	// start out with a bogus color group in case we fail...
	MCColorbookIndex sel = { 0, -1 };
	
	if( !boundsRect || !mousePoint )
		return sel;

	// map the mouse point which is in local coordinates to a group
	Int16 numGroups = GetNumGroups();
	if( !numGroups )
		return sel;
	
	Int32	clientHeight  = boundsRect->bottom - boundsRect->top;
	SpFloat elementHeight = numGroups / (SpFloat)clientHeight;

	Int16 position = (Int16)((mousePoint->v - boundsRect->top) * elementHeight);
	
	if( position >= numGroups )
		position = numGroups - 1;
	
	// select that item
	sel.colorGroup = position;
	
	// let the caller know what the selection is...
	return sel;
}


//////////////////////////////////////////////////////////////////////////////////////////////
//
// Drawing routines
//
//////////////////////////////////////////////////////////////////////////////////////////////

void MCColorbook::DrawSwatches( ADMDrawerRef environment, ASRect* bounds )
{
	if( !bounds )
		return;
	
	// calculate the height of each element for this group...
	Int32 elementHeight = (bounds->bottom - bounds->top) / mColorGroups[mCurrentGroup]->GetCount();
	
	// erase the top and bottom incase the selection was there to deal with rounding errors in list display
	ASRect eraseRect = *bounds;
	eraseRect.bottom = eraseRect.top + kGully;
	ADM_Access::drawing_suite()->ClearRect( environment, &eraseRect );
	
	// here on the bottom we erase one more pixel than normally necessary, this is
	// to deal with rounding errors that occur when drawing the last item's selection rectangle.
	eraseRect = *bounds;
	eraseRect.top = eraseRect.bottom - kGully - 1;
	ADM_Access::drawing_suite()->ClearRect( environment, &eraseRect );

	
	// draw the colors -- account for selection's top
	ASRect drawRect = *bounds;
	drawRect.top   += kGully;
	drawRect.bottom = drawRect.top + elementHeight;
	
	ASRect selectionRect = {0};
	bool   selectionFound = false;
	
	for( Int32 i = 0; i < mColorGroups[mCurrentGroup]->GetCount(); i++ )
	{
		// check to see if the rectangle passed the edge of the display and adjust if so...
		if( drawRect.bottom >= bounds->bottom )
			drawRect.bottom -= (drawRect.bottom - bounds->bottom);

		MCColorbookIndex index = { (Int16)i, mCurrentGroup };
		DrawSwatch( environment, &drawRect, index );

		// see if this index is selected...
		if( mSelection.colorGroup == index.colorGroup && mSelection.colorIndex == index.colorIndex )
		{
			selectionRect  = drawRect;
			selectionFound = true;
		}
		
		// bump rectangle
		drawRect.top    += elementHeight;
		drawRect.bottom += elementHeight;
	}
	
	// frame selection if found
	if( selectionFound )
	{
		ASRect frameRect = selectionRect;
		
		// remove gully from top
		frameRect.top    -= kGully;
		
		for( Int8 i = 0; i < kGully; i++ )
		{
			ADM_Access::drawing_suite()->DrawRect( environment, &frameRect );
			frameRect.top    += 1;
			frameRect.left   += 1;
			frameRect.bottom -= 1;
			frameRect.right  -= 1;
		}
	}
}


void MCColorbook::DrawSwatch( ADMDrawerRef environment, ASRect* bounds, MCColorbookIndex index )
{
	// get the color to draw
	MCColor* colorToDraw = GetColor( index );
	
	if( !colorToDraw )
		return;
	
	// get the color name -- the error case here is a blank name...
	RVWideNameField colorName = GetColorName( index );
	
	// adjust the bounds to account for the selection gully 
	ASRect clientRect = *bounds;
	clientRect.left   += kGully;
	clientRect.bottom -= kGully;
	clientRect.right  -= kGully;

	// we need to erase the sides of the window (and not the whole rectangle)
	ASRect eraseBounds = *bounds;
	
	// erase left side
	eraseBounds.right = eraseBounds.left + kGully;
	ADM_Access::drawing_suite()->ClearRect( environment, &eraseBounds );
	
	// erase right side
	eraseBounds = *bounds;
	eraseBounds.left = eraseBounds.right - kGully;
	ADM_Access::drawing_suite()->ClearRect( environment, &eraseBounds );

	// erase top side
	eraseBounds = *bounds;
	eraseBounds.top -= kGully;
	eraseBounds.bottom = eraseBounds.top + kGully;
	ADM_Access::drawing_suite()->ClearRect( environment, &eraseBounds );

	// adjust the drawing rectangle to make room for the text...
	// erase bottom side -- NOTE: this is the rectangle used for the text display...
	ASRect textBounds = clientRect;
	textBounds.top = textBounds.bottom - kTextHeight;
	ADM_Access::drawing_suite()->ClearRect( environment, &textBounds );

	// draw the color into the rectangle
	ASRect drawRect = clientRect;
	drawRect.bottom -= kTextHeight;
	colorToDraw->Draw( environment, &drawRect, false );

	// draw the text
	ASRGBColor frame_color = {0};	// black
	ADM_Access::drawing_suite()->SetRGBColor( environment, &frame_color );
	ADM_Access::drawing_suite()->SetFont( environment, kADMPaletteFont );
	ADM_Access::drawing_suite()->DrawTextCenteredW( environment, colorName, &textBounds );
}


void MCColorbook::DrawGroups( ADMDrawerRef environment, ASRect* bounds )
{
	if( !bounds )
		return;
	
	// get the current group from the selection...
	Int16 currentGroup = mSelection.colorGroup;

	Int32 numGroups    = mColorGroups.GetCount();
	Int32 clientHeight = bounds->bottom - bounds->top;
	
	ASRect boundsRect = *bounds;

	// initial conditions
	SpFloat colorHeight = boundsRect.top + GetElementHeightFloat( bounds );
	
	// loop thru all the groups and draw the representative color from each...
	for( Int32 i = 0; i < mColorGroups.GetCount(); i++, colorHeight += GetElementHeightFloat( bounds ) )
	{
		MCColorbookIndex index = { mGroupColorIndex, (Int16)i };
		
		// truncate here
		boundsRect.bottom = (long)colorHeight;
		
		MCColor* colorToDraw = GetColor( index );
		if( colorToDraw )
			colorToDraw->Draw( environment, &boundsRect, false );
		
		// update rectangle
		boundsRect.top = boundsRect.bottom;
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////
//
// Accessors
//
//////////////////////////////////////////////////////////////////////////////////////////////


MCColor* MCColorbook::GetColor( MCColorbookIndex index )
{
	// attempt to access the requested record
	MCColorEntry* e = GetColorEntry( index );
	if( !e )
		return NULL;
	
	// return the color record
	return &e->color;
}


MCColor* MCColorbook::GetColor( Int16 group, Int16 index )
{
	MCColorbookIndex cnIndex = { index, group };
	return GetColor( cnIndex );
}


// we assemble the display name for the color...
RVWideNameField MCColorbook::GetColorName( MCColorbookIndex index )
{
	RVWideNameField result;
	
	// grab the color name
	MCColorEntry* e = GetColorEntry( index );
	if( e )
	{
		// construct full name
		result.Set( mNamePrefix );
		result += e->name;
		result += mNamePostfix;
	}
	
	return result;
}


RVWideNameField	MCColorbook::GetColorName( Int16 group, Int16 index )
{
	MCColorbookIndex cnIndex = { index, group };
	return GetColorName( cnIndex );
}



// counting accessors
Int16 MCColorbook::GetNumGroups()
{
	return (UInt16)mColorGroups.GetCount();
}


Int16 MCColorbook::GetNumColorsInGroup( Int16 groupIndex )
{
	try
	{
		return (UInt16)mColorGroups[groupIndex]->GetCount();
	} 
	catch(...) {}
	
	return 0;
}


MCColorEntry* MCColorbook::GetColorEntry( MCColorbookIndex index )
{
	// attempt to access the requested record
	try
	{
		MCColorEntries* entries = mColorGroups[index.colorGroup];
		if( !entries )
			return NULL;
		
		// return selected entry...
//		MCColorEntry* e = (*entries)[index.colorIndex];
		MCColorEntry* e = entries->Lock( index.colorIndex );
		entries->Unlock();
		return e;
	}
	catch(...) {}
	
	return NULL;
}



// EOF
