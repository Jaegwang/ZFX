//---------------------//
// ZTriMeshScatter.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
//         Jaegwang Lim @ Dexter Studios                 //
// last update: 2018.03.08                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZTriMeshScatter::ZTriMeshScatter()
{
	_mesh       = (ZTriMesh*)NULL;
	_triIndices = (ZIntArray*)NULL;
	_baryCoords = (ZFloat3Array*)NULL;
	_totalArea  = 0.0;

	_initParams();
}

ZTriMeshScatter::ZTriMeshScatter( const ZTriMesh& mesh )
{
	_mesh       = const_cast<ZTriMesh*>(&mesh);
	_triIndices = (ZIntArray*)NULL;
	_baryCoords = (ZFloat3Array*)NULL;
	_totalArea  = 0.0;

	_initParams();
}

void
ZTriMeshScatter::reset()
{
	_mesh       = (ZTriMesh*)NULL;
	_triIndices = (ZIntArray*)NULL;
	_baryCoords = (ZFloat3Array*)NULL;
	_totalArea  = 0.0;

	_initParams();

	triangleList.clear();
}

void
ZTriMeshScatter::_initParams()
{
	useOpenMP           = true;
	targetNumber        = 0;
	randomSeed          = 0;
	diskVariance        = 1.f;
	directRadiusControl = true;
	dValueMin           = 0.f;
	dValueMax           = 1.f;
	dValueLift          = 0.f;
	densityMap          = ZString();
	removeMap           = ZString();
	removeValue         = 0.5f;
	method              = ZSamplingMethod::zNone;
}

int
ZTriMeshScatter::scatter( ZIntArray& triIndices, ZFloat3Array& baryCoords )
{
	triIndices.clear();
	baryCoords.clear();

	if( !_mesh ) { return 0; }
	if( targetNumber <= 0 ) { return 0; }

	// initialize
	_triIndices = &triIndices;
	_baryCoords = &baryCoords;
	_totalArea  = 0.0;

	if( method == ZSamplingMethod::zRandomBarycentric1 ) { _scatter01();                        } else
	if( method == ZSamplingMethod::zRandomBarycentric2 ) { _scatter02(); _deleteByDensityMap(); } else
	if( method == ZSamplingMethod::zPoissonDiskOnUV    ) { _scatter03(); _deleteByDensityMap(); } else
	if( method == ZSamplingMethod::zMonteCarlo         ) { _scatter04(); _deleteByDensityMap(); } else
	if( method == ZSamplingMethod::zPoissonDiskOnMesh  ) { _scatter05();                        } else
	{ cout << "Error@ZTriMeshScatter:scatter(): Invalid sampling method." << endl; }

	_deleteByRemoveMap();

	return triIndices.length();;
}

int
ZTriMeshScatter::scatter( ZPointArray& samples, const int num, bool appending )
{
	if( !appending ) { samples.clear(); }

	ZIntArray    triIndices;
	ZFloat3Array baryCoords;
    
    targetNumber = num;
    method = ZSamplingMethod::zMonteCarlo;

	const int count = scatter( triIndices, baryCoords );    

	const ZPointArray& p = _mesh->p;
	const ZInt3Array& v012 = _mesh->v012;
    
    int beginIdn = samples.size();
    samples.resize(beginIdn+count);

    #pragma omp prallel for
	for(int i=0; i<count; i++)
	{
		const int& tIdx = triIndices[i];
		const ZFloat3& coeff = baryCoords[i];

		const int& v0 = v012[tIdx][0];
		const int& v1 = v012[tIdx][1];
		const int& v2 = v012[tIdx][2];

		const ZPoint& p0 = p[v0];
		const ZPoint& p1 = p[v1];
		const ZPoint& p2 = p[v2];

		samples[beginIdn+i] = WeightedSum( p0, p1, p2, coeff );
	}

	return count;
}

