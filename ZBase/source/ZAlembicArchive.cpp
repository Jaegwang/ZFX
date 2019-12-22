//---------------------//
// ZAlembicArchive.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2017.03.03                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZAlembicArchive::ZAlembicArchive()
: _opened(false)
{
	ZAlembicArchive::reset();
}

ZAlembicArchive::ZAlembicArchive( const char* filePathName )
: _opened(false)
{
	ZAlembicArchive::open( filePathName );
}

ZAlembicArchive::~ZAlembicArchive()
{
	ZAlembicArchive::reset();
}

void
ZAlembicArchive::reset()
{
	_opened           = false;
	_archiveType      = 0;
	_filePathName     = "";
	_archive          = Alembic::Abc::IArchive();
	_topObj           = ZAlembicObject();

	_abcVersion       = 0;
	_abcVersionStr    = "";
	_appName          = "";
	_writtenDate      = "";
	_description      = "";

	_archivePtr       = Alembic::Abc::ArchiveReaderPtr();

	_timeSamplingType = 0;
	_numTimeSamples   = 0;
	_startTime        = 0.0;
	_endTime          = 0.0;
	_timeStepSize     = 0.0;
}

Alembic::Abc::IArchive&
ZAlembicArchive::archive()
{
	return _archive;
}

const Alembic::Abc::IArchive&
ZAlembicArchive::archive() const
{
	return _archive;
}

bool
ZAlembicArchive::open( const char* filePathName )
{
	ZAlembicArchive::reset();

	Alembic::AbcCoreFactory::IFactory factory;
	Alembic::AbcCoreFactory::IFactory::CoreType coreType;

	_archive = factory.getArchive( filePathName, coreType );

	if( !_archive.valid() )
	{
		cout << "Error@ZAlembicArchive::open(): Failed to open file." << endl;

		ZAlembicArchive::reset();

		return false;
	}

	_opened = true;

	if( coreType == Alembic::AbcCoreFactory::IFactory::kHDF5 ) {

		_archiveType = 1;

	} else if( coreType == Alembic::AbcCoreFactory::IFactory::kOgawa ) {

		_archiveType = 2;

	} else {

		_archiveType = 0;

	}

	_filePathName = filePathName;

	_topObj.set( _archive.getTop() );

	_abcVersion = (int)_archive.getArchiveVersion();

	Alembic::Abc::MetaData md = _archive.getPtr()->getMetaData();
	{
		_abcVersionStr = md.get( "_ai_AlembicVersion" );
		_appName       = md.get( Alembic::Abc::kApplicationNameKey  );
		_writtenDate   = md.get( Alembic::Abc::kDateWrittenKey      );
		_description   = md.get( Alembic::Abc::kUserDescriptionKey  );
	}

	_archivePtr = _archive.getPtr();

	_numTimeSamples = (int)_archive.getNumTimeSamplings();

	double startTime = +DBL_MAX;
	double endTime   = -DBL_MAX;

	double startSingleTime = +DBL_MAX;
	double endSingleTime   = -DBL_MAX;

	double startDefaultTime = +DBL_MAX;
	double endDefaultTime   = -DBL_MAX;

	FOR( i, 0, _numTimeSamples )
	{
		Alembic::Abc::index_t idx = _archive.getMaxNumSamplesForTimeSamplingIndex( i );
		if( idx == INDEX_UNKNOWN ) { continue; }

		Alembic::Abc::TimeSamplingPtr tsPtr = _archive.getTimeSampling( i );
		if( !tsPtr ) { continue; }

		Alembic::Abc::TimeSamplingType tsType = tsPtr->getTimeSamplingType();

		if( ( idx > 1 ) && ( i != 0 ) ) {

			startTime = ZMin( tsPtr->getSampleTime( 0 ), startTime );
			endTime = ZMax( tsPtr->getSampleTime( idx-1 ), endTime );

		} else if( ( idx == 1 ) && ( i != 0 ) ) {

			startSingleTime = ZMin( tsPtr->getSampleTime( 0 ), startSingleTime );
			endSingleTime = ZMax( tsPtr->getSampleTime( 0 ), endSingleTime );

		} else if( ( idx > 0 ) && ( i == 0 ) ) {

			startDefaultTime = tsPtr->getSampleTime( 0 );
			endDefaultTime = tsPtr->getSampleTime( idx-1 );

		}

		if( i == 0 ) {

				 if( tsType.isUniform() ) { _timeSamplingType = 0; }
			else if( tsType.isCyclic()  ) { _timeSamplingType = 1; }
			else if( tsType.isAcyclic() ) { _timeSamplingType = 2; }

		} else if( i == 1 ) {

			// only valid for uniform time sampling scheme.
			_timeStepSize = tsType.getTimePerCycle();

		}
	}

	// if we had a valid animated start and end time we will use only that
	if( ( startTime != DBL_MAX ) && ( endTime != -DBL_MAX ) )
	{
		_startTime = startTime;
		_endTime   = endTime;
	}
	// this is for the cases where we have custom time samplings but
	// they only have 1 sample
	else if( ( startSingleTime != DBL_MAX ) && ( endSingleTime != -DBL_MAX ) )
	{
		_startTime = startSingleTime;
		_endTime   = endSingleTime;
	}
	// no other valid time was yet to be found, but we found a valid
	// default time with at least one sample, so we will use it.
	else if( ( startDefaultTime != DBL_MAX ) && ( endDefaultTime != -DBL_MAX ) )
	{
		_startTime = startDefaultTime;
		_endTime   = endDefaultTime;
	}
	else
	{
		_startTime = DBL_MAX;
		_endTime   = -DBL_MAX;
	}

 	return true;
}

