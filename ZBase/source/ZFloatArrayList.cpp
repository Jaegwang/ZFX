//---------------------//
// ZFloatArrayList.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2016.07.04                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZFloatArrayList::ZFloatArrayList()
{
}

ZFloatArrayList::ZFloatArrayList( const ZFloatArrayList& a )
: vector<ZFloatArray>(a)
{
}

ZFloatArrayList::ZFloatArrayList( int m, int n )
: vector<ZFloatArray>(m)
{
	FOR( i, 0, m )
	{
		parent::at(i).resize(n);
	}
}

ZFloatArrayList::ZFloatArrayList( const char* filePathName )
: vector<ZFloatArray>()
{
	load( filePathName );
}

ZFloatArrayList&
ZFloatArrayList::operator=( const ZFloatArrayList& other )
{
	parent::operator=( other );
	return (*this);
}

void
ZFloatArrayList::reset()
{
	parent::clear();
}

void
ZFloatArrayList::setSize( int m, int n )
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

			ZFloatArray tmp( n );

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
ZFloatArrayList::fill( float valueForAll )
{
	const int n = (int)parent::size();

	FOR( i, 0, n )
	{
		parent::at(i).fill( valueForAll );
	}
}

void
ZFloatArrayList::setLength( int i, int n )
{
	if( i >= (int)parent::size() ) { return; }
	if( n <= 0 ) { parent::at(i).clear(); return; }
	parent::at(i).setLength( n );
}

int
ZFloatArrayList::length() const
{
	return (int)parent::size();
}

int
ZFloatArrayList::count( const int& i ) const
{
	return (int)parent::at(i).size();
}

int
ZFloatArrayList::totalCount() const
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
ZFloatArrayList::maxCount() const
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
ZFloatArrayList::append( const ZFloatArray& array )
{
	parent::push_back( array );
}

void
ZFloatArrayList::write( ofstream& fout ) const
{
	const int m = length();
	fout.write( (char*)&m, sizeof(int) );

	FOR( i, 0, m )
	{
		const ZFloatArray& array = parent::at(i);
		array.write( fout, true );
	}
}

void
ZFloatArrayList::read( ifstream& fin )
{
	int m = 0;
	fin.read( (char*)&m, sizeof(int) );
	if( !m ) { parent::clear(); }

	setSize( m );

	FOR( i, 0, m )
	{
		ZFloatArray& array = parent::at(i);
		array.read( fin, true );
	}
}

bool
ZFloatArrayList::save( const char* filePathName ) const
{
	ofstream fout( filePathName, ios::out|ios::binary|ios::trunc );

	if( fout.fail() || !fout.is_open() )
	{
		cout << "Error@ZFloatArrayList::save(): Failed to save file: " << filePathName << endl;
		return false;
	}

	write( fout );
	fout.close();
	return true;
}

bool
ZFloatArrayList::load( const char* filePathName )
{
	ifstream fin( filePathName, ios::in|ios::binary );

	if( fin.fail() )
	{
		cout << "Error@ZFloatArrayList::load(): Failed to load file." << endl;
		reset();
		return false;
	}

	read( fin );
	fin.close();
	return true;
}

ostream&
operator<<( ostream& os, const ZFloatArrayList& object )
{
	os << "<ZFloatArrayList>" << endl;

	FOR( i, 0, object.size() )
	{
		const ZFloatArray& a = object[i];

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

