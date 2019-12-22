//--------------------//
// ZAlembicObject.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2018.03.08                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZAlembicObject::ZAlembicObject()
{
	ZAlembicObject::reset();
}

ZAlembicObject::ZAlembicObject( const ZAlembicObject& obj )
{
	this->operator=( obj );
}

void
ZAlembicObject::reset()
{
	_header           = Alembic::Abc::ObjectHeader();
	_object           = Alembic::Abc::IObject();
	_name             = "";
	_fullPath         = "";
	_typeId           = 0;
	_numChildren      = 0;
	_topPrp           = ZAlembicProperty();

	_timeSamplingType = 0;
	_numTimeSamples   = 0;
	_minTime          = +Z_LARGE;
	_maxTime          = -Z_LARGE;
	_timeStepSize     = 0.0;
	_minFrame         = 0.0;
	_maxFrame         = 0.0;
}

void
ZAlembicObject::set( const Alembic::Abc::IObject& obj )
{
	_header   = obj.getHeader();
	_object   = obj;

	_name     = obj.getName();
	_fullPath = obj.getFullName();

	_typeId   = 0;
	{
		     if( Alembic::AbcGeom::IXform    :: matches ( _header ) ) { _typeId = 1; }
		else if( Alembic::AbcGeom::ISubD     :: matches ( _header ) ) { _typeId = 2; }
		else if( Alembic::AbcGeom::IPolyMesh :: matches ( _header ) ) { _typeId = 3; }
		else if( Alembic::AbcGeom::ICamera   :: matches ( _header ) ) { _typeId = 4; }
		else if( Alembic::AbcGeom::ICurves   :: matches ( _header ) ) { _typeId = 5; }
		else if( Alembic::AbcGeom::INuPatch  :: matches ( _header ) ) { _typeId = 6; }
		else if( Alembic::AbcGeom::IPoints   :: matches ( _header ) ) { _typeId = 7; }
		else if( Alembic::AbcGeom::ILight    :: matches ( _header ) ) { _typeId = 8; }
	}

	_numChildren = obj.getNumChildren();

	_topPrp.setAsTop( *this );

	// Caution)
	// _minTime, _maxTime, _timeStepSize, _minFrame, _maxFrame are valid only for UNIFORM time sampling.

	#define GET_TIME_INFO( schema )                               \
		Alembic::Abc::TimeSamplingPtr  tsPtr  = schema.getTimeSampling();       \
		Alembic::Abc::TimeSamplingType tsType = tsPtr->getTimeSamplingType();   \
		     if( schema.isConstant() ) { _timeSamplingType = 0; } \
		else if( tsType.isUniform()  ) { _timeSamplingType = 1; } \
		else if( tsType.isCyclic()   ) { _timeSamplingType = 2; } \
		else if( tsType.isAcyclic()  ) { _timeSamplingType = 3; } \
		_numTimeSamples = (int)schema.getNumSamples();            \
		if( _timeSamplingType > 0 ) {                             \
			_minTime = tsPtr->getSampleTime(0);                   \
			_maxTime = tsPtr->getSampleTime(_numTimeSamples-1);   \
			_timeStepSize = (double)tsType.getTimePerCycle();     \
			_minFrame = _minTime / _timeStepSize;                 \
			_maxFrame = _maxTime / _timeStepSize;                 \
		} else {                                                  \
			_minTime = _maxTime = 0.0;                            \
			_minFrame = _maxFrame = 0.0;                          \
		}                                                         \

	switch( _typeId )
	{
		case 0: // none
		{
			break;
		}

		case 1: // Xform
		{
			Alembic::AbcGeom::IXform xform( _object, Alembic::Abc::kWrapExisting );
			Alembic::AbcGeom::IXformSchema schema = xform.getSchema();
			GET_TIME_INFO( schema );

			break;
		}

		case 2: // SubD
		{
			Alembic::AbcGeom::ISubD xform( _object, Alembic::Abc::kWrapExisting );
			Alembic::AbcGeom::ISubDSchema schema = xform.getSchema();
			GET_TIME_INFO( schema );
			break;
		}

		case 3: // PolyMesh
		{
			Alembic::AbcGeom::IPolyMesh xform( _object, Alembic::Abc::kWrapExisting );
			Alembic::AbcGeom::IPolyMeshSchema schema = xform.getSchema();
			GET_TIME_INFO( schema );
			break;
		}

		case 4: // Camera
		{
			Alembic::AbcGeom::ICamera xform( _object, Alembic::Abc::kWrapExisting );
			Alembic::AbcGeom::ICameraSchema schema = xform.getSchema();
			GET_TIME_INFO( schema );
			break;
		}

		case 5: // Curves
		{
			Alembic::AbcGeom::ICurves xform( _object, Alembic::Abc::kWrapExisting );
			Alembic::AbcGeom::ICurvesSchema schema = xform.getSchema();
			GET_TIME_INFO( schema );
			break;
		}

		case 6: // NuPatch
		{
			Alembic::AbcGeom::INuPatch xform( _object, Alembic::Abc::kWrapExisting );
			Alembic::AbcGeom::INuPatchSchema schema = xform.getSchema();
			GET_TIME_INFO( schema );
			break;
		}

		case 7: // Points
		{
			Alembic::AbcGeom::IPoints xform( _object, Alembic::Abc::kWrapExisting );
			Alembic::AbcGeom::IPointsSchema schema = xform.getSchema();
			GET_TIME_INFO( schema );
			break;
		}

		case 8: // Light
		{
			break;
		}
	}
}

