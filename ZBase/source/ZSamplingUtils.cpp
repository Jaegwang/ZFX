//--------------------//
// ZSamplingUtils.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

int
ScatterOnSphere( int number, const ZPoint& center, float radius, int seed, bool asAppending, bool useOpenMP, ZPointArray& points )
{
	if( asAppending ) {
		if( number <= 0 ) { return points.length(); }
	} else {
		points.clear();
	}

	const int oldNum = points.length();
	points.resize( oldNum + number );
	const int newNum = points.length();

	#pragma omp parallel for if( useOpenMP )
	for( int i=oldNum; i<newNum; ++i )
	{
		const float u = ZRand( seed+i    );
		const float v = ZRand( seed+i+37 );
		const float theta = Z_PIx2 * u;
		float phi = acosf(2*v-1);

		ZPoint& p = points[i];

		p.x = center.x + ( radius * sinf(phi) * cosf(theta) );
		p.y = center.y + ( radius * sinf(phi) * sinf(theta) );
		p.z = center.z + ( radius * cosf(phi) );
	}

	return points.length();
}

int
ScatterInSphere( int number, const ZPoint& center, float radius, int seed, bool asAppending, bool useOpenMP, ZPointArray& points )
{
	if( asAppending ) {
		if( number <= 0 ) { return points.length(); }
	} else {
		points.clear();
	}

	const int oldNum = points.length();
	points.resize( oldNum + number );
	const int newNum = points.length();

	#pragma omp parallel for if( useOpenMP )
	for( int i=oldNum; i<newNum; ++i )
	{
		const float rvals = (float)ZRand( seed+i, -1.f, 1.f );
		const float elevation = asinf( rvals );
		const float azimuth = (float)ZRand( seed+i+31, (float)Z_PIx2 );
		const float radii = powf( (float)ZRand( seed+i+51 ), 1/3.f );

		ZPoint&p = points[i];

		p = ZSphericalToCartesian( azimuth, elevation, radii );

		p *= radius;

		p.x += center.x;
		p.y += center.y;
		p.z += center.z;
	}

	return points.length();
}

