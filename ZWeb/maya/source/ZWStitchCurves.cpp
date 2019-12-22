//--------------------//
// ZWStitchCurves.cpp //
//-------------------------------------------------------//
// author: Jaegwang Lim @ Dexter Studios                 //
// last update: 2017.09.06                               //
//-------------------------------------------------------//

#include <ZWStitchCurves.h>
#include <ZWebCurvesData.h>

bool ZWStitchCurves::enable = true;
std::map<std::string, ZWStitchCurves*> ZWStitchCurves::instances;

MTypeId ZWStitchCurves::id( 0x140002 );
MString ZWStitchCurves::name( "ZWStitchCurves" );

MObject ZWStitchCurves::inWebDataObj;
MObject ZWStitchCurves::inGuideCurveObj;
MObject ZWStitchCurves::inSourceCurveObj;
MObject ZWStitchCurves::outScalesObj;
MObject ZWStitchCurves::outRotatesObj;
MObject ZWStitchCurves::outTranslatesObj;
MObject ZWStitchCurves::outPivotObj;
MObject ZWStitchCurves::seedObj;
MObject ZWStitchCurves::txObj;
MObject ZWStitchCurves::tyObj;
MObject ZWStitchCurves::tzObj;
MObject ZWStitchCurves::rxObj;
MObject ZWStitchCurves::ryObj;
MObject ZWStitchCurves::rzObj;
MObject ZWStitchCurves::sxObj;
MObject ZWStitchCurves::syObj;
MObject ZWStitchCurves::szObj;
MObject ZWStitchCurves::pxObj;
MObject ZWStitchCurves::pyObj;
MObject ZWStitchCurves::pzObj;
MObject ZWStitchCurves::guideBinormalsObj;
MObject ZWStitchCurves::guideNormalsObj;

ZWStitchCurves::ZWStitchCurves()
{}

ZWStitchCurves::~ZWStitchCurves()
{}

void*
ZWStitchCurves::creator()
{
	return new ZWStitchCurves();
}

void
ZWStitchCurves::postConstructor()
{
	nodeObj = thisMObject();
	nodeFn.setObject( nodeObj );
	dagNodeFn.setObject( nodeObj );
}

