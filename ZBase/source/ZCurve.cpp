//------------//
// ZCurve.cpp //
//-------------------------------------------------------//
// author: Taeyong Kim @ Rhythm & Hues                   //
//         Wanho Choi @ Dexter Studios                   //
// last update: 2018.03.08                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZCurve::ZCurve()
{
}

ZCurve::ZCurve( ZCurve const& curve )
: ZPointArray( curve )
{
	(*this) = curve;
}

ZCurve&
ZCurve::operator=( const ZCurve& curve )
{
	ZPointArray::operator=( curve );
	return (*this);
}

void
ZCurve::reset()
{
	ZPointArray::reset();
}

void
ZCurve::setNumCVs( int nCVs )
{
	ZPointArray::setLength( nCVs );
}

void
ZCurve::addCV( const ZPoint& cv )
{
	ZPointArray::push_back( cv );
}

ZPoint
ZCurve::position( float t ) const
{
	int idx[4];
	_whereIsIt( t, idx );

	const ZPoint& P0 = ZPointArray::at( idx[0] );
	const ZPoint& P1 = ZPointArray::at( idx[1] );
	const ZPoint& P2 = ZPointArray::at( idx[2] );
	const ZPoint& P3 = ZPointArray::at( idx[3] );

	return _zeroDerivative( t, P0, P1, P2, P3 );
}

ZVector
ZCurve::tangent( float t ) const
{
	int idx[4];
	_whereIsIt( t, idx );

	const ZPoint& P0 = ZPointArray::at( idx[0] );
	const ZPoint& P1 = ZPointArray::at( idx[1] );
	const ZPoint& P2 = ZPointArray::at( idx[2] );
	const ZPoint& P3 = ZPointArray::at( idx[3] );

	return _firstDerivative( t, P0, P1, P2, P3 ).normalize();
}

ZVector
ZCurve::normal( float t ) const
{
	return ( biNormal(t) ^ tangent(t) ).normalize();
}

// old implementation
//{
//	int idx[4];
//	_whereIsIt( t, idx );
//
//	const ZPoint& P0 = ZPointArray::at( idx[0] );
//	const ZPoint& P1 = ZPointArray::at( idx[1] );
//	const ZPoint& P2 = ZPointArray::at( idx[2] );
//	const ZPoint& P3 = ZPointArray::at( idx[3] );
//
//	ZVector vel( _firstDerivative ( t, P0, P1, P2, P3 ) );
//	ZVector acc( _secondDerivative( t, P0, P1, P2, P3 ) );
//
//	const float vv = vel * vel;
//	const float va = vel * acc;
//
//	ZVector nrm( WeightedSum( vel, acc, -va, vv ) );
//
//	const float lenSQ = nrm.squaredLength();
//
//	if( ZAlmostZero(lenSQ) ) {
//
//		nrm.zeroize();
//
//		const ZVector a( P3 - P0 );
//
//		if( ZAbs(a.x ) < ZAbs(a.y) ) {
//			if( ZAbs(a.z ) < ZAbs(a.y) ) { nrm.z = ZSgn(a.y); }
//			else                         { nrm.x = ZSgn(a.z); }
//		} else {
//			if( ZAbs(a.z ) < ZAbs(a.x) ) { nrm.y = ZSgn(a.x); }
//			else                         { nrm.x = ZSgn(a.z); }
//		}
//
//	} else {
//
//		nrm *= 1 / sqrtf(lenSQ);
//
//	}
//
//	return nrm;
//}

ZVector
ZCurve::biNormal( float t ) const
{
	int idx[4];
	_whereIsIt( t, idx );

	const ZPoint& P0 = ZPointArray::at( idx[0] );
	const ZPoint& P1 = ZPointArray::at( idx[1] );
	const ZPoint& P2 = ZPointArray::at( idx[2] );
	const ZPoint& P3 = ZPointArray::at( idx[3] );

	ZVector vel( _firstDerivative ( t, P0, P1, P2, P3 ) );
	ZVector acc( _secondDerivative( t, P0, P1, P2, P3 ) );

	return ( vel ^ acc ).normalize();
}

// old implementation
//{
//	int idx[4];
//	_whereIsIt( t, idx );
//
//	const ZPoint& P0 = ZPointArray::at( idx[0] );
//	const ZPoint& P1 = ZPointArray::at( idx[1] );
//	const ZPoint& P2 = ZPointArray::at( idx[2] );
//	const ZPoint& P3 = ZPointArray::at( idx[3] );
//
//	ZVector vel( _firstDerivative ( t, P0, P1, P2, P3 ) );
//	ZVector acc( _secondDerivative( t, P0, P1, P2, P3 ) );
//
//	const float vv = vel * vel;
//	const float va = vel * acc;
//
//	ZVector nrm( WeightedSum( vel, acc, -va, vv ) );
//
//	const float lenSQ = nrm.squaredLength();
//
//	if( ZAlmostZero(lenSQ) ) {
//
//		nrm.zeroize();
//
//		const ZVector a( P3 - P0 );
//
//		if( ZAbs(a.x ) < ZAbs(a.y) ) {
//			if( ZAbs(a.z ) < ZAbs(a.y) ) { nrm.z = ZSgn(a.y); }
//			else                         { nrm.x = ZSgn(a.z); }
//		} else {
//			if( ZAbs(a.z ) < ZAbs(a.x) ) { nrm.y = ZSgn(a.x); }
//			else                         { nrm.x = ZSgn(a.z); }
//		}
//
//	} else {
//
//		nrm *= 1 / sqrtf(lenSQ);
//
//	}
//
//	return (vel^nrm).normalize();
//}

