//-------------------//
// ZSimplexNoise.cpp //
//-------------------------------------------------------//
// author: Jinhyuk Bae @ Dexter Studios                  //
//         Wanho Choi @ Dexter Studios					 //
//         Nayoung Kim @ Dexter Studios                  //
// last update: 2018.03.08                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

// Gradient vectors preset for 2D lattice
const float ZSimplexNoise::_grad2lut[8][2] =
{
	{-1.f,-1.f}, {1.f, 0.f}, {-1.f,0.f}, {1.f, 1.f},
	{-1.f, 1.f}, {0.f,-1.f}, { 0.f,1.f}, {1.f,-1.f}
};

// Gradient vectors preset for 3D lattice
// These vectors are based on the midpoints of the 12 edges of a cube. (plus 4 repeats to make the array length a power of two)
// A larger array of random unit length vectors would also do the job, but these 12 work better.
// (They are not real random, so they are carefully chosen to represent a small, isotropic set of directions.)
const float ZSimplexNoise::_grad3lut[16][3] =
{
	{ 1.f, 0.f, 1.f}, { 0.f, 1.f, 1.f}, // 12 cube edges
	{-1.f, 0.f, 1.f}, { 0.f,-1.f, 1.f},
	{ 1.f, 0.f,-1.f}, { 0.f, 1.f,-1.f},
	{-1.f, 0.f,-1.f}, { 0.f,-1.f,-1.f},
	{ 1.f,-1.f, 0.f}, { 1.f, 1.f, 0.f},
	{-1.f, 1.f, 0.f}, {-1.f,-1.f, 0.f},
	{ 1.f, 0.f, 1.f}, {-1.f, 0.f, 1.f}, // 4 repeats to make 16
	{ 0.f, 1.f,-1.f}, { 0.f,-1.f,-1.f}
};

// Gradient vectors preset for 4D lattice
const float ZSimplexNoise::_grad4lut[32][4] =
{
	{ 0.f, 1.f, 1.f, 1.f}, { 0.f, 1.f, 1.f,-1.f}, { 0.f, 1.f,-1.f, 1.f}, { 0.f, 1.f,-1.f,-1.f}, // 32 tesseract edges
	{ 0.f,-1.f, 1.f, 1.f}, { 0.f,-1.f, 1.f,-1.f}, { 0.f,-1.f,-1.f, 1.f}, { 0.f,-1.f,-1.f,-1.f},
	{ 1.f, 0.f, 1.f, 1.f}, { 1.f, 0.f, 1.f,-1.f}, { 1.f, 0.f,-1.f, 1.f}, { 1.f, 0.f,-1.f,-1.f},
	{-1.f, 0.f, 1.f, 1.f}, {-1.f, 0.f, 1.f,-1.f}, {-1.f, 0.f,-1.f, 1.f}, {-1.f, 0.f,-1.f,-1.f},
	{ 1.f, 1.f, 0.f, 1.f}, { 1.f, 1.f, 0.f,-1.f}, { 1.f,-1.f, 0.f, 1.f}, { 1.f,-1.f, 0.f,-1.f},
	{-1.f, 1.f, 0.f, 1.f}, {-1.f, 1.f, 0.f,-1.f}, {-1.f,-1.f, 0.f, 1.f}, {-1.f,-1.f, 0.f,-1.f},
	{ 1.f, 1.f, 1.f, 0.f}, { 1.f, 1.f,-1.f, 0.f}, { 1.f,-1.f, 1.f, 0.f}, { 1.f,-1.f,-1.f, 0.f},
	{-1.f, 1.f, 1.f, 0.f}, {-1.f, 1.f,-1.f, 0.f}, {-1.f,-1.f, 1.f, 0.f}, {-1.f,-1.f,-1.f, 0.f}
};

// A lookup table to traverse the simplex around a given point in 4D.
// Details can be found where this table is used, in the 4D noise method.
// TODO: This should not be required, backport it from Bill's GLSL code!
const unsigned char ZSimplexNoise::_simplex[64][4] =
{
	{0,1,2,3}, {0,1,3,2}, {0,0,0,0}, {0,2,3,1}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {1,2,3,0},
	{0,2,1,3}, {0,0,0,0}, {0,3,1,2}, {0,3,2,1}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {1,3,2,0},
	{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0},
	{1,2,0,3}, {0,0,0,0}, {1,3,0,2}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {2,3,0,1}, {2,3,1,0},
	{1,0,2,3}, {1,0,3,2}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {2,0,3,1}, {0,0,0,0}, {2,1,3,0},
	{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0},
	{2,0,1,3}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {3,0,1,2}, {3,0,2,1}, {0,0,0,0}, {3,1,2,0},
	{2,1,0,3}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {3,1,0,2}, {0,0,0,0}, {3,2,0,1}, {3,2,1,0}
};

