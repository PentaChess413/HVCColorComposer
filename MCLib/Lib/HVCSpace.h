//=========================================================================
//
//	FILE:			HVCSpace.h
//						Copyright © 2004. DVL. All Rights Reserved.
//	
//	PURPOSE:	
//	
//	HISTORY:	1st Version - June 2004.
//				17nov05	alx	Ported to Windows
//				14nov05	alx	Verified this version as 1.3
//	
//	NOTES:		- none -
//
//=========================================================================
#pragma once

#ifndef HVCSpace_h
#define HVCSpace_h

// MCLib
#include "MCDefs.h"

// System
#include <vector>

using namespace std;


class HVCSpace
	{
	public:
		// Construction / destruction
															HVCSpace();
															HVCSpace(long which);
															HVCSpace(unsigned short h, unsigned short v, unsigned short c);
															HVCSpace(const HVCSpace& from);
		virtual									 ~HVCSpace();
		
						void							PopulateChroma(short* data, bool direct = false);
						void							PopulateChroma(unsigned char* lengths, short* data, bool direct = false);
						void							BuildPrep();
						void							BuildMap();
						
						bool							GetRGB(HVCTrip hvc, RGBTrip& rgb);
		inline	bool							GetRGBI(HVCTrip hvc, RGBTrip& rgb);
						void							Convert2ImageLine(unsigned short lineLength, HVCTrip* hvc, RGBQuad* rgb, char* outOfGamutMask = nil);
						void							Convert2ImageLine(unsigned short lineLength, HVCTrip* hvc, RGBQuad* rgb, RGBQuad outOfGamutColor);
						
						HVCTrip						GetHVC(RGBTrip rgb);
		inline	HVCTrip						GetHVCI(RGBTrip rgb);
						void							ConvertImageLine(unsigned short lineLength, RGBQuad* rgb, HVCTrip* hvc);
						
						float							GetMaxChroma(unsigned char h, unsigned char v);
						void							InterpolateLine(RGBTripS* out, long& len0, long& terminal, RGBTripS* in1, float frac1, long len1, RGBTripS* in2, long len2);
						void							InterpolateLine2(RGBTripS* out, long& len0, long& t0, RGBTripS* in1, float frac1, long len1, long term1, RGBTripS* in2, long len2, long term2);

	protected:
		inline	long							FindHue(RGBTrip rgb);
				
	// Class data
	protected:
	
	public:
						unsigned short		numH;
						unsigned short		numV;
						unsigned short		numC;
						float							cScale;
						
						short							*sparseData;
						unsigned char			*cllSparse;
						vector<Sextuplet>	sparseV[3];
						unsigned short		sVIndex[3][256][2];
						vector<RGBTrip>		data;
						unsigned char			cll[256 * 256];
						long							bookmarks[256 * 256];
		
		
		
	//========================================================================
	// Platform specific routines
	//========================================================================
	#ifdef __BUILD_MAC__
	#endif

	#ifdef __BUILD_WIN__
	#endif
	};


#endif HVCSpace_h