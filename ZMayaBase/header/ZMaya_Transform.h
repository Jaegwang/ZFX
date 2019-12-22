//-------------------//
// ZMaya_Transform.h //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#ifndef _ZMaya_Transform_h_
#define _ZMaya_Transform_h_

#include <ZelosBaseForMaya.h>

ZELOS_NAMESPACE_BEGIN

bool GetWorldMatrix( const MObject& dagNodeObj, MMatrix& worldMat );
void ApplyXForm( MPoint& p, const MMatrix& m );
void ApplyXForm( const MMatrix& M, const MPoint& p, MPoint& q );

ZELOS_NAMESPACE_END

#endif

