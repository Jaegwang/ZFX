//---------------//
// ZMaya_Point.h //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2017.02.16                               //
//-------------------------------------------------------//

#ifndef _ZMaya_Point_h_
#define _ZMaya_Point_h_

#include <ZelosBaseForMaya.h>

ZELOS_NAMESPACE_BEGIN

void SetValidPoint( MPoint& p );

void SetValidPoint( MFloatPoint& p );

void SetValidPoints( MPointArray& p );

void SetValidPoints( MFloatPointArray& p );

MPoint Rotate( const MPoint& p, const MVector& unitAxis, double angleInRadians, const MPoint& pivot );

inline MPoint
AsMPoint( const ZVector& v )
{
	return MPoint( v.x, v.y, v.z );
}

inline MPoint
AsMPoint( float v )
{
	return MPoint( (double)v, (double)v, (double)v );
}

inline MPoint
AsMPoint( double v )
{
	return MPoint( v, v, v );
}

ZELOS_NAMESPACE_END

#endif

