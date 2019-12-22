//---------------//
// ZMaya_Light.h //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2016.12.28                               //
//-------------------------------------------------------//

#ifndef _ZMaya_Light_h_
#define _ZMaya_Light_h_

#include <ZelosBaseForMaya.h>

ZELOS_NAMESPACE_BEGIN

// Do not move to Light.cpp.
static void
SetLightAtCamera()
{
	MDagPath camDagPath;
	M3dView::active3dView().getCamera( camDagPath );
	MMatrix camXForm = camDagPath.inclusiveMatrix();
	const GLfloat lightPosition[] = { (float)camXForm[3][0], (float)camXForm[3][1], (float)camXForm[3][2], 1.0f };

	float ambientProperties[]  = { 0.5f, 0.5f, 0.5f, 1.0f };
	float diffuseProperties[]  = { 0.5f, 0.5f, 0.5f, 1.0f };
	float specularProperties[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	glLightfv( GL_LIGHT0, GL_AMBIENT,  ambientProperties  );
	glLightfv( GL_LIGHT0, GL_DIFFUSE,  diffuseProperties  );
	glLightfv( GL_LIGHT0, GL_SPECULAR, specularProperties );
	glLightfv( GL_LIGHT0, GL_POSITION, lightPosition      );
	glLightModelf( GL_LIGHT_MODEL_TWO_SIDE, 1.0 );
	glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE );

	float ambient[4] = { 0.2f, 0.2f, 0.2f, 1.0 };
	float diffuse[4] = { 0.7f, 0.7f, 0.7f, 1.0 };
	float specular[4] = { 0.8f, 0.8f, 0.8f, 1.0 };
	float shininess = 32.f;

	glLightfv( GL_LIGHT0, GL_POSITION, lightPosition );
	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );
	glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, ambient );
	glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse );
	glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, specular );
	glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, shininess );

	glEnable( GL_LIGHT0 );
	glEnable( GL_NORMALIZE );
	glEnable( GL_COLOR_MATERIAL );
	glEnable( GL_LIGHTING );
}

// Do not move to Light.cpp.
static void
SetLightAtCurrentCamera( const MMatrix& invXForm )
{
	MDagPath camDagPath;
	M3dView::active3dView().getCamera( camDagPath );
	MMatrix camXForm = camDagPath.inclusiveMatrix() * invXForm;

	const GLfloat lightPosition[] = { (float)camXForm[3][0], (float)camXForm[3][1], (float)camXForm[3][2], 1.0f };

	float ambientProperties[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
	float diffuseProperties[]  = { 0.2f, 0.2f, 0.2f, 1.0f };
	float specularProperties[] = { 0.9f, 0.9f, 0.9f, 1.0f };

	glLightfv( GL_LIGHT0, GL_AMBIENT,  ambientProperties  );
	glLightfv( GL_LIGHT0, GL_DIFFUSE,  diffuseProperties  );
	glLightfv( GL_LIGHT0, GL_SPECULAR, specularProperties );
	glLightfv( GL_LIGHT0, GL_POSITION, lightPosition      );
	glLightModelf( GL_LIGHT_MODEL_TWO_SIDE, 1.0 );
	glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE );

	float ambient[4] = { 0.0f, 0.0f, 0.0f, 1.0 };
	float diffuse[4] = { 0.2f, 0.2f, 0.2f, 1.0 };
	float specular[4] = { 0.2f, 0.2f, 0.2f, 1.0 };
	float shininess = 4.f;

	glLightfv( GL_LIGHT0, GL_POSITION, lightPosition );
	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );
	glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, ambient );
	glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse );
	glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, specular );
	glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, shininess );

	glEnable( GL_LIGHT0 );
	glEnable( GL_NORMALIZE );
	glEnable( GL_COLOR_MATERIAL );
	glEnable( GL_LIGHTING );
}

ZELOS_NAMESPACE_END

#endif