int
ScatterPoissonDisk2D( float radius, const ZPoint& minPt, const ZPoint& maxPt, int whichPlane, int seed, bool asAppending, ZPointArray& points )
{
	if( asAppending && radius<=Z_EPS ) { return points.length(); }
	if( !asAppending ) { points.clear(); }

	const float& xMin = (whichPlane==0) ? minPt.x : ( (whichPlane==1) ? minPt.y : minPt.z );
	const float& yMin = (whichPlane==0) ? minPt.y : ( (whichPlane==1) ? minPt.z : minPt.x );

	const float& xMax = (whichPlane==0) ? maxPt.x : ( (whichPlane==1) ? maxPt.y : maxPt.z );
	const float& yMax = (whichPlane==0) ? maxPt.y : ( (whichPlane==1) ? maxPt.z : maxPt.x );

	const int N = 2; // dimension
	const int max_sample_attempts = 30;
	const float r  = radius;
	const float r2 = radius*2;
	const float rr = radius*radius;

	points.clear();
	ZIntArray active_list;

	// acceleration grid
	const float h = (0.999f*radius) / sqrtf(N); // a grid cell this size can have at most one sample in it
	const int nx = ZCeil( (xMax-xMin)/h );
	const int ny = ZCeil( (yMax-yMin)/h );
	const int nx_1 = nx-1;
	const int ny_1 = ny-1;
	const int numCells = nx*ny;
	ZIntArray grid( numCells, -1 ); // -1 indicates no sample there; otherwise index of sample point

	#define IDX(i,j) (i+nx*j)

	// first sample
	ZPoint P;
	P.x = (float)ZRand( seed++, xMin, xMax );
	P.y = (float)ZRand( seed++, yMin, yMax );
	points.append( P );
	active_list.append( 0 );
	int ci = ZTrunc( (P.x-xMin)/h );
	int cj = ZTrunc( (P.y-yMin)/h );
	grid[IDX(ci,cj)] = 0;

	ZInt2 j, jMin, jMax;

	while( !active_list.empty() )
	{
		bool found_sample = false;
		const int r = (int)ZRand( seed++, active_list.length()-0.0001f );
		const int& p = active_list[r];
		const ZPoint& center = points[p];

		FOR( attempt, 0, max_sample_attempts )
		{
			const float r = (float)ZRand( seed++, radius, r2 );
			const float a = (float)ZRand( seed++, (float)Z_PIx2 );
			P.set( center.x+r*cosf(a), center.y+r*sinf(a), 0.f );

			if( P.x<xMin || P.x>xMax ) { goto reject_sample; }
			if( P.y<yMin || P.y>yMax ) { goto reject_sample; }

			jMin[0] = ZClamp( ZTrunc( (P.x-radius-xMin)/h ), 0, nx_1 );
			jMax[0] = ZClamp( ZTrunc( (P.x+radius-xMin)/h ), 0, nx_1 );
			jMin[1] = ZClamp( ZTrunc( (P.y-radius-yMin)/h ), 0, ny_1 );
			jMax[1] = ZClamp( ZTrunc( (P.y+radius-yMin)/h ), 0, ny_1 );

			j = jMin;

			while( 1 )
			{
				// check if there's a sample at j that's too close to x
				int k = IDX(j[0],j[1]);
				if( grid[k]>=0 && grid[k]!=p ) // if there is a sample point different from p
				{
					if( P.squaredDistanceTo( points[grid[k]] ) < rr ) { goto reject_sample; }
				}

				// move on to next j
				FOR( i, 0, N )
				{
					++j[i];
					if( j[i] <= jMax[i] ) {
						break;
					} else {
						if( i==N-1 ) { goto done_j_loop; }
						else { j[i] = jMin[i]; } // and try incrementing the next dimension along
					}
				}
			}

			done_j_loop:
			// if we made it here, we're good!
			found_sample = true;
			break;

			// if we goto here, x is too close to an existing sample
			reject_sample:
   		    ; // nothing to do except go to the next iteration in this loop
		}

		if( found_sample ) {

			const int q = points.length(); // the index of the new sample
			points.append( P );
			active_list.append( q );
			ci = ZTrunc( (P.x-xMin)/h );
			cj = ZTrunc( (P.y-yMin)/h );
			grid[IDX(ci,cj)] = q;

		} else {

			// since we couldn't find a sample on p's disk, we remove p from the active list
			active_list[r] = active_list.back();
			active_list.pop_back();

		}
	}

	#undef IDX

	const int nPtc = (int)points.size();
	if( whichPlane==1 ) {

		FOR( i, 0, nPtc )
		{
			ZPoint& p = points[i];
			ZSwap( p.y, p.z );
			ZSwap( p.x, p.y );
		}

	} else if( whichPlane==2 ) {

		FOR( i, 0, nPtc )
		{
			ZPoint& p = points[i];
			ZSwap( p.x, p.z );
			ZSwap( p.x, p.y );
		}
	}

	return nPtc;
}

