//------------//
// ZGlRbo.cpp //
//-------------------------------------------------------//
// author: Junghyun Cho @ Seoul National Univ.           //
//         Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZGlRbo::ZGlRbo ()
{
	_id = 0;
	_width = _height = 0;
}

ZGlRbo::ZGlRbo( GLsizei w, GLsizei h, GLenum internalFormat ) 
{
	create( w, h, internalFormat );
}

ZGlRbo::~ZGlRbo()
{
	reset();
}

void
ZGlRbo::reset()
{
	if( _id ) { glDeleteRenderbuffers( 1, &_id ); }
}

void
ZGlRbo::bind() const 
{
	{
		GLint currentRboId = 0;
		glGetIntegerv( GL_RENDERBUFFER_BINDING, &currentRboId );
		if( currentRboId == (int)_id ) { return; }
	}

	glBindRenderbuffer( GL_RENDERBUFFER, _id );
}

void
ZGlRbo::unbind()
{
	glBindRenderbuffer( GL_RENDERBUFFER, 0 );
}

void
ZGlRbo::create( GLsizei w, GLsizei h, GLenum internalFormat )
{
	if( _id ) { glDeleteRenderbuffers( 1, &_id ); }

	glGenRenderbuffers( 1, &_id );

	int maxSize = 0;
	glGetIntegerv( GL_MAX_RENDERBUFFER_SIZE, &maxSize );

	_width  = ZMin( w, maxSize );
	_height = ZMin( h, maxSize );

	bind();

	// allocate memory for renderBuffer
	glRenderbufferStorage( GL_RENDERBUFFER, internalFormat, _width, _height );

	// switch back to OS provided renderbuffer
	unbind();
}

ZELOS_NAMESPACE_END

