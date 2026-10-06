//=========================================================================
//
//	FILE:			HVCSpace.cpp
//						Copyright © 2004. DVL. All Rights Reserved.
//	
//	PURPOSE:	
//	
//	HISTORY:	1st Version - June 2004.
//				17nov05	alx	Ported to Windows
//				14nov05	alx	Added pragma and verified this version as 1.3
//	
//	NOTES:		- none -
//
//=========================================================================


// MCLib
#include "DefaultSpace.h"
#include "HVCUtils.h"

// System
#include <math.h>
#include <string.h>
#include <algorithm>

// This
#include "HVCSpace.h"


// IMPORTANT:  do not turn off this pragma, if you do the code may generate improper results.
// you will get 99 errors in GetMaxChroma due to rounding differences compared to the 1.3 version
// of the shipping library.
#if __MWERKS__
#pragma push
#pragma strict_ieee_fp on
#endif

#if defined( _WIN32 ) || defined( WIN32 )
inline int rint( double fp ) 
{ 
	int result;
	_asm 
	{ 
		fld		fp		// load argument
		fistp	result	// store as integer
	} 
	return result; 
}

inline int rint( float fp )
{ 
	int result;
	_asm 
	{ 
		fld		fp		// load argument
		fistp	result	// store as integer
	} 
	return result; 
}
#endif


#pragma mark === Construction / Destruction ===
//=========================================================================
//	FUNCTION:	HVCSpace
//	
//	PURPOSE:	Default constructor.
//=========================================================================
HVCSpace::HVCSpace()
	{
	numH = 0;
	numV = 0;
	numC = 0;
	sparseData = nil;
	cllSparse = nil;
	}
	
//=========================================================================
//	FUNCTION:	HVCSpace
//	
//	PURPOSE:	Constructor using a default space specified by the which parameter.
//=========================================================================
HVCSpace::HVCSpace(long which)
	{
	long								numDataPoints;
	long								numCLL;
	Sextuplet						temp;
	RGBTripS						*ptr;
	long								i;
	long								j;
	long								k;
	long								l;
	
	
	switch (which)
		{
		case 0:
			{
			short						tempSparse[] = kDefaultSpace;
			unsigned char		tempLL[] = kLineLengths;
			
			
			numH = 40;
			numV = 10;
			numC = 25;
			numCLL = numH * numV;
			numDataPoints = 0;
			ptr = (RGBTripS*)&tempSparse;
			
			cllSparse = new unsigned char[numCLL];
			for (i=0, numCLL=0; i<numH; i++)
				{
				temp.hue = i;
				for (j=0; j<numV; j++, numCLL++)
					{
					temp.value = j;
					cllSparse[numCLL] = tempLL[numCLL];
					numDataPoints += cllSparse[numCLL];
					
					for (k=0; k<cllSparse[numCLL]; k++, ptr++)
						{
						temp.chroma = k;
						temp.red = ptr->red;
						temp.green = ptr->green;
						temp.blue = ptr->blue;
						
						for (l=0; l<3; l++)
							{
							sparseV[l].push_back(temp);
							}
						}
					}
				}
			
			numDataPoints = numDataPoints * 3;
			sparseData = new short[numDataPoints];
			memcpy(sparseData, tempSparse, numDataPoints * sizeof(short));
			
			break;
			}
		}
	
	BuildPrep();
	BuildMap();
	}
	
	
//=========================================================================
//	FUNCTION:	HVCSpace
//	
//	PURPOSE:	Constructor.
//=========================================================================
HVCSpace::HVCSpace(unsigned short h, unsigned short v, unsigned short c)
	{
	numH = h;
	numV = v;
	numC = c;
	sparseData = nil;
	cllSparse = nil;
	}
	
	
//=========================================================================
//	FUNCTION:	HVCSpace
//	
//	PURPOSE:	Copy constructor.
//=========================================================================
HVCSpace::HVCSpace(const HVCSpace&)
	{
	}


//=========================================================================
//	FUNCTION:	~HVCSpace
//	
//	PURPOSE:	Destructor.
//=========================================================================
HVCSpace::~HVCSpace()
	{
	delete [] sparseData;
	delete [] cllSparse;
	}



#pragma mark -
#pragma mark === Stuff ===
//=========================================================================
//	FUNCTION:	PopulateChroma
//	
//	PURPOSE:	
//=========================================================================
void HVCSpace::PopulateChroma(short* data, bool direct)
	{
	long								numDataPoints;
	long								numCLL;
	Sextuplet						temp;
	RGBTripS						*ptr;
	unsigned char				i;
	unsigned char				j;
	unsigned char				k;
	unsigned char				l;
	
	
	if (direct)
		{
		}
	else
		{
		numCLL = 0;
		numDataPoints = 0;
		cllSparse = new unsigned char[numH * numV];
		ptr = (RGBTripS*)&data;
		
		for (i=0; i<numH; i++)
			{
			temp.hue = i;
			for (j=0; j<numV; j++, numCLL++)
				{
				temp.value = j;
				for (k=0; k<numC; k++, ptr++)
					{
					temp.chroma = k;
					
					temp.red = ptr->red;
					temp.green = ptr->green;
					temp.blue = ptr->blue;
					
					for (l=0; l<3; l++)
						{
						sparseV[l].push_back(temp);
						}
					
					if (OutOfGamut(temp))
						{
						break;
						}
					}
				
				ptr += numC - k;
				cllSparse[numCLL] = k;
				numDataPoints += k;
				}
			}
		}
	}
	
	
