//------------------//
// ZField3DBase.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZField3DBase::ZField3DBase()
: ZGrid3D()
{
	_numElements = _iMax = _jMax = _kMax = _stride0 = _stride1 = 0;
	_location = ZFieldLocation::zNone;
}

ZField3DBase::ZField3DBase( const ZField3DBase& f )
: ZGrid3D(f)
{
	_numElements = f._numElements;
	_iMax        = f._iMax;
	_jMax        = f._jMax;
	_kMax        = f._kMax;
	_stride0     = f._stride0;
	_stride1     = f._stride1;
	_location    = f._location;
}

ZField3DBase::ZField3DBase( const ZGrid3D& grid, ZFieldLocation::FieldLocation loc )
: ZGrid3D(grid)
{
	set( grid, loc );
}

ZField3DBase::ZField3DBase( int Nx, int Ny, int Nz, ZFieldLocation::FieldLocation loc )
{
	set( Nx, Ny, Nz, loc );
}

ZField3DBase::ZField3DBase( int Nx, int Ny, int Nz, float Lx, float Ly, float Lz, ZFieldLocation::FieldLocation loc )
{
	set( Nx, Ny, Nz, Lx, Ly, Lz, loc );
}

void
ZField3DBase::set( const ZGrid3D& grid, ZFieldLocation::FieldLocation loc )
{
	ZGrid3D::operator=( grid );

	switch( loc )
	{
		case ZFieldLocation::zCell:
		{
			_numElements = ZGrid3D::numCells();
			_iMax        = _nx-1;
			_jMax        = _ny-1;
			_kMax        = _nz-1;
			_stride0     = _nx;
			_stride1     = _nx*_ny;

			break;
		}

		case ZFieldLocation::zNode:
		{
			_numElements = ZGrid3D::numNodes();
			_iMax        = _nx;
			_jMax        = _ny;
			_kMax        = _nz;
			_stride0     = _nx+1;
			_stride1     = (_nx+1)*(_ny+1);

			break;
		}

		default:
		{
			ZField3DBase::reset();
			return;
		}
	}

	_location = loc;
}

void
ZField3DBase::set( int Nx, int Ny, int Nz, ZFieldLocation::FieldLocation loc )
{
	ZGrid3D grid( Nx, Ny, Nz );
	ZField3DBase::set( grid, loc );
}

void
ZField3DBase::set( int Nx, int Ny, int Nz, float Lx, float Ly, float Lz, ZFieldLocation::FieldLocation loc )
{
	ZGrid3D grid( Nx, Ny, Nz, Lx, Ly, Lz );
	ZField3DBase::set( grid, loc );
}

void
ZField3DBase::reset()
{
	ZGrid3D::reset();
	_numElements = _iMax = _jMax = _kMax = _stride0 = _stride1 = 0;
	_location = ZFieldLocation::zNone;
}

ZField3DBase&
ZField3DBase::operator=( const ZField3DBase& f )
{
	ZGrid3D::operator=( f );

	_numElements = f._numElements;
	_iMax        = f._iMax;
	_jMax        = f._jMax;
	_kMax        = f._kMax;
	_stride0     = f._stride0;
	_stride1     = f._stride1;
	_location	 = f._location;

	return (*this);
}

bool
ZField3DBase::operator==( const ZField3DBase& f )
{
	if( ZGrid3D::operator!=(f) ) { return false; }
	if( _location != f._location ) { return false; }
	return true;
}

bool
ZField3DBase::operator!=( const ZField3DBase& f )
{
	if( ZGrid3D::operator!=(f) ) { return true; }
	if( _location != f._location ) { return true; }
	return false;
}

bool
ZField3DBase::directComputable( const ZField3DBase& f )
{
	if( ZGrid3D::operator!=(f) ) { return false; }
	if( _location != f._location ) { return false; }
	return true;
}

bool
ZField3DBase::directComputableWithoutLocation( const ZField3DBase& f )
{
	if( ZGrid3D::operator!=(f) ) { return false; }
	return true;
}

void
ZField3DBase::write( ofstream& fout ) const
{
	ZGrid3D::write( fout );
	fout.write( (char*)&_location, sizeof(int) );
}

void
ZField3DBase::read( ifstream& fin )
{
	ZGrid3D grid;
	grid.read( fin );

	ZFieldLocation::FieldLocation loc;
	fin.read( (char*)&loc, sizeof(int) );

	ZField3DBase::set( grid, loc );
}

ostream& operator<<( ostream& os, const ZField3DBase& object )
{
	os << "<ZField3DBase>" << endl;
	os << " resolution: " << object.nx() << " x " << object.ny() << " x " << object.nz() << endl;
	os << " dimension : " << object.lx() << " x " << object.ly() << " x " << object.lz() << endl;
	os << " cell size : " << object.dx() << " x " << object.dy() << " x " << object.dz() << endl;
	os << " domain    : " << object.minPoint() << " ~ " << object.maxPoint() << endl;
	os << " location  : " << ZFieldLocation::name(object.location()) << endl;
	os << endl;
	return os;
}

ZELOS_NAMESPACE_END

