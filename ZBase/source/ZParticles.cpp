//----------------//
// ZParticles.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// 		   Jinhyuk Bae @ Dexter Studios					 //
// last update: 2017.06.09                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZParticles::ZParticles()
: _numAttributes(0), _numParticles(0), _numAllocated(0)
{
	ZParticles::reset();
}

ZParticles::ZParticles( const ZParticles& ptc )
: _numAttributes(0), _numParticles(0), _numAllocated(0)
{
	ZParticles::reset();
	ZParticles::operator=( ptc );
}

ZParticles::ZParticles( const char* filePathName )
: _numAttributes(0), _numParticles(0), _numAllocated(0)
{
	ZParticles::reset();
	ZParticles::load( filePathName );
}

ZParticles::~ZParticles()
{
	ZParticles::reset();
}

void
ZParticles::reset()
{
	if( _numAttributes > 0 )
	{
		_dataType.clear();
		_dataSize.clear();
		_attrName.clear();
		_nameToIndex.clear();

		FOR( i, 0, _numAttributes )
		{
			if( _data[i] )
			{
				free( _data[i] );
			}
		}

		_data.clear();
	}

	_numAttributes = 0;
	_numParticles  = 0;
	_numAllocated  = 0;

	_groupId       = 0;
	_groupColor    = ZColor::white();

	_aabb          = ZBoundingBox();
}

bool
ZParticles::checkCompatible( const ZParticles& ptc ) const
{
	if( _numAttributes != ptc._numAttributes )
	{
		return false;
	}

	FOR( i, 0, _numAttributes )
	{
		if( _dataType[i] != ptc._dataType[i] ) 
		{
			return false;
		}

		if( _attrName[i] != ptc._attrName[i] )
		{
			return false;
		}
	}

	return true;
}

ZParticles&
ZParticles::operator=( const ZParticles& ptc )
{
	bool toAlloc = false;
	{
		if( _numParticles != ptc._numParticles )
		{
			toAlloc = true;
		}

		if( !checkCompatible( ptc ) )
		{
			toAlloc = true;
		}
	}

	if( toAlloc )
	{
		ZParticles::reset();

		FOR( i, 0, ptc._numAttributes )
		{			
			ZParticles::addAttribute( ptc._attrName[i].asChar(), static_cast<ZDataType::DataType>(ptc._dataType[i]) );
		}

		if( !addParticles( ptc._numParticles ) )
		{
			cout << "Error@ZParticles()::operator=(): Failed to add particles." << endl;
			ZParticles::reset();
			return (*this);
		}
	}

	FOR( i, 0, _numAttributes )
	{
		memcpy( _data[i], ptc._data[i], _numParticles*_dataSize[i] );
	}

	_groupId    = ptc._groupId;
	_groupColor = ptc._groupColor;

	_aabb       = ptc._aabb;

	return (*this);
}

int
ZParticles::numParticles() const
{
	return _numParticles;
}

int
ZParticles::numAttributes() const
{
	return _numAttributes;
}

int
ZParticles::dataType( const int& i ) const
{
	return _dataType[i];
}

int
ZParticles::dataSize( const int& i ) const
{
	return _dataSize[i];
}

ZString
ZParticles::attributeName( const int& i ) const
{
	return _attrName[i];
}

int
ZParticles::attributeIndex( const char* attrName ) const
{
	if( _numAttributes <= 0 ) { return -1; }

	std::map<ZString,int>::const_iterator itr = _nameToIndex.find( attrName );

	if( itr == _nameToIndex.end() )
	{ 
		return -1; 
	}

	return ( itr->second );
}

bool
ZParticles::addAttribute( const char* attrName, ZDataType::DataType zDataType )
{
	// You can add a new attribute only when there is no particles.
	if( _numParticles )
	{
		return false;
	}

	// Check if the given attribute already exists.
	if( _numAttributes > 0 )
	{
		if( ZParticles::attributeIndex( attrName ) >= 0 )
		{
			return true;
		}
	}

	const int dataType = static_cast<int>(zDataType);
	const int dataSize = ZDataType::bytes( zDataType );

	_dataType.push_back( dataType );
	_dataSize.push_back( dataSize );
	_attrName.push_back( attrName );

	_nameToIndex[ attrName ] = _numAttributes++;

    _numAllocated = 10;

	char* ptr = (char*)calloc( _numAllocated, dataSize );

	if( !ptr )
	{
		cout << "Error@ZParticles::addAttributes(): Failed to allocated memory." << endl;
		ZParticles::reset();
		return false;
	}

	_data.push_back( ptr );

	return true;
}

