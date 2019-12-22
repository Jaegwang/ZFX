//----------//
// Copy.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2018.03.05                               //
//-------------------------------------------------------//

#include <ZelosBaseForMaya.h>

ZELOS_NAMESPACE_BEGIN

void
Copy( ZMatrix& to, const MMatrix& from, bool transposed )
{
	if( transposed ) { FOR(i,0,4) FOR(j,0,4) { to(i,j) = (float)from(j,i); } }
	else             { FOR(i,0,4) FOR(j,0,4) { to(i,j) = (float)from(i,j); } }
}

void
Copy( MMatrix& to, const ZMatrix& from, bool transposed )
{
	if( transposed ) { FOR(i,0,4) FOR(j,0,4) { to(i,j) = (double)from(j,i); } }
	else             { FOR(i,0,4) FOR(j,0,4) { to(i,j) = (double)from(i,j); } }
}

void
Copy( ZVectorArray& to, const MPointArray& from )
{
	const unsigned int n = from.length();
	if(!n) { to.clear(); return; }
	to.setLength(n);
	for( unsigned int i=0; i<n; ++i )
	{
		ZVector& q = to[i];
		const MPoint& p = from[i];
		q.x = (float)p.x;
		q.y = (float)p.y;
		q.z = (float)p.z;
	}
}

void
Copy( ZVectorArray& to, const MVectorArray& from )
{
	const unsigned int n = from.length();
	if(!n) { to.clear(); return; }
	to.setLength(n);
	for( unsigned int i=0; i<n; ++i )
	{
		ZVector& q = to[i];
		const MVector& p = from[i];
		q.x = (float)p.x;
		q.y = (float)p.y;
		q.z = (float)p.z;
	}
}

void
Copy( ZVectorArray& to, const MFloatPointArray& from )
{
	const unsigned int n = from.length();
	if(!n) { to.clear(); return; }
	to.setLength(n);
	for( unsigned int i=0; i<n; ++i )
	{
		ZVector& q = to[i];
		const MFloatPoint& p = from[i];
		q.x = p.x;
		q.y = p.y;
		q.z = p.z;
	}
}

void
Copy( ZVectorArray& to, const MFloatVectorArray& from )
{
	const unsigned int n = from.length();
	if(!n) { to.clear(); return; }
	to.setLength(n);
	for( unsigned int i=0; i<n; ++i )
	{
		ZVector& q = to[i];
		const MFloatVector& p = from[i];
		q.x = p.x;
		q.y = p.y;
		q.z = p.z;
	}
}

void
Copy( ZVectorArray& to, const MFnPointArrayData& from )
{
	const unsigned int n = from.length();
	if(!n) { to.clear(); return; }
	to.setLength(n);
	for( unsigned int i=0; i<n; ++i )
	{
		ZVector& q = to[i];
		q.x = (float)from[i].x;
		q.y = (float)from[i].y;
		q.z = (float)from[i].z;
	}
}

void
Copy( ZVectorArray& to, const MFnVectorArrayData& from )
{
	const unsigned int n = from.length();
	if(!n) { to.clear(); return; }
	to.setLength(n);
	for( unsigned int i=0; i<n; ++i )
	{
		ZVector& q = to[i];
		q.x = (float)from[i].x;
		q.y = (float)from[i].y;
		q.z = (float)from[i].z;
	}
}

void
Copy( MVectorArray& to, const MFnVectorArrayData& from )
{
	const unsigned int n = from.length();
	if(!n) { to.clear(); return; }
	to.setLength(n);
	for( unsigned int i=0; i<n; ++i )
	{
		MVector& q = to[i];
		q.x = (double)from[i].x;
		q.y = (double)from[i].y;
		q.z = (double)from[i].z;
	}
}

void
Copy( MPointArray& to, const ZVectorArray& from )
{
	const unsigned int n = from.length();
	if(!n) { to.clear(); return; }
	to.setLength(n);
	for( unsigned int i=0; i<n; ++i )
	{
		MPoint& q = to[i];
		const ZVector& p = from[i];
		q.x = (double)p.x;
		q.y = (double)p.y;
		q.z = (double)p.z;
		q.w = 1.0;
	}
}

void
Copy( MVectorArray& to, const ZVectorArray& from )
{
	const unsigned int n = from.length();
	if(!n) { to.clear(); return; }
	to.setLength(n);
	for( unsigned int i=0; i<n; ++i )
	{
		MVector& q = to[i];
		const ZVector& p = from[i];
		q.x = (double)p.x;
		q.y = (double)p.y;
		q.z = (double)p.z;
	}
}

void
Copy( MFloatPointArray& to, const ZVectorArray& from )
{
	const unsigned int n = from.length();
	if(!n) { to.clear(); return; }
	to.setLength(n);
	for( unsigned int i=0; i<n; ++i )
	{
		MFloatPoint& q = to[i];
		const ZVector& p = from[i];
		q.x = p.x;
		q.y = p.y;
		q.z = p.z;
		q.w = 1.f;
	}
}

