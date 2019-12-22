//----------------------//
// ZTriMeshDistTree.cpp //
//-------------------------------------------------------//
// author: Taeyong Kim @ nVidia                          //
//         Wanho Choi @ Dexter Studios                   //
// last update: 2016.02.04                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZTriMeshDistTree::ZTriMeshDistTree( int maxLevel, int maxElements )
: _meshPtr(NULL), _maxLevel(maxLevel), _maxElements(maxElements)
{
}

ZTriMeshDistTree::ZTriMeshDistTree( const ZTriMesh& mesh, int maxLevel, int maxElements )
: _meshPtr(NULL), _maxLevel(maxLevel), _maxElements(maxElements)
{
	set( mesh );
}

void
ZTriMeshDistTree::reset()
{
	_meshPtr     = (ZTriMesh*)NULL;
	_maxLevel    = 10;
	_maxElements = 10;
	_bBoxes      .clear();
	_cells       .clear();
}

bool
ZTriMeshDistTree::set( const ZTriMesh& mesh )
{
	_meshPtr = (ZTriMesh*)&mesh;
	return _initialize();
}

bool
ZTriMeshDistTree::_initialize()
{
    _cells.clear();

	if( !_meshPtr ) { return false; }

	const ZPointArray& vPos = _meshPtr->p;
	const int numVertices = vPos.length();
	if( !numVertices ) { return true; }

	const ZInt3Array& v012 = _meshPtr->v012;
	const int numTriangles = v012.length();
	if( !numTriangles ) { return true; }

	_cells.reserve( numTriangles ); // reserve some memories approximately for efficiency
	_bBoxes.setLength( numTriangles );

	// calculate per triangle bounding box
	FOR( i, 0, numTriangles )
	{
		ZBoundingBox& bBox = _bBoxes[i];
		bBox.reset();

		const ZInt3& t = v012[i];

		bBox.expand( vPos[ t[0] ] );
		bBox.expand( vPos[ t[1] ] );
		bBox.expand( vPos[ t[2] ] );

		bBox.expand();
	}

	// add root cell
	const int root = _addCell(0);
	ZTriMeshDistTree::Cell& rootCell = _cells[root];

	// append all the triangles to the root
	rootCell.ids.setLength( numTriangles );
	FOR( i, 0, numTriangles ) { rootCell.ids[i] = i; }

	// add all the points to the bounding box of the root
	FOR( i, 0, numVertices ) { rootCell.bBox.expand( vPos[i] ); }
	rootCell.bBox.expand();

	// we use deque to implement bread-first subdivision (helps memory locality a bit)
	deque<int> splitQueue;
	splitQueue.push_back( root );

	while( !splitQueue.empty() )
	{
		// full the first cell in the queue
		const int cellToSplit = *splitQueue.begin();
		splitQueue.pop_front();

		// do the split until the stop condition is met.
		if( _subdividable( cellToSplit ) )
		{
			_splitCell( cellToSplit );

			const int& lChild = _cells[cellToSplit].left;
			if( lChild >= 0 ) { splitQueue.push_back( lChild ); }

			const int& rChild = _cells[cellToSplit].right;
			if( rChild >= 0 ) { splitQueue.push_back( rChild ); }
		}
	}

	return true;
}

void
ZTriMeshDistTree::_splitCell( int cellId )
{
	const int pLevel = _cells[cellId].level; // pLevel: level of the parent
	const int lChild = _addCell( pLevel+1 );
	const int rChild = _addCell( pLevel+1 );

	// set these guys as children of the parent cell
	_cells[cellId].left  = lChild;
	_cells[cellId].right = rChild;

	// make a link to the parent
	_cells[lChild].parent = cellId;
	_cells[rChild].parent = cellId;

	// split the parent's box into two boxes of children
	_cells[cellId].bBox.split( _cells[lChild].bBox, _cells[rChild].bBox );

	// now assign element and box id
	const int numTrianglesInThisCell = _cells[cellId].ids.length();
	FOR( i, 0, numTrianglesInThisCell )
	{
		const int& elemId = _cells[cellId].ids[i];

		// get the bounding box of this element
		const ZBoundingBox& elemBox = _bBoxes[elemId];
			
		// If this box intersects with box of lChild, add it to list there.
		if( _cells[lChild].bBox.intersects(elemBox) ) { _cells[lChild].ids.push_back(elemId); }
		// If this box intersects with box of rChild, add it to list there.
		if( _cells[rChild].bBox.intersects(elemBox) ) { _cells[rChild].ids.push_back(elemId); }
	}
}