//=========================================================================
//	FUNCTION:	PopulateChroma
//	
//	PURPOSE:	
//=========================================================================
void HVCSpace::PopulateChroma(unsigned char* lengths, short* data, bool direct)
	{
	long								numDataPoints;
	long								numCLL;
	Sextuplet						temp;
	RGBTripS						*ptr;
	unsigned char				tempLen;
	unsigned char				i;
	unsigned char				j;
	unsigned char				k;
	unsigned char				l;
	
	
	if (direct)
		{
		}
	else
		{
		numCLL = 0;
		numDataPoints = 0;
		cllSparse = new unsigned char[numH * numV];
		ptr = (RGBTripS*)&data;
		
		for (i=0; i<numH; i++)
			{
			temp.hue = i;
			for (j=0; j<numV; j++, numCLL++)
				{
				temp.value = j;
				tempLen = lengths[numCLL];
				cllSparse[numCLL] = tempLen;
				numDataPoints += tempLen;
				
				for (k=0; k<tempLen; k++, ptr++)
					{
					temp.chroma = k;
					
					temp.red = ptr->red;
					temp.green = ptr->green;
					temp.blue = ptr->blue;
					
					for (l=0; l<3; l++)
						{
						sparseV[l].push_back(temp);
						}
					}
				}
			}
		
		numDataPoints = numDataPoints * 3;
		sparseData = new short[numDataPoints];
		memcpy(sparseData, data, numDataPoints * sizeof(short));
		
		BuildPrep();
		}
	}
	
	
//=========================================================================
//	FUNCTION:	BuildPrep
//	
//	PURPOSE:	
//=========================================================================
void HVCSpace::BuildPrep()
	{
	long								sparseSize;
	short								last[3];
	long								i;
	long								j;
	
	
	// build sparse tables for rgb to hvc conversion
	sort(sparseV[0].begin(), sparseV[0].end(), SortRGB);
	sort(sparseV[1].begin(), sparseV[1].end(), SortGRB);
	sort(sparseV[2].begin(), sparseV[2].end(), SortBRG);
		
	memset(last, 0, 3 * sizeof(short));
	memset(sVIndex, 0, 256 * 3 * 2);
	sparseSize = sparseV[0].size();
	for (i=0; i<sparseSize; i++)
		{
		// rgb & rbg
		if ((sparseV[0][i].red > last[0]) && (sparseV[0][i].red < 256))
			{
			for (j=last[0]; j<sparseV[0][i].red; j++)
				{
				sVIndex[0][j][1] = i;
				}
				
			sVIndex[0][j][0] = i;
			last[0] = j;
			}
		
		// grb & gbr
		if ((sparseV[1][i].green > last[1]) && (sparseV[1][i].green < 256))
			{
			for (j=last[1]; j<sparseV[1][i].green; j++)
				{
				sVIndex[1][j][1] = i;
				}
				
			sVIndex[1][j][0] = i;
			last[1] = j;
			}
		
		// brg & bgr
		if ((sparseV[2][i].blue > last[2]) && (sparseV[2][i].blue < 256))
			{
			for (j=last[2]; j<sparseV[2][i].blue; j++)
				{
				sVIndex[2][j][1] = i;
				}
				
			sVIndex[2][j][0] = i;
			last[2] = j;
			}
		}
	
	for (i=0; i<3; i++)
		{
		sVIndex[i][255][1] = sparseSize - 1;
		}
	}
	
	
