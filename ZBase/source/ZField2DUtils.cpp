//-------------------//
// ZField2DUtils.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
//         Nayoung Kim @ Dexter Studios                  //
// last update: 2016.09.19                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

bool
Gradient( ZVectorField2D& v, const ZScalarField2D& s, bool useOpenMP )
{
	if( !v.directComputable(s) )
	{
		cout << "Error@Gradient(): Not direct computable." << endl;
		return false;
	}

	const int n = v.numElements();

	const int iMax = v.iMax();
	const int kMax = v.kMax();

	const float _dx = 1/v.dx();
	const float _dz = 1/v.dz();

	const float _dx2 = 0.5f*_dx;
	const float _dz2 = 0.5f*_dz;

	#pragma omp parallel for if( useOpenMP && n>10000 )
	PER_EACH_ELEMENT_2D( v )

		const int idx = v.index(i,k);

		float _Dx=0;
		int i0=0, i1=0;
		if( iMax > 0 )
		{
			if( i==0 )         { _Dx=_dx;  i0 = idx;       i1 = s.i1(idx); }
			else if( i==iMax ) { _Dx=_dx;  i0 = s.i0(idx); i1 = idx;       }
			else               { _Dx=_dx2; i0 = s.i0(idx); i1 = s.i1(idx); }
		}

		float _Dz=0;
		int k0=0, k1=0;
		if( kMax > 0 )
		{
			if( k==0 )         { _Dz=_dz;  k0 = idx;       k1 = s.k1(idx); }
			else if( k==kMax ) { _Dz=_dz;  k0 = s.k0(idx); k1 = idx;       }
			else               { _Dz=_dz2; k0 = s.k0(idx); k1 = s.k1(idx); }
		}

		ZVector& vv = v[idx];
		{
			vv.x = ( s[i1] - s[i0] ) * _Dx;
			vv.y = 0.f;
			vv.z = ( s[k1] - s[k0] ) * _Dz;
		}


	END_PER_EACH_2D

	return true;
}

bool
Divergence( ZScalarField2D& s, const ZVectorField2D& v, bool useOpenMP )
{
	if( !s.directComputableWithoutLocation(v) )
	{
		cout << "Error@Divergence(): Not direct computable." << endl;
		return false;
	}

	if( s.location()!=ZFieldLocation::zCell || v.location()!=ZFieldLocation::zNode )
	{
		cout << "Error@Divergence(): Invalid location." << endl;
		return false;
	}

	const int nElems = v.numElements();

	const float dx = s.dx();
	const float dz = s.dz();

	#pragma omp parallel for if( useOpenMP && nElems>10000 )
	PER_EACH_ELEMENT_2D( s )

		ZInt4 n;
		s.getNodesOfCell( i,k, n );

		float& dvg = s(i,k);

		dvg += dz * ( ( v[n[1]].x + v[n[2]].x )
                    - ( v[n[0]].x + v[n[3]].x ) );

		dvg += dx * ( ( v[n[2]].z + v[n[3]].z )
                    - ( v[n[0]].z + v[n[1]].z ) );

	END_PER_EACH_2D

	return true;
}

ZPoint 
TracedPositionByLinear( const ZPoint& startPosition, const ZVector& startVelocity, float dt )
{
	return ( startPosition + dt*startVelocity );
}

bool
ZAdvect( ZVectorField2D& v, const ZVectorField2D& vel, float dt, bool useOpenMP )
{
	if( !v.directComputable(vel) )
	{
		cout << "Error@ZAdvect(): Not direct computable." << endl;
		return false;
	}

	if( v.location()!=ZFieldLocation::zNode || vel.location()!=ZFieldLocation::zNode )
	{
		cout << "Error@ZAdvect(): Invalid location." << endl;
		return false;
	}

	ZVectorField2D vectorTmp( v );

	const int nElems = v.numElements();
	#pragma omp parallel for if( useOpenMP && nElems>10000 )
	PER_EACH_ELEMENT_2D( v )
		const int idx = v.index(i,k);
		v[idx] = vectorTmp.lerp( TracedPositionByLinear(v.position(i,k), vel[idx], -dt ) );
	END_PER_EACH_2D

	return true;
}