Alembic::Abc::IObject&
ZAlembicObject::object()
{
	return _object;
}

const Alembic::Abc::IObject&
ZAlembicObject::object() const
{
	return _object;
}

Alembic::Abc::ObjectHeader&
ZAlembicObject::header()
{
	return _header;
}

const Alembic::Abc::ObjectHeader&
ZAlembicObject::header() const
{
	return _header;
}

bool
ZAlembicObject::getMetaData( ZStringArray& keys, ZStringArray& values ) const
{
	const Alembic::Abc::MetaData md = _header.getMetaData();

	return GetMetaData( md, keys, values );
}

bool
ZAlembicObject::getChild( int i, ZAlembicObject& childObject ) const
{
	childObject.reset();

	if( i < 0 || i >= _numChildren ) { return false; }

	childObject.set( _object.getChild(i) );

	return true;
}

bool
ZAlembicObject::getChild( const ZString& iChildName, ZAlembicObject& childObject ) const
{
    childObject.reset();

    Alembic::Abc::IObject ret = _object.getChild(iChildName);
    if(!ret.valid())
        return false;

    childObject.set( ret );

    return true;
}


bool
ZAlembicObject::getParent( ZAlembicObject& parentObject ) const
{
	Alembic::Abc::IObject obj = _object.getParent();

	if( !obj )
	{
		parentObject.reset();
		return false;
	}

	parentObject.set( obj );

	return true;
}

const ZAlembicProperty&
ZAlembicObject::topProperty() const
{
	return _topPrp;
}

ZAlembicProperty&
ZAlembicObject::topProperty()
{
	return _topPrp;
}

ZAlembicObject&
ZAlembicObject::operator=( const ZAlembicObject& obj )
{
	_header           = obj._header;
	_object           = obj._object;
	_name             = obj._name;
	_fullPath         = obj._fullPath;
	_typeId           = obj._typeId;
	_numChildren      = obj._numChildren;
	_topPrp           = obj._topPrp;

	_timeSamplingType = obj._timeSamplingType;
	_numTimeSamples   = obj._numTimeSamples;
	_minTime          = obj._minTime;
	_maxTime          = obj._maxTime;
	_timeStepSize     = obj._timeStepSize;
	_minFrame         = obj._minFrame;
	_maxFrame         = obj._maxFrame;

	return (*this);
}

ZString
ZAlembicObject::name() const
{
	return _name;
}

ZString
ZAlembicObject::fullPath() const
{
	return _fullPath;
}

int
ZAlembicObject::typeId() const
{
	return _typeId;
}

ZString
ZAlembicObject::typeStr() const
{
	if( _typeId == 1 ) { return "Xform";    }
	if( _typeId == 2 ) { return "SubD";     }
	if( _typeId == 3 ) { return "PolyMesh"; }
	if( _typeId == 4 ) { return "Camera";   }
	if( _typeId == 5 ) { return "Curves";   }
	if( _typeId == 6 ) { return "NuPatch";  }
	if( _typeId == 7 ) { return "Points";   }
	if( _typeId == 8 ) { return "Light";    }

	return "none";
}

