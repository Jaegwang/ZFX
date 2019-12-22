//------------//
// ZGlVbo.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZGlVbo::ZGlVbo()
{
	_id = 0;
}

ZGlVbo::~ZGlVbo()
{
	if( _id )
	{
		glDeleteBuffers( 1, &_id );
	}
}

void
ZGlVbo::unbind()
{
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

ostream&
operator<<( ostream& os, const ZGlVbo& object )
{
	os << "<ZGlVbo>" << endl;
	os << endl;

	return os;
}

ZELOS_NAMESPACE_END

