//-------------//
// AttrMap.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#include <ZelosBaseForMaya.h>

ZELOS_NAMESPACE_BEGIN

bool
GetMapValues( MDataBlock& block, const MObject& attrMapObj, MIntArray& mapValues )
{
	MStatus stat = MS::kSuccess;

	MFnIntArrayData attrMapData;
	MObject attrMapDataObj = block.inputValue( attrMapObj, &stat ).data();
	if( stat ) {
		attrMapData.setObject( attrMapDataObj );
	} else {
		mapValues.clear();
		mPrintError( "Error@GetMapValues(): Failed to get attrMapDataObj." );
		return false;
	}

	if( !attrMapData.length() ) // it is really the first time.
	{
		return false;
	}

	if( mapValues.length() != attrMapData.length() )
	{
		mapValues.setLength( attrMapData.length() );
	}

	attrMapData.copyTo( mapValues );

	return true;
}

bool
GetMapValues( MDataBlock& block, const MObject& attrMapObj, MDoubleArray& mapValues )
{
	MStatus stat = MS::kSuccess;

	MFnDoubleArrayData attrMapData;
	MObject attrMapDataObj = block.inputValue( attrMapObj, &stat ).data();
	if( stat ) {
		attrMapData.setObject( attrMapDataObj );
	} else {
		mapValues.clear();
		mPrintError( "Error@GetMapValues(): Failed to get attrMapDataObj." );
		return false;
	}

	if( !attrMapData.length() ) // it is really the first time.
	{
		return false;
	}

	if( mapValues.length() != attrMapData.length() )
	{
		mapValues.setLength( attrMapData.length() );
	}

	attrMapData.copyTo( mapValues );

	return true;
}

bool
GetMapValues( MDataBlock& block, const MObject& attrMapObj, MVectorArray& mapValues )
{
	MStatus stat = MS::kSuccess;

	MFnVectorArrayData attrMapData;
	MObject attrMapDataObj = block.inputValue( attrMapObj, &stat ).data();
	if( stat ) {
		attrMapData.setObject( attrMapDataObj );
	} else {
		mapValues.clear();
		mPrintError( "Error@GetMapValues(): Failed to get attrMapDataObj." );
		return false;
	}

	if( !attrMapData.length() ) // it is really the first time.
	{
		return false;
	}

	if( mapValues.length() != attrMapData.length() )
	{
		mapValues.setLength( attrMapData.length() );
	}

	attrMapData.copyTo( mapValues );

	return true;
}

bool
SetMapValues( MDataBlock& block, const MObject& attrMapObj, MIntArray& mapValues )
{
	MStatus stat = MS::kSuccess;

	MFnIntArrayData attrMapData;
	MObject attrMapDataObj = block.inputValue( attrMapObj, &stat ).data();
	if( stat ) {
		attrMapData.setObject( attrMapDataObj );
	} else {
		mapValues.clear();
		mPrintError( "Error@SetMapValues(): Failed to get attrMapDataObj." );
		return false;
	}

	const int numPoints = (int)mapValues.length();

	mapValues.setLength( numPoints );
	attrMapDataObj = attrMapData.create( mapValues );
	block.inputValue( attrMapObj ).set( attrMapDataObj );

	return true;
}

bool
SetMapValues( MDataBlock& block, const MObject& attrMapObj, MDoubleArray& mapValues )
{
	MStatus stat = MS::kSuccess;

	MFnDoubleArrayData attrMapData;
	MObject attrMapDataObj = block.inputValue( attrMapObj, &stat ).data();
	if( stat ) {
		attrMapData.setObject( attrMapDataObj );
	} else {
		mapValues.clear();
		mPrintError( "Error@SetMapValues(): Failed to get attrMapDataObj." );
		return false;
	}

	const int numPoints = (int)mapValues.length();

	mapValues.setLength( numPoints );
	attrMapDataObj = attrMapData.create( mapValues );
	block.inputValue( attrMapObj ).set( attrMapDataObj );

	return true;
}

bool
SetMapValues( MDataBlock& block, const MObject& attrMapObj, MVectorArray& mapValues )
{
	MStatus stat = MS::kSuccess;

	MFnVectorArrayData attrMapData;
	MObject attrMapDataObj = block.inputValue( attrMapObj, &stat ).data();
	if( stat ) {
		attrMapData.setObject( attrMapDataObj );
	} else {
		mapValues.clear();
		mPrintError( "Error@SetMapValues(): Failed to get attrMapDataObj." );
		return false;
	}

	const int numPoints = (int)mapValues.length();

	mapValues.setLength( numPoints );
	attrMapDataObj = attrMapData.create( mapValues );
	block.inputValue( attrMapObj ).set( attrMapDataObj );

	return true;
}

ZELOS_NAMESPACE_END

