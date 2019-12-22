//-------------------//
// ZTriMeshUtils.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2018.03.08                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

void
GetPositions( const ZTriMesh& mesh, const ZIntArray& triIndices, const ZFloat3Array& baryCoords, ZPointArray& positions, bool useOpenMP )
{
	const int numVertices  = mesh.numVertices();
	const int numTriangles = mesh.numTriangles();
	if( !numVertices || !numTriangles ) { positions.clear(); return; }

	const int numPtc = triIndices.length();
	if( baryCoords.length() != numPtc )
	{
		cout << "Error@GetPositions(): Invalid input data." << endl;
		positions.clear();
		return;
	}

	positions.setLength( numPtc );

	const ZPointArray& p    = mesh.p;
	const ZInt3Array&  v012 = mesh.v012;
	const ZPointArray& uv   = mesh.uv;

	#pragma omp parallel for if( useOpenMP )
	FOR( i, 0, numPtc )
	{
		const ZInt3& tIdx = v012[ triIndices[i] ];
		const int& v0 = tIdx[0];
		const int& v1 = tIdx[1];
		const int& v2 = tIdx[2];

		const ZPoint& p0 = p[v0];
		const ZPoint& p1 = p[v1];
		const ZPoint& p2 = p[v2];

		positions[i] = WeightedSum( p0, p1, p2, baryCoords[i] );
	}
}

void
GetUVs( const ZTriMesh& mesh, const ZIntArray& triIndices, const ZFloat3Array& baryCoords, ZPointArray& uvs, bool useOpenMP )
{
	const int numVertices  = mesh.numVertices();
	const int numTriangles = mesh.numTriangles();
	if( !numVertices || !numTriangles ) { uvs.clear(); return; }

	const int numPtc = triIndices.length();
	if( baryCoords.length() != numPtc )
	{
		cout << "Error@GetUVs(): Invalid input data." << endl;
		uvs.clear();
		return;
	}

	uvs.setLength( numPtc );

	const ZPointArray& p    = mesh.p;
	const ZInt3Array&  v012 = mesh.v012;
	const ZPointArray& uv   = mesh.uv;

	#pragma omp parallel for if( useOpenMP )
	FOR( i, 0, numPtc )
	{
		int idx = 3*triIndices[i];

		const ZPoint& uv0 = uv[  idx];
		const ZPoint& uv1 = uv[++idx];
		const ZPoint& uv2 = uv[++idx];

		uvs[i] = WeightedSum( uv0, uv1, uv2, baryCoords[i] );
	}
}

// Lengyel’s Method
void
GetLocalAxes( const ZTriMesh& mesh, const ZIntArray& triIndices, const ZFloat3Array& baryCoords, const ZAxisArray& vAxes, ZAxisArray& axes, bool useOpenMP )
{
	const int numVertices  = mesh.numVertices();
	const int numTriangles = mesh.numTriangles();
	if( !numVertices || !numTriangles ) { axes.clear(); return; }

	const int numPtc = triIndices.length();
	if( ( baryCoords.length() != numPtc ) || ( vAxes.length() != numVertices ) )
	{
		cout << "Error@GetLocalAxes(): Invalid input data." << endl;
		axes.clear();
		return;
	}

	axes.setLength( numPtc );

	const ZPointArray& p    = mesh.p;
	const ZInt3Array&  v012 = mesh.v012;

	#pragma omp parallel for if( useOpenMP )
	FOR( i, 0, numPtc )
	{
		const ZInt3& tIdx = v012[ triIndices[i] ];
		const int& v0 = tIdx[0];
		const int& v1 = tIdx[1];
		const int& v2 = tIdx[2];

		const ZAxis& a0 = vAxes[v0];
		const ZAxis& a1 = vAxes[v1];
		const ZAxis& a2 = vAxes[v2];

		axes[i] = WeightedSum( a0, a1, a2, baryCoords[i] );
	}
}

void
GetLocalAxes( const ZTriMesh& mesh, const ZIntArray& triIndices, const ZFloat3Array& baryCoords, ZAxisArray& axes, bool useOpenMP )
{
	const int numVertices  = mesh.numVertices();
	const int numTriangles = mesh.numTriangles();
	if( !numVertices || !numTriangles ) { axes.clear(); return; }

	const int numPtc = triIndices.length();
	if( baryCoords.length() != numPtc )
	{
		cout << "Error@GetLocalAxes(): Invalid input data." << endl;
		axes.clear();
		return;
	}

	ZAxisArray vAxes; // tangent space per vertex
	GetTangentSpace( mesh, vAxes, true, useOpenMP );

	GetLocalAxes( mesh, triIndices, baryCoords, vAxes, axes, useOpenMP );
}

