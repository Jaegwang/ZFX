//-----------------//
// Connections.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#include <ZelosBaseForMaya.h>

ZELOS_NAMESPACE_BEGIN

bool
GetConnectedNodeObject( const MObject& nodeObj, const MObject& plgObj, bool asDestination, MObject& connectedNodeObj )
{
	MFnDependencyNode dgFn( nodeObj );
	MPlug plg = dgFn.findPlug( plgObj );
	if( plg.isConnected() )
	{
		MPlugArray connectedPlgs;
		if( asDestination ) { plg.connectedTo( connectedPlgs, true, false ); }
		else                { plg.connectedTo( connectedPlgs, false, true ); }

		connectedNodeObj = connectedPlgs[0].node();

		return true;
	}

	return false;
}

bool
GetConnections( const MObject& nodeObj, const MString& plugName, int index, bool logicalIndex, bool asDestination, MObjectArray& connectedNodeObjs, MPlugArray& connectedPlgs )
{
	MStatus stat = MS::kSuccess;

	connectedNodeObjs.clear();
	connectedPlgs.clear();

	MFnDependencyNode dgFn( nodeObj, &stat );
	if( !stat ) { mPrintError("Error@GetConnections(): Failed to get node object."); return false; }

	MPlug plug = dgFn.findPlug( plugName, &stat );
	if( !stat ) { mPrintError("Error@GetConnections(): Failed to get plug."); return false; }
	if( plug.isArray() )
	{
		if( logicalIndex ) {
			plug = plug.elementByLogicalIndex( index );
		} else {
			plug = plug.elementByPhysicalIndex( index );
		}
	}
	//if( !plug.isConnected() ) { return false; }

	plug.connectedTo( connectedPlgs, asDestination, !asDestination, &stat );
	const int numPlgs = (int)connectedPlgs.length();
	if( !stat || !numPlgs ) { mPrintError("Error@GetConnections(): Failed to find connections."); return false; }

	FOR( i, 0, numPlgs )
	{
		connectedNodeObjs.append( connectedPlgs[i].node() );
	}

	return true;
}

bool
GetConnections( const MObject& nodeObj, const MObject& plugObj, int index, bool logicalIndex, bool asDestination, MObjectArray& connectedNodeObjs, MPlugArray& connectedPlgs )
{
	MStatus stat = MS::kSuccess;

	connectedNodeObjs.clear();
	connectedPlgs.clear();

	MFnDependencyNode dgFn( nodeObj, &stat );
	if( !stat ) { mPrintError("Error@GetConnections(): Failed to get node object."); return false; }

	MPlug plug = dgFn.findPlug( plugObj, &stat );
	if( !stat ) { mPrintError("Error@GetConnections(): Failed to get plug."); return false; }
	if( plug.isArray() )
	{
		if( logicalIndex ) {
			plug = plug.elementByLogicalIndex( index );
		} else {
			plug = plug.elementByPhysicalIndex( index );
		}
	}
	//if( !plug.isConnected() ) { return false; }

	plug.connectedTo( connectedPlgs, asDestination, !asDestination, &stat );
	const int numPlgs = (int)connectedPlgs.length();
	if( !stat || !numPlgs ) { mPrintError("Error@GetConnections(): Failed to find connections."); return false; }

	FOR( i, 0, numPlgs )
	{
		connectedNodeObjs.append( connectedPlgs[i].node() );
	}

	return true;
}

MStringArray
GetConnectedNodeNames( const char* nodeName, const char* plugName, const char* typeName )
{
	MStringArray connectedNodes;
	MStringArray returnStrings;
	MStringArray splits;

	int isSource;
	MGlobal::executeCommand( MString( "connectionInfo -is " ) + nodeName + "." + plugName, isSource );

	if( isSource )
	{
		MGlobal::executeCommand( MString( "connectionInfo -dfs " ) + nodeName + "." + plugName, returnStrings );

		unsigned int j = 0;
		for( ; j<returnStrings.length(); ++j )
		{
			returnStrings[j].split( '.', splits );
			connectedNodes.append( splits[0] );
		}
	}

	splits.clear();

	int isDestination;
	MGlobal::executeCommand( MString( "connectionInfo -id " ) + nodeName + "." + plugName, isDestination );

	if( isDestination )
	{
		MGlobal::executeCommand( MString( "connectionInfo -sfd " ) + nodeName + "." + plugName, returnStrings );

		unsigned int j = 0;
		for( ; j<returnStrings.length(); ++j )
		{
			returnStrings[j].split( '.', splits );
			connectedNodes.append( splits[0] );
		}
	}

	if( typeName )
	{
		MStringArray items( connectedNodes );
		connectedNodes.clear();

		MString nodeType;

		unsigned int j = 0;
		for( ; j<items.length(); ++j )
		{
			MGlobal::executeCommand( MString( "nodeType " ) + items[j], nodeType );
			if( nodeType == typeName ) { connectedNodes.append( items[j] ); }
		}
	}

	return connectedNodes;
}

ZELOS_NAMESPACE_END

