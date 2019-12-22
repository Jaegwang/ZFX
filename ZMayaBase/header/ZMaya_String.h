//----------------//
// ZMaya_String.h //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#ifndef _ZMaya_String_h_
#define _ZMaya_String_h_

#include <ZelosBaseForMaya.h>

ZELOS_NAMESPACE_BEGIN

MString MakePadding( int number, int paddingCount );

void RemoveSpace( MString& str );

bool GetList( const MString& string, MIntArray& list );

ZELOS_NAMESPACE_END

#endif