bool
ZParticles::deleteAttribute( const char* attrName )
{
	std::map<ZString,int>::iterator itr = _nameToIndex.find( attrName );

	// Check if the given attribute already exists.
	if( itr == _nameToIndex.end() )
	{
		return true;
	}

	const int& attrIndex = itr->second;

	_dataType.erase( _dataType.begin() + attrIndex );
	_dataSize.erase( _dataSize.begin() + attrIndex );
	_attrName.erase( _attrName.begin() + attrIndex );
	_nameToIndex.erase( itr );

	if( _data[attrIndex] )
	{
		free( _data[attrIndex] );
	}

	_data.erase( _data.begin() + attrIndex );

	--_numAttributes;

	return true;
}

bool
ZParticles::addParticles( const int& numToAdd )
{
	const int newNumParticles = _numParticles + numToAdd;

	if( newNumParticles > _numAllocated )
	{
		_numAllocated = ZMax( 10, newNumParticles, ZRound(1.5f*_numAllocated) );

		FOR( i, 0, _numAttributes )
		{
			_data[i] = (char*)realloc( _data[i], _numAllocated * _dataSize[i] );

			if( !_data[i] )
			{
				cout << "Error@ZParticles::addParticles(): Failed to allocate memory." << endl;
				ZParticles::reset();
				return false;
			}
		}
	}

	_numParticles = newNumParticles;

	return true;
}

bool
ZParticles::append( const ZParticles& ptc )
{
	if( !ZParticles::checkCompatible( ptc ) )
	{
		cout << "Error@ZParticles@append(): Not compatible." << endl;
		return false;
	}

	const int oldNumParticles = _numParticles;

	ZParticles::addParticles( ptc._numParticles );

	FOR( i, 0, _numAttributes )
	{
		char* ptr = (char*)ZParticles::data( _attrName[i].asChar(), oldNumParticles );

		memcpy( ptr, ptc._data[i], ptc._numParticles*_dataSize[i] );
	}

	return true;
}

int
ZParticles::remove( const ZIntArray& delList )
{
	if( !_numParticles ) { return 0; }

	const int listSize = delList.length();
	if( !listSize ) { return _numParticles; }

	std::vector<bool> mask( _numParticles, false );

	int numToDel = 0;

	std::vector<int>::const_iterator itr = delList.begin();

	for( ; itr!=delList.end(); ++itr )
	{
		const int& idx = *itr;

		if( ( 0 <= idx ) && ( idx < _numParticles ) )
		{
			mask[idx] = true;

			++numToDel;
		}
	}

	if( numToDel == _numParticles )
	{
		ZParticles::reset();
		return 0;
	}

	int startIdx = 0;

	while( startIdx < _numParticles )
	{
		if( mask[startIdx] )
		{
			break;
		}

		++startIdx;
	}

	FOR( i, 0, _numAttributes )
	{
		const int& dataSize = _dataSize[i];

		int next = startIdx + 1;

		FOR( j, startIdx, _numParticles )
		{
			if( next >= _numParticles )
			{
				break;
			}

			while( next < _numParticles )
			{
				if( !mask[next] )
				{
					break;
				}

				++next;
			}

			FOR( k, 0, dataSize )
			{
				_data[i][(j*dataSize)+k] = _data[i][(next*dataSize)+k];
			}

			++next;
		}
	}

	return ( _numParticles -= numToDel );
}

void*
ZParticles::data( const char* attrName, const int& particleIndex ) const
{
	map<ZString,int>::const_iterator itr = _nameToIndex.find( attrName );

	if( itr == _nameToIndex.end() )
	{
		return (void*)(NULL);
	}

	const int& attrIndex = itr->second;

	return ( _data[attrIndex] + ( particleIndex * _dataSize[attrIndex] ) );
}

