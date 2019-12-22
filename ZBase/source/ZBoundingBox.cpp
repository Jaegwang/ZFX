//------------------//
// ZBoundingBox.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2018.03.08                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZBoundingBox::ZBoundingBox()
: _initialized(false)
{}

ZBoundingBox::ZBoundingBox( const ZBoundingBox& b )
: _initialized(b._initialized), _min(b._min), _max(b._max)
{}

ZBoundingBox::ZBoundingBox( const ZPoint& p1, const ZPoint& p2 )
{
	_initialized = true;

	ZMinMax( p1.x, p2.x, _min.x, _max.x );
	ZMinMax( p1.y, p2.y, _min.y, _max.y );
	ZMinMax( p1.z, p2.z, _min.z, _max.z );
}

ZBoundingBox::ZBoundingBox( const ZFloat3& p1, const ZFloat3& p2 )
{
	_initialized = true;

	ZMinMax( p1.data[0], p2.data[0], _min.x, _max.x );
	ZMinMax( p1.data[1], p2.data[1], _min.y, _max.y );
	ZMinMax( p1.data[2], p2.data[2], _min.z, _max.z );
}

ZBoundingBox::ZBoundingBox( const ZDouble3& p1, const ZDouble3& p2 )
{
	_initialized = true;

	const float x1 = (float)p1.data[0];
	const float y1 = (float)p1.data[1];
	const float z1 = (float)p1.data[2];

	const float x2 = (float)p2.data[0];
	const float y2 = (float)p2.data[1];
	const float z2 = (float)p2.data[2];

	ZMinMax( x1, x2, _min.x, _max.x );
	ZMinMax( y1, y2, _min.y, _max.y );
	ZMinMax( z1, z2, _min.z, _max.z );
}

ZBoundingBox::ZBoundingBox( const ZPoint& p1, const ZPoint& p2, const ZPoint& p3 )
{
	_initialized = true;

	ZMinMax( p1.x, p2.x, p3.x, _min.x, _max.x );
	ZMinMax( p1.y, p2.y, p3.y, _min.y, _max.y );
	ZMinMax( p1.z, p2.z, p3.z, _min.z, _max.z );
}

ZBoundingBox::ZBoundingBox( const ZFloat3& p1, const ZFloat3& p2, const ZFloat3& p3 )
{
	_initialized = true;

	ZMinMax( p1.data[0], p2.data[0], p3.data[0], _min.x, _max.x );
	ZMinMax( p1.data[1], p2.data[1], p3.data[1], _min.y, _max.y );
	ZMinMax( p1.data[2], p2.data[2], p3.data[2], _min.z, _max.z );
}

ZBoundingBox::ZBoundingBox( const ZDouble3& p1, const ZDouble3& p2, const ZDouble3& p3 )
{
	_initialized = true;

	const float x1 = (float)p1.data[0];
	const float y1 = (float)p1.data[1];
	const float z1 = (float)p1.data[2];

	const float x2 = (float)p2.data[0];
	const float y2 = (float)p2.data[1];
	const float z2 = (float)p2.data[2];

	const float x3 = (float)p3.data[0];
	const float y3 = (float)p3.data[1];
	const float z3 = (float)p3.data[2];

	ZMinMax( x1, x2, x3, _min.x, _max.x );
	ZMinMax( y1, y2, y3, _min.y, _max.y );
	ZMinMax( z1, z2, z3, _min.z, _max.z );
}

void
ZBoundingBox::reset()
{
	_initialized = false;

	_min.zeroize();
	_max.zeroize();
}

ZBoundingBox&
ZBoundingBox::set( const ZPoint& p1, const ZPoint& p2 )
{
	_initialized = true;

	ZMinMax( p1.x, p2.x, _min.x, _max.x );
	ZMinMax( p1.y, p2.y, _min.y, _max.y );
	ZMinMax( p1.z, p2.z, _min.z, _max.z );

	return (*this);
}

ZBoundingBox&
ZBoundingBox::set( const ZFloat3& p1, const ZFloat3& p2 )
{
	_initialized = true;

	ZMinMax( p1.data[0], p2.data[0], _min.x, _max.x );
	ZMinMax( p1.data[1], p2.data[1], _min.y, _max.y );
	ZMinMax( p1.data[2], p2.data[2], _min.z, _max.z );

	return (*this);
}

ZBoundingBox&
ZBoundingBox::set( const ZDouble3& p1, const ZDouble3& p2 )
{
	_initialized = true;

	const float x1 = (float)p1.data[0];
	const float y1 = (float)p1.data[1];
	const float z1 = (float)p1.data[2];

	const float x2 = (float)p2.data[0];
	const float y2 = (float)p2.data[1];
	const float z2 = (float)p2.data[2];

	ZMinMax( x1, x2, _min.x, _max.x );
	ZMinMax( y1, y2, _min.y, _max.y );
	ZMinMax( z1, z2, _min.z, _max.z );

	return (*this);
}

