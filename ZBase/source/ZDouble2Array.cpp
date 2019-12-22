//-------------------//
// ZDouble2Array.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2016.09.30                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZDouble2Array::ZDouble2Array()
{}

ZDouble2Array::ZDouble2Array( const ZDouble2Array& a )
: ZArray<ZDouble2>::ZArray( a )
{}

ZDouble2Array::ZDouble2Array( int l )
: ZArray<ZDouble2>::ZArray( l )
{}

ostream&
operator<<( ostream& os, const ZDouble2Array& object )
{
	os << "<ZDouble2Array>" << endl;
	const int size     = object.size();
	const int capacity = object.capacity();

	double bytes = size * sizeof(ZDouble2);
	os << " size    : " << size << " (";
	     if( bytes <       1024.0  ) { os << ( bytes                 ) << " bytes)" << endl; }
	else if( bytes < ZPow2(1024.0) ) { os << ( bytes /       1024.0  ) << " kb)"    << endl; }
	else if( bytes < ZPow3(1024.0) ) { os << ( bytes / ZPow2(1024.0) ) << " mb)"    << endl; }
	else if( bytes < ZPow4(1024.0) ) { os << ( bytes / ZPow3(1024.0) ) << " gb)"    << endl; }

	bytes = capacity * sizeof(ZDouble2);
	os << " capacity: " << size << " (";
	     if( bytes <       1024.0  ) { os << ( bytes                 ) << " bytes)" << endl; }
	else if( bytes < ZPow2(1024.0) ) { os << ( bytes /       1024.0  ) << " kb)"    << endl; }
	else if( bytes < ZPow3(1024.0) ) { os << ( bytes / ZPow2(1024.0) ) << " mb)"    << endl; }
	else if( bytes < ZPow4(1024.0) ) { os << ( bytes / ZPow3(1024.0) ) << " gb)"    << endl; }

	os << endl;
	return os;
}

ZELOS_NAMESPACE_END

