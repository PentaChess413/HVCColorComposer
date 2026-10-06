/*
 *
 *	Copyright 2005 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	Source code rewritten by: Alex Lelievre ([PII Redacted]), 2006.
 *
 *
 *	U.S. and Foreign patent applications pending
 *
 */



//////////////////////////////////////////////////////////////////////////////////////////////
//
// Includes
//
//////////////////////////////////////////////////////////////////////////////////////////////

#include "Palette_Display_Handler.h"
#include "Composer_UI.h"
#include "Color_Plane_Selector.h"
#include "SwapByte.h"

#include "admDrawer.h"

#include <math.h>


//////////////////////////////////////////////////////////////////////////////////////////////
//
// Defines
//
//////////////////////////////////////////////////////////////////////////////////////////////

const unsigned short kMCSwatchBookCurrentVersion = 1;
const SpCharPtr		 kMCFileFilter				 = ".aco";

// !!@ we might need packing pragmas here ???

typedef struct
{
	unsigned short	version;			// should be set to 1
	unsigned short	numSwatches;		// number of swatches in the file
} MCSwatchHeader;




//////////////////////////////////////////////////////////////////////////////////////////////
//
// Main routines...
//
//////////////////////////////////////////////////////////////////////////////////////////////


void Palette_Display_Base::Draw( ADMDrawerRef environment )
{
	ASRect		control_bounds = bounds( true );
	ASRGBColor	the_color      = {0};										// Draw the top edge

#if WIN32
	ASPoint		line_start = { control_bounds.left, control_bounds.top };
	ASPoint		line_end = { control_bounds.right, control_bounds.top };
#else
	ASPoint		line_start = { control_bounds.top, control_bounds.left };
	ASPoint		line_end = { control_bounds.top, control_bounds.right };
#endif
	
	// set color to black...
	ADM_Access::drawing_suite()->SetRGBColor( environment, &the_color );
	
	// draw top line
	ADM_Access::drawing_suite()->DrawLine( environment, &line_start, &line_end );
	
	// Draw the bottom edge
	line_start.v = line_end.v = control_bounds.bottom;					
	ADM_Access::drawing_suite()->DrawLine( environment, &line_start, &line_end );

	// Calculate the color size
	++control_bounds.left;												
	
	// that's all the drawing we'll do if we can't gain access to the main dialog
	ComposerUI*	ui = GetComposerUI();
	if( !ui )
		return;
	
#if WIN32	
	ASRect	color_bounds = { control_bounds.left, control_bounds.top, control_bounds.left, control_bounds.top + PALETTE_SIZE };
#else
	ASRect	color_bounds = { control_bounds.top, control_bounds.left, control_bounds.top + PALETTE_SIZE, control_bounds.left };
#endif	
	
	size_t	index = ui->GetControl( PALETTE_SCROLL_BAR ).int_value() * PALETTE_COLUMNS;

	// this code draws every grid location and if there are empty slots
	// it will erase the unused portions of the palette display

	// Draw the rows
	for( size_t row = 0; row < PALETTE_ROWS; ++row )	
	{
		// Calculate the color bounds
		color_bounds.left = control_bounds.left;						
		color_bounds.right = control_bounds.left + PALETTE_SIZE;
		
		for( size_t col = index + PALETTE_COLUMNS; col > index; ++index )													// Draw the colors
		{
			// if the color is in range draw it, otherwise erase there...
			if( index < mPalette.size() )
			{
				// draw the color - we just use the MCColor class to draw the HVC	
				MCColor( mPalette[index] ).Draw( environment, &color_bounds );
			}
			else
			{
				// adjust the bounds so we don't erase grid lines...
				ASRect eraseRect = color_bounds;
				eraseRect.top    += 1;
				eraseRect.bottom -= 1;
				
				// special case the first and last empties
				if( index == mPalette.size() || index == PALETTE_COLUMNS * PALETTE_ROWS )
				{
					eraseRect.left   += 1;
					eraseRect.right  -= 1;
				}
				ADM_Access::drawing_suite()->ClearRect( environment, &eraseRect );
			}
			
			// Calculate the next color's bounds
			color_bounds.left = color_bounds.right - 1;	
			color_bounds.right += PALETTE_SIZE;
		}

		// Calculate the next row's bounds
		color_bounds.top = color_bounds.bottom - 1;						
		color_bounds.bottom += PALETTE_SIZE;
	}
	
}




