//-------------------------//
// ZAlembicObjectArray.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2017.01.20                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZAlembicObjectArray::ZAlembicObjectArray()
{
	ZAlembicObjectArray::reset();
}

ZAlembicObjectArray::~ZAlembicObjectArray()
{
	ZAlembicObjectArray::reset();
}

void
ZAlembicObjectArray::reset()
{
	const int N = (int)_data.size();

	FOR( i, 0, N )
	{
		delete( _data[i] );
	}

	_data.clear();
}

void
ZAlembicObjectArray::append( ZAlembicObject* elementPtr )
{
	_data.push_back( elementPtr );
}

void
ZAlembicObjectArray::append( const ZAlembicObject& element )
{
	ZAlembicObject* elementPtr = new ZAlembicObject();
	*elementPtr = element;

	_data.push_back( elementPtr );
}

int
ZAlembicObjectArray::length() const
{
	return int( _data.size() );
}

ZAlembicObject&
ZAlembicObjectArray::operator[]( const int& i )
{
	return (*_data[i]);
}

ZAlembicObject&
ZAlembicObjectArray::operator()( const int& i )
{
	return (*_data[i]);
}

ZAlembicObject&
ZAlembicObjectArray::last( const int& i )
{
	return (*_data.back());
}

const ZAlembicObject&
ZAlembicObjectArray::operator[]( const int& i ) const
{
	return (*_data[i]);
}

const ZAlembicObject&
ZAlembicObjectArray::operator()( const int& i ) const
{
	return (*_data[i]);
}

const ZAlembicObject&
ZAlembicObjectArray::last( const int& i ) const
{
	return (*_data.back());
}

void
ZAlembicObjectArray::reverse()
{
	std::reverse( _data.begin(), _data.end() );
}

int
ZAlembicObjectArray::remove( const ZIntArray& indicesToBeDeleted )
{
	const int n = (int)_data.size();
	if( !n ) { return n; }

	const int count = indicesToBeDeleted.length();
	if( !count ) { return n; }

	ZCharArray mask( n, (char)false );
	{
		FOR( i, 0, count )
		{
			const int& idx = indicesToBeDeleted[i];

			if( idx <  0 ) { continue; }
			if( idx >= n ) { continue; }

			mask[idx] = (char)true;
		}
	}

	vector<ZAlembicObject*> tmp;
	{
		FOR( i, 0, n )
		{
			if( mask[i] ) { continue; }

			tmp.push_back( _data[i] );
		}
	}

	_data.swap( tmp );

	return (int)_data.size();
}

void
ZAlembicObjectArray::getFrameRange( double& minFrame, double& maxFrame ) const
{
	const int n = (int)_data.size();
	if( !n ) { minFrame = maxFrame = 0.0; return; }

	minFrame = +Z_LARGE;
	maxFrame = -Z_LARGE;

	bool pureStatic = true;

	FOR( i, 0, n )
	{
		const ZAlembicObject& obj = *(_data[i]);

		if( obj.isConstant() ) { continue; }

		minFrame = ZMin( minFrame, obj.minFrame() );
		maxFrame = ZMax( maxFrame, obj.maxFrame() );

		pureStatic = false;
	}

	if( pureStatic )
	{
		minFrame = maxFrame = 0.0;
	}
}

void
ZAlembicObjectArray::getTimeRange( double& minTime, double& maxTime ) const
{
	const int n = (int)_data.size();
	if( !n ) { minTime = maxTime = 0.0; return; }

	minTime = +Z_LARGE;
	maxTime = -Z_LARGE;

	bool pureStatic = true;

	FOR( i, 0, n )
	{
		const ZAlembicObject& obj = *(_data[i]);

		if( obj.isConstant() ) { continue; }

		minTime = ZMin( minTime, obj.minTime() );
		maxTime = ZMax( maxTime, obj.maxTime() );

		pureStatic = false;
	}

	if( pureStatic )
	{
		minTime = maxTime = 0.0;
	}
}

ostream&
operator<<( ostream& os, const ZAlembicObjectArray& object )
{
	os << "<ZAlembicObjectArray>" << endl;
	os << " size: " << object.length() << endl;
	os << endl;

	return os;
}

ZELOS_NAMESPACE_END