bool
ZAlembicObject::isShape() const
{
	if( _typeId == 1 ) { return false; }
	if( _typeId == 2 ) { return true;  }
	if( _typeId == 3 ) { return true;  }
	if( _typeId == 4 ) { return true;  }
	if( _typeId == 5 ) { return true;  }
	if( _typeId == 6 ) { return true;  }
	if( _typeId == 7 ) { return true;  }
	if( _typeId == 8 ) { return true;  }

	return false;
}

int
ZAlembicObject::numChildren() const
{
	return _numChildren;
}

bool
ZAlembicObject::isLeaf() const
{
	return ( _numChildren ? false : true );
}

int
ZAlembicObject::timeSamplingTypeId() const
{
	return _timeSamplingType;
}

ZString
ZAlembicObject::timeSamplingTypeStr() const
{
	if( _timeSamplingType == 1 ) { return "uniform"; }
	if( _timeSamplingType == 2 ) { return "cyclic";  }
	if( _timeSamplingType == 3 ) { return "acyclic"; }

	return "static";
}

int
ZAlembicObject::numTimeSamples() const
{
	return _numTimeSamples;
}

bool
ZAlembicObject::isConstant() const
{
	if( _timeSamplingType ) { return false; }
	return true;
}

double
ZAlembicObject::minTime() const
{
	return _minTime;
}

double
ZAlembicObject::maxTime() const
{
	return _maxTime;
}

double
ZAlembicObject::timeStepSize() const
{
	return _timeStepSize;
}

double
ZAlembicObject::minFrame() const
{
	return _minFrame;
}

double
ZAlembicObject::maxFrame() const
{
	return _maxFrame;
}

bool
ZAlembicObject::getTransformations( ZVector& t, ZVector& r, ZVector& s, int frame ) const
{
	if( _typeId != 1 )
	{
		t.zeroize();
		r.zeroize();
		s.zeroize();

		return false;
	}

	Alembic::Abc::IObject parentObj = _object.getParent();
	Alembic::AbcGeom::IXform xformObj( parentObj, _name );
	Alembic::AbcGeom::IXformSchema& xform = xformObj.getSchema();

	const Alembic::Abc::index_t i = (Alembic::Abc::index_t)ZClamp( frame-_minFrame, 0, _maxFrame-_minFrame );
	const Alembic::Abc::ISampleSelector iss = Alembic::Abc::ISampleSelector(i);

	Alembic::AbcGeom::XformSample sample;
	xform.get( sample, iss );

	Alembic::Abc::V3d T = sample.getTranslation();
	t.set( (float)T.x, (float)T.y, (float)T.z );

	r.x = sample.getXRotation();
	r.y = sample.getYRotation();
	r.z = sample.getZRotation();

	Alembic::Abc::V3d S = sample.getScale();
	s.set( (float)S.x, (float)S.y, (float)S.z );

	return true;
}

bool
ZAlembicObject::getXFormMatrix( ZMatrix& xform, int frame ) const
{
	if( _typeId != 1 ) { return false; }

	const Alembic::Abc::index_t i = (Alembic::Abc::index_t)ZClamp( frame-_minFrame, 0, _maxFrame-_minFrame );
	const Alembic::Abc::ISampleSelector iss = Alembic::Abc::ISampleSelector( i );

	//Alembic::Abc::IObject parentObj = _object.getParent();

	ZVector t, r, s;
	ZAlembicObject::getTransformations( t, r, s, frame );

	xform.setToIdentity();
	xform.setTranslation( t );
	xform.setEulerRotation( r, ZRotationOrder::zZYX, false );
	xform.addScale( s.x, s.y, s.z );

	return true;
}