ASBoolean Palette_Display_Base::HandleTracking( ADMTrackerRef tracker )
{
	ASPoint	mousePoint;
	ASRect bounds = Control_Interface::bounds( true );
	
#if WIN32
	ADM_Access::tracking_suite()->GetPoint( tracker, &mousePoint );

	// convert the rectangle to a windows rectangle because I'm too lazy to write it myself
	RECT winRect    = { bounds.left, bounds.top, bounds.right, bounds.bottom };
	RECT swatchRect = { mSelectedSwatch.left, mSelectedSwatch.top, mSelectedSwatch.right, mSelectedSwatch.bottom };
	POINT targetPt  = { mousePoint.h, mousePoint.v };
	
	bool inDifferentSwatch = (::PtInRect( &winRect, targetPt ) && !PtInRect( &swatchRect, targetPt ));
	
	// if we get a move down or a mouse mouse we are game...as long as they pick a different swatch	
	if( (ADM_Access::tracking_suite()->TestAction( tracker, kADMButtonDownAction ) ||
		ADM_Access::tracking_suite()->TestAction( tracker, kADMMouseMovedDownAction )) &&
		inDifferentSwatch )
	{
//		XDEBUG_PRINT2( "mousePoint: (%ld, %ld)\n", mousePoint.h, mousePoint.v );

		// okay we are in a different swatch (or we are in here for the first time)
		// calculate where the mouse is...
		mousePoint.v /= PALETTE_SIZE;
		mousePoint.h /= PALETTE_SIZE;
		
		// this should be the index of the swatch under the mouse.
		ASInt32 hitIndex = mousePoint.v * PALETTE_COLUMNS + mousePoint.h;

		// update the selected swatch -- !!@ we could use this data to display the selected item as well...
		mSelectedSwatch.top    = PALETTE_SIZE * mousePoint.v;
		mSelectedSwatch.left   = PALETTE_SIZE * mousePoint.h;
		mSelectedSwatch.bottom = mSelectedSwatch.top  + PALETTE_SIZE;
		mSelectedSwatch.right  = mSelectedSwatch.left + PALETTE_SIZE;

//		XDEBUG_PRINT5( "index: %ld, (rect: %ld, %ld, %ld, %ld)\n", hitIndex, mSelectedSwatch.left, mSelectedSwatch.top, mSelectedSwatch.right, mSelectedSwatch.bottom );

		// we don't check to see if the index is in range this happens inside of SelectPaletteColor
		SelectPaletteColor( hitIndex );
	}
#endif
	return true;
}



// Change the selected color to a palette color
bool Palette_Display_Base::SelectPaletteColor( ASInt32 hitIndex )
{
	ComposerUI* ui = GetComposerUI();
	if( !ui )
		return false;

	// What part of the palette is being shown? -- get this from scroll bar position...
	ASInt32 scrollIndex = ui->GetControl( PALETTE_SCROLL_BAR ).int_value() * PALETTE_COLUMNS;
	
	// Calculate the new palette index
	scrollIndex += hitIndex;	
	
	// dp not allow unsafe array access
	if( mPalette.size() <= 0 || scrollIndex >= (ASInt32)mPalette.size() )
		return false;
	
	// look up the HVC color in the palette
	MCColor targetHVC( mPalette[scrollIndex] );

	// Make it the selected color
	ui->SelectNewColor( targetHVC, kUpdateFromPaletteFlag );

	return true;
}



// Check to see if the colors are within accuracy distance of the range value
bool Palette_Display_Base::CheckColorRange( const HVCTrip& i, double range, double accuracy, const HVCTrip& limit, const HVCTrip& )
{
	double mag = ::abs( CalculateDistance( limit, i ) - range );
	return mag <= accuracy;
}

// Check the proportional distances against the accuracy
bool Palette_Display_Base::CheckColorProportion( const HVCTrip& i, double proportion, double accuracy, const HVCTrip& limit_1, const HVCTrip& limit_2 )
{
	double mag = ::abs( CalculateDistance( limit_2, i ) * proportion - CalculateDistance( limit_1, i ) );
	return mag <= accuracy;
}



// Calculate the chroma distance
inline double Palette_Display_Base::CalculateModifier( const double chroma_1, const double chroma_2 )
{
	return (3.0 * chroma_1 * chroma_2 - ::pow(chroma_2, 2.0)) / (6.0 * chroma_1);
}

