//=========================================================================
//
//	FILE:			MCLib.h
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

#ifndef MCLib_h
#define MCLib_h


#include	"MCDefs.h"

#ifdef _WIN32
#define MCAPI __stdcall
#else
#define MCAPI
#endif

#ifdef __cplusplus
extern "C" {
#endif


//=========================================================================


extern	long			MCAPI	CreateDefaultHVC(long which = 0);
extern	long			MCAPI	CreateHVC(unsigned short numH, unsigned short numV, unsigned short numC);
extern	void			MCAPI	PopulateChroma(long hvcRef, short* data, bool direct = false);
extern	void			MCAPI	PopulateChroma1(long hvcRef, unsigned char* lengths, short* data, bool direct = false);
extern	void			MCAPI	BuildHVCMap(long hvcRef);
extern	void			MCAPI	ReleaseHVC(long hvcRef);

extern	bool			MCAPI	HVC2RGB(long hvcRef, HVCTrip hvc, RGBTrip& rgb);
extern	void			MCAPI	HVC2ImageLine(long hvcRef, unsigned short lineLength, HVCTrip* hvc, RGBQuad* rgb, char* outOfGamutMask = nil);
extern	void			MCAPI	HVC2ImageLineSetOOG(long hvcRef, unsigned short lineLength, HVCTrip* hvc, RGBQuad* rgb, RGBQuad outOfGamutColor);

extern	void			MCAPI	RGB2HVC(long hvcRef, RGBTrip rgb, HVCTrip& hvc);
extern	void			MCAPI	ImageLine2HVC(long hvcRef, unsigned short lineLength, RGBQuad* rgb, HVCTrip* hvc);

extern	void			MCAPI	AdjustV(long hvcRef, char hue, float tol, float w, float m, float b);
extern	void			MCAPI	AdjustC(long hvcRef, char hue, float tol, float w, float m, float b);

// bogus routines
extern	float			MCAPI	GetMaxChroma(long hvcRef, unsigned char h, unsigned v);


//=========================================================================


#ifdef __cplusplus
} // end extern "C"
#endif

#endif MCLib_h