void
Copy( MFloatVectorArray& to, const ZVectorArray& from )
{
	const unsigned int n = from.length();
	if(!n) { to.clear(); return; }
	to.setLength(n);
	for( unsigned int i=0; i<n; ++i )
	{
		MFloatVector& q = to[i];
		const ZVector& p = from[i];
		q.x = p.x;
		q.y = p.y;
		q.z = p.z;
	}
}

void
Copy( MFloatArray& to, const ZVectorArray& from )
{
	const unsigned int n = from.size();
	if(!n) { to.clear(); return; }
	to.setLength( 3*n );
	for( unsigned int i=0; i<n; ++i )
	{
		int idx = i*3;
		to[  idx] = from[i].x;
		to[++idx] = from[i].y;
		to[++idx] = from[i].z;
	}
}

void
Copy( MDoubleArray& to, const ZVectorArray& from )
{
	const unsigned int n = from.size();
	if(!n) { to.clear(); return; }
	to.setLength( 3*n );
	for( unsigned int i=0; i<n; ++i )
	{
		int idx = i*3;
		to[  idx] = (double)from[i].x;
		to[++idx] = (double)from[i].y;
		to[++idx] = (double)from[i].z;
	}
}

void
Copy( MFloatArray& to, const ZFloat2Array& from )
{
	const unsigned int n = from.length();
	if(!n) { to.clear(); return; }
	to.setLength(2*n);
	for( unsigned int i=0; i<n; ++i )
	{
		int idx = 2*i;
		to[  idx] = from[i][0];
		to[++idx] = from[i][1];
	}
}

void
Copy( MDoubleArray& to, const ZFloat2Array& from )
{
	const unsigned int n = from.length();
	if(!n) { to.clear(); return; }
	to.setLength(2*n);
	for( unsigned int i=0; i<n; ++i )
	{
		int idx = 2*i;
		to[  idx] = from[i][0];
		to[++idx] = from[i][1];
	}
}

void
Copy( MFloatArray& to, const ZFloat3Array& from )
{
	const unsigned int n = from.length();
	if(!n) { to.clear(); return; }
	to.setLength(3*n);
	for( unsigned int i=0; i<n; ++i )
	{
		int idx = 3*i;
		to[  idx] = from[i][0];
		to[++idx] = from[i][1];
		to[++idx] = from[i][2];
	}
}

void
Copy( MDoubleArray& to, const ZFloat3Array& from )
{
	const unsigned int n = from.length();
	if(!n) { to.clear(); return; }
	to.setLength(3*n);
	for( unsigned int i=0; i<n; ++i )
	{
		int idx = 3*i;
		to[  idx] = from[i][0];
		to[++idx] = from[i][1];
		to[++idx] = from[i][2];
	}
}

void
Copy( MIntArray& to, const ZIntArray& from )
{
	const unsigned int n = from.length();
	if(!n) { to.clear(); return; }
	to.setLength(n);
	for( unsigned int i=0; i<n; ++i )
	{
		to[i] = from[i];
	}
}

void
Copy( MIntArray& to, const MFnIntArrayData& from )
{
	const unsigned int n = from.length();
	if(!n) { to.clear(); return; }
	to.setLength(n);
	for( unsigned int i=0; i<n; ++i )
	{
		to[i] = from[i];
	}
}

void
Copy( ZIntArray& to, const MIntArray& from )
{
	const unsigned int n = from.length();
	if(!n) { to.clear(); return; }
	to.setLength(n);
	for( unsigned int i=0; i<n; ++i )
	{
		to[i] = from[i];
	}
}

void
Copy( ZIntArray& to, const MFnIntArrayData& from )
{
	const unsigned int n = from.length();
	if(!n) { to.clear(); return; }
	to.setLength(n);
	for( unsigned int i=0; i<n; ++i )
	{
		to[i] = from[i];
	}
}

void
Copy( ZFloatArray& to, const MFloatArray& from )
{
	const unsigned int n = from.length();
	if(!n) { to.clear(); return; }
	to.setLength(n);
	for( unsigned int i=0; i<n; ++i )
	{
		to[i] = from[i];
	}
}

void
Copy( ZFloatArray& to, const MDoubleArray& from )
{
	const unsigned int n = from.length();
	if(!n) { to.clear(); return; }
	to.setLength(n);
	for( unsigned int i=0; i<n; ++i )
	{
		to[i] = (float)from[i];
	}
}

void
Copy( ZFloatArray& to, const MFnFloatArrayData& from )
{
	const unsigned int n = from.length();
	if(!n) { to.clear(); return; }
	to.setLength(n);
	for( unsigned int i=0; i<n; ++i )
	{
		to[i] = from[i];
	}
}

void
Copy( ZFloatArray& to, const MFnDoubleArrayData& from )
{
	const unsigned int n = from.length();
	if(!n) { to.clear(); return; }
	to.setLength(n);
	for( unsigned int i=0; i<n; ++i )
	{
		to[i] = (float)from[i];
	}
}

