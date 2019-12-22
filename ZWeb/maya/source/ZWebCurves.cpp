//----------------//
// ZWebCurves.cpp //
//-------------------------------------------------------//
// author: Jaegwang Lim @ Dexter Studios                 //
// last update: 2017.09.06                               //
//-------------------------------------------------------//

#include <ZWebCurves.h>
#include <ZWebCurvesData.h>
#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>

bool ZWebCurves::enable = true;
std::map<std::string, ZWebCurves*> ZWebCurves::instances;

MTypeId ZWebCurves::id( 0x140001 );
MString ZWebCurves::name( "ZWebCurves" );

MObject ZWebCurves::inWebDataObj;
MObject ZWebCurves::inGuideCurveObj;
MObject ZWebCurves::seedObj;
MObject ZWebCurves::outWebCurvesObj;
MObject ZWebCurves::rampSwirlObj;
MObject ZWebCurves::rampNoiseObj;
MObject ZWebCurves::guidePointsObj;
MObject ZWebCurves::guideBinormalsObj;
MObject ZWebCurves::guideNormalsObj;

ZWebCurves::ZWebCurves()
{}

ZWebCurves::~ZWebCurves()
{}

void*
ZWebCurves::creator()
{
	return new ZWebCurves();
}

void
ZWebCurves::postConstructor()
{
	nodeObj = thisMObject();
	nodeFn.setObject( nodeObj );
	dagNodeFn.setObject( nodeObj );

	nodeName = nodeFn.name();
}

MStatus
ZWebCurves::initialize()
{
    MRampAttribute      rAttr;
	MFnEnumAttribute    eAttr;
    MFnUnitAttribute    uAttr;
    MFnTypedAttribute   tAttr;
    MFnNumericAttribute nAttr;
    MFnMatrixAttribute  mAttr;

	CreateIntAttr( nAttr, seedObj, "seed", "seed", 0 );
	CHECK_MSTATUS( addAttribute( seedObj ) );	

	CreateCustomAttr( tAttr, inWebDataObj, "inWebData", "inWebData", ZWebCurvesData );
	tAttr.setHidden(0); tAttr.setWritable(1); tAttr.setKeyable(1);
	CHECK_MSTATUS( addAttribute( inWebDataObj ) );	

	CreateCurveAttr( tAttr, inGuideCurveObj, "inGuideCurve", "inGuideCurve" );
	tAttr.setHidden(0); tAttr.setKeyable(1);
	CHECK_MSTATUS( addAttribute( inGuideCurveObj ) );

	CreateCurveAttr( tAttr, outWebCurvesObj, "outWebCurves", "outWebCurves" );
	tAttr.setHidden(0); tAttr.setWritable(0); tAttr.setArray(1);
	CHECK_MSTATUS( addAttribute( outWebCurvesObj ) );

	rampSwirlObj = MRampAttribute::createCurveRamp( "rampSwirl", "rampSwirl" );
	CHECK_MSTATUS( addAttribute( rampSwirlObj ) );

	rampNoiseObj = MRampAttribute::createCurveRamp( "rampNoise", "rampNoise" );
	CHECK_MSTATUS( addAttribute( rampNoiseObj ) );

	CreatePArrayAttr( tAttr, guidePointsObj, "guidePoints", "guidePoints" );
	CHECK_MSTATUS( addAttribute( guidePointsObj ) );

	CreateVArrayAttr( tAttr, guideBinormalsObj, "guideBinormals", "guideBinormals" );
	CHECK_MSTATUS( addAttribute( guideBinormalsObj ) );

	CreateVArrayAttr( tAttr, guideNormalsObj, "guideNormals", "guideNormals" );
	CHECK_MSTATUS( addAttribute( guideNormalsObj ) );

	attributeAffects( inWebDataObj    , outWebCurvesObj );
	attributeAffects( inGuideCurveObj , outWebCurvesObj );
	attributeAffects( seedObj         , outWebCurvesObj );
	attributeAffects( rampSwirlObj    , outWebCurvesObj );
	attributeAffects( rampNoiseObj    , outWebCurvesObj );

	MDGMessage::addNodeRemovedCallback( ZWebCurves::destructor );
	return MS::kSuccess;
}

