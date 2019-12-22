//-----------------------//
// ZMeshElementArray.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZMeshElementArray::ZMeshElementArray()
{}

ZMeshElementArray::ZMeshElementArray( const ZMeshElementArray& a )
: ZArray<ZMeshElement>::ZArray( a )
{}

ZMeshElementArray::ZMeshElementArray( int l )
: ZArray<ZMeshElement>::ZArray( l )
{}

ostream&
operator<<( ostream& os, const ZMeshElementArray& object )
{
	os << "<ZMeshElementArray>" << endl;
	os << endl;
	return os;
}

ZELOS_NAMESPACE_END

