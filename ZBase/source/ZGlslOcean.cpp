//----------------//
// ZGlslOcean.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// 		   Jinhyuk Bae @ Dexter Studios					 //
// 		   Nayoung Kim @ Dexter Studios					 //
// last update: 2015.10.21                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZGlslOcean::ZGlslOcean()
{
	reset();

	// control parameters
	_preprocessed = false;

//	// control parameters
//	_brightness = 1.0f;
//	_densityFactor = 10.0f;
//	_absorption = 10.0f;
//	_numLightSamples = 32;
//	_numSamples = 256;
//
//	// draw light direction
//	bDrawLightDir = false;
//
//	_preprocessed = false;
}

ZGlslOcean::~ZGlslOcean()
{
	_triMeshPtr = (ZTriMesh*)NULL;
	_normalPtr  = (ZVectorArray*)NULL;
	_foamPtr 	= (ZFloatArray*)NULL;

	if( _triMeshPtr ) delete _triMeshPtr;
	if( _normalPtr  ) delete _normalPtr;
	if( _foamPtr    ) delete _foamPtr;
}

void
ZGlslOcean::setTriMeshPtr( ZTriMesh* triMeshPtr )
{
	if( triMeshPtr ) 
	{ 	
		_triMeshPtr = triMeshPtr; 
		_numOfVtx = _triMeshPtr->numVertices();
	}
}

void
ZGlslOcean::setNormalPtr( ZVectorArray* normalPtr )
{
	if( normalPtr ) { _normalPtr = normalPtr; }
}

void
ZGlslOcean::setFoamPtr( ZFloatArray* foamPtr )
{
	if( foamPtr ) { _foamPtr = foamPtr; }
}

void
ZGlslOcean::setModelViewMatrix( const ZMatrix& modelViewMat )
{
	_modelViewMat = modelViewMat;
}

void
ZGlslOcean::setProjectionMatrix( const ZMatrix& projectionMat )
{
	_projectionMat = projectionMat;
}

void
ZGlslOcean::preprocess( const ZString& vsPath, const ZString& fsPath )
{
 	//=================================
 	// initialize shader
 	//=================================
	//
	ZString vsStr, fsStr;
	vsStr.fromTextFile( vsPath.asChar() );
	fsStr.fromTextFile( fsPath.asChar() );

	setShaders( vsStr.asChar(), fsStr.asChar() );

	// use
	glUseProgram( _programId );

		_vertexId 		= glGetAttribLocation( _programId, "vVertex" );
		_normalId 		= glGetAttribLocation( _programId, "vNormal" );
		_textureId		= glGetAttribLocation( _programId, "vTexture" );
		_uProjectionId 	= glGetUniformLocation( _programId, "Projection" );
		_uModelViewId   = glGetUniformLocation( _programId, "ModelView"  );
		_uLightPosId 	= glGetUniformLocation( _programId, "lightPos"   );
		_uCameraPosId   = glGetUniformLocation( _programId, "cameraPos"  );

	glUseProgram( 0 );

	glGenBuffers( 1, &_vbo_verticesId 	);
	glGenBuffers( 1, &_vbo_normalId 	);
	glGenBuffers( 1, &_vbo_indicesId  	);

	//=================================
 	// load data
 	//=================================
	//
	const int sizeOfIdx = _triMeshPtr->v012.size();
	_vIndexArr.resize( sizeOfIdx * 3 );
		
	FOR( i, 0, sizeOfIdx )
	{
		_vIndexArr[i*3]		= _triMeshPtr->v012[i][0];
		_vIndexArr[i*3+1]	= _triMeshPtr->v012[i][1];
		_vIndexArr[i*3+2]	= _triMeshPtr->v012[i][2];
	}

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _vbo_indicesId );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, 3*(_triMeshPtr->v012.size())*sizeof(GLushort), &_vIndexArr[0], GL_STATIC_DRAW );

	// vertex
	ZPoint* vtxPtr = _triMeshPtr->p.pointer();
	glBindBuffer( GL_ARRAY_BUFFER, _vbo_verticesId );
	glBufferData( GL_ARRAY_BUFFER, _triMeshPtr->numVertices()*sizeof(ZPoint), &vtxPtr[0].x, GL_DYNAMIC_DRAW );

	// normal
	glBindBuffer( GL_ARRAY_BUFFER, _vbo_normalId );
	glBufferData( GL_ARRAY_BUFFER, _normalPtr->length()*sizeof(ZVector), &(*_normalPtr)[0], GL_DYNAMIC_DRAW );

	