const float ZSimplexNoise::_F2 = 0.366025403f;	// = 0.5*(sqrt(3)-1)
const float ZSimplexNoise::_G2 = 0.211324865f;	// = (3-sqrt(3))/6

const float ZSimplexNoise::_F3 = 0.333333333f;	// = 1/3
const float ZSimplexNoise::_G3 = 0.166666667f;	// = 1/6

const float ZSimplexNoise::_F4 = 0.309016994f;	// = (sqrt(5)-1)/4
const float ZSimplexNoise::_G4 = 0.138196601f;	// = (5-sqrt(5))/20

ZSimplexNoise::ZSimplexNoise( unsigned int seed )
{
	_seed = -1;

	offset.zeroize();

	sFreq = ZVector(0.1f,0.1f,0.1f);
	tFreq = 1.0f;

	scale = 1.0f;
	lift  = 0.0f;

	shuffle( seed );
}

ZSimplexNoise::ZSimplexNoise( const ZSimplexNoise& s )
{
	this->operator=( s );
}

ZSimplexNoise&
ZSimplexNoise::operator=( const ZSimplexNoise& s )
{
	_seed = s._seed;
	memcpy( (char*)_perm, (char*)s._perm, 512*sizeof(int) );

	offset = s.offset;
	sFreq  = s.sFreq;
	tFreq  = s.tFreq;

	scale  = s.scale;
	lift   = s.lift;

	return (*this);
}

void
ZSimplexNoise::shuffle( unsigned int seed )
{
	if( _seed == (int)seed ) { return; }

	FOR( i, 0, 256 )
	{
		_perm[i] = _perm[i+256] = ZClamp( (int)ZRand(i+seed,256), 0, 255 );
	}

	_seed = (int)seed;
}

float
ZSimplexNoise::pureValue
(
	float x,
	float* dx
) const
{
	int i0 = ZFloor(x);
	int i1 = i0+1;
	float x0 = x-i0;
	float x1 = x0-1.f;

	float gx0, gx1;
	float n0, n1;
	float t20, t40, t21, t41;

	float x20 = x0*x0;
	float t0 = 1.f - x20;
	// if(t0 < 0.f) t0 = 0.f; // Never happens for 1D: x0<=1 always
	t20 = t0 * t0;
	t40 = t20 * t20;
	_grad( _perm[i0&0xff], &gx0 );
	n0 = t40 * gx0 * x0;

	float x21 = x1*x1;
	float t1 = 1.f - x21;
	// if(t1 < 0.f) t1 = 0.f; // Never happens for 1D: |x1|<=1 always
	t21 = t1 * t1;
	t41 = t21 * t21;
	_grad(_perm[i1&0xff], &gx1 );
	n1 = t41 * gx1 * x1;

	if( dx )
	{
		// Compute derivative according to:
		// *dx = -8.f * t20 * t0 * x0 * (gx0 * x0) + t40 * gx0;
		// *dx += -8.f * t21 * t1 * x1 * (gx1 * x1) + t41 * gx1;
		*dx = t20 * t0 * gx0 * x20;
		*dx += t21 * t1 * gx1 * x21;
		*dx *= -8.f;
		*dx += t40 * gx0 + t41 * gx1;
		*dx *= 0.25f; // Scale derivative to match the noise scaling
	}

	// The maximum value of this noise is 8*(3/4)^4 = 2.53125
	// A factor of 0.395 would scale to fit exactly within [-1,1], but
	// to better match classic Perlin noise, we scale it down some more.
	float noise = 0.25f * ( n0 + n1 );

	return noise;
}

