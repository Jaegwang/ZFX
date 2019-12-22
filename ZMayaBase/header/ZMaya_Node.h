//--------------//
// ZMaya_Node.h //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#ifndef _ZMaya_Node_h_
#define _ZMaya_Node_h_

#include <ZelosBaseForMaya.h>

ZELOS_NAMESPACE_BEGIN

// dag path <- node name
MDagPath NodeNameToDagPath( const MString& dagNodeName );

// dag path <- node object
MDagPath MObjectToDagPath( const MObject& dagNodeObj );

// node object <- node name
MObject NodeNameToMObject( const MString& nodeName );

// node object <- dag path
MObject DagPathToMObject( const MDagPath& dagPath );

// node name <- node object
MString MObjectToNodeName( const MObject& nodeObj );

bool IsShapeNode( const MDagPath& dagPath );
MDagPath GetShapeNodeDagPath( MDagPath dagPath, bool intermediate );
MString ShapeNodeName( const MString& xformNodeName );

void GetSelectedNodes( MDagPathArray& dagPaths, MFn::Type type, bool excludeIntermediateObjects );

MStringArray ParentNodeName( const MString& nodeName );

void CreateNode( const MString& nodeName, const MString& nodeType );
void CreateNode( const MString& nodeName, const MString& nodeType, const MString& parentName );
void CreateNodeIfNotExists( const MString& nodeName, const MString& nodeType );
void CreateNodeIfNotExists( const MString& nodeName, const MString& nodeType, const MString& parentName );
int  NodeExists( const MString& nodeName );
void Rename( const MString& nodeName, const MString& newName );
void SetParent( const MString& child, const MString& parent );

ZELOS_NAMESPACE_END

#endif