//=========================================================================
//	FUNCTION:	BuildMap
//	
//	PURPOSE:	
//=========================================================================
void HVCSpace::BuildMap()
	{
	float								hInc;
	float								vInc;
	float								vSize;
	float								temp;
	unsigned char				hDn;
	unsigned char				hUp;
	unsigned char				vDn;
	unsigned char				vUp;
	float								hDnFrac;
	short								*sparseEntry;
	float								v1;
	float								v2;
	float								c1;
	float								c2;
	long								ll;
	long								i;
	long								j;
	long								k;
	long								la;
	long								lb;
	long								l1;
	long								l2;
	long								runCount;
	long								bm;
	vector<long>				sbm;
	RGBTrip							rgbTemp;
	RGBTrip							rgbTemp1;
	RGBTripS						s0[256];
	RGBTripS						s1[256];
	RGBTripS						s2[256];
	
	// clear the bookmark array!  otherwise we get out of range problems (at index 255) and crash - alx
	for( i = 0; i < sizeof( bookmarks ) / sizeof( long ); i++ )
		bookmarks[i] = 0;
	
	
	// build an index into the sparse data
	runCount = 0;														// hue 0, value 0 starts at beginning of sparse data
	sbm.push_back(0);												// insert this into the bookmarks
	for (i=0, k=0; i<numH; i++)							// iterate through hues of sparse data
		{
		for (j=0; j<numV; j++, k++)						// iterate through values of sparse data
			{
			runCount += cllSparse[k];						// calculate location
			sbm.push_back(runCount * 3);				// insert this location into the bookmarks (3 shorts per entry)
			}
		}
	
	hInc = (numH - 1) / 255.0;							// calculate normailization factors
	vInc = (numV + 1) / 255.0;
	vSize = 1.0 / vInc;
	cScale = (numC - 1) / 255.0;
	
	// ready to buid
	for (i=0; i<=255; i++)									// iterate through each of the 256 (iterpolated) hues
		{
		temp = hInc * i;											// the hue scaled to 0-255
		hDn = floor(temp);
		hUp = ceil(temp);
		
		if ((hDn == hUp) || (hDn >= 255))			// true if the hue is explicitly defined, otherwise it must be interpolated
			{
			// extrpolate data prior to the first value line in the dataset
			ll = SLL(hDn, 0);										// find number of chromas for this first value in the dataset for this hue
			sparseEntry = &SData(hDn, 0);
			for (j=0; j<vSize; j++)							// iterate through 0 to beginning of the dataset
				{
				temp = j * vInc;
				BM(i, j) = data.size();						// bookmark location of this h&v in the data table
				CL(i, j) = floor(temp * ll / cScale);			// insert line length
				
				for (k=0; (k/cScale)<CL(i, j); k++)
					{
					rgbTemp.red = sparseEntry[0] * temp;
					rgbTemp.green = sparseEntry[1] * temp;
					rgbTemp.blue = sparseEntry[2] * temp;
					
					data.push_back(rgbTemp);
					}
				
				rgbTemp.red = sparseEntry[0] * temp;
				rgbTemp.green = sparseEntry[1] * temp;
				rgbTemp.blue = sparseEntry[2] * temp;
				
				data.push_back(rgbTemp);
				}
			
			// interpolate the data from the given dataset
			for (; j<=255-vSize; j++)						// iterate through each of the 256 (iterpolated) values
				{
				BM(i, j) = data.size();						// bookmark location of this h&v in the data table
				temp = (vInc * j) - 1;						// the value scaled to 0-255
				vDn = floor(temp);
				vUp = ceil(temp);
				
				if ((temp - vDn) < .02)						// true if the value is explicitly defined, otherwise it must be interpolated
					{
					ll = SLL(hDn, vDn);							// find number of chromas for this hue and value
					CL(i, j) = floor(ll / cScale);	// insert line length
					sparseEntry = &SData(hDn, vDn);
					for (k=0; k<ll; k++)						// iterate through the chromas
						{
						rgbTemp.red = sparseEntry[0];
						rgbTemp.green = sparseEntry[1];
						rgbTemp.blue = sparseEntry[2];
						sparseEntry += 3;
						
						data.push_back(rgbTemp);
						}
					}
				else
					{
					// we have an explicit hue, but we need to interpolate the value
					InterpolateLine(s0, ll, la, (RGBTripS*)&SData(hDn, vDn), vUp - temp, SLL(hDn, vDn), (RGBTripS*)&SData(hDn, vUp), SLL(hDn, vUp));
					CL(i, j) = la;									// insert line length
					for (k=0; k<ll; k++)						// move the data into the map
						{
						rgbTemp.red = s0[k].red;
						rgbTemp.green = s0[k].green;
						rgbTemp.blue = s0[k].blue;
						
						data.push_back(rgbTemp);
						}
					}
				}
			
			// extrapolate the data after the last value line in the dataset
			ll = SLL(hDn, numV - 1);						// find number of chromas for this first value in the dataset for this hue
			sparseEntry = &SData(hDn, numV - 1) + ((ll - 1) * 3);
			for (; j<255; j++)									// iterate through 0 to beginning of the dataset
				{
				temp = (255 - j) * vInc;
				BM(i, j) = data.size();						// bookmark location of this h&v in the data table
				CL(i, j) = floor(temp * ll / cScale);			// insert line length
				
				if (floor(CL(i, j) * cScale) >= 1)
					{
					RGBTripS		x;
					
					
					v1 = (255.0 - j) / vSize;
					v2 = 1 - v1;
					for (k=0; (k/cScale)<CL(i, j); k++)
						{
						c1 = k / (cScale * CL(i, j));
						c2 = 1 - c1;
						x = *(RGBTripS*)(&SData(hDn, numV - 1) + (k * 3));
						rgbTemp.red = (((sparseEntry[0] * v1) + (255 * v2)) * c1) + (x.red * c2);
						rgbTemp.green = (((sparseEntry[1] * v1) + (255 * v2)) * c1) + (x.green * c2);
						rgbTemp.blue = (((sparseEntry[2] * v1) + (255 * v2)) * c1) + (x.blue * c2);
						
						data.push_back(rgbTemp);
						}
					}
				
				rgbTemp.red = (sparseEntry[0] * temp) + (255 * (1 - temp));
				rgbTemp.green = (sparseEntry[1] * temp) + (255 * (1 - temp));
				rgbTemp.blue = (sparseEntry[2] * temp) + (255 * (1 - temp));
				
				data.push_back(rgbTemp);
				}
			
			CL(i, 0) = 1;
			CL(i, 255) = 1;
			}
		else
			{
			hDnFrac = hUp - temp;temp = (vInc * j) - 1;
			temp = (vInc * ceil(vSize)) - 1;
			vDn = floor(temp);
			vUp = ceil(temp);

			if ((temp - vDn) < .02)							// precompute the first value line so we have a baseline to interpolate from
				{
				InterpolateLine(s0, ll, la, (RGBTripS*)&SData(hDn, vDn), hDnFrac, SLL(hDn, vDn), (RGBTripS*)&SData(hUp, vDn), SLL(hUp, vDn));
				}
			else
				{
				InterpolateLine(s1, l1, la, (RGBTripS*)&SData(hDn, vDn), hDnFrac, SLL(hDn, vDn), (RGBTripS*)&SData(hUp, vDn), SLL(hUp, vDn));
				InterpolateLine(s2, l2, lb, (RGBTripS*)&SData(hDn, vUp), hDnFrac, SLL(hDn, vUp), (RGBTripS*)&SData(hUp, vUp), SLL(hUp, vUp));
				
				InterpolateLine2(s0, ll, la, s1, vUp - temp, l1, la, s2, l2, lb);
				}

			rgbTemp1.red = s0[ll - 1].red;
			rgbTemp1.green = s0[ll - 1].green;
			rgbTemp1.blue = s0[ll - 1].blue;
			for (j=0; j<vSize; j++)							// iterate through 0 to beginning of the dataset
				{
				temp = j * vInc;
				BM(i, j) = data.size();						// bookmark location of this h&v in the data table
				CL(i, j) = floor(temp * la);			// insert line length
				
				if (floor(CL(i, j) * cScale) >= 1)
					{
					v1 = j / vSize;
					for (k=0; (k/cScale)<CL(i, j); k++)
						{
						c1 = k / (cScale * CL(i, j));
						c2 = 1 - c1;
						rgbTemp.red = ((rgbTemp1.red * v1) * c1) + (s0[k].red * c2);
						rgbTemp.green = ((rgbTemp1.green * v1) * c1) + (s0[k].green * c2);
						rgbTemp.blue = ((rgbTemp1.blue * v1) * c1) + (s0[k].blue * c2);
						
						data.push_back(rgbTemp);
						}
					}
				
				rgbTemp.red = rgbTemp1.red * temp;
				rgbTemp.green = rgbTemp1.green * temp;
				rgbTemp.blue = rgbTemp1.blue * temp;
				
				data.push_back(rgbTemp);
				}
				
			// interpolate the data from the given dataset
			for (; j<=255-vSize; j++)						// iterate through each of the 256 (iterpolated) values
				{
				BM(i, j) = data.size();						// bookmark location of this h&v in the data table
				temp = (vInc * j) - 1;
				vDn = floor(temp);
				vUp = ceil(temp);
				
				if ((temp - vDn) < .02)						// true if the value is explicitly defined, otherwise it must be interpolated (in addition to interpolating the hue)
					{
					InterpolateLine(s0, ll, la, (RGBTripS*)&SData(hDn, vDn), hDnFrac, SLL(hDn, vDn), (RGBTripS*)&SData(hUp, vDn), SLL(hUp, vDn));
					}
				else
					{
					InterpolateLine(s1, l1, la, (RGBTripS*)&SData(hDn, vDn), hDnFrac, SLL(hDn, vDn), (RGBTripS*)&SData(hUp, vDn), SLL(hUp, vDn));
					InterpolateLine(s2, l2, lb, (RGBTripS*)&SData(hDn, vUp), hDnFrac, SLL(hDn, vUp), (RGBTripS*)&SData(hUp, vUp), SLL(hUp, vUp));
					
					InterpolateLine2(s0, ll, la, s1, vUp - temp, l1, la, s2, l2, lb);
					}
				
				CL(i, j) = la;										// insert line length
				for (k=0; k<ll; k++)							// move the data into the map
					{
					rgbTemp.red = s0[k].red;
					rgbTemp.green = s0[k].green;
					rgbTemp.blue = s0[k].blue;
					
					data.push_back(rgbTemp);
					}
				}
			
			// extrapolate the data after the last value line in the dataset
			ll = CL(i, j - 1);									// find number of chromas in the prior value line
			rgbTemp1 = rgbTemp;
			bm = BM(i, j - 1);
			for (; j<255; j++)									// iterate through 0 to beginning of the dataset
				{
				temp = (255 - j) * vInc;
				BM(i, j) = data.size();						// bookmark location of this h&v in the data table
				CL(i, j) = floor(temp * ll);			// insert line length
				
				if (floor(CL(i, j) * cScale) >= 1)
					{
					RGBTrip			x;
					
					
					v1 = (255.0 - j) / vSize;
					v2 = 1 - v1;
					for (k=0; (k/cScale)<CL(i, j); k++)
						{
						c1 = k / (cScale * CL(i, j));
						c2 = 1 - c1;
						x = data[bm + k];
						rgbTemp.red = (((rgbTemp1.red * v1) + (255 * v2)) * c1) + (x.red * c2);
						rgbTemp.green = (((rgbTemp1.green * v1) + (255 * v2)) * c1) + (x.green * c2);
						rgbTemp.blue = (((rgbTemp1.blue * v1) + (255 * v2)) * c1) + (x.blue * c2);
						
						data.push_back(rgbTemp);
						}
					}
				
				rgbTemp.red = (rgbTemp1.red * temp) + (255 * (1 - temp));
				rgbTemp.green = (rgbTemp1.green * temp) + (255 * (1 - temp));
				rgbTemp.blue = (rgbTemp1.blue * temp) + (255 * (1 - temp));
				
				data.push_back(rgbTemp);
				}
			
			CL(i, 0) = 1;
			CL(i, 255) = 1;
			}
		}
	
	// clean up
	sbm.clear();
	}
	
	
