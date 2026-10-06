/*
 *
 *	Copyright 2006 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	Source code rewritten by: Alex Lelievre ([PII Redacted]), 2006.
 *
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#ifndef MCGotoURL_H
#define MCGotoURL_H


// Function to pass a url to the system browser
class MCURL
{
	public:
		static bool Go( SPPluginRef plugin_ref, ASInt32 string_index )
		{
			const size_t bufferSize = 1024;
			
			ASErr error = noErr;
			char  the_url[bufferSize] = { 0 };

			error = ADM_Access::basic_suite()->GetIndexString( plugin_ref, 16100, string_index, the_url, bufferSize );
			if( !error )
				sPSFileList->BrowseUrl( (const char*)the_url );
			
			return error == noErr;
		}
		
		static bool Go( const char* url )
		{
			return sPSFileList->BrowseUrl( url ) == noErr;
		}
};

#endif // !MCGotoURL_H

// EOF
