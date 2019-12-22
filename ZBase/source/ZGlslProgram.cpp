//------------------//
// ZGlslProgram.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZGlslProgram::ZGlslProgram()
{
	_init();
}

//ZGlslProgram::ZGlslProgram( const char* vs, const char* fs )
//{
//	_init();
//	setShaders( vs, fs );
//}

void
ZGlslProgram::_init()
{
	_id        = 0;
	_texTarget = GL_NONE;
}

ZGlslProgram::~ZGlslProgram()
{
	reset();
}

void
ZGlslProgram::reset()
{
	if( !_id ) { return; }

	if( _vs.id() ) { glDetachShader(_id,_vs.id()); _vs.reset(); }
	if( _fs.id() ) { glDetachShader(_id,_fs.id()); _fs.reset(); }
	if( _gs.id() ) { glDetachShader(_id,_gs.id()); _gs.reset(); }

	glDeleteProgram( _id );

	// Indicate no program is current.
	glUseProgram( 0 );

	_init();

	_attributeList.clear();
	_uniformLocationList.clear();
}

void
ZGlslProgram::enable() const
{
 	glUseProgram( _id );
}

void
ZGlslProgram::disable()
{
	if( _texTarget )
	{
		glBindTexture( _texTarget, 0 );
	}

	// Indicate no program is current.
	glUseProgram( 0 );
}

bool
ZGlslProgram::load( const char* vsFilePathName, const char* fsFilePathName )
{
	reset();

	if( !_vs.loadFromFile( vsFilePathName, GL_VERTEX_SHADER ) )
	{
		cout << "Error@ZGlslProgram::load(): Failed to load " << vsFilePathName << endl;
		reset();
		return false;
	}

	if( !_fs.loadFromFile( fsFilePathName, GL_FRAGMENT_SHADER ) )
	{
		cout << "Error@ZGlslProgram::load(): Failed to load " << fsFilePathName << endl;
		reset();
		return false;
	}

	_id = glCreateProgram();

	glAttachShader( _id, _vs.id() );
	glAttachShader( _id, _fs.id() );

	glLinkProgram( _id );
	{
		bool printLog = false;
		if( printLog )
		{
			const unsigned int BUFFER_SIZE = 512;
			char log[BUFFER_SIZE];
			memset( log, 0, BUFFER_SIZE );
			GLsizei logLength = 0;
			
			glGetProgramInfoLog( _id, BUFFER_SIZE, &logLength, log );
			if( logLength > 0 )
			{
				cout << log << endl;
			}
		}
 
		GLint status;
		glValidateProgram( _id );
		glGetProgramiv( _id, GL_VALIDATE_STATUS, &status );
		if( status == GL_FALSE )
		{
			cout << "Error@ZGlslProgram::load(): Failed to link program." << endl;
			reset();
			return false;
		}
	}

	// Disable the programmable processors so that the fixed functionality will be used.
	glUseProgram( 0 );

	return true;
}

bool
ZGlslProgram::setShaders( const char* vs, const char* fs ) 
{
	reset();

	const GLuint vsId = glCreateShader( GL_VERTEX_SHADER   );
	const GLuint fsId = glCreateShader( GL_FRAGMENT_SHADER );

	GLint compiled = 0;

	// Specify the shader source codes.
	glShaderSource( vsId, 1, &vs, 0 );
	glShaderSource( fsId, 1, &fs, 0 );

	// Compile the vertex shader.
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
			glDetachShader( _id, vsId );
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
			glDetachShader( _id, fsId );
			glDeleteShader( fsId );
			return false;
		}
	}

	// Create a program object to attach the shaders into.
	_id = glCreateProgram();

	// Attach the compiled shaders to the program object.
	glAttachShader( _id, vsId );
	glAttachShader( _id, fsId );

	// Link the program object to the application.
	glLinkProgram( _id );
	{
		bool printLog = false;
		if( printLog )
		{
			const unsigned int BUFFER_SIZE = 512;
			char log[BUFFER_SIZE];
			memset( log, 0, BUFFER_SIZE );
			GLsizei logLength = 0;
			
			glGetProgramInfoLog( _id, BUFFER_SIZE, &logLength, log );
			if( logLength > 0 )
			{
				cout << log << endl;
			}
		}
 
		GLint status;
		glValidateProgram( _id );
		glGetProgramiv( _id, GL_VALIDATE_STATUS, &status );
		if( status == GL_FALSE )
		{
			cout << "Error@ZGLProgram::setShaders(): Failed to link program." << endl;
			glDetachShader( _id, vsId );
			glDetachShader( _id, fsId );
			glDeleteShader( vsId );
			glDeleteShader( fsId );
			if( _id ) { glDeleteProgram( _id ); _id = 0; }
			return false;
		}
	}

	// Disable the programmable processors so that the fixed functionality will be used.
	glUseProgram( 0 );

	return true;
}

