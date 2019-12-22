//-------------//
// ZGrid2D.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2018.03.08                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZGrid2D::ZGrid2D()
{
	reset();
}

ZGrid2D::ZGrid2D( const ZGrid2D& grid )
{
	*this = grid;
}

ZGrid2D::ZGrid2D( int Nx, int Nz, float Lx, float Lz )
{
	set( Nx,Nz, Lx,Lz );
}

ZGrid2D::ZGrid2D( int Nx, int Nz, const ZBoundingBox& bBox )
{
	set( Nx,Nz, bBox );
}

ZGrid2D::ZGrid2D( int subdivision, const ZBoundingBox& bBox )
{
	set( subdivision, bBox );
}

ZGrid2D::ZGrid2D( float h, int maxSubdivision, const ZBoundingBox& bBox )
{
	set( h, maxSubdivision, bBox );
}

void
ZGrid2D::reset()
{
	_nx    = _nz    = 0;
	_lx    = _lz    = 0.f;
	_dx    = _dz    = 0.f;
	_minPt.zeroize();
	_maxPt.zeroize();
	_y = 0.f;

	_nxp1  = _nzp1  = 0;
	_nxm1  = _nzm1  = 0;
	_dxd2  = _dzd2  = 0.f;
	_ddx   = _ddz   = 0.f;
}

void
ZGrid2D::set( int Nx, int Nz, float Lx, float Lz )
{
	reset();

	if( Nx<1 || Nz<1 || Lx<Z_EPS || Lz<Z_EPS )
	{
		cout << "Error@ZGrid2D::set(): Invalid grid parameters." << endl;
		return;
	}

	_nx = ZMax(Nx,1);   _lx = Lx;   _dx = _lx / float(_nx);
	_nz = ZMax(Nz,1);   _lz = Lz;   _dz = _lz / float(_nz);

	_minPt.set( 0.f, 0.f, 0.f );
	_maxPt.set( _lx, 0.f, _lz );

	_y = 0.f;

	_calcFreqAskedVariables();
}

void
ZGrid2D::set( int Nx, int Nz, const ZBoundingBox& bBox )
{
	const float Lx = bBox.width(0);
	const float Lz = bBox.width(2);

	set( Nx,Nz, Lx,Lz );

	_minPt = bBox.minPoint();
	_maxPt = bBox.maxPoint();
}

void
ZGrid2D::set( int subdivision, const ZBoundingBox& bBox )
{
	if( subdivision <= 0 )
	{
		cout << "Error@ZGrid2D::set(): Invalid subdivision parameter." << endl;
		return;
	}
	
	const float Lx = bBox.width(0);
	const float Lz = bBox.width(2);

	const float h = ZMax(Lx,Lz) / (float)subdivision;

	if( h < Z_EPS )
	{
		cout << "Error@ZGrid2D::set(): Invalid grid parameters." << endl;
		return;
	}

	const int Nx = ZMax( ZRound(int(Lx/h)), 1 );
	const int Nz = ZMax( ZRound(int(Lz/h)), 1 );

	set( Nx,Nz, Lx,Lz );

	_minPt = bBox.minPoint();
	_maxPt = bBox.maxPoint();
}

void
ZGrid2D::set( float h, int maxSubdivision, const ZBoundingBox& bBox )
{
	if( h<Z_EPS || maxSubdivision<1 )
	{
		cout << "Error@ZGrid2D::set(): Invalid grid parameters." << endl;
		return;
	}

	const float Lx = bBox.width(0);
	const float Lz = bBox.width(2);

	const int Nx = ZMax( ZRound(int(Lx/h)+0.5f), 1 );
	const int Nz = ZMax( ZRound(int(Lz/h)+0.5f), 1 );

	set( Nx,Nz, Lx,Lz );

	_minPt = bBox.minPoint();
	_maxPt = bBox.maxPoint();
}

void
ZGrid2D::_calcFreqAskedVariables()
{
	_nxp1 = _nx + 1;
	_nzp1 = _nz + 1;

	_nxm1 = _nx - 1;
	_nzm1 = _nz - 1;

	_dxd2 = 0.5f * _dx;
	_dzd2 = 0.5f * _dz;

	_ddx  = 1 / _dx;
	_ddz  = 1 / _dz;
}

ZGrid2D&
ZGrid2D::operator=( const ZGrid2D& g )
{
	_nx = g._nx;
	_nz = g._nz;

	_lx = g._lx;
	_lz = g._lz;

	_dx = g._dx;
	_dz = g._dz;

	_minPt = g._minPt;
	_maxPt = g._maxPt;

	_y = g._y;

	_calcFreqAskedVariables();

	return (*this);
}

bool
ZGrid2D::operator==( const ZGrid2D& g )
{
	if( _nx != g._nx ) { return false; }
	if( _nz != g._nz ) { return false; }

	if( _lx != g._lx ) { return false; }
	if( _lz != g._lz ) { return false; }

	if( _minPt != g._minPt ) { return false; }
	if( _maxPt != g._maxPt ) { return false; }

	if( _y != g._y ) { return false; }

	return true;
}

bool
ZGrid2D::operator!=( const ZGrid2D& g )
{
	if( _nx != g._nx ) { return true; }
	if( _nz != g._nz ) { return true; }

	if( _lx != g._lx ) { return true; }
	if( _lz != g._lz ) { return true; }

	if( _minPt != g._minPt ) { return true; }
	if( _maxPt != g._maxPt ) { return true; }

	if( _y != g._y ) { return true; }

	return false;
}

void
ZGrid2D::drawGrid() const
{
	if( _nx<=0 ) { return; }

	glBegin( GL_LINES );
	{
		for( int i=0; i<=_nx; ++i )
		{
			const float x = i * _dx + _minPt.x;

			glVertex3f( x, _y, _minPt.z );
			glVertex3f( x, _y, _maxPt.z );
		}

		for( int k=0; k<=_nz; ++k )
		{
			const float z = k * _dz + _minPt.z;

			glVertex3f( _minPt.x, _y, z );
			glVertex3f( _maxPt.x, _y, z );
		}
	}
	glEnd();
}

void
ZGrid2D::write( ofstream& fout ) const
{
	const ZInt2   n( _nx, _nz ); n.write( fout );
	const ZFloat2 l( _lx, _lz ); l.write( fout );
	const ZFloat2 d( _dx, _dz ); d.write( fout );

	_minPt.write( fout );
	_maxPt.write( fout );

	fout.write( (char*)&_y, sizeof(float) );
}

void
ZGrid2D::read( ifstream& fin )
{
	ZInt2   n; n.read( fin ); _nx=n[0]; _nz=n[1];
	ZFloat2 l; l.read( fin ); _lx=l[0]; _lz=l[1];
	ZFloat2 d; d.read( fin ); _dx=d[0]; _dz=d[1];

	_minPt.read( fin );
	_maxPt.read( fin );

	fin.read( (char*)&_y, sizeof(float) );

	_calcFreqAskedVariables();
}

ostream&
operator<<( ostream& os, const ZGrid2D& object )
{
	os << "<ZGrid2D>" << endl;
	os << " resolution: " << object.nx() << " x " << object.nz() << endl;
	os << " dimension : " << object.lx() << " x " << object.lz() << endl;
	os << " cell size : " << object.dx() << " x " << object.dz() << endl;
	os << " domain    : " << object.minPoint() << " ~ " << object.maxPoint() << endl;
	os << " y         : " << object.getY() << endl;
	os << endl;
	return os;
}

ZELOS_NAMESPACE_END

