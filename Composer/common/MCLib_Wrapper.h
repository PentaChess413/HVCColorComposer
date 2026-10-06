/*
 *
 *	Copyright 2005 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#ifndef MCLib_Wrapper_h
#define MCLib_Wrapper_h

#include "MCLib.h"

// C++ wrapper for the MCLib C interface
class	MCLib_Wrapper
{
public:
	// Constructors:
			MCLib_Wrapper(const long which = 0);
			MCLib_Wrapper(const unsigned short numH, const unsigned short numV,
							const unsigned short numC);
			~MCLib_Wrapper();

	// Accessors:
	void	populate_chroma(short* data, const bool direct = false);
	void	populate_chroma1(unsigned char* lengths, short* data, const bool direct = false);
	void	build_map();

	bool	hvc_2_rgb(const HVCTrip& hvc, RGBTrip& rgb) const;
	void	hvc_2_image_line(const unsigned short lineLength, HVCTrip* hvc, RGBQuad* rgb,
								char* outOfGamutMask = nil) const;
	void	hvc_2_image_line_set_OOG(const unsigned short lineLength, HVCTrip* hvc, RGBQuad* rgb,
										const RGBQuad& outOfGamutColor) const;

	void	rgb_2_hvc(const RGBTrip& rgb, HVCTrip& hvc) const;
	void	image_line_2_hvc(unsigned short lineLength, RGBQuad* rgb, HVCTrip* hvc) const;

	void	adjust_v(const char hue, const float tol, const float w, const float m, const float b);
	void	adjust_c(const char hue, const float tol, const float w, const float m, const float b);

	float	max_chroma(const unsigned char hue, const unsigned char value) const;

private:
	// Colorspace reference
	long	m_reference;
};

inline MCLib_Wrapper::MCLib_Wrapper(const long which)	:
						m_reference(CreateDefaultHVC(which))
{
}

inline MCLib_Wrapper::MCLib_Wrapper(const unsigned short numH, const unsigned short numV,
						const unsigned short numC)	:
						m_reference(CreateHVC(numH, numV, numC))
{
}

inline MCLib_Wrapper::~MCLib_Wrapper()
{
	ReleaseHVC(m_reference);
}

inline void MCLib_Wrapper::populate_chroma(short* data, const bool direct)
{
	PopulateChroma(m_reference, data, direct);
}

inline void MCLib_Wrapper::populate_chroma1(unsigned char* lengths, short* data, const bool direct)
{
	PopulateChroma1(m_reference, lengths, data, direct);
}

inline void MCLib_Wrapper::build_map()
{
	BuildHVCMap(m_reference);
}

inline bool MCLib_Wrapper::hvc_2_rgb(const HVCTrip& hvc, RGBTrip& rgb) const
{
	return HVC2RGB(m_reference, hvc, rgb);
}

inline void MCLib_Wrapper::hvc_2_image_line(const unsigned short lineLength, HVCTrip* hvc,
							RGBQuad* rgb, char* outOfGamutMask) const
{
	HVC2ImageLine(m_reference, lineLength, hvc, rgb, outOfGamutMask);
}

inline void MCLib_Wrapper::hvc_2_image_line_set_OOG(const unsigned short lineLength, HVCTrip* hvc,
							RGBQuad* rgb, const RGBQuad& outOfGamutColor) const
{
	HVC2ImageLineSetOOG(m_reference, lineLength, hvc, rgb, outOfGamutColor);
}

inline void MCLib_Wrapper::rgb_2_hvc(const RGBTrip& rgb, HVCTrip& hvc) const
{
	RGB2HVC(m_reference, rgb, hvc);
}

inline void MCLib_Wrapper::image_line_2_hvc(unsigned short lineLength, RGBQuad* rgb,
							HVCTrip* hvc) const
{
	ImageLine2HVC(m_reference, lineLength, rgb, hvc);
}

inline void MCLib_Wrapper::adjust_v(const char hue, const float tol, const float w, const float m,
							const float b)
{
	AdjustV(m_reference, hue, tol, w, m, b);
}

inline void MCLib_Wrapper::adjust_c(const char hue, const float tol, const float w, const float m,
							const float b)
{
	AdjustC(m_reference, hue, tol, w, m, b);
}

inline float MCLib_Wrapper::max_chroma(const unsigned char hue, const unsigned char value) const
{
	return GetMaxChroma(m_reference, hue, value);
}

#endif

// EOF