bool
ZAlembicArchive::opened() const
{
	return _opened;
}

void
ZAlembicArchive::close()
{
	ZAlembicArchive::reset();
}

bool
ZAlembicArchive::getMetaData( ZStringArray& keys, ZStringArray& values ) const
{
	const Alembic::Abc::MetaData md = _archivePtr->getMetaData();

	return GetMetaData( md, keys, values );
}

const ZAlembicObject&
ZAlembicArchive::topObject() const
{
	return _topObj;
}

ZString
ZAlembicArchive::archiveTypeStr() const
{
	if( _archiveType == 1 ) { return "HDF5";  }
	if( _archiveType == 2 ) { return "Ogawa"; }

	return "none";
}

int
ZAlembicArchive::archiveType() const
{
	return _archiveType;
}

ZString
ZAlembicArchive::filePathName() const
{
	return _filePathName;
}

int
ZAlembicArchive::abcVersion() const
{
	return _abcVersion;
}

ZString
ZAlembicArchive::abcVersionStr() const
{
	return _abcVersionStr;
}

ZString
ZAlembicArchive::appName() const
{
	return _appName;
}

ZString
ZAlembicArchive::writtenDate() const
{
	return _writtenDate;
}

ZString
ZAlembicArchive::description() const
{
	return _description;
}

int
ZAlembicArchive::timeSamplingTypeId() const
{
	return _timeSamplingType;
}

ZString
ZAlembicArchive::timeSamplingTypeStr() const
{
	if( _timeSamplingType == 1 ) { return "uniform"; }
	if( _timeSamplingType == 2 ) { return "cyclic";  }
	if( _timeSamplingType == 3 ) { return "acyclic"; }

	return "static";
}

int
ZAlembicArchive::numTimeSamples() const
{
	return _numTimeSamples;
}

double
ZAlembicArchive::startTime() const
{
	return _startTime;
}

double
ZAlembicArchive::endTime() const
{
	return _endTime;
}

double
ZAlembicArchive::timeStepSize() const
{
	return _timeStepSize;
}

ostream&
operator<<( ostream& os, const ZAlembicArchive& object )
{
	os << "<ZAlembicArchive>" << endl;
	os << " File Path Name  : " << object.filePathName() << endl;
	os << " Archive Type    : " << object.archiveTypeStr() << endl;
	os << " Alembic Version : " << object.abcVersionStr() << " (" << object.abcVersion() << ")" << endl;
	os << " Application Name: " << object.appName() << endl;
	os << " Written Date    : " << object.writtenDate() << endl;
	os << " Description     : " << object.description() << endl;
	os << " Time Range      : " << object.startTime() << " ~ " << object.endTime() << endl;
	os << endl;

	return os;
}

ZELOS_NAMESPACE_END