ZBoundingBox&
ZBoundingBox::set( const ZPoint& p1, const ZPoint& p2, const ZPoint& p3 )
{
	_initialized = true;

	ZMinMax( p1.x, p2.x, p3.x, _min.x, _max.x );
	ZMinMax( p1.y, p2.y, p3.y, _min.y, _max.y );
	ZMinMax( p1.z, p2.z, p3.z, _min.z, _max.z );

	return (*this);
}

ZBoundingBox&
ZBoundingBox::set( const ZFloat3& p1, const ZFloat3& p2, const ZFloat3& p3 )
{
	_initialized = true;

	ZMinMax( p1.data[0], p2.data[0], p3.data[0], _min.x, _max.x );
	ZMinMax( p1.data[1], p2.data[1], p3.data[1], _min.y, _max.y );
	ZMinMax( p1.data[2], p2.data[2], p3.data[2], _min.z, _max.z );

	return (*this);
}

ZBoundingBox&
ZBoundingBox::set( const ZDouble3& p1, const ZDouble3& p2, const ZDouble3& p3 )
{
	_initialized = true;

	const float x1 = (float)p1.data[0];
	const float y1 = (float)p1.data[1];
	const float z1 = (float)p1.data[2];

	const float x2 = (float)p2.data[0];
	const float y2 = (float)p2.data[1];
	const float z2 = (float)p2.data[2];

	const float x3 = (float)p3.data[0];
	const float y3 = (float)p3.data[1];
	const float z3 = (float)p3.data[2];

	ZMinMax( x1, x2, x3, _min.x, _max.x );
	ZMinMax( y1, y2, y3, _min.y, _max.y );
	ZMinMax( z1, z2, z3, _min.z, _max.z );

	return (*this);
}

ZBoundingBox&
ZBoundingBox::operator=( const ZBoundingBox& b )
{
	_initialized = b._initialized;

	_min = b._min;
	_max = b._max;

	return (*this);
}

ZBoundingBox&
ZBoundingBox::operator*=( const int& scale )
{
	const ZPoint c( Center( _min, _max ) );

	_min = ( (float)scale * ( _min - c ) ) + c;
	_max = ( (float)scale * ( _max - c ) ) + c;

	return (*this);
}

ZBoundingBox&
ZBoundingBox::operator*=( const float& scale )
{
	const ZPoint c( Center( _min, _max ) );

	_min = ( scale * ( _min - c ) ) + c;
	_max = ( scale * ( _max - c ) ) + c;

	return (*this);
}

ZBoundingBox&
ZBoundingBox::operator*=( const double& scale )
{
	const ZPoint c( Center( _min, _max ) );

	_min = ( (float)scale * ( _min - c ) ) + c;
	_max = ( (float)scale * ( _max - c ) ) + c;

	return (*this);
}

ZBoundingBox&
ZBoundingBox::expand( const ZPoint& p )
{
	if( _initialized ) {

		_min.x = ZMin( _min.x, p.x );
		_min.y = ZMin( _min.y, p.y );
		_min.z = ZMin( _min.z, p.z );

		_max.x = ZMax( _max.x, p.x );
		_max.y = ZMax( _max.y, p.y );
		_max.z = ZMax( _max.z, p.z );

	} else {

		_min = _max = p;
		_initialized = true;

	}

	return (*this);
}

ZBoundingBox&
ZBoundingBox::expand( const ZFloat3& p )
{
	if( _initialized ) {

		_min.x = ZMin( _min.x, p.data[0] );
		_min.y = ZMin( _min.y, p.data[1] );
		_min.z = ZMin( _min.z, p.data[2] );

		_max.x = ZMax( _max.x, p.data[0] );
		_max.y = ZMax( _max.y, p.data[1] );
		_max.z = ZMax( _max.z, p.data[2] );

	} else {

		_min.x = _max.x = p.data[0];
		_min.y = _max.y = p.data[1];
		_min.z = _max.z = p.data[2];

		_initialized = true;

	}

	return (*this);
}

ZBoundingBox&
ZBoundingBox::expand( const ZDouble3& p )
{
	if( _initialized ) {

		_min.x = ZMin( _min.x, (float)p[0] );
		_min.y = ZMin( _min.y, (float)p[1] );
		_min.z = ZMin( _min.z, (float)p[2] );

		_max.x = ZMax( _max.x, (float)p[0] );
		_max.y = ZMax( _max.y, (float)p[1] );
		_max.z = ZMax( _max.z, (float)p[2] );

	} else {

		_min.x = _max.x = (float)p[0];
		_min.y = _max.y = (float)p[1];
		_min.z = _max.z = (float)p[2];

		_initialized = true;

	}

	return (*this);
}

ZBoundingBox&
ZBoundingBox::expand( const ZBoundingBox& b )
{
	if( !b._initialized ) {

		return (*this);

	} else if( !_initialized ) {

		this->operator=(b);
		return (*this);

	} else {

		_min.x = ZMin( _min.x, b._min.x );
		_min.y = ZMin( _min.y, b._min.y );
		_min.z = ZMin( _min.z, b._min.z );

		_max.x = ZMax( _max.x, b._max.x );
		_max.y = ZMax( _max.y, b._max.y );
		_max.z = ZMax( _max.z, b._max.z );

		return (*this);

	}
}

