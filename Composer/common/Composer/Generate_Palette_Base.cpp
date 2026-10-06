/*
 *
 *	Copyright 2005 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

//
// The palette generation algorithms use separate loop control variables because the HVC color
//	structures use single bytes for each component.  The loop control needs more than that to
//	correctly determine when the loop has reached its end. 
//

#include "Generate_Palette_Base.h"
#include "MCLib_Wrapper.h"
#include "Helpers.h"

// Call the m_check_proc to see if the color should be added
inline void Generate_Palette_Base::check_color(HVCArray& result, double p_parameter,
									const HVCTrip& i) const
{
	if ((*m_check_proc)(i, p_parameter, m_accuracy, m_limit_1, m_limit_2))
		result.push_back(i);												// Add the color
}

// Generate the palette using the HVC algorithm
void Generate_Palette_Base::generate_hvc( HVCArray& result, double p_parameter, MCLib_Wrapper& p_mclib ) const
{
	HVCTrip	i = { 0 };									// Current color
	int32	value, step_size = RAW_MAX / m_step_count;	// Loop control variables

	for (value = 0; 255 >= value; value += step_size)	// Start with the Gray (choma==0) colors
	{
		i.value = (int8)value;								// Copy the loop control to the color
		check_color(result, p_parameter, i);			// Add valid colors
	}

	for (int hue = 0; 255 >= hue; hue += step_size)		// Add the non-Gray (chroma!=0) colors
	{
		i.hue = hue;									// Copy the loop control to the color
		for (value = 0; 255 >= value; value += step_size)
		{
			i.value = (int8)value;							// Copy the loop control to the color
			
			// !!@ truncate value
			const uint8 end_chroma = (uint8)p_mclib.max_chroma( i.hue, i.value );	// Don't loop over invalid colors

			// i.chroma can be used as the loop control here
//			for (i.chroma = step_size; end_chroma >= i.chroma; i.chroma += step_size)
			for( int chroma = step_size; chroma <= end_chroma; chroma += step_size )
			{
				i.chroma = chroma;
				
				check_color( result, p_parameter, i );	// Add valid colors
			}
		}
	}
}

// Generate the palette using the HCV algorithm
void Generate_Palette_Base::generate_hcv(HVCArray& result, double p_parameter, MCLib_Wrapper& p_mclib) const
{
	HVCTrip	i = { 0 };									// Current color
	int32	value; 
	int32	step_size = RAW_MAX / m_step_count;	// Loop control variables

	for (value = 0; 255 >= value; value += step_size)	// Start with the Gray (choma==0) colors
	{
		i.value = (int8)value;								// Copy the loop control to the color
		check_color(result, p_parameter, i);			// Add valid colors
	}

	for (int hue = 0; 255 >= hue; hue += step_size)		// Add the non-Gray (chroma!=0) colors
	{
		i.hue = hue;									// Copy the loop control to the color
		for (int chroma = step_size; 255 >= chroma; chroma += step_size)
		{
			i.chroma = chroma;							// Copy the loop control to the color
			value = 0;
			
			// Don't loop over invalid colors
			while( 255 >= value && p_mclib.max_chroma( i.hue, (uint8)value ) < chroma )	
				value += step_size;

			// Don't loop over invalid colors
			for( ; 255 >= value && p_mclib.max_chroma( i.hue, (uint8)value ) >= chroma; value += step_size )	
			{
				i.value = (uint8)value;						// Copy the loop control to the color
				check_color( result, p_parameter, i );	// Add valid colors
			}
		}
	}
}

// Generate the palette using the CHV algorithm
void Generate_Palette_Base::generate_chv( HVCArray& result, double p_parameter, MCLib_Wrapper& p_mclib ) const
{
	HVCTrip	i = { 0 };									// Current color
	int32 value; 
	int32 step_size = RAW_MAX / m_step_count;	// Loop control variables

	for( value = 0; 255 >= value; value += step_size )	// Start with the Gray (choma==0) colors
	{
		i.value = (int8)value;							// Copy the loop control to the color
		check_color( result, p_parameter, i );			// Add valid colors
	}

	for( int chroma = step_size; 255 >= chroma; chroma += step_size )	// Add the non-Gray (chroma!=0) colors
	{
		i.chroma = chroma;								// Copy the loop control to the color
		for( int hue = 0; 255 >= hue; hue += step_size )
		{
			i.hue = hue;								// Copy the loop control to the color
			for( value = 0; 255 >= value; value += step_size )
			{
				i.value = (uint8)value;						// Copy the loop control to the color
				if( p_mclib.max_chroma( i.hue, i.value ) >= chroma )	// Don't loop over invalid colors
					check_color( result, p_parameter, i );	// Add valid colors
			}
		}
	}
}

// Generate the palette using the CVH algorithm
void Generate_Palette_Base::generate_cvh(HVCArray& result, double p_parameter, MCLib_Wrapper& p_mclib) const
{
	HVCTrip	i = { 0 };									// Current color
	int32 value;
	int32 step_size = RAW_MAX / m_step_count;	// Loop control variables

	for( value = 0; 255 >= value; value += step_size )	// Start with the Gray (choma==0) colors
	{
		i.value = (int8)value;								// Copy the loop control to the color
		check_color(result, p_parameter, i);			// Add valid colors
	}

	for (int chroma = step_size; 255 >= chroma; chroma += step_size)	// Add the non-Gray (chroma!=0) colors
	{
		i.chroma = chroma;								// Copy the loop control to the color
		for (value = 0; 255 >= value; value += step_size)
		{
			i.value = (int8)value;							// Copy the loop control to the color
			for (int hue = 0; 255 >= hue; hue += step_size)
			{
				i.hue = hue;							// Copy the loop control to the color
				if (p_mclib.max_chroma(i.hue, i.value) >= chroma)	// Don't loop over invalid colors
					check_color(result, p_parameter, i);	// Add valid colors
			}
		}
	}
}

// Generate the palette using the VCH algorithm
void Generate_Palette_Base::generate_vch(HVCArray& result, double p_parameter, MCLib_Wrapper& p_mclib) const
{
	HVCTrip	i = { 0 };									// Current color
	int32 value;
	int32 step_size = RAW_MAX / m_step_count;	// Loop control variables

	for (value = 0; 255 >= value; value += step_size)	// Start with the Gray (choma==0) colors
	{
		i.value = (int8)value;								// Copy the loop control to the color
		check_color(result, p_parameter, i);			// Add valid colors
	}

	for (value = 0; 255 >= value; value += step_size)	// Add the non-Gray (chroma!=0) colors
	{
		i.value = (int8)value;								// Copy the loop control to the color
		for (int chroma = step_size; 255 >= chroma; chroma += step_size)
		{
			i.chroma = chroma;							// Copy the loop control to the color
			for (int hue = 0; 255 >= hue; hue += step_size)
			{
				i.hue = hue;							// Copy the loop control to the color
				if (p_mclib.max_chroma(i.hue, i.value) >= chroma)	// Don't loop over invalid colors
					check_color(result, p_parameter, i);	// Add valid colors
			}
		}
	}
}

// Generate the palette using the VHC algorithm
void Generate_Palette_Base::generate_vhc( HVCArray& result, double p_parameter, MCLib_Wrapper& p_mclib ) const
{
	HVCTrip	i = { 0 };									// Current color
	int32	value;
	int32	step_size = RAW_MAX / m_step_count;	// Loop control variables

	for( value = 0; 255 >= value; value += step_size )	// Start with the Gray (choma==0) colors
	{
		i.value = (int8)value;								// Copy the loop control to the color
		check_color( result, p_parameter, i );			// Add valid colors
	}

	for( value = 0; 255 >= value; value += step_size )	// Add the non-Gray (chroma!=0) colors
	{
		i.value = (int8)value;								// Copy the loop control to the color
		for( int hue = 0; 255 >= hue; hue += step_size )
		{
			i.hue = hue;								// Copy the loop control to the color

			const uint8 end_chroma = (uint8)p_mclib.max_chroma( i.hue, i.value );	// Don't loop over invalid colors

			// i.chroma can be used as the loop control here
			for( i.chroma = (uint8)step_size; end_chroma >= i.chroma; i.chroma += (uint8)step_size )
				check_color( result, p_parameter, i );	// Add valid colors
		}
	}
}

// EOF