int
ZTriMeshScatter::scatter( ZPointArray& positions, ZVectorArray& normals, const int num, bool appending )
{
	if( !appending ) 
	{
		 positions.clear();
		 normals.clear(); 
	}
	
	ZIntArray    triIndices;
	ZFloat3Array baryCoords;
    
    targetNumber = num;
    method = ZSamplingMethod::zMonteCarlo;
	
	ZVectorArray normalArray;
	_mesh->getTriangleNormals(normalArray, true);		

	const int count = scatter( triIndices, baryCoords );    

	const ZPointArray& p = _mesh->p;
	const ZInt3Array& v012 = _mesh->v012;
    
    int beginIdn = positions.size();
    positions.resize(beginIdn+count);
	normals.resize(beginIdn+count);

    #pragma omp prallel for
	for(int i=0; i<count; i++)
	{
		const int& tIdx = triIndices[i];
		const ZFloat3& coeff = baryCoords[i];

		const int& v0 = v012[tIdx][0];
		const int& v1 = v012[tIdx][1];
		const int& v2 = v012[tIdx][2];

		const ZPoint& p0 = p[v0];
		const ZPoint& p1 = p[v1];
		const ZPoint& p2 = p[v2];

		positions[beginIdn+i] = WeightedSum( p0, p1, p2, coeff );
		normals  [beginIdn+i] = normalArray[tIdx]; 
	}

	return count;
}

int 
ZTriMeshScatter::scatter( ZTriMesh* mesh0, ZTriMesh* mesh1, ZPointArray& positions, ZVectorArray& normals, ZVectorArray& velocities, const float dt, const int num, bool appending )
{
	if( !appending ) 
	{
		 positions.clear();
		 normals.clear();
		 velocities.clear(); 
	}
	
	_mesh = mesh1;
	
	ZIntArray    triIndices;
	ZFloat3Array baryCoords;
    
    targetNumber = num;
    method = ZSamplingMethod::zMonteCarlo;
	
	ZVectorArray normalArray;
	_mesh->getVertexNormals(normalArray, true);			

	const int count = scatter( triIndices, baryCoords );    

	const ZPointArray& p = _mesh->p;
	const ZInt3Array& v012 = _mesh->v012;
    
    int beginIdn = positions.size();
	
    positions.resize(beginIdn+count);
	normals.resize(beginIdn+count);
	velocities.resize(beginIdn+count);

    #pragma omp prallel for
	for(int i=0; i<count; i++)
	{
		const int& tIdx = triIndices[i];
		const ZFloat3& coeff = baryCoords[i];

		const int& v0 = v012[tIdx][0];
		const int& v1 = v012[tIdx][1];
		const int& v2 = v012[tIdx][2];

		const ZPoint& p0 = p[v0];
		const ZPoint& p1 = p[v1];
		const ZPoint& p2 = p[v2];
		
		const ZVector& n0 = normalArray[v0];
		const ZVector& n1 = normalArray[v1];
		const ZVector& n2 = normalArray[v2];
		
		const ZVector d0 = mesh0 ? (p0 - mesh0->p[v0])/dt : ZVector();
		const ZVector d1 = mesh0 ? (p1 - mesh0->p[v1])/dt : ZVector();
		const ZVector d2 = mesh0 ? (p2 - mesh0->p[v2])/dt : ZVector();

		positions [beginIdn+i] = WeightedSum( p0, p1, p2, coeff );
		velocities[beginIdn+i] = WeightedSum( d0, d1, d2, coeff );
		normals   [beginIdn+i] = WeightedSum( n0, n1, n2, coeff );		
	}

	return count;
} 