const ZTriMesh&
ZTriMeshDistTree::mesh() const
{
	return (*_meshPtr);
}

float
ZTriMeshDistTree::getClosestPoint( const ZPoint& pt, ZPoint& closestPt, int& closestTri, float maxDist ) const
{
	bool success = true;

	if( !_meshPtr ) { success = false; }
	if( !_meshPtr->numTriangles() ) { success = false; }

	ZTriMeshDistTree::Data data;
	data.dist = maxDist;

	_compute( 0, pt, data ); // search from root cell
	if( !data.valid() ) { success = false; }

	if( success ) {

		closestPt  = data.pt;
		closestTri = data.id;
		return data.dist;

	} else {

		closestPt.set( Z_LARGE, Z_LARGE, Z_LARGE );
		closestTri = -1;
		return Z_LARGE;

	}
}

float
ZTriMeshDistTree::getClosestPoint( const ZPoint& pt, int& closestTri, float& a, float& b, float maxDist ) const
{
	bool success = true;

	if( !_meshPtr ) { success = false; }
	if( !_meshPtr->numTriangles() ) { success = false; }

	ZTriMeshDistTree::Data data;
	data.dist = maxDist;

	_compute( 0, pt, data ); // search from root cell
	if( !data.valid() ) { success = false; }

	if( success ) {

		closestTri = data.id;
		a          = data.a;
		b          = data.b;

		return data.dist;

	} else {

		closestTri = -1;
		a          = 0.f;
		b          = 0.f;

		return Z_LARGE;

	}
}

float
ZTriMeshDistTree::getClosestPoint( const ZPoint& pt, ZPoint& closestPt, int& closestTri, float& a, float& b, float maxDist ) const
{
	bool success = true;

	if( !_meshPtr ) { success = false; }
	if( !_meshPtr->numTriangles() ) { success = false; }

	ZTriMeshDistTree::Data data;
	data.dist = maxDist;

	_compute( 0, pt, data ); // search from root cell
	if( !data.valid() ) { success = false; }

	if( success ) {

		closestPt  = data.pt;
		closestTri = data.id;
		a          = data.a;
		b          = data.b;

		return data.dist;

	} else {

		closestPt.set( Z_LARGE, Z_LARGE, Z_LARGE );
		closestTri = -1;
		a          = 0.f;
		b          = 0.f;

		return Z_LARGE;

	}
}

void
ZTriMeshDistTree::getTriangles( const ZBoundingBox& bBox, ZIntArray& triangles, bool accurate ) const
{
	triangles.clear();

	if( !_meshPtr ) { return; }
	if( !_meshPtr->numTriangles() ) { return; }

	const int cellId = _findDeepestCellIncludingBox( 0, bBox );
	if( cellId < 0 ) { return; }

	if( !_meshPtr ) { return; }
	const ZPointArray& vPos = _meshPtr->p;
	const ZInt3Array& v012 = _meshPtr->v012;

	if( accurate ) {

		ZIntArray candidates( _cells[cellId].ids );
		const int numCandidates = candidates.length();
		FOR( i, 0, numCandidates )
		{
			const int triIdx = candidates[i];
			const int& v0 = v012[triIdx][0];
			const int& v1 = v012[triIdx][1];
			const int& v2 = v012[triIdx][2];
			const ZPoint& p0 = vPos[v0];
			const ZPoint& p1 = vPos[v1];
			const ZPoint& p2 = vPos[v2];
			if( bBox.intersectsWithTriangle( p0,p1,p2 ) )
			{
				triangles.push_back( triIdx );
			}
		}

	} else {

		triangles = _cells[cellId].ids;

	}
}

