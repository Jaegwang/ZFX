//-------------------//
// ZAlembicUtils.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2017.05.25                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

bool
GetMetaData( const Alembic::Abc::MetaData& md, ZStringArray& keys, ZStringArray& values )
{
	keys.clear();
	values.clear();

	const int count = (int)md.size();
	if( !count ) { return false; }

	keys.setLength( count );
	values.setLength( count );

	Alembic::Abc::MetaData::const_iterator itr = md.begin();
	for( int i=0; itr!=md.end(); ++itr, ++i )
	{
		keys[i]   = itr->first;
		values[i] = itr->second;
	}

	return true;
}

void
AddChildToList( const ZAlembicObject& parentObject, ZAlembicObjectArray& list, bool leavesOnly )
{
	const int nChildren = parentObject.numChildren();

	FOR( i, 0, nChildren )
	{
		ZAlembicObject* childObjPtr = new ZAlembicObject();
		parentObject.getChild( i, *childObjPtr );

		if( leavesOnly ) {

			if( childObjPtr->isLeaf() )
			{
				list.append( childObjPtr );
			}

		} else {

			list.append( childObjPtr );

		}

		AddChildToList( *childObjPtr, list, leavesOnly );

	}
}

void
GetAllDescendantObjects( const ZAlembicObject& progenitorObject, ZAlembicObjectArray& list, bool leavesOnly )
{
	list.reset();

	AddChildToList( progenitorObject, list, leavesOnly );
}

void
AddChildToList( const ZAlembicProperty& parentProperty, ZAlembicPropertyArray& list )
{
	const int nChildren = parentProperty.numChildren();

	FOR( i, 0, nChildren )
	{
		ZAlembicProperty* childPrpPtr = new ZAlembicProperty();
		parentProperty.getChild( i, *childPrpPtr );

		list.append( childPrpPtr );

		if( childPrpPtr->typeId() == 1 ) // if it is a compound property
		{
			AddChildToList( *childPrpPtr, list );
		}
	}
}

void
GetAllDescendantProperties( const ZAlembicProperty& progenitorProperty, ZAlembicPropertyArray& list )
{
	list.reset();

	AddChildToList( progenitorProperty, list );
}

void
AddParentToList( const ZAlembicObject& childObject, ZAlembicObjectArray& list )
{
	ZAlembicObject* parentObjPtr = new ZAlembicObject();
	ZAlembicObject& obj = *parentObjPtr;

	if( childObject.getParent( obj ) ) {

		list.append( parentObjPtr );

		AddParentToList( obj, list );

	} else {

		delete parentObjPtr;

	}
}

void
GetAllParentObjects( const ZAlembicObject& childObject, ZAlembicObjectArray& list )
{
	list.reset();

	AddParentToList( childObject, list );
}

void
GetWorldMatrix( const ZAlembicObject& shapeObject, ZMatrix& worldMatrix, int frame )
{
	worldMatrix.setToIdentity();

	if( !shapeObject.isShape() ) { return; }

	ZAlembicObjectArray list;
	GetAllParentObjects( shapeObject, list );

	list.reverse();

	const int count = list.length();

	ZVector t, r, s;

	FOR( i, 0, count )
	{
		list[i].getTransformations( t, r, s, frame );

		ZMatrix xform;
		xform.setTranslation( t );
		xform.setEulerRotation( r, ZRotationOrder::zZYX, false );
		xform.addScale( s.x, s.y, s.z );

		worldMatrix *= xform;
	}
}


void
ZDrawPolyMesh( const ZPointArray& vPos, const ZIntArray& vCount, const ZIntArray& vConnects )
{
	const int numVertices = vPos.length();
	const int numPolygons = vCount.length();

	int idx = 0;

	FOR( i, 0, numPolygons )
	{
		const int& N = vCount[i];

		glBegin( GL_POLYGON );
		{
			FOR( j, 0, N )
			{
				glVertex( vPos[ vConnects[idx] ] );
				++idx;
			}
		}
		glEnd();
	}
}

ZELOS_NAMESPACE_END

