//----------//
// Plug.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.12.26                               //
//-------------------------------------------------------//

#include <ZelosBaseForMaya.h>

ZELOS_NAMESPACE_BEGIN

bool
GetPlugByName( const MString& objName, const MString& attrName, MPlug& plg )
{
	MObject object = NodeNameToMObject( objName );

	if( object != MObject::kNullObj )
	{
		MFnDependencyNode nodeFn( object );

		plg = nodeFn.findPlug( attrName );

		return true;
	}

	return false;
}

ZELOS_NAMESPACE_END

