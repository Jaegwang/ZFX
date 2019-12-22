//------------------//
// ZStringUtils.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZString Trim( const ZString& str )
{
	int length = (int)str.length();
	if( length == 0 ) { return str; }

	ZString retval;
	for( int i=0; i<length; ++i )
	{
		if( !isspace( str[i] ) )
		{
			retval = str.substr(i);
			break;
		}
    }

	length = (int)retval.length();
	for( int i=length-1; i>= 0; --i )
	{
		if( !isspace( retval[i] ) )
		{
			retval = retval.substr( 0, i+1 );
			break;
		}
	}

	return retval;
}

void
ZTokenize( const ZString& str, const ZString& delimiter, ZStringArray& tokens )
{
	tokens.clear();

	string::size_type lastPos = str.find_first_not_of( delimiter, 0 );
	string::size_type pos = str.find_first_of( delimiter, lastPos );

	while( string::npos != pos || string::npos != lastPos )
	{
		tokens.push_back( str.substr( lastPos, pos - lastPos ) );
		lastPos = str.find_first_not_of( delimiter, pos );
		pos = str.find_first_of( delimiter, lastPos );
	}
}

void
ZTokenize( const ZString& str, const ZString& delimiter, ZIntArray& tokens )
{
	tokens.clear();

	string::size_type lastPos = str.find_first_not_of( delimiter, 0 );
	string::size_type pos = str.find_first_of( delimiter, lastPos );

	while( string::npos != pos || string::npos != lastPos )
	{
		
		tokens.push_back( atoi( str.substr( lastPos, pos - lastPos ).c_str() ) );
		lastPos = str.find_first_not_of( delimiter, pos );
		pos = str.find_first_of( delimiter, lastPos );
	}
}

ZString
ZFileExtension( const ZString& filePathName )
{
	ZStringArray tokens;
	ZTokenize( filePathName, ".", tokens );
	return tokens.last();
}

ZString
ZFileFrameNumber( float currentTime )
{
	ZString ret;

	ZString currentTimeStr( ZString("") + currentTime );
	ZStringArray tokens;
	ZTokenize( currentTimeStr, '.', tokens );
	const int numTokens = (int)tokens.length();

	if( numTokens == 1 ) {
		ret = currentTimeStr;
	} else if( numTokens == 2 ) {
		ret = tokens[0] + "_" + tokens[1];
	} else {
		cout << "Error@FileNo(): Invalid time." << endl;
	}

	return ret;
}

bool
ZGetList( const ZString& string, ZIntArray& list )
{
	ZString str( string );
	str.removeSpace();

	vector<ZString> tokens0;
	str.split( ',', tokens0 );

	const int len0 = (int)tokens0.size();
	FOR( i, 0, len0 )
	{
		vector<ZString> tokens;
		tokens0[i].split( '-', tokens );

		bool failed = false;

		const int len = (int)tokens.size();
		if( len == 1 ) {
			const char* c = tokens[0].asChar();
			if( !ZNumericChar(c[0]) ) { failed = true; }
			list.push_back( tokens[0].asInt() );
		} else if( len == 2 ) {
			const char* c0 = tokens[0].asChar();
			const char* c1 = tokens[1].asChar();
			if( !( ZNumericChar(c0[0]) && ZNumericChar(c1[0]) ) ) { failed = true; }
			const int i0 = tokens[0].asInt();
			const int i1 = tokens[1].asInt();
			int min, max;   ZMinMax( i0, i1, min, max );
			FOR(j,min,max+1) { list.push_back(j); }
		} else {
			failed = true;
		}

		if( failed ) { list.clear(); return false; }
	}

	return true;
}

ZELOS_NAMESPACE_END

