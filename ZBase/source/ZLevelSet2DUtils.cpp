//----------------------//
// ZLevelSet2DUtils.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2018.03.08                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

void
Voxelize( const ZTriMesh& mesh, ZScalarField2D& lvs, bool useOpenMP )
{
	lvs.fill( Z_LARGE );

	const int numV = mesh.numVertices();
	const int numT = mesh.numTriangles();
	if( numV<3 || numT<1 ) { return; }

	const int numElements = lvs.numElements();
	if( numElements<1 ) { return; }

	ZIntArray trianglesToBeDeleted;
	trianglesToBeDeleted.reserve( numT/2 );

	const float yLevel = lvs.getY();
	float yMin=0, yMax=0;

	FOR( iTri, 0, numT )
	{
		const ZInt3& tIdx = mesh.v012[iTri];
		const int& v0=tIdx[0];   const ZPoint& p0=mesh.p[v0];
		const int& v1=tIdx[1];   const ZPoint& p1=mesh.p[v1];
		const int& v2=tIdx[2];   const ZPoint& p2=mesh.p[v2];

		ZMinMax( p0.y, p1.y, p2.y, yMin, yMax );

		yMin -= Z_EPS;
		yMax += Z_EPS;

		if( yMin<yLevel && yLevel<yMax ) { continue; }

		trianglesToBeDeleted.push_back( iTri );
	}

	ZTriMesh testMesh( mesh );
	testMesh.deleteTriangles( trianglesToBeDeleted );
	ZTriMeshDistTree distTree( testMesh );

	if( testMesh.numVertices() <3 ) { return; }
	if( testMesh.numTriangles()<1 ) { return; }

	#pragma omp parallel for if( numElements>1000 && useOpenMP )
	PER_EACH_ELEMENT_2D( lvs )

		const int idx = lvs.index(i,k);
		const ZPoint p( lvs.position(i,k) );

		ZPoint closestPt;
		int closestTriangle = -1;
		distTree.getClosestPoint( p, closestPt, closestTriangle );
		if( closestTriangle<0 ) { continue; }

		const ZInt3& tIdx = testMesh.v012[ closestTriangle ];
		const int& v0=tIdx[0];   const ZPoint& p0=testMesh.p[v0];
		const int& v1=tIdx[1];   const ZPoint& p1=testMesh.p[v1];
		const int& v2=tIdx[2];   const ZPoint& p2=testMesh.p[v2];

		const ZVector nrm( (p1-p0)^(p2-p0) );
		const ZVector dir( p - closestPt );

		lvs[idx] = ZSgn(nrm*dir) * dir.length();

	END_PER_EACH_2D

	lvs.setMinMax();
}

void
Voxelize( const ZTriMesh& mesh, const ZVectorArray& vVel, ZScalarField2D& lvs, ZVectorField2D& vel, bool useOpenMP )
{
	if( lvs.directComputable( vel ) )
	{
		cout << "error@voxelize(): invalid input data." << endl;
		return;
	}

	lvs.fill( Z_LARGE );
	vel.zeroize();

	const int numV = mesh.numVertices();
	const int numT = mesh.numTriangles();
	if( numV<3 || numT<1 ) { return; }

	const int numElements = lvs.numElements();
	if( numElements<1 ) { return; }

	ZIntArray trianglesToBeDeleted;
	trianglesToBeDeleted.reserve( numT/2 );

	const float yLevel = lvs.getY();
	float yMin=0, yMax=0;

	FOR( iTri, 0, numT )
	{
		const ZInt3& tIdx = mesh.v012[iTri];
		const int& v0=tIdx[0];   const ZPoint& p0=mesh.p[v0];
		const int& v1=tIdx[1];   const ZPoint& p1=mesh.p[v1];
		const int& v2=tIdx[2];   const ZPoint& p2=mesh.p[v2];

		ZMinMax( p0.y, p1.y, p2.y, yMin, yMax );

		yMin -= Z_EPS;
		yMax += Z_EPS;

		if( yMin<yLevel && yLevel<yMax ) { continue; }

		trianglesToBeDeleted.push_back( iTri );
	}

	ZTriMesh testMesh( mesh );
	testMesh.deleteTriangles( trianglesToBeDeleted );
	ZTriMeshDistTree distTree( testMesh );

	if( testMesh.numVertices() <3 ) { return; }
	if( testMesh.numTriangles()<1 ) { return; }

	#pragma omp parallel for if( numElements>1000 && useOpenMP )
	PER_EACH_ELEMENT_2D( lvs )

		const int idx = lvs.index(i,k);
		const ZPoint p( lvs.position(i,k) );

		ZPoint closestPt;
		int closestTriangle = -1;
		distTree.getClosestPoint( p, closestPt, closestTriangle );
		if( closestTriangle<0 ) { continue; }

		const ZInt3& tIdx = testMesh.v012[ closestTriangle ];
		const int& v0=tIdx[0];   const ZPoint& p0=testMesh.p[v0];
		const int& v1=tIdx[1];   const ZPoint& p1=testMesh.p[v1];
		const int& v2=tIdx[2];   const ZPoint& p2=testMesh.p[v2];

		const ZVector nrm( (p1-p0)^(p2-p0) );
		const ZVector dir( p - closestPt );

		lvs[idx] = ZSgn(nrm*dir) * dir.length();
		vel[idx] = WeightedSum( vVel[v0], vVel[v1], vVel[v2], 0.333333f, 0.333333f, 0.333333f );

	END_PER_EACH_2D

	lvs.setMinMax();
}

ZELOS_NAMESPACE_END

