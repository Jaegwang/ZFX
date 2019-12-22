//---------------------------//
// ZWStitchCurvesControl.cpp //
//-------------------------------------------------------//
// author: Jaegwang Lim @ Dexter Studios                 //
// last update: 2017.08.22                               //
//-------------------------------------------------------//

#include <ZWStitchCurvesControl.h>
#include <ZWebCurvesData.h>

MTypeId ZWStitchCurvesControl::id( 0x140005 );
MString ZWStitchCurvesControl::name( "ZWStitchCurvesControl" );

MObject ZWStitchCurvesControl::inLinkObj;
MObject ZWStitchCurvesControl::rotateFactorObj;
MObject ZWStitchCurvesControl::rotateVarianceObj;
MObject ZWStitchCurvesControl::scaleFactorObj;
MObject ZWStitchCurvesControl::scaleVarianceObj;
MObject ZWStitchCurvesControl::spaceObj;
MObject ZWStitchCurvesControl::numStitchsObj;
MObject ZWStitchCurvesControl::outWebDataObj;

ZWStitchCurvesControl::ZWStitchCurvesControl()
{}

ZWStitchCurvesControl::~ZWStitchCurvesControl()
{}

void*
ZWStitchCurvesControl::creator()
{
	return new ZWStitchCurvesControl();
}

void
ZWStitchCurvesControl::postConstructor()
{
	nodeObj = thisMObject();
	nodeFn.setObject( nodeObj );
	dagNodeFn.setObject( nodeObj );

	nodeName = nodeFn.name();
}

MStatus
ZWStitchCurvesControl::initialize()
{
    MRampAttribute      rAttr;
	MFnEnumAttribute    eAttr;
    MFnUnitAttribute    uAttr;
    MFnTypedAttribute   tAttr;
    MFnNumericAttribute nAttr;
    MFnMatrixAttribute  mAttr;

	CreateIntAttr( nAttr, inLinkObj, "inLInk", "inLink", 0 );
	nAttr.setArray(1);
	CHECK_MSTATUS( addAttribute( inLinkObj ) );	

	CreateIntAttr( nAttr, numStitchsObj, "numStitchs", "numStitchs", 1 );
	CHECK_MSTATUS( addAttribute( numStitchsObj ) );

	CreateFloatAttr( nAttr, spaceObj, "space", "space", 1.0f );
	nAttr.setMin( 0.0f ); nAttr.setSoftMax( 2.f );
	CHECK_MSTATUS( addAttribute( spaceObj ) );	

	CreateFloatAttr( nAttr, scaleVarianceObj, "scaleVariance", "scaleVariance", 0.0f );
	nAttr.setMin( 0.0f ); nAttr.setMax( 1.f );
	CHECK_MSTATUS( addAttribute( scaleVarianceObj ) );

	CreateFloatAttr( nAttr, scaleFactorObj, "scaleFactor", "scaleFactor", 1.f );
	nAttr.setMin( 0.0f ); nAttr.setSoftMax( 5.f );
	CHECK_MSTATUS( addAttribute( scaleFactorObj ) );

	CreateFloatAttr( nAttr, rotateFactorObj, "rotateFactor", "rotateFactor", 0.f );
	nAttr.setMin( 0.0f ); nAttr.setMax( 1.f );
	CHECK_MSTATUS( addAttribute( rotateFactorObj ) );	

	CreateCustomAttr( tAttr, outWebDataObj, "outWebData", "outWebData", ZWebCurvesData );
	tAttr.setHidden(0); tAttr.setWritable(0); tAttr.setStorable(0);
	CHECK_MSTATUS( addAttribute( outWebDataObj ) );	

	attributeAffects( spaceObj         , outWebDataObj );
	attributeAffects( numStitchsObj    , outWebDataObj );
    attributeAffects( rotateFactorObj  , outWebDataObj );
	attributeAffects( scaleVarianceObj , outWebDataObj );
    attributeAffects( scaleFactorObj   , outWebDataObj );

	return MS::kSuccess;
}

MStatus
ZWStitchCurvesControl::compute( const MPlug& plug, MDataBlock& block )
{
	if( plug != outWebDataObj ) return MS::kUnknownParameter;

	blockPtr = &block;
	nodeName = nodeFn.name();
	MThreadUtils::syncNumOpenMPThreads();

	MFnPluginData dataFn;
	dataFn.create( ZWebCurvesData::id );
	ZWebCurvesData* newData = (ZWebCurvesData*)dataFn.data();
	{
		newData->numStrands = block.inputValue( numStitchsObj ).asInt();
        newData->scale = block.inputValue( scaleFactorObj ).asFloat();
		newData->scaleVariance = block.inputValue( scaleVarianceObj ).asFloat();
        newData->rotate = block.inputValue( rotateFactorObj ).asFloat();
		newData->space = block.inputValue( spaceObj ).asFloat();
	}
	MDataHandle outDataHnd = block.outputValue( outWebDataObj );
	outDataHnd.set( newData );

	block.setClean( plug );
	return MS::kSuccess;
}

MStatus ZWStitchCurvesControl::connectionMade( const MPlug& plug, const MPlug& otherPlug, bool asSrc )
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

