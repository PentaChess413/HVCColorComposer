//=========================================================================
//
//	FILE:			MCLib.cpp
//						Copyright © 2004. DVL. All Rights Reserved.
//	
//	PURPOSE:	
//	
//	HISTORY:	1st Version - June 2004.
//				17nov05	alx	Ported to Windows and slight cleanup
//				14nov05	alx	Verified this version as 1.3
//	
//	NOTES:		- none -
//
//=========================================================================



#include "HVCSpace.h"


#include <map>


#include "MCLib.h"

using namespace std;

// make sure not to create static allocations!!! we are in a plugin...
static	long					gNumInstances = 0;
static	map<long, HVCSpace*>*	gSpaceMapPtr  = NULL;



#pragma mark === Creation / Initialization ===
//=========================================================================
//	FUNCTION:	CreateDefaultHVC
//	
//	PURPOSE:	Create a instance of the HVC space and populate with default values
//=========================================================================
long MCAPI CreateDefaultHVC(long which)
{
	long result = kGenErr;
	
	// check to see if we need to create the map
	if( !gSpaceMapPtr )
		gSpaceMapPtr = new map<long, HVCSpace*>;
	
	
	try
	{
		HVCSpace* newSpace = new HVCSpace(which);
		
		if( newSpace )
		{
			(*gSpaceMapPtr)[gNumInstances] = newSpace;
			result = gNumInstances++;
		}
	}
	catch (...) {}
		
	return result;
}


//=========================================================================
//	FUNCTION:	CreateHVC
//	
//	PURPOSE:	Create an empty instance of the HVC space
//=========================================================================
long MCAPI CreateHVC( unsigned short /*numH*/, unsigned short /*numV*/, unsigned short /*numC*/ )
{
	// check to see if we need to create the map
	if( !gSpaceMapPtr )
		gSpaceMapPtr = new map<long, HVCSpace*>;

	return kNoErr;
}


//=========================================================================
//	FUNCTION:	PopulateChroma
//	
//	PURPOSE:	
//=========================================================================
void MCAPI PopulateChroma(long hvcRef, short* data, bool direct)
	{
	try
		{
		map<long, HVCSpace*>::iterator		theSpaceIt;
		
		
		theSpaceIt = gSpaceMapPtr->find(hvcRef);
		if (theSpaceIt != gSpaceMapPtr->end())
			{
			HVCSpace		*theSpace = (*theSpaceIt).second;
			
			
			theSpace->PopulateChroma(data, direct);
			}
		}
	catch (...)
		{
		}
	}


//=========================================================================
//	FUNCTION:	PopulateChroma1
//	
//	PURPOSE:	
//=========================================================================
void MCAPI PopulateChroma1(long hvcRef, unsigned char* lengths, short* data, bool direct)
	{
	try
		{
		map<long, HVCSpace*>::iterator		theSpaceIt;
		
		
		theSpaceIt = gSpaceMapPtr->find(hvcRef);
		if (theSpaceIt != gSpaceMapPtr->end())
			{
			HVCSpace		*theSpace = (*theSpaceIt).second;
			
			
			theSpace->PopulateChroma(lengths, data, direct);
			}
		}
	catch (...)
		{
		}
	}


//=========================================================================
//	FUNCTION:	BuildHVCMap
//	
//	PURPOSE:	Interpolate values for all hues and values
//=========================================================================
void MCAPI BuildHVCMap(long hvcRef)
	{
	try
		{
		map<long, HVCSpace*>::iterator		theSpaceIt;
		
		
		theSpaceIt = gSpaceMapPtr->find(hvcRef);
		if (theSpaceIt != gSpaceMapPtr->end())
			{
			HVCSpace		*theSpace = (*theSpaceIt).second;
			
			
			theSpace->BuildMap();
			}
		}
	catch (...)
		{
		}
	}


//=========================================================================
//	FUNCTION:	ReleaseHVC
//	
//	PURPOSE:	
//=========================================================================
void MCAPI ReleaseHVC(long hvcRef)
{
	try
	{
		map<long, HVCSpace*>::iterator		theSpaceIt;
		
		
		theSpaceIt = gSpaceMapPtr->find( hvcRef );
		if( theSpaceIt != gSpaceMapPtr->end() )
		{
			HVCSpace* theSpace = theSpaceIt->second;
			
			// yank it from the map
			gSpaceMapPtr->erase( theSpaceIt );
			
			// toss the actual object
			delete theSpace;
		}
			
		// now see if we have any clients remaining and if not destroy the map<>
		if( gSpaceMapPtr->empty() )
		{
			delete gSpaceMapPtr;
			
			// always do this as we are in a plugin 
			// and must clear our own statics to be safe!!!
			gSpaceMapPtr  = NULL;
			gNumInstances = 0;
		}
	}
	catch (...) {}
}



