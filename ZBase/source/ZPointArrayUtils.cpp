//----------------------//
// ZPointArrayUtils.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2018.03.08                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

void
GetNormalizedPositions( const ZPointArray& input, ZPointArray& output, bool useOpenMP )
{
	const int N = (int)input.size();
	if( !N )
	{
		output.clear();
		return;
	}
	output.resize( N );

	ZBoundingBox bBox( input.boundingBox( useOpenMP ) );
	bBox.expand();

	const float _dx = 1 / bBox.width(0);
	const float _dy = 1 / bBox.width(1);
	const float _dz = 1 / bBox.width(2);

	const ZPoint min( bBox.minPoint() );
	const ZPoint max( bBox.maxPoint() );

	#pragma omp parallel for if( useOpenMP )
	FOR( i, 0, N )
	{
		const ZPoint& a = input[i];
		ZPoint& b = output[i];

		b.x = ( a.x - min.x ) * _dx;
		b.y = ( a.y - min.y ) * _dy;
		b.z = ( a.z - min.z ) * _dz;
	}
}

int
DoKMeansClustering( const ZPointArray& nPoints, const ZPointArray& kInitCentroids, int maxIterations, float tolerance, bool useOpenMP, vector<ZIntArray>& clusterIndices )
{
	clusterIndices.clear();

	const int N = nPoints.length();
	const int K = kInitCentroids.length();

	if( N==0 || K==0 )
	{
		return 0;
	}

	if( K==1 )
	{
		ZIntArray indices( N );
		clusterIndices.push_back( indices );
		return 0;
	}

	ZPointArray nPtc( N );	// normalized positions of n-points
	ZPointArray kPtc( K );	// normalzied positions of k-centroids
	{
		ZBoundingBox bBox( nPoints.boundingBox( useOpenMP ) );
		bBox.expand();

		const float _dx = 1 / bBox.width(0);
		const float _dy = 1 / bBox.width(1);
		const float _dz = 1 / bBox.width(2);

		const ZPoint min( bBox.minPoint() );
		const ZPoint max( bBox.maxPoint() );

		#pragma omp parallel for if( useOpenMP )
		FOR( i, 0, N )
		{
			const ZPoint& wp = nPoints[i];
			nPtc[i].set( (wp.x-min.x)*_dx, (wp.y-min.y)*_dy, (wp.z-min.z)*_dz );
		}

		#pragma omp parallel for if( useOpenMP )
		FOR( k, 0, K )
		{
			const ZPoint& wp = kInitCentroids[k];
			kPtc[k].set( (wp.x-min.x)*_dx, (wp.y-min.y)*_dy, (wp.z-min.z)*_dz );
		}
	}
	ZDoubleArray cx(K), cy(K), cz(K);
	ZIntArray count(K);

	ZIntArray closestCentroidId( N, -1 );
	ZIntArray closestCentroidId0( N, -1 );

	ZPointsDistTree kdTree;

	int numIters = 1;
	FOR( itr, 0, maxIterations )
	{
		kdTree.setPoints( kPtc );

		#pragma omp parallel for if( useOpenMP )
		FOR( i, 0, N )
		{
			int& centroidId = closestCentroidId[i];
			float dist2 = 0.f;
			kdTree.findClosestPoint( nPtc[i], centroidId, dist2 );
		}

		{
			cx.zeroize();
			cy.zeroize();
			cz.zeroize();
			count.zeroize();

			FOR( i, 0, N )
			{
				const ZPoint& p = nPtc[i];
				const int& k = closestCentroidId[i];
				cx[k] += p.x;
				cy[k] += p.y;
				cz[k] += p.z;
				++count[k];
			}

			FOR( k, 0, K )
			{
				ZPoint& p = kPtc[k];
				const double denom = 1 / (double)count[k];
				p.x = cx[k] * denom; 
				p.y = cy[k] * denom; 
				p.z = cz[k] * denom; 
			}
		}

		++numIters;
	}

	return numIters;
}

ZELOS_NAMESPACE_END