// Calculate the distance between 2 HVC colors
inline double Palette_Display_Base::CalculateDistance( const HVCTrip& first_color, const HVCTrip& second_color )
{
	// Normalize the chroma values
	double	chroma_1 = static_cast<double> (first_color.chroma) * MAX_CHROMA / RAW_MAX_D;
	double	chroma_2 = static_cast<double> (second_color.chroma) * MAX_CHROMA / RAW_MAX_D;
	// Calculate the simple contrast
	double	contrast = ::hypot(static_cast<double> (static_cast<int32> (first_color.value) -
						static_cast<int32> (second_color.value)) * MAX_VALUE / RAW_MAX_D,
						chroma_1 - chroma_2);

	// Not so simple -- none of Orson's code is simple!!  it's all twisted.
	if( 0 != first_color.chroma && 0 != second_color.chroma )	
	{
		double	hue_dist = std::abs(static_cast<int32> (first_color.hue) -
									static_cast<int32> (second_color.hue));

		if( HALF_MAX_HUE < hue_dist )					// Hue is a circle
			hue_dist = RAW_MAX_D - hue_dist;			// Limit the distance to half the circle

		// Normalize the hue distance
		hue_dist *= MAX_DIST / RAW_MAX_D;
		// Multiply by the chroma distance
		hue_dist *= first_color.chroma > second_color.chroma ? CalculateModifier( chroma_1, chroma_2 ) : CalculateModifier( chroma_2, chroma_1 );

		// Calculate the complex result
		contrast = ::hypot(hue_dist, contrast);
	}

	return contrast;
}



void Palette_Display_Base::SavePalette()
{
	ADMPlatformFileTypesSpecification3	spec	= {0};
	SPPlatformFileSpecification			result	= {};
//	spec.types		= NULL;
//	spec.numTypes	= 1;
	::strncpy( spec.filter, kMCFileFilter, kADMMaxFilterLength );

	// put up the save dialog -- !!@ need resource string here...  too lazy
	ASBoolean sucess = ADM_Access::basic_suite()->StandardPutFileDialog( "Save", &spec, NULL, NULL, &result ); 
 	if( !sucess )
		return;

#if WIN32		
	// ??? the freak'n name has an extra . in it... annoying
	size_t len = ::strlen( result.path );
	
	// if we find a  dot in there we assume there are two...
	SpCharPtr found = ::strstr( result.path, "." );
	if( found )
	{
		size_t dotLen = ::strlen( found );
		::memmove( found, found + 1, dotLen * sizeof( SpChar ) );
	}
	
	// write file to disk !!@ note: path will only work for windows !!@
	ASErr err = WritePalette( result.path );
#else
	// ??? the freak'n name has an extra . in it... annoying
//	size_t len = ::strlen( result.path );
	
	// if we find a  dot in there we assume there are two...
//	SpCharPtr found = ::strstr( result.path, "." );
//	if( found )
//	{
//		size_t dotLen = ::strlen( found );
//		::memmove( found, found + 1, dotLen * sizeof( SpChar ) );
//	}
	
	// write file to disk -- convert pascal string to C string...  !!@
//	ASErr err = WritePalette( result.name );
	ASErr err = noErr;
#endif
	
	// handle error case -- replace with resource string!!@
	if( err )
	{
		char buffer[1024] = {0};
		::sprintf( buffer, "An Error occurred. (%ld)", err );
		ADM_Access::basic_suite()->ErrorAlert( buffer );
	}
}


ASErr Palette_Display_Base::WritePalette( ConstSpCharPtr paletteFilePath )
{
	if( !paletteFilePath )
		return noErr;
		
	// open/create the file
	SpFilePtr swatchFile = ::fopen( paletteFilePath, "wb" );
	if( !swatchFile  )
		return openErr;

	// write header portion
	MCSwatchHeader header;
	header.version = kMCSwatchBookCurrentVersion;
	header.numSwatches = mPalette.size();

// this needs to check for endianess for MacIntel instead of against Windows... !!@	
#if WIN32
	::SwapShort( header.version );
	::SwapShort( header.numSwatches );
#endif	
	
	size_t writeSize = ::fwrite( &header, sizeof( header ), 1, swatchFile ); 
	if( writeSize != 1 )
	{
		::fclose( swatchFile );
		return writErr;
	}
	
	// write out the palette data
	for( ASUInt32 i = 0; i < mPalette.size(); i++ )
	{
		// write out a single element...  the deal is that we know the colors are all HVC 
		// so we need them in RGB because PhotoShop doesn't know anything about HVC.
		MCColor rgb = MCColor( mPalette[i] ).GetColorInRGB();
		
		// now stuff the packed version in the file...
		MCPackedColor color = rgb.GetPackedColorForPS();

#if WIN32
		::SwapShort( color.colorSpace );
		::SwapShort( color.components[0] );
		::SwapShort( color.components[1] );
		::SwapShort( color.components[2] );
		::SwapShort( color.components[3] );
#endif	
		
		writeSize = ::fwrite( &color, sizeof( color ), 1, swatchFile ); 
		if( writeSize != 1 )
		{
			::fclose( swatchFile );
			return writErr;
		}
	}

	// close the file
	::fclose( swatchFile );
	return noErr;
}



// EOF
