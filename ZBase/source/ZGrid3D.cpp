//-------------//
// ZGrid3D.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2018.03.08                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZGrid3D::ZGrid3D()
{
	reset();
}

ZGrid3D::ZGrid3D( const ZGrid3D& grid )
{
	*this = grid;
}

ZGrid3D::ZGrid3D( int Nx, int Ny, int Nz, float Lx, float Ly, float Lz )
{
	set( Nx,Ny,Nz, Lx,Ly,Lz );
}

ZGrid3D::ZGrid3D( int Nx, int Ny, int Nz, const ZBoundingBox& bBox )
{
	set( Nx,Ny,Nz, bBox );
}

ZGrid3D::ZGrid3D( int subdivision, const ZBoundingBox& bBox )
{
	set( subdivision, bBox );
}

ZGrid3D::ZGrid3D( float h, int maxSubdivision, const ZBoundingBox& bBox )
{
	set( h, maxSubdivision, bBox );
}

void
ZGrid3D::reset()
{
	_nx    = _ny    = _nz    = 0;
	_lx    = _ly    = _lz    = 0.f;
	_dx    = _dy    = _dz    = 0.f;
	_minPt.zeroize();
	_maxPt.zeroize();

	_nxp1  = _nyp1  = _nzp1  = 0;
	_nxm1  = _nym1  = _nzm1  = 0;
	_nxny  = 0;
	_nxp1nyp1 = 0;
	_dxd2  = _dyd2  = _dzd2  = 0.f;
	_ddx   = _ddy   = _ddz   = 0.f;
}

void
ZGrid3D::set( int Nx, int Ny, int Nz, float Lx, float Ly, float Lz )
{
	reset();

	if( Nx<1 || Ny<1 || Nz<1 || Lx<Z_EPS || Ly<Z_EPS || Lz<Z_EPS )
	{
		cout << "Error@ZGrid3D::set(): Invalid grid parameters." << endl;
		return;
	}

	_nx= ZMax(Nx,1);   _lx=Lx;   _dx=_lx/float(_nx);
	_ny= ZMax(Ny,1);   _ly=Ly;   _dy=_ly/float(_ny);
	_nz= ZMax(Nz,1);   _lz=Lz;   _dz=_lz/float(_nz);

	_minPt.set( 0.f, 0.f, 0.f );
	_maxPt.set( _lx, _ly, _lz );

	_calcFreqAskedVariables();
}

void
ZGrid3D::set( int Nx, int Ny, int Nz, const ZBoundingBox& bBox )
{
	const float Lx = bBox.width(0);
	const float Ly = bBox.width(1);
	const float Lz = bBox.width(2);

	set( Nx,Ny,Nz, Lx,Ly,Lz );

	_minPt = bBox.minPoint();
	_maxPt = bBox.maxPoint();
}

void
ZGrid3D::set( int subdivision, const ZBoundingBox& bBox )
{
	if( subdivision <= 0 )
	{
		cout << "Error@ZGrid3D::set(): Invalid subdivision parameter." << endl;
		return;
	}

	const float Lx = bBox.width(0);
	const float Ly = bBox.width(1);
	const float Lz = bBox.width(2);

	const float h = ZMax(Lx,Ly,Lz) / (float)subdivision;

	if( h < Z_EPS )
	{
		cout << "Error@ZGrid3D::set(): Invalid grid parameters." << endl;
		return;
	}

	const int Nx = ZMax( ZRound(int(Lx/h)+0.5f), 1 );
	const int Ny = ZMax( ZRound(int(Ly/h)+0.5f), 1 );
	const int Nz = ZMax( ZRound(int(Lz/h)+0.5f), 1 );

	set( Nx,Ny,Nz, Lx,Ly,Lz );

	_minPt = bBox.minPoint();
	_maxPt = bBox.maxPoint();
}

void
ZGrid3D::set( float h, int maxSubdivision, const ZBoundingBox& bBox )
{
	if( h<Z_EPS || maxSubdivision<1 )
	{
		cout << "Error@ZGrid3D::set(): Invalid grid parameters." << endl;
		return;
	}

	const float Lx = bBox.width(0);
	const float Ly = bBox.width(1);
	const float Lz = bBox.width(2);

	const int Nx = ZMax( ZRound(int(Lx/h)+0.5f), 1 );
	const int Ny = ZMax( ZRound(int(Ly/h)+0.5f), 1 );
	const int Nz = ZMax( ZRound(int(Lz/h)+0.5f), 1 );

	set( Nx,Ny,Nz, Lx,Ly,Lz );

	_minPt = bBox.minPoint();
	_maxPt = bBox.maxPoint();
}

void
ZGrid3D::_calcFreqAskedVariables()
{
	_nxp1 = _nx + 1;
	_nyp1 = _ny + 1;
	_nzp1 = _nz + 1;

	_nxm1 = _nx - 1;
	_nym1 = _ny - 1;
	_nzm1 = _nz - 1;

	_nxny = _nx * _ny;
	_nxp1nyp1 = _nxp1 * _nyp1;

	_dxd2 = 0.5f * _dx;
	_dyd2 = 0.5f * _dy;
	_dzd2 = 0.5f * _dz;

	_ddx  = 1 / _dx;
	_ddy  = 1 / _dy;
	_ddz  = 1 / _dz;
}

