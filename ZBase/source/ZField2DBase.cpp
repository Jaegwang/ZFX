//------------------//
// ZField2DBase.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZField2DBase::ZField2DBase()
: ZGrid2D()
{
	_numElements = _iMax = _kMax = _stride = 0;
	_location = ZFieldLocation::zNone;
}

ZField2DBase::ZField2DBase( const ZField2DBase& f )
: ZGrid2D(f)
{
	_numElements = f._numElements;
	_iMax        = f._iMax;
	_kMax        = f._kMax;
	_stride      = f._stride;
	_location    = f._location;
}

ZField2DBase::ZField2DBase( const ZGrid2D& grid, ZFieldLocation::FieldLocation loc )
: ZGrid2D(grid)
{
	set( grid, loc );
}

ZField2DBase::ZField2DBase( int Nx, int Nz, ZFieldLocation::FieldLocation loc )
{
	set( Nx, Nz, loc );
}

ZField2DBase::ZField2DBase( int Nx, int Nz, float Lx, float Lz, ZFieldLocation::FieldLocation loc )
{
	set( Nx, Nz, Lx, Lz, loc );
}

void
ZField2DBase::set( const ZGrid2D& grid, ZFieldLocation::FieldLocation loc )
{
	ZGrid2D::operator=( grid );

	switch( loc )
	{
		case ZFieldLocation::zCell:
		{
			_numElements = ZGrid2D::numCells();
			_iMax        = _nx-1;
			_kMax        = _nz-1;
			_stride      = _nx;

			break;
		}

		case ZFieldLocation::zNode:
		{
			_numElements = ZGrid2D::numNodes();
			_iMax        = _nx;
			_kMax        = _nz;
			_stride      = _nx+1;

			break;
		}

		default:
		{
			ZField2DBase::reset();
			return;
		}
	}

	_location = loc;
}

void
ZField2DBase::set( int Nx, int Nz, ZFieldLocation::FieldLocation loc )
{
	ZGrid2D grid( Nx, Nz );
	ZField2DBase::set( grid, loc );
}

void
ZField2DBase::set( int Nx, int Nz, float Lx, float Lz, ZFieldLocation::FieldLocation loc )
{
	ZGrid2D grid( Nx, Nz, Lx, Lz );
	ZField2DBase::set( grid, loc );
}

void
ZField2DBase::reset()
{
	ZGrid2D::reset();
	_numElements = _iMax = _kMax = _stride = 0;
	_location = ZFieldLocation::zNone;
}

ZField2DBase&
ZField2DBase::operator=( const ZField2DBase& f )
{
	ZGrid2D::operator=( f );

	_numElements = f._numElements;
	_iMax        = f._iMax;
	_kMax        = f._kMax;
	_stride      = f._stride;

	return (*this);
}

bool
ZField2DBase::operator==( const ZField2DBase& f )
{
	if( ZGrid2D::operator!=(f) ) { return false; }
	if( _location != f._location ) { return false; }
	return true;
}

bool
ZField2DBase::operator!=( const ZField2DBase& f )
{
	if( ZGrid2D::operator!=(f) ) { return true; }
	if( _location != f._location ) { return true; }
	return false;
}

bool
ZField2DBase::directComputable( const ZField2DBase& f )
{
	if( ZGrid2D::operator!=(f) ) { return false; }
	if( _location != f._location ) { return false; }
	return true;
}

bool
ZField2DBase::directComputableWithoutLocation( const ZField2DBase& f )
{
	if( ZGrid2D::operator!=(f) ) { return false; }
	return true;
}

void
ZField2DBase::write( ofstream& fout ) const
{
	ZGrid2D::write( fout );
	fout.write( (char*)&_location, sizeof(int) );
}

void
ZField2DBase::read( ifstream& fin )
{
	ZGrid2D grid;
	grid.read( fin );

	ZFieldLocation::FieldLocation loc;
	fin.read( (char*)&loc, sizeof(int) );

	ZField2DBase::set( grid, loc );
}

ostream& operator<<( ostream& os, const ZField2DBase& object )
{
	os << "<ZField2DBase>" << endl;
	os << " resolution: " << object.nx() << " x " << object.nz() << endl;
	os << " dimension : " << object.lx() << " x " << object.lz() << endl;
	os << " cell size : " << object.dx() << " x " << object.dz() << endl;
	os << " domain    : " << object.minPoint() << " ~ " << object.maxPoint() << endl;
	os << " location  : " << ZFieldLocation::name(object.location()) << endl;
	os << " y         : " << object.getY() << endl;
	os << endl;
	return os;
}

ZELOS_NAMESPACE_END

