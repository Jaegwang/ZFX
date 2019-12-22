//-----------//
// ZJSON.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2016.09.06                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZJSON::ZJSON()
{}

void
ZJSON::clear()
{
	data.clear();
}

void
ZJSON::append( const char* key, int value )
{
	data[ ZString( key ) ] = ZString( value );
}

void
ZJSON::append( const char* key, float value )
{
	data[ ZString( key ) ] = ZString( value );
}

void
ZJSON::append( const char* key, double value )
{
	data[ ZString( key ) ] = ZString( value );
}

void
ZJSON::append( const char* key, const ZString& value )
{
	data[ ZString( key ) ] = value;
}

ZString
ZJSON::operator[]( const char* key ) const
{
	std::map<ZString,ZString>::const_iterator itr = data.find( key );

	if( itr == data.end() ) { return ZString(); }

	return (itr->second );
}

ZString
ZJSON::get( const char* key ) const
{
	std::map<ZString,ZString>::const_iterator itr = data.find( key );

	if( itr == data.end() ) { return ZString(); }

	return (itr->second );
}

bool
ZJSON::get( const char* key, int& value ) const
{
	std::map<ZString,ZString>::const_iterator itr = data.find( key );
	if( itr == data.end() ) { return false; }

	value = (itr->second).asInt();

	return true;
}

bool
ZJSON::get( const char* key, float& value ) const
{
	std::map<ZString,ZString>::const_iterator itr = data.find( key );
	if( itr == data.end() ) { return false; }

	value = (itr->second).asFloat();

	return true;
}

bool
ZJSON::get( const char* key, ZString& value ) const
{
	std::map<ZString,ZString>::const_iterator itr = data.find( key );
	if( itr == data.end() ) { return false; }

	value = itr->second;

	return true;
}

bool
ZJSON::save( const char* filePathName ) const
{
	ofstream fout( filePathName, ios::out );

	fout << "{" << endl;

	std::map<ZString,ZString>::const_iterator itr = data.begin();

	for( ; itr != data.end(); ++itr )
	{
		fout << "\t\"" << itr->first << "\":\"" << itr->second << "\"," << endl;
	}

	fout << "}" << endl;

	fout.close();

	return true;
}

bool
ZJSON::load( const char* filePathName )
{
	clear();

	ifstream fin( filePathName, ios::in );

	if( fin.fail() )
	{
		cout << "Error@ZJSON::load(): Failed to load file." << endl;
		return false;
	}

	ZString str;
	ZStringArray tokens;

	while( !fin.eof() )
	{
		getline( fin, str );

		str.split( "\":\"", tokens );
		if( tokens.size() < 4 ) { continue; }

		append( tokens[1].asChar(), tokens[2] );
	}

	fin.close();

	return true;
}

ostream&
operator<<( ostream& os, const ZJSON& obj )
{
	os << "<ZJSON>" << endl;

	std::map<ZString,ZString>::const_iterator itr = obj.data.begin();

	for( ; itr != obj.data.end(); ++itr )
	{
		cout << itr->first << ": " << itr->second << endl;
	}

	os << endl;

	return os;
}

ZELOS_NAMESPACE_END

