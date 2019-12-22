//----------------------//
// ZKmeanClustering.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
//         Jinhyuk Bae @ Dexter Studios                  //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZKmeanClustering::ZKmeanClustering()
{
	reset();
}

void
ZKmeanClustering::reset()
{
	_points = (ZPoint*)NULL;

	_clusterId.clear();
	FOR( i, 0, _pointId.size() )
	{
		_pointId[i].clear();
	}
	_numPtcEachCluster.clear();
	_centerPos.clear();
	_centerPosNew.clear();

	_tolerance = Z_LARGE;
	_maxIter   = 0;
}

void
ZKmeanClustering::setPointSet( ZPoint* posArray, int numPtc )
{
	_points = posArray;
	_numPtc = numPtc;
}

void 
ZKmeanClustering::getClusterNum( ZIntArray& numArray )
{
	numArray.resize( _numPtcEachCluster.length() );
	numArray = _numPtcEachCluster;
}

void 
ZKmeanClustering::getClusterId( ZIntArray& idArray )
{
	idArray = _clusterId;
}

void 
ZKmeanClustering::getPointIndex( int k, ZIntArray& numArray )
{
	if( k < 0 || k >= _numPtcEachCluster.size() )
	{
		cout << "Error@ZKmeanClustering::getPointIndex - Invalid index." << endl;
		return;
	}
	numArray.resize( _numPtcEachCluster[k] );
	numArray = _pointId[k];
}

void 
ZKmeanClustering::getCenterPos( ZPointArray& centerPosArray )
{
	centerPosArray = _centerPos;
}

void 
ZKmeanClustering::getCenterPos( int k, ZPoint& centerPos )
{
	if( k < 0 || k >= _numPtcEachCluster.size() )
	{
		cout << "Error@ZKmeanClustering::getCenterPos - Invalid index." << endl;
		return;
	}
	centerPos = _centerPos[k];
}

void 
ZKmeanClustering::getCovMatrix( int k, ZMatrix& covMatrix )
{
	if( k < 0 || k >= _centerPos.size() )
	{
		cout << "Error@ZKmeanClustering::getCovMatrix- Invalid index." << endl;
		return;
	}

	const ZIntArray& idArray	= _pointId[k];
	const int sizeOfKcluster	= _pointId[k].length();
	const int numOfKcluster     = _numPtcEachCluster[k];
	const ZPoint& centerPos 	= _centerPos[k];

if( sizeOfKcluster != numOfKcluster ) 
{ cout << "Error:: sizeOfKcluster, numOfKcluster=" << sizeOfKcluster << "," << numOfKcluster << endl; }

	// compute center
	ZPoint center(0,0,0);
	FOR( i, 0, sizeOfKcluster )
	{
		center += _points[idArray[i]];
	}
	center.x /= (float)sizeOfKcluster;
	center.y /= (float)sizeOfKcluster;
	center.z /= (float)sizeOfKcluster;

	// compute the matrix elements
	float xx = 0;
	float yy = 0;
	float zz = 0;
	float xy = 0;
	float xz = 0;
	float yz = 0;
	
	FOR(  i, 0, sizeOfKcluster )
	{
		const ZPoint& pt = _points[idArray[i]];
		float xValue = pt.x - center.x;
		float yValue = pt.y - center.y;
		float zValue = pt.z - center.z;
		xx += xValue*xValue;
		yy += yValue*yValue;
		zz += zValue*zValue;
		xy += xValue*yValue;
		xz += xValue*zValue;
		yz += yValue*zValue;
	}

	covMatrix.set( xx, xy, xz, 0,
				   xy, yy, yz, 0,
				   xz, yz, zz, 0,
				   0, 0, 0,    1 );
}

void
ZKmeanClustering::run( int numK, float tol, int maxItr, const ZPointArray& initPos )
{
	int numPtc = _numPtc; //_points->length();

	// initialize
	{
		_clusterId.resize( numPtc, -1 );
		_numPtcEachCluster.resize( numK, 0 );
		_pointId.resize( numK );

		_centerPos.resize( numK );
		_centerPosNew.resize( numK );
	}

	// initialize center pos 
	{
		if ( initPos.size() != numK )
		{
			FOR( k, 0, numK )
			{
				if(k%2 == 0)
					_centerPos[k] = (_points)[k];
				else 
					_centerPos[k] = (_points)[numPtc-k-1];
			}
		}
		else
		{
			_centerPos = initPos;
		}
	}

	bool loop = true;
	int  cnt = 0;
	while( loop )
	{
		// initialize 
		_clusterId.fill( -1 );
		FOR( k, 0, numK )
		{
			_numPtcEachCluster[k] = 0;
		}

		//===========================
		// searching nearest centerPos 
		//
		// set : _clusterId, _numPtcEachCluster
		FOR( i, 0, numPtc )
		{
			const ZPoint& pPos = (_points)[i];
			float minDistSQ = Z_LARGE;

			FOR( k, 0, numK )
			{
				const ZPoint& cPos = _centerPos[k];
				float distSQ = pPos.squaredDistanceTo( cPos );
				if( distSQ < minDistSQ )
				{
					minDistSQ = distSQ;
					_clusterId[i] = k;
				}
			}

			FOR( k, 0, numK )
			{
				if( _clusterId[i] == k ) {
					_numPtcEachCluster[k]++;
					continue;
				}
			}
		}

		// set : _pointId at each cluster 
		FOR( k, 0, numK )
		{
			_pointId[k].reset();
		}

		// set : pointId at each cluster
		FOR( i, 0, numPtc )
		{
			FOR( k, 0, numK )
			{
				if( _clusterId[i] == k )
				{
					_pointId[k].push_back(i);
					//continue;
				}
			}
		}
		
		//===========================
		// compute center of cluster
		//
		FOR( k, 0, numK )
		{
			_centerPosNew[k] = ZPoint(0,0,0);
		}

		FOR( i, 0, numPtc )
		{
			FOR( k, 0, numK )
			{
				if( _clusterId[i] == k )
				{
					_centerPosNew[k] += (_points)[i];
					continue;
				}
			}			
		}

		FOR( k, 0, numK )
		{
			_centerPosNew[k] /= _numPtcEachCluster[k];
		}

		//===========================
		// check for loop conditions
		//
		float maxTol = 0;
		FOR( k, 0, numK )
		{
			float distSQ = _centerPos[k].squaredDistanceTo( _centerPosNew[k] );
			if( maxTol < distSQ )
				maxTol = distSQ;
		}

		_centerPos = _centerPosNew;

		if( cnt++ > maxItr ||  maxTol < tol )
			loop = false;
	}

	// error checking
	FOR( k, 0, numK )
	{
		if ( _pointId[k].length() != _numPtcEachCluster[k] )
		{ 
			cout << "Error::ZKmeanClustering::run()" << endl;
			cout << "K, _pointId[k].length(), _numPtcEachCluster[k]=" << k << ", " 
																	  << _pointId[k].length() << "," 
																	  << _numPtcEachCluster[k] << endl; 
		}
	}
}

ZELOS_NAMESPACE_END