void*
ZParticles::data( const int& attrIndex, const int& particleIndex ) const
{
	if( ( attrIndex < 0 ) || ( attrIndex >= _numAttributes ) )
	{
		return (void*)(NULL);
	}

	return ( _data[attrIndex] + ( particleIndex * _dataSize[attrIndex] ) );
}

bool
ZParticles::computeBoundingBox( const char* attrName, bool useOpenMP )
{
	_aabb.reset();

	const int attrIdx = ZParticles::attributeIndex( attrName );

	if( attrIdx < 0 )
	{
		return false;
	}

	if( ZParticles::dataType(attrIdx) != static_cast<int>(ZDataType::zPoint) )
	{
		return false;
	}

	const int& n = _numParticles;

	if( n <= 0 )
	{
		return false;
	}

	const ZPoint* pPos = (ZPoint*)ZParticles::data( attrIdx );

	if( useOpenMP && n>10000 ) {

		const int numCPUs = omp_get_num_procs();
		const int numThreads = 2*numCPUs;

		ZBoundingBoxArray bBoxes( numThreads );

		#pragma omp parallel for
		FOR( threadId, 0, numThreads )
		{
			const int startIdx = (int)( (threadId*n) / numThreads );
			const int endIdx   = (int)ZMin( startIdx+((n+1)/numThreads), n );

			FOR( i, startIdx, endIdx )
			{
				bBoxes[threadId].expand( pPos[i] );
			}
		}

		_aabb = bBoxes.boundingBox();

	} else {

		FOR( i, 0, n )
		{
			_aabb.expand( pPos[i] );
		}

	}

	return true;
}

int
ZParticles::groupId() const
{
	return _groupId;
}

ZColor
ZParticles::groupColor() const
{
	return _groupColor;
}

ZBoundingBox
ZParticles::boundingBox() const
{
	return _aabb;
}

float
ZParticles::minMagnitude( const char* attrName, bool useOpenMP ) const
{
	const int attrIdx = ZParticles::attributeIndex( attrName );

	if( attrIdx < 0 )
	{
		return false;
	}

	if( ZParticles::dataType(attrIdx) != static_cast<int>(ZDataType::zVector) )
	{
		return false;
	}

	const int& n = _numParticles;

	if( n <= 0 )
	{
		return false;
	}

	float minMag = Z_LARGE;

	const ZVector* pVec = (ZVector*)ZParticles::data( attrIdx );

	if( useOpenMP && n>10000 ) {

		const int numCPUs = omp_get_num_procs();
		const int numThreads = 2*numCPUs;

		ZFloatArray localMins( numThreads, Z_LARGE );

		#pragma omp parallel for
		FOR( threadId, 0, numThreads )
		{
			const int startIdx = (int)( (threadId*n) / numThreads );
			const int endIdx   = (int)ZMin( startIdx+((n+1)/numThreads), n );

			FOR( i, startIdx, endIdx )
			{
				localMins[threadId] = ZMin( localMins[threadId], pVec[i].squaredLength() );
			}
		}

		minMag = localMins.min();


	} else {

		FOR( i, 0, n )
		{
			minMag = ZMin( minMag, pVec[i].squaredLength() );
		}

	}

	return sqrtf( minMag );
}

