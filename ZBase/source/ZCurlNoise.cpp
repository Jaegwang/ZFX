//----------------//
// ZCurlNoise.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
//         Jinhyuk Bae @ Dexter Studios                  //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZCurlNoise::ZCurlNoise()
: ZSimplexNoise()
{
	_delta = 1e-4f;
	_denom = 1 / (2*_delta);
}

ZVector
ZCurlNoise::velocity( float x, float y, float z, float t )
{
	const ZVector x0( ZSimplexNoise::vector(x-_delta,y,z,t) );
	const ZVector x1( ZSimplexNoise::vector(x+_delta,y,z,t) );
	const ZVector y0( ZSimplexNoise::vector(x,y-_delta,z,t) );
	const ZVector y1( ZSimplexNoise::vector(x,y+_delta,z,t) );
	const ZVector z0( ZSimplexNoise::vector(x,y,z-_delta,t) );
	const ZVector z1( ZSimplexNoise::vector(x,y,z+_delta,t) );

	return ZVector( ((y1.z-y0.z)-(z1.y-z0.y))*_denom,
                    ((z1.x-z0.x)-(x1.z-x0.z))*_denom,
                    ((x1.y-x0.y)-(y1.x-y0.x))*_denom );
}

ostream&
operator<<( ostream& os, const ZCurlNoise& object )
{
	os << "<ZCurlNoise>" << endl;
	os << " offset           : " << object.offset << endl;
	os << " Spatial Freqency : " << object.sFreq << endl;
	os << " Time    Freqency : " << object.tFreq << endl;
	os << " Scale			 : " << object.scale << endl;
	os << " Lift 			 : " << object.lift  << endl;
	
	return os;
}

ZELOS_NAMESPACE_END