ZBoundingBox&
ZBoundingBox::expand( float e )
{
	if( _initialized )
	{
		_min.x -= e;
		_min.y -= e;
		_min.z -= e;

		_max.x += e;
		_max.y += e;
		_max.z += e;
	}

	return (*this);
}

ZBoundingBox&
ZBoundingBox::scaleAboutCenter( const ZVector& s )
{
	const float& sx = s.x;
	const float& sy = s.y;
	const float& sz = s.z;

	const ZPoint c = Center( _min, _max );

	const float& cx = c.x;
	const float& cy = c.y;
	const float& cz = c.z;

	_min.x = sx * ( _min.x - c.x ) + c.x;
	_max.x = sx * ( _max.x - c.x ) + c.x;

	_min.y = sy * ( _min.y - c.y ) + c.y;
	_max.y = sy * ( _max.y - c.y ) + c.y;

	_min.z = sz * ( _min.z - c.z ) + c.z;
	_max.z = sz * ( _max.z - c.z ) + c.z;

	return (*this);
}

ZBoundingBox&
ZBoundingBox::scale( const ZVector& s, const ZPoint& pivot )
{
	const float& sx = s.x;
	const float& sy = s.y;
	const float& sz = s.z;

	_min.x = sx * ( _min.x - pivot.x ) + pivot.x;
	_max.x = sx * ( _max.x - pivot.x ) + pivot.x;

	_min.y = sy * ( _min.y - pivot.y ) + pivot.y;
	_max.y = sy * ( _max.y - pivot.y ) + pivot.y;

	_min.z = sz * ( _min.z - pivot.z ) + pivot.z;
	_max.z = sz * ( _max.z - pivot.z ) + pivot.z;

	return (*this);
}

ZBoundingBox&
ZBoundingBox::move( const ZVector& translation )
{
	_min += translation;
	_max += translation;

	return (*this);
}

ZBoundingBox&
ZBoundingBox::merge( const ZBoundingBox& b0, const ZBoundingBox& b1 )
{
	if( b0._initialized )
	{
		const ZPoint& minPt = b0.minPoint();
		const ZPoint& maxPt = b0.maxPoint();

		_min.x = ZMin( _min.x, minPt.x );
		_min.y = ZMin( _min.y, minPt.y );
		_min.z = ZMin( _min.z, minPt.z );

		_max.x = ZMax( _max.x, maxPt.x );
		_max.y = ZMax( _max.y, maxPt.y );
		_max.z = ZMax( _max.z, maxPt.z );

		_initialized = true;
	}

	if( b1._initialized )
	{
		const ZPoint& minPt = b1.minPoint();
		const ZPoint& maxPt = b1.maxPoint();

		_min.x = ZMin( _min.x, minPt.x );
		_min.y = ZMin( _min.y, minPt.y );
		_min.z = ZMin( _min.z, minPt.z );

		_max.x = ZMax( _max.x, maxPt.x );
		_max.y = ZMax( _max.y, maxPt.y );
		_max.z = ZMax( _max.z, maxPt.z );

		_initialized = true;
	}

	return (*this);
}

bool
ZBoundingBox::initialized() const
{
	return _initialized;
}

bool
ZBoundingBox::contains( const ZPoint& p ) const
{
	if( !_initialized ) { return false; }

	if( p.x < _min.x ) { return false; }
	if( p.y < _min.y ) { return false; }
	if( p.z < _min.z ) { return false; }

	if( p.x > _max.x ) { return false; }
	if( p.y > _max.y ) { return false; }
	if( p.z > _max.z ) { return false; }

	return true;
}

bool
ZBoundingBox::contains( const ZFloat3& p ) const
{
	if( !_initialized ) { return false; }

	if( p[0] < _min.x ) { return false; }
	if( p[1] < _min.y ) { return false; }
	if( p[2] < _min.z ) { return false; }

	if( p[0] > _max.x ) { return false; }
	if( p[1] > _max.y ) { return false; }
	if( p[2] > _max.z ) { return false; }

	return true;
}

bool
ZBoundingBox::contains( const ZDouble3& p ) const
{
	if( !_initialized ) { return false; }

	if( (float)p[0] < _min.x ) { return false; }
	if( (float)p[1] < _min.y ) { return false; }
	if( (float)p[2] < _min.z ) { return false; }

	if( (float)p[0] > _max.x ) { return false; }
	if( (float)p[1] > _max.y ) { return false; }
	if( (float)p[2] > _max.z ) { return false; }

	return true;
}

bool
ZBoundingBox::intersects( const ZBoundingBox& b ) const
{
	if( !_initialized || !b._initialized ) { return false; }

	if( _max.x < b._min.x ) { return false; }
	if( _max.y < b._min.y ) { return false; }
	if( _max.z < b._min.z ) { return false; }

	if( _min.x > b._max.x ) { return false; }
	if( _min.y > b._max.y ) { return false; }
	if( _min.z > b._max.z ) { return false; }

	return true;
}

