//-------------------//
// ZPseudoSpring.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2018.03.08                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZPseudoSpring::ZPseudoSpring()
: k(0.1f), d(0.1f), a(1.f), dt(0.1f)
{
	f.zeroize();
	_p.zeroize();
	_p0.zeroize();
	_v.zeroize();
}

ZPseudoSpring::ZPseudoSpring( const ZPseudoSpring& spring )
: k(0.1f), d(0.1f), a(1.f), dt(0.1f)
{
	*this = spring;
}

ZPseudoSpring::ZPseudoSpring( const ZPoint& initialPosition )
: k(0.1f), d(0.1f), a(1.f), dt(0.1f)
{
	f.zeroize();
	_p = _p0 = initialPosition;
	_v.zeroize();
}

ZPseudoSpring&
ZPseudoSpring::operator=( const ZPseudoSpring& spring )
{
	k   = spring.k;
	d   = spring.d;
	a   = spring.a;
	dt  = spring.dt;
	f   = spring.f;

	_p0 = spring._p0;
	_p  = spring._p;
	_v  = spring._v;

	return (*this);
}

ZPoint
ZPseudoSpring::position() const
{
	return _p;
}

ZVector
ZPseudoSpring::velocity() const
{
	return _v;
}

void
ZPseudoSpring::reInitialize( const ZPoint& goalPosition )
{
	_p0 = _p = goalPosition;
	_v.zeroize();
}

ZPoint
ZPseudoSpring::update( const ZPoint& g )
{
	float r = 1.f - d;

	// update valocity with external force
	_v.x = ( ( _v.x + ( ( g.x - _p.x ) * k ) ) * r ) + ( f.x * dt );
	_v.y = ( ( _v.y + ( ( g.y - _p.y ) * k ) ) * r ) + ( f.y * dt );
	_v.z = ( ( _v.z + ( ( g.z - _p.z ) * k ) ) * r ) + ( f.z * dt );

	// update position
	_p.x += _v.x * dt;
	_p.y += _v.y * dt;
	_p.z += _v.z * dt;

	// limit displacement from the goal position
	{
		float dx = _p.x - g.x;
		float dy = _p.y - g.y;
		float dz = _p.z - g.z;

		r = dx*dx + dy*dy + dz*dz;

		if( r > a )
		{
			r = a / r;

			_p.x = g.x + ( dx * r );
			_p.y = g.y + ( dy * r );
			_p.z = g.z + ( dz * r );
		}
	}

	// PBD like velocity update
	_v.x = ( _p.x - _p0.x ) / dt;
	_v.y = ( _p.y - _p0.y ) / dt;
	_v.z = ( _p.z - _p0.z ) / dt;

	// backup position
	_p0.x = _p.x;
	_p0.y = _p.y;
	_p0.z = _p.z;

	return _p;
}

ostream&
operator<<( ostream& os, const ZPseudoSpring& spring )
{
	os << "<ZPseudoSpring>" << endl;
	os << "stiffness     : " << spring.k  << endl;
	os << "damping       : " << spring.d  << endl;
	os << "amplitude     : " << spring.a  << endl;
	os << "time step size: " << spring.dt << endl;

	os << endl;

	return os;
}

ZELOS_NAMESPACE_END