bool
ZAlembicObject::getPolygonMeshInfo( int& numVertices, int& numPolygons, int& numUVs, int frame ) const
{
	numVertices = numPolygons = numUVs = 0;

	if( _typeId != 3 ) { return false; }

	const Alembic::Abc::index_t i = (Alembic::Abc::index_t)ZClamp( frame-_minFrame, 0, _maxFrame-_minFrame );
	const Alembic::Abc::ISampleSelector iss = Alembic::Abc::ISampleSelector(i);

	Alembic::Abc::IObject parentObj = _object.getParent();
	Alembic::AbcGeom::IPolyMesh meshObj( parentObj, _name );
	Alembic::AbcGeom::IPolyMeshSchema& mesh = meshObj.getSchema();

	Alembic::AbcGeom::IPolyMeshSchema::Sample sample;
	mesh.get( sample, iss );	

	numVertices = (int)sample.getPositions()->size();
	numPolygons = (int)sample.getFaceCounts()->size();

	return true;
}

// Caution) vConnections and uvIndices must be reversed per each polygon.
// Use the ReverseConnections() function in ZArrayUtils.h.
bool
ZAlembicObject::getPolyMeshData
(
	ZPointArray*  vPos,
	ZVectorArray* vVel,
	ZIntArray*    vCounts,
	ZIntArray*    vConnections,
	ZBoundingBox* bBox,
	ZMatrix*      worldMat,
	ZFloatArray*  uvs,
	ZIntArray*    uvIndices,
	int           frame
) const
{
	if( _typeId != 3 ) { return false; }

	const Alembic::Abc::index_t i = (Alembic::Abc::index_t)ZClamp( frame-_minFrame, 0, _maxFrame-_minFrame );
	const Alembic::Abc::ISampleSelector iss = Alembic::Abc::ISampleSelector( i );

	Alembic::Abc::IObject parentObj = _object.getParent();
	Alembic::AbcGeom::IPolyMesh meshObj( parentObj, _name );
	Alembic::AbcGeom::IPolyMeshSchema& mesh = meshObj.getSchema();

	Alembic::AbcGeom::IPolyMeshSchema::Sample sample;
	mesh.get( sample, iss );	

	// vertex positions
	if( vPos )
	{
		Alembic::Abc::P3fArraySamplePtr data = sample.getPositions();

		if( data ) {

			const int nVerts = (int)data->size();
			vPos->setLength( nVerts );

			if( nVerts ) {

				memcpy( (char*)&((*vPos)[0]), (char*)&((*data)[0]), sizeof(float)*3*nVerts );

			} else {

				vPos->zeroize();

			}

		} else {

			vPos->reset();

		}
	}

	// vertex velocities
	if( vVel )
	{
		Alembic::Abc::V3fArraySamplePtr data = sample.getVelocities();

		if( data ) {

			const int nVerts = (int)data->size();
			vVel->setLength( nVerts );

			if( nVerts ) {

				memcpy( (char*)&((*vVel)[0]), (char*)&((*data)[0]), sizeof(float)*3*nVerts );

			} else {

				vVel->zeroize();

			}

		} else {

			vVel->reset();

		}
	}

	// vCounts for other outputs
	ZIntArray* _vCounts;
	ZIntArray tmp;

	// polygon vertex counts
	if( vCounts ) { _vCounts = vCounts; }
	else          { _vCounts = &tmp;    }

	{
		Alembic::Abc::Int32ArraySamplePtr data = sample.getFaceCounts();

		if( data ) {

			const int N = (int)data->size();
			_vCounts->setLength( N );

			if( N ) {

				memcpy( (char*)&((*_vCounts)[0]), (char*)&((*data)[0]), sizeof(int)*N );

			} else {

				_vCounts->zeroize();

			}

		} else {

			_vCounts->reset();

		}
	}

	// polygon vertex indices
	if( vConnections )
	{
		Alembic::Abc::Int32ArraySamplePtr data = sample.getFaceIndices();

		if( data ) {

			const int N = (int)data->size();
			vConnections->setLength( N );

			if( N ) {

				memcpy( (char*)&((*vConnections)[0]), (char*)&((*data)[0]), sizeof(int)*N );

			} else {

				vConnections->zeroize();

			}

		} else {

			vConnections->reset();

		}
	}

	if( bBox )
	{
		const Alembic::Abc::Box3d b = sample.getSelfBounds();

		const ZPoint minPt( (float)b.min.x, (float)b.min.y, (float)b.min.z );
		const ZPoint maxPt( (float)b.max.x, (float)b.max.y, (float)b.max.z );

		bBox->set( minPt, maxPt );
	}

	if( worldMat )
	{
		ZAlembicObjectArray parentObjs;
		{
			ZAlembicObject obj = *this;
			ZAlembicObject parentObj;

			while( obj.getParent( parentObj ) )
			{
				parentObjs.append( parentObj );

				obj = parentObj;
			}

			parentObjs.reverse();
		}

		ZMatrix& worldMatrix = *worldMat;
		worldMatrix.setToIdentity();

		const int count = parentObjs.length();

		FOR( i, 1, count ) // Skip the top-most object.
		{
			ZVector t, r, s;
			parentObjs[i].getTransformations( t, r, s, frame );

			ZMatrix xform;
			xform.setTranslation( t );
			xform.setEulerRotation( r, ZRotationOrder::zZYX, false );
			xform.addScale( s.x, s.y, s.z );

			worldMatrix *= xform;
		}
	}

	Alembic::AbcGeom::IV2fGeomParam uvParam = mesh.getUVsParam();

	if( uvs )
	{
		if( uvParam.valid() ) {

			Alembic::AbcGeom::IV2fGeomParam::Sample uvSample = uvParam.getIndexedValue();
			Alembic::Abc::V2fArraySamplePtr data = uvSample.getVals();

			if( data ) {

				const int N = (int)data->size();
				uvs->setLength( N*2 );

				if( N ) {

					memcpy( (char*)&((*uvs)[0]), (char*)&((*data)[0]), sizeof(float)*N*2 );

				} else {

					uvs->reset();

				}

			} else {

				uvs->reset();

			}

		} else {

			uvs->reset();

		}
	}

	if( uvIndices )
	{
		if( uvParam.valid() ) {

			Alembic::AbcGeom::IV2fGeomParam::Sample uvSample = uvParam.getIndexedValue();
			Alembic::Abc::UInt32ArraySamplePtr data = uvSample.getIndices();

			if( data ) {

				const int N = (int)data->size();
				uvIndices->setLength( N );

				if( N ) {

					memcpy( (char*)&((*uvIndices)[0]), (char*)&((*data)[0]), sizeof(int)*N );

				} else {

					uvIndices->reset();

				}

			} else {

				uvIndices->reset();

			}

		} else {

			uvIndices->reset();

		}
	}

	return true;
}