bool
ZBoundingBox::intersectsWithLineSegment( const ZPoint& p0, const ZPoint& p1 ) const
{
	ZPoint c( Center( _min, _max ) );	// box center-point
	ZVector e( _max - c );				// box half-length extents
	ZPoint m( 0.5f * ( p0 + p1 ) );		// segment midpoint
	ZVector d( p1 - m );				// Segment halflength vector
	m -= c;								// translate box and segment to origin

	// try world coordinate axes as separating axes
	float adx = ZAbs(d.x);
	if( ZAbs(m.x) > e.x + adx ) { return false; }
	float ady = ZAbs(d.y);
	if( ZAbs(m.y) > e.y + ady ) { return false; }
	float adz = ZAbs(d.z);
	if( ZAbs(m.z) > e.z + adz ) { return false; }

	// Add in an epsilon term to counteract arithmetic errors when segment is
	// (near) parallel to a coordinate axis (see text for detail)
	adx += Z_EPS; ady += Z_EPS; adz += Z_EPS;

	// try cross products of segment direction vector with coordinate axes
	if( ZAbs( m.y*d.z - m.z*d.y ) > ( e.y*adz + e.z*ady ) ) { return false; }
	if( ZAbs( m.z*d.x - m.x*d.z ) > ( e.x*adz + e.z*adx ) ) { return false; }
	if( ZAbs( m.x*d.y - m.y*d.x ) > ( e.x*ady + e.y*adx ) ) { return false; }

	// no separating axis found; segment must be overlapping AABB
	return true;
}

bool
ZBoundingBox::intersectsWithRay( const ZPoint& o, const ZVector& d, ZPoint* near, ZPoint* far, float eps ) const
{
	float tMin = 0;
	float tMax = Z_LARGE;
	float denom, t1, t2;

	if( near ) { near->zeroize(); }
	if( far  ) { far->zeroize();  }

	if( ZAbs(d.x) < eps ) {
		if( o.x > _min.x ) { return false; }
		if( o.x > _max.x ) { return false; }
	} else {
		denom = 1 / d.x;
		t1 = (-o.x - _min.x) * denom;
		t2 = (-o.x - _max.x) * denom;
		if( t1 > t2 ) { ZSwap( t1, t2 ); }
		tMin = ZMax(tMin, t1);
		tMax = ZMin(tMax, t2);
		if( tMin > tMax ) { return false; }
	}

	if( ZAbs(d.y) < eps ) {
		if( o.y > _min.y ) { return false; }
		if( o.y > _max.y ) { return false; }
	} else {
		denom = 1 / d.y;
		t1 = (-o.y - _min.y) * denom;
		t2 = (-o.y - _max.y) * denom;
		if( t1 > t2 ) { ZSwap( t1, t2 ); }
		tMin = ZMax(tMin, t1);
		tMax = ZMin(tMax, t2);
		if( tMin > tMax ) { return false; }
	}

	if( ZAbs(d.z) < eps ) {
		if( o.z > _min.z ) { return false; }
		if( o.z > _max.z ) { return false; }
	} else {
		denom = 1 / d.z;
		t1 = (-o.z - _min.z) * denom;
		t2 = (-o.z - _max.z) * denom;
		if( t1 > t2 ) { ZSwap( t1, t2 ); }
		tMin = ZMax(tMin, t1);
		tMax = ZMin(tMax, t2);
		if( tMin > tMax ) { return false; }
	}

	if( near ) { *near = o + tMin * d; }
	if( far  ) { *far  = o + tMax * d; }
 
	return true;
}