void
GetNormalizedPositions( const ZTriMesh& mesh, const ZIntArray& triIndices, const ZFloat3Array& baryCoords, ZPointArray& normalizedPositions, bool useOpenMP )
{
	const int numVertices  = mesh.numVertices();
	const int numTriangles = mesh.numTriangles();
	if( !numVertices || !numTriangles ) { normalizedPositions.clear(); return; }

	const int numPtc = triIndices.length();
	if( baryCoords.length() != numPtc )
	{
		cout << "Error@GetNormalizedPositions(): Invalid input data." << endl;
		normalizedPositions.clear();
		return;
	}

	normalizedPositions.setLength( numPtc );

	const ZPointArray& p    = mesh.p;
	const ZInt3Array&  v012 = mesh.v012;
	const ZPointArray& uv   = mesh.uv;

	ZBoundingBox bBox( p.boundingBox() );
	bBox.expand( 0.001f * bBox.maxWidth() );

	const ZPoint minPt = bBox.minPoint();
	const float maxWidth = bBox.maxWidth();

	if( ZAlmostZero( maxWidth ) ) {

		normalizedPositions.zeroize();

	} else {

		const float _maxWidth = 1 / maxWidth;

		#pragma omp parallel for if( useOpenMP )
		FOR( i, 0, numPtc )
		{
			const ZInt3& tIdx = v012[ triIndices[i] ];
			const int& v0 = tIdx[0];
			const int& v1 = tIdx[1];
			const int& v2 = tIdx[2];

			const ZPoint& p0 = p[v0];
			const ZPoint& p1 = p[v1];
			const ZPoint& p2 = p[v2];

			ZPoint& np = normalizedPositions[i];
			np = WeightedSum( p0, p1, p2, baryCoords[i] );

			( np.x -= minPt.x ) *= _maxWidth;
			( np.y -= minPt.y ) *= _maxWidth;
			( np.z -= minPt.z ) *= _maxWidth;
		}

	}
}

void
GetVertexDisplacement( const ZTriMesh& currMesh, const ZTriMesh& prevMesh, ZVectorArray& vVel, bool useOpenMP )
{
	const int numVertices = currMesh.numVertices();
	if( numVertices != prevMesh.numVertices() )
	{
		cout << "Error@GetMeshVertexDisplacement(): Invalid input data." << endl;
		return;
	}

	vVel.setLength( numVertices );

	#pragma omp parallel for if( useOpenMP )
	FOR( i, 0, numVertices )
	{
		vVel[i] = currMesh.p[i] - prevMesh.p[i];
	}
}

// Lengyel’s Method
void
GetTangentSpace( const ZTriMesh& mesh, ZAxisArray& axes, bool atVertex, bool useOpenMP )
{
	const int numVertices  = mesh.numVertices();
	const int numTriangles = mesh.numTriangles();
	const int numUVs       = mesh.numUVs();

	if( !numVertices || !numTriangles || !numUVs )
	{
		axes.clear();
		return;
	}

	const ZPointArray& p    = mesh.p;
	const ZInt3Array&  v012 = mesh.v012;
	const ZPointArray& uv   = mesh.uv;

	axes.setLength( numTriangles );

	#pragma omp parallel for if( useOpenMP )
	FOR( i, 0, numTriangles )
	{
		const ZInt3& tIdx = v012[i];
		const int& v0 = tIdx[0];
		const int& v1 = tIdx[1];
		const int& v2 = tIdx[2];

		const ZPoint& p0 = p[v0];
		const ZPoint& p1 = p[v1];
		const ZPoint& p2 = p[v2];

		const int i3 = i*3;

		const ZPoint& uv0 = uv[i3  ];
		const ZPoint& uv1 = uv[i3+1];
		const ZPoint& uv2 = uv[i3+2];

		const ZVector& dP1( p1 - p0 );
		const ZVector& dP2( p2 - p0 );

		const ZVector& dUV1( uv1 - uv0 );
		const ZVector& dUV2( uv2 - uv0 );

		const float r = 1.f / ( dUV1.x * dUV2.y - dUV1.y * dUV2.x + Z_EPS );

		ZAxis& a = axes[i];
		a.origin = Center( p0, p1, p2 );
		ZVector& ax = a.xAxis = ( ( dP1 * dUV2.y - dP2 * dUV1.y ) * r ).robustNormalize(); // tangent
		ZVector& ay = a.yAxis = ( ( dP2 * dUV1.x - dP1 * dUV2.x ) * r ).robustNormalize(); // bi-tangent
		ZVector& az = a.zAxis = RobustNormal( p0, p1, p2 );                                // normal
	}

	if( atVertex )
	{
		ZAxisArray vAxes( numVertices );

		ZFloatArray area;
		mesh.getTriangleAreas( area, useOpenMP );

		ZDoubleArray sum( numVertices );
		ZCharArray mask( numVertices );
		ZVectorArray dir( numVertices );

		FOR( i, 0, numTriangles )
		{
			const ZInt3& tIdx = v012[i];
			const int& v0 = tIdx[0];
			const int& v1 = tIdx[1];
			const int& v2 = tIdx[2];

			const ZPoint& p0 = p[v0];
			const ZPoint& p1 = p[v1];
			const ZPoint& p2 = p[v2];

			const float& ar = area[i];

			sum[v0] += ar;
			sum[v1] += ar;
			sum[v2] += ar;

			ZAxis& ta = axes[i];	// triangle axes
			ZPoint& X = ta.xAxis;	// tangent
			ZPoint& Y = ta.yAxis;	// bi-tangent
			ZPoint& Z = ta.zAxis;	// normal

			INCMUL( vAxes[v0].xAxis, ar, X );
			INCMUL( vAxes[v1].xAxis, ar, X );
			INCMUL( vAxes[v2].xAxis, ar, X );

			INCMUL( vAxes[v0].yAxis, ar, Y );
			INCMUL( vAxes[v1].yAxis, ar, Y );
			INCMUL( vAxes[v2].yAxis, ar, Y );

			INCMUL( vAxes[v0].zAxis, ar, Z );
			INCMUL( vAxes[v1].zAxis, ar, Z );
			INCMUL( vAxes[v2].zAxis, ar, Z );

			// just in case of singularities
			if( !mask[v0] ) { dir[v0]=p1-p0; mask[v0]=1; }
			if( !mask[v1] ) { dir[v1]=p2-p1; mask[v1]=1; }
			if( !mask[v2] ) { dir[v2]=p0-p2; mask[v2]=1; }
		}

		#pragma omp parallel for if( useOpenMP )
		FOR( i, 0, numVertices )
		{
			const float _sum = 1 / ( sum[i] + Z_EPS );

			ZAxis& a = vAxes[i];
			a.origin = p[i];
			a.xAxis *= _sum;
			a.yAxis *= _sum;
			a.zAxis *= _sum;
		}

		axes.exchange( vAxes );

		#pragma omp parallel for if( useOpenMP )
		FOR( i, 0, numVertices )
		{
			ZAxis& a = axes[i];
			ZVector& T = a.xAxis; // tangent
			ZVector& B = a.yAxis; // bi-tangent
			ZVector& N = a.zAxis; // normal

			N.robustNormalize();

			// Gram-Schmidt orthonormalize
			T = ( T - (N*(N*T)) ).robustNormalize();
			B = ( N ^ T ).robustNormalize();
		}
	}
}

