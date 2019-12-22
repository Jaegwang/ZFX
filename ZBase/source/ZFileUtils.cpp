//----------------//
// ZFileUtils.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2018.03.19                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

bool
ZIsDirectory( const char* path )
{
	struct stat fstat;
	lstat( path, &fstat );
	if( S_ISDIR(fstat.st_mode) ) { return true; }
	return false;
}

bool
ZIsSymbolicLink( const char* path )
{
	struct stat fstat;
	lstat( path, &fstat );
	if( S_ISLNK(fstat.st_mode) ) { return true; }
	return false;
}

bool
ZFileExist( const char* path )
{
    struct stat buffer;
    const int exist = stat( path, &buffer );
    return ( (exist==0) ? true : false );
}

long int
ZFileSize( const char* filePathName )
//{
//	if( !ZFileExist(filePathName) ) { return 0; }
//
//	long int fileSize = 0;
//	FILE* fp = (FILE*)NULL;
//
//	#ifdef OS_LINUX
//	 fp = fopen( filePathName, "r" );
//	#endif
//
//	#ifdef OS_WINDOWS
//	 fopen_s( &fp, filePathName, "r" );
//	#endif
//
//	fseek( fp, 0, SEEK_END );
//	fileSize = ftell( fp );
//	fseek( fp, 0, SEEK_SET );
//	fclose( fp );
//
//	return fileSize;
//}
{
    if( !ZFileExist(filePathName) ) { return 0; }

    struct stat buffer;

    stat( filePathName, &buffer );

    return (long int)buffer.st_size; // type -> size_t
}

ZString
ZFileSizeAsString( const char* filePathName )
{
	const long int bytes = ZFileSize( filePathName );

	ZString str;
	{
		if( bytes > 1073741824 ) // 1073741824 = 1024^3
		{
			const double size = bytes / 1073741824.0;

			str += size;
			str += " GB";
		}
		else if( bytes > 1048576 ) // 1048576 = 1024^2
		{
			const double size = bytes / 1048576.0;

			str += size;
			str += " MB";
		}
		else if( bytes > 1024 )
		{
			const double size = bytes / 1024.0;

			str += size;
			str += " KB";
		}
		else
		{
			str += bytes;
			str += " Bytes";
		}
	}

	return str;
}

bool
ZCreateDirectory( const char* path, mode_t permission )
{
	#ifdef OS_LINUX
	 if( ZFileExist( path ) ) { return true; }
	#endif

	ZString tmp( path );
	ZStringArray tokens;
	ZTokenize( tmp, "/", tokens );
	const int N = (int)tokens.size();
	if( N == 0 ) { return false; }
	tmp.clear();
	
	#ifdef OS_LINUX
	FOR( i, 0, N )
	{
		tmp += "/" + tokens[i];
		if( !ZFileExist( tmp.c_str() ) )
		if( mkdir( tmp.c_str(), permission ) )
		{
			cout << "Error@ZCreateDirectory(): Failed to create a directory." << endl;
			return false;
		}
	}
	#endif
	
	#ifdef OS_WINDOWS
	tmp = tokens[0];
	FOR( i, 1, N )
	{
		tmp += "/" + tokens[i];
		_mkdir( tmp.c_str() );
	}
	#endif

	return true;
}

bool
ZDeleteDirectory( const char* path )
{
	ZString pathStr( path );

    const size_t numAsterisks = pathStr.count( '*' );

    if( numAsterisks > 0 )
    {
        cout << "Error@ZDeleteDirectory(): You cannot use * in the path for safety." << endl;
        return false;
    }

	#ifdef OS_LINUX
	 ZString cmd( "rm -rf " );
	 cmd += ZString( path );
	 system( cmd.c_str() );
	#endif

	#ifdef OS_WINDOWS
	 ZString pathStr( path );
	 ReplaceSeparators( pathStr );
	 ZString cmd( "rd /s /q " );
	 cmd += ZString( pathStr );
	 system( cmd.c_str() );
	#endif

    return true;
}

ZString
ZCurrentPath()
{
	char cdir[512];
	getcwd( cdir, 512 );
	return ZString( cdir );
}

ZString
ZFileExtension( const char* fileName )
{
	ZString file( fileName );
	ZStringArray tokens;
	ZTokenize( file, ".", tokens );


	return tokens.last();
}

