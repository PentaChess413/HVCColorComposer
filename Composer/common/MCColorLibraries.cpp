/*
 *
 *	Copyright 2006 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */
#include "PhotoShopSDK.h"

#include "MCColorLibraries.h"
#include "MCSystemRegistry.h"


const SpCharPtr kAdobeHostString		= "Photoshop";
const SpCharPtr	kColorBookSearchString  = "%sPresets\\Color Books\\*.acb";
const SpCharPtr	kColorBookPathString	= "%sPresets\\Color Books\\";

//////////////////////////////////////////////////////////////////////////////////////////////
//
// Constructor
//
//////////////////////////////////////////////////////////////////////////////////////////////

MCColorLibraries::MCColorLibraries() : 
	mCurrentBook( 0 ),
	mOpened( false )
{
	// clean out the string arrays
	::memset( mRootPath, kMaxBookPathLength, sizeof( SpChar ) );
	::memset( mSearchPath, kMaxBookPathLength, sizeof( SpChar ) );
	::memset( mLibrariesPath, kMaxBookPathLength, sizeof( SpChar ) );

	// get path to our host.  this is our root directory.
	{	
#if WIN32
		MCSystemRegistry localMachine( HKEY_LOCAL_MACHINE );
#else
		MCSystemRegistry localMachine;
#endif
		ConstSpCharPtr pathStr = localMachine.GetPathToHost( kAdobeHostString );
		
		// copy this string
		if( pathStr && ::strlen( pathStr ) < kMaxBookPathLength )
		{
			::strcpy( mRootPath, pathStr );
		
			// create the libraries path now
			::sprintf( mSearchPath, kColorBookSearchString, mRootPath );
			::sprintf( mLibrariesPath, kColorBookPathString, mRootPath );
		}
	}	

	OpenBooks();
}


MCColorLibraries::~MCColorLibraries()
{
	DisposeBooks();
}



//////////////////////////////////////////////////////////////////////////////////////////////
//
// Methods
//
//////////////////////////////////////////////////////////////////////////////////////////////

// look for color books in the color book directory...
bool MCColorLibraries::OpenBooks()
{
	if( mOpened )
		return true;

#if WIN32		
	// use windows calls. 
	WIN32_FIND_DATA winData;
	HANDLE files = ::FindFirstFile( mSearchPath, &winData );
		
	if( files == INVALID_HANDLE_VALUE )	
		return false;

	do
	{
		// create a color book object and point it to this path...
		SpCharPtr path = new SpChar[kMaxBookPathLength];
		
		if( path )
		{
			::strcpy( path, mLibrariesPath );
			::strcat( path, winData.cFileName );
			
			MCBookData* bookData = new MCBookData;
			bookData->path = path;
			bookData->loaded = false;
			bookData->colorBook = new MCColorbook;
			if( bookData->colorBook )
			{
				// just read the header data...
				if( bookData->colorBook->ReadColorbook( path, true ) )
				{
					// insert that book in our list
					mBooks << bookData;
				}
			}
		}
//		XDEBUG_PRINT1( "pathname: %s\n", path );

	} while( ::FindNextFile( files, &winData ) );
	
	::FindClose( files );
	mOpened = true;

	return true;
#endif
		
	return false;
}


// dispose color books...
void MCColorLibraries::DisposeBooks()
{
	Int32 count = mBooks.GetCount();
	if( !count )
		return;
		
	MCBookData** ptr = mBooks.Lock(0);
	if( ptr )
	{
		// loop through all the color books and toss them...
		for( Int32 i = 0; i < count; i++ )
		{
			delete [] ptr[i]->path;
			delete ptr[i]->colorBook;
			delete ptr[i];
		}
	}
	mBooks.Unlock();
	
	// remove all entries
	mBooks.Remove( 0, -1 );
}


// get the number of books found
Int32 MCColorLibraries::GetNumBooks()
{
	return mBooks.GetCount();
}


/*
// get a color book by index. 
MCColorbook* MCColorLibraries::GetBook( Int32 bookIndex )
{
	return NULL;
}
*/

// get the book's title...  (short-cut)
bool MCColorLibraries::GetBookTitle( Int32 bookIndex, RVWideNameField& name )
{
	try
	{
		MCBookData* data = mBooks[bookIndex];
		
		if( data )
		{
			RVWideNameField resultName = data->colorBook->GetBookTitle();
			name = resultName; // damn gcc
			return true;
		}
	}
	catch(...) {}
	
	
	return false;
}