int
ZAlembicObject::particleCount( int frame ) const
{
	if( _typeId != 7 ) { return false; }

	const Alembic::Abc::index_t i = (Alembic::Abc::index_t)ZClamp( frame-_minFrame, 0, _maxFrame-_minFrame );
	const Alembic::Abc::ISampleSelector iss = Alembic::Abc::ISampleSelector(i);

	Alembic::Abc::IObject parentObj = _object.getParent();
	Alembic::AbcGeom::IPoints pointsObj( parentObj, _name );
	Alembic::AbcGeom::IPointsSchema& points = pointsObj.getSchema();

	Alembic::AbcGeom::IPointsSchema::Sample sample;
	points.get( sample, iss );

	const Alembic::Abc::P3fArraySamplePtr pPtr = sample.getPositions();

	return (int)( pPtr->size() );
}

bool
ZAlembicObject::getParticlePositions( ZPointArray& positions, int frame ) const
{
	if( _typeId != 7 ) { positions.clear(); return false; }

	const Alembic::Abc::index_t i = (Alembic::Abc::index_t)ZClamp( frame-_minFrame, 0, _maxFrame-_minFrame );
	const Alembic::Abc::ISampleSelector iss = Alembic::Abc::ISampleSelector(i);

	Alembic::Abc::IObject parentObj = _object.getParent();
	Alembic::AbcGeom::IPoints pointsObj( parentObj, _name );
	Alembic::AbcGeom::IPointsSchema& points = pointsObj.getSchema();

	Alembic::AbcGeom::IPointsSchema::Sample sample;
	points.get( sample, iss );

	const Alembic::Abc::P3fArraySamplePtr ptr = sample.getPositions();

	const int count = (int)( ptr->size() );
	if( !count ) { positions.clear(); return true; }

	positions.setLength( count );
	memcpy( (char*)&(positions[0]), (char*)&((*ptr)[0]), sizeof(float)*3*count );

	return true;
}