bool
ZAdvect( ZScalarField2D& s, const ZVectorField2D& vel, float dt, bool useOpenMP )
{
	if( !s.directComputable(vel) )
	{
		cout << "Error@ZAdvect(): Not direct computable." << endl;
		return false;
	}

	if( s.location()!=ZFieldLocation::zNode || vel.location()!=ZFieldLocation::zNode )
	{
		cout << "Error@ZAdvect(): Invalid location." << endl;
		return false;
	}

	ZScalarField2D scalarTmp( s );

	const int nElems = s.numElements();
	#pragma omp parallel for if( useOpenMP && nElems>10000 )
	PER_EACH_ELEMENT_2D( s )
		const int idx = s.index(i,k);
		ZPoint backPos = TracedPositionByLinear( s.position(i,k), vel[idx], -dt );
		s[idx] = scalarTmp.lerp( backPos );
	END_PER_EACH_2D

	return true;
}

void 
GetVelField( ZVectorField2D& vel, const ZPointArray& vPos, const ZPointArray& vPos0, bool useOpenMP )
{
	const int numVel   = vel.length();
	const int numVPos  = vPos.length();
	const int numVPos0 = vPos0.length();

	if( numVel != numVPos || numVPos != numVPos0 )
	{
		cout << "Error@GetVelField(): Different array length." << endl;
		return;
	}

	#pragma omp parallel for if( useOpenMP && numVel>10000 )
	PER_EACH_ELEMENT_2D( vel )
		const int idx = vel.index(i,k);
		vel[idx] = vPos[idx] - vPos0[idx];
	END_PER_EACH_2D
}

ZVector
ZMinValue( const ZVectorField2D& field, bool useOpenMP )
{
	const int length = field.length();
	if( length <= 0 ) { return ZVector(0,0,0); }
	
	float minValue = Z_LARGE;
	ZVector ret(0,0,0);

	#pragma omp parallel for if( useOpenMP && length>10000 )
	PER_EACH_ELEMENT_2D( field )
		const int idx = field.index(i,k);
		float lengthSQ = field[idx].squaredLength();
		if( lengthSQ < minValue ) 
		{
			ret = field[idx];
			minValue = lengthSQ;
		}
	END_PER_EACH_2D

	return ret;
}

ZVector
ZMaxValue( const ZVectorField2D& field, bool useOpenMP )
{
	const int length = field.length();
	if( length <= 0 ) { return ZVector(0,0,0); }
	
	float maxValue = 0;
	ZVector ret(0,0,0);

	#pragma omp parallel for if( useOpenMP && length>10000 )
	PER_EACH_ELEMENT_2D( field )
		const int idx = field.index(i,k);
		float lengthSQ = field[idx].squaredLength();
		if( lengthSQ > maxValue ) 
		{
			ret = field[idx];
			maxValue = lengthSQ;
		}
	END_PER_EACH_2D

	return ret;
}

float
ZMinValue( const ZScalarField2D& field, bool useOpenMP )
{
	const int length = field.length();
	if( length <= 0 ) { return 0; }
	
	float minValue = Z_LARGE;

	#pragma omp parallel for if( useOpenMP && length>10000 )
	PER_EACH_ELEMENT_2D( field )
		const int idx = field.index(i,k);
		if( field[idx] < minValue ) 
		{
			minValue = field[idx];
		}
	END_PER_EACH_2D

	return minValue;
}

float
ZMaxValue( const ZScalarField2D& field, bool useOpenMP )
{
	const int length = field.length();
	if( length <= 0 ) { return 0; }
	
	float maxValue = -Z_LARGE;

	#pragma omp parallel for if( useOpenMP && length>10000 )
	PER_EACH_ELEMENT_2D( field )
		const int idx = field.index(i,k);
		if( field[idx] > maxValue ) 
		{
			maxValue = field[idx];
		}
	END_PER_EACH_2D

	return maxValue;
}

float
ZMinAbsValue( const ZScalarField2D& field, bool useOpenMP )
{
	const int length = field.length();
	if( length <= 0 ) { return 0; }
	
	float minValue = Z_LARGE;

	#pragma omp parallel for if( useOpenMP && length>10000 )
	PER_EACH_ELEMENT_2D( field )
		const int idx = field.index(i,k);
		if( ZAbs(field[idx]) < minValue ) 
		{
			minValue = field[idx];
		}
	END_PER_EACH_2D

	return minValue;
}

