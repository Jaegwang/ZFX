//--------------------//
// ZImageMapUtils.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

#include <half.h>
#include <OpenImageIO/imageio.h>
OIIO_NAMESPACE_USING

ZELOS_NAMESPACE_BEGIN

bool 
CalculateNormalMap( ZImageMap& nrmMap, const ZImageMap& hgtMap, float strength, bool useOpenMP )
{
	const int wSize = hgtMap.width();
	const int hSize = hgtMap.height();
	const int wSize_1 = wSize-1;
	const int hSize_1 = hSize-1;

	if( nrmMap.width() != wSize || nrmMap.height() != hSize )
	{
		cout << "Error@CalculateNormalMap():: Different image resolution." << endl;
		return false;
	}

	#pragma omp parallel for if( useOpenMP )
	PER_EACH_PIXEL( nrmMap )

		const int ci = ZClamp( i  ,  0, wSize_1 );
		const int cj = ZClamp( j  ,  0, hSize_1 );
		const int i0 = ZClamp( ci+1, 0, wSize_1 );
		const int i1 = ZClamp( ci-1, 0, wSize_1 );
		const int j0 = ZClamp( cj-1, 0, hSize_1 );
		const int j1 = ZClamp( cj+1, 0, hSize_1 );

		const float tl = hgtMap.intensity(i0,j1);
		const float t  = hgtMap.intensity(ci,j1);
		const float tr = hgtMap.intensity(i1,j1);
		const float l  = hgtMap.intensity(i0,cj);
		const float r  = hgtMap.intensity(i1,cj);
		const float bl = hgtMap.intensity(i0,j0);
		const float b  = hgtMap.intensity(ci,j0);
		const float br = hgtMap.intensity(i1,j0);

		// sobel filter
		const float dX = (tr + 2.f*r + br) - (tl + 2.f*l + bl);
		const float dY = (bl + 2.f*b + br) - (tl + 2.f*t + tl);
		const float dZ = 1.f/(strength);
		ZVector nrmVec( dX, dY, dZ );
		nrmVec.normalize();
		nrmMap(ci,cj,0) = nrmVec.x;
		nrmMap(ci,cj,1) = nrmVec.z;
		nrmMap(ci,cj,2) = nrmVec.y;

	END_PER_EACH_PIXEL
}

ZELOS_NAMESPACE_END

