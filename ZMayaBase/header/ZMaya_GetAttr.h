//-----------------//
// ZMaya_GetAttr.h //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#ifndef _ZMaya_GetAttr_h_
#define _ZMaya_GetAttr_h_

#include <ZelosBaseForMaya.h>

ZELOS_NAMESPACE_BEGIN

// bool, int, unsigned int, float, double, MString
template <typename T>
inline MStatus
ZGetAttr1( const MObject& nodeObj, const MObject& attrObj, T& val )
{
	MPlug plg( nodeObj, attrObj );
	MStatus stat = plg.getValue(val);
	if( !stat ) { mPrintError( "Error@ZGetAttr1(): Failed to get value of the plug." ); }
	return stat;
}

// ZGetAttr2: float2, ZFloat2
template <typename T>
inline MStatus
ZGetAttr2( const MObject& nodeObj, const MObject& attrObj, T& val )
{
	MPlug plg( nodeObj, attrObj );
	MStatus stat = MS::kSuccess;
	FOR( i, 0, 2 )
	{
		stat = plg.child(i).getValue( val[i] );
		if( !stat ) { mPrintError( "Error@ZGetAttr2(): Failed to get value of the plug." ); }
	}
	return stat;
}

// MPoint, MVector, MColor, float3, ZFloat3, etc.
template <typename T>
inline MStatus
ZGetAttr3( const MObject& nodeObj, const MObject& attrObj, T& val )
{
	MPlug plg( nodeObj, attrObj );
	MStatus stat = MS::kSuccess;
	FOR( i, 0, 3 )
	{
		stat = plg.child(i).getValue(val[i]);
		if( !stat ) { mPrintError( "Error@ZGetAttr3(): Failed to get value of the plug." ); }
	}
	return stat;
}

// ZFloat4, etc.
template <typename T>
inline MStatus
ZGetAttr4( const MObject& nodeObj, const MObject& attrObj, T& val )
{
	MPlug plg( nodeObj, attrObj );
	MStatus stat = MS::kSuccess;
	FOR( i, 0, 4 )
	{
		stat = plg.child(i).getValue(val[i]);
		if( !stat ) { mPrintError( "Error@ZGetAttr4(): Failed to get value of the plug." ); }
	}
	return stat;
}

inline MColor
AttrColor( const MObject& nodeObj, const MObject& attrObj )
{
	MPlug plug( nodeObj, attrObj );
	MObject data;
	plug.getValue( data );
	MFnNumericData numFn( data );
	ZFloat3 color;
	numFn.getData( color[0], color[1], color[2] );
	return MColor( color[0], color[1], color[2] );
}

ZELOS_NAMESPACE_END

#endif