// zRandomBarycentric1
void
ZTriMeshScatter::_scatter01()
{
	const ZTriMesh& mesh = *_mesh;
	const int nVertices  = mesh.numVertices();
	const int nTriangles = mesh.numTriangles();
	if( !nVertices || !nTriangles ) { return; }

	const ZPointArray& p    = mesh.p;
	const ZInt3Array&  v012 = mesh.v012;

	ZIntArray&    triIndices = *_triIndices;
	ZFloat3Array& baryCoords = *_baryCoords;

	ZFloatArray triDensity;
	mesh.getTriangleCenterValue( densityMap.asChar(), triDensity, 0, useOpenMP );

	if( !triDensity.length() )
	{
		triDensity.setLengthWithValue( nTriangles, 1.f );
	}

	ZIntArray tFlags;
	{
		tFlags.setLengthWithValue( nTriangles, 1 );

		if( triangleList.length() )
		{
			tFlags.zeroize();

			FOR( i, 0, triangleList.length() )
			{
				const int& tIdx = triangleList[i];

				if( tIdx <  0          ) { continue; }
				if( tIdx >= nTriangles ) { continue; }

				tFlags[tIdx] = 1;
			}
		}
	}

	ZDoubleArray tArea;
	{
		tArea.setLength( nTriangles );

		#pragma omp parallel for if( useOpenMP )
		FOR( i, 0, nTriangles )
		{
			if( tFlags[i] )
			{
				const ZInt3& tIdx = v012[i];

				const ZPoint& p0 = p[tIdx[0]];
				const ZPoint& p1 = p[tIdx[1]];
				const ZPoint& p2 = p[tIdx[2]];

				tArea[i] = Area(p0,p1,p2) * triDensity[i];
			}
		}

		_totalArea = tArea.sum( useOpenMP );

		if( ZAlmostZero( _totalArea ) )
		{
			return;
		}

		tArea.multiply( 1/_totalArea, useOpenMP );
	}

	int seed = randomSeed;
	float residue = 0.f;

	FOR( i, 0, nTriangles )
	{
		const float& area = tArea[i];

		int numToGen = int(targetNumber*area);
		residue += (targetNumber*area) - numToGen;
		if( residue > 1 ) { residue-=1.f; ++numToGen; }

		FOR( j, 0, numToGen )
		{
			float a = ZRand( ++seed );
			float b = ZRand( ++seed );
			if( a+b >= 1.f ) { a=1.f-a; b=1.f-b; }

			triIndices.push_back( i );
			baryCoords.push_back( ZFloat3( a, b, 1.f-a-b ) );
		}
	}
}

// zRandomBarycentric2
void
ZTriMeshScatter::_scatter02()
{
	const ZTriMesh& mesh = *_mesh;
	const int nVertices  = mesh.numVertices();
	const int nTriangles = mesh.numTriangles();
	if( !nVertices || !nTriangles ) { return; }

	const ZPointArray& p    = mesh.p;
	const ZInt3Array&  v012 = mesh.v012;

	ZIntArray&    triIndices = *_triIndices;
	ZFloat3Array& baryCoords = *_baryCoords;

	ZIntArray tFlags;
	{
		tFlags.setLengthWithValue( nTriangles, 1 );

		if( triangleList.length() )
		{
			tFlags.zeroize();

			FOR( i, 0, triangleList.length() )
			{
				const int& tIdx = triangleList[i];

				if( tIdx <  0          ) { continue; }
				if( tIdx >= nTriangles ) { continue; }

				tFlags[tIdx] = 1;
			}
		}
	}

	ZDoubleArray tArea;
	{
		tArea.setLength( nTriangles );

		#pragma omp parallel for if( useOpenMP )
		FOR( i, 0, nTriangles )
		{
			if( tFlags[i] )
			{
				const ZInt3& tIdx = v012[i];

				const ZPoint& p0 = p[tIdx[0]];
				const ZPoint& p1 = p[tIdx[1]];
				const ZPoint& p2 = p[tIdx[2]];

				tArea[i] = Area(p0,p1,p2);
			}
		}

		_totalArea = tArea.sum( useOpenMP );
		if( ZAlmostZero( _totalArea ) ) { return; }

		tArea.multiply( 1/_totalArea, useOpenMP );
	}

	int seed = randomSeed;
	float residue = 0.f;

	FOR( i, 0, nTriangles )
	{
		const float& area = tArea[i];

		int numToGen = int(targetNumber*area);
		residue += (targetNumber*area) - numToGen;
		if( residue > 1 ) { residue-=1.f; ++numToGen; }

		FOR( j, 0, numToGen )
		{
			float a = ZRand( ++seed );
			float b = ZRand( ++seed );
			if( a+b >= 1.f ) { a=1.f-a; b=1.f-b; }

			triIndices.push_back( i );
			baryCoords.push_back( ZFloat3( a, b, 1.f-a-b ) );
		}
	}
}

