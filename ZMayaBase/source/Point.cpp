//-----------//
// Point.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2016.01.12                               //
//-------------------------------------------------------//

#include <ZelosBaseForMaya.h>

ZELOS_NAMESPACE_BEGIN

void
SetValidPoint( MPoint& p )
{
	p.w = 1;
}

void
SetValidPoint( MFloatPoint& p )
{
	p.w = 1;
}

void
SetValidPoints( MPointArray& p )
{
	uint num = p.length();
	for( uint i=0; i<num; ++i )
	{
		p[i].w = 1;
	}
}

void
SetValidPoints( MFloatPointArray& p )
{
	uint num = p.length();
	for( uint i=0; i<num; ++i )
	{
		p[i].w = 1;
	}
}

MPoint
Rotate( const MPoint& p, const MVector& unitAxis, double angleInRadians, const MPoint& pivot )
{
	MPoint P( p - pivot );

	const double c = cos( angleInRadians );
	const double s = sin( angleInRadians );

	const MVector cross( unitAxis ^ P );
	const double dot = unitAxis * P;

	P.x = c*P.x + s*cross.x + (1-c)*dot*unitAxis.x + pivot.x;
	P.y = c*P.y + s*cross.y + (1-c)*dot*unitAxis.y + pivot.y;
	P.z = c*P.z + s*cross.z + (1-c)*dot*unitAxis.z + pivot.z;

	return P;
}

ZELOS_NAMESPACE_END