void
ZTriMeshDistTree::_compute( int cellId, const ZPoint& pt, ZTriMeshDistTree::Data& data ) const
{
	const ZTriMeshDistTree::Cell& cell = _cells[cellId];

	// current distance is already closer than this cell
	if( data.dist <= cell.bBox.distanceFromOutside(pt) ) { return; }

	if( !_meshPtr ) { return; }
	const ZPointArray& vPos = _meshPtr->p;
	const ZInt3Array&  v012 = _meshPtr->v012;

	// If this is leaf, compute distance to each element
	if( cell.isLeaf() ) {

		const int numTrianglesInThisCell = _cells[cellId].ids.length();
		FOR( i, 0, numTrianglesInThisCell )
		{
			const int& tId = _cells[cellId].ids[i]; // i-th triangle index in this cell
			const ZInt3& t = v012[tId];

			const ZPoint& p0 = vPos[ t[0] ];
			const ZPoint& p1 = vPos[ t[1] ];
			const ZPoint& p2 = vPos[ t[2] ];

			ZFloat3 coeff;
			ZPoint closestPt( ClosestPointOnTriangle( pt, p0,p1,p2, coeff ) );
			const float dist = pt.distanceTo( closestPt );

			// if this triangle is closer, update the distance information
			if( dist < data.dist )
			{
				data.id   = tId;
				data.dist = dist;
				data.pt   = closestPt;
				data.a    = coeff[0];
				data.b    = coeff[1];
			}
		}

	} else { // for non-leaf node, visit closer child first

		const ZTriMeshDistTree::Cell& lChild = _cells[cell.left ];
		const ZTriMeshDistTree::Cell& rChild = _cells[cell.right];

		const float lBoundSq = lChild.bBox.distanceFromOutside( pt, true );
		const float rBoundSq = rChild.bBox.distanceFromOutside( pt, true );

		if( lBoundSq < rBoundSq ) {
			_compute( cell.left,  pt, data );
			_compute( cell.right, pt, data );
		} else {
			_compute( cell.right, pt, data );
			_compute( cell.left,  pt, data );
		}

	}
}

int
ZTriMeshDistTree::_findDeepestCellIncludingBox( int cellId, const ZBoundingBox& bBox ) const
{
	const ZTriMeshDistTree::Cell& cell = _cells[cellId];

	if( cell.isLeaf() ) {

		return cellId;

	} else {

		if( !cell.bBox.intersects(bBox) ) { return -1; }

		const ZTriMeshDistTree::Cell& lChild = _cells[cell.left ];
		const ZTriMeshDistTree::Cell& rChild = _cells[cell.right];

		const bool lIntersect = lChild.bBox.intersects(bBox);
		const bool rIntersect = rChild.bBox.intersects(bBox);

		if( lIntersect && rIntersect ) { return cellId; }
		else if( lIntersect ) { _findDeepestCellIncludingBox( cell.left,  bBox ); }
		else if( rIntersect ) { _findDeepestCellIncludingBox( cell.right, bBox ); }
		else { return -1; }

	}

	return cellId;
}

int
ZTriMeshDistTree::maxLevel() const
{
	return _maxLevel;
}

int
ZTriMeshDistTree::numVertices() const
{
	if( !_meshPtr ) { return 0; }
	return _meshPtr->numVertices();
}

int
ZTriMeshDistTree::numTriangles() const
{
	if( !_meshPtr ) { return 0; }
	return _meshPtr->numTriangles();
}

int
ZTriMeshDistTree::numCells() const
{
	return (int)_cells.size();
}

int
ZTriMeshDistTree::numLeafCells() const
{
	int sum = 0;

	FOR( i, 0, _cells.size() )
	{
		if( _cells[i].isLeaf() ) { ++sum; }
	}

	return sum;
}

const ZBoundingBoxArray&
ZTriMeshDistTree::boundingBoxes() const
{
	return _bBoxes;
}

float
ZTriMeshDistTree::averageNumTriangles() const
{
	int sum = 0;
	int numCells = 0;

	FOR( i, 0, _cells.size() )
	{
		if( _cells[i].isLeaf() )
		{
			sum += _cells[i].ids.length();
			++numCells;
		}
	}

	if( !numCells ) { return 0; } // avoid division by zero

	return (sum/(float)numCells);
}

ZELOS_NAMESPACE_END