//	glGetIntegerv( GL_VIEWPORT, _viewport );	// get viewport
//	setupOcean3D( density );					// ZScalarField3D -> 3D texture
//	setupFbo();
//	setupProgram();
}

void
ZGlslOcean::postprocess()
{
	_preprocessed = false;
}

void
ZGlslOcean::draw()
{
	if( _triMeshPtr ) return;

	
	
}

void
ZGlslOcean::reset()
{
	_programId = 0;
	_vertexId = _normalId = 0;

	_uProjectionId = _uModelViewId = _uLightPosId = 0;
	_uCameraPosId = 0;
	_vbo_verticesId = 0;
	_vbo_indicesId  = 0;
	_vaoId = 0;
}

bool
ZGlslOcean::setShaders( const char* vsSrc, const char* fsSrc )
{
	const GLuint vsId = glCreateShader( GL_VERTEX_SHADER   );
	const GLuint fsId = glCreateShader( GL_FRAGMENT_SHADER );

	GLint compiled = 0;

	// Specify the shader source codes
	glShaderSource( vsId, 1, &vsSrc, 0 );
	glShaderSource( fsId, 1, &fsSrc, 0 );
	
	// compile the vertex shader
	glCompileShader( vsId );
	{
		GLint status;
		glGetShaderiv( vsId, GL_COMPILE_STATUS, &status );

		if( status == GL_FALSE )
		{
			int logLength = 0;
			glGetShaderiv( vsId, GL_INFO_LOG_LENGTH, &logLength );
			if( logLength )
			{
				GLchar* log = new GLchar[logLength];
				glGetShaderInfoLog( vsId, logLength, &logLength, log );
				cout << "Error@ZGlslProgram::setShaders(): " << (char*)log << endl;
				delete[] log;
			}
			glDetachShader( _programId, vsId );
			glDeleteShader( vsId );
			return false;
		}
	}
	
	// Compile the fragment shader.
	glCompileShader( fsId );
	{ 
		GLint status;
		glGetShaderiv( fsId, GL_COMPILE_STATUS, &status );
		if( status == GL_FALSE )
		{
			int logLength = 0;
			glGetShaderiv( fsId, GL_INFO_LOG_LENGTH, &logLength );
			if( logLength )
			{
				GLchar* log = new GLchar[logLength];
				glGetShaderInfoLog( fsId, logLength, &logLength, log );
				cout << "Error@ZGlslProgram::setShaders(): " << (char*)log << endl;
				delete[] log;
			}
			glDetachShader( _programId, fsId );
			glDeleteShader( fsId );
			return false;
		}
	}

 	// program
	_programId = glCreateProgram();
	
	glAttachShader( _programId, vsId );
	glAttachShader( _programId, fsId );

	glLinkProgram( _programId );
	{
		bool printLog = false;
		if( printLog )
		{
			const unsigned int BUFFER_SIZE = 512;
			char log[BUFFER_SIZE];
			memset( log, 0, BUFFER_SIZE );
			GLsizei logLength = 0;
			
			glGetProgramInfoLog( _programId, BUFFER_SIZE, &logLength, log );
			if( logLength > 0 )
			{
				cout << log << endl;
			}
		}

		GLint status;
		glValidateProgram( _programId );
		glGetProgramiv( _programId, GL_VALIDATE_STATUS, &status );
		if( status == GL_FALSE )
		{
			cout << "Error@ZGLProgram::setShaders(): Failed to link program." << endl;
			glDetachShader( _programId, vsId );
			glDetachShader( _programId, fsId );
			glDeleteShader( vsId );
			glDeleteShader( fsId );
			if( _programId ) { glDeleteProgram( _programId ); _programId = 0; }
			return false;
		}
	}

	glUseProgram(0);

	return true;
}


