//------------------//
// ZDoubleArray.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.24                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZDoubleArray::ZDoubleArray()
{}

ZDoubleArray::ZDoubleArray( const ZDoubleArray& a )
: ZArray<double>::ZArray( a )
{}

ZDoubleArray::ZDoubleArray( int l )
: ZArray<double>::ZArray( l )
{}

ZDoubleArray::ZDoubleArray( int l, double v )
: ZArray<double>::ZArray( l, v )
{}

void
ZDoubleArray::add( double v, bool useOpenMP )
{
	const int N = (int)parent::size();
	double* data = pointer();

	#pragma omp parallel for if( useOpenMP && N>10000 )
	FOR( i, 0, N )
	{
		data[i] += v;
	}
}

void
ZDoubleArray::setRandomValues( int seed, double min, double max )
{
	const int n = parent::length();

	FOR( i, 0, n )
	{
		double& v = parent::operator[](i);

		v = (double)ZRand( i+seed, min, max );
	}
}

void
ZDoubleArray::multiply( double v, bool useOpenMP )
{
	const int N = (int)parent::size();
	double* data = pointer();

	#pragma omp parallel for if( useOpenMP && N>10000 )
	FOR( i, 0, N )
	{
		data[i] *= v;
	}
}

double
ZDoubleArray::min( bool useOpenMP ) const
{
	double ret = Z_DBLMAX;

	const int N = (int)parent::size();
	if( !N ) { return ret; }

	const double* data = parent::pointer();

	if( useOpenMP ) {

		const int nCPUs = omp_get_num_procs();
		const int nThreads = 2*nCPUs;
		const int nGroups = nThreads;
		int nPerThread = (int)(N/nThreads);
		const int nRemainders = N-(nThreads*nPerThread);
		if( nRemainders > 0 ) { ++nPerThread; }

		ZDoubleArray localMin( nGroups, ret );

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

		std::vector<double>::const_iterator itr;
		for( itr=parent::begin(); itr!=parent::end(); ++itr )
		{
			ret = ZMin( ret, *itr );
		}

	}

	return ret;
}

double
ZDoubleArray::max( bool useOpenMP ) const
{
	double ret = -Z_DBLMAX;

	const int N = (int)parent::size();
	if( !N ) { return ret; }

	const double* data = parent::pointer();

	if( useOpenMP ) {

		const int nCPUs = omp_get_num_procs();
		const int nThreads = 2*nCPUs;
		const int nGroups = nThreads;
		int nPerThread = (int)(N/nThreads);
		const int nRemainders = N-(nThreads*nPerThread);
		if( nRemainders > 0 ) { ++nPerThread; }

		ZDoubleArray localMax( nGroups, ret );

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

		std::vector<double>::const_iterator itr;
		for( itr=parent::begin(); itr!=parent::end(); ++itr )
		{
			ret = ZMax( ret, *itr );
		}

	}

	return ret;
}

double
ZDoubleArray::absMax( bool useOpenMP ) const
{
	double ret = 0.f;

	const int N = (int)parent::size();
	if( !N ) { return ret; }

	const double* data = parent::pointer();

	if( useOpenMP ) {

		const int nCPUs = omp_get_num_procs();
		const int nThreads = 2*nCPUs;
		const int nGroups = nThreads;
		int nPerThread = (int)(N/nThreads);
		const int nRemainders = N-(nThreads*nPerThread);
		if( nRemainders > 0 ) { ++nPerThread; }

		ZDoubleArray localMax( nGroups, ret );

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

		std::vector<double>::const_iterator itr;
		for( itr=parent::begin(); itr!=parent::end(); ++itr )
		{
			ret = ZAbsMax( ret, *itr );
		}

	}

	return ret;
}

void
ZDoubleArray::getMinMax( double& min, double& max, bool useOpenMP ) const
{
	min =  Z_DBLMAX;
	max = -Z_DBLMAX;

	const int N = (int)parent::size();
	if( !N ) { return; }

	const double* data = parent::pointer();

	if( useOpenMP ) {

		const int nCPUs = omp_get_num_procs();
		const int nThreads = 2*nCPUs;
		const int nGroups = nThreads;
		int nPerThread = (int)(N/nThreads);
		const int nRemainders = N-(nThreads*nPerThread);
		if( nRemainders > 0 ) { ++nPerThread; }

		ZDoubleArray localMin( nGroups, min );
		ZDoubleArray localMax( nGroups, max );

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

		std::vector<double>::const_iterator itr;
		for( itr=parent::begin(); itr!=parent::end(); ++itr )
		{
			min = ZMin( min, *itr );
			max = ZMax( max, *itr );
		}

	}
}

void
ZDoubleArray::normalize( bool useOpenMP )
{
	const int N = (int)parent::size();
	if( !N ) { return; }

	const double totalSum = sum( useOpenMP );
	const double denom = 1.0 / totalSum;

	#pragma omp parallel for if( useOpenMP )
	FOR( i, 0, N )
	{
		double& v = parent::operator[](i);

		v *= denom;
	}
}

double
ZDoubleArray::sum( bool useOpenMP ) const
{
	double ret = 0.0;

	const int N = (int)parent::size();
	if( !N ) { return ret; }

	const double* data = pointer();

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

		std::vector<double>::const_iterator itr;
		for( itr=parent::begin(); itr!=parent::end(); ++itr )
		{
			ret += *itr;
		}

	}

	return ret;
}

double
ZDoubleArray::average( bool useOpenMP ) const
{
	double ret = 0.0;

	const int N = (int)parent::size();
	if( !N ) { return (double)ret; }

	const double _N = 1/(double)N;
	const double* data = pointer();

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

		std::vector<double>::const_iterator itr;
		for( itr=parent::begin(); itr!=parent::end(); ++itr )
		{
			ret += (*itr) * _N;
		}

	}

	return (double)ret;
}

void
ZDoubleArray::getAccumulated( ZDoubleArray& accumulated ) const
{
	const int N = (int)parent::size();
	if( !N ) { accumulated.clear(); return; }

	const double* data = pointer();

	accumulated.setLength( N );

	accumulated[0] = data[0];

	FOR( i, 1, N )
	{
		accumulated[i] = data[i] + accumulated[i-1];
	}
}

ostream&
operator<<( ostream& os, const ZDoubleArray& object )
{
	os << "<ZDoubleArray>" << endl;
	const int size     = object.size();
	const int capacity = object.capacity();

	double bytes = size * sizeof(double);
	os << " size    : " << size << " (";
	     if( bytes <       1024.0  ) { os << ( bytes                 ) << " bytes)" << endl; }
	else if( bytes < ZPow2(1024.0) ) { os << ( bytes /       1024.0  ) << " kb)"    << endl; }
	else if( bytes < ZPow3(1024.0) ) { os << ( bytes / ZPow2(1024.0) ) << " mb)"    << endl; }
	else if( bytes < ZPow4(1024.0) ) { os << ( bytes / ZPow3(1024.0) ) << " gb)"    << endl; }

	bytes = capacity * sizeof(double);
	os << " capacity: " << size << " (";
	     if( bytes <       1024.0  ) { os << ( bytes                 ) << " bytes)" << endl; }
	else if( bytes < ZPow2(1024.0) ) { os << ( bytes /       1024.0  ) << " kb)"    << endl; }
	else if( bytes < ZPow3(1024.0) ) { os << ( bytes / ZPow2(1024.0) ) << " mb)"    << endl; }
	else if( bytes < ZPow4(1024.0) ) { os << ( bytes / ZPow3(1024.0) ) << " gb)"    << endl; }

	os << endl;
	return os;
}

ZELOS_NAMESPACE_END

