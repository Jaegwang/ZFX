//--------------//
// ZGlUtils.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2017.04.13                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

void
ZResetDisplayList( int& dispListId )
{
	if( dispListId < 0 ) { return; }

	glDeleteLists( dispListId, 1 );

	dispListId = -1;
}

void
ZCallDisplayList( const int& dispListId )
{
	if( dispListId < 0 ) { return; }

	glCallList( dispListId );
}

void
ZDrawCube( const ZPoint& minPt, const ZPoint& maxPt )
{
	glBegin( GL_LINES );
	{
		// bottom
		glVertex(minPt.x,minPt.y,minPt.z); glVertex(maxPt.x,minPt.y,minPt.z);
		glVertex(maxPt.x,minPt.y,minPt.z); glVertex(maxPt.x,minPt.y,maxPt.z);
		glVertex(maxPt.x,minPt.y,maxPt.z); glVertex(minPt.x,minPt.y,maxPt.z);
		glVertex(minPt.x,minPt.y,maxPt.z); glVertex(minPt.x,minPt.y,minPt.z);

		// top
		glVertex(minPt.x,maxPt.y,minPt.z); glVertex(maxPt.x,maxPt.y,minPt.z);
		glVertex(maxPt.x,maxPt.y,minPt.z); glVertex(maxPt.x,maxPt.y,maxPt.z);
		glVertex(maxPt.x,maxPt.y,maxPt.z); glVertex(minPt.x,maxPt.y,maxPt.z);
		glVertex(minPt.x,maxPt.y,maxPt.z); glVertex(minPt.x,maxPt.y,minPt.z);

		// vertical lines
		glVertex(minPt.x,minPt.y,minPt.z); glVertex(minPt.x,maxPt.y,minPt.z);
		glVertex(maxPt.x,minPt.y,minPt.z); glVertex(maxPt.x,maxPt.y,minPt.z);
		glVertex(maxPt.x,minPt.y,maxPt.z); glVertex(maxPt.x,maxPt.y,maxPt.z);
		glVertex(minPt.x,minPt.y,maxPt.z); glVertex(minPt.x,maxPt.y,maxPt.z);
	}
	glEnd();
}

void
ZDrawCircle( float radius, int numSlices, int numLoops )
{
	GLUquadricObj* obj = gluNewQuadric();
	gluQuadricOrientation( obj, GLU_INSIDE );
	gluDisk( obj, 0.f, radius, numSlices, numLoops );
	gluDeleteQuadric( obj ); 
}

void
ZDrawCircleOnXYPlane( const ZPoint& center, float radius, int numSegments, bool asWireFrame )
{
	float coeff = Z_PIx2 / (float)numSegments;
	float x=0.f, y=0.f;

	if( asWireFrame ) {

		glBegin( GL_LINE_LOOP );
		{
			FOR( i, 0, numSegments )
			{
				x = center.x + radius * cosf( i * coeff );
				y = center.y + radius * sinf( i * coeff );

				glVertex( x, y );
			}
		}
		glEnd();

	} else {

		glBegin( GL_TRIANGLE_FAN );
		{
			FOR( i, 0, numSegments )
			{
				x = center.x + radius * cosf( i * coeff );
				y = center.y + radius * sinf( i * coeff );

				glVertex( x, y );
			}
		}
		glEnd();

	}
}

void
ZDrawCircleOnYZPlane( const ZPoint& center, float radius, int numSegments, bool asWireFrame )
{
	float coeff = Z_PIx2 / (float)numSegments;
	float y=0.f, z=0.f;

	if( asWireFrame ) {

		glBegin( GL_LINE_LOOP );
		{
			FOR( i, 0, numSegments )
			{
				y = center.y + radius * cosf( i * coeff );
				z = center.z + radius * sinf( i * coeff );

				glVertex( 0.f, y, z );
			}
		}
		glEnd();

	} else {

		glBegin( GL_TRIANGLE_FAN );
		{
			FOR( i, 0, numSegments )
			{
				y = center.y + radius * cosf( i * coeff );
				z = center.z + radius * sinf( i * coeff );

				glVertex( 0.f, y, z );
			}
		}
		glEnd();
	}
}

