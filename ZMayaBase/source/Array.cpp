//-----------//
// Array.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#include <ZelosBaseForMaya.h>

ZELOS_NAMESPACE_BEGIN

MPoint
Center( MPointArray& verts )
{
	MPoint c;

	unsigned int numVerts = verts.length();
	double d = 1 / (double)numVerts;

	for( unsigned int i=0; i<numVerts; ++i )
	{
		c.x += verts[i].x * d;
		c.y += verts[i].y * d;
		c.z += verts[i].z * d;
	}

	return c;
}

MVector
Center( MVectorArray& verts )
{
	MVector c;

	unsigned int numVerts = verts.length();
	double d = 1 / (double)numVerts;

	for( unsigned int i=0; i<numVerts; ++i )
	{
		c.x += verts[i].x * d;
		c.y += verts[i].y * d;
		c.z += verts[i].z * d;
	}

	return c;
}

void
GetMinMaxPoints( MFloatPointArray& pos, MFloatPoint& minPt, MFloatPoint& maxPt )
{
	minPt.x = minPt.y = minPt.z =  (float)Z_LARGE;
	maxPt.x = maxPt.y = maxPt.z = -(float)Z_LARGE;

	MFloatPoint p;
	unsigned int N = pos.length();

	for( unsigned int i=0; i<N; ++i )
	{
		p = pos[i];

		minPt.x = ZMin( minPt.x, p.x );
		minPt.y = ZMin( minPt.y, p.y );
		minPt.z = ZMin( minPt.z, p.z );

		maxPt.x = ZMax( maxPt.x, p.x );
		maxPt.y = ZMax( maxPt.y, p.y );
		maxPt.z = ZMax( maxPt.z, p.z );
	}
}

void
GetMinMaxPoints( MPointArray& pos, MPoint& minPt, MPoint& maxPt )
{
	minPt.x = minPt.y = minPt.z =  (double)Z_LARGE;
	maxPt.x = maxPt.y = maxPt.z = -(double)Z_LARGE;

	MPoint p;
	unsigned int N = pos.length();

	for( unsigned int i=0; i<N; ++i )
	{
		p = pos[i];

		minPt.x = ZMin( minPt.x, p.x );
		minPt.y = ZMin( minPt.y, p.y );
		minPt.z = ZMin( minPt.z, p.z );

		maxPt.x = ZMax( maxPt.x, p.x );
		maxPt.y = ZMax( maxPt.y, p.y );
		maxPt.z = ZMax( maxPt.z, p.z );
	}
}

ZELOS_NAMESPACE_END

