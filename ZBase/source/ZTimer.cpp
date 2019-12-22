//------------//
// ZTimer.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZTimer::ZTimer()
{
	start();
}

ZTimer::ZTimer( const ZTimer& timer )
{
	*this = timer;
}

ZTimer&
ZTimer::operator=( const ZTimer& timer )
{
	#ifdef OS_LINUX
		_start   = timer._start;
		_end     = timer._end;
		_elapsed = timer._elapsed;
	#endif

	#ifdef OS_WINDOWS
		_start   = timer._start;
		_end     = timer._end;
		_elapsed = timer._elapsed;
		_freq    = timer._freq;
	#endif

	return (*this);
}

void
ZTimer::start()
{
	#ifdef OS_LINUX
		gettimeofday( &_start, 0 );
	#endif

	#ifdef OS_WINDOWS
		QueryPerformanceFrequency( reinterpret_cast<LARGE_INTEGER*>(&_freq) );
		QueryPerformanceCounter( reinterpret_cast<LARGE_INTEGER*>(&_start) );
	#endif
}

float
ZTimer::check( bool asMilliSecond )
{
	#ifdef OS_LINUX
		gettimeofday( &_end, 0 );
		timersub( &_end, &_start, &_elapsed );
		if( asMilliSecond ) { // as millisecond (1/1,000sec.)
			return (float)((1e+3)*_elapsed.tv_sec+(1e-3)*_elapsed.tv_usec);
		} else { // as second
			return (float)(_elapsed.tv_sec+(1e-6)*_elapsed.tv_usec);
		}
	#endif

	#ifdef OS_WINDOWS
		QueryPerformanceCounter( reinterpret_cast<LARGE_INTEGER*>(&_end) );
		if( asMilliSecond ) { // as millisecond (1/1,000sec.)
			return (float)(1000*((_end.QuadPart-_start.QuadPart)/_freq.QuadPart));
		} else { // as second
			return (float)((_end.QuadPart-_start.QuadPart)/_freq.QuadPart);
		}
	#endif
}

float
ZTimer::stop( bool asMilliSecond )
{
	#ifdef OS_LINUX
		float elapsedTime = check( asMilliSecond );
		_start = _end;
		return elapsedTime;
	#endif

	#ifdef OS_WINDOWS
		float elapsedTime = check( asMilliSecond );
		_start = _end;
		return elapsedTime;
	#endif
}

ostream&
operator<<( ostream& os, const ZTimer& object )
{
	os << "<ZTimer>" << endl;
	return os;
}

ZELOS_NAMESPACE_END

