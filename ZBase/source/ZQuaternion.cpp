//-----------------//
// ZQuaternion.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2018.10.23                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZQuaternion&
ZQuaternion::fromRotationMatrix( const ZMatrix& R )
{
	// verify the given matrix is really a rotation
	// but, we omit this process for efficiency, i.e. we assume the given matrix is a rotation matrix.
	//if( !R.isUnitary() ) { ... }

    float Rx=0, Ry=0, Rz=0;
    R.getRotation( Rx, Ry, Rz, false );

    ZQuaternion::fromEulerAngle( Rx, Ry, Rz );

    return (*this);
}

ZQuaternion&
ZQuaternion::fromAxisAngle( const ZVector& axis, double angle )
{
	const double len = axis.length();

	if( len < Z_EPS )
    {
		w = 1.0;
		x = y = z = 0.0;
	}
    else
    {
		const double s = sin( 0.5*angle );

		w = cos( 0.5 * angle );
		x = s * ( axis.x / len );
		y = s * ( axis.y / len );
		z = s * ( axis.z / len );
	}

	return (*this);
}

ZQuaternion&
ZQuaternion::fromEulerAngle( const double& Rx, const double& Ry, const double& Rz )
{
	const double t0 = cos( Rz * 0.5 );
	const double t1 = sin( Rz * 0.5 );
	const double t2 = cos( Rx * 0.5 );
	const double t3 = sin( Rx * 0.5 );
	const double t4 = cos( Ry * 0.5 );
	const double t5 = sin( Ry * 0.5 );

	w = t0*t2*t4 + t1*t3*t5;
	x = t0*t3*t4 - t1*t2*t5;
	y = t0*t2*t5 + t1*t3*t4;
	z = t1*t2*t4 - t0*t3*t5;

	return (*this);
}

void
ZQuaternion::toRotationMatrix( ZMatrix& R ) const
{
	const double xx = 2*x*x, xy = 2*x*y, xw = 2*x*w;
	const double yy = 2*y*y, yz = 2*y*z, yw = 2*y*w;
	const double zz = 2*z*z, zx = 2*z*x, zw = 2*z*w;

	R._00 = 1-(yy+zz);   R._01 = xy-zw;       R._02 = zx+yw;
	R._10 = xy+zw;       R._11 = 1-(xx+zz);   R._12 = yz-xw;
	R._20 = zx-yw;       R._21 = yz+xw;       R._22 = 1-(xx+yy);
}

void
ZQuaternion::toAxisAngle( ZVector& unitAxis, double& angle ) const
{
    angle = 2 * acos(w);
    const double d = 1 / ( sqrt(1-w*w) + Z_EPS );
    unitAxis.set( x*d, y*d, z*d );
}

void
ZQuaternion::toEulerAngle( double& Rx, double& Ry, double& Rz ) const
//{
//	const double xx = x*x, yy = y*y, zz = z*z;
//
//	Rx = atan2( 2*(x*w+y*z)+1e-20, 1-2*(xx+yy) );
//	Ry = asin( ZClamp( 2*(y*w-x*z), -1.0, 1.0 ) );
//	Rz = atan2( 2*(z*w+x*y)+1e-20, 1-2*(yy+zz) );
//}
{
    const double Nq = w*w + x*x + y*y + z*z;
    const double s = ( Nq > 0 ) ? (2/Nq) : 0;

    const double xs = x*s,  ys = y*s,  zs = z*s;
    const double wx = w*xs, wy = w*ys, wz = w*zs;
    const double xx = x*xs, xy = x*ys, xz = x*zs;
    const double yy = y*ys, yz = y*zs, zz = z*zs;

    const double m00 = 1-(yy+zz), m01 = xy-wz,     m02 = xz+wy;
    const double m10 = xy+wz,     m11 = 1-(xx+zz), m12 = yz-wx;
    const double m20 = xz-wy,     m21 = yz+wx,     m22 = 1-(xx+yy);

    const double cy = sqrt( m00*m00 + m10*m10 );

    if( cy > Z_EPS )
    {
        Rx = atan2(  m21, m22 );
        Ry = atan2( -m20, cy  );
        Rz = atan2(  m10, m00 );
    }
    else
    {
        Rx = atan2( -m12, m11 );
        Ry = atan2( -m20, cy  );
        Rz = 0;
    } 
}

ZELOS_NAMESPACE_END

