//------------------//
// ZMeshElement.cpp //
//-------------------------------------------------------//
// author: Taeyong Kim @ nVidia                          //
//         Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZMeshElement::ZMeshElement()
{
	id   = 0;
	type = ZMeshElementType::zNone;
}

ZMeshElement::ZMeshElement( const ZMeshElement& meshElement )
{
	this->operator=( meshElement );
}

ZMeshElement::ZMeshElement( ZMeshElementType::MeshElementType elementType, int numVertices )
{
	if( elementType == ZMeshElementType::zNone )
	{
		reset();
		return;
	}

	id   = 0;
	type = elementType;

	setCount( numVertices );
}

void
ZMeshElement::reset()
{
	id   = 0;
	type = ZMeshElementType::zNone;

	_vert.clear();
	_uv.clear();
}

void
ZMeshElement::setCount( int numVertices )
{
	if( count() != numVertices )
	{
		_vert.setLength ( numVertices );
		_uv.setLength   ( numVertices );
	}
}

ZMeshElement&
ZMeshElement::operator=( const ZMeshElement& other )
{
	id    = other.id;
	type  = other.type;
	_vert = other._vert;
	_uv   = other._uv;

	return (*this);
}

ostream&
operator<<( ostream& os, const ZMeshElement& object )
{
	os << "<ZMeshElement>" << endl;
	os << " ElementType  : " << ZMeshElementType::name(object.type) << endl;
	os << " Group ID     : " << object.id << endl;
	os << " # of vertices: " << object.count() << endl;
	FOR( i, 0, object.count() )
	{
		os << i << ": " << object[i] << " " << object(i) << endl;
	}
	return os;
}

ZELOS_NAMESPACE_END