MCColorbook* MCColorLibraries::GetCurrentBook()
{
	try
	{
		MCBookData* data = mBooks[mCurrentBook];
		
		// now see if this book is partially loaded...
		if( data )
		{
			if( data->colorBook && !data->loaded )
			{
				if( data->colorBook->ReadColorbook( data->path ) )
					data->loaded = true;

			}
			
			return data->colorBook;
		}
	}
	catch(...) {}
	
	return NULL;
}

void MCColorLibraries::SetCurrentBook( Int32 bookIndex )
{
	if( bookIndex < 0 )
		bookIndex = 0;
		
	mCurrentBook = bookIndex;
}







/*
#if 0

// Constructor:
inline Add_File_To_Menu::Add_File_To_Menu( MenuRef p_menu, MCColorLibraries::File_List& p_good_files, CFStringRef p_target_name, ControlRef p_menu_controller )	:
							m_menu( p_menu ), 
							m_good_files( p_good_files ),
							m_target_name( p_target_name ),
							m_menu_controller( p_menu_controller ), 
							m_count( 0 )
{
}

// Add the files title to the menu
void MCColorLibraries::AddToPopup()( SpFilePtr file )
{
	mac_ifstream	read_file(file);			// Open the file

	// Verify the file header
	if (!read_file.good() || 0x3842 != read_short(read_file) || 0x4342 != read_short(read_file) || 1 != read_short(read_file))
	{
		return;
	}

	m_good_files.push_back(file);				// Add it to the list of added files
	read_short(read_file);						// Skip extra

	std::wstring	raw_title = read_definition(read_file, L"title");	// Read the title
	Auto_CFString	title(CFStringCreateWithCharacters(NULL,			// Convert it to a system string
							reinterpret_cast<UInt16*> (raw_title.begin()), raw_title.length()));

	// Add the system string to the menu
	THROW_IF_ERR(AppendMenuItemTextWithCFString(m_menu, title, 0, 0, NULL));

	++m_count;									// Update the count
	// Is this the selected file
	if (kCFCompareEqualTo == CFStringCompare(title, m_target_name, 0))
		SetControl32BitValue(m_menu_controller, m_count);	// Select the name in the popup menu
}


// Find the color books folder within a folder
void MCColorLibraries::GetRootFolder( FSRef& book_folder )
{
	const UniChar*	presets = reinterpret_cast<const UniChar*> (L"Presets");	// Hard coded location
	const UniChar*	color_books = reinterpret_cast<const UniChar*> (L"Color Books");

	// Find the folder as a sub folder of the folder passed in
	THROW_IF_ERR(FSMakeFSRefUnicode(&book_folder, 7, presets, kTextEncodingMacRoman,
									&book_folder));
	THROW_IF_ERR(FSMakeFSRefUnicode(&book_folder, 11, color_books, kTextEncodingMacRoman,
									&book_folder));
}

// Find the color books folder
SpFilePtr MCColorLibraries::GetRootFolder()
{
	SpFilePtr book_folder = NULL;

	try
	{
		// Check the Applications folder for the Photoshop folder
		System_Folder_FSRef	application_folder(kSystemDomain, kApplicationsFolderType, false);
		const UniChar*		application = reinterpret_cast<const UniChar*> (L"Adobe Photoshop CS");

		THROW_IF_ERR(FSMakeFSRefUnicode(&application_folder.get(), 18, application,
										kTextEncodingMacRoman, &book_folder));
		get_color_book_folder(book_folder);	// Check the Photoshop folder for the color books folder
	}
	catch(...)
	{
		// Use the location of the current resource file
		FCBPBRec	pb = { 0 };

		pb.ioRefNum = CurResFile();					// Get the resource file index
		THROW_IF_ERR(PBGetFCBInfoSync(&pb));		// Get the resource file location

		FSSpec			file = { pb.ioVRefNum, pb.ioFCBParID };
		const UniChar*	plugins = reinterpret_cast<const UniChar*> (L"Plug-Ins");
		HFSUniStr255	folder_name = { 0 };

		THROW_IF_ERR(FSpMakeFSRef(&file, &book_folder));	// Get an FSRef for the resource file location

		// Move up the folder hierarchy until we find the "Plug-Ins" folder
		while (8 != folder_name.length || !std::equal(plugins, plugins + 8, folder_name.unicode))
			THROW_IF_ERR(FSGetCatalogInfo(&book_folder, kFSCatInfoNone, NULL, &folder_name, NULL,
											&book_folder));

		// Use the parent of the "Plug-Ins" folder to search for the color books folder
		get_color_book_folder(book_folder);
	}

	return book_folder;
}
#endif
*/

// EOF
