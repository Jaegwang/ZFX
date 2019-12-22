//---------------------//
// ZDoubleSetArray.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2018.03.08                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZDoubleSetArray::ZDoubleSetArray()
{
}

ZDoubleSetArray::ZDoubleSetArray( const ZDoubleSetArray& source )
{
	*this = source;
}

ZDoubleSetArray::ZDoubleSetArray( const ZIntArray& numElements )
{
	set( numElements );
}

ZDoubleSetArray::ZDoubleSetArray( const char* filePathName )
{
	load( filePathName );
}

void
ZDoubleSetArray::reset()
{
	_n.clear();
	_i.clear();
	_v.clear();
}

ZDoubleSetArray&
ZDoubleSetArray::operator=( const ZDoubleSetArray& source )
{
	_n = source._n;
	_i = source._i;
	_v = source._v;

	return (*this);
}

void
ZDoubleSetArray::set( const ZIntArray& numElements )
{
	_n = numElements;
	_allocate();
}

void
ZDoubleSetArray::from( const ZDoubleSetArray& other, const ZCharArray& mask )
{
	ZDoubleSetArray::reset();

	const int nSets = other.numSets();

	if( nSets != mask.length() )
	{
		cout << "Error@ZDoubleSetArray::from(): Invalid input data." << endl;
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
ZDoubleSetArray::fill( double v )
{
	_v.fill( v );
}

void
ZDoubleSetArray::zeroize()
{
	_v.zeroize();
}

// _n must be set before!
void
ZDoubleSetArray::_allocate()
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
ZDoubleSetArray::getStartElements( ZDoubleArray& elements ) const
{
	const int nSets = numSets();

	elements.setLength( nSets );

	FOR( i, 0, nSets )
	{
		elements[i] = _v[ _i[i] ];
	}
}

void
ZDoubleSetArray::getEndElements( ZDoubleArray& elements ) const
{
	const int nSets = numSets();

	elements.setLength( nSets );

	FOR( i, 0, nSets )
	{
		elements[i] = _v[ _i[i] + _n[i] - 1 ];
	}
}

void
ZDoubleSetArray::append( const ZDoubleSetArray& other )
{
	const int nSetsOld = _n.length();
	const int nTotalElements = ZDoubleSetArray::numTotalElements();

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
ZDoubleSetArray::exchangeData( ZDoubleSetArray& other )
{
	_v.exchange( other._v );
}

void
ZDoubleSetArray::write( ofstream& fout ) const
{
	// other data don't need to be saved.
	_n.write( fout, true );
	_v.write( fout, true );
}

void
ZDoubleSetArray::read( ifstream& fin )
{
	_n.read( fin, true );
	_allocate();
	_v.read( fin, true );
}

bool
ZDoubleSetArray::save( const char* filePathName ) const
{
	ofstream fout( filePathName, ios::out|ios::binary|ios::trunc );

	if( fout.fail() || !fout.is_open() )
	{
		cout << "Error@ZDoubleSetArray::save(): Failed to save file: " << filePathName << endl;
		return false;
	}

	write( fout );

	fout.close();

	return true;
}

bool
ZDoubleSetArray::load( const char* filePathName )
{
	ifstream fin( filePathName, ios::in|ios::binary );

	if( fin.fail() )
	{
		cout << "Error@ZDoubleSetArray::load(): Failed to load file." << endl;
		reset();
		return false;
	}

	read( fin );

	fin.close();

	return true;
}

double
ZDoubleSetArray::usedMemorySize( ZDataUnit::DataUnit dataUnit ) const
{
	return ( _n.usedMemorySize(dataUnit) + _i.usedMemorySize(dataUnit) + _v.usedMemorySize(dataUnit) );
}

void
ZDoubleSetArray::print() const
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
operator<<( ostream& os, const ZDoubleSetArray& object )
{
	os << "<ZDoubleSetArray>" << endl;
	return os;
}

ZELOS_NAMESPACE_END

