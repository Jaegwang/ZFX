//-----------//
// Joint.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#include <ZelosBaseForMaya.h>

ZELOS_NAMESPACE_BEGIN

bool
FindJoints( const MString& parent, MStringArray& jointList )
{
	MString nodeType;
	MGlobal::executeCommand( MString( "nodeType " ) + parent, nodeType );
	if( nodeType != "joint" )
	{
		mPrintError( MString( "There exists not joint type objects." ) );
		return false;
	}

	jointList.append( parent );

	MStringArray children;

	MGlobal::executeCommand( MString( "listRelatives -c " ) + parent, children );

	if( children.length() == 0 ) return true;

	uint j = 0;
	for( ; j<children.length(); ++j )
	{
		FindJoints( children[j], jointList );
	}

	return true;
}

ZELOS_NAMESPACE_END