// zPoissonDiskOnUV
void
ZTriMeshScatter::_scatter03()
{
	const ZTriMesh& mesh = *_mesh;
	const int nVertices  = mesh.numVertices();
	const int nTriangles = mesh.numTriangles();
	const int nUVs       = mesh.numUVs();
	if( !nVertices || !nTriangles || !nUVs ) { return; }

	const ZPointArray& p    = mesh.p;
	const ZInt3Array&  v012 = mesh.v012;
	const ZPointArray& uv   = mesh.uv;

	ZIntArray&    triIndices = *_triIndices;
	ZFloat3Array& baryCoords = *_baryCoords;

	const ZBoundingBox bBox( uv.boundingBox() );
	ZPoint minPt( bBox.minPoint() );
	ZPoint maxPt( bBox.maxPoint() );

	const float radius = 0.8f * sqrtf( bBox.width(0) * bBox.width(1) / (float)targetNumber );

	ZPointArray uvs;
	ScatterPoissonDisk2D( radius, minPt, maxPt, 0, randomSeed, false, uvs ); // 0: xy-plane

	GetPointsFromUVs( mesh, uvs, triIndices, baryCoords, useOpenMP );
}

// zMonteCarlo
void
ZTriMeshScatter::_scatter04()
{
	const ZTriMesh& mesh = *_mesh;
	const int nVertices  = mesh.numVertices();
	const int nTriangles = mesh.numTriangles();
	if( !nVertices || !nTriangles ) { return; }

	const ZPointArray& p    = mesh.p;
	const ZInt3Array&  v012 = mesh.v012;

	ZIntArray&    triIndices = *_triIndices;
	ZFloat3Array& baryCoords = *_baryCoords;

	std::vector<pair<float,int> > intervals( nTriangles+1 );
	intervals[0] = pair<float,int>( 0.f, -1 );

	ZIntArray tFlags;
	{
		tFlags.setLengthWithValue( nTriangles, 1 );

		if( triangleList.length() )
		{
			tFlags.zeroize();

			FOR( i, 0, triangleList.length() )
			{
				const int& tIdx = triangleList[i];

				if( tIdx <  0          ) { continue; }
				if( tIdx >= nTriangles ) { continue; }

				tFlags[tIdx] = 1;
			}
		}
	}

	// Don't use OpenMP here.
	FOR( i, 0, nTriangles )
	{
		const ZInt3& tIdx = v012[i];

		const ZPoint& p0 = p[tIdx[0]];
		const ZPoint& p1 = p[tIdx[1]];
		const ZPoint& p2 = p[tIdx[2]];

		const float area = tFlags[i] ? Area(p0,p1,p2) : 0.f;

		intervals[i+1] = std::make_pair( intervals[i].first+area, i );
	}

	_totalArea = intervals.back().first;

	int seed = randomSeed;

	FOR( i, 0, targetNumber )
	{
		const float val = _totalArea * ZRand( ++seed );

		std::vector<pair<float,int> >::iterator itr;
		itr = lower_bound( intervals.begin(), intervals.end(), std::make_pair(val,-1) );

		const int& tIdx = itr->second;
		if( tIdx < 0 ) { continue; }

		float a = ZRand( ++seed );
		float b = ZRand( ++seed );
		if( a+b >= 1.f ) { a=1.f-a; b=1.f-b; }

		triIndices.push_back( itr->second );
		baryCoords.push_back( ZFloat3( a, b, 1.f-a-b ) );
	}
}

