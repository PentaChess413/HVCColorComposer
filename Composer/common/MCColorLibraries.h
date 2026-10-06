/*
 *
 *	Copyright 2006 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */


#ifndef _H_MCColorLibraries
#define _H_MCColorLibraries


//////////////////////////////////////////////////////////////////////////////////////////////
//
// Includes
//
//////////////////////////////////////////////////////////////////////////////////////////////

#include "MCColorbook.h"


//////////////////////////////////////////////////////////////////////////////////////////////
//
// Defines
//
//////////////////////////////////////////////////////////////////////////////////////////////

const Int32 kMaxBookPathLength = 4096;


typedef struct
{
	SpCharPtr		path;		// pointer to full path
	MCColorbook*	colorBook;	// pointer to the color book
	bool			loaded;		// is the book fully loaded or just the header?
} MCBookData;

typedef CDynamicArray<MCBookData*> MCBookArray;


//////////////////////////////////////////////////////////////////////////////////////////////
//
// Class
//
//////////////////////////////////////////////////////////////////////////////////////////////

class MCColorLibraries
{
	public:
		MCColorLibraries();
		virtual ~MCColorLibraries();

		// call this before anything else... if you get back false there are no books to read...
		bool			OpenBooks();	
		
		// get the number of books found
		Int32			GetNumBooks();
		
		// get a color book by index. 
//		MCColorbook*	GetBook( Int32 bookIndex );
		
		// get the book's title...  (short-cut)
		bool			GetBookTitle( Int32 bookIndex, RVWideNameField& name );
		
		// get the currently selected book 
		MCColorbook*	GetCurrentBook();
		void			SetCurrentBook( Int32 bookIndex );

	protected:
		void			DisposeBooks();

	private:
		bool			mOpened;
		MCBookArray		mBooks;
	
		Int32			mCurrentBook;
		SpChar			mRootPath[kMaxBookPathLength];
		SpChar			mSearchPath[kMaxBookPathLength];
		SpChar			mLibrariesPath[kMaxBookPathLength];
};

#endif // !_H_MCColorLibraries

// EOF