//void
//ZGlslOcean::setupOcean3D( const ZScalarField3D& density )
//{
//	_density = (ZScalarField3D*)&density;
//
//	const int nx = density.nx();
//	const int ny = density.ny();
//	const int nz = density.nz();
//
//	_volumeData.create( nx, ny, nz, GL_R32F, GL_RED, GL_FLOAT, GL_LINEAR, GL_CLAMP_TO_BORDER, (GLvoid*)density.pointer() );
//}
//
//void
//ZGlslOcean::setupFbo()
//{
//	_fbo.create();
//	_tex.create( _viewport[2], _viewport[3], GL_RGBA16F, GL_RGBA, GL_FLOAT,GL_LINEAR,GL_CLAMP_TO_EDGE );
//	_rbo.create( _viewport[2], _viewport[3], GL_DEPTH_COMPONENT );
//	_fbo.attach( _tex, GL_COLOR_ATTACHMENT0 );
//	_fbo.attach( _rbo, GL_DEPTH_ATTACHMENT );
//	_fbo.checkStatus();
//}
//
//void
//ZGlslOcean::setupProgram()
//{
//	ZString path( "/home/rnd00/" );
//	ZString vs, fs;
//	vs.fromTextFile( ( path + "rayMarcherVS.glsl" ).asChar() );
//	fs.fromTextFile( ( path + "rayMarcherFS.glsl" ).asChar() );
//	_programRaymarcher.setShaders( vs.asChar(), fs.asChar() );
//}
//
//void
//ZGlslOcean::toggleDrawLightDir()
//{
//	bDrawLightDir = bDrawLightDir ? false : true;
//}
//
//void
//ZGlslOcean::draw()
//{
//	_eye = ZCurrentCameraPosition();
//	//--------------------------------------------------------
//	// if viewport changed, reset fbo
//	GLint vp[4];
//	glGetIntegerv(GL_VIEWPORT, (GLint*)vp);
//	if(_viewport[2] != vp[2] || _viewport[3] != vp[3]) {
//		for(int i=0; i<4; ++i) _viewport[i] = vp[i];
//		setupFbo();
//	}
//
//	//--------------------------------------------------------
//	// 1. render to fbo
//	_fbo.bind();
//	rayCasting();
//	_fbo.unbind();
//
//
//	//--------------------------------------------------------
//	// 2. render texture to screen
//	renderTexturedQuad();
//
//
//	//--------------------------------------------------------
//	// draw light direction
//	if(bDrawLightDir) drawLightDir();
//
//
//}
//
//void ZGlslOcean::drawLightDir() {
//	glPushAttrib(GL_ALL_ATTRIB_BITS);
//	glDisable ( GL_LIGHTING );
//	glDisable( GL_DEPTH_TEST );
//
//	glColor3f ( 1, 1, 0 );
//	glBegin ( GL_LINES );
//	glVertex ( _center );
//	glVertex( _light );
//	glEnd();
//
//	glColor3f ( 0, 0, 1 );
//	glBegin ( GL_LINES );
//	glVertex ( _center );
//	glVertex ( _eye );
//	glEnd();
//
//	glLineWidth ( 1.f );
//	glPopAttrib();
//}
//
//
//
//
//void ZGlslOcean::renderTexturedQuad() {
//	glMatrixMode(GL_PROJECTION);
//	glPushMatrix();
//	glLoadIdentity();
//	glMatrixMode(GL_MODELVIEW);
//	glPushMatrix();
//	glLoadIdentity();
//	glPushAttrib(GL_ALL_ATTRIB_BITS);
//
//	glEnable(GL_TEXTURE_2D);
//	glEnable(GL_BLEND);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	_tex.bind();
//	glDisable(GL_LIGHTING);
//	glColor4f(1,1,1,1);
//
//	glBegin(GL_QUADS);
//	glTexCoord2f(0,0);
//	glVertex3i(-1,-1,-1);
//	glTexCoord2f(1,0);
//	glVertex3i( 1,-1,-1);
//	glTexCoord2f(1,1);
//	glVertex3i( 1, 1,-1);
//	glTexCoord2f(0,1);
//	glVertex3i(-1, 1,-1);
//	glEnd();
//
//	glPopAttrib();
//	glMatrixMode(GL_PROJECTION);
//	glPopMatrix();
//	glMatrixMode(GL_MODELVIEW);
//	glPopMatrix();
//}
//
//
//void ZGlslOcean::drawBox(float w, float h, float d) {
//	float wm = w*0.5f, hm = h*0.5f, dm = d*0.5f;
//
//	glBegin(GL_QUADS);
//	glVertex3f(-wm, -hm, -dm);
//	glVertex3f(-wm, hm, -dm);
//	glVertex3f(wm, hm, -dm);
//	glVertex3f(wm, -hm, -dm);
//
//	glVertex3f(-wm, -hm, dm);
//	glVertex3f(wm, -hm, dm);
//	glVertex3f(wm, hm, dm);
//	glVertex3f(-wm, hm, dm);
//
//	glVertex3f(-wm, hm, -dm);
//	glVertex3f(-wm, hm, dm);
//	glVertex3f(wm, hm, dm);
//	glVertex3f(wm, hm, -dm);
//
//	glVertex3f(-wm, -hm, -dm);
//	glVertex3f(wm, -hm, -dm);
//	glVertex3f(wm, -hm, dm);
//	glVertex3f(-wm, -hm, dm);
//
//	glVertex3f(-wm, -hm, -dm);
//	glVertex3f(-wm, -hm, dm);
//	glVertex3f(-wm, hm, dm);
//	glVertex3f(-wm, hm, -dm);
//
//	glVertex3f(wm, -hm, -dm);
//	glVertex3f(wm, hm, -dm);
//	glVertex3f(wm, hm, dm);
//	glVertex3f(wm, -hm, dm);
//	glEnd();
//}
//
//
//void ZGlslOcean::rayCasting() {
//
//	float axisMax = (float)ZMax(_density->nx(), _density->ny(), _density->nz());
//
//	float boxMin[3],boxMax[3];
//	boxMin[0] = (float)_density->nx()/axisMax * (-0.5f);
//	boxMin[1] = (float)_density->ny()/axisMax * (-0.5f);
//	boxMin[2] = (float)_density->nz()/axisMax * (-0.5f);
//	boxMax[0] = (float)_density->nx()/axisMax * (0.5f);
//	boxMax[1] = (float)_density->ny()/axisMax * (0.5f);
//	boxMax[2] = (float)_density->nz()/axisMax * (0.5f);
//
//
//	glClearColor(0,0,0,0);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
//
//	_programRaymarcher.enable();
//
//	_programRaymarcher.bindTexture("volumeData",_volumeData.id(), GL_TEXTURE_3D, 0);
//	_programRaymarcher.setUniform3f("boxMin", boxMin[0], boxMin[1], boxMin[2]);
//	_programRaymarcher.setUniform3f("boxMax", boxMax[0], boxMax[1], boxMax[2]);
//	_programRaymarcher.setUniform1f("brightness", _brightness);
//	_programRaymarcher.setUniform1f("densityFactor", _densityFactor);
//	_programRaymarcher.setUniform1f("absorption", _absorption);
//	_programRaymarcher.setUniform1i("numLightSamples", _numLightSamples);
//	_programRaymarcher.setUniform1i("numSamples", _numSamples);
//	_programRaymarcher.setUniform3f("lightPos", _light.x, _light.y, _light.z );
//
//	drawBox(boxMax[0]-boxMin[0],boxMax[1]-boxMin[1],boxMax[2]-boxMin[2]);
//
//	_programRaymarcher.disable();
//}

ZELOS_NAMESPACE_END

