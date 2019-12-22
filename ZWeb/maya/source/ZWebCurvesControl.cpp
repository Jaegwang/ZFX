//-----------------------//
// ZWebCurvesControl.cpp //
//-------------------------------------------------------//
// author: Jaegwang Lim @ Dexter Studios                 //
// last update: 2017.08.16                               //
//-------------------------------------------------------//

#include <ZWebCurvesControl.h>
#include <ZWebCurvesData.h>

MTypeId ZWebCurvesControl::id( 0x140003 );
MString ZWebCurvesControl::name( "ZWebCurvesControl" );

MObject ZWebCurvesControl::inLinkObj;
MObject ZWebCurvesControl::numStrandsObj;
MObject ZWebCurvesControl::segmentScaleObj;
MObject ZWebCurvesControl::webRadiusObj;
MObject ZWebCurvesControl::webSwirlObj;
MObject ZWebCurvesControl::outWebDataObj;

// noise parameters
MObject ZWebCurvesControl::raNoiseFreqObj;
MObject ZWebCurvesControl::raNoiseOffsetObj;
MObject ZWebCurvesControl::raNoiseScaleObj;
MObject ZWebCurvesControl::raNoiseOctavesObj;


ZWebCurvesControl::ZWebCurvesControl()
{}

ZWebCurvesControl::~ZWebCurvesControl()
{}

void*
ZWebCurvesControl::creator()
{
	return new ZWebCurvesControl();
}

void
ZWebCurvesControl::postConstructor()
{
	nodeObj = thisMObject();
	nodeFn.setObject( nodeObj );
	dagNodeFn.setObject( nodeObj );

	nodeName = nodeFn.name();
}

MStatus
ZWebCurvesControl::initialize()
{
    MRampAttribute      rAttr;
	MFnEnumAttribute    eAttr;
    MFnUnitAttribute    uAttr;
    MFnTypedAttribute   tAttr;
    MFnNumericAttribute nAttr;
    MFnMatrixAttribute  mAttr;

	CreateIntAttr( nAttr, inLinkObj, "inLink", "inLink", 0 );
	nAttr.setArray(1);
	CHECK_MSTATUS( addAttribute( inLinkObj ) );

	CreateIntAttr( nAttr, numStrandsObj, "numStrands", "numStrands", 1 );
	CHECK_MSTATUS( addAttribute( numStrandsObj ) );

	CreateFloatAttr( nAttr, segmentScaleObj, "segmentScale", "segmentScale", 1.f );
	nAttr.setMin( 0.0f ); nAttr.setSoftMax( 2.f );
	CHECK_MSTATUS( addAttribute( segmentScaleObj ) );

	CreateFloatAttr( nAttr, webRadiusObj, "radius", "radius", 1.f );
	nAttr.setMin( 0.f ); nAttr.setSoftMax( 2.f );
	CHECK_MSTATUS( addAttribute( webRadiusObj ) );

	CreateFloatAttr( nAttr, webSwirlObj, "swirl", "swril", 0.f );
	nAttr.setMin( 0.0f ); nAttr.setSoftMax( 2.f );
	CHECK_MSTATUS( addAttribute( webSwirlObj ) );

	CreateFloatAttr( nAttr, raNoiseFreqObj, "raNoiseFreq", "raNoiseFreq", 1.f );
	nAttr.setMin( 0.0f ); nAttr.setSoftMax( 3.f );
	CHECK_MSTATUS( addAttribute( raNoiseFreqObj ) );

	CreateFloatAttr( nAttr, raNoiseOffsetObj, "raNoiseOffset", "raNoiseOffset", 1.f );
	nAttr.setSoftMin( 0.0f ); nAttr.setSoftMax( 3.f );
	CHECK_MSTATUS( addAttribute( raNoiseOffsetObj ) );

	CreateFloatAttr( nAttr, raNoiseScaleObj, "raNoiseScale", "raNoiseScale", 0.f );
	nAttr.setMin( 0.0f ); nAttr.setSoftMax( 3.f );
	CHECK_MSTATUS( addAttribute( raNoiseScaleObj ) );

	CreateIntAttr( nAttr, raNoiseOctavesObj, "raNoiseOctaves", "raNoiseOctaves", 4 );
	CHECK_MSTATUS( addAttribute( raNoiseOctavesObj ) );

	CreateCustomAttr( tAttr, outWebDataObj, "outWebData", "outWebData", ZWebCurvesData );
	tAttr.setHidden(0); tAttr.setWritable(0); tAttr.setStorable(0);
	CHECK_MSTATUS( addAttribute( outWebDataObj ) );	

	attributeAffects( numStrandsObj    , outWebDataObj );
	attributeAffects( segmentScaleObj  , outWebDataObj );
	attributeAffects( webRadiusObj     , outWebDataObj );
	attributeAffects( webSwirlObj      , outWebDataObj );
	attributeAffects( raNoiseFreqObj   , outWebDataObj );
	attributeAffects( raNoiseOffsetObj , outWebDataObj );
	attributeAffects( raNoiseScaleObj  , outWebDataObj );
	attributeAffects( raNoiseOctavesObj, outWebDataObj );

	return MS::kSuccess;
}

