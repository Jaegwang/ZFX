//------------------//
// ZMaya_ZTriMesh.h //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#ifndef _ZMaya_ZTriMesh_h_
#define _ZMaya_ZTriMesh_h_

#include <ZelosBaseForMaya.h>

ZELOS_NAMESPACE_BEGIN

bool GetMesh( ZTriMesh& mesh, const char* meshShapeNodeName, bool vPosOnly=false, const char* uvSetName=NULL );

bool Convert( ZTriMesh& mesh, MObject& meshObj, bool vPosOnly=false, const char* uvSetName=NULL );
bool Convert( ZTriMesh& mesh, MObjectArray& meshObjs );

bool MPolyPlaneToNormalMap( const char* meshShapeNodeName, ZImageMap& normalMap, ZImageMap& hgtMap, int imgResolution=1, bool xFlip=true, bool zFlip=false, float strength=1.f );

bool GetHeightMap( ZImageMap& hgt, const ZTriMesh& mesh, int subWidth, int subHeight, int imgResolution=1, bool xFlip=true, bool zFlip=false );
bool GetNormalMap( ZImageMap& nrm, const ZTriMesh& mesh, int subWidth, int subHeight, int imgResolution=1, bool xFlip=true, bool zFlip=false, float strength=1.f );

ZELOS_NAMESPACE_END

#endif

