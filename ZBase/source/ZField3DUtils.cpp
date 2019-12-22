//-------------------//
// ZField3DUtils.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

bool
Gradient( ZVectorField3D& v, const ZScalarField3D& s, bool useOpenMP )
{
	if( !v.directComputable(s) )
	{
		cout << "Error@Gradient(): Not direct computable." << endl;
		return false;
	}

	const int nElems = v.numElements();

	const int iMax = v.iMax();
	const int jMax = v.jMax();
	const int kMax = v.kMax();

	const float _dx = 1/v.dx();
	const float _dy = 1/v.dy();
	const float _dz = 1/v.dz();

	const float _dx2 = 0.5f*_dx;
	const float _dy2 = 0.5f*_dy;
	const float _dz2 = 0.5f*_dz;

	#pragma omp parallel for if( useOpenMP && nElems>10000 )
	PER_EACH_ELEMENT_3D( v )

		const int idx = v.index(i,j,k);

		float _Dx=0;
		int i0=0, i1=0;
		if( iMax > 0 )
		{
			if( i==0 )         { _Dx=_dx;  i0 = idx;       i1 = s.i1(idx); }
			else if( i==iMax ) { _Dx=_dx;  i0 = s.i0(idx); i1 = idx;       }
			else               { _Dx=_dx2; i0 = s.i0(idx); i1 = s.i1(idx); }
		}

		float _Dy=0;
		int j0=0, j1=0;
		if( jMax > 0 )
		{
			if( j==0 )         { _Dy=_dy;  j0 = idx;       j1 = s.j1(idx); }
			else if( j==jMax ) { _Dy=_dy;  j0 = s.j0(idx); j1 = idx;       }
			else               { _Dy=_dy2; j0 = s.j0(idx); j1 = s.j1(idx); }
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
			vv.y = ( s[j1] - s[j0] ) * _Dy;
			vv.z = ( s[k1] - s[k0] ) * _Dz;
		}


	END_PER_EACH_3D

	return true;
}

bool
Divergence( ZScalarField3D& s, const ZVectorField3D& v, bool useOpenMP )
{
	if( !s.directComputable(v) )
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

	const float dxdy = s.dx() * s.dy();
	const float dydz = s.dy() * s.dz();
	const float dzdx = s.dz() * s.dx();

	#pragma omp parallel for if( useOpenMP && nElems>10000 )
	PER_EACH_ELEMENT_3D( s )

		ZInt8 n;
		s.getNodesOfCell( i,j,k, n );

		float& dvg = s(i,j,k);

		dvg += dydz * ( ( v[n[1]].x + v[n[2]].x + v[n[5]].x + v[n[6]].x )
                      - ( v[n[0]].x + v[n[3]].x + v[n[4]].x + v[n[7]].x ) );

		dvg += dzdx * ( ( v[n[4]].y + v[n[5]].y + v[n[6]].y + v[n[7]].y )
                      - ( v[n[0]].y + v[n[1]].y + v[n[2]].y + v[n[3]].y ) );

		dvg += dxdy * ( ( v[n[2]].z + v[n[3]].z + v[n[6]].z + v[n[7]].z )
                      - ( v[n[0]].z + v[n[1]].z + v[n[4]].z + v[n[5]].z ) );

	END_PER_EACH_3D

	return true;
}

ZELOS_NAMESPACE_END

