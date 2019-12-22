//--------------//
// ZMaya_Plug.h //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.12.26                               //
//-------------------------------------------------------//

#ifndef _ZMaya_Plug_h_
#define _ZMaya_Plug_h_

#include <ZelosBaseForMaya.h>

ZELOS_NAMESPACE_BEGIN

bool GetPlugByName( const MString& objName, const MString& attrName, MPlug& plg );

ZELOS_NAMESPACE_END

#endif