bool
ZBoundingBox::intersectsWithTriangle( const ZPoint& V0, const ZPoint& V1, const ZPoint& V2 ) const
{
	ZPoint C( 0.5f * ( _min + _max ) );
	ZVector boxHalfSize( _max - C );

	//    use separating axis theorem to test overlap between triangle and box 
	//    need to test for overlap in these directions: 
	//    1) the {x,y,z}-directions (actually, since we use the AABB of the triangle 
	//       we do not even need to test these) 
	//    2) normal of the triangle 
	//    3) crossproduct(edge from tri, {x,y,z}-directin) 
	//       this gives 3x3=9 more tests 

	// This is the fastest branch on Sun
	// move everything so that the box center is in (0,0,0)
	ZPoint v0(V0-C), v1(V1-C), v2(V2-C);

	// compute triangle edges
	ZVector e0(v1-v0), e1(v2-v1), e2(v0-v2);

	float p0,p1,p2, min,max, rad; // temporary variables for macros
	float fex=0, fey=0, fez=0;

	//======================== X-tests ========================
	#define AXISTEST_X01(a, b, fa, fb)                     \
		p0 = a*v0.y - b*v0.z;                              \
		p2 = a*v2.y - b*v2.z;                              \
		if(p0<p2) {min=p0; max=p2;} else {min=p2; max=p0;} \
		rad = fa * boxHalfSize.y + fb * boxHalfSize.z;     \
		if(min>rad || max<-rad) { return false; }
	#define AXISTEST_X2(a, b, fa, fb)                      \
		p0 = a*v0.y - b*v0.z;                              \
		p1 = a*v1.y - b*v1.z;                              \
		if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
		rad = fa * boxHalfSize.y + fb * boxHalfSize.z;     \
		if(min>rad || max<-rad) { return false; }
	//======================== Y-tests ========================
	#define AXISTEST_Y02(a, b, fa, fb)                     \
		p0 = -a*v0.x + b*v0.z;                             \
		p2 = -a*v2.x + b*v2.z;                             \
		if(p0<p2) {min=p0; max=p2;} else {min=p2; max=p0;} \
		rad = fa * boxHalfSize.x + fb * boxHalfSize.z;     \
		if(min>rad || max<-rad) { return false; }
	#define AXISTEST_Y1(a, b, fa, fb)                      \
		p0 = -a*v0.x + b*v0.z;                             \
		p1 = -a*v1.x + b*v1.z;                             \
		if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
		rad = fa * boxHalfSize.x + fb * boxHalfSize.z;     \
		if(min>rad || max<-rad) { return false; }
	//======================== Z-tests ========================
	#define AXISTEST_Z12(a, b, fa, fb)                     \
		p1 = a*v1.x - b*v1.y;                              \
		p2 = a*v2.x - b*v2.y;                              \
		if(p2<p1) {min=p2; max=p1;} else {min=p1; max=p2;} \
		rad = fa * boxHalfSize.x + fb * boxHalfSize.y;     \
		if(min>rad || max<-rad) { return false; }
	#define AXISTEST_Z0(a, b, fa, fb)                      \
		p0 = a*v0.x - b*v0.y;                              \
		p1 = a*v1.x - b*v1.y;                              \
		if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
		rad = fa * boxHalfSize.x + fb * boxHalfSize.y;     \
		if(min>rad || max<-rad) { return false; }

	// Bullet 3:
	//  test the 9 tests first (this was faster)
	fex = ZAbs(e0.x);
	fey = ZAbs(e0.y);
	fez = ZAbs(e0.z);
	AXISTEST_X01(e0.z, e0.y, fez, fey);
	AXISTEST_Y02(e0.z, e0.x, fez, fex);
	AXISTEST_Z12(e0.y, e0.x, fey, fex);
	fex = ZAbs(e1.x);
	fey = ZAbs(e1.y);
	fez = ZAbs(e1.z);
	AXISTEST_X01(e1.z, e1.y, fez, fey);
	AXISTEST_Y02(e1.z, e1.x, fez, fex);
	AXISTEST_Z0(e1.y, e1.x, fey, fex);
	fex = ZAbs(e2.x);
	fey = ZAbs(e2.y);
	fez = ZAbs(e2.z);
	AXISTEST_X2(e2.z, e2.y, fez, fey);
	AXISTEST_Y1(e2.z, e2.x, fez, fex);
	AXISTEST_Z12(e2.y, e2.x, fey, fex);
	
	#undef AXISTEST_X01
	#undef AXISTEST_X2
	#undef AXISTEST_Y02
	#undef AXISTEST_Y1
	#undef AXISTEST_Z12
	#undef AXISTEST_Z0

	// Bullet 1:
	//  first test overlap in the {x,y,z}-directions 
	//  find min, max of the triangle each direction, and test for overlap in 
	//  that direction -- this is equivalent to testing a minimal AABB around 
	//  the triangle against the AABB 
	// test in X-direction 
	ZMinMax( v0.x, v1.x, v2.x, min, max );
	if( min>boxHalfSize.x || max<-boxHalfSize.x ) { return false; }
	// test in Y-direction 
	ZMinMax( v0.y, v1.y, v2.y, min, max );
	if( min>boxHalfSize.y || max<-boxHalfSize.y ) { return false; }
	// test in Z-direction 
	ZMinMax( v0.z, v1.z, v2.z, min, max );
	if( min>boxHalfSize.z || max<-boxHalfSize.z ) { return false; }
	
	// Bullet 2: 
	//  test if the box intersects the plane of the triangle 
	//  compute plane equation of triangle: normal*x+d=0 
	ZVector normal( e0^e1 );

	// plane vs AABB overlap test
	{
		ZVector vMin;
		vMin.x = (normal.x>0) ? (-boxHalfSize.x-v0.x) : ( boxHalfSize.x-v0.x);
		vMin.y = (normal.y>0) ? (-boxHalfSize.y-v0.y) : ( boxHalfSize.y-v0.y);
		vMin.z = (normal.z>0) ? (-boxHalfSize.z-v0.z) : ( boxHalfSize.z-v0.z);
		if( (normal*vMin) >  0 ) { return false; }

		ZVector vMax;
		vMax.x = (normal.x>0) ? ( boxHalfSize.x-v0.x) : (-boxHalfSize.x-v0.x);
		vMax.y = (normal.y>0) ? ( boxHalfSize.y-v0.y) : (-boxHalfSize.y-v0.y);
		vMax.z = (normal.z>0) ? ( boxHalfSize.z-v0.z) : (-boxHalfSize.z-v0.z);
		if( (normal*vMax) >= 0 ) { return true; }
	}

	return true; // AABB and triangle overlaps 
}

