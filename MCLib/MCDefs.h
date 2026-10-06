//=========================================================================
//
//	FILE:			MCDefs.h
//						Copyright � 2004. DVL. All Rights Reserved.
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
//#pragma once

#ifndef MCDefs_h
#define MCDefs_h


#define		kNoErr					0
#define		kGenErr				 -1
#define		kUnknown			 -1

#if defined( _WIN32 ) || defined( WIN32 )
// who uses nil?  I thought that was for Pascal?
#ifndef nil
#define nil NULL
#endif
#endif

enum SpaceMode
	{
//kUnknown,										//defined as -1 above
	kIndex = 0,
	kLean,
	kExplicit
	};


struct RGBTrip
	{
	unsigned char		red;
	unsigned char		green;
	unsigned char		blue;
	};

struct RGBTripS
	{
	short						red;
	short						green;
	short						blue;
	};

struct RGBTripF
	{
	float						red;
	float						green;
	float						blue;
	};

struct HVCTrip
	{
	unsigned char		hue;
	unsigned char		value;
	unsigned char		chroma;
	};


struct Sextuplet
	{
	short						hue;
	short						value;
	short						chroma;
	short						red;
	short						green;
	short						blue;
	};
	
	
#ifdef __BUILD_WIN__
struct RGBQuad
	{
	unsigned char		blue;
	unsigned char		green;
	unsigned char		red;
	unsigned char		alpha;
	};
#else
struct RGBQuad
	{
	unsigned char		alpha;
	unsigned char		red;
	unsigned char		green;
	unsigned char		blue;
	};
#endif





#endif //MCDefs_h