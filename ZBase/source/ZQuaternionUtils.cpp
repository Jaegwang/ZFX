//----------------------//
// ZQuaternionUtils.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZQuaternion
Slerp( const ZQuaternion& q1, const ZQuaternion& q2, float t, float tol )
{
	const float qdot = q1.dot(q2);

	float angle=0, sineAngle=0;

	if( ZAbs(qdot) < 1 ) {
		angle     = acosf(qdot);
		sineAngle = sinf(angle);
	}

	// Denominator close to 0 corresponds to the case where the two quaternions are close to the same rotation.
	// In this case linear interpolation is used but we normalize to guarantee unit length.
	if( sineAngle <= tol ) {

		const float s = 1-t;

		ZQuaternion qTmp( s*q1.x+t*q2.x, s*q1.y+t*q2.y, s*q1.z+t*q2.z, s*q1.w+t*q2.w );

		// Check the case where two close to antipodal quaternions were blended resulting in a nearly zero result which can happen,
		// for example, if t is close to 0.5. In this case it is not safe to project back onto the sphere.
		const float lenSq = qTmp.squaredLength();

		if( lenSq <= ZPow2(tol) ) {
			qTmp = (t<0.5f) ? q1 : q2;
		} else {
			qTmp *= 1.f / sqrtf(lenSq);
		}

        return qTmp;

    } else {

		const float sine  = 1.f / sineAngle;
		const float a = sinf((1-t)*angle) * sine;
		const float b = sinf(t*angle) * sine;

		return ZQuaternion( a*q1.x+b*q2.x, a*q1.y+b*q2.y, a*q1.z+b*q2.z, a*q1.w+b*q2.w );
	}
}

ZMatrix
Slerp( const ZMatrix& m1, const ZMatrix& m2, float t )
{
	ZQuaternion q1(m1), q2(m2);

	if( q1.dot(q2) < 0 ) { q2 *= -1; }

	ZQuaternion qslerp( Slerp( q1, q2, t ) );

	ZMatrix m;
	qslerp.toRotationMatrix( m );

    return m;
}

ZMatrix
BezLerp( const ZMatrix& m1, const ZMatrix& m2, const ZMatrix& m3, const ZMatrix& m4, float t )
{
	ZMatrix m00( Slerp( m1, m2, t ) );
	ZMatrix m01( Slerp( m2, m3, t ) );
	ZMatrix m02( Slerp( m3, m4, t ) );

	ZMatrix m10( Slerp( m00, m01, t) );
	ZMatrix m11( Slerp( m01, m02, t) );

	return Slerp( m10, m11, t );
}

ZELOS_NAMESPACE_END