bool
ZBoundingBox::intersectsWithSphere( const ZPoint& c, float r ) const
{
	ZPoint C( 0.5f * ( _min + _max ) );
	ZVector boxHalfSize( _max - C );

	if( ZAbs( C.x - c.x ) > ( r + boxHalfSize.x ) ) { return false; }
	if( ZAbs( C.y - c.y ) > ( r + boxHalfSize.y ) ) { return false; }
	if( ZAbs( C.z - c.z ) > ( r + boxHalfSize.z ) ) { return false; }

	return true;
}

float
ZBoundingBox::distanceFromOutside( const ZPoint& p, bool asSquaredDist ) const
{
	const float dx = (p.x<_min.x)?(_min.x-p.x):((p.x>_max.x)?(p.x-_max.x):0);
	const float dy = (p.y<_min.y)?(_min.y-p.y):((p.y>_max.y)?(p.y-_max.y):0);
	const float dz = (p.z<_min.z)?(_min.z-p.z):((p.z>_max.z)?(p.z-_max.z):0);

	const float dist = ZPow2(dx) + ZPow2(dy) + ZPow2(dz);

	if( asSquaredDist ) { return dist; }
	return sqrtf(dist);
}

void
ZBoundingBox::split( ZBoundingBox& child1, ZBoundingBox& child2 ) const
{
	const int d = maxDimension();
	float kDiv = 0.5f * ( _min[d] + _max[d] );

	ZPoint bMin1( _min ), bMax1( _max );
	bMax1[d] = kDiv;
	child1.set( bMin1, bMax1 );

	ZPoint bMin2( _min ), bMax2( _max );
	bMin2[d] = kDiv;
	child2.set( bMin2, bMax2 );
}

void
ZBoundingBox::offset( const ZVector& displacement )
{
	_min += displacement;
	_max += displacement;
}

ZPoint
ZBoundingBox::center() const
{
	return Center( _min, _max );
}

float
ZBoundingBox::xWidth() const
{
	return ( _max.x - _min.x );
}

float
ZBoundingBox::yWidth() const
{
	return ( _max.y - _min.y );
}

float
ZBoundingBox::zWidth() const
{
	return ( _max.z - _min.z );
}

float
ZBoundingBox::width( int dim ) const
{
	switch( dim )
	{
		default:
		case 0: { return ( _max.x - _min.x ); break; }
		case 1: { return ( _max.y - _min.y ); break; }
		case 2: { return ( _max.z - _min.z ); break; }
	}
}

float
ZBoundingBox::maxWidth() const
{
	return ZMax( (_max.x-_min.x), (_max.y-_min.y), (_max.z-_min.z) );
}

float
ZBoundingBox::minWidth() const
{
	return ZMin( (_max.x-_min.x), (_max.y-_min.y), (_max.z-_min.z) );
}

float
ZBoundingBox::volume() const
{
	return ( (_max.x-_min.x) * (_max.y-_min.y) * (_max.z-_min.z) );
}

float
ZBoundingBox::diagonalLength() const
{
	return sqrtf( ZPow2( _max.x - _min.x ) + ZPow2( _max.y - _min.y ) + ZPow2( _max.z - _min.z ) );
}

int
ZBoundingBox::maxDimension() const
{
	const float xDim = _max.x - _min.x;
	const float yDim = _max.y - _min.y;
	const float zDim = _max.z - _min.z;

	return ( ( xDim > yDim ) ? ( (xDim > zDim) ? 0 : 2 ) : ( ( yDim > zDim ) ? 1 : 2 ) );
}

void
ZBoundingBox::getBoundingSphere( ZPoint& center, float& radius ) const
{
	center = Center( _min, _max );
	radius = center.distanceTo( _min );
}

void
ZBoundingBox::getEightCorners( ZPoint& p0, ZPoint& p1, ZPoint& p2, ZPoint& p3, ZPoint& p4, ZPoint& p5, ZPoint& p6, ZPoint& p7 ) const
{
	p0.set( _min.x, _min.y, _min.z );
	p1.set( _min.x, _max.y, _min.z );
	p2.set( _max.x, _max.y, _min.z );
	p3.set( _max.x, _min.y, _min.z );
	p4.set( _min.x, _min.y, _max.z );
	p5.set( _min.x, _max.y, _max.z );
	p6.set( _max.x, _max.y, _max.z );
	p7.set( _max.x, _min.y, _max.z );
}

void
ZBoundingBox::getEightCorners( ZPoint p[8] ) const
{
	p[0].set( _min.x, _min.y, _min.z );
	p[1].set( _min.x, _max.y, _min.z );
	p[2].set( _max.x, _max.y, _min.z );
	p[3].set( _max.x, _min.y, _min.z );
	p[4].set( _min.x, _min.y, _max.z );
	p[5].set( _min.x, _max.y, _max.z );
	p[6].set( _max.x, _max.y, _max.z );
	p[7].set( _max.x, _min.y, _max.z );
}

