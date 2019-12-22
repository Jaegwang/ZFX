//--------------//
// ZMaya_Time.h //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2016.12.28                               //
//-------------------------------------------------------//

#ifndef _ZMaya_Time_h_
#define _ZMaya_Time_h_

#include <ZelosBaseForMaya.h>

ZELOS_NAMESPACE_BEGIN

float ZCurrentTime();
float ZCurrentTime( MDataBlock& block, MObject& timeObj );
float ZCurrentTimeStepSize();
float ZFPS();

void SetPlayback( double min, double max );

ZELOS_NAMESPACE_END

#endif