MStatus
ZWStitchCurves::initialize()
{
    MRampAttribute       rAttr;
	MFnEnumAttribute     eAttr;
    MFnUnitAttribute     uAttr;
    MFnTypedAttribute    tAttr;
    MFnNumericAttribute  nAttr;
    MFnMatrixAttribute   mAttr;
	MFnCompoundAttribute cAttr;	

	CreateIntAttr( nAttr, seedObj, "seed", "seed", 0 );
	CHECK_MSTATUS( addAttribute( seedObj ) );

	CreateCustomAttr( tAttr, inWebDataObj, "inWebData", "inWebData", ZWebCurvesData );
	tAttr.setHidden(0); tAttr.setWritable(1); tAttr.setKeyable(1);
	CHECK_MSTATUS( addAttribute( inWebDataObj ) );

	CreateCurveAttr( tAttr, inGuideCurveObj, "inGuideCurve", "inGuideCurve" );
	tAttr.setHidden(0); tAttr.setKeyable(1);
	CHECK_MSTATUS( addAttribute( inGuideCurveObj ) );

	CreateCurveAttr( tAttr, inSourceCurveObj, "inSourceCurve", "inSourceCurve" );
	tAttr.setHidden(0); tAttr.setKeyable(1);
	CHECK_MSTATUS( addAttribute( inSourceCurveObj ) );

	CreateDoubleAttr( nAttr, txObj, "tx", "tx", 0.0 );
	CHECK_MSTATUS( addAttribute( txObj ) );

	CreateDoubleAttr( nAttr, tyObj, "ty", "ty", 0.0 );
	CHECK_MSTATUS( addAttribute( tyObj ) );

	CreateDoubleAttr( nAttr, tzObj, "tz", "tz", 0.0 );
	CHECK_MSTATUS( addAttribute( tzObj ) );

	CreateCompoundAttr( cAttr, outTranslatesObj, "outTranslates", "outTranslates" );
	cAttr.setHidden(0); cAttr.setWritable(0); cAttr.setArray(1);
	cAttr.addChild(txObj); cAttr.addChild(tyObj); cAttr.addChild(tzObj);
	CHECK_MSTATUS( addAttribute( outTranslatesObj ) );

	CreateDoubleAttr( nAttr, rxObj, "rx", "rx", 0.0 );
	CHECK_MSTATUS( addAttribute( rxObj ) );

	CreateDoubleAttr( nAttr, ryObj, "ry", "ry", 0.0 );
	CHECK_MSTATUS( addAttribute( ryObj ) );

	CreateDoubleAttr( nAttr, rzObj, "rz", "rz", 0.0 );
	CHECK_MSTATUS( addAttribute( rzObj ) );

	CreateCompoundAttr( cAttr, outRotatesObj, "outRotates", "outRotates" );
	cAttr.setHidden(0); cAttr.setWritable(0); cAttr.setArray(1);
	cAttr.addChild(rxObj); cAttr.addChild(ryObj); cAttr.addChild(rzObj);
	CHECK_MSTATUS( addAttribute( outRotatesObj ) );

	CreateDoubleAttr( nAttr, sxObj, "sx", "sx", 1.0 );
	CHECK_MSTATUS( addAttribute( sxObj ) );

	CreateDoubleAttr( nAttr, syObj, "sy", "sy", 1.0 );
	CHECK_MSTATUS( addAttribute( syObj ) );

	CreateDoubleAttr( nAttr, szObj, "sz", "sz", 1.0 );
	CHECK_MSTATUS( addAttribute( szObj ) );	

	CreateCompoundAttr( cAttr, outScalesObj, "outScales", "outScales" );
	cAttr.setHidden(0); cAttr.setWritable(0); cAttr.setArray(1);
	cAttr.addChild(sxObj); cAttr.addChild(syObj); cAttr.addChild(szObj);
	CHECK_MSTATUS( addAttribute( outScalesObj ) );


	CreateDoubleAttr( nAttr, pxObj, "px", "px", 0.0 );
	CHECK_MSTATUS( addAttribute( pxObj ) );

	CreateDoubleAttr( nAttr, pyObj, "py", "py", 0.0 );
	CHECK_MSTATUS( addAttribute( pyObj ) );

	CreateDoubleAttr( nAttr, pzObj, "pz", "pz", 0.0 );
	CHECK_MSTATUS( addAttribute( pzObj ) );	

	CreateCompoundAttr( cAttr, outPivotObj, "outPivot", "outPivot" );
	cAttr.addChild(pxObj); cAttr.addChild(pyObj); cAttr.addChild(pzObj);
	CHECK_MSTATUS( addAttribute( outPivotObj ) );

	attributeAffects( inWebDataObj    , outRotatesObj );
	attributeAffects( inGuideCurveObj , outRotatesObj );
	attributeAffects( inSourceCurveObj, outRotatesObj );

	attributeAffects( inWebDataObj    , outTranslatesObj );
	attributeAffects( inGuideCurveObj , outTranslatesObj );
	attributeAffects( inSourceCurveObj, outTranslatesObj );

	attributeAffects( inWebDataObj    , outScalesObj );
	attributeAffects( inGuideCurveObj , outScalesObj );
	attributeAffects( inSourceCurveObj, outScalesObj );

	attributeAffects( inWebDataObj    , outPivotObj );
	attributeAffects( inGuideCurveObj , outPivotObj );
	attributeAffects( inSourceCurveObj, outPivotObj );

	MDGMessage::addNodeRemovedCallback( ZWStitchCurves::destructor );
	return MS::kSuccess;
}

void
ZWStitchCurves::destructor( MObject& node, void* data )
{
	MStatus status = MS::kSuccess;
	MFnDependencyNode nodeFn( node, & status );

	if( nodeFn.typeId() == ZWStitchCurves::id )
	{
		ZWStitchCurves::instances.erase( nodeFn.name().asChar() );
	}	
}

