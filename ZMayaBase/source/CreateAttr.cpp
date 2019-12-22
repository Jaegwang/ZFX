//----------------//
// CreateAttr.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2016.11.07                               //
//-------------------------------------------------------//

#include <ZelosBaseForMaya.h>

ZELOS_NAMESPACE_BEGIN

bool
CreateTimeAttr( MFnUnitAttribute& uAttr, MObject& attrObj, const MString& longName, const MString& shortName, double initValue )
{
	MStatus status;
	attrObj = uAttr.create( longName, shortName, MFnUnitAttribute::kTime, initValue, &status );
	ZCheckError( status, MString("FAILED to create") + "'." + longName + "' attribute" );
	return false;
}

bool
CreateAngleAttr( MFnUnitAttribute& uAttr, MObject& attrObj, const MString& longName, const MString& shortName, double initValue )
{
	MStatus status;
	attrObj = uAttr.create( longName, shortName, MFnUnitAttribute::kAngle, initValue, &status );
	ZCheckError( status, MString("FAILED to create") + "'." + longName + "' attribute" );
	return false;
}

bool
CreateBoolAttr( MFnNumericAttribute& nAttr, MObject& attrObj, const MString& longName, const MString& shortName, bool initValue )
{
	MStatus status;
	attrObj = nAttr.create( longName, shortName, MFnNumericData::kBoolean, initValue, &status );
	ZCheckError( status, MString("FAILED to create") + "'." + longName + "' attribute" );
	return false;
}

bool
CreateIntAttr( MFnNumericAttribute& nAttr, MObject& attrObj, const MString& longName, const MString& shortName, int initValue )
{
	MStatus status;
	attrObj = nAttr.create( longName, shortName, MFnNumericData::kInt, initValue, &status );
	ZCheckError( status, MString("FAILED to create") + "'." + longName + "' attribute" );
	return false;
}

bool
CreateShortAttr( MFnNumericAttribute& nAttr, MObject& attrObj, const MString& longName, const MString& shortName, short initValue )
{
	MStatus status;
	attrObj = nAttr.create( longName, shortName, MFnNumericData::kShort, initValue, &status );
	ZCheckError( status, MString("FAILED to create") + "'." + longName + "' attribute" );
	return false;
}

bool
CreateFloatAttr( MFnNumericAttribute& nAttr, MObject& attrObj, const MString& longName, const MString& shortName, float initValue )
{
	MStatus status;
	attrObj = nAttr.create( longName, shortName, MFnNumericData::kFloat, initValue, &status );
	ZCheckError( status, MString("FAILED to create") + "'." + longName + "' attribute" );
	return false;
}

bool
CreateDoubleAttr( MFnNumericAttribute& nAttr, MObject& attrObj, const MString& longName, const MString& shortName, double initValue )
{
	MStatus status;
	attrObj = nAttr.create( longName, shortName, MFnNumericData::kDouble, initValue, &status );
	ZCheckError( status, MString("FAILED to create") + "'." + longName + "' attribute" );
	return false;
}

bool
CreatePointAttr( MFnNumericAttribute& nAttr, MObject& attrObj, const MString& longName, const MString& shortName )
{
	MStatus status;
	attrObj = nAttr.createPoint( longName, shortName, &status );
	ZCheckError( status, MString("FAILED to create") + "'." + longName + "' attribute" );
	return false;
}

bool
CreateI2Attr( MFnNumericAttribute& nAttr, MObject& attrObj, const MString& longName, const MString& shortName, int initValue )
{
	MStatus status;
	attrObj = nAttr.create( longName, shortName, MFnNumericData::k2Int, initValue, &status );
	ZCheckError( status, MString("FAILED to create") + "'." + longName + "' attribute" );
	return false;
}

bool
CreateI3Attr( MFnNumericAttribute& nAttr, MObject& attrObj, const MString& longName, const MString& shortName, int initValue )
{
	MStatus status;
	attrObj = nAttr.create( longName, shortName, MFnNumericData::k3Int, initValue, &status );
	ZCheckError( status, MString("FAILED to create") + "'." + longName + "' attribute" );
	return false;
}

bool
CreateF2Attr( MFnNumericAttribute& nAttr, MObject& attrObj, const MString& longName, const MString& shortName, float initValue )
{
	MStatus status;
	attrObj = nAttr.create( longName, shortName, MFnNumericData::k2Float, initValue, &status );
	ZCheckError( status, MString("FAILED to create") + "'." + longName + "' attribute" );
	return false;
}

