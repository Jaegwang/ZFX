//--------------------------//
// ZUnboundedHashGrid3D.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2017.12.19                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZUnboundedHashGrid3D::ZUnboundedHashGrid3D()
{
	_h    = 1.f;
	_hInv = 1.f / _h;;
}

ZUnboundedHashGrid3D::ZUnboundedHashGrid3D( float h )
{
	set( h );
}

void
ZUnboundedHashGrid3D::reset()
{
	_h    = 1.f;
	_hInv = 1.f / _h;;

	_hash.clear();
}

void
ZUnboundedHashGrid3D::set( float h )
{
	_h    = h;
	_hInv = 1.f / _h;;

	_hash.clear();
}

bool
ZUnboundedHashGrid3D::empty() const
{
	return _hash.empty();
}

bool
ZUnboundedHashGrid3D::empty( const ZInt3& cell ) const
{
	return ( _hash.find(cell) == _hash.end() );
}

int
ZUnboundedHashGrid3D::numElements( const ZInt3& cell ) const
{
	return _hash.count(cell);
}

void
ZUnboundedHashGrid3D::getAllocatedCells( ZInt3Array& cells ) const
{
	cells.clear();

	if( _hash.empty() ) { return; }

	for( HashConstItr itr=_hash.begin(); itr!=_hash.end(); ++itr )
	{
		cells.push_back( itr->first );
	}
}

void
ZUnboundedHashGrid3D::addItem( int id, const ZPoint& p )
{
	const ZInt3 cell( int(p.x*_hInv), int(p.y*_hInv), int(p.z*_hInv) );
	_hash.insert( std::make_pair( cell, id ) );
	//_hash.insert( typename Hash::value_type( cell, id ) );
}

void
ZUnboundedHashGrid3D::addItem( int id, const ZBoundingBox& bBox )
{
	const ZPoint& minPt = bBox.minPoint();
	const ZPoint& maxPt = bBox.maxPoint();

	const int i0 = int( minPt.x * _hInv );
	const int i1 = int( maxPt.x * _hInv );
	const int j0 = int( minPt.y * _hInv );
	const int j1 = int( maxPt.y * _hInv );
	const int k0 = int( minPt.z * _hInv );
	const int k1 = int( maxPt.z * _hInv );

	// We don't need to check cell index range
	// because this is an unbounded spatial hash grid.

	for( int i=i0; i<=i1; ++i )
	for( int j=j0; j<=j1; ++j )
	for( int k=k0; k<=k1; ++k )
	{{{
		const ZInt3 cell( i, j, k );
		_hash.insert( std::make_pair( cell, id ) );
	}}}
}

int
ZUnboundedHashGrid3D::firstItem( const ZInt3& cell ) const
{
	std::pair<HashConstItr,HashConstItr> range = _hash.equal_range( cell );
	HashConstItr itr = range.first;
	return (itr->second);
}

void
ZUnboundedHashGrid3D::getCandidates( ZIntArray& candidates, const ZPoint& p, float radius, bool removeDuplications, bool asAppending ) const
{
	if( !asAppending )
	{
		candidates.clear();
	}

	const float rr = ZPow2( radius + 0.5f*sqrtf(3*_h*_h) );

	const int i0 = int( ( p.x - radius ) * _hInv );
	const int i1 = int( ( p.x + radius ) * _hInv );
	const int j0 = int( ( p.y - radius ) * _hInv );
	const int j1 = int( ( p.y + radius ) * _hInv );
	const int k0 = int( ( p.z - radius ) * _hInv );
	const int k1 = int( ( p.z + radius ) * _hInv );

	// We don't need to check cell index range
	// because this is an unbounded spatial hash grid.

	for( int k=k0; k<=k1; ++k )
	for( int j=j0; j<=j1; ++j )
	for( int i=i0; i<=i1; ++i )
	{{{
		const ZPoint c( cellCenter( ZInt3(i,j,k) ) );
		if( c.squaredDistanceTo(p) > rr ) { continue; }

		const ZInt3 cell( i, j, k );
		std::pair<HashConstItr,HashConstItr> range = _hash.equal_range( cell );
		HashConstItr start = range.first;
		HashConstItr end   = range.second;

		for( HashConstItr itr=start; itr!=end; ++itr )
		{
			candidates.push_back( itr->second );
		}
	}}}

	if( removeDuplications )
	{
		candidates.deduplicateAndSort();
	}
}

