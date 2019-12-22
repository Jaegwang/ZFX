//--------------//
// ZGlTex3D.cpp //
//-------------------------------------------------------//
// author: Junghyun Cho @ Seoul National Univ.           //
//         Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZGlTex3D::ZGlTex3D()
{
	_id = 0;
	_width = _height = _depth = 0;
}

ZGlTex3D::ZGlTex3D
(
	GLsizei w, GLsizei h, GLsizei d,
	GLenum internalFormat, GLenum format, GLenum type,
	GLenum filter, GLenum clamp,
	const GLvoid* data
)
{
	create
	(
		w, h, d,
		internalFormat, format, type,
		filter, clamp,
		data
	);
}

ZGlTex3D::~ZGlTex3D()
{
	reset();
}

void
ZGlTex3D::reset()
{
	if( _id ) { glDeleteTextures( 1, &_id ); }
}

void
ZGlTex3D::create
(
	GLsizei w, GLsizei h, GLsizei d,
	GLenum internalFormat, GLenum format, GLenum type,
	GLenum filter, GLenum clamp,
	const GLvoid* data
)
{
	if( _id ) { glDeleteTextures( 1, &_id ); }

	glGenTextures( 1, &_id );

	_width  = w;
	_height = h;
	_depth  = d;

	bind();

	// Set texture environment parameters.
	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

	// Set filter when the display size is smaller/larger thant the size of the texture.
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, filter );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, filter );

	// Set clamp in s-/t- directions.
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, clamp );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, clamp );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, clamp );

	// Allocate GPU memory and load data.
	glTexImage3D( GL_TEXTURE_3D, 0, internalFormat, w, h, d, 0, format, type, data );

	// Indicate no 3D texture object is current.
	unbind();
}

void
ZGlTex3D::enable()
{
	glEnable( GL_TEXTURE_3D );
}

void
ZGlTex3D::disable()
{
	glDisable( GL_TEXTURE_3D );
}

void
ZGlTex3D::bind() const
{
	glBindTexture( GL_TEXTURE_3D, _id );
}

void
ZGlTex3D::unbind()
{
	glBindTexture( GL_TEXTURE_3D, 0 );
}

void
ZGlTex3D::setFilter( GLenum magFilter, GLenum minFilter )
{
	bind();

	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, magFilter );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, minFilter );

	unbind();
}

void
ZGlTex3D::setClamp( GLenum sClamp, GLenum tClamp, GLenum rClamp )
{
	bind();

	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, sClamp );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, tClamp );
	glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, rClamp );

	unbind();
}

ZELOS_NAMESPACE_END