void
ZBoundingBox::applyTransform( const ZMatrix& xform )
{
	ZPoint p0, p1, p2, p3, p4, p5, p6, p7;

	ZBoundingBox::getEightCorners( p0, p1, p2, p3, p4, p5, p6, p7 );

	p0 = xform.transform( p0, false );
	p1 = xform.transform( p1, false );
	p2 = xform.transform( p2, false );
	p3 = xform.transform( p3, false );
	p4 = xform.transform( p4, false );
	p5 = xform.transform( p5, false );
	p6 = xform.transform( p6, false );
	p7 = xform.transform( p7, false );

	ZBoundingBox::reset();

	ZBoundingBox::expand( p0 );
	ZBoundingBox::expand( p1 );
	ZBoundingBox::expand( p2 );
	ZBoundingBox::expand( p3 );
	ZBoundingBox::expand( p4 );
	ZBoundingBox::expand( p5 );
	ZBoundingBox::expand( p6 );
	ZBoundingBox::expand( p7 );
}

void
ZBoundingBox::write( ofstream& fout ) const
{
	_min.write( fout );
	_max.write( fout );
}

void
ZBoundingBox::read( ifstream& fin )
{
	ZPoint minPt;
	minPt.read( fin );

	ZPoint maxPt;
	maxPt.read( fin );

	set( minPt, maxPt );
}

void
ZBoundingBox::draw() const
{
	if( !_initialized ) { return; }

	glBegin( GL_LINES );
	{
		glVertex(_min.x,_min.y,_min.z); glVertex(_min.x,_min.y,_max.z);
		glVertex(_min.x,_min.y,_max.z); glVertex(_max.x,_min.y,_max.z);
		glVertex(_max.x,_min.y,_max.z); glVertex(_max.x,_min.y,_min.z);
		glVertex(_max.x,_min.y,_min.z); glVertex(_min.x,_min.y,_min.z);

		glVertex(_min.x,_max.y,_min.z); glVertex(_min.x,_max.y,_max.z);
		glVertex(_min.x,_max.y,_max.z); glVertex(_max.x,_max.y,_max.z);
		glVertex(_max.x,_max.y,_max.z); glVertex(_max.x,_max.y,_min.z);
		glVertex(_max.x,_max.y,_min.z); glVertex(_min.x,_max.y,_min.z);

		glVertex(_min.x,_min.y,_min.z); glVertex(_min.x,_max.y,_min.z);
		glVertex(_min.x,_min.y,_max.z); glVertex(_min.x,_max.y,_max.z);
		glVertex(_max.x,_min.y,_max.z); glVertex(_max.x,_max.y,_max.z);
		glVertex(_max.x,_min.y,_min.z); glVertex(_max.x,_max.y,_min.z);
	}
	glEnd();
}

void
ZBoundingBox::drawVertices() const
{
	if( !_initialized ) { return; }

	glBegin( GL_POINTS );
	{
		glVertex(_max.x,_max.y,_min.z);
		glVertex(_min.x,_max.y,_min.z);
		glVertex(_min.x,_max.y,_max.z);
		glVertex(_max.x,_max.y,_max.z);

		glVertex(_max.x,_min.y,_max.z);
		glVertex(_min.x,_min.y,_max.z);
		glVertex(_min.x,_min.y,_min.z);
		glVertex(_max.x,_min.y,_min.z);
	}
	glEnd();
}

void
ZBoundingBox::drawWireframe() const
{
	if( !_initialized ) { return; }

	glBegin( GL_LINES );
	{
		glVertex(_min.x,_min.y,_min.z); glVertex(_min.x,_min.y,_max.z);
		glVertex(_min.x,_min.y,_max.z); glVertex(_max.x,_min.y,_max.z);
		glVertex(_max.x,_min.y,_max.z); glVertex(_max.x,_min.y,_min.z);
		glVertex(_max.x,_min.y,_min.z); glVertex(_min.x,_min.y,_min.z);

		glVertex(_min.x,_max.y,_min.z); glVertex(_min.x,_max.y,_max.z);
		glVertex(_min.x,_max.y,_max.z); glVertex(_max.x,_max.y,_max.z);
		glVertex(_max.x,_max.y,_max.z); glVertex(_max.x,_max.y,_min.z);
		glVertex(_max.x,_max.y,_min.z); glVertex(_min.x,_max.y,_min.z);

		glVertex(_min.x,_min.y,_min.z); glVertex(_min.x,_max.y,_min.z);
		glVertex(_min.x,_min.y,_max.z); glVertex(_min.x,_max.y,_max.z);
		glVertex(_max.x,_min.y,_max.z); glVertex(_max.x,_max.y,_max.z);
		glVertex(_max.x,_min.y,_min.z); glVertex(_max.x,_max.y,_min.z);
	}
	glEnd();
}

