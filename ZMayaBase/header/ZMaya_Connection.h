//---------------------//
// ZMaya_Connections.h //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#ifndef _ZMaya_Connections_h_
#define _ZMaya_Connections_h_

#include <ZelosBaseForMaya.h>

ZELOS_NAMESPACE_BEGIN

bool GetConnectedNodeObject( const MObject& nodeObj, const MObject& plgObj, bool asDestination, MObject& connectedNodeObj );

// when the connection to get is not an array attribute, index and logicalIndex arguments are not meaningful.
bool GetConnections( const MObject& nodeObj, const MString& plugName, int index, bool logicalIndex, bool asDestination, MObjectArray& connectedNodeObjs, MPlugArray& connectedPlgs );

// when the connection to get is not an array attribute, index and logicalIndex arguments are not meaningful.
bool GetConnections( const MObject& nodeObj, const MObject& plugObj, int index, bool logicalIndex, bool asDestination, MObjectArray& connectedNodeObjs, MPlugArray& connectedPlgs );

MStringArray GetConnectedNodeNames( const char* nodeName, const char* plugName, const char* typeName=NULL );

ZELOS_NAMESPACE_END

#endif