void
ZDrawCircleOnZXPlane( const ZPoint& center, float radius, int numSegments, bool asWireFrame )
{
	float coeff = Z_PIx2 / (float)numSegments;
	float x=0.f, z=0.f;

	if( asWireFrame ) {

		glBegin( GL_LINE_LOOP );
		{
			FOR( i, 0, numSegments )
			{
				x = center.x + radius * cosf( i * coeff );
				z = center.z + radius * sinf( i * coeff );

				glVertex( x, 0.f, z );
			}
		}
		glEnd();

	} else {

		glBegin( GL_TRIANGLE_FAN );
		{
			FOR( i, 0, numSegments )
			{
				x = center.x + radius * cosf( i * coeff );
				z = center.z + radius * sinf( i * coeff );

				glVertex( x, 0.f, z );
			}
		}
		glEnd();

	}
}

void
ZDrawArcLineOnXYPlane( const ZPoint& center, float radius, float startAngle, float endAngle, int numSegs )
{
	if( numSegs < 1 ) { return; }

	float x=0.f, y=0.f;
	float theta = 0.f;
	float delta = ( endAngle - startAngle ) / (float)numSegs;

	glBegin( GL_LINE_STRIP );
	{
		FOR( i, 0, numSegs )
		{
			x = center.x + radius * cosf( i*delta + startAngle );
			y = center.y + radius * sinf( i*delta + startAngle );

			glVertex( x, y, 0.f );
		}
	}
	glEnd();

	glBegin( GL_LINES );
	{
		glVertex( x, y, 0.f );

		x = center.x + radius * cosf( endAngle );
		y = center.y + radius * sinf( endAngle );

		glVertex( x, y, 0.f );
	}
	glEnd();
}

void
ZDrawArcLineOnYZPlane( const ZPoint& center, float radius, float startAngle, float endAngle, int numSegs )
{
	if( numSegs < 1 ) { return; }

	float y=0.f, z=0.f;
	float theta = 0.f;
	float delta = ( endAngle - startAngle ) / (float)numSegs;

	glBegin( GL_LINE_STRIP );
	{
		FOR( i, 0, numSegs )
		{
			y = center.y + radius * sinf( i*delta + startAngle );
			z = center.z + radius * cosf( i*delta + startAngle );

			glVertex( 0.f, y, z );
		}
	}
	glEnd();

	glBegin( GL_LINES );
	{
		glVertex( 0.f, y, z );

		y = center.y + radius * sinf( endAngle );
		z = center.z + radius * cosf( endAngle );

		glVertex( 0.f, y, z );
	}
	glEnd();
}

void
ZDrawArcLineOnZXPlane( const ZPoint& center, float radius, float startAngle, float endAngle, int numSegs )
{
	if( numSegs < 1 ) { return; }

	float z=0.f, x=0.f;
	float theta = 0.f;
	float delta = ( endAngle - startAngle ) / (float)numSegs;

	glBegin( GL_LINE_STRIP );
	{
		FOR( i, 0, numSegs )
		{
			z = center.z + radius * cosf( i*delta + startAngle );
			x = center.x + radius * sinf( i*delta + startAngle );

			glVertex( x, 0.f, z );
		}
	}
	glEnd();

	glBegin( GL_LINES );
	{
		glVertex( x, 0.f, z );

		z = center.z + radius * cosf( endAngle );
		x = center.x + radius * sinf( endAngle );

		glVertex( x, 0.f, z );
	}
	glEnd();
}

void
ZDrawSphere( const float& radius, const int& numSlices, const int& numStacks )
{
	GLUquadricObj* obj = gluNewQuadric();
	gluSphere( obj, radius, numSlices, numStacks );
	gluDeleteQuadric( obj );
}

ZPoint
ZCurrentCameraPosition()
{
	float m[16];
	glGetFloatv( GL_MODELVIEW_MATRIX, m );

	return ZPoint( -(m[0]*m[12] + m[1]*m[13] + m[ 2]*m[14]),
				   -(m[4]*m[12] + m[5]*m[13] + m[ 6]*m[14]),
				   -(m[8]*m[12] + m[9]*m[13] + m[10]*m[14]) );
}

void
ZMultiplyMatrixByVector( float dst[4], float mat[16], float src[3] )
{
    dst[0] = mat[0] * src[0] + mat[4] * src[1] + mat[8 ] * src[2] + mat[12] * src[3];
    dst[1] = mat[1] * src[0] + mat[5] * src[1] + mat[9 ] * src[2] + mat[13] * src[3];
    dst[2] = mat[2] * src[0] + mat[6] * src[1] + mat[10] * src[2] + mat[14] * src[3];
    dst[3] = mat[3] * src[0] + mat[7] * src[1] + mat[11] * src[2] + mat[15] * src[3];
}

ZELOS_NAMESPACE_END