void
ZUnboundedHashGrid3D::findPoints( const ZPoint& p, float radius, ZIntArray& pointIds, ZFloatArray& distSQ, const ZPointArray& sP ) const
{
	pointIds.clear();
	distSQ.clear();

	const float r2 = ZPow2( radius );
	const float rr = ZPow2( radius + 0.5f*sqrtf(3*_h*_h) );

	const int i0 = int( ( p.x - radius ) * _hInv );
	const int i1 = int( ( p.x + radius ) * _hInv );
	const int j0 = int( ( p.y - radius ) * _hInv );
	const int j1 = int( ( p.y + radius ) * _hInv );
	const int k0 = int( ( p.z - radius ) * _hInv );
	const int k1 = int( ( p.z + radius ) * _hInv );

	// We don't need to check cell index range
	// because this is an unbounded spatial hash grid.

	for( int k=k0; k<=k1; ++k )
	for( int j=j0; j<=j1; ++j )
	for( int i=i0; i<=i1; ++i )
	{{{
		const ZPoint c( cellCenter( ZInt3(i,j,k) ) );
		if( c.squaredDistanceTo(p) > rr ) { continue; }

		const ZInt3 cell( i, j, k );
		std::pair<HashConstItr,HashConstItr> range = _hash.equal_range( cell );
		HashConstItr start = range.first;
		HashConstItr end   = range.second;

		ZIntArray candidates;
		for( HashConstItr itr=start; itr!=end; ++itr )
		{
			candidates.push_back( itr->second );
		}

		const int nCandidates = candidates.length();
		FOR( l, 0, nCandidates )
		{
			const int& idx = candidates[l];
			const float distSq = p.squaredDistanceTo(sP[idx]);
			if( distSq > r2 ) { continue; }

			pointIds.push_back( idx );
			distSQ.push_back( distSq );
		}
	}}}
}

void
ZUnboundedHashGrid3D::removeAllItems( const ZInt3& cell )
{
	std::pair<HashItr,HashItr> range = _hash.equal_range( cell );
	HashItr start = range.first;
	HashItr end   = range.second;

	_hash.erase( start, end );
}

void
ZUnboundedHashGrid3D::remove( int id )
{
	for( HashItr itr=_hash.begin(); itr!=_hash.end(); ++itr )
	{
		const int& id = itr->second;

		if( id == itr->second )
		{
			_hash.erase( itr );
		}
	}
}

void
ZUnboundedHashGrid3D::remove( const ZPoint& p, float radius, const ZPointArray& sP )
{
	const int i0 = int( ( p.x - radius ) * _hInv );
	const int i1 = int( ( p.x + radius ) * _hInv );
	const int j0 = int( ( p.y - radius ) * _hInv );
	const int j1 = int( ( p.y + radius ) * _hInv );
	const int k0 = int( ( p.z - radius ) * _hInv );
	const int k1 = int( ( p.z + radius ) * _hInv );

	// We don't need to check cell index range
	// because this is an unbounded spatial hash grid.

	const float rr = ZPow2(radius);
	std::vector<HashItr> delList;

	for( int i=i0; i<=i1; ++i )
	for( int j=j0; j<=j1; ++j )
	for( int k=k0; k<=k1; ++k )
	{{{
		// range.first : the first element of the list at cell (i,j,k)
		// range.second: the last  element of the list at cell (i,j,k)
		std::pair<HashItr,HashItr> range = _hash.equal_range( ZInt3(i,j,k) );

		HashItr start = range.first;
		HashItr end   = range.second;

		for( HashItr itr=start; itr!=end; ++itr )
		{
			const int id = itr->second;
			const ZPoint& q = sP[id];
			const float dist = p.squaredDistanceTo(q);

			if( dist <= rr )
			{
				delList.push_back( itr );
			}
		}
	}}}

	for( std::vector<HashItr>::iterator itr=delList.begin(); itr!=delList.end(); ++itr )
	{
		_hash.erase( *itr );
	}
}

void
ZUnboundedHashGrid3D::remove( const ZPoint& p, const ZVector& pn, float radius, const ZPointArray& sP, const ZVectorArray& sN )
{
	const int i0 = int( ( p.x - radius ) * _hInv );
	const int i1 = int( ( p.x + radius ) * _hInv );
	const int j0 = int( ( p.y - radius ) * _hInv );
	const int j1 = int( ( p.y + radius ) * _hInv );
	const int k0 = int( ( p.z - radius ) * _hInv );
	const int k1 = int( ( p.z + radius ) * _hInv );

	// We don't need to check cell index range
	// because this is an unbounded spatial hash grid.

	std::vector<HashItr> delList;

	for( int i=i0; i<=i1; ++i )
	for( int j=j0; j<=j1; ++j )
	for( int k=k0; k<=k1; ++k )
	{{{
		// range.first : the first element of the list at cell (i,j,k)
		// range.second: the last  element of the list at cell (i,j,k)
		std::pair<HashItr,HashItr> range = _hash.equal_range( ZInt3(i,j,k) );
		HashItr start = range.first;
		HashItr end   = range.second;

		for( HashItr itr=start; itr!=end; ++itr )
		{
			const int id = itr->second;
			const ZPoint&  q  = sP[id];
			const ZVector& qn = sN[id];
			const float dist = DST( p,q, pn,qn );

			if( dist <= radius )
			{
				delList.push_back( itr );
			}
		}
	}}}

	for( std::vector<HashItr>::iterator itr=delList.begin(); itr!=delList.end(); ++itr )
	{
		_hash.erase( *itr );
	}
}

float
ZUnboundedHashGrid3D::cellSize() const
{
	return _h;
}

ZPoint
ZUnboundedHashGrid3D::cellCenter( const ZInt3& cell ) const
{
	return ZPoint( (cell[0]+.5f)*_h, (cell[1]+.5f)*_h, (cell[2]+.5f)*_h );
}

ostream&
operator<<( ostream& os, const ZUnboundedHashGrid3D& object )
{
	os << "<ZUnboundedHashGrid3D>" << endl;
	os << " cell size: " << object.cellSize() << endl;
	os << endl;

	return os;
}

ZELOS_NAMESPACE_END