#pragma mark -
#pragma mark === hvc->rgb ===
//=========================================================================
//	FUNCTION:	HVC2RGB
//	
//	PURPOSE:	
//=========================================================================
bool MCAPI HVC2RGB( long hvcRef, HVCTrip hvc, RGBTrip& rgb )
{
	try
	{
		map<long, HVCSpace*>::iterator  theSpaceIt;
		
		
		theSpaceIt = gSpaceMapPtr->find(hvcRef);
		if( theSpaceIt != gSpaceMapPtr->end() )
		{
			HVCSpace *theSpace = theSpaceIt->second;
			
			if( theSpace )		
				return theSpace->GetRGB( hvc, rgb );
		}
	}
	catch(...)	
	{}
		
	return false;
}

	
//=========================================================================
//	FUNCTION:	HVC2ImageLine
//	
//	PURPOSE:	
//=========================================================================
void MCAPI HVC2ImageLine(long hvcRef, unsigned short lineLength, HVCTrip* hvc, RGBQuad* rgb, char* outOfGamutMask)
	{
	try
		{
		map<long, HVCSpace*>::iterator		theSpaceIt;
		
		
		theSpaceIt = gSpaceMapPtr->find(hvcRef);
		if (theSpaceIt != gSpaceMapPtr->end())
			{
			HVCSpace		*theSpace = (*theSpaceIt).second;
			
			
			theSpace->Convert2ImageLine(lineLength, hvc, rgb, outOfGamutMask);
			}
		}
	catch (...)
		{
		}
	}
	
	
//=========================================================================
//	FUNCTION:	HVC2ImageLineSetOOG
//	
//	PURPOSE:	
//=========================================================================
void MCAPI HVC2ImageLineSetOOG(long hvcRef, unsigned short lineLength, HVCTrip* hvc, RGBQuad* rgb, RGBQuad outOfGamutColor)
	{
	try
		{
		map<long, HVCSpace*>::iterator		theSpaceIt;
		
		
		theSpaceIt = gSpaceMapPtr->find(hvcRef);
		if (theSpaceIt != gSpaceMapPtr->end())
			{
			HVCSpace		*theSpace = (*theSpaceIt).second;
			
			
			theSpace->Convert2ImageLine(lineLength, hvc, rgb, outOfGamutColor);
			}
		}
	catch (...)
		{
		}
	}



#pragma mark -
#pragma mark === rgb->hvc ===
//=========================================================================
//	FUNCTION:	RGB2HVC
//	
//	PURPOSE:	
//=========================================================================
void MCAPI RGB2HVC(long hvcRef, RGBTrip rgb, HVCTrip& hvc)
	{
	try
		{
		map<long, HVCSpace*>::iterator		theSpaceIt;
		
		
		theSpaceIt = gSpaceMapPtr->find(hvcRef);
		if (theSpaceIt != gSpaceMapPtr->end())
			{
			HVCSpace		*theSpace = (*theSpaceIt).second;
			
			
			hvc = theSpace->GetHVC(rgb);
			}
		}
	catch (...)
		{
		}
	}
	
	
//=========================================================================
//	FUNCTION:	ImageLine2HVC
//	
//	PURPOSE:	
//=========================================================================
void MCAPI ImageLine2HVC(long hvcRef, unsigned short lineLength, RGBQuad* rgb, HVCTrip* hvc)
	{
	try
		{
		map<long, HVCSpace*>::iterator		theSpaceIt;
		
		
		theSpaceIt = gSpaceMapPtr->find(hvcRef);
		if (theSpaceIt != gSpaceMapPtr->end())
			{
			HVCSpace		*theSpace = (*theSpaceIt).second;
			
			
			theSpace->ConvertImageLine(lineLength, rgb, hvc);
			}
		}
	catch (...)
		{
		}
	}
	
	
	
#pragma mark -
#pragma mark === bogus routines ===
//=========================================================================
//	FUNCTION:	ImageLine2HVC
//	
//	PURPOSE:	
//=========================================================================
float MCAPI GetMaxChroma(long hvcRef, unsigned char h, unsigned v)
	{
	float				result = -1.0;
	
	
	try
		{
		map<long, HVCSpace*>::iterator		theSpaceIt;
		
		
		theSpaceIt = gSpaceMapPtr->find(hvcRef);
		if (theSpaceIt != gSpaceMapPtr->end())
			{
			HVCSpace		*theSpace = (*theSpaceIt).second;
			
			
			result = theSpace->GetMaxChroma(h, v);
			}
		}
	catch (...)
		{
		}
	
	return result;
	}