float
ZMaxAbsValue( const ZScalarField2D& field, bool useOpenMP )
{
	const int length = field.length();
	if( length <= 0 ) { return 0; }
	
	float maxValue = 0;

	#pragma omp parallel for if( useOpenMP && length>10000 )
	PER_EACH_ELEMENT_2D( field )
		const int idx = field.index(i,k);
		if( ZAbs(field[idx]) > maxValue ) 
		{
			maxValue = field[idx];
		}
	END_PER_EACH_2D

	return maxValue;
}

bool
MapToField( const ZImageMap& img, ZVectorField2D& rgb, ZScalarField2D& cusp, ZScalarField2D& foam, bool useOpenMP )
{
	const int Nx = img.width();
	const int Nz = img.height();

	if( Nx != Nz )
	{
		cout << "Error@ZField2DUtils::MapToField(): Image width and height values are not equal." << endl;
		return false;
	}

	const int length = Nx * Nz;
	const int numChannels = img.numChannels();

	if( numChannels < 3 )
	{
		cout << "Error@ZField2DUtils::MapToField(): The number of the image channels should be at least 3." << endl;
		return false;
	}

	// Displacement in RGB channel
	rgb.set( Nx, Nz );

	#pragma omp parallel for if( useOpenMP && length>10000 )
	PER_EACH_ELEMENT_2D( rgb )
		const int idx = rgb.index(i,k);
		ZVector& val = rgb[idx];
		val.x = img(i,k,0);
		val.y = img(i,k,1);
		val.z = img(i,k,2);
	END_PER_EACH_2D

	if( numChannels == 4 || numChannels == 5 )
	{
		// Cusp in Alpha channel
		cusp.set( Nx, Nz );

		#pragma omp parallel for if( useOpenMP && length>10000 )
		PER_EACH_ELEMENT_2D( cusp )
			const int idx = cusp.index(i,k);
			float& a = cusp[idx];
			a = img(i,k,3);
		END_PER_EACH_2D

		if( numChannels == 5 )
		{
			// Accumulated foam in 5th channel
			foam.set( Nx, Nz );

			#pragma omp parallel for if( useOpenMP && length>10000 )
			PER_EACH_ELEMENT_2D( foam )
				const int idx = foam.index(i,k);
				float& f = foam[idx];
				f = img(i,k,4);
			END_PER_EACH_2D
		}
	}

	return true;
}

bool
MapToVectorField( const ZImageMap& img, ZVectorField2D& field, bool useOpenMP )
{
	const int Nx = img.width();
	const int Nz = img.height();

	if( Nx != Nz )
	{
		cout << "Error@ZField2DUtils::MapToVectorField(): Image width and height values are not equal." << endl;
		return false;
	}

	const int length = Nx * Nz;
	const int numChannels = img.numChannels();

	if( numChannels < 3 )
	{
		cout << "Error@ZField2DUtils::MapToVectorField(): The number of the image channels should be at least 3." << endl;
		return false;
	}

	field.set( Nx, Nz );

	#pragma omp parallel for if( useOpenMP && length>10000 )
	PER_EACH_ELEMENT_2D( field )
		const int idx = field.index(i,k);
		ZVector& val = field[idx];
		val.x = img(i,k,0);
		val.y = img(i,k,1);
		val.z = img(i,k,2);
	END_PER_EACH_2D

	return true;
}

bool
MapToScalarField( const ZImageMap& img, ZScalarField2D& field, int n, bool useOpenMP )
{
	const int Nx = img.width();
	const int Nz = img.height();

	if( Nx != Nz )
	{
		cout << "Error@ZField2DUtils::MapToScalarField(): Image width and height values are not equal." << endl;
		return false;
	}

	const int length = Nx * Nz;
	const int numChannels = img.numChannels();

	if( n+1 > numChannels )
	{
		cout << "Error@ZField2DUtils::MapToScalarField(): This image has only " << numChannels << " channels." << endl;
		return false;
	}

	field.set( Nx, Nz );

	#pragma omp parallel for if( useOpenMP && length>10000 )
	PER_EACH_ELEMENT_2D( field )
		const int idx = field.index(i,k);
		float& val = field[idx];
		val = img(i,k,n);
	END_PER_EACH_2D

	return true;
}

