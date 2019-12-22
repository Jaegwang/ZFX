//----------------//
// ZMaya_OpenGL.h //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#ifndef _ZMaya_OpenGL_h_
#define _ZMaya_OpenGL_h_

#include <ZelosBaseForMaya.h>

ZELOS_NAMESPACE_BEGIN

inline void glColor( const MColor& c )
{ glColor4fv( &c.r ); }

inline void glVertex( const MPoint& p )
{ glVertex3dv( &p.x ); }

inline void glLine( const MPoint& p, const MVector& v )
{ glVertex(p); glVertex3d(p.x+v.x,p.y+v.y,p.z+v.z); }

void glDrawCube( const MPoint& min, const MPoint& max );

ZELOS_NAMESPACE_END

#endif

