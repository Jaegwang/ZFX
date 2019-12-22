//----------//
// ZRay.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2018.03.08                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZRay::ZRay()
{
	ZRay::reset();
}

ZRay::ZRay( const ZRay& ray )
{
	*this = ray;
}

ZRay::ZRay( const ZPoint& origin, const ZVector& direction, const float& min, const float& max )
{
	ZRay::set( origin, direction, min, max );
}

void
ZRay::reset()
{
	_origin.zeroize();
	_direction.zeroize();
	_min = 0.f;
	_max = Z_FLTMAX;
}

ZRay&
ZRay::set( const ZPoint& origin, const ZVector& direction, const float& min, const float& max )
{
	_origin    = origin;
	_direction = direction;
	_min       = min;
	_max       = max;

	return (*this);
}

ZRay&
ZRay::operator=( const ZRay& ray )
{
	_origin    = ray._origin;
	_direction = ray._direction;
	_min       = ray._min;
	_max       = ray._max;

	return (*this);
}

void
ZRay::getPoint( const float& t, ZPoint& p ) const
{
	p.x = _origin.x + t * _direction.x;
	p.y = _origin.y + t * _direction.y;
	p.z = _origin.z + t * _direction.z;
}

ZPoint
ZRay::point( const float& t ) const
{
	ZPoint p;

	p.x = _origin.x + t * _direction.x;
	p.y = _origin.y + t * _direction.y;
	p.z = _origin.z + t * _direction.z;

	return p;
}

const ZPoint&
ZRay::origin() const
{
	return _origin;
}

const ZVector&
ZRay::direction() const
{
	return _direction;
}

const float&
ZRay::min() const
{
	return _min;
}

const float&
ZRay::max() const
{
	return _max;
}

void
ZRay::draw() const
{
	glBegin( GL_LINES );
	{
		glVertex( _origin + _min * _direction );
		glVertex( _origin + _max * _direction );
	}
	glEnd();
}

void
ZRay::write( ofstream& fout ) const
{
	fout.write( (char*)&_origin,    sizeof(ZPoint)  );
	fout.write( (char*)&_direction, sizeof(ZVector) );
	fout.write( (char*)&_min,       sizeof(float)   );
	fout.write( (char*)&_max,       sizeof(float)   );
}

void
ZRay::read( ifstream& fin )
{
	fin.read( (char*)&_origin,    sizeof(ZPoint)  );
	fin.read( (char*)&_direction, sizeof(ZVector) );
	fin.read( (char*)&_min,       sizeof(float)   );
	fin.read( (char*)&_max,       sizeof(float)   );
}

ostream&
operator<<( ostream& os, const ZRay& object )
{
	os << "<ZRay>" << endl;
	os << " origin       : " << object.origin()    << endl;
	os << " direction    : " << object.direction() << endl;
	os << " min. distance: " << object.min()       << endl;
	os << " max. distance: " << object.max()       << endl;
	return os;
}

ZELOS_NAMESPACE_END

