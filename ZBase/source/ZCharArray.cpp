//----------------//
// ZCharArray.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2018.03.08                               //
//-------------------------------------------------------//

#include <ZelosBase.h>
ZELOS_NAMESPACE_BEGIN

ZCharArray::ZCharArray()
{}

ZCharArray::ZCharArray( const ZCharArray& a )
: ZArray<char>::ZArray( a )
{}

ZCharArray::ZCharArray( int l )
: ZArray<char>::ZArray( l )
{}

ZCharArray::ZCharArray( int l, int v )
: ZArray<char>::ZArray( l, v )
{}

void
ZCharArray::setMask( const ZCharArray& indices, bool value )
{
	const int len0 = (int)parent::size();
	const int len1 = indices.length();

	if( value ) { // mark true

		parent::zeroize();

		FOR( i, 0, len1 )
		{
			const int& idx = indices[i];
			if( idx < 0 || idx >= len0 ) { continue; }

			parent::at(idx) = 1;
		}

	} else { // mark false

		parent::fill( 1 );

		FOR( i, 0, len1 )
		{
			const int& idx = indices[i];
			if( idx < 0 || idx >= len0 ) { continue; }

			parent::at(idx) = 0;
		}

	}
}

ostream&
operator<<( ostream& os, const ZCharArray& object )
{
	os << "<ZCharArray>" << endl;
	const int size     = object.size();
	const int capacity = object.capacity();

	double bytes = size * sizeof(int);
	os << " size    : " << size << " (";
	     if( bytes <       1024.0  ) { os << ( bytes                 ) << " bytes)" << endl; }
	else if( bytes < ZPow2(1024.0) ) { os << ( bytes /       1024.0  ) << " kb)"    << endl; }
	else if( bytes < ZPow3(1024.0) ) { os << ( bytes / ZPow2(1024.0) ) << " mb)"    << endl; }
	else if( bytes < ZPow4(1024.0) ) { os << ( bytes / ZPow3(1024.0) ) << " gb)"    << endl; }

	bytes = capacity * sizeof(int);
	os << " capacity: " << size << " (";
	     if( bytes <       1024.0  ) { os << ( bytes                 ) << " bytes)" << endl; }
	else if( bytes < ZPow2(1024.0) ) { os << ( bytes /       1024.0  ) << " kb)"    << endl; }
	else if( bytes < ZPow3(1024.0) ) { os << ( bytes / ZPow2(1024.0) ) << " mb)"    << endl; }
	else if( bytes < ZPow4(1024.0) ) { os << ( bytes / ZPow3(1024.0) ) << " gb)"    << endl; }

	os << endl;
	return os;
}

ZELOS_NAMESPACE_END