#pragma mark -
#pragma mark === hvc->rgb ===
//=========================================================================
//	FUNCTION:	GetRGB
//	
//	PURPOSE:	
//=========================================================================
bool HVCSpace::GetRGB(HVCTrip hvc, RGBTrip& rgb)
	{
	return GetRGBI(hvc, rgb);
	}
	
	
//=========================================================================
//	FUNCTION:	GetRGBI
//	
//	PURPOSE:	
//=========================================================================
inline bool HVCSpace::GetRGBI(HVCTrip hvc, RGBTrip& rgb)
	{
	float		scaled;
	long		hiC;
	long		idx;
	RGBTrip		lo;
	RGBTrip		hi;
	float		loFrac;
	float		hiFrac;
	bool		result = true;
	
	
	scaled = hvc.chroma * cScale;
	hiC = ceil(scaled);
	idx = CL(hvc.hue, hvc.value);
	if (hiC <= 1)
		{
		hi = data[BM(hvc.hue, hvc.value)];
		if (hvc.chroma < idx)
			{
			loFrac = hvc.value * (1.0 - scaled);
			hiFrac = scaled;
			
			rgb.red = rint(loFrac + (hi.red * hiFrac));
			rgb.green = rint(loFrac + (hi.green * hiFrac));
			rgb.blue = rint(loFrac + (hi.blue * hiFrac));
			}
		else
			{
			rgb.red = 255;
			rgb.green = 255;
			rgb.blue = 255;
			
			result = false;
			}
		}
	else
		{
		if (hiC < floor(idx * cScale))
			{
			lo = data[BM(hvc.hue, hvc.value) + floor(scaled) - 1];
			hi = data[BM(hvc.hue, hvc.value) + hiC - 1];
			
			loFrac = hiC - scaled;
			hiFrac = 1.0 - loFrac;
			
			rgb.red = rint((lo.red * loFrac) + (hi.red * hiFrac));
			rgb.green = rint((lo.green * loFrac) + (hi.green * hiFrac));
			rgb.blue = rint((lo.blue * loFrac) + (hi.blue * hiFrac));
			}
		else
			{
			if (hvc.chroma <= idx)
				{
				long			loC;
				
				
				loC = floor((hiC - 1) / cScale);
				lo = data[BM(hvc.hue, hvc.value) + floor(scaled) - 1];
				hi = data[BM(hvc.hue, hvc.value) + hiC - 1];
				
				hiFrac = (loC - hvc.chroma) / (loC - idx);
				loFrac = 1.0 - hiFrac;
				
				rgb.red = rint((lo.red * loFrac) + (hi.red * hiFrac));
				rgb.green = rint((lo.green * loFrac) + (hi.green * hiFrac));
				rgb.blue = rint((lo.blue * loFrac) + (hi.blue * hiFrac));
				}
			else
				{
				rgb.red = 255;
				rgb.green = 255;
				rgb.blue = 255;
			
				result = false;
				}
			}
		}
		
	
	return result;
	}

	
//=========================================================================
//	FUNCTION:	Convert2ImageLine
//	
//	PURPOSE:	
//=========================================================================
void HVCSpace::Convert2ImageLine(unsigned short lineLength, HVCTrip* hvc, RGBQuad* rgb, char* outOfGamutMask)
	{
	RGBTrip					temp;
	RGBQuad					*out = rgb;
	HVCTrip					*in = hvc;
	char						*oog = outOfGamutMask;
	long						i;
	
	
	for (i=0; i<lineLength; i++, in++, out++, oog++)
		{
		if (outOfGamutMask == nil)
			{
			GetRGBI(*in, temp);
			out->red = temp.red;
			out->green = temp.green;
			out->blue = temp.blue;
			}
		else
			{
			if (GetRGBI(*in, temp))
				{
				out->red = temp.red;
				out->green = temp.green;
				out->blue = temp.blue;
				*oog = true;
				}
			else
				{
				*oog = false;
				}
			}
		}
	}
	
	
