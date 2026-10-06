///////////////////////////////////////////////////////////////////////////////
//
// Copyright 1998-2004 FOInc.  All rights reserved.  $Revision: 1.1.1.1 $
//      This material is the confidential trade secret and proprietary
//      information of FOInc.  It may not be reproduced, used,
//      sold or transferred to any third party without the prior written
//      consent of FOInc.  All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////

/*	History:
04oct26 lma FOSS: Rewritten to use stb_ds instead of FOInc's stuff
22mar04	alx	Created - written for UNIX style OS's with resizable memory support
*/

#ifndef _H_DynaArray
#define _H_DynaArray


///////////////////////////////////////////////////////////////////////////////////
//
// Includes
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef _H_FastAlloc
#include "FastAlloc.h"
#endif
// wire macros to use photoshop allocation - not taking any risks here
#define STBDS_REALLOC(context, ptr, size)	FastAlloc::NewClear(size)
#define STBDS_FREE(context, ptr)			FastAlloc::Delete(ptr)

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"
#include <stdexcept>


/*-------------------------------------------------------------------------*/

#define kDynaArrayDefaultGrowSize			4	// this is in # of items

template <class T> class CDynamicArray
{
	
	public:
		typedef int (*CDynamicArrayCompareProc)( ConstSpVoidPtr item1, ConstSpVoidPtr item2 );
//		typedef int (*CDynamicArrayCompareProc)( const T* item1, const T* item2 );

/*----------------------- Constructors / Destructor -----------------------*/

        CDynamicArray() :
			mCount( 0 ),
			mGrowSize( 4 ),
			mLockCount( 0 ),
			mLock( NULL ),
			mMemory( NULL ),
			mSorted( false )
		{
			// ctor
		}

		// use this constructor for a different block size
		CDynamicArray( SpLong growSize ) : 	
			mCount( 0 ),
			mGrowSize( 4 ),
			mLockCount( 0 ),
			mLock( NULL ),
			mMemory( NULL ),
			mSorted( false )
		{

		}

        ~CDynamicArray( void )
		{
			arrfree(mMemory);
		}

/*--------------------------- Public Methods ------------------------------*/

		// returns the number of elements in array
        SpLong	GetCount( void )
		{
			return arrlen(mMemory);
		}

/*-------------------------------------------------------------------------*/
		
		// array operator - throws SRuntimeError exception for out of range
        const T& operator[]( SpLong index )
		{
			if( index >= 0 && index < arrlen(mMemory) )
			{
				return mMemory[index];
			}
			else
				// stb_ds doesn't handle oor exceptions oob
				throw std::out_of_range("That item is out of range!");
		}
/*-------------------------------------------------------------------------*/
		
		// this is the insert operator, think of it as the stuffer
        void operator<<( const T& item )
		{
			arrput(mMemory, item);
		}

		

/*-------------------------------------------------------------------------*/
		
		// operator = makes a copy of the passed in object
        void operator=( CDynamicArray& copyMe )
		{
			CloneObject( copyMe );
		}

/*-------------------------------------------------------------------------*/
		
		// This lets you remove elements, 
		// specify -1 in howMany to clear to end of list.
		// If you specify more than is avail it will clear to the end
        void Remove( SpLong item, SpLong howMany = 1 )
		{
			SpLong numToRemove = 0;

			if( (item < 0) || (item > arrlen(mMemory)) )
				throw std::out_of_range("Tried to remove item out of range!");

			if( ( item + howMany ) > arrlen(mMemory) )
				numToRemove = -1;
			else
				numToRemove = howMany;

			if (numToRemove == -1)
				arrsetlen(mMemory, item);
			else
				arrdeln(mMemory, item, howMany);
				
		}
/*-------------------------------------------------------------------------*/
		// use this method to gain access to the data rather than a copy of it.
		// use unlock when you are done, your reference will no longer be valid 
		// after calling unlock!
        T* Lock( SpLong index )
		{
		if( index >= 0 && index < mCount )
		{
			if( mLockCount++ == 0 )
			{
				mLock = mMemory;
			}
			if( !mLock )
				throw std::bad_alloc();
			return &mLock[index];
		} else
			throw std::out_of_range("Tried to lock item out of range!");
		return NULL;
		}

        void Unlock()
		{
			if( --mLockCount == 0 )
			{
				mLock = NULL;
			}
		}
	
		// members
		SpUlong mLockCount;

		SpLong mCount;
		SpLong mGrowSize;

		T* mMemory;

		T mTemp;
		T* mLock;
		bool mSorted;
/*--------------------------- Private Methods -----------------------------*/
		
	private:
        void CloneObject( CDynamicArray& objectToClone )
		{
			if (mMemory != nullptr) {
				arrfree(mMemory);
				mMemory = nullptr;
			}

			if (objectToClone.mMemory != nullptr) {
				size_t len = arrlenu(objectToClone.mMemory);
				for (size_t i = 0; i < len; ++i) {
					arrput(mMemory, objectToClone.mMemory[i]);
				}
			}
		}
};
#endif	// !_H_DynaArray

// EOF