// zPoissonDiskOnMesh
void
ZTriMeshScatter::_scatter05()
{
	const ZTriMesh& mesh = *_mesh;
	const int nVertices  = mesh.numVertices();
	const int nTriangles = mesh.numTriangles();
	if( !nVertices || !nTriangles ) { return; }

	const ZPointArray& p    = mesh.p;
	const ZInt3Array&  v012 = mesh.v012;
	const ZPointArray& uv   = mesh.uv;

	ZIntArray&    triIndices = *_triIndices;
	ZFloat3Array& baryCoords = *_baryCoords;

	// Pre-sampling by Monte Carlo method
	const int _targetNumber = targetNumber;
	const int mcRate = 20;
	targetNumber *= mcRate;
	_scatter04();
	targetNumber = _targetNumber;

	ZIntArray    triIndices0;
	ZFloat3Array baryCoords0;
	{
		triIndices0.exchange( triIndices );
		baryCoords0.exchange( baryCoords );
	}

	ZPointArray  sP; // pre-sample positions
	ZVectorArray sN; // pre-sample normals
	{
		mesh.getPositions( triIndices0, baryCoords0, sP, useOpenMP );
		mesh.getNormals  ( triIndices0, baryCoords0, sN, useOpenMP );
	}

	const int nPreSamples = sP.length();
	const float radius = sqrtf( _totalArea / ( 0.7 * Z_PI * targetNumber ) );

	ZFloatArray sR; // pre-sample radius
	{
		sR.setLengthWithValue( nPreSamples, radius );

		ZImageMap dMap;
		if( dMap.load( densityMap.asChar() ) && uv.length() ) {

			#pragma omp parallel for if( useOpenMP )
			FOR( i, 0, nPreSamples )
			{
				float& r = sR[i];

				int idx = 3*triIndices0[i];
				const ZPoint& uv0 = uv[  idx];
				const ZPoint& uv1 = uv[++idx];
				const ZPoint& uv2 = uv[++idx];

				const ZFloat3& bary = baryCoords0[i];
				const float& a = bary[0];
				const float& b = bary[1];
				const float& c = bary[2];

				const float u = ( a*uv0.x + b*uv1.x + c*uv2.x );
				const float v = ( a*uv0.y + b*uv1.y + c*uv2.y );

				if( directRadiusControl ) {

					const float dValue = 1.f - ZClamp( dMap.fastValue(u,v), 0.f, 1.f );
					r = ( dValueMax - dValueMin ) * dValue + dValueMin + dValueLift;

				} else {

					const float dValue = ZClamp( dMap.fastValue(u,v), 0.f, 1.f );
					r = radius * ( 2 - dValue ); // 0(black): 2*radius, 1(white): radius

				}
			}

			if( !directRadiusControl )
			{
				float minR=0.f, maxR=0.f;
				sR.getMinMax( minR, maxR, useOpenMP );

				const float minRad = radius / diskVariance;
				const float maxRad = radius * diskVariance;

				const float deltaR   = maxR - minR;
				const float deltaRad = maxRad - minRad;

				#pragma omp parallel for if( useOpenMP )
				FOR( i, 0, nPreSamples )
				{
					sR[i] = minRad + deltaRad * ( ( sR[i] - minR ) / deltaR );
				}
			}

		} else {

			sR.fill( radius );

		}
	}

	ZUnboundedHashGrid3D hash;
	{
		const float h = ( 2.f * radius ) / sqrtf(3.f); // cell size
		hash.set( h );

		FOR( i, 0, nPreSamples )
		{
			hash.addItem( i, sP[i] );
		}
	}

	ZInt3Array cells;
	hash.getAllocatedCells( cells );
	cells.shuffle();

	while( !cells.empty() )
	{
		FOR( i, 0, cells.size() )
		{
			const ZInt3& cell = cells[i];
			if( hash.empty( cell ) ) { continue; }

			const int idx = hash.firstItem( cell );

			triIndices.push_back( triIndices0[idx] );
			baryCoords.push_back( baryCoords0[idx] );

			hash.remove( sP[idx], sN[idx], sR[idx], sP, sN );
		}

		hash.getAllocatedCells( cells );
	}
}