//=========================================================================
//	FUNCTION:	Convert2ImageLine
//	
//	PURPOSE:	
//=========================================================================
void HVCSpace::Convert2ImageLine(unsigned short lineLength, HVCTrip* hvc, RGBQuad* rgb, RGBQuad outOfGamutColor)
	{
	RGBTrip					temp;
	RGBQuad					*out = rgb;
	HVCTrip					*in = hvc;
	long						i;
	
	
	for (i=0; i<lineLength; i++, in++, out++)
		{
		if (GetRGBI(*in, temp))
			{
			out->red = temp.red;
			out->green = temp.green;
			out->blue = temp.blue;
			}
		else
			{
			*out = outOfGamutColor;
			}
		}
	}



#define	kW0			1.0
#define	kW1			1.0
#define	kW2			1.0
#define	kSpread	2
#pragma mark -
#pragma mark === rgb->hvc ===
//=========================================================================
//	FUNCTION:	GetHVC
//	
//	PURPOSE:	
//=========================================================================
HVCTrip HVCSpace::GetHVC(RGBTrip rgb)
	{
	return GetHVCI(rgb);
	}
	
	
//=========================================================================
//	FUNCTION:	GetHVCI
//	
//	PURPOSE:	
//=========================================================================
inline HVCTrip HVCSpace::GetHVCI(RGBTrip rgb)
	{
	HVCTrip					hvc;
	float						rgbMax;
	float						rgbMid;
	float						rgbMin;
	long						order;
	long						tempHue;
	float						a;
	float						a1;
	float						b;
	float						c = 0.0f;	// was uninitialized !!@ alx
	
	
	rgbMax = std::max(rgb.red, std::max(rgb.green, rgb.blue));
	rgbMin = std::min(rgb.red, std::min(rgb.green, rgb.blue));
	if (rgb.green == rint(rgbMax))
		{
		if (rgb.blue == rint(rgbMin))
			{
			order = kGRB;
			rgbMid = rgb.red;
			}
		else
			{
			order = kGBR;
			rgbMid = rgb.blue;
			}
		}
	else
		{
		if (rgb.red == rint(rgbMax))
			{
			if (rgb.blue == rint(rgbMin))
				{
				order = kRGB;
				rgbMid = rgb.green;
				}
			else
				{
				order = kRBG;
				rgbMid = rgb.blue;
				}
			}
		else
			{
			if (rgb.red == rint(rgbMin))
				{
				order = kBGR;
				rgbMid = rgb.green;
				}
			else
				{
				order = kBRG;
				rgbMid = rgb.red;
				}
			}
		}
	
if ((rgb.red == 255) && (rgb.green == 128) && ((rgb.blue == 120) || (rgb.blue == 136)))
	{
	tempHue = 0;
	}
	a = rgbMid / rgbMax;
	a1 = ((rgbMid - rgbMin) / rgbMax);
	b = rgbMin / rgbMax;
	if (rint(rgbMax) == 0)
		{
		tempHue = 200;
		}
	else
		{
		if ((rgbMax - rgbMid) > (rgbMid - rgbMin))
			{
			switch (order)
				{
				case kRGB:
					{
					tempHue = 23.0 + rint(62.0 * a) - rint(53.0 * b);
					hvc.chroma = ((rgbMax - rgbMin) / (rgbMax + rgbMin)) * (214 - (69.0 * a1));
					c = 110 + (120.0 * a1);
					break;
					}
					
				case kRBG:
					{
					tempHue = 23.0 + rint(62.0 * b) - rint(53.0 * a);
					hvc.chroma = ((rgbMax - rgbMin) / (rgbMax + rgbMin)) * (214 - (35.0 * a1));
					c = 110 + (6.0 * a1);
					break;
					}
					
				case kGRB:
					{
					tempHue = 103.0 - rint(18.0 * a) + rint(68.0 * b);
					hvc.chroma = ((rgbMax - rgbMin) / (rgbMax + rgbMin)) * (159 - (14.0 * a1));
					c = 185 + (45.0 * a1);
					break;
					}
					
				case kGBR:
					{
					tempHue = 103.0 - rint(18.0 * b) + rint(68.0 * a);
					hvc.chroma = ((rgbMax - rgbMin) / (rgbMax + rgbMin)) * (159 - (63.0 * a1));
					c = 185 + (24.0 * a1);
					break;
					}
					
				case kBRG:
					{
					tempHue = 200.0 + rint(25.0 * a) - rint(29.0 * b);
					hvc.chroma = ((rgbMax - rgbMin) / (rgbMax + rgbMin)) * (181 - (2.0 * a1));
					c = 93 + (23.0 * a1);
					break;
					}
					
				case kBGR:
					{
					tempHue = 200.0 + rint(25.0 * b) - rint(29.0 * a);
					hvc.chroma = ((rgbMax - rgbMin) / (rgbMax + rgbMin)) * (181 - (85.0 * a1));
					c = 93 + (116.0 * a1);
					break;
					}
				}
			}
		else
			{
			a = 1 - a;
			switch (order)
				{
				case kRGB:
					{
					tempHue = 85.0 - rint(62.0 * a);
					hvc.chroma = ((rgbMax - rgbMin) / (rgbMax + rgbMin)) * (145 + (69.0 * a));
					c = 230 - (120.0 * a);
					break;
					}
					
				case kRBG:
					{
					tempHue = 225.0 + rint(53.0 * a);
					hvc.chroma = ((rgbMax - rgbMin) / (rgbMax + rgbMin)) * (179 + (35.0 * a));
					c = 116 - (6.0 * a);
					break;
					}
					
				case kGRB:
					{
					tempHue = 85.0 + rint(18.0 * a);
					hvc.chroma = ((rgbMax - rgbMin) / (rgbMax + rgbMin)) * (145 + (14.0 * a));
					c = 230 - (45.0 * a);
					break;
					}
					
				case kGBR:
					{
					tempHue = 171.0 - rint(68.0 * a);
					hvc.chroma = ((rgbMax - rgbMin) / (rgbMax + rgbMin)) * (96 + (63.0 * a));
					c = 209 - (24.0 * a);
					break;
					}
					
				case kBRG:
					{
					tempHue = 225.0 - rint(25.0 * a);
					hvc.chroma = ((rgbMax - rgbMin) / (rgbMax + rgbMin)) * (179 + (2.0 * a));
					c = 116 - (23.0 * a);
					break;
					}
					
				case kBGR:
					{
					tempHue = 171.0 + rint(29.0 * a);
					hvc.chroma = ((rgbMax - rgbMin) / (rgbMax + rgbMin)) * (96 + (85.0 * a));
					c = 209 - (116.0 * a);
					break;
					}
				}
			}
		}
	
	tempHue = (tempHue <= 255) ? tempHue : tempHue - 255;
	tempHue = (tempHue >= 0) ? tempHue : tempHue + 255;
	hvc.hue = tempHue;
	hvc.value = ((1 - b) * ((rgbMax * c) / 255.0)) + (b * rgbMax);
hvc.chroma = ((rgbMax - rgbMin) / (rgbMax + rgbMin)) * GetMaxChroma(hvc.hue, hvc.value);
hvc.chroma = ((rgbMax - rgbMin) / rgbMax) * 255.0;
	hvc.chroma = std::min((float)hvc.chroma, GetMaxChroma(hvc.hue, hvc.value)-1);
	
	return hvc;
	}


