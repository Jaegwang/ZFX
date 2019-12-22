//---------------------//
// ZPointsDistTree.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2017.11.01                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZPointsDistTree::ZPointsDistTree()
{}

ZPointsDistTree::ZPointsDistTree( const ZPointArray& points )
{
	ZPointsDistTree::setPoints( points );
}

void
ZPointsDistTree::clear()
{
	_ids.clear();
	_points.clear();
	_bBox.reset();
}

const ZPointArray&
ZPointsDistTree::points() const
{
	return _points;
}

const ZBoundingBox&
ZPointsDistTree::boundingBox() const
{
	return _bBox;
}

void
ZPointsDistTree::setPoints( const ZPointArray& points, const ZIntArray* ids )
{
    ZPointsDistTree::clear();

	const int n = points.length();

	if( n <= 0 ) { return; }

    const bool hasInputIds = ids ? true : false;

    if( hasInputIds )
    {
        if( n != ids->length() )
        {
            cout << "Error@ZPointsDistTree::setPoints(): Invalid input." << endl;
            return;
        }
    }
    else
    {
        _ids.resize( n );

	    FOR( i, 0, n )
        {
            _ids[i] = i;
        }
    }

	_points = points;

	_bBox = points.boundingBox();
	_bBox.expand();

	_sort();
}

void
ZPointsDistTree::addPoints( const ZPointArray& points, const ZIntArray* ids )
{
	const int old_n = _points.size();
	const int add_n = points.size();
	const int new_n = old_n + add_n;

    const bool hasInputIds = ids ? true : false;

    if( hasInputIds )
    {
        _ids.append( *ids );
    }
    else
    {
        _ids.resize( new_n );

        FOR( i, old_n, new_n )
        {
            _ids[i] = i;
        }
    }

    _points.append( points );

    _bBox.expand( points.boundingBox() );
}

void
ZPointsDistTree::finalizeAddingPoints()
{
	_bBox.expand();

	_sort();
}

void
ZPointsDistTree::_sort()
{
	const int n = _points.length();
	if( n <= 1 ) { return; }

	_sortSubtree( 0, n, 0 );

	ZPointArray newPoints( n );

	FOR( i, 0, n )
    {
        newPoints[i] = _points[ _ids[i] ];
    }

	_points.exchange( newPoints );
}

void
ZPointsDistTree::_sortSubtree( int n, int size, int j )
{
	int left=0, right=0;
	_computeSubtreeSizes( size, left, right );

	std::nth_element( &_ids[n], &_ids[n+left], &_ids[n+size], ComparePointsById( &_points[0][j] ) );

	ZSwap( _ids[n], _ids[n+left] );

	if( left <= 1 ) { return; }
	j = (j+1)%3;

	_sortSubtree( n+1, left, j );

	if( right <= 1 ) { return; }

	_sortSubtree( n+left+1, right, j );
}

void
ZPointsDistTree::_computeSubtreeSizes( int size, int& left, int& right ) const
{
	const bool balanced = ( ( (size+1) & size ) == 0 );

	if( balanced )
    {
		left = right = size>>1;
	}
    else if( size == 2 )
    {
		left = 1;
		right = 0;
	}
    else
    {
		int i = 0;
		for( int c=size; c!=1; c>>=1 ) { ++i; }
		left = (1<<i)-1;
		right = size - left - 1;
	}
}

float
ZPointsDistTree::_insertToHeap( int* result, float* dist2, int heap_size, int new_id, float new_dist2 ) const
{
	// new_distSQ >= distSQ[0] !!!

	int current_parent = 0;

	for(;;)
	{
		const int left  = 2*current_parent+1;
		const int right = 2*current_parent+2;

		int index_of_largest = current_parent;

		if( left >= heap_size )
        {
            break;
        }
		else if( right >= heap_size || dist2[left] > dist2[right] )
        {
            index_of_largest = left;
        }
		else
        {
            index_of_largest = right;
        }

		if( new_dist2 > dist2[index_of_largest] )
        {
            break;
        }

		ZSwap( result[index_of_largest], result[current_parent] );
		ZSwap( dist2[index_of_largest], dist2[current_parent] );

		current_parent = index_of_largest;
	}

	dist2[current_parent] = new_dist2;
	result[current_parent] = new_id;

	return dist2[0];
}

float
ZPointsDistTree::_buildHeap( int* result, float* dist2, int heap_size ) const
{
	const int max_non_leaf_index = heap_size/2-1;

	for( int subtreeParent=max_non_leaf_index; subtreeParent>=0; --subtreeParent )
	{
		int current_parent = subtreeParent;

		while( current_parent <= max_non_leaf_index )
		{
			const int left  = 2*current_parent+1;
			const int right = 2*current_parent+2;

			int largest = current_parent;

			if( left < heap_size )
            {
			    if( dist2[left] > dist2[largest] )
                {
                    largest = left;
                }
            }

			if( right < heap_size )
            {
			    if( dist2[right] > dist2[largest] )
                {
                    largest = right;
                }
            }

			if( largest == current_parent )
            {
                break;
            }

			ZSwap( result[largest], result[current_parent] );
			ZSwap( dist2[largest], dist2[current_parent] );

			current_parent = largest;
		}
	}

	return dist2[0];
}

