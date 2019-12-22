//-----------------//
// ZMaya_ZCurves.h //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.11.24                               //
//-------------------------------------------------------//

#ifndef _ZMaya_ZCurves_h_
#define _ZMaya_ZCurves_h_

#include <ZelosBaseForMaya.h>

ZELOS_NAMESPACE_BEGIN

bool Convert( ZCurves& curves, const MObjectArray& curveObjs, const MMatrixArray& worldMats, bool toResample );

bool Convert( ZCurves& curves, const MObjectArray& curveObjs, const MMatrixArray& worldMats, bool toResample, const ZCharArray& onOff );

ZELOS_NAMESPACE_END

#endif

