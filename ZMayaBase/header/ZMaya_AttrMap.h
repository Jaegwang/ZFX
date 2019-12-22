//-----------------//
// ZMaya_AttrMap.h //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.05.08                               //
//-------------------------------------------------------//

#ifndef _ZMaya_AttrMap_h_
#define _ZMaya_AttrMap_h_

#include <ZelosBaseForMaya.h>

ZELOS_NAMESPACE_BEGIN

bool GetMapValues( MDataBlock& block, const MObject& attrMapObj, MIntArray&    mapValues );
bool GetMapValues( MDataBlock& block, const MObject& attrMapObj, MDoubleArray& mapValues );
bool GetMapValues( MDataBlock& block, const MObject& attrMapObj, MVectorArray& mapValues );
bool SetMapValues( MDataBlock& block, const MObject& attrMapObj, MIntArray&    mapValues );
bool SetMapValues( MDataBlock& block, const MObject& attrMapObj, MDoubleArray& mapValues );
bool SetMapValues( MDataBlock& block, const MObject& attrMapObj, MVectorArray& mapValues );

ZELOS_NAMESPACE_END

#endif

