//-------------//
// ZLogger.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZLogger::ZLogger()
{
}

void
ZLogger::setFilePathName( const char* filePathName )
{
	_filePathName = filePathName;
}

ZString
ZLogger::getFilePathName() const
{
	return _filePathName;
}

void
ZLogger::deleteFile() const
{
	ZDeleteDirectory( _filePathName.asChar() );
}

void
ZLogger::dump( const char* format, ... ) const
{
	va_list args;
    
	FILE *console = fopen( _filePathName.asChar(), "a" );
	va_start( args, format );
	vfprintf( console, format, args );
	va_end( args );
	fclose( console );
}

void
ZLogger::dump( bool onConsole, bool onFileDump, const char* format, ... ) const
{
	va_list args;
    
	if( onFileDump )
	{
		FILE *console = fopen( _filePathName.asChar(), "a" );
		va_start( args, format );
		vfprintf( console, format, args );
		va_end( args );
		fclose( console );
	}

	if( onConsole )
	{
		va_start( args, format );
		vprintf( format, args );
		va_end( args );
	}
}

ostream&
operator<<( ostream& os, const ZLogger& object )
{
	os << "<ZLogger>" << endl;
	os << "File path & name: " << object.getFilePathName() << endl;
	return os;
}

ZELOS_NAMESPACE_END

