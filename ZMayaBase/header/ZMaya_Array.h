//---------------//
// ZMaya_Array.h //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.08                               //
//-------------------------------------------------------//

#ifndef _ZMaya_Array_h_
#define _ZMaya_Array_h_

#include <ZelosBaseForMaya.h>

ZELOS_NAMESPACE_BEGIN

MPoint Center( MPointArray& verts );

MVector Center( MVectorArray& verts );

void GetMinMaxPoints( MFloatPointArray& pos, MFloatPoint& minPt, MFloatPoint& maxPt );

void GetMinMaxPoints( MPointArray& pos, MPoint& minPt, MPoint& maxPt );

ZELOS_NAMESPACE_END

#endif

