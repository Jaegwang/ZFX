//------------------//
// ZStringArray.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZStringArray::ZStringArray()
{}

ZStringArray::ZStringArray( const ZStringArray& a )
: ZArray<ZString>::ZArray( a )
{}

ZStringArray::ZStringArray( int l )
: ZArray<ZString>::ZArray( l )
{}

ZStringArray::ZStringArray( int l, const ZString& v )
: ZArray<ZString>::ZArray( l, v )
{}

ZString&
ZStringArray::append( const char* str )
{
	parent::push_back( ZString(str) );
	return *parent::rbegin();
}

ZString&
ZStringArray::append( const ZString& str )
{
	parent::push_back( str );
	return *parent::rbegin();
}

void
ZStringArray::append( const ZStringArray& a )
{
	parent::resize( parent::size()+a.size() );
	std::copy( a.begin(), a.end(), parent::end()-a.size() );
}

ZString
ZStringArray::combine( const ZString& separator ) const
{
	const int len = (int)parent::size();

	ZString str( parent::at(0) );
	FOR( i, 1, len )
	{
		str += separator + parent::at(i);
	}

	return str;
}

void
ZStringArray::write( ofstream& fout ) const
{
	const int n = (int)parent::size();

	fout.write( (char*)&n, sizeof(int) );

	FOR( i, 0, n )
	{
		const ZString& str = parent::at(i);
		str.write( fout, true );
	}
}

void 
ZStringArray::read( ifstream& fin )
{
	reset();

	int n = 0;
	fin.read( (char*)&n, sizeof(int) );
	if( !n ) { return; }

	ZString str;

	FOR( i, 0, n )
	{
		str.read( fin, true );
		parent::push_back( str );
	}
}

bool 
ZStringArray::save( const char* filePathName ) const
{
	ofstream fout( filePathName, ios::out|ios::binary|ios::trunc );

	if( fout.fail() || !fout.is_open() )
	{
		cout << "Error@ZStringArray::save(): Failed to save file: " << filePathName << endl;
		return false;
	}

	write( fout );
	fout.close();

	return true;
}

bool 
ZStringArray::load( const char* filePathName )
{
	ifstream fin( filePathName, ios::in|ios::binary );

	if( fin.fail() )
	{
		cout << "Error@ZStringArray::load(): Failed to load file." << endl;
		reset();
		return false;
	}

	read( fin );
	fin.close();

	return true;
}

ostream&
operator<<( ostream& os, const ZStringArray& object )
{
	os << "<ZStringArray>" << endl;
	os << " size     : " << object.size() << endl;
	os << " capacity : " << object.capacity() << endl;
	os << endl;
	return os;
}

ZELOS_NAMESPACE_END

