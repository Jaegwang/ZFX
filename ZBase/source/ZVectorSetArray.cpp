//---------------------//
// ZVectorSetArray.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2018.03.08                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZVectorSetArray::ZVectorSetArray()
{
}

ZVectorSetArray::ZVectorSetArray( const ZVectorSetArray& source )
{
	*this = source;
}

ZVectorSetArray::ZVectorSetArray( const ZIntArray& numElements )
{
	set( numElements );
}

ZVectorSetArray::ZVectorSetArray( const char* filePathName )
{
	load( filePathName );
}

void
ZVectorSetArray::reset()
{
	_n.clear();
	_i.clear();
	_v.clear();
}

ZVectorSetArray&
ZVectorSetArray::operator=( const ZVectorSetArray& source )
{
	_n = source._n;
	_i = source._i;
	_v = source._v;

	return (*this);
}

void
ZVectorSetArray::set( const ZIntArray& nCVs )
{
	_n = nCVs;
	_allocate();
}

void
ZVectorSetArray::fill( const ZVector& v )
{
	_v.fill( v );
}

void
ZVectorSetArray::zeroize()
{
	_v.zeroize();
}

// _n must be set before!
void
ZVectorSetArray::_allocate()
{
	const int nSets = _n.length();
	_i.setLength( nSets );

	int nTotalElements = 0;
	FOR( i, 0, nSets )
	{
		_i[i] = nTotalElements;
		nTotalElements += _n[i];
	}

	_v.setLength( nTotalElements );
}

ZBoundingBox
ZVectorSetArray::boundingBox( bool onlyEndPoints, bool useOpenMP ) const
{
	if( onlyEndPoints ) {

		const int nSets = numSets();

		ZPointArray ends( 2*nSets );
		FOR( i, 0, nSets )
		{
			int i2 = i*2;
			ends[  i2] = start(i);
			ends[++i2] = end  (i);
		}

		return ends.boundingBox( useOpenMP );

	} else {

		return _v.boundingBox( useOpenMP );

	}
}

void
ZVectorSetArray::getStartElements( ZVectorArray& elements ) const
{
	const int nSets = numSets();

	elements.setLength( nSets );

	FOR( i, 0, nSets )
	{
		elements[i] = _v[ _i[i] ];
	}
}

void
ZVectorSetArray::getEndElements( ZVectorArray& elements ) const
{
	const int nSets = numSets();

	elements.setLength( nSets );

	FOR( i, 0, nSets )
	{
		elements[i] = _v[ _i[i] + _n[i] - 1 ];
	}
}

void
ZVectorSetArray::append( const ZVectorSetArray& other )
{
	const int old_n = _i.length();

	const int last_i = _i.last();
	const int last_n = _n.last();

	_n.append( other._n );
	_i.append( other._i );
	_v.append( other._v );

	const int new_n = _i.length();

	FOR( i, old_n, new_n )
	{
		_i[i] += last_i + last_n;
	}
}

void
ZVectorSetArray::exchangeData( ZVectorSetArray& other )
{
	_v.exchange( other._v );
}

void
ZVectorSetArray::write( ofstream& fout ) const
{
	// other data don't need to be saved.
	_n.write( fout, true );
	_v.write( fout, true );
}

void
ZVectorSetArray::read( ifstream& fin )
{
	_n.read( fin, true );
	_allocate();
	_v.read( fin, true );
}

bool
ZVectorSetArray::save( const char* filePathName ) const
{
	ofstream fout( filePathName, ios::out|ios::binary|ios::trunc );

	if( fout.fail() || !fout.is_open() )
	{
		cout << "Error@ZVectorSetArray::save(): Failed to save file: " << filePathName << endl;
		return false;
	}

	write( fout );

	fout.close();

	return true;
}

bool
ZVectorSetArray::load( const char* filePathName )
{
	ifstream fin( filePathName, ios::in|ios::binary );

	if( fin.fail() )
	{
		cout << "Error@ZVectorSetArray::load(): Failed to load file." << endl;
		reset();
		return false;
	}

	read( fin );

	fin.close();

	return true;
}

double
ZVectorSetArray::usedMemorySize( ZDataUnit::DataUnit dataUnit ) const
{
	return ( _n.usedMemorySize(dataUnit) + _i.usedMemorySize(dataUnit) + _v.usedMemorySize(dataUnit) );
}

ostream&
operator<<( ostream& os, const ZVectorSetArray& object )
{
	os << "<ZVectorSetArray>" << endl;
	return os;
}

ZELOS_NAMESPACE_END

