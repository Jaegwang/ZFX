//---------------//
// ZTriangle.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2018.03.08                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZTriangle::ZTriangle()
{
	ZTriangle::reset();
}

ZTriangle::ZTriangle( const ZTriangle& triangle )
{
	*this = triangle;
}

ZTriangle::ZTriangle( const ZPoint& a, const ZPoint& b, const ZPoint& c )
{
	ZTriangle::set( a, b, c );
}

void
ZTriangle::reset()
{
	_a.zeroize();
	_b.zeroize();
	_c.zeroize();
}

ZTriangle&
ZTriangle::set( const ZPoint& a, const ZPoint& b, const ZPoint& c )
{
	_a = a;
	_b = b;
	_c = c;

	return (*this);
}

ZTriangle&
ZTriangle::operator=( const ZTriangle& triangle )
{
	_a = triangle._a;
	_b = triangle._b;
	_c = triangle._c;

	return (*this);
}

const ZPoint&
ZTriangle::a() const
{
	return _a;
}

const ZPoint&
ZTriangle::b() const
{
	return _b;
}

const ZPoint&
ZTriangle::c() const
{
	return _c;
}

ZPoint
ZTriangle::center() const
{
	return Center( _a, _b, _c );
}

float
ZTriangle::area() const
{
	return Area( _a, _b, _c );
}

void
ZTriangle::draw( bool shaded ) const
{
	if( shaded )
	{
		glBegin( GL_TRIANGLES );
		{
			glVertex( _a );
			glVertex( _b );
			glVertex( _c );
		}
		glEnd();
	}
	else
	{
		glBegin( GL_LINE_LOOP );
		{
			glVertex( _a );
			glVertex( _b );
			glVertex( _c );
		}
		glEnd();
	}
}

void
ZTriangle::write( ofstream& fout ) const
{
	_a.write( fout );
	_b.write( fout );
	_c.write( fout );
}

void
ZTriangle::read( ifstream& fin )
{
	_a.read( fin );
	_b.read( fin );
	_c.read( fin );
}

ostream&
operator<<( ostream& os, const ZTriangle& object )
{
	os << "<ZTriangle>" << endl;
	return os;
}

ZELOS_NAMESPACE_END

