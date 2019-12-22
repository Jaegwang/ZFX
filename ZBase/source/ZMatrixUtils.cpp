//------------------//
// ZMatrixUtils.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2016.06.20                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZMatrix
ShapeMatchingMatrix( const ZPointArray& source, const ZPointArray& target )
{
	ZMatrix xForm;

	const int N = source.length();
	if( N != target.length() ) { return xForm; }

	if( N == 1 )
	{
		xForm.setToIdentity();
		xForm.setTranslation( target[0] - source[0] );
		return xForm;
	}

	ZPointArray q( source );
	ZPointArray p( target );

	const ZPoint qC( q.center() );
	const ZPoint pC( p.center() );

	FOR( i, 0, N )
	{
		q[i] -= qC;
		p[i] -= pC;
	}

	ZMatrix pq, qq, nm; // nm: normal matrix

	{
		pq.setToOuterProduct( p[0], q[0] );
		qq.setToOuterProduct( q[0], q[0] );
	}

	FOR( i, 1, N )
	{
		nm.setToOuterProduct( p[i], q[i] );
		pq += nm;

		nm.setToOuterProduct( q[i], q[i] );
		qq += nm;
	}

	xForm = pq * qq.inverse();
	xForm.setTranslation( pC - qC );

	return xForm;
}

ZMatrix
ShapeMatchingMatrix( const ZPointArray& source, const ZPointArray& target, const ZPoint& sourcePivotPosition )
{
	ZMatrix xForm;

	const int N = source.length();
	if( N != target.length() ) { return xForm; }

	if( N == 1 )
	{
		xForm.setToIdentity();
		xForm.setTranslation( target[0] - source[0] );
		return xForm;
	}

	ZPointArray q( source );
	ZPointArray p( target );

	const ZPoint qC( q.center() );
	const ZPoint pC( p.center() );

	FOR( i, 0, N )
	{
		q[i] -= qC;
		p[i] -= pC;
	}

	ZMatrix pq, qq, nm; // nm: normal matrix

	{
		pq.setToOuterProduct( p[0], q[0] );
		qq.setToOuterProduct( q[0], q[0] );
	}

	FOR( i, 1, N )
	{
		nm.setToOuterProduct( p[i], q[i] );
		pq += nm;

		nm.setToOuterProduct( q[i], q[i] );
		qq += nm;
	}

	xForm = pq * qq.inverse();

	ZVector dir0( sourcePivotPosition - qC );
	ZVector dir1( xForm.transform( dir0, true ) );
	xForm.setTranslation( pC + dir1 );

	return xForm;
}

ZELOS_NAMESPACE_END

