//-------------------//
// ZIntArrayList.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2016.07.04                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZIntArrayList::ZIntArrayList()
{
}

ZIntArrayList::ZIntArrayList( const ZIntArrayList& a )
: vector<ZIntArray>(a)
{
}

ZIntArrayList::ZIntArrayList( int m, int n )
: vector<ZIntArray>(m)
{
	FOR( i, 0, m )
	{
		parent::at(i).resize(n);
	}
}

ZIntArrayList::ZIntArrayList( const char* filePathName )
: vector<ZIntArray>()
{
	load( filePathName );
}

ZIntArrayList&
ZIntArrayList::operator=( const ZIntArrayList& other )
{
	parent::operator=( other );
	return (*this);
}

void
ZIntArrayList::reset()
{
	parent::clear();
}

void
ZIntArrayList::setSize( int m, int n )
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

			ZIntArray tmp( n );

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
ZIntArrayList::fill( int valueForAll )
{
	const int n = (int)parent::size();

	FOR( i, 0, n )
	{
		parent::at(i).fill( valueForAll );
	}
}

void
ZIntArrayList::setLength( int i, int n )
{
	if( i >= (int)parent::size() ) { return; }
	if( n <= 0 ) { parent::at(i).clear(); return; }
	parent::at(i).setLength( n );
}

int
ZIntArrayList::length() const
{
	return (int)parent::size();
}

int
ZIntArrayList::count( const int& i ) const
{
	return (int)parent::at(i).size();
}

int
ZIntArrayList::totalCount() const
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
ZIntArrayList::maxCount() const
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
ZIntArrayList::append( const ZIntArray& array )
{
	parent::push_back( array );
}

void
ZIntArrayList::write( ofstream& fout ) const
{
	const int m = length();
	fout.write( (char*)&m, sizeof(int) );

	FOR( i, 0, m )
	{
		const ZIntArray& array = parent::at(i);
		array.write( fout, true );
	}
}

void
ZIntArrayList::read( ifstream& fin )
{
	int m = 0;
	fin.read( (char*)&m, sizeof(int) );
	if( !m ) { parent::clear(); }

	setSize( m );

	FOR( i, 0, m )
	{
		ZIntArray& array = parent::at(i);
		array.read( fin, true );
	}
}

bool
ZIntArrayList::save( const char* filePathName ) const
{
	ofstream fout( filePathName, ios::out|ios::binary|ios::trunc );

	if( fout.fail() || !fout.is_open() )
	{
		cout << "Error@ZIntArrayList::save(): Failed to save file: " << filePathName << endl;
		return false;
	}

	write( fout );
	fout.close();
	return true;
}

bool
ZIntArrayList::load( const char* filePathName )
{
	ifstream fin( filePathName, ios::in|ios::binary );

	if( fin.fail() )
	{
		cout << "Error@ZIntArrayList::load(): Failed to load file." << endl;
		reset();
		return false;
	}

	read( fin );
	fin.close();
	return true;
}

ostream&
operator<<( ostream& os, const ZIntArrayList& object )
{
	os << "<ZIntArrayList>" << endl;

	FOR( i, 0, object.size() )
	{
		const ZIntArray& a = object[i];

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

