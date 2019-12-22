//--------------//
// MGlUtils.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#include <ZelosBaseForMaya.h>

ZELOS_NAMESPACE_BEGIN

void
glDrawCube( const MPoint& minPt, const MPoint& maxPt )
{
	glBegin( GL_LINES );

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

	glEnd();

}

ZELOS_NAMESPACE_END

