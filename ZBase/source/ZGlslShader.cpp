//-----------------//
// ZGlslShader.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZGlslShader::ZGlslShader()
{
	_init();
}

ZGlslShader::ZGlslShader( const char* filePathName, GLenum shaderType )
{
	_init();

	loadFromFile( filePathName, shaderType );
}

void
ZGlslShader::_init()
{
	_id   = 0;
	_type = GL_NONE;
}

ZGlslShader::~ZGlslShader()
{
	reset();
}

void
ZGlslShader::reset()
{
	if( !_id ) { return; }
	glDeleteShader( _id );
	_init();
}

bool
ZGlslShader::loadFromFile( const char* filePathName, GLenum shaderType )
{
	reset();

	if( shaderType!=GL_VERTEX_SHADER && shaderType!=GL_FRAGMENT_SHADER )
	{
		cout << "Error@ZGlslShader::loadFromFile(): Invalid shader type." << endl;
		return false;
	}

	_id = glCreateShader( shaderType );
	_type = shaderType;

	ZString shaderSrc;
	shaderSrc.fromTextFile( filePathName );
	const GLchar* shaderSrcText = (const GLchar*)shaderSrc.asChar();
	GLint shaderSrcTextLength = (GLint)shaderSrc.length();

	glShaderSource( _id, 1, &shaderSrcText, &shaderSrcTextLength );
	glCompileShader( _id );

	GLint status;
	glGetShaderiv( _id, GL_COMPILE_STATUS, &status );

	if( status == GL_FALSE )
	{
		cout << "Error@ZGlslShader::loadFromFile(): Failed to compile " << filePathName << endl;

		int logLength = 0;
		glGetShaderiv( _id, GL_INFO_LOG_LENGTH, &logLength );
		if( logLength )
		{
			GLchar* log = new GLchar[logLength];
			glGetShaderInfoLog( _id, logLength, &logLength, log );
			cout << log << endl;
			delete[] log;
		}

		reset();

		return false;
	} 

	return true;
}

ZELOS_NAMESPACE_END

