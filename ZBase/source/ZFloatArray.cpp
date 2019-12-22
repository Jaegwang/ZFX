//-----------------//
// ZFloatArray.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2019.03.26                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZFloatArray::ZFloatArray()
{}

ZFloatArray::ZFloatArray( const ZFloatArray& a )
: parent::ZArray( a )
{}

ZFloatArray::ZFloatArray( int l )
: parent::ZArray( l )
{}

ZFloatArray::ZFloatArray( int l, float v )
: parent::ZArray( l, v )
{}

void
ZFloatArray::add( float v, bool useOpenMP )
{
	const int N = (int)parent::size();
	float* data = parent::pointer();

	#pragma omp parallel for if( useOpenMP && N>10000 )
	FOR( i, 0, N )
	{
		data[i] += v;
	}
}

void
ZFloatArray::multiply( float v, bool useOpenMP )
{
	const int N = (int)parent::size();
	float* data = parent::pointer();

	#pragma omp parallel for if( useOpenMP && N>10000 )
	FOR( i, 0, N )
	{
		data[i] *= v;
	}
}

void
ZFloatArray::setRandomValues( int seed, float min, float max )
{
	const int n = parent::length();

	FOR( i, 0, n )
	{
		float& v = parent::operator[](i);

		v = ZRand( i+seed, min, max );
	}
}

int
ZFloatArray::minIndex() const
{
	const int N = (int)parent::size();
	if(N<=0) {return -1;}
	const float* a = (const float*)parent::pointer();
    int idx=-1; float min=Z_LARGE;
    FOR(i,0,N) { if(a[i]<min){min=a[i]; idx=i;} }
    return idx;
}

int
ZFloatArray::maxIndex() const
{
	const int N = (int)parent::size();
	if(N<=0) {return -1;}
	const float* a = (const float*)parent::pointer();
    int idx=-1; float max=-Z_LARGE;
    FOR(i,0,N) { if(a[i]>max){max=a[i]; idx=i;} }
    return idx;
}

float
ZFloatArray::min( bool useOpenMP ) const
{
	float ret = Z_FLTMAX;

	const int N = (int)parent::size();
	if( !N ) { return ret; }

	const float* data = parent::pointer();

	if( useOpenMP ) {

		const int nCPUs = omp_get_num_procs();
		const int nThreads = 2*nCPUs;
		const int nGroups = nThreads;
		int nPerThread = (int)(N/nThreads);
		const int nRemainders = N-(nThreads*nPerThread);
		if( nRemainders > 0 ) { ++nPerThread; }

		ZFloatArray localMin( nGroups, ret );

		#pragma omp parallel for
		FOR( i, 0, nGroups )
		{
			const int startIdx = i*nPerThread;
			const int endIdx   = ZMin( (i+1)*nPerThread, N );

			FOR( j, startIdx, endIdx )
			{
				localMin[i] = ZMin( localMin[i], data[j] );
			}
		}

		ret = localMin.min();

	} else {

		std::vector<float>::const_iterator itr;
		for( itr=parent::begin(); itr!=parent::end(); ++itr )
		{
			ret = ZMin( ret, *itr );
		}

	}

	return ret;
}

float
ZFloatArray::max( bool useOpenMP ) const
{
	float ret = -Z_FLTMAX;

	const int N = (int)parent::size();
	if( !N ) { return ret; }

	const float* data = parent::pointer();

	if( useOpenMP ) {

		const int nCPUs = omp_get_num_procs();
		const int nThreads = 2*nCPUs;
		const int nGroups = nThreads;
		int nPerThread = (int)(N/nThreads);
		const int nRemainders = N-(nThreads*nPerThread);
		if( nRemainders > 0 ) { ++nPerThread; }

		ZFloatArray localMax( nGroups, ret );

		#pragma omp parallel for
		FOR( i, 0, nGroups )
		{
			const int startIdx = i*nPerThread;
			const int endIdx   = ZMin( (i+1)*nPerThread, N );

			FOR( j, startIdx, endIdx )
			{
				localMax[i] = ZMax( localMax[i], data[j] );
			}
		}

		ret = localMax.max();

	} else {

		std::vector<float>::const_iterator itr;
		for( itr=parent::begin(); itr!=parent::end(); ++itr )
		{
			ret = ZMax( ret, *itr );
		}

	}

	return ret;
}

float
ZFloatArray::absMax( bool useOpenMP ) const
{
	float ret = 0.f;

	const int N = (int)parent::size();
	if( !N ) { return ret; }

	const float* data = parent::pointer();

	if( useOpenMP ) {

		const int nCPUs = omp_get_num_procs();
		const int nThreads = 2*nCPUs;
		const int nGroups = nThreads;
		int nPerThread = (int)(N/nThreads);
		const int nRemainders = N-(nThreads*nPerThread);
		if( nRemainders > 0 ) { ++nPerThread; }

		ZFloatArray localMax( nGroups, ret );

		#pragma omp parallel for
		FOR( i, 0, nGroups )
		{
			const int startIdx = i*nPerThread;
			const int endIdx   = ZMin( (i+1)*nPerThread, N );

			FOR( j, startIdx, endIdx )
			{
				localMax[i] = ZAbsMax( localMax[i], data[j] );
			}
		}

		ret = localMax.max();

	} else {

		std::vector<float>::const_iterator itr;
		for( itr=parent::begin(); itr!=parent::end(); ++itr )
		{
			ret = ZAbsMax( ret, *itr );
		}

	}

	return ret;
}