float
ZSimplexNoise::pureValue
(
	float x, float y,
	float* dx, float* dy
) const
{
	float n0, n1, n2; // Noise contributions from the three simplex corners
	float gx0, gy0, gx1, gy1, gx2, gy2; // Gradients at simplex corners

	// Skew the input space to determine which simplex cell we're in
	float s = (x+y)*_F2; // Hairy factor for 2D
	float xs = x+s;
	float ys = y+s;
	int i = ZFloor(xs);
	int j = ZFloor(ys);

	float t = ( float )(i+j)*_G2;
	float X0 = i-t; // Unskew the cell origin back to (x,y) space
	float Y0 = j-t;
	float x0 = x-X0; // The x,y distances from the cell origin
	float y0 = y-Y0;

	// For the 2D case, the simplex shape is an equilateral triangle.
	// Determine which simplex we are in.
	int i1, j1; // Offsets for second (middle) corner of simplex in (i,j) coords
	if( x0 > y0 ) { i1 = 1; j1 = 0; } // lower triangle, XY order: (0,0)->(1,0)->(1,1)
	else          { i1 = 0; j1 = 1; } // upper triangle, YX order: (0,0)->(0,1)->(1,1)

	// A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
	// a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
	// c = (3-sqrt(3))/6
	float x1 = x0 - i1 + _G2; // Offsets for middle corner in (x,y) unskewed coords
	float y1 = y0 - j1 + _G2;
	float x2 = x0 - 1.f + 2.f * _G2; // Offsets for last corner in (x,y) unskewed coords
	float y2 = y0 - 1.f + 2.f * _G2;

	// Wrap the integer indices at 256, to avoid indexing _perm[] out of bounds
	int ii = i&0xff;
	int jj = j&0xff;

	// Calculate the contribution from the three corners
	float t0 = 0.5f - x0 * x0 - y0 * y0;
	float t20, t40;
	if( t0 < 0.f ) t40 = t20 = t0 = n0 = gx0 = gy0 = 0.f; // No influence
	else {
		_grad( _perm[ii + _perm[jj]], &gx0, &gy0 );
		t20 = t0 * t0;
		t40 = t20 * t20;
		n0 = t40 * ( gx0 * x0 + gy0 * y0 );
	}

	float t1 = 0.5f - x1 * x1 - y1 * y1;
	float t21, t41;
	if( t1 < 0.f ) t21 = t41 = t1 = n1 = gx1 = gy1 = 0.f; // No influence
	else {
		_grad( _perm[ii + i1 + _perm[jj + j1]], &gx1, &gy1 );
		t21 = t1 * t1;
		t41 = t21 * t21;
		n1 = t41 * ( gx1 * x1 + gy1 * y1 );
	}

	float t2 = 0.5f - x2 * x2 - y2 * y2;
	float t22, t42;
	if( t2 < 0.f ) t42 = t22 = t2 = n2 = gx2 = gy2 = 0.f; // No influence
	else {
		_grad( _perm[ii + 1 + _perm[jj + 1]], &gx2, &gy2 );
		t22 = t2 * t2;
		t42 = t22 * t22;
		n2 = t42 * ( gx2 * x2 + gy2 * y2 );
	}

	// Add contributions from each corner to get the final noise value.
	// The result is scaled to return values in the interval [-1,1].
	float noise = 40.f * ( n0 + n1 + n2 );

	// Compute derivative, if requested by supplying non-null pointers for the last two arguments
	if( dx && dy )
	{
		//  A straight, unoptimised calculation would be like:
		//   *dx = -8.f * t20 * t0 * x0 * ( gx0 * x0 + gy0 * y0 ) + t40 * gx0;
		//   *dy = -8.f * t20 * t0 * y0 * ( gx0 * x0 + gy0 * y0 ) + t40 * gy0;
		//   *dx += -8.f * t21 * t1 * x1 * ( gx1 * x1 + gy1 * y1 ) + t41 * gx1;
		//   *dy += -8.f * t21 * t1 * y1 * ( gx1 * x1 + gy1 * y1 ) + t41 * gy1;
		//   *dx += -8.f * t22 * t2 * x2 * ( gx2 * x2 + gy2 * y2 ) + t42 * gx2;
		//   *dy += -8.f * t22 * t2 * y2 * ( gx2 * x2 + gy2 * y2 ) + t42 * gy2;
		float temp0 = t20 * t0 * ( gx0* x0 + gy0 * y0 );
		*dx = temp0 * x0;
		*dy = temp0 * y0;
		float temp1 = t21 * t1 * ( gx1 * x1 + gy1 * y1 );
		*dx += temp1 * x1;
		*dy += temp1 * y1;
		float temp2 = t22 * t2 * ( gx2* x2 + gy2 * y2 );
		*dx += temp2 * x2;
		*dy += temp2 * y2;
		*dx *= -8.f;
		*dy *= -8.f;
		*dx += t40 * gx0 + t41 * gx1 + t42 * gx2;
		*dy += t40 * gy0 + t41 * gy1 + t42 * gy2;
		*dx *= 40.f; // Scale derivative to match the noise scaling
		*dy *= 40.f;
	}

	return noise;
}