float
ZCurve::speed( float t ) const
{
	int idx[4];
	_whereIsIt( t, idx );

	const ZPoint& P0 = ZPointArray::at( idx[0] );
	const ZPoint& P1 = ZPointArray::at( idx[1] );
	const ZPoint& P2 = ZPointArray::at( idx[2] );
	const ZPoint& P3 = ZPointArray::at( idx[3] );

	return _firstDerivative( t, P0, P1, P2, P3 ).length();
}

void
ZCurve::getPositionAndTangent( int i, float t, ZPoint& P, ZVector& T ) const
{
	int idx[4];
	_whereIsIt( t, idx );

	const ZPoint& P0 = ZPointArray::at( idx[0] );
	const ZPoint& P1 = ZPointArray::at( idx[1] );
	const ZPoint& P2 = ZPointArray::at( idx[2] );
	const ZPoint& P3 = ZPointArray::at( idx[3] );

	P = _zeroDerivative( t, P0, P1, P2, P3 );
	T = _firstDerivative( t, P0, P1, P2, P3 ).normalize();
}

float
ZCurve::lineLength() const
{
	const int nCVs   = (int)ZPointArray::size();
	const int nCVs_1 = nCVs-1;

	int i0, i1;

	float totalLength = 0;

	for( int i0=0; i0<nCVs_1; ++i0 )
	{
		i1 = i0+1;

		const ZPoint& p0 = ZPointArray::at(i0);
		const ZPoint& p1 = ZPointArray::at(i1);

		totalLength += p0.distanceTo( p1 );
	}

	return totalLength;
}

// ref) Parallel Poisson Disk Sampling with Spectrum Analysis on Surfaces: eqn. (1)
float
ZCurve::curveLength() const
{
	const int nCVs   = (int)ZPointArray::size();
	const int nCVs_1 = nCVs-1;

	const float dt = 1 / (float)nCVs_1;

	ZVectorArray nrm( nCVs );
	for( int i=0; i<nCVs; ++i ) { nrm[i] = normal(i*dt); }

	float totalLength = 0;

	for( int i0=0; i0<nCVs_1; ++i0 )
	{
		const int i1 = i0+1;

		const ZPoint&  p0 = ZPointArray::at(i0);
		const ZPoint&  p1 = ZPointArray::at(i1);
		const ZVector& n0 = nrm[i0];
		const ZVector& n1 = nrm[i1];

		totalLength += DST( p0,p1, n0,n1 );
	}

	return totalLength;
}

float
ZCurve::curveLength( float t0, float t1 ) const
{
	t0 = ZClamp( t0, 0.f, 1.f );
	t1 = ZClamp( t1, 0.f, 1.f );

	if( t0 > t1 ) { ZSwap(t0,t1); }

	const int nCVs   = (int)ZPointArray::size();
	const int nCVs_1 = nCVs-1;

	float dt = 1 / (float)nCVs_1;

	int idx0 = (t0==0) ? 0      : ZCeil ( t0 / dt );
	int idx1 = (t1==1) ? nCVs_1 : ZFloor( t1 / dt );
	int diff = idx1 - idx0;

	ZVectorArray nrm( diff+1 );
	for( int i=0; i<=diff; ++i ) { nrm[i] = normal((i+idx0)*dt); }

	float totalLength = 0.f;

	// for the first segment
	{
		const ZPoint   p0( position(t0) );
		const ZPoint&  p1 = ZPointArray::at(idx0);
		const ZVector  n0( normal(t0) );
		const ZVector& n1 = nrm[0];
		totalLength += DST( p0,p1, n0,n1 );
	}

	// for the last segment
	{
		const ZPoint&  p0 = ZPointArray::at(idx1);
		const ZPoint   p1 = position(t1);
		const ZVector& n0 = nrm[diff];
		const ZVector  n1( normal(t1) );
		totalLength += DST( p0,p1, n0,n1 );
	}

	// for the other segments
	for( int i0=idx0, idx=idx0; i0<idx1; ++i0, ++idx )
	{
		const int i1 = i0+1;

		const ZPoint&  p0 = ZPointArray::at(i0);
		const ZPoint&  p1 = ZPointArray::at(i1);
		const ZVector& n0 = nrm[idx];
		const ZVector& n1 = nrm[idx+1];

		totalLength += DST( p0,p1, n0,n1 );
	}

	return totalLength;
}

ZELOS_NAMESPACE_END

