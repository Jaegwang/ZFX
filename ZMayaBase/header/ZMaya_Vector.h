//----------------//
// ZMaya_Vector.h //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2016.01.12                               //
//-------------------------------------------------------//

#ifndef _ZMaya_Vector_h_
#define _ZMaya_Vector_h_

#include <ZelosBaseForMaya.h>

ZELOS_NAMESPACE_BEGIN

MVector Rotate( const MVector& v, const MVector& unitAxis, double angleInRadians );

inline MVector
AsMVector( const ZVector& v )
{
	return MPoint( v.x, v.y, v.z );
}

ZELOS_NAMESPACE_END

#endif