void
ZGlslProgram::setUniform1i( const char* name, int value )
{
	GLint loc = glGetUniformLocation( _id, name );
	if( loc < 0 ) { return; }

	glUniform1i( loc, value );
}

void
ZGlslProgram::setUniform1f( const char* name, float value )
{
	GLint loc = glGetUniformLocation( _id, name );
	if( loc < 0 ) { return; }

	glUniform1f( loc, value );
}

void
ZGlslProgram::setUniform2f( const char* name, float x, float y )
{
	GLint loc = glGetUniformLocation( _id, name );
	if( loc < 0 ) { return; }

	glUniform2f( loc, x, y );
}

void
ZGlslProgram::setUniform3f( const char* name, float x, float y, float z )
{
	GLint loc = glGetUniformLocation( _id, name );
	if( loc < 0 ) { return; }

	glUniform3f( loc, x, y, z );
}

void
ZGlslProgram::setUniform4f( const char* name, float x, float y, float z, float w )
{
	GLint loc = glGetUniformLocation( _id, name );
	if( loc < 0 ) { return; }

	glUniform4f( loc, x, y, z, w );
}

void
ZGlslProgram::setUniformfv( const GLchar* name, GLfloat* v, int elementSize, int count )
{
	GLint loc = glGetUniformLocation( _id, name );
	if( loc < 0 ) { return; }

	switch( elementSize )
	{
		case 1:
		{
			glUniform1fv( loc, count, v );
			break;
		}
		case 2:
		{
			glUniform2fv( loc, count, v );
			break;
		}
		case 3:
		{
			glUniform3fv( loc, count, v );
			break;
		}
		case 4:
		{
			glUniform4fv( loc, count, v );
			break;
		}
	}
}

void
ZGlslProgram::setUniformMatrix4fv( const GLchar* name, GLfloat* m, bool transpose )
{
	GLint loc = glGetUniformLocation( _id, name );
	if( loc < 0 ) { return; }

	glUniformMatrix4fv( loc, 1, transpose, m );
}

void
ZGlslProgram::bindTexture( const char* name, GLuint texId, GLenum textureTarget, GLint unit )
{
	GLint loc = glGetUniformLocation( _id, name );
	if( loc < 0 ) { return; }

	glActiveTexture( GL_TEXTURE0 + unit );
	glBindTexture( textureTarget, texId );
	glUseProgram( _id );
	glUniform1i( loc, unit );
	glActiveTexture( GL_TEXTURE0 );

	_texTarget = textureTarget;
}

void 
ZGlslProgram::addAttribute( const string& attribute )
{
	_attributeList[attribute]= glGetAttribLocation( _id, attribute.c_str() );
}

void 
ZGlslProgram::addUniform( const string& uniform )
{
	_uniformLocationList[uniform] = glGetUniformLocation( _id, uniform.c_str() );
}

GLuint 
ZGlslProgram::operator []( const string& attribute )
{
	return _attributeList[attribute];
}

GLuint 
ZGlslProgram::operator()( const string& uniform )
{
	return _uniformLocationList[uniform];
}

void 
ZGlslProgram::bindAttribLocation( GLuint location, const char* name)
{
    glBindAttribLocation( _id, location, name );
}

void 
ZGlslProgram::bindFragDataLocation( GLuint location, const char* name )
{
    glBindFragDataLocation( _id, location, name );
}

void 
ZGlslProgram::printActiveUniforms() 
{
	GLint nUniforms, size, location, maxLen;
	GLchar * name;
	GLsizei written;
	GLenum type;

	glGetProgramiv( _id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLen);
	glGetProgramiv( _id, GL_ACTIVE_UNIFORMS, &nUniforms);
	
	name = (GLchar *) malloc( maxLen );

	cout << "================================= "<< endl;
	cout << " < Uniforms > " << endl;
	cout << " Location | Name " << endl;
	cout << "--------------------------------- "<< endl;
    
	FOR( i, 0, nUniforms )
	{
		glGetActiveUniform( _id, i, maxLen, &written, &size, &type, name );
		location = glGetUniformLocation( _id, name );
		cout.width(9);
		cout << location << " | " << name << endl;
	}

    free( name );
}

void
ZGlslProgram::printActiveAttribs()
{
	GLint written, size, location, maxLength, nAttribs;
	GLenum type;
	GLchar* name;

	glGetProgramiv( _id, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxLength );
	glGetProgramiv( _id, GL_ACTIVE_ATTRIBUTES, &nAttribs );
	
	name = (GLchar *) malloc( maxLength );

	cout << "================================= "<< endl;
	cout << " < Attributes > " << endl;
	cout << " Index | Name " << endl;
	cout << "--------------------------------- "<< endl;
	FOR( i, 0, nAttribs )
	{
		glGetActiveAttrib( _id, i, maxLength, &written, &size, &type, name );
		location = glGetAttribLocation( _id, name );
		cout.width(6);
		cout << location << " | " << name << endl;
	}
		
	free( name );
}

ZELOS_NAMESPACE_END