//=========================================================================
//	FUNCTION:	ConvertImageLine
//	
//	PURPOSE:	
//=========================================================================
void HVCSpace::ConvertImageLine(unsigned short lineLength, RGBQuad* rgb, HVCTrip* hvc)
	{
	RGBTrip					temp;
	HVCTrip					*out = hvc;
	RGBQuad					*in = rgb;
	long						i;
	
	
	for (i=0; i<lineLength; i++, in++, out++)
		{
		temp.red = in->red;
		temp.green = in->green;
		temp.blue = in->blue;
		*out = GetHVCI(temp);
		}
	}
	
	
	
#pragma mark -
#pragma mark === rgb->hvc ===
//=========================================================================
//	FUNCTION:	GetMaxChroma
//	
//	PURPOSE:	
//=========================================================================
float HVCSpace::GetMaxChroma(unsigned char h, unsigned char v)
	{
	float						result;
	
	
	result = CL(h, v);
	
	return result;
	}
	
	
	
#pragma mark -
#pragma mark === rgb->hvc ===
//=========================================================================
//	FUNCTION:	InterpolateLine
//	
//	PURPOSE:	
//=========================================================================
void HVCSpace::InterpolateLine(RGBTripS* out, long& len0, long& terminal, RGBTripS* in1, float frac1, long len1, RGBTripS* in2, long len2)
	{
	RGBTripS					*ptr1;
	RGBTripS					*ptr2;
	long							l1;
	long							l2;
	float							f1;
	float							f2;
	float							fR;
	float							fG;
	float							fB;
	float							delta;
	long							i;
	long							j;
	
	
	if (len1 < len2)
		{
		ptr1 = in1;
		ptr2 = in2;
		l1 = len1;
		l2 = len2;
		f1 = frac1;
		f2 = 1 - frac1;
		}
	else
		{
		ptr1 = in2;
		ptr2 = in1;
		l1 = len2;
		l2 = len1;
		f1 = 1 - frac1;
		f2 = frac1;
		}
	
	// interpolate between matching chromas
	for (i=0; i<l1; i++)
		{
		out[i].red = rint((ptr1[i].red * f1) + (ptr2[i].red * f2));
		out[i].green = rint((ptr1[i].green * f1) + (ptr2[i].green * f2));
		out[i].blue = rint((ptr1[i].blue * f1) + (ptr2[i].blue * f2));
		}
	
	delta = (l1 * f1) + (l2 * f2);
	if (l1 == l2)
		{
		terminal = std::min(ceil(delta / cScale), 255.0f);
		fR = (ptr1[l1 - 1].red * f1) + (ptr2[l2 - 1].red * f2);
		fG = (ptr1[l1 - 1].green * f1) + (ptr2[l2 - 1].green * f2);
		fB = (ptr1[l1 - 1].blue * f1) + (ptr2[l2 - 1].blue * f2);
		}
	else
		{
		// if the line lengths don't match, interpolate between terminal chromas
		float						sR;
		float						sG;
		float						sB;
		
		
		terminal = std::min(floor(delta / cScale), 255.0f);
		delta -= l1;
		
		// start by caculating the endpoint
		fR = (ptr1[l1 - 1].red * f1) + (ptr2[l2 - 1].red * f2);
		fG = (ptr1[l1 - 1].green * f1) + (ptr2[l2 - 1].green * f2);
		fB = (ptr1[l1 - 1].blue * f1) + (ptr2[l2 - 1].blue * f2);
		
		// calculate the steps
		sR = (fR - out[l1 - 1].red) / delta;
		sG = (fG - out[l1 - 1].green) / delta;
		sB = (fB - out[l1 - 1].blue) / delta;
		
		len0 = floor(terminal * cScale);
		for (j=1; i<len0; i++, j++)
			{
			out[i].red = rint(out[l1 - 1].red + (j * sR));
			out[i].green = rint(out[l1 - 1].green + (j * sG));
			out[i].blue = rint(out[l1 - 1].blue + (j * sB));
			}
		}
	
	out[i].red = rint(fR);
	out[i].green = rint(fG);
	out[i].blue = rint(fB);
	len0 = i + 1;
	}


