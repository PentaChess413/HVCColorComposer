//=========================================================================
//
//	FILE:			HVCUtils.h
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

#ifndef HVCUtils_h
#define HVCUtils_h

// MCLib
#include "MCDefs.h"

// System



int SortRGB(const Sextuplet p1, const Sextuplet p2);
int SortRBG(const Sextuplet p1, const Sextuplet p2);
int SortGRB(const Sextuplet p1, const Sextuplet p2);
int SortGBR(const Sextuplet p1, const Sextuplet p2);
int SortBRG(const Sextuplet p1, const Sextuplet p2);
int SortBGR(const Sextuplet p1, const Sextuplet p2);


enum SortOrder
	{
	kRGB = 0,
	kRBG,
	kGRB,
	kGBR,
	kBRG,
	kBGR
	};


#define BMIndex(hue, val)									((hue << 8) + val)
#define BM(hue, val)											(bookmarks[BMIndex(hue, val)])
#define CL(hue, val)											(cll[BMIndex(hue, val)])
#define LAST(hue, val)										(last[BMIndex(hue, val)])

#define SIndex(hue, val)									((hue * numV) + val)
#define SDataA(hue, val)									(sbm[SIndex(hue, val)])
#define SData(hue, val)										(sparseData[SDataA(hue, val)])
#define SLL(hue, val)											(cllSparse[SIndex(hue, val)])

#define InterpC(v1, v2)										((*v1 * vDnFrac) + (*v2 * vUpFrac))
#define IInterpC(v1, v2)									(rint(InterpC(v1, v2)))
#define OoG(r, g, b)											((r > 255) || (g > 255) || (b > 255) || (r < 0) || (g < 0) || (b < 0))
#define OutOfGamut(x)											(OoG(x.red, x.green, x.blue))

#define	RGBDist(a, b)											(((a.red - b.red) * (a.red - b.red)) + ((a.green - b.green) * (a.green - b.green)) + ((a.blue - b.blue) * (a.blue - b.blue)))	// since we're just using as a comparator and not looking for a real distance, we don't need to sqrt
//#define	RGBDist(a, b)											sqrt(((a.red - b.red) * (a.red - b.red)) + ((a.green - b.green) * (a.green - b.green)) + ((a.blue - b.blue) * (a.blue - b.blue)))
//#define	RGBDist(a, b)											(abs(rint(a.red - b.red)) + abs(rint(a.green - b.green)) + abs(rint(a.blue - b.blue)))
#define	RGBWDist(a, b, w)									(abs(rint((a.red - b.red) * w[0])) + abs(rint((a.green - b.green) * w[1])) + abs(rint((a.blue - b.blue) * w[2])))
#define	ChannelDelta(a)										(abs(rint(a.red - a.green)) + abs(rint(a.green - a.blue)))
#define	ChannelMax(a)											((a.red > a.green) ? ((a.red > a.blue) ? a.red : a.blue) : ((a.green > a.blue) ? a.green : a.blue))



#endif HVCSpace_h