float
ZSimplexNoise::pureValue
(
	float x, float y, float z,
	float* dx, float* dy, float* dz
) const
{
	float n0, n1, n2, n3; // Noise contributions from the four simplex corners
	float noise;          // Return value
	float gx0, gy0, gz0, gx1, gy1, gz1; // Gradients at simplex corners
	float gx2, gy2, gz2, gx3, gy3, gz3;

	// Skew the input space to determine which simplex cell we're in
	float s = (x+y+z)*_F3; // Very nice and simple skew factor for 3D
	float xs = x+s;
	float ys = y+s;
	float zs = z+s;
	int i = ZFloor(xs);
	int j = ZFloor(ys);
	int k = ZFloor(zs);

	float t = (float)(i+j+k)*_G3; 
	float X0 = i-t; // Unskew the cell origin back to (x,y,z) space
	float Y0 = j-t;
	float Z0 = k-t;
	float x0 = x-X0; // The x,y,z distances from the cell origin
	float y0 = y-Y0;
	float z0 = z-Z0;

	// For the 3D case, the simplex shape is a slightly irregular tetrahedron.
	// Determine which simplex we are in.
	int i1, j1, k1; // Offsets for second corner of simplex in (i,j,k) coords
	int i2, j2, k2; // Offsets for third corner of simplex in (i,j,k) coords

	// TODO: This code would benefit from a backport from the GLSL version!
	if( x0 >= y0 ) {
		if( y0 >=z0 )       { i1=1; j1=0; k1=0; i2=1; j2=1; k2=0; } // X Y Z order
		else if( x0 >= z0 ) { i1=1; j1=0; k1=0; i2=1; j2=0; k2=1; } // X Z Y order
		else                { i1=0; j1=0; k1=1; i2=1; j2=0; k2=1; } // Z X Y order
	} else { // x0 < y0
		if( y0 < z0 )       { i1=0; j1=0; k1=1; i2=0; j2=1; k2=1; } // Z Y X order
		else if( x0 < z0 )  { i1=0; j1=1; k1=0; i2=0; j2=1; k2=1; } // Y Z X order
		else                { i1=0; j1=1; k1=0; i2=1; j2=1; k2=0; } // Y X Z order
	}

	// A step of (1,0,0) in (i,j,k) means a step of (1-c,-c,-c) in (x,y,z),
	// a step of (0,1,0) in (i,j,k) means a step of (-c,1-c,-c) in (x,y,z), and
	// a step of (0,0,1) in (i,j,k) means a step of (-c,-c,1-c) in (x,y,z), where
	// c = 1/6.

	float x1 = x0 - i1 + _G3; // Offsets for second corner in (x,y,z) coords
	float y1 = y0 - j1 + _G3;
	float z1 = z0 - k1 + _G3;
	float x2 = x0 - i2 + 2.f * _G3; // Offsets for third corner in (x,y,z) coords
	float y2 = y0 - j2 + 2.f * _G3;
	float z2 = z0 - k2 + 2.f * _G3;
	float x3 = x0 - 1.f + 3.f * _G3; // Offsets for last corner in (x,y,z) coords
	float y3 = y0 - 1.f + 3.f * _G3;
	float z3 = z0 - 1.f + 3.f * _G3;

	// Wrap the integer indices at 256, to avoid indexing _perm[] out of bounds
	int ii = i&0xff;
	int jj = j&0xff;
	int kk = k&0xff;

	// Calculate the contribution from the four corners
	float t0 = 0.6f - x0*x0 - y0*y0 - z0*z0;
	float t20, t40;
	if( t0 < 0.f ) {
		n0 = t0 = t20 = t40 = gx0 = gy0 = gz0 = 0.f;
	} else {
		_grad( _perm[ii + _perm[jj + _perm[kk]]], &gx0, &gy0, &gz0 );
		t20 = t0 * t0;
		t40 = t20 * t20;
		n0 = t40 * ( gx0 * x0 + gy0 * y0 + gz0 * z0 );
	}

	float t1 = 0.6f - x1*x1 - y1*y1 - z1*z1;
	float t21, t41;
	if( t1 < 0.f ) {
		n1 = t1 = t21 = t41 = gx1 = gy1 = gz1 = 0.f;
	} else {
		_grad( _perm[ii + i1 + _perm[jj + j1 + _perm[kk + k1]]], &gx1, &gy1, &gz1 );
		t21 = t1 * t1;
		t41 = t21 * t21;
		n1 = t41 * ( gx1 * x1 + gy1 * y1 + gz1 * z1 );
	}

	float t2 = 0.6f - x2*x2 - y2*y2 - z2*z2;
	float t22, t42;
	if(t2 < 0.f) {
		n2 = t2 = t22 = t42 = gx2 = gy2 = gz2 = 0.f;
	} else {
		_grad( _perm[ii + i2 + _perm[jj + j2 + _perm[kk + k2]]], &gx2, &gy2, &gz2 );
		t22 = t2 * t2;
		t42 = t22 * t22;
		n2 = t42 * ( gx2 * x2 + gy2 * y2 + gz2 * z2 );
	}

	float t3 = 0.6f - x3*x3 - y3*y3 - z3*z3;
	float t23, t43;
	if(t3 < 0.f) {
		n3 = t3 = t23 = t43 = gx3 = gy3 = gz3 = 0.f;
	} else {
		_grad( _perm[ii + 1 + _perm[jj + 1 + _perm[kk + 1]]], &gx3, &gy3, &gz3 );
		t23 = t3 * t3;
		t43 = t23 * t23;
		n3 = t43 * ( gx3 * x3 + gy3 * y3 + gz3 * z3 );
	}

	// Add contributions from each corner to get the final noise value.
	// The result is scaled to return values in the range [-1,1]
	noise = 28.f * (n0 + n1 + n2 + n3);

	// Compute derivative, if requested by supplying non-null pointers
	// for the last three arguments
	if( dx && dy && dz )
	{
		// A straight, unoptimised calculation would be like:
		//    *dx = -8.f * t20 * t0 * x0 * dot(gx0, gy0, gz0, x0, y0, z0) + t40 * gx0;
		//   *dy = -8.f * t20 * t0 * y0 * dot(gx0, gy0, gz0, x0, y0, z0) + t40 * gy0;
		//   *dz = -8.f * t20 * t0 * z0 * dot(gx0, gy0, gz0, x0, y0, z0) + t40 * gz0;
		//   *dx += -8.f * t21 * t1 * x1 * dot(gx1, gy1, gz1, x1, y1, z1) + t41 * gx1;
		//   *dy += -8.f * t21 * t1 * y1 * dot(gx1, gy1, gz1, x1, y1, z1) + t41 * gy1;
		//   *dz += -8.f * t21 * t1 * z1 * dot(gx1, gy1, gz1, x1, y1, z1) + t41 * gz1;
		//   *dx += -8.f * t22 * t2 * x2 * dot(gx2, gy2, gz2, x2, y2, z2) + t42 * gx2;
		//   *dy += -8.f * t22 * t2 * y2 * dot(gx2, gy2, gz2, x2, y2, z2) + t42 * gy2;
		//   *dz += -8.f * t22 * t2 * z2 * dot(gx2, gy2, gz2, x2, y2, z2) + t42 * gz2;
		//   *dx += -8.f * t23 * t3 * x3 * dot(gx3, gy3, gz3, x3, y3, z3) + t43 * gx3;
		//   *dy += -8.f * t23 * t3 * y3 * dot(gx3, gy3, gz3, x3, y3, z3) + t43 * gy3;
		//   *dz += -8.f * t23 * t3 * z3 * dot(gx3, gy3, gz3, x3, y3, z3) + t43 * gz3;
		float temp0 = t20 * t0 * ( gx0 * x0 + gy0 * y0 + gz0 * z0 );
		*dx = temp0 * x0;
		*dy = temp0 * y0;
		*dz = temp0 * z0;
		float temp1 = t21 * t1 * ( gx1 * x1 + gy1 * y1 + gz1 * z1 );
		*dx += temp1 * x1;
		*dy += temp1 * y1;
		*dz += temp1 * z1;
		float temp2 = t22 * t2 * ( gx2 * x2 + gy2 * y2 + gz2 * z2 );
		*dx += temp2 * x2;
		*dy += temp2 * y2;
		*dz += temp2 * z2;
		float temp3 = t23 * t3 * ( gx3 * x3 + gy3 * y3 + gz3 * z3 );
		*dx += temp3 * x3;
		*dy += temp3 * y3;
		*dz += temp3 * z3;
		*dx *= -8.f;
		*dy *= -8.f;
		*dz *= -8.f;
		*dx += t40 * gx0 + t41 * gx1 + t42 * gx2 + t43 * gx3;
		*dy += t40 * gy0 + t41 * gy1 + t42 * gy2 + t43 * gy3;
		*dz += t40 * gz0 + t41 * gz1 + t42 * gz2 + t43 * gz3;
		*dx *= 28.f; // Scale derivative to match the noise scaling
		*dy *= 28.f;
		*dz *= 28.f;
	}

	return noise;
}

