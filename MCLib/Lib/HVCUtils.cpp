//=========================================================================
//
//	FILE:			HVCUtils.cpp
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


// MCLib

// System
#include <math.h>
#include <string.h>

// This
#include "HVCUtils.h"
















#pragma mark -
#pragma mark === Sort Routines ===
int SortRGB(const Sextuplet p1, const Sextuplet p2)
	{
	int				result = 0;
	
	
	if (p1.red < p2.red)
		{
		result = 1;
		}
	else
		{
		if (p1.red == p2.red)
			{
			if (p1.green < p2.green)
				{
				result = 1;
				}
			else
				{
				if (p1.green == p2.green)
					{
					if (p1.blue < p2.blue)
						{
						result = 1;
						}
					}
				}
			}
		}
	
	return result;
	}


int SortRBG(const Sextuplet p1, const Sextuplet p2)
	{
	int				result = 0;
	
	
	if (p1.red < p2.red)
		{
		result = 1;
		}
	else
		{
		if (p1.red == p2.red)
			{
			if (p1.blue < p2.blue)
				{
				result = 1;
				}
			else
				{
				if (p1.blue == p2.blue)
					{
					if (p1.green < p2.green)
						{
						result = 1;
						}
					}
				}
			}
		}
	
	return result;
	}


int SortGRB(const Sextuplet p1, const Sextuplet p2)
	{
	int				result = 0;
	
	
	if (p1.green < p2.green)
		{
		result = 1;
		}
	else
		{
		if (p1.green == p2.green)
			{
			if (p1.red < p2.red)
				{
				result = 1;
				}
			else
				{
				if (p1.red == p2.red)
					{
					if (p1.blue < p2.blue)
						{
						result = 1;
						}
					}
				}
			}
		}
	
	return result;
	}


int SortGBR(const Sextuplet p1, const Sextuplet p2)
	{
	int				result = 0;
	
	
	if (p1.green < p2.green)
		{
		result = 1;
		}
	else
		{
		if (p1.green == p2.green)
			{
			if (p1.blue < p2.blue)
				{
				result = 1;
				}
			else
				{
				if (p1.blue == p2.blue)
					{
					if (p1.red < p2.red)
						{
						result = 1;
						}
					}
				}
			}
		}
	
	return result;
	}


int SortBRG(const Sextuplet p1, const Sextuplet p2)
	{
	int				result = 0;
	
	
	if (p1.blue < p2.blue)
		{
		result = 1;
		}
	else
		{
		if (p1.blue == p2.blue)
			{
			if (p1.red < p2.red)
				{
				result = 1;
				}
			else
				{
				if (p1.red == p2.red)
					{
					if (p1.green < p2.green)
						{
						result = 1;
						}
					}
				}
			}
		}
	
	return result;
	}


int SortBGR(const Sextuplet p1, const Sextuplet p2)
	{
	int				result = 0;
	
	
	if (p1.blue < p2.blue)
		{
		result = 1;
		}
	else
		{
		if (p1.blue == p2.blue)
			{
			if (p1.green < p2.green)
				{
				result = 1;
				}
			else
				{
				if (p1.green == p2.green)
					{
					if (p1.red < p2.red)
						{
						result = 1;
						}
					}
				}
			}
		}
	
	return result;
	}