ZGrid3D&
ZGrid3D::operator=( const ZGrid3D& g )
{
	_nx = g._nx;
	_ny = g._ny;
	_nz = g._nz;

	_lx = g._lx;
	_ly = g._ly;
	_lz = g._lz;

	_dx = g._dx;
	_dy = g._dy;
	_dz = g._dz;

	_minPt = g._minPt;
	_maxPt = g._maxPt;

	_calcFreqAskedVariables();

	return (*this);
}

bool
ZGrid3D::operator==( const ZGrid3D& g )
{
	if( _nx != g._nx ) { return false; }
	if( _ny != g._ny ) { return false; }
	if( _nz != g._nz ) { return false; }

	if( _lx != g._lx ) { return false; }
	if( _ly != g._ly ) { return false; }
	if( _lz != g._lz ) { return false; }

	if( _minPt != g._minPt ) { return false; }
	if( _maxPt != g._maxPt ) { return false; }

	return true;
}

bool
ZGrid3D::operator!=( const ZGrid3D& g )
{
	if( _nx != g._nx ) { return true; }
	if( _ny != g._ny ) { return true; }
	if( _nz != g._nz ) { return true; }

	if( _lx != g._lx ) { return true; }
	if( _ly != g._ly ) { return true; }
	if( _lz != g._lz ) { return true; }

	if( _minPt != g._minPt ) { return true; }
	if( _maxPt != g._maxPt ) { return true; }

	return false;
}

void
ZGrid3D::drawGrid( bool x0, bool x1, bool y0, bool y1, bool z0, bool z1 ) const
{
	if( x0 ) { _drawXSlice( 0 ); }	
	if( x1 ) { _drawXSlice(_nx); }	
	if( y0 ) { _drawYSlice( 0 ); }	
	if( y1 ) { _drawYSlice(_ny); }	
	if( z0 ) { _drawZSlice( 0 ); }	
	if( z1 ) { _drawZSlice(_nz); }	

	ZDrawCube( _minPt, _maxPt );
}

void
ZGrid3D::_drawXSlice( int i ) const
{
	if( _nx<=0 || i<0 || i>_nx ) { return; }
	const float x = i*_dx+_minPt.x;
	glBegin( GL_LINES );
		for( int j=0; j<=_ny; ++j ) { const float y=j*_dy+_minPt.y; glVertex(x,y,_minPt.z); glVertex(x,y,_maxPt.z); }
		for( int k=0; k<=_nz; ++k ) { const float z=k*_dz+_minPt.z; glVertex(x,_minPt.y,z); glVertex(x,_maxPt.y,z); }
	glEnd();
}

void
ZGrid3D::_drawYSlice( int j ) const
{
	if( _ny<=0 || j<0 || j>_ny ) { return; }
	const float y = j*_dy+_minPt.y;
	glBegin( GL_LINES );
		for( int k=0; k<=_nz; ++k ) { const float z=k*_dz+_minPt.z; glVertex(_minPt.x,y,z); glVertex(_maxPt.x,y,z); }
		for( int i=0; i<=_nx; ++i ) { const float x=i*_dx+_minPt.x; glVertex(x,y,_minPt.z); glVertex(x,y,_maxPt.z); }
	glEnd();
}

void
ZGrid3D::_drawZSlice( int k ) const
{
	if( _nz<=0 || k<0 || k>_nz ) { return; }
	const float z=k*_dz+_minPt.z;
	glBegin( GL_LINES );
		for( int i=0; i<=_nx; ++i ) { const float x=i*_dx+_minPt.x; glVertex(x,_minPt.y,z); glVertex(x,_maxPt.y,z); }
		for( int j=0; j<=_ny; ++j ) { const float y=j*_dy+_minPt.y; glVertex(_minPt.x,y,z); glVertex(_maxPt.x,y,z); }
	glEnd();
}

void
ZGrid3D::write( ofstream& fout ) const
{
	const ZInt3   n( _nx, _ny, _nz ); n.write( fout );
	const ZFloat3 l( _lx, _ly, _lz ); l.write( fout );
	const ZFloat3 d( _dx, _dy, _dz ); d.write( fout );

	_minPt.write( fout );
	_maxPt.write( fout );
}

void
ZGrid3D::read( ifstream& fin )
{
	ZInt3   n; n.read( fin ); _nx=n[0]; _ny=n[1]; _nz=n[2];
	ZFloat3 l; l.read( fin ); _lx=l[0]; _ly=l[1]; _lz=l[2];
	ZFloat3 d; d.read( fin ); _dx=d[0]; _dy=d[1]; _dz=d[2];

	_minPt.read( fin );
	_maxPt.read( fin );

	_calcFreqAskedVariables();
}

ostream&
operator<<( ostream& os, const ZGrid3D& grid )
{
	os << "<ZGrid3D>" << endl;
	os << " resolution: " << grid.nx() << " x " << grid.ny() << " x " << grid.nz() << endl;
	os << " dimension : " << grid.lx() << " x " << grid.ly() << " x " << grid.lz() << endl;
	os << " cell size : " << grid.dx() << " x " << grid.dy() << " x " << grid.dz() << endl;
	os << " domain    : " << grid.minPoint() << " ~ " << grid.maxPoint() << endl;
	os << endl;
	return os;
}

ZELOS_NAMESPACE_END

