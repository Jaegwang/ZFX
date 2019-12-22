//------------//
// ZGlFbo.cpp //
//-------------------------------------------------------//
// author: Junghyun Cho @ Seoul National Univ.           //
//         Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZGlFbo::ZGlFbo()
{
	_depthOnly = false;
	_id = _id0 = 0;
}

ZGlFbo::~ZGlFbo()
{
	reset();
}

void
ZGlFbo::reset()
{
	if( _id ) { glDeleteFramebuffers( 1, &_id ); }
}

void
ZGlFbo::create() 
{
	if( _id ) { glDeleteFramebuffers( 1, &_id ); }

	glGenFramebuffers( 1, &_id );

	GLint currentFboId = 0;
	glGetIntegerv( GL_FRAMEBUFFER_BINDING, &currentFboId );
	_id0 = (GLuint)currentFboId;

	// Switch back to OS provided framebuffer.
	unbind();
}

void
ZGlFbo::create( const ZGlTex2D& tex, bool isDepth )
{
	create();

	if( isDepth ) {
		attach( tex, GL_DEPTH_ATTACHMENT  );
		_depthOnly = true;
	} else {
		attach( tex, GL_COLOR_ATTACHMENT0 );
		_depthOnly = false;
	}
}

void
ZGlFbo::create( const ZGlTex2D& color, const ZGlTex2D& depth )
{
	create();

	attach( color, GL_COLOR_ATTACHMENT0 );
	attach( depth, GL_DEPTH_ATTACHMENT  );
}

void
ZGlFbo::create( const ZGlTex2D& color, const ZGlRbo& depth )
{
	create();

	attach( color, GL_COLOR_ATTACHMENT0 );
	attach( depth, GL_DEPTH_ATTACHMENT  );
}

void
ZGlFbo::bind() const
{
	glBindFramebuffer( GL_FRAMEBUFFER, _id );
}

void
ZGlFbo::unbind()
{
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

void
ZGlFbo::attach( const ZGlTex2D& tex, GLenum attachment ) const
{
	_guardedBind();

	// Attach the texture to FBO color or depth attachment point.
	glFramebufferTexture2D( GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, tex.id(), 0 );

	_guardedUnbind();
}

void
ZGlFbo::attach( const ZGlRbo& rbo, GLenum attachment ) const
{
	_guardedBind();

	glFramebufferRenderbuffer( GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, rbo.id() );

	_guardedUnbind();
}

void
ZGlFbo::detach( GLenum attachment ) const
{
	_guardedBind();

	switch( attachedType( attachment ) )
	{
		default:
		case GL_NONE:
		{
			break;
		}

		case GL_RENDERBUFFER:
		{
			glFramebufferRenderbuffer( GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, 0 );
			break;
		}

		case GL_TEXTURE:
		{
			glFramebufferTexture2D( GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, 0, 0 );
			break;
		}
	}

	_guardedUnbind();
}

void
ZGlFbo::detachAll() const
{
	const int numAttachments = getMaxColorAttachments();

	for( int i=0; i<numAttachments; ++i )
	{
		detach( GL_COLOR_ATTACHMENT0+i );
	}
}

int ZGlFbo::getMaxColorAttachments()
{
	GLint maxAttach = 0;
	glGetIntegerv( GL_MAX_COLOR_ATTACHMENTS, &maxAttach );
	return maxAttach;
}

void
ZGlFbo::_guardedBind() const 
{
	GLint currentFboId = 0;
	glGetIntegerv( GL_FRAMEBUFFER_BINDING, &currentFboId );
	_id0 = (GLuint)currentFboId;

	// only binds if _id is different than the currently bound FBO
	if( _id == _id0 ) { return; }

	glBindFramebuffer( GL_FRAMEBUFFER, _id );

	// Instruct OpenGL that we won't bind a color texture with the currently bound FBO.
	if( _depthOnly )
	{
		glDrawBuffer( GL_NONE );
		glReadBuffer( GL_NONE );
	}
}

void
ZGlFbo::_guardedUnbind() const
{
	if( _id == _id0 ) { return; }

	glBindFramebuffer( GL_FRAMEBUFFER, (GLuint)_id0 );
}

bool
ZGlFbo::checkStatus() const
{ 
	if( !_id ) { return true; }

	_guardedBind();
	GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER );
	_guardedUnbind();

	if( status != GL_FRAMEBUFFER_COMPLETE )
	{
		cout << "CANNOT use FBO" << endl;
		return false;
	}

	return true;
}

GLenum
ZGlFbo::attachedType( GLenum attachment ) const
{
	_guardedBind();

	GLint type = 0;
	glGetFramebufferAttachmentParameteriv( GL_FRAMEBUFFER, attachment, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &type );

	_guardedUnbind();

	return (GLenum)type; // GL_RENDERBUFFERor GL_TEXTURE
}

GLuint
ZGlFbo::attachedId( GLenum attachment ) const
{
	_guardedBind();

	GLint id = 0;
	glGetFramebufferAttachmentParameteriv( GL_FRAMEBUFFER, attachment, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &id );

	_guardedUnbind();

	return GLuint(id);
}

ZELOS_NAMESPACE_END

