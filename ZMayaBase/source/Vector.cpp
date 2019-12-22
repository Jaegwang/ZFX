//------------//
// Vector.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2016.01.12                               //
//-------------------------------------------------------//

#include <ZelosBaseForMaya.h>

ZELOS_NAMESPACE_BEGIN

MVector
Rotate( const MVector& v, const MVector& unitAxis, double angleInRadians )
{
	MVector q;

	const double c = cos( angleInRadians );
	const double s = sin( angleInRadians );

	const MVector cross( unitAxis ^ v );
	const double dot = unitAxis * v;

	q.x = c*v.x + s*cross.x + (1-c)*dot*unitAxis.x;
	q.y = c*v.y + s*cross.y + (1-c)*dot*unitAxis.y;
	q.z = c*v.z + s*cross.z + (1-c)*dot*unitAxis.z;

	return q;
}

ZELOS_NAMESPACE_END

