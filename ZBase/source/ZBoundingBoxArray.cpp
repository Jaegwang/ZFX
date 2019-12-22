//-----------------------//
// ZBoundingBoxArray.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZBoundingBoxArray::ZBoundingBoxArray()
{}

ZBoundingBoxArray::ZBoundingBoxArray( const ZBoundingBoxArray& a )
: ZArray<ZBoundingBox>::ZArray( a )
{}

ZBoundingBoxArray::ZBoundingBoxArray( int l )
: ZArray<ZBoundingBox>::ZArray( l )
{}

ZBoundingBox
ZBoundingBoxArray::boundingBox() const
{
	ZBoundingBox bBox;

	const int N = parent::length();

	FOR( i, 0, N )
	{
		bBox.expand( this->at(i) );
	}

	return bBox;
}

ostream&
operator<<( ostream& os, const ZBoundingBoxArray& object )
{
	os << "<ZBoundingBoxArray>" << endl;
	const int size     = object.size();
	const int capacity = object.capacity();

	double bytes = size * sizeof(ZBoundingBox);
	os << " size    : " << size << " (";
	     if( bytes <       1024.0  ) { os << ( bytes                 ) << " bytes)" << endl; }
	else if( bytes < ZPow2(1024.0) ) { os << ( bytes /       1024.0  ) << " kb)"    << endl; }
	else if( bytes < ZPow3(1024.0) ) { os << ( bytes / ZPow2(1024.0) ) << " mb)"    << endl; }
	else if( bytes < ZPow4(1024.0) ) { os << ( bytes / ZPow3(1024.0) ) << " gb)"    << endl; }

	bytes = capacity * sizeof(ZBoundingBox);
	os << " capacity: " << size << " (";
	     if( bytes <       1024.0  ) { os << ( bytes                 ) << " bytes)" << endl; }
	else if( bytes < ZPow2(1024.0) ) { os << ( bytes /       1024.0  ) << " kb)"    << endl; }
	else if( bytes < ZPow3(1024.0) ) { os << ( bytes / ZPow2(1024.0) ) << " mb)"    << endl; }
	else if( bytes < ZPow4(1024.0) ) { os << ( bytes / ZPow3(1024.0) ) << " gb)"    << endl; }

	os << endl;
	return os;
}

ZELOS_NAMESPACE_END