bool
ArrayToField( const ZVectorArray& arr, ZVectorField2D& field, bool useOpenMP )
{
	const int numV = arr.length();

	if( !numV )
	{
		cout << "Error@ZField2DUtils: the input array is empty." << endl;
		return false;
	}

	const float nf = sqrtf( numV );
	const int n = (int)nf;

	if( (nf-n) != 0 )
	{
		cout << "Error@ZField2DUtils: the input mesh is not square." << endl;
		return false;
	}

	field.set( n, n );
	{
		PER_EACH_ELEMENT_2D( field )
			const int idx = field.index(i,k);
			ZVector& v = field[idx];
			v = arr[idx];
		END_PER_EACH_2D
	}

	return true;
}

void
VectorFieldLerp( const ZPoint& p, const ZVectorField2D& field, ZVector& vec, float Lx, float Lz )
{
	const int Nx = field.nx();
	const int Nz = field.nz();

	float x = p.x;
	float z = p.z;

	// to the periodic voxel space
	float u = fmod( x/Lx, 1.f );
	float v = fmod( z/Lz, 1.f );

	if( u < 0 ) { u += 1.f; }
	if( v < 0 ) { v += 1.f; }

	float uu = u*Nx;
	float vv = v*Nz;

	int i0 = (int)floor(uu);
	int k0 = (int)floor(vv);

	int i1 = i0+1;
	int k1 = k0+1;

	float fx = uu-i0;
	float fz = vv-k0;

	i0 = i0 % Nx;
	k0 = k0 % Nz;

	i1 = i1 % Nx;
	k1 = k1 % Nz;

	const int idx[4] = { field.index(i0,k0), field.index(i1,k0), field.index(i1,k1), field.index(i0,k1) };

	const float _fx = 1-fx;
	const float _fz = 1-fz;
	const float wgt[4] = { _fx*_fz, fx*_fz, fx*fz, _fx*fz };

	const ZVector* _field = field.pointer();
	FOR(l,0,4) { vec += wgt[l] * _field[idx[l]]; }
}

void
VectorFieldInterpolate( const ZPointArray& pos, const ZVectorField2D& field, ZVectorArray& arr, float Lx, float Lz )
{
	const int numVerts = pos.length();
	if( arr.length() != numVerts ) { arr.setLength( numVerts ); }

	#pragma omp parallel for
	FOR( i, 0, numVerts )
	{
		VectorFieldLerp( pos[i], field, arr[i], Lx, Lz );
	}
}

void
VectorFieldLerpUV( const ZPoint& uv, const ZVectorField2D& field, ZVector& vec, float scaleS, float scaleT, float offsetS, float offsetT )
{
	const int Nx = field.nx();
	const int Nz = field.nz(); 

	float u = uv.x;
	float v = 1-uv.y;

	if( u < 0 ) { u += 1.f; }
	if( v < 0 ) { v += 1.f; }

//	float uu = u * Nx;
//	float vv = v * Nz;

	float uu = ( u * scaleS + offsetS ) * Nx;
	float vv = ( v * scaleT + offsetT ) * Nz;

	// avoid the negative value.
	while( uu < 0 ) { uu += 1.f; }
	while( vv < 0 ) { vv += 1.f; }

	int i0 = (int)floor(uu);
	int k0 = (int)floor(vv);

	int i1 = i0+1;
	int k1 = k0+1;

	float fx = uu-i0;
	float fz = vv-k0;

	i0 = i0 % Nx;
	k0 = k0 % Nz;

	i1 = i1 % Nx;
	k1 = k1 % Nz;

	const int idx[4] = { field.index(i0,k0), field.index(i1,k0), field.index(i1,k1), field.index(i0,k1) };

	const float _fx = 1-fx;
	const float _fz = 1-fz;
	const float wgt[4] = { _fx*_fz, fx*_fz, fx*fz, _fx*fz };

	const float st = ZMax( scaleS, scaleT );

	const ZVector* _field = field.pointer();
	FOR(l,0,4) { vec += wgt[l] * (_field[idx[l]]/st); }

	return;
}

