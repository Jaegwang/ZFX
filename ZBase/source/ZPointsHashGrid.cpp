//---------------------//
// ZPointsHashGrid.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2017.12.19                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZPointsHashGrid::ZPointsHashGrid()
: _numBuckets(1), _h(1)
{
	_numBuckets = 1;

	_list = new ZPointsHashGrid::Item*[_numBuckets];
	_list[0] = (ZPointsHashGrid::Item*)NULL;

	_h = 1;
}

ZPointsHashGrid::ZPointsHashGrid( int numBuckets, float voxelSize )
{
	_numBuckets = ZMax( numBuckets, 1 );

	_list = new ZPointsHashGrid::Item*[_numBuckets];
	FOR( i, 0, _numBuckets )
	{
		_list[i] = (ZPointsHashGrid::Item*)NULL;
	}

	_h = ZMax( voxelSize, 0.001f );
}

ZPointsHashGrid::~ZPointsHashGrid()
{
	reset();
}

void
ZPointsHashGrid::reset()
{
	FOR( i, 0, _numBuckets )
	{
		ZPointsHashGrid::Item* itr = _list[i];

		while( itr )
		{
			ZPointsHashGrid::Item* next = itr->next;
			delete itr;
			itr = next;
		}
	}

	_numBuckets = 0;
	_h = 0;
}

float
ZPointsHashGrid::voxelSize() const
{
	return _h;
}

int
ZPointsHashGrid::numItems( int i ) const
{
	if( i < 0 || i >= _numBuckets ) { return 0; }

	int count = 0;

	ZPointsHashGrid::Item* itr = _list[i];
	for( ; itr; itr=itr->next )
	{
		++count;
	}

	return count;
}

int
ZPointsHashGrid::numTotalItems() const
{
	int count = 0;

	FOR( i, 0, _numBuckets )
	{
		count += numItems( i );
	}

	return count;
}

int
ZPointsHashGrid::findPoints( ZIntArray& neighbors, const ZPoint& p, float maxDist, bool removeRedundancy, bool asAppending ) const
{
	if( !asAppending ) { neighbors.reset(); }

	const float maxDist2 = ZPow2( maxDist );

	const int i0=(int)((p.x-maxDist)/_h), i1=(int)((p.x+maxDist)/_h)+1;
	const int j0=(int)((p.y-maxDist)/_h), j1=(int)((p.y+maxDist)/_h)+1;
	const int k0=(int)((p.z-maxDist)/_h), k1=(int)((p.z+maxDist)/_h)+1;

	for( int i=i0; i<i1; ++i )
	for( int j=j0; j<j1; ++j )
	for( int k=k0; k<k1; ++k )
	{{{
		const int idx = index( i, j, k );
		ZPointsHashGrid::Item* itr = _list[idx];
		for( ; itr; itr=itr->next )
		{
			if( p.squaredDistanceTo( itr->position ) < maxDist2 )
			{
				neighbors.push_back( itr->id );
			}
		}
	}}}

	if( removeRedundancy )
	{
		neighbors.deduplicateAndSort();
	}

	return neighbors.length();
}

ostream&
operator<<( ostream& os, const ZPointsHashGrid& hash )
{
	os << "<ZPointsHashGrid>" << endl;
	os << "voxel size: " << hash.voxelSize() << endl;
	return os;
}

ZELOS_NAMESPACE_END

