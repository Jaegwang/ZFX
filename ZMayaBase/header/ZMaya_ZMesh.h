//---------------//
// ZMaya_ZMesh.h //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#ifndef _ZMaya_ZMesh_h_
#define _ZMaya_ZMesh_h_

#include <ZelosBaseForMaya.h>

ZELOS_NAMESPACE_BEGIN

bool Convert( ZMesh& mesh, MObject& meshObj, bool vPosOnly=false, const char* uvSetName=NULL );
bool Convert( ZMesh& mesh, MObjectArray& meshObjs, const char* uvSetName="currentUVSet" );

ZELOS_NAMESPACE_END

#endif