void
Copy( MFloatArray& to, const ZFloatArray& from )
{
	const unsigned int n = from.length();
	if(!n) { to.clear(); return; }
	to.setLength(n);
	for( unsigned int i=0; i<n; ++i )
	{
		to[i] = from[i];
	}
}

void
Copy( MDoubleArray& to, const ZFloatArray& from )
{
	const unsigned int n = from.length();
	if(!n) { to.clear(); return; }
	to.setLength(n);
	for( unsigned int i=0; i<n; ++i )
	{
		to[i] = (double)from[i];
	}
}

void
Copy( ZDoubleArray& to, const MFloatArray& from )
{
	const unsigned int n = from.length();
	if(!n) { to.clear(); return; }
	to.setLength(n);
	for( unsigned int i=0; i<n; ++i )
	{
		to[i] = (double)from[i];
	}
}

void
Copy( ZDoubleArray& to, const MDoubleArray& from )
{
	const unsigned int n = from.length();
	if(!n) { to.clear(); return; }
	to.setLength(n);
	for( unsigned int i=0; i<n; ++i )
	{
		to[i] = from[i];
	}
}

void
Copy( ZDoubleArray& to, const MFnFloatArrayData& from )
{
	const unsigned int n = from.length();
	if(!n) { to.clear(); return; }
	to.setLength(n);
	for( unsigned int i=0; i<n; ++i )
	{
		to[i] = (double)from[i];
	}
}

void
Copy( ZDoubleArray& to, const MFnDoubleArrayData& from )
{
	const unsigned int n = from.length();
	if(!n) { to.clear(); return; }
	to.setLength(n);
	for( unsigned int i=0; i<n; ++i )
	{
		to[i] = from[i];
	}
}

void
Copy( MFloatArray& to, const ZDoubleArray& from )
{
	const unsigned int n = from.length();
	if(!n) { to.clear(); return; }
	to.setLength(n);
	for( unsigned int i=0; i<n; ++i )
	{
		to[i] = (float)from[i];
	}
}

void
Copy( MDoubleArray& to, const ZDoubleArray& from )
{
	const unsigned int n = from.length();
	if(!n) { to.clear(); return; }
	to.setLength(n);
	for( unsigned int i=0; i<n; ++i )
	{
		to[i] = from[i];
	}
}

void
Copy( MDoubleArray& to, const MFnDoubleArrayData& from )
{
	const unsigned int n = from.length();
	if(!n) { to.clear(); return; }
	to.setLength(n);
	for( unsigned int i=0; i<n; ++i )
	{
		to[i] = from[i];
	}
}

void
Copy( ZInt3Array& to, const MIntArray& from )
{
	const unsigned int n3=from.length();
	const unsigned int n=(int)(n3/3);
	if(!n) { to.clear(); return; }
	to.setLength(n);
	for( unsigned int idx=0, i=0; i<n; ++i )
	{
		ZInt3& q = to[i];
		q[0] = from[idx++];
		q[1] = from[idx++];
		q[2] = from[idx++];
	}
}

void
Copy( MIntArray& to, const ZInt3Array& from )
{
	const unsigned int n=from.length();
	if(!n) { to.clear(); return; }
	to.setLength( n*3 );
	for( unsigned int idx=0, i=0; i<n; ++i )
	{
		const ZInt3& p = from[i];
		to[idx++] = p[0];
		to[idx++] = p[1];
		to[idx++] = p[2];
	}
}

void
Copy( ZInt3Array& to, const MFnIntArrayData& from )
{
	const unsigned int n3=from.length();
	const unsigned int n=(int)(n3/3);
	if(!n) { to.clear(); return; }
	to.setLength(n);
	for( unsigned int idx=0, i=0; i<n; ++i )
	{
		ZInt3& q = to[i];
		q[0] = from[idx++];
		q[1] = from[idx++];
		q[2] = from[idx++];
	}
}

void
Copy( ZFloat2Array& uv, const MFloatArray& u, const MFloatArray& v )
{
	const unsigned int n=u.length();
	if(!n||n!=v.length()) { uv.clear(); return; }
	uv.setLength(n);
	for( unsigned int i=0; i<n; ++i )
	{
		uv[i][0] = u[i];
		uv[i][1] = v[i];
	}
}

void
Copy( MBoundingBox& to, const ZBoundingBox& from )
{
	to.clear();

	const ZPoint minPt( from.minPoint() );
	const ZPoint maxPt( from.maxPoint() );

	to.expand( MPoint( minPt.x, minPt.y, minPt.z ) );
	to.expand( MPoint( maxPt.x, maxPt.y, maxPt.z ) );
}

void
Copy( ZBoundingBox& to, const MBoundingBox& from )
{
	to.reset();

	const MFloatPoint minPt( from.min() );
	const MFloatPoint maxPt( from.max() );

	to.expand( ZPoint( minPt.x, minPt.y, minPt.z ) );
	to.expand( ZPoint( maxPt.x, maxPt.y, maxPt.z ) );
}

ZELOS_NAMESPACE_END

