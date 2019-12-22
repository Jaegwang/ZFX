//-------------//
// ZString.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2017.11.22                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZString::ZString()
: string()
{}

ZString::ZString( const char* str )
: string( str )
{}

ZString::ZString( const string& str )
: string( str )
{
}

ZString::ZString( const ZString& str )
: string()
{
	parent::operator=( str );
}

ZString::ZString( char c )
: string()
{
	parent::operator=( c );
}

ZString::ZString( unsigned char c )
: string()
{
	parent::operator=( c );
}

ZString::ZString( char c, int length )
: string()
{
	parent::assign( length, c );
}

ZString::ZString( unsigned char c, int length )
: string()
{
	parent::assign( length, c );
}

ZString::ZString( bool v )
: string()
{
	ostringstream oss;

	if( v ) { oss << "true";  }
	else    { oss << "false"; }

	parent::assign( oss.str() );
}

ZString::ZString( short v )
: string()
{
	ostringstream oss;
	oss << v;
	parent::assign( oss.str() );
}

ZString::ZString( unsigned short v )
: string()
{
	ostringstream oss;
	oss << v;
	parent::assign( oss.str() );
}

ZString::ZString( int v )
: string()
{
	ostringstream oss;
	oss << v;
	parent::assign( oss.str() );
}

ZString::ZString( unsigned int v )
: string()
{
	ostringstream oss;
	oss << v;
	parent::assign( oss.str() );
}

ZString::ZString( long int v )
: string()
{
	ostringstream oss;
	oss << v;
	parent::assign( oss.str() );
}

ZString::ZString( unsigned long int v )
: string()
{
	ostringstream oss;
	oss << v;
	parent::assign( oss.str() );
}

ZString::ZString( float v )
: string()
{
	ostringstream oss;
	oss << v;
	parent::assign( oss.str() );
}

ZString::ZString( double v )
: string()
{
	ostringstream oss;
	oss << v;
	parent::assign( oss.str() );
}

ZString::ZString( long double v )
: string()
{
	ostringstream oss;
	oss << v;
	parent::assign( oss.str() );
}

ZString&
ZString::fromTextFile( const char* filePathName )
{
	char* text = (char*)NULL;

	// Open the file and move the current position to the end position of the file.
	ifstream file( filePathName, ios::in | ios::ate );

	if( file.fail() ) {

		parent::clear();

	} else {

		const int textLength = file.tellg(); // the current position
		text = new char[textLength]; // memory allocation for storing text
		file.seekg( 0, ios::beg ); // move to the start position of the file
		file.read( text, textLength ); // read data
		file.close(); // close the file

		if( text ) { text[textLength] = '\0'; } // add an end line char at the end

		this->operator=( text ); // copy to this ZString intance
		if( text ) { delete[] text; } // delete memories

	}

	return (*this);
}

// file pointer version
//ZString&
//ZString::fromTextFile( const char* filePathName )
//{
//	FILE* fp = fopen( filePathName, "rt" );
//
//	if( fp ) {
//
//		fseek( fp, 0, SEEK_END ); // to the end position of the file
//		int count = (int)ftell( fp ); // get the current position  in the stream
//		rewind( fp ); // to the start position of the file
//
//		char* text = (char*)NULL;
//		if( count > 0 )
//		{
//			text = (char*)malloc( sizeof(char)*(count+1) );
//			count = fread( text, sizeof(char), count, fp );
//			text[count] = '\0';
//		}
//
//		fclose( fp );
//
//		this->operator=( text );
//
//	} else {
//
//		parent::clear();
//
//	}
//
//	return (*this);
//}

ZString&
ZString::operator=( const char* str )
{
	if( !str ) { parent::clear(); }
	parent::assign( str );
	return (*this);
}

ZString&
ZString::operator=( const string& str )
{
	parent::assign( str );
	return (*this);
}

ZString&
ZString::operator=( const ZString& str )
{
	parent::assign( str );
	return (*this);
}

ZString&
ZString::operator=( char c )
{
	parent::operator=( c );
	return (*this);
}

ZString&
ZString::operator=( unsigned char c )
{
	parent::operator=( c );
	return (*this);
}

ZString&
ZString::operator=( bool v )
{
	if( v ) { parent::operator=( "true"  ); }
	else    { parent::operator=( "false" ); }

	return (*this);
}

ZString&
ZString::operator=( short v )
{
	ostringstream oss;
	oss << v;
	parent::operator=( oss.str() );
	return (*this);
}

ZString&
ZString::operator=( unsigned short v )
{
	ostringstream oss;
	oss << v;
	parent::operator=( oss.str() );
	return (*this);
}

ZString&
ZString::operator=( int v )
{
	ostringstream oss;
	oss << v;
	parent::operator=( oss.str() );
	return (*this);
}

ZString&
ZString::operator=( unsigned int v )
{
	ostringstream oss;
	oss << v;
	parent::operator=( oss.str() );
	return (*this);
}