float
ZParticles::maxMagnitude( const char* attrName, bool useOpenMP ) const
{
	const int attrIdx = ZParticles::attributeIndex( attrName );

	if( attrIdx < 0 )
	{
		return false;
	}

	if( ZParticles::dataType(attrIdx) != static_cast<int>(ZDataType::zVector) )
	{
		return false;
	}

	const int& n = _numParticles;

	if( n <= 0 )
	{
		return false;
	}

	float maxMag = -Z_LARGE;

	const ZVector* pVec = (ZVector*)ZParticles::data( attrIdx );

	if( useOpenMP && n>10000 ) {

		const int numCPUs = omp_get_num_procs();
		const int numThreads = 2*numCPUs;

		ZFloatArray localMaxs( numThreads, -Z_LARGE );

		#pragma omp parallel for
		FOR( threadId, 0, numThreads )
		{
			const int startIdx = (int)( (threadId*n) / numThreads );
			const int endIdx   = (int)ZMax( startIdx+((n+1)/numThreads), n );

			FOR( i, startIdx, endIdx )
			{
				localMaxs[threadId] = ZMax( localMaxs[threadId], pVec[i].squaredLength() );
			}
		}

		maxMag = localMaxs.max();


	} else {

		FOR( i, 0, n )
		{
			maxMag = ZMax( maxMag, pVec[i].squaredLength() );
		}

	}

	return sqrtf( maxMag );
}

bool
ZParticles::save( const char* filePathName ) const
{
//	ofstream fout( filePathName, ios::out|ios::binary );
//
//	//if( !ZWriteFileHeader( fout, filePathName, "ZParticle", 1.0f ) ) { return false; }
//
//	fout.write( (char*)&groupId, sizeof(int) );
//	groupColor.write( fout );
//
//	fout.write( (char*)&_numParticles,  sizeof(int) );
//	fout.write( (char*)&_numAttributes, sizeof(int) );
//
//	_dataType.write( fout, true );
//	_attrName.write( fout );
//
//	FOR( i, 0, _numAttributes )
//	{
//		fout.write( (char*)_data[i], _numParticles * _dataSize[i] );
//	}
//
//	fout.close();

	return true;
}

bool
ZParticles::load( const char* filePathName )
{
//	ifstream fin( filePathName, ios::in|ios::binary );
//	
//	//if( !ZReadFileHeader( fin, filePathName, "ZParticle", 1.0f ) )
//	{
//		ZParticles::reset();
//		return false;
//	}
//
//	fin.read( (char*)&groupId, sizeof(int) );
//	groupColor.read( fin );
//
//	int nParticles  = 0;   fin.read( (char*)&nParticles,  sizeof(int) );
//	int nAttributes = 0;   fin.read( (char*)&nAttributes, sizeof(int) );
//	
//	ZIntArray dType;       dType.read( fin, true );
//	ZStringArray aName;    aName.read( fin );
//
//	if( !( (_numParticles==nParticles) && (_numAttributes==nAttributes) && (_dataType==dType) && (_attrName == aName) ) )
//	{
//		ZParticles::reset();
//
//		FOR( i, 0, nAttributes )
//		{
//			addAttribute( aName[i].asChar(), static_cast<ZDataType::DataType>(dType[i]) );
//		}
//
//		addParticles( nParticles );
//	}
//
//	FOR( i, 0, _numAttributes )
//	{
//		fin.read( (char*)_data[i], _numParticles * _dataSize[i] );
//	}
//
//	fin.close();

	return true;
}

bool
ZParticles::_save_zelos( const char* filePathName ) const
{
	return true;
}

bool
ZParticles::_save_classicBGEO( const char* filePathName ) const
{
	return true;
}

bool
ZParticles::_save_alembic( const char* filePathName ) const
{
	return true;
}

bool
ZParticles::_save_bifrost( const char* filePathName ) const
{
	return true;
}

bool
ZParticles::_load_zelos( const char* filePathName )
{
	return true;
}

bool
ZParticles::_load_classicBGEO( const char* filePathName )
{
	return true;
}

bool
ZParticles::_load_alembic( const char* filePathName )
{
	return true;
}

bool
ZParticles::_load_bifrost( const char* filePathName )
{
	return true;
}

ostream&
operator<<( ostream& os, const ZParticles& ptc )
{
	os << "<ZParticles>" << endl;
	os << "# of particles : " << ptc.numParticles()  << endl;
	os << "# of attributes: " << ptc.numAttributes() << endl;

	FOR( i, 0, ptc.numAttributes() )
	{
		os << i << ": " << ptc.attributeName(i) << " ( " << ZDataType::name( static_cast<ZDataType::DataType>(ptc.dataType(i)) ) << " ) " << endl;
	}
	return os;
}

ZELOS_NAMESPACE_END