void
ZPointsDistTree::findPoints( const ZBoundingBox& box, ZIntArray& result ) const
{
	if( !_points.size() ) { return; }
	if( !box.intersects(_bBox) ) { return; }

	_findPoints( box, 0, _points.size(), 0, result );

	const int n = result.size();

	FOR( i, 0, n )
    {
        result[i] = _ids[ result[i] ];
    }
}

void
ZPointsDistTree::_findPoints( const ZBoundingBox& box, int n, int size, int j, ZIntArray& result ) const
{
	const ZPoint& p = _points[n];

	if( box.contains(p) )
    {
        result.push_back(n);
    }

	if( size == 1 ) { return; }

	int left=0, right=0;
	_computeSubtreeSizes( size, left, right );

	const int nextj = (j+1)%3;

	if( p[j] >= box.minPoint()[j] )
	{
		_findPoints( box, n+1, left, nextj, result );
	}

	if( right && p[j] <= box.maxPoint()[j] )
	{
		_findPoints( box, n+left+1, right, nextj, result );
	}
}

float
ZPointsDistTree::findNPoints( const ZPoint& p, int nPoints, float maxSearchRadius, ZIntArray& result, ZFloatArray& dist2 ) const
{
	result.resize( nPoints );
	dist2.resize( nPoints );

	float finalSearchRadius = maxSearchRadius;

	const int size = _findNPoints( p, nPoints, maxSearchRadius, &result[0], &dist2[0], finalSearchRadius );

	result.resize( size );
	dist2.resize( size );

	FOR( i, 0, size )
    {
        result[i] = _ids[ result[i] ];
    }

	return finalSearchRadius;
}

int
ZPointsDistTree::_findNPoints( const ZPoint& p, int nPoints, float maxRadius, int* result, float* dist2, float& finalSearchRadius ) const
{
	if( !_points.size() || nPoints<1 ) { return 0; }

	const float radius2 = ZPow2( maxRadius );

	ZPointsDistTree::QueryData query( result, dist2, p, nPoints, radius2 );

	_findNPoints( query, 0, _points.size(), 0 );

	finalSearchRadius = sqrtf( query.maxRadius2 );

	return query.foundCount;
}

void
ZPointsDistTree::findClosestPoint( const ZPoint& p, int& closestPointId, float& closestDist2 ) const
{
	if( !_points.size() )
	{
		closestPointId = -1;
		closestDist2 = Z_LARGE;
		return;
	}

	ZIntArray result;
	ZFloatArray dist2;

	ZPointsDistTree::findNPoints( p, 1, Z_LARGE, result, dist2 );

	closestPointId = result[0];
	closestDist2 = dist2[0];
}

void
ZPointsDistTree::_findNPoints( ZPointsDistTree::QueryData& query, int n, int size, int j ) const
{
	const ZPoint& p = _points[n];

	if( size > 1 )
	{
		const float axis_distance = query.point[j] - p[j];

		int left=0, right=0;
		_computeSubtreeSizes( size, left, right );

		const int nextj = (j+1)%3;

		if( axis_distance > 0 )
        {
			if( right )
            {
                _findNPoints( query, n+left+1, right, nextj );
            }

			if( ZPow2(axis_distance) < query.maxRadius2 )
            {
                _findNPoints( query, n+1, left, nextj );
            }
		}
        else
        {
			_findNPoints( query, n+1, left, nextj );

			if( right && ZPow2(axis_distance) < query.maxRadius2 )
            {
                _findNPoints( query, n+left+1, right, nextj );
            }
		}
	}

	const float dist2 = p.squaredDistanceTo( query.point );

	if( dist2 < query.maxRadius2 )
	{
		if( query.foundCount < query.maxCount )
        {
			query.result[query.foundCount] = n;
			query.dist2[query.foundCount] = dist2;
			++query.foundCount;

			if( query.foundCount == query.maxCount )
            {
                query.maxRadius2 = _buildHeap( query.result, query.dist2, query.foundCount );
            }
		}
        else
        {
			query.maxRadius2 = _insertToHeap( query.result, query.dist2, query.foundCount, n, dist2 );
		}
	}
}

ostream&
operator<<( ostream& os, const ZPointsDistTree& object )
{
	os << "<ZPointsDistTree>" << endl;
	os << endl;

	return os;
}

ZELOS_NAMESPACE_END

