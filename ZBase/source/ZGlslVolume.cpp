//-----------------//
// ZGlslVolume.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZGlslVolume::ZGlslVolume()
{
	_light.set(-120,60,-40);

	// control parameters
	_brightness = 1.0f;
	_densityFactor = 10.0f;
	_absorption = 10.0f;
	_numLightSamples = 32;
	_numSamples = 256;

	// draw light direction
	bDrawLightDir = false;

	_preprocessed = false;
}

void
ZGlslVolume::preprocess( const ZScalarField3D& density )
{
	glGetIntegerv( GL_VIEWPORT, _viewport );	// get viewport
	setupVolume3D( density );					// ZScalarField3D -> 3D texture
	setupFbo();
	setupProgram();
}

void
ZGlslVolume::postprocess()
{
	_preprocessed = false;
}

void
ZGlslVolume::setupVolume3D( const ZScalarField3D& density )
{
	_density = (ZScalarField3D*)&density;

	const int nx = density.nx();
	const int ny = density.ny();
	const int nz = density.nz();

	_volumeData.create( nx, ny, nz, GL_R32F, GL_RED, GL_FLOAT, GL_LINEAR, GL_CLAMP_TO_BORDER, (GLvoid*)density.pointer() );
}

void
ZGlslVolume::setupFbo()
{
	_fbo.create();
	_tex.create( _viewport[2], _viewport[3], GL_RGBA16F, GL_RGBA, GL_FLOAT,GL_LINEAR,GL_CLAMP_TO_EDGE );
	_rbo.create( _viewport[2], _viewport[3], GL_DEPTH_COMPONENT );
	_fbo.attach( _tex, GL_COLOR_ATTACHMENT0 );
	_fbo.attach( _rbo, GL_DEPTH_ATTACHMENT );
	_fbo.checkStatus();
}

void
ZGlslVolume::setupProgram()
{
	ZString path( "/home/rnd00/" );
	ZString vs, fs;
	vs.fromTextFile( ( path + "rayMarcherVS.glsl" ).asChar() );
	fs.fromTextFile( ( path + "rayMarcherFS.glsl" ).asChar() );
	_programRaymarcher.setShaders( vs.asChar(), fs.asChar() );
}

void
ZGlslVolume::toggleDrawLightDir()
{
	bDrawLightDir = bDrawLightDir ? false : true;
}

void
ZGlslVolume::draw()
{
	_eye = ZCurrentCameraPosition();
	//--------------------------------------------------------
	// if viewport changed, reset fbo
	GLint vp[4];
	glGetIntegerv(GL_VIEWPORT, (GLint*)vp);
	if(_viewport[2] != vp[2] || _viewport[3] != vp[3]) {
		for(int i=0; i<4; ++i) _viewport[i] = vp[i];
		setupFbo();
	}

	//--------------------------------------------------------
	// 1. render to fbo
	_fbo.bind();
	rayCasting();
	_fbo.unbind();


	//--------------------------------------------------------
	// 2. render texture to screen
	renderTexturedQuad();


	//--------------------------------------------------------
	// draw light direction
	if(bDrawLightDir) drawLightDir();


}

void ZGlslVolume::drawLightDir() {
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable ( GL_LIGHTING );
	glDisable( GL_DEPTH_TEST );

	glColor3f ( 1, 1, 0 );
	glBegin ( GL_LINES );
	glVertex ( _center );
	glVertex( _light );
	glEnd();

	glColor3f ( 0, 0, 1 );
	glBegin ( GL_LINES );
	glVertex ( _center );
	glVertex ( _eye );
	glEnd();

	glLineWidth ( 1.f );
	glPopAttrib();
}




void ZGlslVolume::renderTexturedQuad() {
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	_tex.bind();
	glDisable(GL_LIGHTING);
	glColor4f(1,1,1,1);

	glBegin(GL_QUADS);
	glTexCoord2f(0,0);
	glVertex3i(-1,-1,-1);
	glTexCoord2f(1,0);
	glVertex3i( 1,-1,-1);
	glTexCoord2f(1,1);
	glVertex3i( 1, 1,-1);
	glTexCoord2f(0,1);
	glVertex3i(-1, 1,-1);
	glEnd();

	glPopAttrib();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}


void ZGlslVolume::drawBox(float w, float h, float d) {
	float wm = w*0.5f, hm = h*0.5f, dm = d*0.5f;

	glBegin(GL_QUADS);
	glVertex3f(-wm, -hm, -dm);
	glVertex3f(-wm, hm, -dm);
	glVertex3f(wm, hm, -dm);
	glVertex3f(wm, -hm, -dm);

	glVertex3f(-wm, -hm, dm);
	glVertex3f(wm, -hm, dm);
	glVertex3f(wm, hm, dm);
	glVertex3f(-wm, hm, dm);

	glVertex3f(-wm, hm, -dm);
	glVertex3f(-wm, hm, dm);
	glVertex3f(wm, hm, dm);
	glVertex3f(wm, hm, -dm);

	glVertex3f(-wm, -hm, -dm);
	glVertex3f(wm, -hm, -dm);
	glVertex3f(wm, -hm, dm);
	glVertex3f(-wm, -hm, dm);

	glVertex3f(-wm, -hm, -dm);
	glVertex3f(-wm, -hm, dm);
	glVertex3f(-wm, hm, dm);
	glVertex3f(-wm, hm, -dm);

	glVertex3f(wm, -hm, -dm);
	glVertex3f(wm, hm, -dm);
	glVertex3f(wm, hm, dm);
	glVertex3f(wm, -hm, dm);
	glEnd();
}


void ZGlslVolume::rayCasting() {

	float axisMax = (float)ZMax(_density->nx(), _density->ny(), _density->nz());

	float boxMin[3],boxMax[3];
	boxMin[0] = (float)_density->nx()/axisMax * (-0.5f);
	boxMin[1] = (float)_density->ny()/axisMax * (-0.5f);
	boxMin[2] = (float)_density->nz()/axisMax * (-0.5f);
	boxMax[0] = (float)_density->nx()/axisMax * (0.5f);
	boxMax[1] = (float)_density->ny()/axisMax * (0.5f);
	boxMax[2] = (float)_density->nz()/axisMax * (0.5f);


	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	_programRaymarcher.enable();

	_programRaymarcher.bindTexture("volumeData",_volumeData.id(), GL_TEXTURE_3D, 0);
	_programRaymarcher.setUniform3f("boxMin", boxMin[0], boxMin[1], boxMin[2]);
	_programRaymarcher.setUniform3f("boxMax", boxMax[0], boxMax[1], boxMax[2]);
	_programRaymarcher.setUniform1f("brightness", _brightness);
	_programRaymarcher.setUniform1f("densityFactor", _densityFactor);
	_programRaymarcher.setUniform1f("absorption", _absorption);
	_programRaymarcher.setUniform1i("numLightSamples", _numLightSamples);
	_programRaymarcher.setUniform1i("numSamples", _numSamples);
	_programRaymarcher.setUniform3f("lightPos", _light.x, _light.y, _light.z );

	drawBox(boxMax[0]-boxMin[0],boxMax[1]-boxMin[1],boxMax[2]-boxMin[2]);

	_programRaymarcher.disable();
}

ZELOS_NAMESPACE_END