ZString&
ZString::operator=( long int v )
{
	ostringstream oss;
	oss << v;
	parent::operator=( oss.str() );
	return (*this);
}

ZString&
ZString::operator=( unsigned long int v )
{
	ostringstream oss;
	oss << v;
	parent::operator=( oss.str() );
	return (*this);
}

ZString&
ZString::operator=( float v )
{
	ostringstream oss;
	oss << v;
	parent::operator=( oss.str() );
	return (*this);
}

ZString&
ZString::operator=( double v )
{
	ostringstream oss;
	oss << v;
	parent::operator=( oss.str() );
	return (*this);
}

ZString&
ZString::operator=( long double v )
{
	ostringstream oss;
	oss << v;
	parent::operator=( oss.str() );
	return (*this);
}

bool
ZString::operator==( const char* other ) const
{
	if( !other ) { return false; }
	ZString tmpStr( other );
	return this->operator==( tmpStr );
}

bool
ZString::operator==( const ZString& other ) const
{
	const int len = (int)parent::length();

	if( len != (int)other.length() )
	{
		return false;
	}

	FOR( i, 0, len )
	{
		if( parent::at(i) != other[i] ) { return false; }
	}

	return true;
}

bool
ZString::operator!=( const char* other ) const
{
	if( !other ) { return false; }
	ZString tmpStr( other );
	return this->operator!=( tmpStr );
}

bool
ZString::operator!=( const ZString& other ) const
{
	const int len = (int)parent::length();

	if( len != (int)other.length() )
	{
		return true;
	}

	FOR( i, 0, len )
	{
		if( parent::at(i) != other[i] ) { return true; }
	}

	return false;
}

void
ZString::set( const char* str, ... )
//{
//  va_list arglist;
//  va_start( arglist, str );
//  vprintf( str, arglist );
//  va_end( arglist );
//}
{
	va_list args;

	va_start( args, str );
	vprintf( str, args );
	va_end( args );
}

int
ZString::length() const
{
	return (int)parent::length();
}

int
ZString::firstIndexOf( char c ) const
{
	return parent::find_first_of( c );
}

int
ZString::lastIndexOf( char c ) const
{
	return parent::find_last_of( c );
}

ZString
ZString::subString( int start, int end ) const
{
	const int len = end - start + 1;
	if( len <= 0 ) { return ZString(); }

	ZString substr;
    substr.resize( len );
	memcpy( (void*)&substr[0], (const void*)&parent::at(start), len*sizeof(char) );

	return substr;
}

bool
ZString::isDigit() const
{
	const ZString& s = *this;
	const int len = (int)s.length();

	FOR( i, 0, len )
	{
		if( !ZIsDigit( s[i] ) ) { return false; }
	}

	return true;
}

bool
ZString::isAlpha() const
{
	const ZString& s = *this;
	const int len = (int)s.length();

	FOR( i, 0, len )
	{
		if( !( ZIsLower(s[i]) || ZIsUpper(s[i]) ) ) { return false; }
	}

	return true;
}

bool
ZString::isAlnum() const
{
	const ZString& s = *this;
	const int len = (int)s.length();

	FOR( i, 0, len )
	{
		if( !( ZIsDigit(s[i]) || ZIsLower(s[i]) || ZIsUpper(s[i]) ) ) { return false; }
	}

	return true;
}

bool
ZString::isLower() const
{
	const ZString& s = *this;
	const int len = (int)s.length();

	FOR( i, 0, len )
	{
		if( ZIsUpper( s[i] ) ) { return false; }
	}

	return true;
}

bool
ZString::isUpper() const
{
	const ZString& s = *this;
	const int len = (int)s.length();

	FOR( i, 0, len )
	{
		if( ZIsLower( s[i] ) ) { return false; }
	}

	return true;
}

ZString&
ZString::lowerize()
{
	ZString& s = *this;
	const int len = (int)s.length();

	FOR( i, 0, len )
	{
		if( ZIsUpper( s[i] ) ) { s[i] = ZToLower( s[i] ); }
	}

	return (*this);
}

ZString&
ZString::upperize()
{
	ZString& s = *this;
	const int len = (int)s.length();

	FOR( i, 0, len )
	{
		if( ZIsLower( s[i] ) ) { s[i] = ZToUpper( s[i] ); }
	}

	return (*this);
}

ZString
ZString::toLower() const
{
	ZString s( *this );
	s.lowerize();
	return s;
}

ZString
ZString::toUpper() const
{
	ZString s( *this );
	s.upperize();
	return s;
}

ZString&
ZString::capitalize()
{
	ZString& s = *this;
	const int len = (int)s.length();

	if( len > 0 )
	{
		if( ZIsLower( s[0] ) ) { s[0] = ZToUpper( s[0] ); }
	}

	FOR( i, 1, len )
	{
		if( ZIsUpper( s[i] ) ) { s[i] = ZToLower( s[i] ); }
	}

	return (*this);
}

