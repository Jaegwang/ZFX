//---------------------------//
// ZAlembicPropertyArray.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2017.01.20                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZAlembicPropertyArray::ZAlembicPropertyArray()
{
	ZAlembicPropertyArray::reset();
}

ZAlembicPropertyArray::~ZAlembicPropertyArray()
{
	ZAlembicPropertyArray::reset();
}

void
ZAlembicPropertyArray::reset()
{
	const int N = (int)_data.size();

	FOR( i, 0, N )
	{
		delete( _data[i] );
	}

	_data.clear();
}

void
ZAlembicPropertyArray::append( ZAlembicProperty* elementPtr )
{
	_data.push_back( elementPtr );
}

void
ZAlembicPropertyArray::append( const ZAlembicProperty& element )
{
	ZAlembicProperty* elementPtr = new ZAlembicProperty();
	*elementPtr = element;

	_data.push_back( elementPtr );
}

int
ZAlembicPropertyArray::length() const
{
	return int( _data.size() );
}

ZAlembicProperty&
ZAlembicPropertyArray::operator[]( const int& i )
{
	return (*_data[i]);
}

ZAlembicProperty&
ZAlembicPropertyArray::last( const int& i )
{
	return (*_data.back());
}

ZAlembicProperty&
ZAlembicPropertyArray::operator()( const int& i )
{
	return (*_data[i]);
}

const ZAlembicProperty&
ZAlembicPropertyArray::operator[]( const int& i ) const
{
	return (*_data[i]);
}

const ZAlembicProperty&
ZAlembicPropertyArray::operator()( const int& i ) const
{
	return (*_data[i]);
}

const ZAlembicProperty&
ZAlembicPropertyArray::last( const int& i ) const
{
	return (*_data.back());
}

void
ZAlembicPropertyArray::reverse()
{
	std::reverse( _data.begin(), _data.end() );
}

ostream&
operator<<( ostream& os, const ZAlembicPropertyArray& object )
{
	os << "<ZAlembicPropertyArray>" << endl;
	os << " size: " << object.length() << endl;
	os << endl;

	return os;
}

ZELOS_NAMESPACE_END

