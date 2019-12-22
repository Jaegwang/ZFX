//----------------//
// ZAxisArray.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZAxisArray::ZAxisArray()
{}

ZAxisArray::ZAxisArray( const ZAxisArray& a )
: ZArray<ZAxis>::ZArray( a )
{}

ZAxisArray::ZAxisArray( int l )
: ZArray<ZAxis>::ZArray( l )
{}

void
ZAxisArray::changeHandedness( int i )
{
	const int num = parent::length();

	FOR( i, 0, num )
	{
		parent::at(i).changeHandedness( i );
	}
}

void
ZAxisArray::offset( int whichAxis, float e )
{
	const int num = parent::length();

	switch( whichAxis )
	{
		default:
		case 0:
		{
			FOR( i, 0, num )
			{
				ZAxis& axis = parent::at(i);
				axis.origin += axis.xAxis * e;
			}
			break;
		}

		case 1:
		{
			FOR( i, 0, num )
			{
				ZAxis& axis = parent::at(i);
				axis.origin += axis.yAxis * e;
			}
			break;
		}

		case 2:
		{
			FOR( i, 0, num )
			{
				ZAxis& axis = parent::at(i);
				axis.origin += axis.zAxis * e;
			}
			break;
		}
	}
}

void
ZAxisArray::drawOrigins() const
{
	const int num = parent::length();

	glBegin( GL_POINTS );
		FOR( i, 0, num )
		{
			const ZAxis& axis = parent::at(i);
			glVertex( axis.origin );
		}
	glEnd();
}

void
ZAxisArray::draw( float scale, bool bySimpleLine ) const
{
	const int num = parent::length();

	FOR( i, 0, num )
	{
		const ZAxis& axis = parent::at(i);

		glPushMatrix();
		{
			glTranslate(  axis.origin );
			glScale( scale );
			glTranslate( -axis.origin );

			axis.draw( bySimpleLine );
		}
		glPopMatrix();
	}
}

ostream&
operator<<( ostream& os, const ZAxisArray& object )
{
	os << "<ZAxisArray>" << endl;
	const int size     = object.size();
	const int capacity = object.capacity();

	double bytes = size * sizeof(ZAxis);
	os << " size    : " << size << " (";
	     if( bytes <       1024.0  ) { os << ( bytes                 ) << " bytes)" << endl; }
	else if( bytes < ZPow2(1024.0) ) { os << ( bytes /       1024.0  ) << " kb)"    << endl; }
	else if( bytes < ZPow3(1024.0) ) { os << ( bytes / ZPow2(1024.0) ) << " mb)"    << endl; }
	else if( bytes < ZPow4(1024.0) ) { os << ( bytes / ZPow3(1024.0) ) << " gb)"    << endl; }

	bytes = capacity * sizeof(ZAxis);
	os << " capacity: " << size << " (";
	     if( bytes <       1024.0  ) { os << ( bytes                 ) << " bytes)" << endl; }
	else if( bytes < ZPow2(1024.0) ) { os << ( bytes /       1024.0  ) << " kb)"    << endl; }
	else if( bytes < ZPow3(1024.0) ) { os << ( bytes / ZPow2(1024.0) ) << " mb)"    << endl; }
	else if( bytes < ZPow4(1024.0) ) { os << ( bytes / ZPow3(1024.0) ) << " gb)"    << endl; }

	os << endl;
	return os;
}

ZELOS_NAMESPACE_END