ZString&
ZString::swapCase()
{
	ZString& s = *this;
	const int len = (int)s.length();

	FOR( i, 0, len )
	{
		if( ZIsLower( s[i] ) ) { s[i] = ZToUpper( s[i] ); }
		else if( ZIsUpper( s[i] ) ) { s[i] = ZToLower( s[i] ); }
	}

	return (*this);
}

const char*
ZString::asChar() const
{
	return parent::c_str();
}

char
ZString::at0() const
{
	return parent::at(0);
}

int
ZString::asInt() const
{
	return atoi( parent::c_str() );
}

float
ZString::asFloat() const
{
	return atof( parent::c_str() );
}

double
ZString::asDouble() const
{
	return strtod( parent::c_str(), NULL );
}

void
ZString::replace( char from, char to )
{
	ZString& s = *this;
	const int len = (int)s.length();

	FOR( i, 0, len )
	{
		if( s[i] == from ) { s[i] = to; }
	}
}

void
ZString::replace( const char* from, const char* to )
{
	const int fromLen = (int)strlen( from );
	const int toLen   = (int)strlen( to );

	if( !fromLen || !toLen )
	{
		return;
	}

	int index = 0;

	while( (index=std::string::find(from,index)) != std::string::npos )
	{
		std::string::replace( index, fromLen, to );
		index += toLen;
	}
}

void
ZString::reverse()
{
	ZString& s = *this;
	const int len = (int)s.length();

	FOR( i, 0, len/2 )
	{
		ZSwap( s[i], s[len-1-i] );
	}
}

ZString
ZString::commify( int num )
{
	bool minus = false;
	if( num < 0 )
	{
		minus = true;
		num *= -1;
	}

	stringstream ss;
	ss << num;
	string str = ss.str();

	int len = (int)str.size();
	if( len == 0 ) { return str; }

	int numCommas = (int)((len-1)/3);
	int totalLen = len + numCommas;

	string result( totalLen, '.' );

	int shift = -len;

	int i=0, j=0;
	while( result[i] )
	{
		result[i++] = str[j++];
		if( ++shift && (shift%3) == 0 ) { result[i++] = ','; }
	}

	if( minus )
	{
		result.insert( 0, "-" );
	}

	return result;
}

int
ZString::split( const ZString& delimiter, vector<ZString>& tokens ) const
{
	tokens.clear();

	parent::size_type lastPos = parent::find_first_not_of( delimiter, 0 );
	parent::size_type pos = parent::find_first_of( delimiter, lastPos );

	while( parent::npos != pos || parent::npos != lastPos )
	{
		tokens.push_back( parent::substr( lastPos, pos - lastPos ) );
		lastPos = parent::find_first_not_of( delimiter, pos );
		pos = parent::find_first_of( delimiter, lastPos );
	}

	return (int)tokens.size();
}

void
ZString::removeSpace()
{
	vector<ZString> tokens;
	split( " ", tokens );
	parent::clear();
	const int len = (int)tokens.size();
	FOR( i, 0, len )
	{
		*this += tokens[i];
	}
}

int
ZString::count( char c ) const
{
    return (int)std::count( parent::begin(), parent::end(), c );
}

ZString
ZString::makePadding( int number, int padding )
{
	ostringstream oss;
	oss << setfill('0') << setw(padding) << number;
	return oss.str();
}

void
ZString::write( ofstream& fout, bool writeStringLength ) const
{
	const int n = length();
	if( writeStringLength ) { fout.write( (char*)&n, sizeof(int) ); }
	if( n ) { fout.write( (char*)&parent::at(0), n*sizeof(char) ); }
}

void
ZString::read( ifstream& fin, bool readStringLength )
{
	int n = length();
	if( readStringLength ) { fin.read( (char*)&n, sizeof(int) ); parent::resize( n ); }
	if( n ) { fin.read( (char*)&parent::at(0), n*sizeof(char) ); } else { parent::clear(); }
}

bool
ZString::save( const char* filePathName ) const
{
	ofstream fout( filePathName, ios::out|ios::binary|ios::trunc );

	if( fout.fail() || !fout.is_open() )
	{
		cout << "Error@ZString::save(): Failed to save file: " << filePathName << endl;
		return false;
	}

	write( fout, true );

	fout.close();

	return true;
}

bool
ZString::load( const char* filePathName )
{
	ifstream fin( filePathName, ios::in|ios::binary );

	if( fin.fail() )
	{
		cout << "Error@ZString::load(): Failed to load file." << endl;
		parent::clear();
		return false;
	}

	read( fin, true );

	fin.close();

	return true;
}

ostream&
operator<<( ostream& os, const ZString& object )
{
	const int n = object.length();
	FOR( i, 0, n ) { os << object[i]; }
	return os;
}

ZELOS_NAMESPACE_END

