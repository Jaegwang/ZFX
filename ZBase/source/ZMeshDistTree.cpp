//-------------------//
// ZMeshDistTree.cpp //
//-------------------------------------------------------//
// author: Taeyong Kim @ nVidia                          //
//         Wanho Choi @ Dexter Studios                   //
// last update: 2018.03.08                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZMeshDistTree::ZMeshDistTree( int maxLevel, int maxElements )
: _maxLevel(maxLevel), _maxElements(maxElements)
{}

ZMeshDistTree::ZMeshDistTree( const ZMesh& mesh, int maxLevel, int maxElements )
: _maxLevel(maxLevel), _maxElements(maxElements)
{
	set( mesh );
}

void
ZMeshDistTree::reset()
{
	_maxLevel    = 10;
	_maxElements = 10;
	_points      .clear();
	_triangles   .clear();
	_cells       .clear();
	_bBoxes      .clear();
}

bool
ZMeshDistTree::set( const ZMesh& mesh )
{
	mesh.getTriangleIndices( _triangles );
	_points = mesh.points();

	return _initialize();
}

bool
ZMeshDistTree::_initialize()
{
    _cells.clear();

	const int numVertices = _points.length();
	if( !numVertices ) { return true; }

	const int numTriangles = _triangles.length();
	if( !numTriangles ) { return true; }

	_cells.reserve( numTriangles ); // reserve some memories approximately for efficiency
	_bBoxes.setLength( numTriangles );

	// calculate per triangle bounding box
	FOR( i, 0, numTriangles )
	{
		ZBoundingBox& bBox = _bBoxes[i];
		bBox.reset();

		const ZInt3& tId = _triangles[i];

		bBox.expand( _points[ tId[0] ] );
		bBox.expand( _points[ tId[1] ] );
		bBox.expand( _points[ tId[2] ] );

		bBox.expand();
	}

	// add root cell
	const int root = _addCell(0);
	ZMeshDistTree::Cell& rootCell = _cells[root];

	// append all the triangles to the root
	rootCell.ids.setLength( numTriangles );
	FOR( i, 0, numTriangles ) { rootCell.ids[i] = i; }

	// add all the points to the bounding box of the root
	FOR( i, 0, numVertices ) { rootCell.bBox.expand( _points[i] ); }
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

int
ZMeshDistTree::_addCell( int level )
{
	_cells.push_back( ZMeshDistTree::Cell( level ) );
	return ( (int)_cells.size()-1 );
}

float
ZMeshDistTree::averageNumTriangles() const
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

int
ZMeshDistTree::maxLevel() const
{
	return _maxLevel;
}

int
ZMeshDistTree::numCells() const
{
	return (int)_cells.size();
}

int
ZMeshDistTree::numLeafCells() const
{
	int sum = 0;

	FOR( i, 0, _cells.size() )
	{
		if( _cells[i].isLeaf() ) { ++sum; }
	}

	return sum;
}

const ZPointArray&
ZMeshDistTree::points() const
{
	return _points;
}

const ZInt3Array&
ZMeshDistTree::triangles() const
{
	return _triangles;
}

const ZBoundingBoxArray&
ZMeshDistTree::boundingBoxes() const
{
	return _bBoxes;
}

bool
ZMeshDistTree::_subdividable( int cellId ) const
{
	if( cellId >= (int)_cells.size() ) { return false; }
	if( _cells[cellId].level >= _maxLevel ) { return false; }
	if( _cells[cellId].ids.length() <= _maxElements ) { return false; }
	return true;
}

void
ZMeshDistTree::_splitCell( int cellId )
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

float
ZMeshDistTree::getClosestPoint( const ZPoint& pos, ZPoint& closestPoint, int& closestTriangle, float maxDist ) const
{
	if( _triangles.empty() ) { closestPoint.zeroize(); return Z_LARGE; }

	const int numTriangles = _triangles.length();

	ZMeshDistTree::Data distData;
	distData.dist = maxDist;

	_compute( 0, pos, distData ); // search from root cell

	if( distData.valid() ) {
		closestPoint = distData.pt;
		closestTriangle = distData.id;
		return distData.dist;
	} else {
		closestPoint.set( Z_LARGE );
		closestTriangle=-1;
		return Z_LARGE;
	}
}

void
ZMeshDistTree::getTriangles( const ZBoundingBox& bBox, ZIntArray& triangles, bool accurate ) const
{
	triangles.clear();

	if( _triangles.empty() ) { return; }

	const int cellId = _findDeepestCellIncludingBox( 0, bBox );
	if( cellId < 0 ) { return; }

	if( accurate ) {

		ZIntArray candidates( _cells[cellId].ids );
		const int numCandidates = candidates.length();
		FOR( i, 0, numCandidates )
		{
			const int triIdx = candidates[i];
			const int& v0 = _triangles[triIdx][0];
			const int& v1 = _triangles[triIdx][1];
			const int& v2 = _triangles[triIdx][2];
			const ZPoint& p0 = _points[v0];
			const ZPoint& p1 = _points[v1];
			const ZPoint& p2 = _points[v2];
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
ZMeshDistTree::_compute( int cellId, const ZPoint& pos, ZMeshDistTree::Data& distData ) const
{
	const ZMeshDistTree::Cell& cell = _cells[cellId];

	// current distance is already closer than this cell
	if( distData.dist <= cell.bBox.distanceFromOutside(pos) ) { return; }

	// If this is leaf, compute distance to each element
	if( cell.isLeaf() ) {

		const int numTrianglesInThisCell = _cells[cellId].ids.length();
		FOR( i, 0, numTrianglesInThisCell )
		{
			const int tId = _cells[cellId].ids[i];
			const ZInt3& t = _triangles[tId];

			const ZPoint& p0 = _points[ t[0] ];
			const ZPoint& p1 = _points[ t[1] ];
			const ZPoint& p2 = _points[ t[2] ];

			ZFloat3 coeff;
			ZPoint closestPt = ClosestPointOnTriangle( pos, p0,p1,p2, coeff );
			const float dist = pos.distanceTo( closestPt );

			// if this triangle is closer, update the distance information
			if( dist < distData.dist )
			{
				distData.id   = tId;
				distData.dist = dist;
				distData.pt   = closestPt;
			}
		}

	} else { // for non-leaf node, visit closer child first

		const ZMeshDistTree::Cell& lChild = _cells[cell.left ];
		const ZMeshDistTree::Cell& rChild = _cells[cell.right];

		const float lBoundSq = lChild.bBox.distanceFromOutside( pos, true );
		const float rBoundSq = rChild.bBox.distanceFromOutside( pos, true );

		if( lBoundSq < rBoundSq ) {
			_compute( cell.left,  pos, distData );
			_compute( cell.right, pos, distData );
		} else {
			_compute( cell.right, pos, distData );
			_compute( cell.left,  pos, distData );
		}

	}
}

int
ZMeshDistTree::_findDeepestCellIncludingBox( int cellId, const ZBoundingBox& bBox ) const
{
	const ZMeshDistTree::Cell& cell = _cells[cellId];

	if( cell.isLeaf() ) {

		return cellId;

	} else {

		if( !cell.bBox.intersects(bBox) ) { return -1; }

		const ZMeshDistTree::Cell& lChild = _cells[cell.left ];
		const ZMeshDistTree::Cell& rChild = _cells[cell.right];

		const bool lIntersect = lChild.bBox.intersects(bBox);
		const bool rIntersect = rChild.bBox.intersects(bBox);

		if( lIntersect && rIntersect ) { return cellId; }
		else if( lIntersect ) { _findDeepestCellIncludingBox( cell.left,  bBox ); }
		else if( rIntersect ) { _findDeepestCellIncludingBox( cell.right, bBox ); }
		else { return -1; }

	}

	return cellId;
}

ZELOS_NAMESPACE_END

