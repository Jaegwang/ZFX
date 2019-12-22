//--------------//
// ZMaya_File.h //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#ifndef _ZMaya_File_h_
#define _ZMaya_File_h_

#include <ZelosBaseForMaya.h>

ZELOS_NAMESPACE_BEGIN

bool ZSave( const MDoubleArray& a, const char* filePathName );
bool ZSave( const MPointArray&  a, const char* filePathName );

bool ZLoad( MDoubleArray& a, const char* filePathName );
bool ZLoad( MPointArray&  a, const char* filePathName );

ZELOS_NAMESPACE_END

#endif

