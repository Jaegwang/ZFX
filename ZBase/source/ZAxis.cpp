//-----------//
// ZAxis.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2018.03.08                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZAxis::ZAxis()
{
	reset();
}

ZAxis::ZAxis( const ZAxis& source )
{
	origin = source.origin;

	xAxis = source.xAxis;
	yAxis = source.yAxis;
	zAxis = source.zAxis;
}

void
ZAxis::reset()
{
	origin.zeroize();

	xAxis.set( 1.f, 0.f, 0.f );
	yAxis.set( 0.f, 1.f, 0.f );
	zAxis.set( 0.f, 0.f, 1.f );
}

ZAxis&
ZAxis::operator=( const ZAxis& axis )
{
	origin = axis.origin;

	xAxis = axis.xAxis;
	yAxis = axis.yAxis;
	zAxis = axis.zAxis;

	return (*this);
}

void
ZAxis::set( const ZAxis& a0, const ZAxis& a1, const ZAxis& a2, float w0, float w1, float w2, bool robust )
{
	origin = WeightedSum( a0.origin, a1.origin, a2.origin, w0, w1, w2 );
	xAxis  = WeightedSum( a0.xAxis,  a1.xAxis,  a2.xAxis,  w0, w1, w2 );
	yAxis  = WeightedSum( a0.yAxis,  a1.yAxis,  a2.yAxis,  w0, w1, w2 );
	zAxis  = WeightedSum( a0.zAxis,  a1.zAxis,  a2.zAxis,  w0, w1, w2 );

	if( robust ) {

		xAxis.robustNormalize();
		yAxis.robustNormalize();
		zAxis.robustNormalize();

	} else {

		xAxis.normalize( true );
		yAxis.normalize( true );
		zAxis.normalize( true );

	}

	// Gram-Schmidt orthonormalization
	xAxis = ( xAxis - (zAxis*(zAxis*xAxis)) ).normalize();
	yAxis = ( zAxis ^ xAxis ).normalize();
}

ZAxis&
ZAxis::robustNormalize()
{
	xAxis.robustNormalize();
	yAxis.robustNormalize();
	zAxis.robustNormalize();

	return (*this);
}

ZAxis&
ZAxis::normalize( bool accurate )
{
	xAxis.normalize( accurate );
	yAxis.normalize( accurate );
	zAxis.normalize( accurate );

	return (*this);
}

void
ZAxis::changeHandedness( const int& i )
{
	switch( i )
	{
		default:
		case 0: { xAxis.reverse(); break; }
		case 1: { yAxis.reverse(); break; }
		case 2: { zAxis.reverse(); break; }
	}
}

ZVector
ZAxis::worldToLocal( const ZVector& v, bool asVector, bool hasNormalizedAxes ) const
{
	if( hasNormalizedAxes ) { // Faster, but xAxis, yAxis, and zAxis must be unit vectors!

		if( asVector )
		{
			return ZVector( v*xAxis, v*yAxis, v*zAxis );
		}

		const ZVector V( v.x-origin.x, v.y-origin.y, v.z-origin.z );
		return ZVector( V*xAxis, V*yAxis, V*zAxis );

	} else {

		if( asVector )
		{
			return ZVector( v*xAxis.direction(), v*yAxis.direction(), v*zAxis.direction() );
		}

		const ZVector V( v.x-origin.x, v.y-origin.y, v.z-origin.z );
		return ZVector( V*xAxis.direction(), V*yAxis.direction(), V*zAxis.direction() );

	}
}

ZVector
ZAxis::localToWorld( const ZVector& v, bool asVector, bool hasNormalizedAxes ) const
{
	ZVector w;

	if( hasNormalizedAxes ) { // Faster, but xAxis, yAxis, and zAxis must be unit vectors!

		w = v.x*xAxis + v.y*yAxis + v.z*zAxis;

	} else {

		w = v.x*xAxis.direction() + v.y*yAxis.direction() + v.z*zAxis.direction();

	}

	if( !asVector ) { w += origin; }

	return w;
}

bool
ZAxis::align( const ZVector& aim, ZDirection::Direction whichAxis, bool accurate )
{
	float angle = 0;
	ZVector rotAxis;

	switch( whichAxis )
	{
		case ZDirection::xPositive:
		{
			rotAxis = xAxis ^ aim;
			angle = Angle( xAxis, aim );
			break;
		}

		case ZDirection::yPositive:
		{
			rotAxis = yAxis ^ aim;
			angle = Angle( yAxis, aim );
			break;
		}

		case ZDirection::zPositive:
		{
			rotAxis = zAxis ^ aim;
			angle = Angle( zAxis, aim );
			break;
		}

		default:
		{
			cout << "Error@ZAxis::align(): Invalid axis." << endl;
			return false;
		}
	}

	rotAxis.normalize();

	xAxis = Rotate( xAxis, rotAxis, angle );
	yAxis = Rotate( yAxis, rotAxis, angle );
	zAxis = Rotate( zAxis, rotAxis, angle );

	xAxis.normalize( accurate );
	yAxis.normalize( accurate );
	zAxis.normalize( accurate );

	return true;
}

void
ZAxis::draw( bool bySimpleLine ) const
{
	if( bySimpleLine ) {

		ZPoint X( origin + xAxis );
		ZPoint Y( origin + yAxis );
		ZPoint Z( origin + zAxis );

		glBegin( GL_LINES );
			glColorRed();   glVertex( origin ); glVertex( X );
			glColorGreen(); glVertex( origin ); glVertex( Y );
			glColorBlue();  glVertex( origin ); glVertex( Z );
		glEnd();

	} else {

		static GLUquadricObj *qObj = gluNewQuadric();

		glPushMatrix();

		glScalef( 0.2f, 0.2f, 0.2f );

		glColorRed();
		glPushMatrix();
		glRotatef( 90, 0, 1, 0 );
		glPushMatrix();
		glTranslatef( 0, 0, 5 );
		gluDisk( qObj, 0, 1, 8, 2 );
		glutSolidCone( 1, 2, 8, 3 );
		glPopMatrix();
		gluCylinder( qObj, 0.5, 0.5, 5, 8, 1 );
		glPopMatrix();

		glColorGreen();
		glPushMatrix();
		glRotatef( -90, 1, 0, 0 );
		glPushMatrix();
		glTranslatef( 0, 0, 5 );
		gluDisk( qObj, 0, 1, 8, 2 );
		glutSolidCone( 1, 2, 8, 3 );
		glPopMatrix();
		gluCylinder( qObj, 0.5, 0.5, 5, 8, 1 );
		glPopMatrix();

		glColorBlue();
		glPushMatrix();
		glTranslatef( 0, 0, 5 );
		gluDisk( qObj, 0, 1, 8, 2 );
		glutSolidCone( 1, 2, 8, 3 );
		glPopMatrix();
		gluCylinder( qObj, 0.5, 0.5, 5, 8, 1 );

		glColorYellow();
		glutSolidSphere( 1.0, 30, 30 );

		glPopMatrix();
	}
}

ostream&
operator<<( ostream& os, const ZAxis& object )
{
	os << "<ZAxis>" << endl;
	os << " origin: " << object.origin << endl;
	os << " x-axis: " << object.xAxis << endl;
	os << " y-axis: " << object.yAxis << endl;
	os << " z-axis: " << object.zAxis << endl;
	os << endl;

	return os;
}

ZELOS_NAMESPACE_END