ZString
ZRemoveExtension( const char* fileName )
{
	ZString file( fileName );
	return file.subString( 0, file.lastIndexOf('.')-1 );
}

ZString
ZChangeSeparators( const char* fileName )
{
	ZString path( fileName );

	int N = path.length();
	FOR( i, 0, N )
	{
		char& c = path[i];

		if( c == '/' )       { c = '\\'; }
		else if( c == '\\' ) { c = '/';  }
	}

	return path;
}

bool GetFileList( const char* path, ZStringArray& files, bool asFullPath )
{
    files.clear();

    DIR* dp;
    struct dirent* dirp;

    if( !( dp = opendir( path ) ) )
    {
        cout << "Error@GetFileList(): Failed open file " << path << endl;
        return false;
    }

    while( ( dirp = readdir(dp) ) != NULL )
    {
        const std::string shortPath = dirp->d_name;

        std::string fullPath = path;
        fullPath += "/" + shortPath;

        if( ZIsDirectory( fullPath.c_str() ) ) { continue; }

        if( asFullPath )
        {
            files.push_back( fullPath );
        }
        else
        {
            files.push_back( shortPath );
        }
    }

    closedir( dp );

	return true;
}

bool GetFileList( const char* path, const ZString& extension, ZStringArray& files, bool asFullPath )
{
    ZStringArray candidates;

    if( !GetFileList( path, candidates, asFullPath ) )
    {
        return false;
    }

	const size_t numFiles = candidates.size();

    for( size_t i=0; i<numFiles; ++i )
	{
        const ZString ext = ZFileExtension( candidates[i].c_str() );

		if( ext == extension )
		{
			files.push_back( candidates[i] );
		}
	}

	return true;
}

bool GetDirectoryList( const char* path, ZStringArray& directories, bool asFullPath )
{
    directories.clear();

    DIR* dp;
    struct dirent* dirp;

    if( !( dp = opendir( path ) ) )
    {
        cout << "Error@GetDirectoryList(): Failed open file " << path << endl;
        return false;
    }

    while( ( dirp = readdir(dp) ) != NULL )
    {
        const std::string shortPath = dirp->d_name;

        if( shortPath == "." ) { continue; }
        if( shortPath == ".." ) { continue; }

        std::string fullPath = path;
        fullPath += "/" + shortPath;

        if( ZIsDirectory( fullPath.c_str() ) )
        {
            if( asFullPath )
            {
                directories.push_back( fullPath );
            }
            else
            {
                directories.push_back( shortPath );
            }
        }
    }

    closedir( dp );

	return true;
}

bool
ZGetFileFrames( const ZStringArray& fileNames, ZIntArray& frames )
{
	const int nFiles = fileNames.length();

	frames.setLength( nFiles );

	const ZString file = ZRemoveExtension( fileNames[0].asChar() );

	int startIndex = -1;
	int endIndex   = -1;
	{
		const int file_len = file.length();

		for( int i=file_len-1; i>=0; --i )
		{
			const char& c = file[i];

			if( ZIsDigit( c ) ) {

				if( endIndex < 0 ) { endIndex   = i; }
				else               { startIndex = i; }

			} else {

				if( endIndex >= 0 ) { break; }

			}
		}
	}

	if( startIndex < 0 ) { return false; }
	if( endIndex   < 0 ) { return false; }

	for( int i=0; i<nFiles; ++i )
	{
		frames[i] = fileNames[i].subString( startIndex, endIndex ).asInt();
	}

	return true;
}

void
ZWriteHoudiniStr( std::ofstream& fout, const string& str )
{
	ZWrite( fout, (short)str.size(), true );
	fout.write( str.c_str(), str.size() );
}

char* ZTextFromFile( const char *filePathName, int& textLength )
{
	char* text = (char*)NULL;

	// Open the file and move the current position to the end position of the file.
	ifstream file( filePathName, ios::in | ios::ate );

	if( file.fail() )
	{
		cout << "Failed to open file: " << filePathName << endl;
		return (char*)NULL;
	}

	textLength = file.tellg(); // the current position
	text = new char[textLength+1]; // memory allocation for storing text
	file.seekg( 0, ios::beg ); // move to the start position of the file
	file.read( text, textLength ); // read data
	file.close(); // close the file

	if( text ) { text[textLength] = '\0'; }

	return text;
}

ZELOS_NAMESPACE_END