void
ZBoundingBox::drawSurface( bool withNormal ) const
{
	if( !_initialized ) { return; }

	if( withNormal ) {

		glBegin( GL_QUADS );
		{
			// top (y-max.)
			glNormal( 1.f, 1.f,-1.f); glVertex(_max.x,_max.y,_min.z);
			glNormal(-1.f, 1.f,-1.f); glVertex(_min.x,_max.y,_min.z);
			glNormal(-1.f, 1.f, 1.f); glVertex(_min.x,_max.y,_max.z);
			glNormal( 1.f, 1.f, 1.f); glVertex(_max.x,_max.y,_max.z);

			// bottom (y-min.)
			glNormal( 1.f,-1.f, 1.f); glVertex(_max.x,_min.y,_max.z);
			glNormal(-1.f,-1.f, 1.f); glVertex(_min.x,_min.y,_max.z);
			glNormal(-1.f,-1.f,-1.f); glVertex(_min.x,_min.y,_min.z);
			glNormal( 1.f,-1.f,-1.f); glVertex(_max.x,_min.y,_min.z);

			// front (z-max.)
			glNormal( 1.f, 1.f, 1.f); glVertex(_max.x,_max.y,_max.z);
			glNormal(-1.f, 1.f, 1.f); glVertex(_min.x,_max.y,_max.z);
			glNormal(-1.f,-1.f, 1.f); glVertex(_min.x,_min.y,_max.z);
			glNormal( 1.f,-1.f, 1.f); glVertex(_max.x,_min.y,_max.z);

			// back (z-min.)
			glNormal( 1.f,-1.f,-1.f); glVertex(_max.x,_min.y,_min.z);
			glNormal(-1.f,-1.f,-1.f); glVertex(_min.x,_min.y,_min.z);
			glNormal(-1.f, 1.f,-1.f); glVertex(_min.x,_max.y,_min.z);
			glNormal( 1.f, 1.f,-1.f); glVertex(_max.x,_max.y,_min.z);

			// left (x-min.)
			glNormal(-1.f, 1.f, 1.f); glVertex(_min.x,_max.y,_max.z);
			glNormal(-1.f, 1.f,-1.f); glVertex(_min.x,_max.y,_min.z);
			glNormal(-1.f,-1.f,-1.f); glVertex(_min.x,_min.y,_min.z);
			glNormal(-1.f,-1.f, 1.f); glVertex(_min.x,_min.y,_max.z);

			// right (x-max.)
			glNormal( 1.f, 1.f,-1.f); glVertex(_max.x,_max.y,_min.z);
			glNormal( 1.f, 1.f, 1.f); glVertex(_max.x,_max.y,_max.z);
			glNormal( 1.f,-1.f, 1.f); glVertex(_max.x,_min.y,_max.z);
			glNormal( 1.f,-1.f,-1.f); glVertex(_max.x,_min.y,_min.z);
		}
		glEnd();

	} else {

		glBegin( GL_QUADS );
		{
			// top (y-max.)
			glVertex(_max.x,_max.y,_min.z);
			glVertex(_min.x,_max.y,_min.z);
			glVertex(_min.x,_max.y,_max.z);
			glVertex(_max.x,_max.y,_max.z);

			// bottom (y-min.)
			glVertex(_max.x,_min.y,_max.z);
			glVertex(_min.x,_min.y,_max.z);
			glVertex(_min.x,_min.y,_min.z);
			glVertex(_max.x,_min.y,_min.z);

			// front (z-max.)
			glVertex(_max.x,_max.y,_max.z);
			glVertex(_min.x,_max.y,_max.z);
			glVertex(_min.x,_min.y,_max.z);
			glVertex(_max.x,_min.y,_max.z);

			// back (z-min.)
			glVertex(_max.x,_min.y,_min.z);
			glVertex(_min.x,_min.y,_min.z);
			glVertex(_min.x,_max.y,_min.z);
			glVertex(_max.x,_max.y,_min.z);

			// left (x-min.)
			glVertex(_min.x,_max.y,_max.z);
			glVertex(_min.x,_max.y,_min.z);
			glVertex(_min.x,_min.y,_min.z);
			glVertex(_min.x,_min.y,_max.z);

			// right (x-max.)
			glVertex(_max.x,_max.y,_min.z);
			glVertex(_max.x,_max.y,_max.z);
			glVertex(_max.x,_min.y,_max.z);
			glVertex(_max.x,_min.y,_min.z);
		}
		glEnd();

	}
}

void
ZBoundingBox::drawWireSurface( const ZColor& wireColor, bool withNormal ) const
{
	if( !_initialized ) { return; }

	const bool lightingWasOn  = glIsEnabled( GL_LIGHTING ) ? true : false;
	const bool lightingWasOff = !lightingWasOn;

	if( lightingWasOff && withNormal ) { glEnable( GL_LIGHTING ); }

	glEnable( GL_POLYGON_OFFSET_FILL );
	glPolygonOffset( 1.f, 1.f );
	drawSurface( withNormal );
	glDisable( GL_POLYGON_OFFSET_FILL );

	if( lightingWasOn ) { glDisable( GL_LIGHTING ); }

	glColor( wireColor );
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	drawSurface( withNormal );
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

	if( lightingWasOn ) { glEnable( GL_LIGHTING ); }
}

ostream&
operator<<( ostream& os, const ZBoundingBox& object )
{
	os << "<ZBoundingBox>" << endl;
	os << " domain: " << object.minPoint() << " ~ " << object.maxPoint() << endl;
	os << " size  : " << object.width(0) << " x " << object.width(1) << " x " << object.width(2) << endl;
	return os;
}

ZELOS_NAMESPACE_END

