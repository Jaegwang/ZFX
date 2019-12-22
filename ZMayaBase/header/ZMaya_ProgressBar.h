//---------------------//
// ZMaya_ProgressBar.h //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#ifndef _ZMaya_ProgressBar_h_
#define _ZMaya_ProgressBar_h_

#include <ZelosBaseForMaya.h>

ZELOS_NAMESPACE_BEGIN

void StartProgressBar( MString title, int count );

void StepProgressBar( int step );

bool ProgressBarCancelled();

void EndProgressBar();

ZELOS_NAMESPACE_END

#endif