void
ZWebCurves::destructor( MObject& node, void* data )
{
	MStatus status = MS::kSuccess;
	MFnDependencyNode nodeFn( node, & status );

	if( nodeFn.typeId() == ZWebCurves::id )
	{
		ZWebCurves::instances.erase( nodeFn.name().asChar() );
	}	
}

MStatus
ZWebCurves::compute( const MPlug& plug, MDataBlock& block )
{
	if( plug != outWebCurvesObj ) return MS::kUnknownParameter;

	blockPtr = &block;
	nodeName = nodeFn.name();
	MThreadUtils::syncNumOpenMPThreads();

	MArrayDataHandle outCurvesHnd = block.outputArrayValue( outWebCurvesObj );
	const int count = outCurvesHnd.elementCount();

	if( count == 0 || ZWebCurves::enable == false )
	{ // No connections.
		block.inputValue( inGuideCurveObj );
		block.inputValue( inWebDataObj );

		block.setClean( plug );
		return MS::kSuccess;
	}

	MObject guideCurveObj = block.inputValue( inGuideCurveObj ).asNurbsCurveTransformed();

	if( guideCurveObj.isNull() == true )
	{
		guideCurvePoints.clear();
		guideCurveBinormals.clear();
		guideCurveNormals.clear();
	}

	if( guideCurveObj.isNull() == false )
	{
		// Get web parameters
		float segScale(1.f);
		float radius(1.f);
		float swirl(0.f);

		float xFreq(0.f), xOffset(0.f), xScale(1.f);
		int   xOctaves(1);

		MFnNurbsCurve guideCurve( guideCurveObj );
		MRampAttribute rampSwirl( nodeObj, rampSwirlObj );
		MRampAttribute rampNoise( nodeObj, rampNoiseObj );

		int seed = block.inputValue( seedObj ).asInt();

		double guideLength = guideCurve.length();

		const ZWebCurvesData* params = (ZWebCurvesData*)block.inputValue( inWebDataObj ).asPluginData();
		if( params )
		{
			segScale = params->segScale;
			swirl = params->swirl;
			radius = params->radius;

			xFreq = params->noiseFrequency;
			xOffset = params->noiseOffset;
			xScale = params->noiseScale;
			xOctaves = params->noiseOctaves;
			xOctaves = std::max( xOctaves, 1 );
		}

		int nCV = guideCurve.numCVs()*segScale;
		double step = guideLength / float( nCV-1 );

		if( nCV != guideCurvePoints.length() )
		{
			guideCurvePoints.setLength( nCV );
			guideCurveBinormals.setLength( nCV );
			guideCurveNormals.setLength( nCV );
		}

		MVector tan = guideCurve.tangent( 0.01f ).normal();
		MVector nor;
		
		{
			MVector tan0( 0.f, 0.f, 1.f );
			MVector nor0( 0.f, 1.f, 0.f );

			if( guideCurveNormals[0].length() > 1e-5 )
			{
				nor0 = guideCurveNormals[0];
				tan0 = (guideCurveBinormals[0]^guideCurveNormals[0]).normal();
			}

			float angle = acos( (tan0*tan) / (tan0.length()*tan.length()) );

			if( ZAbs( angle ) > 1e-05 )
			{
				MVector axis = (tan0^tan) / (tan0.length()*tan.length());
				nor = nor0*cos(angle) + (axis*nor0)*axis*(1.0-cos(angle)) + (axis^nor0)*sin(angle);
				nor.normalize();
			}
			else
			{
				nor = nor0;
			}
		}

		for( int n=0; n<nCV; ++n )
		{
			double d = guideCurve.findParamFromLength( (float)n*step );

			MPoint pos;
			guideCurve.getPointAtParam( d, pos );

			MVector tan0 = tan;
			tan = guideCurve.tangent( d ).normal();
	
			float angle = acos( (tan0*tan) / (tan0.length()*tan.length()) );

			if( ZAbs(angle) > 1e-5 )
			{
				MVector axis = (tan0^tan) / (tan0.length()*tan.length());
				
				nor = nor*cos(angle) + (axis*nor)*axis*(1.0-cos(angle)) + (axis^nor)*sin(angle);

				MVector bin = (nor^tan).normal();
				nor = (tan^bin).normal();

				guideCurvePoints[n] = pos;
				guideCurveNormals[n] = nor;				
				guideCurveBinormals[n] = bin;
			}
			else
			{
				guideCurvePoints[n] = pos;
				guideCurveNormals[n] = nor;				
				guideCurveBinormals[n] = (nor^tan).normal();
			}
		}

		cvPoints.resize( count );

		#pragma omp parallel for
		for( int n=0; n<count; ++n )
		{		
			MArrayDataHandle outCurvesHnd = block.outputArrayValue( outWebCurvesObj );
			outCurvesHnd.jumpToElement( n );			

			MPointArray& cvs = cvPoints[n];
			if( cvs.length() != nCV ) cvs.setLength( nCV );

			float sx = (ZRand( n+25612*seed )-0.5f)*2.f;
			float sy = (ZRand( n+63188*seed )-0.5f)*2.f;
			float sr = (ZRand( n+13188*seed )-0.5f)*2.f;
			float of = (ZRand( n+53188*seed )-0.5f)*2.f;

			double rot = ZRand( n+44188*seed )*360.f;

			for( int i=0; i<nCV; ++i )
			{
				const float s = (float)i*step;
				const float a = s / guideLength;

				float ex_s(1.f);	
				rampSwirl.getValueAtPosition( a, ex_s );
				ex_s = std::max( 0.f, ex_s );

				// noise compute
				float noise_rad( 0.f );
				float ofreq  = xFreq;
				float oScale = xScale; 
				for( int n=0; n<xOctaves; ++n )
				{
					noise_rad += glm::perlin( glm::vec4( a*ofreq + xOffset+of, 0.f,0.f,0.f ) ) * oScale;
					//noise_rad += radiusNoise.pureValue( a*ofreq + xOffset+of ) * oScale;
					ofreq  *= 2.f;
					oScale *= 0.5f;
				}

				if( noise_rad < -1.f ) noise_rad = -1.f;

				float ex_n(1.f);
				rampNoise.getValueAtPosition( a, ex_n );

				noise_rad *= ex_n;

				float rx = sx * ( radius*noise_rad + radius );
				float ry = sy * ( radius*noise_rad + radius );

				const MVector& pos = guideCurvePoints[i];
				const MVector& nor = guideCurveNormals[i];
				const MVector& bin = guideCurveBinormals[i];

				rot += sr * swirl * ex_s;
				cvs[i] = pos + nor*sin(rot)*rx + bin*cos(rot)*ry;
			}

			MFnNurbsCurve newCurveFn;
			MFnNurbsCurveData dataCreator;
			MObject newCurveData = dataCreator.create();
			newCurveFn.createWithEditPoints( cvs, 3, MFnNurbsCurve::kOpen, false, true, true, newCurveData );

			outCurvesHnd.outputValue().set( newCurveData );
		}		
	}

	block.setClean( plug );
	return MS::kSuccess;
}