MStatus
ZWStitchCurves::compute( const MPlug& plug, MDataBlock& block )
{
	//if( plug != outRotatesObj ) return MS::kUnknownParameter;	
	blockPtr = &block;
	nodeName = nodeFn.name();
	MThreadUtils::syncNumOpenMPThreads();

	const int count = block.outputArrayValue( outTranslatesObj ).elementCount();
	if( count == 0 || ZWStitchCurves::enable == false )
	{ // No connections.
		block.inputValue( inWebDataObj );
		block.inputValue( inGuideCurveObj );
		block.inputValue( inSourceCurveObj );

		block.setClean( plug );
		return MS::kSuccess;
	}	

    MObject guideCurveObj = block.inputValue( inGuideCurveObj ).asNurbsCurveTransformed();
	MObject sourceCurveObj = block.inputValue( inSourceCurveObj ).asNurbsCurve();

    MVector xaxis( 1.f, 0.f, 0.f );
    MVector yaxis( 0.f, 1.f, 0.f );
    MVector zaxis( 0.f, 0.f, 1.f );

	/*
	if( count != guideCurveNormals.length() )
	{
		guideCurveNormals.setLength( count );
		guideCurveBinormals.setLength( count );
	}
	*/

    if( guideCurveObj.isNull() == false && sourceCurveObj.isNull() == false )
    {
		MFnNurbsCurve guideCurve( guideCurveObj );
		MFnNurbsCurve sourceCurve( sourceCurveObj );

		float guideLength = guideCurve.length();
		int seed = block.inputValue( seedObj ).asInt();

		MPoint cObject, cWorld;
		sourceCurve.getCV( 0, cObject, MSpace::kObject );
		sourceCurve.getCV( 0, cWorld , MSpace::kWorld  );

		MDataHandle pivotHnd = block.outputValue( outPivotObj );
		pivotHnd.child( pxObj ).setDouble( cObject.x );
		pivotHnd.child( pyObj ).setDouble( cObject.y );
		pivotHnd.child( pzObj ).setDouble( cObject.z );

		float rFactor(0.f);
		float sFactor(0.f);

		const ZWebCurvesData* params = (ZWebCurvesData*)block.inputValue( inWebDataObj ).asPluginData();
		if( params )
		{
			rFactor = params->rotate;
			sFactor = params->scale;
		}

		float span = guideLength/(float)(count-1);
		span *= params->space;
	
        #pragma omp parallel for
        for( int n=0; n<count; ++n )
        {
			MArrayDataHandle outScalesHnd = block.outputArrayValue( outScalesObj );
			outScalesHnd.jumpToElement( n );
			MDataHandle scaleHnd = outScalesHnd.outputValue();

			MArrayDataHandle outRotatesHnd = block.outputArrayValue( outRotatesObj );
			outRotatesHnd.jumpToElement( n );
			MDataHandle rotateHnd = outRotatesHnd.outputValue();

			MArrayDataHandle outTranslatesHnd = block.outputArrayValue( outTranslatesObj );
			outTranslatesHnd.jumpToElement( n );
			MDataHandle translateHnd = outTranslatesHnd.outputValue();
           
			//const float d = ZRand( n*1351/*seed*/+354/*offset*/ )*guideLength;
            //const float param = guideCurve.findParamFromLength( d );
			
			const float r = (ZRand( n*1351/*seed*/+354/*offset*/ )-0.5f)*2.f;
			
			const float d = span*(float)n+(span*0.5f) + r*(span*0.5f);
			const float param = guideCurve.findParamFromLength( d );			

			MPoint pos;
			guideCurve.getPointAtParam( param, pos );

			MVector tan0( 0.f, 0.f, 1.f );
			MVector nor0( 0.f, 1.f, 0.f );

			/* z-Axis */
            MVector tan = (guideCurve.tangent( param )).normal();

			/* y-Axis */
			MVector nor = nor0;

			float angle = acos( (tan0*tan) / (tan0.length()*tan.length()) );
			if( ZAbs( angle ) > 1e-05 )
			{
				MVector axis = (tan0^tan) / (tan0.length()*tan.length());
				nor = nor0*cos(angle) + (axis*nor0)*axis*(1.0-cos(angle)) + (axis^nor0)*sin(angle);
				nor.normalize();
			}
	
			/* x- Axis */			
            const MVector bin = (nor^tan).normal();

			tan = (bin^nor).normal();

			//guideCurveNormals[n] = nor;
			//guideCurveBinormals[n] = bin;

			const float rxx = ZRand( n+seed*13351 ) * M_PI * 2.f * rFactor;
			const float ryy = ZRand( n+seed*23251 ) * M_PI * 2.f * rFactor;
			const float rzz = ZRand( n+seed*33151 ) * M_PI * 2.f * rFactor;
			const float rss = ZRand( n+seed*73351 ) * params->scaleVariance;

			float sss = sFactor - sFactor*rss;
			if( d > guideLength ) sss = 0.f;

            rotateHnd.child( rzObj ).setDouble( acos( tan*zaxis ) + rxx );
			rotateHnd.child( ryObj ).setDouble( acos( nor*yaxis ) + ryy );
            rotateHnd.child( rxObj ).setDouble( acos( bin*xaxis ) + rzz );

			scaleHnd.child( sxObj ).setDouble( sss );
            scaleHnd.child( syObj ).setDouble( sss );
            scaleHnd.child( szObj ).setDouble( sss );

			translateHnd.child( txObj ).setDouble( pos.x - cObject.x );
            translateHnd.child( tyObj ).setDouble( pos.y - cObject.y );
            translateHnd.child( tzObj ).setDouble( pos.z - cObject.z );
        }
    }
	else
	{
		//guideCurveBinormals.clear();
		//guideCurveNormals.clear();
	}

	block.setClean( plug );
	return MS::kSuccess;
}

MStatus ZWStitchCurves::connectionMade( const MPlug& plug, const MPlug& otherPlug, bool asSrc )
{
	if( isThe1stTime && plug == outPivotObj )
	{
		isThe1stTime = false;
		ZWStitchCurves::instances[MPxNode::name().asChar()] = this;
	}

	return MPxNode::connectionMade( plug, otherPlug, asSrc );
}

MStatus ZWStitchCurves::connectionBroken( const MPlug& plug, const MPlug& otherPlug, bool asSrc )
{
	return MPxNode::connectionBroken( plug, otherPlug, asSrc );
}