bool
CreateF3Attr( MFnNumericAttribute& nAttr, MObject& attrObj, const MString& longName, const MString& shortName, float initValue )
{
	MStatus status;
	attrObj = nAttr.create( longName, shortName, MFnNumericData::k3Float, initValue, &status );
	ZCheckError( status, MString("FAILED to create") + "'." + longName + "' attribute" );
	return false;
}

bool
CreateD2Attr( MFnNumericAttribute& nAttr, MObject& attrObj, const MString& longName, const MString& shortName, double initValue )
{
	MStatus status;
	attrObj = nAttr.create( longName, shortName, MFnNumericData::k2Double, initValue, &status );
	ZCheckError( status, MString("FAILED to create") + "'." + longName + "' attribute" );
	return false;
}

bool
CreateD3Attr( MFnNumericAttribute& nAttr, MObject& attrObj, const MString& longName, const MString& shortName, double initValue )
{
	MStatus status;
	attrObj = nAttr.create( longName, shortName, MFnNumericData::k3Double, initValue, &status );
	ZCheckError( status, MString("FAILED to create") + "'." + longName + "' attribute" );
	return false;
}

bool
CreateD4Attr( MFnNumericAttribute& nAttr, MObject& attrObj, const MString& longName, const MString& shortName, double initValue )
{
	MStatus status;
	attrObj = nAttr.create( longName, shortName, MFnNumericData::k4Double, initValue, &status );
	ZCheckError( status, MString("FAILED to create") + "'." + longName + "' attribute" );
	return false;
}

bool
CreateEnumAttr( MFnEnumAttribute& eAttr, MObject& attrObj, const MString& longName, const MString& shortName, int initValue )
{
	MStatus status;
	attrObj = eAttr.create( longName, shortName, initValue );
	ZCheckError( status, MString("FAILED to create") + "'." + longName + "' attribute" );
	return false;
}

bool
CreateStringAttr( MFnTypedAttribute& tAttr, MObject& attrObj, const MString& longName, const MString& shortName, const MString str )
{
	MStatus status;
	MFnStringData strDataFn;
	MObject defaultStrObj( strDataFn.create( str ) );
	attrObj = tAttr.create( longName, shortName, MFnData::kString, defaultStrObj, &status );
	ZCheckError( status, MString("FAILED to create") + "'." + longName + "' attribute" );
	return false;
}

bool
CreateColorAttr( MFnNumericAttribute& nAttr, MObject& attrObj, const MString& longName, const MString& shortName )
{
	MStatus status;
	attrObj = nAttr.createColor( longName, shortName, &status );
	ZCheckError( status, MString("FAILED to create") + "'." + longName + "' attribute" );
	return false;
}

bool
CreateCurveAttr( MFnTypedAttribute& tAttr, MObject& attrObj, const MString& longName, const MString& shortName )
{
	MStatus status;
	attrObj = tAttr.create( longName, shortName, MFnData::kNurbsCurve, &status );
	ZCheckError( status, MString("FAILED to create") + "'." + longName + "' attribute" );
	return false;
}

bool
CreateCurveDataAttr( MFnTypedAttribute& tAttr, MObject& attrObj, const MString& longName, const MString& shortName )
{
	MStatus status;
	attrObj = tAttr.create( longName, shortName, MFnNurbsCurveData::kNurbsCurve, &status );
	ZCheckError( status, MString("FAILED to create") + "'." + longName + "' attribute" );
	return false;
}

bool
CreateMeshAttr( MFnTypedAttribute& tAttr, MObject& attrObj, const MString& longName, const MString& shortName )
{
	MStatus status;
	attrObj = tAttr.create( longName, shortName, MFnData::kMesh, &status );
	ZCheckError( status, MString("FAILED to create") + "'." + longName + "' attribute" );
	return false;
}

bool
CreateIArrayAttr( MFnTypedAttribute& tAttr, MObject& attrObj, const MString& longName, const MString& shortName )
{
	MStatus status;
	attrObj = tAttr.create( longName, shortName, MFnData::kIntArray, &status );
	ZCheckError( status, MString("FAILED to create") + "'." + longName + "' attribute" );
	return false;
}

