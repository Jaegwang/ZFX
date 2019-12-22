//----------//
// Node.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.12.26                               //
//-------------------------------------------------------//

#include <ZelosBaseForMaya.h>

ZELOS_NAMESPACE_BEGIN

MDagPath
NodeNameToDagPath( const MString& dagNodeName )
{
	MDagPath dagPath;
	MSelectionList sList;
	MStatus stat = MGlobal::getSelectionListByName( dagNodeName, sList );
	if( !stat ) { mPrintError("Error@DagPath(): Failed."); return MDagPath(); }
	sList.getDagPath( 0, dagPath );
	return dagPath;
}

MDagPath
MObjectToDagPath( const MObject& dagNodeObj )
{
	MStatus stat = MS::kSuccess;
	MFnDagNode dagFn( dagNodeObj, &stat );
	if( !stat ) { mPrintError("Error@DagPath(): Failed to get MFnDagNode."); return MDagPath(); }
	MDagPath dagPath;
	stat = dagFn.getPath( dagPath );
	if( !stat ) { mPrintError("Error@DagPath(): Failed to get MDagPath."); return MDagPath(); }
	return dagPath;
}

MObject
NodeNameToMObject( const MString& nodeName )
{
	MObject obj;
	MSelectionList sList;
	MStatus stat = MGlobal::getSelectionListByName( nodeName, sList );
	if( !stat ) { mPrintError( "Error@DagPath(): Failed." ); return MObject::kNullObj; }
	sList.getDependNode( 0, obj );
	return obj;
}

MObject
DagPathToMObject( const MDagPath& dagPath )
{
	return dagPath.node();
}

MString
MObjectToNodeName( const MObject& nodeObj )
{
	MString nodeName;

	if( nodeObj.hasFn( MFn::kDagNode ) ) {

		MFnDagNode dagNodeFn( nodeObj );
		nodeName = dagNodeFn.fullPathName();

	} else if( nodeObj.hasFn( MFn::kDependencyNode ) ) {

		MFnDependencyNode dgNodeFn( nodeObj );
		nodeName = dgNodeFn.name();

	} else {

		return MString();

	}

	return nodeName;
}

bool
IsShapeNode( const MDagPath& dagPath )
{
	MObject obj = dagPath.node();

	if( obj.hasFn( MFn::kMesh         ) ) { return true; }
	if( obj.hasFn( MFn::kNurbsCurve   ) ) { return true; }
	if( obj.hasFn( MFn::kNurbsSurface ) ) { return true; }

	return false;
}

MDagPath
GetShapeNodeDagPath( MDagPath dagPath, bool intermediate )
{
	MStatus stat = MS::kSuccess;

	MDagPath emptyDagPath;

	if( IsShapeNode( dagPath ) )
	{
		return dagPath;
	}

	if( !dagPath.hasFn( MFn::kTransform ) )
	{
		return emptyDagPath;
	}

	unsigned int numberOfShapes = 0;
	{
		stat = dagPath.numberOfShapesDirectlyBelow( numberOfShapes );

		if( stat != MStatus::kSuccess )
		{
			return emptyDagPath;
		}

		if( !numberOfShapes )
		{
			return emptyDagPath;
		}
	}

	for( unsigned int i=0; i< numberOfShapes; ++i )
	{
		stat = dagPath.extendToShapeDirectlyBelow( i );
		{
			if( stat != MStatus::kSuccess )
			{
				return emptyDagPath;
			}
		}

		// Make sure it is not an intermediate object.
		MFnDagNode dagNodeFn( dagPath, &stat );
		{
			if( stat != MStatus::kSuccess )
			{
				return emptyDagPath;
			}
		}

		if( ( !dagNodeFn.isIntermediateObject() && !intermediate )
		 || (  dagNodeFn.isIntermediateObject() &&  intermediate ) )
		{
			return dagPath;
		}
	}
}

MString
ShapeNodeName( const MString& xformNodeName )
{
	MString shapeNodeName;
	MString nodeType;
	MStringArray children;

	MGlobal::executeCommand( MString( "listRelatives -c " ) + xformNodeName, children );

	unsigned int i = 0;
	for( ; i<children.length(); ++i )
	{
		MGlobal::executeCommand( MString( "nodeType " ) + children[i], nodeType );

		if( nodeType == "mesh" )
		{
			shapeNodeName = children[i];
		}
	}

	return shapeNodeName;
}

