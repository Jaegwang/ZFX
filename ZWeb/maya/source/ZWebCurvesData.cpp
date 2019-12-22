//--------------------//
// ZWebCurvesData.cpp //
//-------------------------------------------------------//
// author: Jaegwawng Lim @ Dexter Studios                //
// last update: 2017.08.16                               //
//-------------------------------------------------------//

#include <ZWebCurvesData.h>

MTypeId ZWebCurvesData::id( 0x140004 );
MString ZWebCurvesData::typeName( "ZWebCurvesData" );

ZWebCurvesData::ZWebCurvesData() : MPxData()
{
}

void* 
ZWebCurvesData::creator()
{
	return new ZWebCurvesData();
}

MTypeId 
ZWebCurvesData::typeId() const
{
	return id;
}

MString 
ZWebCurvesData::name() const
{
	return typeName;
}

void 
ZWebCurvesData::copy( const MPxData& other )
{
	const ZWebCurvesData& data = (const ZWebCurvesData&)other;

    numStrands = data.numStrands;
    numScale = data.numScale;
    segScale = data.segScale;
    radius = data.radius;
    swirl = data.swirl;
    noiseScale = data.noiseScale;
    noiseFrequency = data.noiseFrequency;
    noiseOffset = data.noiseOffset;
    noiseOctaves = data.noiseOctaves;
    scale = data.scale;
    rotate = data.rotate;
    translate = data.translate;
    space = data.space;
    scaleVariance = data.scaleVariance;
    rotateVariance = data.rotateVariance;
}