MStatus ZWebCurves::connectionMade( const MPlug& plug, const MPlug& otherPlug, bool asSrc )
{
	if( isThe1stTime && plug == outWebCurvesObj )
	{
		isThe1stTime = false;
		ZWebCurves::instances[MPxNode::name().asChar()] = this;

		MFloatArray positions;
		MFloatArray values;
		MIntArray interps;

		positions.setLength(1); positions[0] = 0.5f;
		values.setLength(1); values[0] = 1.f;
		interps.setLength(1); interps[0] = MRampAttribute::kSmooth;
		
		MRampAttribute rampSwirl( nodeObj, rampSwirlObj );
		if( rampSwirl.getNumEntries() == 0 )
		{
			rampSwirl.addEntries( positions, values, interps );
		}
		rampSwirl.setValueAtIndex( 1.f, 0 );
	
		MRampAttribute rampNoise( nodeObj, rampNoiseObj );
		if( rampNoise.getNumEntries() == 0 )
		{
			rampNoise.addEntries( positions, values, interps );
		}
		rampNoise.setValueAtIndex( 1.f, 0 );
	}

	return MPxNode::connectionMade( plug, otherPlug, asSrc );
}

MStatus ZWebCurves::connectionBroken( const MPlug& plug, const MPlug& otherPlug, bool asSrc )
{
	return MPxNode::connectionBroken( plug, otherPlug, asSrc );
}

