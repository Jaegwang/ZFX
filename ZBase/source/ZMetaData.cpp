//---------------//
// ZMetaData.cpp //
//-------------------------------------------------------//
// author: Jaegwang Lim @ Dexter Studios                 //
// last update: 2017.04.16                               //
//-------------------------------------------------------//

#include <ZMetaData.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

ZELOS_NAMESPACE_BEGIN

ZMetaData::ZMetaData()
{
}

void
ZMetaData::clear()
{
	intData.clear();
	floatData.clear();
	vectorData.clear();
	stringData.clear();
}

void
ZMetaData::append( const char* name, const float& value )
{
	floatData[ ZString(name) ] = value;
}

void
ZMetaData::append( const char* name, const double& value )
{
	floatData[ ZString(name) ] = (float)value;
}

void
ZMetaData::append( const char* name, const int& value )
{
	intData[ ZString(name) ] = value;
}

void
ZMetaData::append( const char* name, const ZString& value )
{
	stringData[ ZString(name) ] = value;
}

void
ZMetaData::append( const char* name, const ZVector& value )
{
	vectorData[ ZString(name) ] = value;
}

bool
ZMetaData::get( const char* name, float& value )
{
	std::map<ZString,float>::const_iterator itr = floatData.find( name );
	if( itr == floatData.end() ) return false;

	value = itr->second;
	return true;
}

bool
ZMetaData::get( const char* name, double& value )
{
	std::map<ZString,float>::const_iterator itr = floatData.find( name );
	if( itr == floatData.end() ) return false;

	value = (double)itr->second;
	return true;
}

bool
ZMetaData::get( const char* name, int& value )
{
	std::map<ZString,int>::const_iterator itr = intData.find( name );
	if( itr == intData.end() ) return false;

	value = itr->second;
	return true;
}

bool
ZMetaData::get( const char* name, ZVector& value )
{	
	std::map<ZString,ZVector>::const_iterator itr = vectorData.find( name );
	if( itr == vectorData.end() ) return false;

	value = itr->second;
	return true;
}

bool
ZMetaData::get( const char* name, ZString& value )
{
	std::map<ZString,ZString>::const_iterator itr = stringData.find( name );
	if( itr == stringData.end() ) return false;

	value = itr->second;
	return true;
}

ZString
ZMetaData::string()
{
	std::stringstream buffer;

	for( std::map<ZString,int>::iterator it=intData.begin(); it!=intData.end(); ++it )
	{
		buffer << "int?";
		buffer << it->first << "=";
		buffer << it->second << "#";
	}

	for( std::map<ZString,float>::iterator it=floatData.begin(); it!=floatData.end(); ++it )
	{
		buffer << "float?";
		buffer << it->first << "=";
		buffer << it->second << "#";
	}

	for( std::map<ZString,ZVector>::iterator it=vectorData.begin(); it!=vectorData.end(); ++it )
	{
		ZVector vec = it->second;
		buffer << "vector?";
		buffer << it->first << "=";
		buffer << vec.x << "," << vec.y << "," << vec.z << "#";
	}

	for( std::map<ZString,ZString>::iterator it=stringData.begin(); it!=stringData.end(); ++it )
	{
		buffer << "string?";
		buffer << it->first << "=";
		buffer << it->second << "#";
	}

	return ZString( buffer.str().c_str() );
}

void
ZMetaData::loadString( const ZString& str )
{
	ZStringArray strArray;
	ZTokenize( str, "#", strArray );

	for( int n=0; n<strArray.length(); ++n )
	{
		ZString& token = strArray[n];
		const int b = (int)token.find('?');
		const int m = (int)token.find('=');

		if( b+1 < 0 ) continue;
		if( m+1 < 0 ) continue;

		ZString name  = ZString( token.substr( b+1, m-b-1 ) );
		ZString value = ZString( token.substr( m+1 ) );

		if( (int)token.find( "int?" ) >= 0 )
		{
			intData[ name ] = value.asInt();
		}
		else if( (int)token.find( "float?" ) >= 0 )
		{
			floatData[ name ] = value.asFloat();
		}
		else if( (int)token.find( "vector?" ) >= 0 )
		{
			ZStringArray vecArray;
			ZTokenize( value, ",", vecArray );
			if( vecArray.length() == 3 )
			{
				ZVector val( vecArray[0].asFloat(), vecArray[1].asFloat(), vecArray[2].asFloat() );
				vectorData[ name ] = val;
			}
		}
		else if( (int)token.find( "string?" ) >= 0 )
		{
			stringData[ name ] = value;
		}
	}
}

ZString
ZMetaData::json()
{
	boost::property_tree::ptree root;

	for( std::map<ZString,int>::iterator it=intData.begin(); it!=intData.end(); ++it )
	{
		ZString name = ZString("int.") + it->first;
		root.put( name.asChar(), it->second );
	}

	for( std::map<ZString,float>::iterator it=floatData.begin(); it!=floatData.end(); ++it )
	{
		ZString name = ZString("float.") + it->first;
		root.put( name.asChar(), it->second );
	}

	for( std::map<ZString,ZString>::iterator it=stringData.begin(); it!=stringData.end(); ++it )
	{	
		ZString name = ZString("string.") + it->first;
		root.put( name.asChar(), (it->second).asChar() );
	}	
	
	for( std::map<ZString,ZVector>::iterator it=vectorData.begin(); it!=vectorData.end(); ++it )
	{
		ZString name = ZString("vector.") + it->first;

		boost::property_tree::ptree row;
		for(int i=0; i<3; ++i )
		{
			boost::property_tree::ptree cell;
			cell.put_value( (it->second).x );

			row.push_back( std::make_pair("", cell) );
		}

		root.add_child( name.asChar(), row );
	}

	boost::property_tree::write_json( std::cout, root );

	return ZString();
}

void
ZMetaData::loadJson( const ZString& str )
{
	boost::property_tree::ptree props;
	boost::property_tree::read_json( str.c_str(), props );

}

ZELOS_NAMESPACE_END

