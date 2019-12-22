//----------------------//
// ZDoubleArrayList.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2016.07.04                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZDoubleArrayList::ZDoubleArrayList()
{
}

ZDoubleArrayList::ZDoubleArrayList( const ZDoubleArrayList& a )
: vector<ZDoubleArray>(a)
{
}

ZDoubleArrayList::ZDoubleArrayList( int m, int n )
: vector<ZDoubleArray>(m)
{
	FOR( i, 0, m )
	{
		parent::at(i).resize(n);
	}
}

ZDoubleArrayList::ZDoubleArrayList( const char* filePathName )
: vector<ZDoubleArray>()
{
	load( filePathName );
}

ZDoubleArrayList&
ZDoubleArrayList::operator=( const ZDoubleArrayList& other )
{
	parent::operator=( other );
	return (*this);
}

void
ZDoubleArrayList::reset()
{
	parent::clear();
}

void
ZDoubleArrayList::setSize( int m, int n )
{
	if( m <= 0 )
	{
		parent::clear();
		return;
	}

	bool reAlloc = false;

	if( (int)parent::size() == m ) {

		FOR( i, 0, m )
		{
			if( count(i) != n )
			{
				reAlloc = true;
				break;
			}
		}

	} else {

		reAlloc = true;

	}

	if( reAlloc )
	{
		parent::clear();

		if( n > 0 ) {

			ZDoubleArray tmp( n );

			FOR( i, 0, m )
			{
				parent::push_back( tmp );
			}

		} else {

			FOR( i, 0, m )
			{
				parent::clear();
			}

		}
	}
}

void
ZDoubleArrayList::fill( double valueForAll )
{
	const int n = (int)parent::size();

	FOR( i, 0, n )
	{
		parent::at(i).fill( valueForAll );
	}
}

void
ZDoubleArrayList::setLength( int i, int n )
{
	if( i >= (int)parent::size() ) { return; }
	if( n <= 0 ) { parent::at(i).clear(); return; }
	parent::at(i).setLength( n );
}

int
ZDoubleArrayList::length() const
{
	return (int)parent::size();
}

int
ZDoubleArrayList::count( const int& i ) const
{
	return (int)parent::at(i).size();
}

int
ZDoubleArrayList::totalCount() const
{
	int cnt = 0;

	const int N = length();
	FOR( i, 0, N )
	{
		cnt += parent::at(i).length();
	}

	return cnt;
}

int
ZDoubleArrayList::maxCount() const
{
	const int m = (int)parent::size();

	int maxCnt = 0;

	FOR( i, 0, m )
	{
		maxCnt = ZMax( maxCnt, count(i) );
	}

	return maxCnt;
}

void
ZDoubleArrayList::append( const ZDoubleArray& array )
{
	parent::push_back( array );
}

void
ZDoubleArrayList::write( ofstream& fout ) const
{
	const int m = length();
	fout.write( (char*)&m, sizeof(int) );

	FOR( i, 0, m )
	{
		const ZDoubleArray& array = parent::at(i);
		array.write( fout, true );
	}
}

void
ZDoubleArrayList::read( ifstream& fin )
{
	int m = 0;
	fin.read( (char*)&m, sizeof(int) );
	if( !m ) { parent::clear(); }

	setSize( m );

	FOR( i, 0, m )
	{
		ZDoubleArray& array = parent::at(i);
		array.read( fin, true );
	}
}

bool
ZDoubleArrayList::save( const char* filePathName ) const
{
	ofstream fout( filePathName, ios::out|ios::binary|ios::trunc );

	if( fout.fail() || !fout.is_open() )
	{
		cout << "Error@ZDoubleArrayList::save(): Failed to save file: " << filePathName << endl;
		return false;
	}

	write( fout );
	fout.close();
	return true;
}

bool
ZDoubleArrayList::load( const char* filePathName )
{
	ifstream fin( filePathName, ios::in|ios::binary );

	if( fin.fail() )
	{
		cout << "Error@ZDoubleArrayList::load(): Failed to load file." << endl;
		reset();
		return false;
	}

	read( fin );
	fin.close();
	return true;
}

ostream&
operator<<( ostream& os, const ZDoubleArrayList& object )
{
	os << "<ZDoubleArrayList>" << endl;

	FOR( i, 0, object.size() )
	{
		const ZDoubleArray& a = object[i];

		FOR( j, 0, a.length() )
		{
			os << a[j] << " ";
		}

		os << endl;
	}

	os << endl;

	return os;
}

ZELOS_NAMESPACE_END