bool
ZAlembicObject::getParticleIds( ZIntArray& ids, int frame ) const
{
	if( _typeId != 7 ) { ids.clear(); return false; }

	const Alembic::Abc::index_t i = (Alembic::Abc::index_t)ZClamp( frame-_minFrame, 0, _maxFrame-_minFrame );
	const Alembic::Abc::ISampleSelector iss = Alembic::Abc::ISampleSelector(i);

	Alembic::Abc::IObject parentObj = _object.getParent();
	Alembic::AbcGeom::IPoints pointsObj( parentObj, _name );
	Alembic::AbcGeom::IPointsSchema& points = pointsObj.getSchema();

	Alembic::AbcGeom::IPointsSchema::Sample sample;
	points.get( sample, iss );

	const Alembic::Abc::UInt64ArraySamplePtr ptr = sample.getIds();

	const int count = (int)( ptr->size() );
	if( !count ) { return true; }

	ids.setLength( count );

	FOR( i, 0, count )
	{
		ids[i] = (int)( (*ptr)[i] );
	}

	return true;
}

bool
ZAlembicObject::getParticleVelocities( ZPointArray& velocities, int frame ) const
{
	if( _typeId != 7 ) { velocities.clear(); return false; }

	const Alembic::Abc::index_t i = (Alembic::Abc::index_t)ZClamp( frame-_minFrame, 0, _maxFrame-_minFrame );
	const Alembic::Abc::ISampleSelector iss = Alembic::Abc::ISampleSelector(i);

	Alembic::Abc::IObject parentObj = _object.getParent();
	Alembic::AbcGeom::IPoints pointsObj( parentObj, _name );
	Alembic::AbcGeom::IPointsSchema& points = pointsObj.getSchema();

	Alembic::AbcGeom::IPointsSchema::Sample sample;
	points.get( sample, iss );

	const Alembic::Abc::V3fArraySamplePtr ptr = sample.getVelocities();

	const int count = (int)( ptr->size() );
	if( !count ) { velocities.clear(); return true; }

	velocities.setLength( count );
	memcpy( (char*)&(velocities[0]), (char*)&((*ptr)[0]), sizeof(float)*3*count );

	return true;
}

bool
ZAlembicObject::getParticleAABB( ZBoundingBox& aabb, int frame ) const
{
	aabb.reset();

	if( _typeId != 7 ) { return false; }

	const Alembic::Abc::index_t i = (Alembic::Abc::index_t)ZClamp( frame-_minFrame, 0, _maxFrame-_minFrame );
	const Alembic::Abc::ISampleSelector iss = Alembic::Abc::ISampleSelector(i);

	Alembic::Abc::IObject parentObj = _object.getParent();
	Alembic::AbcGeom::IPoints pointsObj( parentObj, _name );
	Alembic::AbcGeom::IPointsSchema& points = pointsObj.getSchema();

	Alembic::AbcGeom::IPointsSchema::Sample sample;
	points.get( sample, iss );

	const Alembic::Abc::Box3d b = sample.getSelfBounds();

	const ZPoint minPt( (float)b.min.x, (float)b.min.y, (float)b.min.z );
	const ZPoint maxPt( (float)b.max.x, (float)b.max.y, (float)b.max.z );

	aabb.set( minPt, maxPt );

	return true;
}

bool ZAlembicObject::valid() const
{
    return _object.valid();
}


ostream&
operator<<( ostream& os, const ZAlembicObject& object )
{
	os << "<ZAlembicObject>" << endl;
	os << " Name              : " << object.name() << endl;
	os << " Full Path         : " << object.fullPath() << endl;
	os << " Type              : " << object.typeStr() << endl;
	os << " # of Children     : " << object.numChildren() << endl;

	os << " Time Sampling Type: " << object.timeSamplingTypeStr() << endl;

	if( object.timeSamplingTypeId() > 0 ) // non-static
	{
		os << " # of Time Samples : " << object.numTimeSamples() << endl;
		os << " Time Range        : " << object.minTime() << " ~ " << object.maxTime() << endl;
		os << " Time Step Size    : " << object.timeStepSize() << endl;
		os << " Frame Range       : " << object.minFrame() << " ~ " << object.maxFrame() << endl;
	}

	os << endl;

	return os;
}

ZELOS_NAMESPACE_END

