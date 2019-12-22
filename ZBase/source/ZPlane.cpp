//------------//
// ZPlane.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2018.03.08                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZPlane::ZPlane()
{
	ZPlane::reset();
}

ZPlane::ZPlane( const ZPlane& plane )
{
	*this = plane;
}

ZPlane::ZPlane( const float& a, const float& b, const float& c, const float& d  )
{
	ZPlane::set( a, b, c, d );
}

ZPlane::ZPlane( const ZPoint& p0, const ZPoint& p1, const ZPoint& p2 )
{
	ZPlane::set( p0, p1, p2 );
}

ZPlane::ZPlane( const ZPoint& p, const ZVector& unitNormal )
{
	ZPlane:set( p, unitNormal );
}

void
ZPlane::reset()
{
	_normal.zeroize();
	_d = 0.f;
}

ZPlane&
ZPlane::set( const float& a, const float& b, const float& c, const float& d  )
{
	_normal.set( a, b, c );

	const float _len = 1.f / _normal.length();

	_normal.x = a * _len;
	_normal.y = b * _len;
	_normal.z = c * _len;
	_d        = d * _len;

	return (*this);
}

ZPlane&
ZPlane::set( const ZPoint& p0, const ZPoint& p1, const ZPoint& p2 )
{
	_normal = Normal( p0, p1, p2 );
	_d = -( ( _normal.x * p0.x ) + ( _normal.y * p0.y ) + ( _normal.z * p0.z ) );

	return (*this);
}

ZPlane&
ZPlane::set( const ZPoint& p, const ZVector& unitNormal )
{
	_normal = unitNormal;
	_d = -( ( _normal.x * p.x ) + ( _normal.y * p.y ) + ( _normal.z * p.z ) );

	return (*this);
}

ZPlane&
ZPlane::operator=( const ZPlane& plane )
{
	_normal = plane._normal;
	_d      = plane._d;

	return (*this);
}

void
ZPlane::reverse()
{
	_normal.reverse();
}

const float&
ZPlane::a() const
{
	return _normal.x;
}

const float&
ZPlane::b() const
{
	return _normal.y;
}

const float&
ZPlane::c() const
{
	return _normal.z;
}

const float&
ZPlane::d() const
{
	return _d;
}

void
ZPlane::getCoefficients( float& a, float& b, float& c, float& d ) const
{
	a = _normal.x;
	b = _normal.y;
	c = _normal.z;
	d = _d;
}

const ZVector&
ZPlane::normal() const
{
	return _normal;
}

bool 
ZPlane::isOnThePlane( const ZPoint& p, const float& tolerance ) const
{
	return ( ZAlmostZero( (_normal*p)+_d, tolerance ) );
}

bool
ZPlane::isOutside( const ZPoint& p, const float& tolerance ) const
{
	return ( ( ZPlane::signedDistanceFrom(p) > -tolerance ) ? true : false );
}

bool
ZPlane::isInside( const ZPoint& p, const float& tolerance ) const
{
	return ( ( ZPlane::signedDistanceFrom(p) < tolerance ) ? true : false );
}

float
ZPlane::signedDistanceFrom( const ZPoint& p ) const
{
	return ( ( _normal * p ) + _d );
}

float
ZPlane::distanceFrom( const ZPoint& p ) const
{
	return ZAbs( ( _normal * p ) + _d );
}

float
ZPlane::signedDistanceFromOrigin() const
{
	return _d;
}

float
ZPlane::distanceFromOrigin() const
{
	return ZAbs(_d);
}

ZPoint
ZPlane::closestPoint( const ZPoint& p ) const
{
	const ZPoint&  P = p;
	const ZVector& N = _normal;

	const float& a = N.x;
	const float& b = N.y;
	const float& c = N.z;
	const float& d = _d;

	ZPoint O; // a point on the plane
	{
		if( !ZAlmostZero(a) )
		{
			O.set( -d/a, 0.f, 0.f );
		}
		else if( !ZAlmostZero(b) )
		{
			O.set( 0.f, -d/b, 0.f );
		}
		else if( !ZAlmostZero(c) )
		{
			O.set( 0.f, 0.f, -d/c );
		}
		else
		{
			return ZPoint( Z_LARGE );
		}
	}

	return ClosestPointOnPlane( P, O, N );
}

void
ZPlane::draw() const
{
	// TO DO: draw the plane
}

void
ZPlane::write( ofstream& fout ) const
{
	fout.write( (char*)&_normal, sizeof(ZVector) );
	fout.write( (char*)&_d,      sizeof(float)   );
}

void
ZPlane::read( ifstream& fin )
{
	fin.read( (char*)&_normal, sizeof(ZVector) );
	fin.read( (char*)&_d,      sizeof(float)   );
}

ostream&
operator<<( ostream& os, const ZPlane& object )
{

	os << "<ZPlane>" << endl;
	os << " normal  : " << object.normal() << endl;
	os << " distance: " << object.distanceFromOrigin() << endl;
	return os;
}

ZELOS_NAMESPACE_END