void
ZTriMeshScatter::_deleteByDensityMap()
{
	if( !_mesh || !_triIndices || !_baryCoords ) { return; }
	if( !_mesh->numUVs() ) { return; }

	ZImageMap dMap;
	if( !dMap.load( densityMap.asChar() ) ) { return; }

	const ZPointArray& uv = _mesh->uv;

	ZIntArray&    triIndices = *_triIndices;
	ZFloat3Array& baryCoords = *_baryCoords;

	const int nSamples = triIndices.length();
	ZCharArray mark( nSamples );

	#pragma omp parallel for if( useOpenMP )
	FOR( i, 0, nSamples )
	{
		int idx = 3*triIndices[i];

		const ZPoint& uv0 = uv[  idx];
		const ZPoint& uv1 = uv[++idx];
		const ZPoint& uv2 = uv[++idx];

		const ZFloat3& bary = baryCoords[i];
		const float& a = bary[0];
		const float& b = bary[1];
		const float& c = bary[2];

		const float u = ( a*uv0.x + b*uv1.x + c*uv2.x );
		const float v = ( a*uv0.y + b*uv1.y + c*uv2.y );

		const float dValue = dMap.fastValue( u, v );

		if( dValue < ZRand(i) )
		{
			mark[i] = (char)1;
		}
	}

	ZIntArray delList;

	FOR( i, 0, nSamples )
	{
		if( mark[i] )
		{
			delList.push_back( i );
		}
	}

	triIndices.remove( delList );
	baryCoords.remove( delList );
}

void
ZTriMeshScatter::_deleteByRemoveMap()
{
	if( !_mesh || !_triIndices || !_baryCoords ) { return; }
	if( !_mesh->numUVs() ) { return; }

	ZImageMap rMap;
	if( !rMap.load( removeMap.asChar() ) ) { return; }

	const ZPointArray& uv = _mesh->uv;

	ZIntArray&    triIndices = *_triIndices;
	ZFloat3Array& baryCoords = *_baryCoords;

	const int nSamples = triIndices.length();
	ZCharArray mark( nSamples );

	#pragma omp parallel for if( useOpenMP )
	FOR( i, 0, nSamples )
	{
		int idx = 3*triIndices[i];

		const ZPoint& uv0 = uv[  idx];
		const ZPoint& uv1 = uv[++idx];
		const ZPoint& uv2 = uv[++idx];

		const ZFloat3& bary = baryCoords[i];
		const float& a = bary[0];
		const float& b = bary[1];
		const float& c = bary[2];

		const float u = ( a*uv0.x + b*uv1.x + c*uv2.x );
		const float v = ( a*uv0.y + b*uv1.y + c*uv2.y );

		const float dValue = rMap.fastValue( u, v );

		if( dValue < removeValue )
		{
			mark[i] = (char)1;
		}
	}

	ZIntArray delList;

	FOR( i, 0, nSamples )
	{
		if( mark[i] )
		{
			delList.push_back( i );
		}
	}

	triIndices.remove( delList );
	baryCoords.remove( delList );
}

ostream&
operator<<( ostream& os, const ZTriMeshScatter& object )
{
	os << "<ZTriMeshScatter>" << endl;
	os << endl;

	return os;
}

ZELOS_NAMESPACE_END