MStatus
ZWebCurvesControl::compute( const MPlug& plug, MDataBlock& block )
{
	if( plug != outWebDataObj ) return MS::kUnknownParameter;

	blockPtr = &block;
	nodeName = nodeFn.name();
	MThreadUtils::syncNumOpenMPThreads();

	MFnPluginData dataFn;
	dataFn.create( ZWebCurvesData::id );
	ZWebCurvesData* newData = (ZWebCurvesData*)dataFn.data();
	{
		newData->numStrands = block.inputValue( numStrandsObj ).asInt();
		newData->segScale = block.inputValue( segmentScaleObj ).asFloat();
		newData->radius = block.inputValue( webRadiusObj ).asFloat();
		newData->swirl = block.inputValue( webSwirlObj ).asFloat();
		newData->noiseScale = block.inputValue( raNoiseScaleObj ).asFloat();
		newData->noiseFrequency = block.inputValue( raNoiseFreqObj ).asFloat();
		newData->noiseOffset = block.inputValue( raNoiseOffsetObj ).asFloat();
		newData->noiseOctaves = block.inputValue( raNoiseOctavesObj ).asInt();
	}
	MDataHandle outDataHnd = block.outputValue( outWebDataObj );
	outDataHnd.set( newData );

	block.setClean( plug );
	return MS::kSuccess;
}

MStatus ZWebCurvesControl::connectionMade( const MPlug& plug, const MPlug& otherPlug, bool asSrc )
{
	if( isThe1stTime && plug == outWebDataObj )
	{
		isThe1stTime = false;

		nodeName = nodeFn.name();

		int check(0);
		MGlobal::executeCommand( "exists rmanGetAttrName", check );

		if( check )
		{
			std::stringstream ss;
			ss << "rmanAddAttr " << nodeName << " `rmanGetAttrName customShadingGroup` " << "\"\"";

			MGlobal::executeCommand( ss.str().c_str() );
			ss.str("");

			ss << "rmanAddAttr " << nodeName << " `rmanGetAttrName curveBaseWidth` " << "\"\"";
			MGlobal::executeCommand( ss.str().c_str() );
			ss.str("");

			ss << "rmanAddAttr " << nodeName << " `rmanGetAttrName curveTipWidth` " << "\"\"";
			MGlobal::executeCommand( ss.str().c_str() );
			ss.str("");

			ss << "rmanAddAttr " << nodeName << " `rmanGetAttrName dice:hair` " << "0";
			MGlobal::executeCommand( ss.str().c_str() );
			ss.str("");

			ss << "rmanAddAttr " << nodeName << " `rmanGetAttrName dice:roundcurve` " << "1";
			MGlobal::executeCommand( ss.str().c_str() );
			ss.str("");
		}
	}

	return MPxNode::connectionMade( plug, otherPlug, asSrc );
}