bool
CreateFArrayAttr( MFnTypedAttribute& tAttr, MObject& attrObj, const MString& longName, const MString& shortName )
{
	MStatus status;
	attrObj = tAttr.create( longName, shortName, MFnData::kFloatArray, &status );
	ZCheckError( status, MString("FAILED to create") + "'." + longName + "' attribute" );
	return false;
}

bool
CreateDArrayAttr( MFnTypedAttribute& tAttr, MObject& attrObj, const MString& longName, const MString& shortName )
{
	MStatus status;
	attrObj = tAttr.create( longName, shortName, MFnData::kDoubleArray, &status );
	ZCheckError( status, MString("FAILED to create") + "'." + longName + "' attribute" );
	return false;
}

bool
CreatePArrayAttr( MFnTypedAttribute& tAttr, MObject& attrObj, const MString& longName, const MString& shortName )
{
	MStatus status;
	attrObj = tAttr.create( longName, shortName, MFnData::kPointArray, &status );
	ZCheckError( status, MString("FAILED to create") + "'." + longName + "' attribute" );
	return false;
}

bool
CreateVArrayAttr( MFnTypedAttribute& tAttr, MObject& attrObj, const MString& longName, const MString& shortName )
{
	MStatus status;
	attrObj = tAttr.create( longName, shortName, MFnData::kVectorArray, &status );
	ZCheckError( status, MString("FAILED to create") + "'." + longName + "' attribute" );
	return false;
}

bool
CreateMatrixAttr( MFnMatrixAttribute& mAttr, MObject& attrObj, const MString& longName, const MString& shortName )
{
	MStatus status;
	attrObj = mAttr.create( longName, shortName, MFnMatrixAttribute::kDouble, &status );
	ZCheckError( status, MString("FAILED to create") + "'." + longName + "' attribute" );
	return false;
}

bool
CreateSArrayAttr( MFnTypedAttribute& tAttr, MObject& attrObj, const MString& longName, const MString& shortName )
{
	MStatus status;
	attrObj = tAttr.create( longName, shortName, MFnData::kStringArray, attrObj, &status );
	ZCheckError( status, MString("FAILED to create") + "'." + longName + "' attribute" );
	return false;
}

bool
CreateCompoundAttr( MFnCompoundAttribute& cAttr, MObject& attrObj, const MString& longName, const MString& shortName )
{
	MStatus status;
	attrObj = cAttr.create( longName, shortName, &status );
	ZCheckError( status, MString("FAILED to create") + "'." + longName + "' attribute" );
	return false;
}

bool
CreateMessageAttr( MFnMessageAttribute& gAttr, MObject& attrObj, const MString& longName, const MString& shortName )
{
	MStatus status;
	attrObj = gAttr.create( longName, shortName, &status );
	ZCheckError( status, MString("FAILED to create") + "'." + longName + "' attribute" );
	return false;
}

bool
CreateVectorAttr( MFnNumericAttribute& nAttr, MObject& attrObj, const MString& longName, const MString& shortName, MObject& xObj, MObject& yObj, MObject& zObj )
{
	MStatus status;
	attrObj = nAttr.create( longName, shortName, xObj, yObj, zObj, &status );
	ZCheckError( status, MString("FAILED to create") + "'." + longName + "' attribute" );
	return false;
}

bool
Create3TupleAttr( MFnNumericAttribute& nAttr, MObject& attrObj, const MString& longName, const MString& shortName, MObject& xObj, MObject& yObj, MObject& zObj )
{
	MStatus status;
	attrObj = nAttr.create( longName, shortName, xObj, yObj, zObj, &status );
	ZCheckError( status, MString("FAILED to create") + "'." + longName + "' attribute" );
	return false;
}

bool
CreateCurveRampAttr( MRampAttribute& rAttr, MObject& attrObj, const MString& longName, const MString& shortName )
{
	MStatus status;
	attrObj = rAttr.createCurveRamp( longName, shortName, &status );
	ZCheckError( status, MString("FAILED to create") + "'." + longName + "' attribute" );
	return false;
}

bool
CreateColorRampAttr( MRampAttribute& rAttr, MObject& attrObj, const MString& longName, const MString& shortName )
{
	MStatus status;
	attrObj = rAttr.createColorRamp( longName, shortName, &status );
	ZCheckError( status, MString("FAILED to create") + "'." + longName + "' attribute" );
	return false;
}

ZELOS_NAMESPACE_END