//=========================================================================
//	FUNCTION:	InterpolateLine2
//	
//	PURPOSE:	
//=========================================================================
void HVCSpace::InterpolateLine2(RGBTripS* out, long& len0, long& t0, RGBTripS* in1, float frac1, long len1, long term1, RGBTripS* in2, long len2, long term2)
	{
	RGBTripS					*ptr1;
	RGBTripS					*ptr2;
	long							l1;
	long							l2;
	long							t1;
	long							t2;
	float							l1s;
	float							f2a;
	float							f2b;
	float							fa;
	float							f1;
	float							f2;
	float							fR;
	float							fG;
	float							fB;
	float							sR;
	float							sG;
	float							sB;
	long							i;
	
	
	if (term1 < term2)
		{
		ptr1 = in1;
		ptr2 = in2;
		l1 = len1;
		l2 = len2;
		t1 = term1;
		t2 = term2;
		f1 = frac1;
		f2 = 1 - frac1;
		}
	else
		{
		ptr1 = in2;
		ptr2 = in1;
		l1 = len2;
		l2 = len1;
		t1 = term2;
		t2 = term1;
		f1 = 1 - frac1;
		f2 = frac1;
		}
	
	// interpolate between matching chromas
	for (i=0; i<l1-1; i++)									// the last entry contains the terminal values
		{
		out[i].red = rint((ptr1[i].red * f1) + (ptr2[i].red * f2));
		out[i].green = rint((ptr1[i].green * f1) + (ptr2[i].green * f2));
		out[i].blue = rint((ptr1[i].blue * f1) + (ptr2[i].blue * f2));
		}
	
	l1s = (l1 - 1) / cScale;
	f2b = (l1 == l2) ? ((t1 - l1s) / (t2 - l1s)) : ((t1 - l1s) / ((l1 / cScale) - l1s));
	f2a = 1 - f2b;
	sR = (ptr1[i].red * f1) + (((ptr2[l1 - 2].red * f2a) + (ptr2[l1 - 1].red * f2b)) * f2);
	sG = (ptr1[i].green * f1) + (((ptr2[l1 - 2].green * f2a) + (ptr2[l1 - 1].green * f2b)) * f2);
	sB = (ptr1[i].blue * f1) + (((ptr2[l1 - 2].blue * f2a) + (ptr2[l1 - 1].blue * f2b)) * f2);
		
	// start by caculating the terminal entry and values
	t0 = floor((t1 * f1) + (t2 * f2));
	fR = (ptr1[l1 - 1].red * f1) + (ptr2[l2 - 1].red * f2);
	fG = (ptr1[l1 - 1].green * f1) + (ptr2[l2 - 1].green * f2);
	fB = (ptr1[l1 - 1].blue * f1) + (ptr2[l2 - 1].blue * f2);
	
	for (; i<l2-1; i++)
		{
		fa = i / cScale;
		if (fa > t0)
			{
			len0 = i + 1;
			break;
			}
		
		f2b = (fa - t1) / (t2 - t1);
		f2a = 1 - f2b;
		out[i].red = rint((sR * f2a) + (fR * f2b));
		out[i].green = rint((sG * f2a) + (fG * f2b));
		out[i].blue = rint((sB * f2a) + (fB * f2b));
		}
	
	out[i].red = rint(fR);
	out[i].green = rint(fG);
	out[i].blue = rint(fB);
	len0 = i + 1;
	}


















