//------------------//
// ZIntSetArray.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2018.03.08                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZIntSetArray::ZIntSetArray()
{
}

ZIntSetArray::ZIntSetArray( const ZIntSetArray& source )
{
	*this = source;
}

ZIntSetArray::ZIntSetArray( const ZIntArray& numElements )
{
	set( numElements );
}

ZIntSetArray::ZIntSetArray( const char* filePathName )
{
	load( filePathName );
}

void
ZIntSetArray::reset()
{
	_n.clear();
	_i.clear();
	_v.clear();
}

ZIntSetArray&
ZIntSetArray::operator=( const ZIntSetArray& source )
{
	_n = source._n;
	_i = source._i;
	_v = source._v;

	return (*this);
}

void
ZIntSetArray::set( const ZIntArray& numElements )
{
	_n = numElements;
	_allocate();
}

void
ZIntSetArray::from( const ZIntSetArray& other, const ZCharArray& mask )
{
	ZIntSetArray::reset();

	const int nSets = other.numSets();

	if( nSets != mask.length() )
	{
		cout << "Error@ZIntSetArray::from(): Invalid input data." << endl;
		return;
	}

	int count = 0;
	{
		FOR( i, 0, nSets )
		{
			if( mask[i] )
			{
				++count;
			}
		}
	}

	_n.setLength( count );
	_i.setLength( count );

	int idx = 0;
	int nTotalElements = 0;
	FOR( i, 0, nSets )
	{
		if( mask[i] )
		{
			_i[idx] = nTotalElements;
			nTotalElements += ( _n[idx] = other._n[i] );

			++idx;
		}
	}

	_v.reserve( nTotalElements );

	FOR( i, 0, nSets )
	{
		if( mask[i] )
		{
			const int& nElems = other.count(i);

			FOR( j, 0, nElems )
			{
				_v.push_back( other(i,j) );
			}
		}
	}
}

void
ZIntSetArray::fill( int v )
{
	_v.fill( v );
}

void
ZIntSetArray::zeroize()
{
	_v.zeroize();
}

// _n must be set before!
void
ZIntSetArray::_allocate()
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

void
ZIntSetArray::getStartElements( ZIntArray& elements ) const
{
	const int nSets = numSets();

	elements.setLength( nSets );

	FOR( i, 0, nSets )
	{
		elements[i] = _v[ _i[i] ];
	}
}

void
ZIntSetArray::getEndElements( ZIntArray& elements ) const
{
	const int nSets = numSets();

	elements.setLength( nSets );

	FOR( i, 0, nSets )
	{
		elements[i] = _v[ _i[i] + _n[i] - 1 ];
	}
}

void
ZIntSetArray::append( const ZIntSetArray& other )
{
	const int nSetsOld = _n.length();
	const int nTotalElements = ZIntSetArray::numTotalElements();

	_n.append( other._n );
	_i.append( other._i );
	_v.append( other._v );

	const int nSetsNew = _n.length();

	FOR( i, nSetsOld, nSetsNew )
	{
		_i[i] += nTotalElements;
	}
}

void
ZIntSetArray::exchangeData( ZIntSetArray& other )
{
	_v.exchange( other._v );
}

void
ZIntSetArray::write( ofstream& fout ) const
{
	// other data don't need to be saved.
	_n.write( fout, true );
	_v.write( fout, true );
}

void
ZIntSetArray::read( ifstream& fin )
{
	_n.read( fin, true );
	_allocate();
	_v.read( fin, true );
}

bool
ZIntSetArray::save( const char* filePathName ) const
{
	ofstream fout( filePathName, ios::out|ios::binary|ios::trunc );

	if( fout.fail() || !fout.is_open() )
	{
		cout << "Error@ZIntSetArray::save(): Failed to save file: " << filePathName << endl;
		return false;
	}

	write( fout );

	fout.close();

	return true;
}

bool
ZIntSetArray::load( const char* filePathName )
{
	ifstream fin( filePathName, ios::in|ios::binary );

	if( fin.fail() )
	{
		cout << "Error@ZIntSetArray::load(): Failed to load file." << endl;
		reset();
		return false;
	}

	read( fin );

	fin.close();

	return true;
}

double
ZIntSetArray::usedMemorySize( ZDataUnit::DataUnit dataUnit ) const
{
	return ( _n.usedMemorySize(dataUnit) + _i.usedMemorySize(dataUnit) + _v.usedMemorySize(dataUnit) );
}

void
ZIntSetArray::print() const
{
	FOR( i, 0, numSets() )
	{
		cout << i << ": ";

		FOR( j, 0, count(i) )
		{
			cout << operator()(i,j) << " ";
		}

		cout << endl;
	}
}

ostream&
operator<<( ostream& os, const ZIntSetArray& object )
{
	os << "<ZIntSetArray>" << endl;
	return os;
}

ZELOS_NAMESPACE_END

