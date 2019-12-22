//--------------------//
// ZMaya_CreateAttr.h //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2016.11.07                               //
//-------------------------------------------------------//

#ifndef _ZMaya_CreateAttr_h_
#define _ZMaya_CreateAttr_h_

#include <ZelosBaseForMaya.h>

ZELOS_NAMESPACE_BEGIN

bool CreateTimeAttr      ( MFnUnitAttribute&     uAttr, MObject& attrObj, const MString& longName, const MString& shortName, double initValue=0 );
bool CreateAngleAttr     ( MFnUnitAttribute&     uAttr, MObject& attrObj, const MString& longName, const MString& shortName, double initValue=0 );
bool CreateBoolAttr      ( MFnNumericAttribute&  nAttr, MObject& attrObj, const MString& longName, const MString& shortName, bool   initValue=0 );
bool CreateShortAttr     ( MFnNumericAttribute&  nAttr, MObject& attrObj, const MString& longName, const MString& shortName, short  initValue=0 );
bool CreateIntAttr       ( MFnNumericAttribute&  nAttr, MObject& attrObj, const MString& longName, const MString& shortName, int    initValue=0 );
bool CreateFloatAttr     ( MFnNumericAttribute&  nAttr, MObject& attrObj, const MString& longName, const MString& shortName, float  initValue=0 );
bool CreateDoubleAttr    ( MFnNumericAttribute&  nAttr, MObject& attrObj, const MString& longName, const MString& shortName, double initValue=0 );
bool CreatePointAttr     ( MFnNumericAttribute&  nAttr, MObject& attrObj, const MString& longName, const MString& shortName );
bool CreateI2Attr        ( MFnNumericAttribute&  nAttr, MObject& attrObj, const MString& longName, const MString& shortName, int    initValue=0 );
bool CreateI3Attr        ( MFnNumericAttribute&  nAttr, MObject& attrObj, const MString& longName, const MString& shortName, int    initValue=0 );
bool CreateF2Attr        ( MFnNumericAttribute&  nAttr, MObject& attrObj, const MString& longName, const MString& shortName, float  initValue=0 );
bool CreateF3Attr        ( MFnNumericAttribute&  nAttr, MObject& attrObj, const MString& longName, const MString& shortName, float  initValue=0 );
bool CreateD2Attr        ( MFnNumericAttribute&  nAttr, MObject& attrObj, const MString& longName, const MString& shortName, double initValue=0 );
bool CreateD3Attr        ( MFnNumericAttribute&  nAttr, MObject& attrObj, const MString& longName, const MString& shortName, double initValue=0 );
bool CreateD4Attr        ( MFnNumericAttribute&  nAttr, MObject& attrObj, const MString& longName, const MString& shortName, double initValue=0 );
bool CreateEnumAttr      ( MFnEnumAttribute&     eAttr, MObject& attrObj, const MString& longName, const MString& shortName, int    initValue=0 );
bool CreateStringAttr    ( MFnTypedAttribute&    tAttr, MObject& attrObj, const MString& longName, const MString& shortName, const MString str="" );
bool CreateColorAttr     ( MFnNumericAttribute&  nAttr, MObject& attrObj, const MString& longName, const MString& shortName );
bool CreateCurveAttr     ( MFnTypedAttribute&    tAttr, MObject& attrObj, const MString& longName, const MString& shortName );
bool CreateCurveDataAttr ( MFnTypedAttribute&    tAttr, MObject& attrObj, const MString& longName, const MString& shortName );
bool CreateMeshAttr      ( MFnTypedAttribute&    tAttr, MObject& attrObj, const MString& longName, const MString& shortName );
bool CreateIArrayAttr    ( MFnTypedAttribute&    tAttr, MObject& attrObj, const MString& longName, const MString& shortName );
bool CreateFArrayAttr    ( MFnTypedAttribute&    tAttr, MObject& attrObj, const MString& longName, const MString& shortName );
bool CreateDArrayAttr    ( MFnTypedAttribute&    tAttr, MObject& attrObj, const MString& longName, const MString& shortName );
bool CreateSArrayAttr    ( MFnTypedAttribute&    tAttr, MObject& attrObj, const MString& longName, const MString& shortName );
bool CreatePArrayAttr    ( MFnTypedAttribute&    tAttr, MObject& attrObj, const MString& longName, const MString& shortName );
bool CreateVArrayAttr    ( MFnTypedAttribute&    tAttr, MObject& attrObj, const MString& longName, const MString& shortName );
bool CreateMatrixAttr    ( MFnMatrixAttribute&   mAttr, MObject& attrObj, const MString& longName, const MString& shortName );
bool CreateCompoundAttr  ( MFnCompoundAttribute& cAttr, MObject& attrObj, const MString& longName, const MString& shortName );
bool CreateMessageAttr   ( MFnMessageAttribute&  gAttr, MObject& attrObj, const MString& longName, const MString& shortName );
bool CreateVectorAttr    ( MFnNumericAttribute&  nAttr, MObject& attrObj, const MString& longName, const MString& shortName, MObject& xObj, MObject& yObj, MObject& zObj );
bool Create3TupleAttr    ( MFnNumericAttribute&  nAttr, MObject& attrObj, const MString& longName, const MString& shortName, MObject& xObj, MObject& yObj, MObject& zObj );
bool CreateCurveRampAttr ( MRampAttribute&       rAttr, MObject& attrObj, const MString& longName, const MString& shortName );
bool CreateColorRampAttr ( MRampAttribute&       rAttr, MObject& attrObj, const MString& longName, const MString& shortName );

// data
#define CreateCustomAttr( tAttr, attrObj, longName, shortName, class )\
{\
	MStatus s = MS::kSuccess;\
	attrObj = tAttr.create( longName, shortName, class::id, MObject::kNullObj );\
}

ZELOS_NAMESPACE_END

#endif