void
ZFloatArray::normalize( bool useOpenMP )
{
	const int N = (int)parent::size();
	if( !N ) { return; }

	const double totalSum = sum( useOpenMP );
	const double denom = (float)( 1.0 / totalSum );

	#pragma omp parallel for if( useOpenMP )
	FOR( i, 0, N )
	{
		float& v = parent::operator[](i);

		v *= denom;
	}
}

void
ZFloatArray::getMinMax( float& min, float& max, bool useOpenMP ) const
{
	min =  Z_FLTMAX;
	max = -Z_FLTMAX;

	const int N = (int)parent::size();
	if( !N ) { return; }

	const float* data = parent::pointer();

	if( useOpenMP ) {

		const int nCPUs = omp_get_num_procs();
		const int nThreads = 2*nCPUs;
		const int nGroups = nThreads;
		int nPerThread = (int)(N/nThreads);
		const int nRemainders = N-(nThreads*nPerThread);
		if( nRemainders > 0 ) { ++nPerThread; }

		ZFloatArray localMin( nGroups, min );
		ZFloatArray localMax( nGroups, max );

		#pragma omp parallel for
		FOR( i, 0, nGroups )
		{
			const int startIdx = i*nPerThread;
			const int endIdx   = ZMin( (i+1)*nPerThread, N );

			FOR( j, startIdx, endIdx )
			{
				localMin[i] = ZMin( localMin[i], data[j] );
				localMax[i] = ZMax( localMax[i], data[j] );
			}
		}

		min = localMin.min();
		max = localMax.max();

	} else {

		std::vector<float>::const_iterator itr;
		for( itr=parent::begin(); itr!=parent::end(); ++itr )
		{
			min = ZMin( min, *itr );
			max = ZMax( max, *itr );
		}

	}
}

double
ZFloatArray::sum( bool useOpenMP ) const
{
	double ret = 0.0;

	const int N = (int)parent::size();
	if( !N ) { return ret; }

	const float* data = pointer();

	if( useOpenMP ) {

		const int nCPUs = omp_get_num_procs();
		const int nThreads = 2*nCPUs;
		const int nGroups = nThreads;
		int nPerThread = (int)(N/nThreads);
		const int nRemainders = N-(nThreads*nPerThread);
		if( nRemainders > 0 ) { ++nPerThread; }

		ZDoubleArray localSum( nGroups );

		#pragma omp parallel for
		FOR( i, 0, nGroups )
		{
			const int startIdx = i*nPerThread;
			const int endIdx   = ZMin( (i+1)*nPerThread, N );

			FOR( j, startIdx, endIdx )
			{
				localSum[i] += data[j];
			}
		}

		ret = localSum.sum();

	} else {

		std::vector<float>::const_iterator itr;
		for( itr=parent::begin(); itr!=parent::end(); ++itr )
		{
			ret += *itr;
		}

	}

	return ret;
}

double
ZFloatArray::average( bool useOpenMP ) const
{
	double ret = 0.0;

	const int N = (int)parent::size();
	if( !N ) { return ret; }

	const double _N = 1/(double)N;
	const float* data = pointer();

	if( useOpenMP ) {

		const int nCPUs = omp_get_num_procs();
		const int nThreads = 2*nCPUs;
		const int nGroups = nThreads;
		int nPerThread = (int)(N/nThreads);
		const int nRemainders = N-(nThreads*nPerThread);
		if( nRemainders > 0 ) { ++nPerThread; }

		ZDoubleArray localAvg( nGroups );

		#pragma omp parallel for
		FOR( i, 0, nGroups )
		{
			const int startIdx = i*nPerThread;
			const int endIdx   = ZMin( (i+1)*nPerThread, N );

			FOR( j, startIdx, endIdx )
			{
				localAvg[i] += data[j] * _N;
			}
		}

		ret += localAvg.sum();

	} else {

		std::vector<float>::const_iterator itr;
		for( itr=parent::begin(); itr!=parent::end(); ++itr )
		{
			ret += (*itr) * _N;
		}

	}

	return ret;
}

void
ZFloatArray::getAccumulated( ZFloatArray& accumulated ) const
{
	const int N = (int)parent::size();
	if( !N ) { accumulated.clear(); return; }

	const float* data = pointer();

	accumulated.setLength( N );

	accumulated[0] = data[0];

	FOR( i, 1, N )
	{
		accumulated[i] = data[i] + accumulated[i-1];
	}
}

ostream&
operator<<( ostream& os, const ZFloatArray& object )
{
	os << "<ZFloatArray>" << endl;
	const int size     = object.size();
	const int capacity = object.capacity();

	double bytes = size * sizeof(float);
	os << " size    : " << size << " (";
	     if( bytes <       1024.0  ) { os << ( bytes                 ) << " bytes)" << endl; }
	else if( bytes < ZPow2(1024.0) ) { os << ( bytes /       1024.0  ) << " kb)"    << endl; }
	else if( bytes < ZPow3(1024.0) ) { os << ( bytes / ZPow2(1024.0) ) << " mb)"    << endl; }
	else if( bytes < ZPow4(1024.0) ) { os << ( bytes / ZPow3(1024.0) ) << " gb)"    << endl; }

	bytes = capacity * sizeof(float);
	os << " capacity: " << size << " (";
	     if( bytes <       1024.0  ) { os << ( bytes                 ) << " bytes)" << endl; }
	else if( bytes < ZPow2(1024.0) ) { os << ( bytes /       1024.0  ) << " kb)"    << endl; }
	else if( bytes < ZPow3(1024.0) ) { os << ( bytes / ZPow2(1024.0) ) << " mb)"    << endl; }
	else if( bytes < ZPow4(1024.0) ) { os << ( bytes / ZPow3(1024.0) ) << " gb)"    << endl; }

	os << endl;
	return os;
}

ZELOS_NAMESPACE_END

