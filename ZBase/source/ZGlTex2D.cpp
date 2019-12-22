//--------------//
// ZGlTex2D.cpp //
//-------------------------------------------------------//
// author: Junghyun Cho @ Seoul National Univ.           //
//         Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZGlTex2D::ZGlTex2D()
{
	_id = 0;
	_width = _height = 0;
}

ZGlTex2D::ZGlTex2D
(
	GLsizei w, GLsizei h,
	GLenum internalFormat, GLenum format, GLenum type,
	GLenum filter, GLenum clamp,
	const GLvoid* data
)
{
	create
	(
		w, h,
		internalFormat, format, type,
		filter, clamp,
		data
	);
}

ZGlTex2D::~ZGlTex2D()
{
	reset();
}

void
ZGlTex2D::reset()
{
	if( _id ) { glDeleteTextures( 1, &_id ); }
}

void
ZGlTex2D::create
(
	GLsizei w, GLsizei h,
	GLenum internalFormat, GLenum format, GLenum type,
	GLenum filter, GLenum clamp,
	const GLvoid* data
)
{
	if( _id ) { glDeleteTextures( 1, &_id ); }

	glGenTextures( 1, &_id );

	_width  = w;
	_height = h;

	bind();

	// Set texture environment parameters.
	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

	// Set filter when the display size is smaller/larger thant the size of the texture.
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter );

	// Set clamp in s-/t- directions.
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, clamp );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, clamp );

	// Allocate GPU memory and load data.
	glTexImage2D( GL_TEXTURE_2D, 0, internalFormat, w, h, 0, format, type, data );

	// Indicate no 2D texture object is current.
	unbind();
}

void
ZGlTex2D::enable()
{
	glEnable( GL_TEXTURE_2D );
}

void
ZGlTex2D::disable()
{
	glDisable( GL_TEXTURE_2D );
}

void
ZGlTex2D::bind() const
{
	glBindTexture( GL_TEXTURE_2D, _id );
}

void
ZGlTex2D::unbind()
{
	glBindTexture( GL_TEXTURE_2D, 0 );
}

void
ZGlTex2D::setFilter( GLenum magFilter, GLenum minFilter )
{
	bind();

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter );

	unbind();
}

void
ZGlTex2D::setClamp( GLenum sClamp, GLenum tClamp )
{
	bind();

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sClamp );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tClamp );

	unbind();
}

ZELOS_NAMESPACE_END