void
GetPointsFromUVs( const ZTriMesh& mesh, const ZPointArray& uvs, ZIntArray& triIndices, ZFloat3Array& baryCoords, bool useOpenMP )
{
	const int numInputUVs = (int)uvs.size();
	if( numInputUVs <= 0 ) { return; }

	const int numMeshUVs = mesh.numUVs();
	if( numMeshUVs <= 0 ) { return; }

	const ZPointArray& meshUV = mesh.uv;

	const ZBoundingBox meshAABB( meshUV.boundingBox() );
	const ZPoint& minPt = meshAABB.minPoint();
	const ZPoint& maxPt = meshAABB.maxPoint();

	const ZBox2d domainAABB
	(
		ZDouble2( (double)minPt.x, (double)minPt.y ),
		ZDouble2( (double)maxPt.x, (double)maxPt.y )
	);

	// Get minimum edge length in uv space for estimating hash cell size for ZHashGrid2D.
	float minL=0.f, maxL=0.f;
	mesh.getMinMaxUVEdgeLength( minL, maxL );

	// Create a 2D hash on xz-plane.
	ZHashGrid2D hash( domainAABB, 2*maxL, ZPow2(1024) );

	// Register uv triangles into the hash hash.
	const int nTri = mesh.numTriangles();
	FOR( i, 0, nTri )
	{
		const int& v0 = i*3;;
		const int& v1 = v0+1;
		const int& v2 = v0+2;

		ZBox2d aabb;
		aabb.expand( ZDouble2( (double)meshUV[v0].x, (double)meshUV[v0].y ) );
		aabb.expand( ZDouble2( (double)meshUV[v1].x, (double)meshUV[v1].y ) );
		aabb.expand( ZDouble2( (double)meshUV[v2].x, (double)meshUV[v2].y ) );

		hash.add( i, aabb );
	}

	triIndices.setLength( numInputUVs );
	baryCoords.setLength( numInputUVs );
	ZCharArray finded( numInputUVs );

	#pragma omp parallel for if( useOpenMP )
	FOR( i, 0, numInputUVs )
	{
		const ZDouble2 uv( (double)uvs[i].x, (double)uvs[i].y );

		const vector<int>& candidates = hash.items( uv );

		const int numCandidates = (int)candidates.size();

		FOR( j, 0, numCandidates )
		{
			const int v0 = 3*candidates[j];
			const int v1 = v0+1;
			const int v2 = v0+2;

			const ZPoint& uv0 = meshUV[v0];
			const ZPoint& uv1 = meshUV[v1];
			const ZPoint& uv2 = meshUV[v2];

			if( BaryCoords( uvs[i], uv0,uv1,uv2, 0, baryCoords[i] ) )
			{
				triIndices[i] = candidates[j];
				finded[i] = (char)1;

				break;
			}
		}
	}

	ZIntArray delList;

	FOR( i, 0, numInputUVs )
	{
		if( finded[i] ) { continue; }
		delList.push_back( i );
	}

	triIndices.remove( delList );
	baryCoords.remove( delList );
}

ZELOS_NAMESPACE_END