//=========================================================================
//	FUNCTION:	FindHue
//	
//	PURPOSE:	
//=========================================================================
long HVCSpace::FindHue(RGBTrip rgb)
	{
	SortOrder				order;
	long						rgbMax;
	long						cMax;
	long						cMaxV;
	float						r1;
	float						r2;
	float						rMin;
	float						rTemp;
	RGBTrip					rgbTemp;
	long						i;
	long						j;
	long						start;
	long						stop;
	long						idx;
	long						result;
	
	
	// determine the relative magnitude of components
	rgbMax = std::max(rgb.red, std::max(rgb.green, rgb.blue));
	if (rgb.green == rgbMax)
		{
		order = (rgb.red < rgb.blue) ? kGBR : kGRB;
		}
	else
		{
		if (rgb.red == rgbMax)
			{
			order = (rgb.green < rgb.blue) ? kRBG : kRGB;
			}
		else
			{
			order = (rgb.green < rgb.red) ? kBRG : kBGR;
			}
		}
	
	if (rgbMax == 0)
		{
		result = 200;
		}
	else
		{
		switch (order)
			{
			case kRGB:
				{
				result = 23 + rint(62.0 * (rgb.green - rgb.blue) / rgb.red);
				break;
				}
				
			case kRBG:
				{
				result = 23 + rint(53.0 * (rgb.green - rgb.blue) / rgb.red);
				break;
				}
				
			case kGRB:
				{
				result = 103 + rint(18.0 * (rgb.blue - rgb.red) / rgb.green);
				break;
				}
				
			case kGBR:
				{
				result = 103 + rint(68.0 * (rgb.blue - rgb.red) / rgb.green);
				break;
				}
				
			case kBRG:
				{
				result = 200 + rint(25.0 * (rgb.red - rgb.green) / rgb.blue);
				break;
				}
				
			case kBGR:
				{
				result = 200 + rint(29.0 * (rgb.red - rgb.green) / rgb.blue);
				break;
				}
			}
		
		result = (result <= 255) ? result : result - 255;
		result = (result >= 0) ? result : result + 255;
return result;	//=======================================<<<<<<<<<<<<<<<<<<<<<<<<<
		start = result - 10;
		stop = result + 10;
		
		switch (order)
			{
			case kRGB:
				{
				if ((rgb.green == 0) || (rgb.green == rgb.blue))
					{
					result = 23;
					}
				else
					{
					rMin = 9999.0;
					r1 = (10.0 * rgb.green / rgb.red) + (rgb.blue / (float)rgb.green);
					for (idx=start; idx<stop; idx++)
						{
						i = (idx <= 255) ? idx : idx - 255;
						i = (idx >= 0) ? i : i + 255;
						cMax = 0;
						cMaxV = 0;
						for (j=0; j<256; j++)
							{
							if (cMax <= CL(i, j))
								{
								cMax = CL(i, j);
								cMaxV = j;
								}
							}
						
						rgbTemp = data[BM(i, cMaxV) + floor(cMax * cScale)];
						if ((rgbTemp.red >= rgbTemp.green) && (rgbTemp.green >= rgbTemp.blue))
							{
							r2 = (10.0 * rgbTemp.green / rgbTemp.red) + (rgbTemp.blue / (float)rgbTemp.green);
							rTemp = fabs(r1 - r2);
							if (rMin > rTemp)
								{
								rMin = rTemp;
								result = i;
								}
							}
						}
					}
				
				break;
				}
				
			case kRBG:
				{
				if ((rgb.blue == 0) || (rgb.blue == rgb.green))
					{
					result = 23;
					}
				else
					{
					rMin = 9999.0;
					r1 = (10.0 * rgb.blue / rgb.red) + (rgb.green / rgb.blue);
					for (idx=start; idx<stop; idx++)
						{
						i = (idx <= 255) ? idx : idx - 255;
						i = (idx >= 0) ? i : i + 255;
						cMax = 0;
						cMaxV = 0;
						for (j=0; j<256; j++)
							{
							if (cMax <= CL(i, j))
								{
								cMax = CL(i, j);
								cMaxV = j;
								}
							}
						
						rgbTemp = data[BM(i, cMaxV) + floor(cMax * cScale)];
						if ((rgbTemp.red >= rgbTemp.blue) && (rgbTemp.blue >= rgbTemp.green))
							{
							r2 = (10.0 * rgbTemp.blue / rgbTemp.red) + (rgbTemp.green / (float)rgbTemp.blue);
							rTemp = fabs(r1 - r2);
							if (rMin > rTemp)
								{
								rMin = rTemp;
								result = i;
								}
							}
						}
					}
				
				break;
				}
				
			case kGRB:
				{
				if ((rgb.red == 0) || (rgb.red == rgb.blue))
					{
					result = 103;
					}
				else
					{
					rMin = 9999.0;
					r1 = (10.0 * rgb.red / rgb.green) + (rgb.blue / (float)rgb.red);
					for (idx=start; idx<stop; idx++)
						{
						i = (idx <= 255) ? idx : idx - 255;
						i = (idx >= 0) ? i : i + 255;
						cMax = 0;
						cMaxV = 0;
						for (j=0; j<256; j++)
							{
							if (cMax < CL(i, j))
								{
								cMax = CL(i, j);
								cMaxV = j;
								}
							}
						
						rgbTemp = data[BM(i, cMaxV) + floor(cMax * cScale)];
						if ((rgbTemp.green >= rgbTemp.red) && (rgbTemp.red >= rgbTemp.blue))
							{
							r2 = (10.0 * rgbTemp.red / rgbTemp.green) + (rgbTemp.blue / (float)rgbTemp.red);
							rTemp = fabs(r1 - r2);
							if (rMin > rTemp)
								{
								rMin = rTemp;
								result = i;
								}
							}
						}
					}
				
				break;
				}
				
			case kGBR:
				{
				if ((rgb.blue == 0) || (rgb.blue == rgb.red))
					{
					result = 103;
					}
				else
					{
					rMin = 9999.0;
					r1 = (10.0 * rgb.blue / rgb.green) + (rgb.red / (float)rgb.blue);
					for (idx=start; idx<stop; idx++)
						{
						i = (idx <= 255) ? idx : idx - 255;
						i = (idx >= 0) ? i : i + 255;
						cMax = 0;
						cMaxV = 0;
						for (j=0; j<256; j++)
							{
							if (cMax < CL(i, j))
								{
								cMax = CL(i, j);
								cMaxV = j;
								}
							}
						
						rgbTemp = data[BM(i, cMaxV) + floor(cMax * cScale)];
						if ((rgbTemp.green >= rgbTemp.blue) && (rgbTemp.blue >= rgbTemp.red))
							{
							r2 = (10.0 * rgbTemp.blue / rgbTemp.green) + (rgbTemp.red / (float)rgbTemp.blue);
							rTemp = fabs(r1 - r2);
							if (rMin > rTemp)
								{
								rMin = rTemp;
								result = i;
								}
							}
						}
					}
				
				break;
				}
				
			case kBRG:
				{
				if ((rgb.red == 0) || (rgb.red == rgb.green))
					{
					result = 200;
					}
				else
					{
					rMin = 9999.0;
					r1 = (10.0 * rgb.red / rgb.blue) + (rgb.green / (float)rgb.red);
					for (idx=start; idx<stop; idx++)
						{
						i = (idx <= 255) ? idx : idx - 255;
						i = (idx >= 0) ? i : i + 255;
						cMax = 0;
						cMaxV = 0;
						for (j=0; j<256; j++)
							{
							if (cMax < CL(i, j))
								{
								cMax = CL(i, j);
								cMaxV = j;
								}
							}
						
						rgbTemp = data[BM(i, cMaxV) + floor(cMax * cScale)];
						if ((rgbTemp.blue >= rgbTemp.red) && (rgbTemp.red >= rgbTemp.green))
							{
							r2 = (10.0 * rgbTemp.red / rgbTemp.blue) + (rgbTemp.green / (float)rgbTemp.red);
							rTemp = fabs(r1 - r2);
							if (rMin > rTemp)
								{
								rMin = rTemp;
								result = i;
								}
							}
						}
					}
				
				break;
				}
				
			case kBGR:
				{
				if ((rgb.green == 0) || (rgb.green == rgb.red))
					{
					result = 200;
					}
				else
					{
					rMin = 9999.0;
					r1 = (10.0 * rgb.green / rgb.blue) + (rgb.red / (float)rgb.green);
					for (idx=start; idx<stop; idx++)
						{
						i = (idx <= 255) ? idx : idx - 255;
						i = (idx >= 0) ? i : i + 255;
						cMax = 0;
						cMaxV = 0;
						for (j=0; j<256; j++)
							{
							if (cMax < CL(i, j))
								{
								cMax = CL(i, j);
								cMaxV = j;
								}
							}
						
						rgbTemp = data[BM(i, cMaxV) + floor(cMax * cScale)];
						if ((rgbTemp.blue >= rgbTemp.green) && (rgbTemp.green >= rgbTemp.red))
							{
							r2 = (10.0 * rgbTemp.green / rgbTemp.blue) + (rgbTemp.red / (float)rgbTemp.green);
							rTemp = fabs(r1 - r2);
							if (rMin > rTemp)
								{
								rMin = rTemp;
								result = i;
								}
							}
						}
					}
				
				break;
				}
			}
		}
	
	return result;
	}



#if __MWERKS__
#pragma pop
#endif

// EOF