void
GetSelectedNodes( MDagPathArray& dagPaths, MFn::Type type, bool excludeIntermediateObjects )
{
	dagPaths.clear();

	MSelectionList sList;
	MGlobal::getActiveSelectionList( sList );

	MItSelectionList itr( sList, type );
	for( ; !itr.isDone(); itr.next() )
	{
		MDagPath dagPath;
		itr.getDagPath( dagPath );

		bool toAppend = true;

		if( excludeIntermediateObjects )
		{
			MFnDagNode dagFn( dagPath );
			if( dagFn.isIntermediateObject() )
			{
				toAppend = false;
			}
		}

		if( toAppend )
		{
			dagPaths.append( dagPath );
		}
	}
}
// other possible implementation
//{
//	dagPaths.clear();
//
//	MSelectionList selectionList;
//	MGlobal::getActiveSelectionList( selectionList );
//
//	MItSelectionList itList( selectionList );
//	for( ; !itList.isDone(); itList.next() )
//	{
//		MDagPath dagPath;
//		itList.getDagPath( dagPath );
//
//		FOR( i, 0, dagPath.childCount() )
//		{
//			dagPath.push( dagPath.child(i) );
//
//			if( dagPath.node().hasFn( type ) )
//			{
//				bool toAppend = true;
//
//				if( excludeIntermediateObjects )
//				{
//					MFnDagNode dagFn( dagPath );
//					if( dagFn.isIntermediateObject() )
//					{
//						toAppend = false;
//					}
//				}
//
//				if( toAppend )
//				{
//					dagPaths.append( dagPath );
//				}
//			}
//
//			dagPath.pop();
//		}
//	}
//}

MStringArray
ParentNodeNames( const MString& nodeName )
{
	MStringArray tempStrings;
	MStringArray parents;
	MString parent;

	MString currentNode = nodeName;

	while( currentNode != "" )
	{
		MGlobal::executeCommand( MString( "listRelatives -p " ) + currentNode, tempStrings );
		if( tempStrings[0] != "" )	parents.append( tempStrings[0] );
		currentNode = tempStrings[0];
	}

	return parents;
}

void
CreateNode( const MString& nodeName, const MString& nodeType )
{
	MGlobal::executeCommand( MString( "createNode " ) + nodeType + " -n \"" + nodeName + "\"" );
}

void
CreateNode( const MString& nodeName, const MString& nodeType, const MString& parentName )
{
	MGlobal::executeCommand( MString( "createNode " ) + nodeType + " -n \"" + nodeName + "\"" + " -p \"" + parentName + "\"" );
}

void
CreateNodeIfNotExists( const MString& nodeName, const MString& nodeType )
{
	int existQuary;
	MGlobal::executeCommand( MString( "objExists " ) + nodeName, existQuary );
	if( existQuary ) { return; }
	MGlobal::executeCommand( MString( "createNode " ) + nodeType + " -n \"" + nodeName + "\"" );
}

void
CreateNodeIfNotExists( const MString& nodeName, const MString& nodeType, const MString& parentName )
{
	int existQuary;
	MGlobal::executeCommand( MString( "objExists " ) + nodeName, existQuary );
	if( existQuary ) { return; }
	MGlobal::executeCommand( MString( "createNode " ) + nodeType + " -n \"" + nodeName + "\"" + " -p \"" + parentName + "\"" );
}

int
NodeExists( const MString& nodeName )
{
	int existQuary;
	MGlobal::executeCommand( MString( "objExists " ) + nodeName, existQuary );
	return existQuary;
}

void
Rename( const MString& nodeName, const MString& newName )
{
	MGlobal::executeCommand( MString( "rename " ) + nodeName + " " + newName );
}

void
SetParent( const MString& child, const MString& parent )
{
	MGlobal::executeCommand( MString( "parent " ) + child + " " + parent );
}

ZELOS_NAMESPACE_END

