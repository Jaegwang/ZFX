//-------------//
// ZSphere.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2018.03.08                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZSphere::ZSphere()
{
	ZSphere::reset();
}

ZSphere::ZSphere( const ZSphere& sphere )
{
	*this = sphere;
}

ZSphere::ZSphere( const ZPoint& center, const float& radius )
{
	ZSphere::set( center, radius );
}

void
ZSphere::reset()
{
	_center.zeroize();
	_radius = 0.f;
	_r2     = 0.f;
}

ZSphere&
ZSphere::set( const ZPoint& center, const float& radius )
{
	_center = center;
	_radius = ZAbs( radius );
	_r2     = _radius * _radius;

	return (*this);
}

ZSphere&
ZSphere::set( const ZTriangle& triangle )
{
	_center = triangle.center();
	_radius = _center.distanceTo( triangle.a() );
}

ZSphere&
ZSphere::operator=( const ZSphere& sphere )
{
	_center = sphere._center;
	_radius = sphere._radius;
	_r2     = sphere._r2;

	return (*this);
}

const ZPoint&
ZSphere::center() const
{
	return _center;
}

const float&
ZSphere::radius() const
{
	return _radius;
}

const float&
ZSphere::squaredRadius() const
{
	return _r2;
}

bool
ZSphere::contains( const ZPoint& point ) const
{
	if( _center.squaredDistanceTo( point ) < _r2 )
	{
		return true;
	}

	return false;
}

// www.ambrsoft.com/TrigoCalc/Sphere/SpherLineIntersection_.htm
bool
ZSphere::intersects( const ZLine& line ) const
{
	const ZPoint& a = line.a();
	const ZPoint& b = line.b();
	const ZPoint& c = _center;

	const float A = a.squaredDistanceTo( b );
	const float B = 2.f * ( (b.x-a.x)*(a.x-c.x) + (b.y-a.y)*(a.y-c.y) + (b.z-a.z)*(a.z-c.z) );
	const float C = c.squaredLength() + a.squaredLength() - 2.f * ( c.x*a.x + c.y*a.y + c.z*a.z ) - _r2;

	// const float t0 = ( -B + sqrtf(B*B-4*A*C) ) / (2*A);
	// const float t1 = ( -B - sqrtf(B*B-4*A*C) ) / (2*A);
	// intersectionPoint[0] = a + ( b - a ) * t0
	// intersectionPoint[1] = a + ( b - a ) * t1

	if( ( B*B - 4.f*A*C ) < 0.f )
	{
		return false;
	}

	return true;
}

bool
ZSphere::intersects( const ZPoint& p0, const ZPoint& p1 ) const
{
	const ZPoint& a = p0;
	const ZPoint& b = p1;
	const ZPoint& c = _center;

	const float A = a.squaredDistanceTo( b );
	const float B = 2.f * ( (b.x-a.x)*(a.x-c.x) + (b.y-a.y)*(a.y-c.y) + (b.z-a.z)*(a.z-c.z) );
	const float C = c.squaredLength() + a.squaredLength() - 2.f * ( c.x*a.x + c.y*a.y + c.z*a.z ) - _r2;

	// const float t0 = ( -B + sqrtf(B*B-4*A*C) ) / (2*A);
	// const float t1 = ( -B - sqrtf(B*B-4*A*C) ) / (2*A);
	// intersectionPoint[0] = a + ( b - a ) * t0
	// intersectionPoint[1] = a + ( b - a ) * t1

	if( ( B*B - 4.f*A*C ) < 0.f )
	{
		return false;
	}

	return true;
}

void
ZSphere::draw() const
{
	// TO DO: draw the sphere
}

void
ZSphere::write( ofstream& fout ) const
{
	_center.write( fout );
	fout.write( (char*)&_radius, sizeof(float) );
}

void
ZSphere::read( ifstream& fin )
{
	_center.read( fin );
	fin.read( (char*)&_radius, sizeof(float) );

	_r2 = _radius * _radius;
}

ostream&
operator<<( ostream& os, const ZSphere& object )
{
	os << "<ZSphere>" << endl;
	os << " center: " << object.center() << endl;
	os << " radius: " << object.radius() << endl;
	return os;
}

ZELOS_NAMESPACE_END