float
ZSimplexNoise::pureValue
(
	float x, float y, float z, float w,
	float* dx, float* dy, float* dz, float* dw
) const
{
	float n0, n1, n2, n3, n4; // Noise contributions from the five corners
	float noise; // Return value
	float gx0, gy0, gz0, gw0, gx1, gy1, gz1, gw1; // Gradients at simplex corners
	float gx2, gy2, gz2, gw2, gx3, gy3, gz3, gw3, gx4, gy4, gz4, gw4;
	float t20, t21, t22, t23, t24;
	float t40, t41, t42, t43, t44;

	// Skew the (x,y,z,w) space to determine which cell of 24 simplices we're in
	float s = (x + y + z + w) * _F4; // Factor for 4D skewing
	float xs = x + s;
	float ys = y + s;
	float zs = z + s;
	float ws = w + s;
	int i = ZFloor(xs);
	int j = ZFloor(ys);
	int k = ZFloor(zs);
	int l = ZFloor(ws);

	float t = (i + j + k + l) * _G4; // Factor for 4D unskewing
	float X0 = i - t; // Unskew the cell origin back to (x,y,z,w) space
	float Y0 = j - t;
	float Z0 = k - t;
	float W0 = l - t;

	float x0 = x - X0;  // The x,y,z,w distances from the cell origin
	float y0 = y - Y0;
	float z0 = z - Z0;
	float w0 = w - W0;

	// For the 4D case, the simplex is a 4D shape I won't even try to describe.
	// To find out which of the 24 possible simplices we're in, we need to
	// determine the magnitude ordering of x0, y0, z0 and w0.
	// The method below is a reasonable way of finding the ordering of x,y,z,w
	// and then find the correct traversal order for the simplex were in.
	// First, six pair-wise comparisons are performed between each possible pair
	// of the four coordinates, and then the results are used to add up binary
	// bits for an integer index into a precomputed lookup table, simplex[].
	int c1 = (x0 > y0) ? 32 : 0;
	int c2 = (x0 > z0) ? 16 : 0;
	int c3 = (y0 > z0) ?  8 : 0;
	int c4 = (x0 > w0) ?  4 : 0;
	int c5 = (y0 > w0) ?  2 : 0;
	int c6 = (z0 > w0) ?  1 : 0;
	int c = c1 | c2 | c3 | c4 | c5 | c6; // '|' is mostly faster than '+'

	int i1, j1, k1, l1; // The integer offsets for the second simplex corner
	int i2, j2, k2, l2; // The integer offsets for the third simplex corner
	int i3, j3, k3, l3; // The integer offsets for the fourth simplex corner

	// simplex[c] is a 4-vector with the numbers 0, 1, 2 and 3 in some order.
	// Many values of c will never occur, since e.g. x>y>z>w makes x<z, y<w and x<w
	// impossible. Only the 24 indices which have non-zero entries make any sense.
	// We use a thresholding to set the coordinates in turn from the largest magnitude.
	// The number 3 in the "simplex" array is at the position of the largest coordinate.
	i1 = _simplex[c][0]>=3 ? 1 : 0;
	j1 = _simplex[c][1]>=3 ? 1 : 0;
	k1 = _simplex[c][2]>=3 ? 1 : 0;
	l1 = _simplex[c][3]>=3 ? 1 : 0;
	// The number 2 in the "simplex" array is at the second largest coordinate.
	i2 = _simplex[c][0]>=2 ? 1 : 0;
	j2 = _simplex[c][1]>=2 ? 1 : 0;
	k2 = _simplex[c][2]>=2 ? 1 : 0;
	l2 = _simplex[c][3]>=2 ? 1 : 0;
	// The number 1 in the "simplex" array is at the second smallest coordinate.
	i3 = _simplex[c][0]>=1 ? 1 : 0;
	j3 = _simplex[c][1]>=1 ? 1 : 0;
	k3 = _simplex[c][2]>=1 ? 1 : 0;
	l3 = _simplex[c][3]>=1 ? 1 : 0;
	// The fifth corner has all coordinate offsets = 1, so no need to look that up.

	float x1 = x0 - i1 + _G4; // Offsets for second corner in (x,y,z,w) coords
	float y1 = y0 - j1 + _G4;
	float z1 = z0 - k1 + _G4;
	float w1 = w0 - l1 + _G4;
	float x2 = x0 - i2 + 2.f * _G4; // Offsets for third corner in (x,y,z,w) coords
	float y2 = y0 - j2 + 2.f * _G4;
	float z2 = z0 - k2 + 2.f * _G4;
	float w2 = w0 - l2 + 2.f * _G4;
	float x3 = x0 - i3 + 3.f * _G4; // Offsets for fourth corner in (x,y,z,w) coords
	float y3 = y0 - j3 + 3.f * _G4;
	float z3 = z0 - k3 + 3.f * _G4;
	float w3 = w0 - l3 + 3.f * _G4;
	float x4 = x0 - 1.f + 4.f * _G4; // Offsets for last corner in (x,y,z,w) coords
	float y4 = y0 - 1.f + 4.f * _G4;
	float z4 = z0 - 1.f + 4.f * _G4;
	float w4 = w0 - 1.f + 4.f * _G4;

	// Wrap the integer indices at 256, to avoid indexing _perm[] out of bounds
	int ii = i&0xff;
	int jj = j&0xff;
	int kk = k&0xff;
	int ll = l&0xff;

	// Calculate the contribution from the five corners
	float t0 = 0.6f - x0*x0 - y0*y0 - z0*z0 - w0*w0;
	if( t0 < 0.f ) {
		n0 = t0 = t20 = t40 = gx0 = gy0 = gz0 = gw0 = 0.f;
	} else {
		t20 = t0 * t0;
		t40 = t20 * t20;
		_grad(_perm[ii+_perm[jj+_perm[kk+_perm[ll]]]], &gx0, &gy0, &gz0, &gw0 );
		n0 = t40 * ( gx0 * x0 + gy0 * y0 + gz0 * z0 + gw0 * w0 );
	}

	float t1 = 0.6f - x1*x1 - y1*y1 - z1*z1 - w1*w1;
	if( t1 < 0.f ) {
		n1 = t1 = t21 = t41 = gx1 = gy1 = gz1 = gw1 = 0.f;
	} else {
		t21 = t1 * t1;
		t41 = t21 * t21;
		_grad(_perm[ii+i1+_perm[jj+j1+_perm[kk+k1+_perm[ll+l1]]]], &gx1, &gy1, &gz1, &gw1 );
		n1 = t41 * ( gx1 * x1 + gy1 * y1 + gz1 * z1 + gw1 * w1 );
	}

	float t2 = 0.6f - x2*x2 - y2*y2 - z2*z2 - w2*w2;
	if( t2 < 0.f ) {
		n2 = t2 = t22 = t42 = gx2 = gy2 = gz2 = gw2 = 0.f;
	} else {
		t22 = t2 * t2;
		t42 = t22 * t22;
		_grad(_perm[ii+i2+_perm[jj+j2+_perm[kk+k2+_perm[ll+l2]]]], &gx2, &gy2, &gz2, &gw2 );
		n2 = t42 * ( gx2 * x2 + gy2 * y2 + gz2 * z2 + gw2 * w2 );
	}

	float t3 = 0.6f - x3*x3 - y3*y3 - z3*z3 - w3*w3;
	if( t3 < 0.f ) {
		n3 = t3 = t23 = t43 = gx3 = gy3 = gz3 = gw3 = 0.f;
	} else {
		t23 = t3 * t3;
		t43 = t23 * t23;
		_grad(_perm[ii+i3+_perm[jj+j3+_perm[kk+k3+_perm[ll+l3]]]], &gx3, &gy3, &gz3, &gw3 );
		n3 = t43 * ( gx3 * x3 + gy3 * y3 + gz3 * z3 + gw3 * w3 );
	}

	float t4 = 0.6f - x4*x4 - y4*y4 - z4*z4 - w4*w4;
	if( t4 < 0.f ) {
		n4 = t4 = t24 = t44 = gx4 = gy4 = gz4 = gw4 = 0.f;
	} else {
		t24 = t4 * t4;
		t44 = t24 * t24;
		_grad(_perm[ii+1+_perm[jj+1+_perm[kk+1+_perm[ll+1]]]], &gx4, &gy4, &gz4, &gw4 );
		n4 = t44 * ( gx4 * x4 + gy4 * y4 + gz4 * z4 + gw4 * w4 );
	}

	// Sum up and scale the result to cover the range [-1,1]
	noise = 27.f * (n0 + n1 + n2 + n3 + n4); // TODO: The scale factor is preliminary!

	// Compute derivative, if requested by supplying non-null pointers
	// for the last four arguments
	if( dx && dy && dz && dw )
	{
		//  A straight, unoptimised calculation would be like:
		//   *dx = -8.f * t20 * t0 * x0 * dot(gx0, gy0, gz0, gw0, x0, y0, z0, w0) + t40 * gx0;
		//   *dy = -8.f * t20 * t0 * y0 * dot(gx0, gy0, gz0, gw0, x0, y0, z0, w0) + t40 * gy0;
		//   *dz = -8.f * t20 * t0 * z0 * dot(gx0, gy0, gz0, gw0, x0, y0, z0, w0) + t40 * gz0;
		//   *dw = -8.f * t20 * t0 * w0 * dot(gx0, gy0, gz0, gw0, x0, y0, z0, w0) + t40 * gw0;
		//   *dx += -8.f * t21 * t1 * x1 * dot(gx1, gy1, gz1, gw1, x1, y1, z1, w1) + t41 * gx1;
		//   *dy += -8.f * t21 * t1 * y1 * dot(gx1, gy1, gz1, gw1, x1, y1, z1, w1) + t41 * gy1;
		//   *dz += -8.f * t21 * t1 * z1 * dot(gx1, gy1, gz1, gw1, x1, y1, z1, w1) + t41 * gz1;
		//   *dw = -8.f * t21 * t1 * w1 * dot(gx1, gy1, gz1, gw1, x1, y1, z1, w1) + t41 * gw1;
		//   *dx += -8.f * t22 * t2 * x2 * dot(gx2, gy2, gz2, gw2, x2, y2, z2, w2) + t42 * gx2;
		//   *dy += -8.f * t22 * t2 * y2 * dot(gx2, gy2, gz2, gw2, x2, y2, z2, w2) + t42 * gy2;
		//   *dz += -8.f * t22 * t2 * z2 * dot(gx2, gy2, gz2, gw2, x2, y2, z2, w2) + t42 * gz2;
		//   *dw += -8.f * t22 * t2 * w2 * dot(gx2, gy2, gz2, gw2, x2, y2, z2, w2) + t42 * gw2;
		//   *dx += -8.f * t23 * t3 * x3 * dot(gx3, gy3, gz3, gw3, x3, y3, z3, w3) + t43 * gx3;
		//   *dy += -8.f * t23 * t3 * y3 * dot(gx3, gy3, gz3, gw3, x3, y3, z3, w3) + t43 * gy3;
		//   *dz += -8.f * t23 * t3 * z3 * dot(gx3, gy3, gz3, gw3, x3, y3, z3, w3) + t43 * gz3;
		//   *dw += -8.f * t23 * t3 * w3 * dot(gx3, gy3, gz3, gw3, x3, y3, z3, w3) + t43 * gw3;
		//   *dx += -8.f * t24 * t4 * x4 * dot(gx4, gy4, gz4, gw4, x4, y4, z4, w4) + t44 * gx4;
		//   *dy += -8.f * t24 * t4 * y4 * dot(gx4, gy4, gz4, gw4, x4, y4, z4, w4) + t44 * gy4;
		//   *dz += -8.f * t24 * t4 * z4 * dot(gx4, gy4, gz4, gw4, x4, y4, z4, w4) + t44 * gz4;
		//   *dw += -8.f * t24 * t4 * w4 * dot(gx4, gy4, gz4, gw4, x4, y4, z4, w4) + t44 * gw4;
		float temp0 = t20 * t0 * ( gx0 * x0 + gy0 * y0 + gz0 * z0 + gw0 * w0 );
		*dx = temp0 * x0;
		*dy = temp0 * y0;
		*dz = temp0 * z0;
		*dw = temp0 * w0;
		float temp1 = t21 * t1 * ( gx1 * x1 + gy1 * y1 + gz1 * z1 + gw1 * w1 );
		*dx += temp1 * x1;
		*dy += temp1 * y1;
		*dz += temp1 * z1;
		*dw += temp1 * w1;
		float temp2 = t22 * t2 * ( gx2 * x2 + gy2 * y2 + gz2 * z2 + gw2 * w2 );
		*dx += temp2 * x2;
		*dy += temp2 * y2;
		*dz += temp2 * z2;
		*dw += temp2 * w2;
		float temp3 = t23 * t3 * ( gx3 * x3 + gy3 * y3 + gz3 * z3 + gw3 * w3 );
		*dx += temp3 * x3;
		*dy += temp3 * y3;
		*dz += temp3 * z3;
		*dw += temp3 * w3;
		float temp4 = t24 * t4 * ( gx4 * x4 + gy4 * y4 + gz4 * z4 + gw4 * w4 );
		*dx += temp4 * x4;
		*dy += temp4 * y4;
		*dz += temp4 * z4;
		*dw += temp4 * w4;
		*dx *= -8.f;
		*dy *= -8.f;
		*dz *= -8.f;
		*dw *= -8.f;
		*dx += t40 * gx0 + t41 * gx1 + t42 * gx2 + t43 * gx3 + t44 * gx4;
		*dy += t40 * gy0 + t41 * gy1 + t42 * gy2 + t43 * gy3 + t44 * gy4;
		*dz += t40 * gz0 + t41 * gz1 + t42 * gz2 + t43 * gz3 + t44 * gz4;
		*dw += t40 * gw0 + t41 * gw1 + t42 * gw2 + t43 * gw3 + t44 * gw4;

		*dx *= 28.f; // Scale derivative to match the noise scaling
		*dy *= 28.f;
		*dz *= 28.f;
		*dw *= 28.f;
	}

	return noise;
}

float
ZSimplexNoise::turbulence( float x, float y, float z, float t, int numOctaves ) const
{
	float sum  = 0;
	float freq = 1;

	FOR( o, 0, numOctaves )
	{
		sum += value( freq*x, freq*y, freq*z, t ) / freq;
		freq *= 2;
	}

	return sum;
}

float
ZSimplexNoise::fBm( float x, float y, float z, float t, int numOctaves, float amp, float freq, float rough ) const
{
	float sum = 0.f;
	float lun = 2.f;

	FOR( o, 0, numOctaves )
	{
		sum  += value( freq*x, freq*y, freq*z, t ) * amp;
		amp  *= rough;
		freq *= lun;
	}

	return sum;
}

ostream&
operator<<( ostream& os, const ZSimplexNoise& object )
{
	os << "<ZSimplexNoise>" << endl;
	os << " offset           : " << object.offset << endl;
	os << " Spatial Freqency : " << object.sFreq << endl;
	os << " Time    Freqency : " << object.tFreq << endl;
	os << " Scale			 : " << object.scale << endl;
	os << " Lift 			 : " << object.lift  << endl;
	
	return os;
}

ZELOS_NAMESPACE_END