void
ScalarFieldLerpUV( const ZPoint& uv, const ZScalarField2D& field, float& f, float scaleS, float scaleT, float offsetS, float offsetT )
{
	const int Nx = field.nx();
	const int Nz = field.nz();

	float u = uv.x;
	float v = 1-uv.y;

	if( u < 0 ) { u += 1.f; }
	if( v < 0 ) { v += 1.f; }

//	float uu = u * Nx;
//	float vv = v * Nz;

	float uu = ( u * scaleS + offsetS ) * Nx;
	float vv = ( v * scaleT + offsetT ) * Nz;

	// avoid the negative value.
	while( uu < 0 ) { uu += 1.f; }
	while( vv < 0 ) { vv += 1.f; }

	int i0 = (int)floor(uu);
	int k0 = (int)floor(vv);

	int i1 = i0+1;
	int k1 = k0+1;

	float fx = uu-i0;
	float fz = vv-k0;

	i0 = i0 % Nx;
	k0 = k0 % Nz;

	i1 = i1 % Nx;
	k1 = k1 % Nz;

	const int idx[4] = { field.index(i0,k0), field.index(i1,k0), field.index(i1,k1), field.index(i0,k1) };

	const float _fx = 1-fx;
	const float _fz = 1-fz;
	const float wgt[4] = { _fx*_fz, fx*_fz, fx*fz, _fx*fz };

	const float st = ZMax( scaleS, scaleT );

	const float* _field = field.pointer();
	FOR(l,0,4) { f += wgt[l] * (_field[idx[l]]/st); }

	return;
}

void
VectorFieldCatromUV( const ZPoint& uv, const ZVectorField2D& field, ZVector& vec, float scaleS, float scaleT, float offsetS, float offsetT )
{
	const int Nx = field.nx();
	const int Nz = field.nz();

	float u = uv.x;
	float v = 1-uv.y;

	if( u < 0 ) { u += 1.f; }
	if( v < 0 ) { v += 1.f; }

//	float uu = u * Nx;
//	float vv = v * Nz;

	float uu = ( u * scaleS + offsetS ) * Nx;
	float vv = ( v * scaleT + offsetT ) * Nz;

	// avoid the negative value.
	while( uu < 0 ) { uu += 1.f; }
	while( vv < 0 ) { vv += 1.f; }

	int i1 = (int)floor(uu);
	int k1 = (int)floor(vv);

	int i2 = i1+1;
	int k2 = k1+1;

	float fx = uu-i1;
	float fz = vv-k1;

	i1 = i1 % Nx;
	k1 = k1 % Nz;

	i2 = i2 % Nx;
	k2 = k2 % Nz;

	int i0 = i1-1;
	int i3 = i2+1;
	i0 = (i0<  0) ? (i0+Nx) : i0;
	i3 = (i3>=Nx) ? (i3-Nx) : i3;

	int k0 = k1-1;
	int k3 = k2+1;
	k0 = (k0<  0) ? (k0+Nz) : k0;
	k3 = (k3>=Nz) ? (k3-Nz) : k3;

	const int idx[4][4] = { { field.index(i0,k0), field.index(i1,k0), field.index(i2,k0), field.index(i3,k0) },
							{ field.index(i0,k1), field.index(i1,k1), field.index(i2,k1), field.index(i3,k1) },
							{ field.index(i0,k2), field.index(i1,k2), field.index(i2,k2), field.index(i3,k2) },
							{ field.index(i0,k3), field.index(i1,k3), field.index(i2,k3), field.index(i3,k3) } };

	float t = fx;
	float tt = t*t;
	float ttt = tt*t;
	const float xw[4] = { 0.5f*(-t+2*tt-ttt), 0.5f*(2-5*tt+3*ttt), 0.5f*(t+4*tt-3*ttt), 0.5f*(-tt+ttt) };

	t = fz;
	tt = t*t;
	ttt = tt*t;
	const float zw[4] = { 0.5f*(-t+2*tt-ttt), 0.5f*(2-5*tt+3*ttt), 0.5f*(t+4*tt-3*ttt), 0.5f*(-tt+ttt) };

	const float st = ZMax( scaleS, scaleT );

	const ZVector* _field = field.pointer();
	FOR(k,0,4) { ZVector tmp(0,0,0); FOR(i,0,4) { tmp+=xw[i]*_field[idx[k][i]]; } vec += zw[k]*tmp/st; }

	return;
}

