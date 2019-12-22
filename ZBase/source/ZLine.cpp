//-----------//
// ZLine.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2018.03.08                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZLine::ZLine()
{
	ZLine::reset();
}

ZLine::ZLine( const ZLine& line )
{
	*this = line;
}

ZLine::ZLine( const ZPoint& a, const ZPoint& b )
{
	ZLine::set( a, b );
}

void
ZLine::reset()
{
	_a.zeroize();
	_b.zeroize();
}

ZLine&
ZLine::set( const ZPoint& a, const ZPoint& b )
{
	_a = a;
	_b = b;

	return (*this);
}

ZLine&
ZLine::operator=( const ZLine& line )
{
	_a = line._a;
	_b = line._b;

	return (*this);
}

const ZPoint&
ZLine::a() const
{
	return _a;
}

const ZPoint&
ZLine::b() const
{
	return _b;
}

ZPoint
ZLine::center() const
{
	return Center( _a, _b );
}

float
ZLine::length() const
{
	return _a.distanceTo( _b );
}

void
ZLine::draw() const
{
	glBegin( GL_LINES );
	{
		glVertex( _a );
		glVertex( _b );
	}
	glEnd();
}

void
ZLine::write( ofstream& fout ) const
{
	_a.write( fout );
	_b.write( fout );
}

void
ZLine::read( ifstream& fin )
{
	_a.read( fin );
	_b.read( fin );
}

ostream&
operator<<( ostream& os, const ZLine& object )
{
	os << "<ZLine>" << endl;
	return os;
}

ZELOS_NAMESPACE_END