int
ScatterPoissonDisk3D( float radius, const ZPoint& minPt, const ZPoint& maxPt, int seed, bool asAppending, ZPointArray& points )
{
	if( asAppending && radius<=Z_EPS ) { return points.length(); }
	if( !asAppending ) { points.clear(); }

	const int N = 3; // dimension
	const int max_sample_attempts = 30;
	const float r  = radius;
	const float r2 = radius*2;
	const float rr = radius*radius;

	points.clear();
	ZIntArray active_list;

	// acceleration grid
	const float h = (0.999f*radius) / sqrtf(N); // a grid cell this size can have at most one sample in it
	const int nx = ZCeil( (maxPt.x-minPt.x)/h );
	const int ny = ZCeil( (maxPt.y-minPt.y)/h );
	const int nz = ZCeil( (maxPt.z-minPt.z)/h );
	const int nx_1 = nx-1;
	const int ny_1 = ny-1;
	const int nz_1 = nz-1;
	const int nxny = nx*ny;
	const int numCells = nx*ny*nz;
	ZIntArray grid( numCells, -1 ); // -1 indicates no sample there; otherwise index of sample point

	#define IDX(i,j,k) (i+nx*j+nxny*k)

	// first sample
	ZPoint P;
	P.x = (float)ZRand( seed++, minPt.x, maxPt.x );
	P.y = (float)ZRand( seed++, minPt.y, maxPt.y );
	P.z = (float)ZRand( seed++, minPt.z, maxPt.z );
	points.append( P );
	active_list.append( 0 );
	int ci = ZTrunc( (P.x-minPt.x)/h );
	int cj = ZTrunc( (P.y-minPt.y)/h );
	int ck = ZTrunc( (P.z-minPt.z)/h );
	grid[IDX(ci,cj,ck)] = 0;

	ZInt3 j, jMin, jMax;

	while( !active_list.empty() )
	{
		bool found_sample = false;
		const int r = (int)ZRand( seed++, active_list.length()-0.0001f );
		const int& p = active_list[r];
		const ZPoint& center = points[p];

		FOR( attempt, 0, max_sample_attempts )
		{
			const float r = (float)ZRand( seed++, radius, r2 );
			const float s = (float)ZRand( seed++, (float)-1, (float)1 );
			const float phi = (float)ZRand( seed++, (float)Z_PIx2 );
			const float b = r;
			const float ab = (float)sqrt(1-s*s) * b;
			P.set( center.x+ab*cosf(phi), center.y+ab*sinf(phi), center.z+b*s );

			if( P.x<minPt.x || P.x>maxPt.x ) { goto reject_sample; }
			if( P.y<minPt.y || P.y>maxPt.y ) { goto reject_sample; }
			if( P.z<minPt.z || P.z>maxPt.z ) { goto reject_sample; }

			jMin[0] = ZClamp( ZTrunc( (P.x-radius-minPt.x)/h ), 0, nx_1 );
			jMax[0] = ZClamp( ZTrunc( (P.x+radius-minPt.x)/h ), 0, nx_1 );
			jMin[1] = ZClamp( ZTrunc( (P.y-radius-minPt.y)/h ), 0, ny_1 );
			jMax[1] = ZClamp( ZTrunc( (P.y+radius-minPt.y)/h ), 0, ny_1 );
			jMin[2] = ZClamp( ZTrunc( (P.z-radius-minPt.z)/h ), 0, nz_1 );
			jMax[2] = ZClamp( ZTrunc( (P.z+radius-minPt.z)/h ), 0, nz_1 );

			j = jMin;

			while( 1 )
			{
				// check if there's a sample at j that's too close to x
				int k = IDX(j[0],j[1],j[2]);
				if( grid[k]>=0 && grid[k]!=p ) // if there is a sample point different from p
				{
					if( P.squaredDistanceTo( points[grid[k]] ) < rr ) { goto reject_sample; }
				}

				// move on to next j
				FOR( i, 0, N )
				{
					++j[i];
					if( j[i] <= jMax[i] ) {
						break;
					} else {
						if( i==N-1 ) { goto done_j_loop; }
						else { j[i] = jMin[i]; } // and try incrementing the next dimension along
					}
				}
			}

			done_j_loop:
			// if we made it here, we're good!
			found_sample = true;
			break;

			// if we goto here, x is too close to an existing sample
			reject_sample:
   		    ; // nothing to do except go to the next iteration in this loop
		}

		if( found_sample ) {

			const int q = points.length(); // the index of the new sample
			points.append( P );
			active_list.append( q );
			ci = ZTrunc( (P.x-minPt.x)/h );
			cj = ZTrunc( (P.y-minPt.y)/h );
			ck = ZTrunc( (P.z-minPt.z)/h );
			grid[IDX(ci,cj,ck)] = q;

		} else {

			// since we couldn't find a sample on p's disk, we remove p from the active list
			active_list[r] = active_list.back();
			active_list.pop_back();

		}
	}

	#undef IDX

	return points.length();
}

ZELOS_NAMESPACE_END