void
ScalarFieldCatromUV( const ZPoint& uv, const ZScalarField2D& field, float& f, float scaleS, float scaleT, float offsetS, float offsetT )
{
	const int Nx = field.nx();
	const int Nz = field.nz();

	float u = uv.x;
	float v = 1-uv.y;

	if( u < 0 ) { u += 1.f; }
	if( v < 0 ) { v += 1.f; }

//	float uu = u * Nx;
//	float vv = v * Nz;

	float uu = ( u * scaleS + offsetS ) * Nx;
	float vv = ( v * scaleT + offsetT ) * Nz;

	// avoid the negative value.
	while( uu < 0 ) { uu += 1.f; }
	while( vv < 0 ) { vv += 1.f; }

	int i1 = (int)floor(uu);
	int k1 = (int)floor(vv);

	int i2 = i1+1;
	int k2 = k1+1;

	float fx = uu-i1;
	float fz = vv-k1;

	i1 = i1 % Nx;
	k1 = k1 % Nz;

	i2 = i2 % Nx;
	k2 = k2 % Nz;

	int i0 = i1-1;
	int i3 = i2+1;
	i0 = (i0<  0) ? (i0+Nx) : i0;
	i3 = (i3>=Nx) ? (i3-Nx) : i3;

	int k0 = k1-1;
	int k3 = k2+1;
	k0 = (k0<  0) ? (k0+Nz) : k0;
	k3 = (k3>=Nz) ? (k3-Nz) : k3;

	const int idx[4][4] = { { field.index(i0,k0), field.index(i1,k0), field.index(i2,k0), field.index(i3,k0) },
							{ field.index(i0,k1), field.index(i1,k1), field.index(i2,k1), field.index(i3,k1) },
							{ field.index(i0,k2), field.index(i1,k2), field.index(i2,k2), field.index(i3,k2) },
							{ field.index(i0,k3), field.index(i1,k3), field.index(i2,k3), field.index(i3,k3) } };

	float t = fx;
	float tt = t*t;
	float ttt = tt*t;
	const float xw[4] = { 0.5f*(-t+2*tt-ttt), 0.5f*(2-5*tt+3*ttt), 0.5f*(t+4*tt-3*ttt), 0.5f*(-tt+ttt) };

	t = fz;
	tt = t*t;
	ttt = tt*t;
	const float zw[4] = { 0.5f*(-t+2*tt-ttt), 0.5f*(2-5*tt+3*ttt), 0.5f*(t+4*tt-3*ttt), 0.5f*(-tt+ttt) };

	const float st = ZMax( scaleS, scaleT );

	const float* _field = field.pointer();
	FOR(k,0,4) { t=0.f; FOR(i,0,4) { t+=xw[i]*_field[idx[k][i]]; } f += zw[k]*t/st; }

	return;
}

void
VectorFieldInterpolateUV( const ZPointArray& vUV, const ZVectorField2D& field, ZVectorArray& arr, float scaleS, float scaleT, float offsetS, float offsetT )
{
	const int numVerts = vUV.length();
	if( arr.length() != numVerts ) { arr.setLength( numVerts ); }

	#pragma omp parallel for
	FOR( i, 0, numVerts )
	{
		VectorFieldLerpUV( vUV[i], field, arr[i], scaleS, scaleT, offsetS, offsetT );
	}

	return;
}

void
ScalarFieldInterpolateUV( const ZPointArray& vUV, const ZScalarField2D& field, ZFloatArray& arr, float scaleS, float scaleT, float offsetS, float offsetT )
{
	const int numVerts = vUV.length();
	if( arr.length() != numVerts ) { arr.setLength( numVerts ); }

	#pragma omp parallel for
	FOR( i, 0, numVerts )
	{
		ScalarFieldLerpUV( vUV[i], field, arr[i], scaleS, scaleT, offsetS, offsetT );
	}

	return;
} 

ZELOS_NAMESPACE_END

