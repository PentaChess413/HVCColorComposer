/*
 *
 *	Copyright 2005 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#ifndef HVC_RGB_Color_Pair_h
#define HVC_RGB_Color_Pair_h

#include "MCDefs.h"
#include "ASTypes.h"

class HVC_RGB_Color_Pair
{
	public:
		ASRGBColor&			rgb_color()									{ return m_rgb_color; }
		const ASRGBColor&	rgb_color() const							{ return m_rgb_color; }
		HVCTrip&			hvc_color()									{ return m_hvc_color; }
		const HVCTrip&		hvc_color() const							{ return m_hvc_color; }

		void clear()
		{
			m_rgb_color.red = m_rgb_color.green = m_rgb_color.blue = 0;
			m_hvc_color.hue = m_hvc_color.value = m_hvc_color.chroma = 0;
		}

	private:
		ASRGBColor	m_rgb_color;
		HVCTrip		m_hvc_color;
};


#endif

// EOF
