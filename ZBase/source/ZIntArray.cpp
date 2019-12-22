//---------------//
// ZIntArray.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2018.03.08                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZIntArray::ZIntArray()
{}

ZIntArray::ZIntArray( const ZIntArray& a )
: ZArray<int>::ZArray( a )
{}

ZIntArray::ZIntArray( int l )
: ZArray<int>::ZArray( l )
{}

ZIntArray::ZIntArray( int l, int v )
: ZArray<int>::ZArray( l, v )
{}

void
ZIntArray::serialize( int startIndex )
{
	const int N = (int)parent::size();
	if( !N ) { return; }

	FOR( i, 0, N )
	{
		parent::at(i) = startIndex + i;
	}
}

void
ZIntArray::setRandomValues( int seed, int min, int max )
{
	const int n = parent::length();

	ZRandSeed( seed );

	FOR( i, 0, n )
	{
		int& v = parent::operator[](i);

		v = ZRandInt1( i );
	}
}

void
ZIntArray::add( int numberToBeAdded )
{
	const int n = parent::length();

	FOR( i, 0, n )
	{
		int& v = parent::operator[](i);

		v += numberToBeAdded;
	}
}

int
ZIntArray::min( bool useOpenMP ) const
{
	int ret = Z_INTMAX;

	const int N = (int)parent::size();
	if( !N ) { return ret; }

	const int* data = parent::pointer();

	if( useOpenMP ) {

		const int nCPUs = omp_get_num_procs();
		const int nThreads = 2*nCPUs;
		const int nGroups = nThreads;
		int nPerThread = (int)(N/nThreads);
		const int nRemainders = N-(nThreads*nPerThread);
		if( nRemainders > 0 ) { ++nPerThread; }

		ZIntArray localMin( nGroups, ret );

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

		std::vector<int>::const_iterator itr;
		for( itr=parent::begin(); itr!=parent::end(); ++itr )
		{
			ret = ZMin( ret, *itr );
		}

	}

	return ret;
}

int
ZIntArray::max( bool useOpenMP ) const
{
	int ret = -Z_INTMAX;

	const int N = (int)parent::size();
	if( !N ) { return ret; }

	const int* data = parent::pointer();

	if( useOpenMP ) {

		const int nCPUs = omp_get_num_procs();
		const int nThreads = 2*nCPUs;
		const int nGroups = nThreads;
		int nPerThread = (int)(N/nThreads);
		const int nRemainders = N-(nThreads*nPerThread);
		if( nRemainders > 0 ) { ++nPerThread; }

		ZIntArray localMax( nGroups, ret );

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

		std::vector<int>::const_iterator itr;
		for( itr=parent::begin(); itr!=parent::end(); ++itr )
		{
			ret = ZMax( ret, *itr );
		}

	}

	return ret;
}

int
ZIntArray::absMax( bool useOpenMP ) const
{
	int ret = 0.f;

	const int N = (int)parent::size();
	if( !N ) { return ret; }

	const int* data = parent::pointer();

	if( useOpenMP ) {

		const int nCPUs = omp_get_num_procs();
		const int nThreads = 2*nCPUs;
		const int nGroups = nThreads;
		int nPerThread = (int)(N/nThreads);
		const int nRemainders = N-(nThreads*nPerThread);
		if( nRemainders > 0 ) { ++nPerThread; }

		ZIntArray localMax( nGroups, ret );

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

		std::vector<int>::const_iterator itr;
		for( itr=parent::begin(); itr!=parent::end(); ++itr )
		{
			ret = ZAbsMax( ret, *itr );
		}

	}

	return ret;
}

void
ZIntArray::getMinMax( int& min, int& max, bool useOpenMP ) const
{
	min =  Z_INTMAX;
	max = -Z_INTMAX;

	const int N = (int)parent::size();
	if( !N ) { return; }

	const int* data = parent::pointer();

	if( useOpenMP ) {

		const int nCPUs = omp_get_num_procs();
		const int nThreads = 2*nCPUs;
		const int nGroups = nThreads;
		int nPerThread = (int)(N/nThreads);
		const int nRemainders = N-(nThreads*nPerThread);
		if( nRemainders > 0 ) { ++nPerThread; }

		ZIntArray localMin( nGroups, min );
		ZIntArray localMax( nGroups, max );

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

		std::vector<int>::const_iterator itr;
		for( itr=parent::begin(); itr!=parent::end(); ++itr )
		{
			min = ZMin( min, *itr );
			max = ZMax( max, *itr );
		}

	}
}

double
ZIntArray::sum( bool useOpenMP ) const
{
	double ret = 0.0;

	const int N = (int)parent::size();
	if( !N ) { return ret; }

	const int* data = pointer();

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

		std::vector<int>::const_iterator itr;
		for( itr=parent::begin(); itr!=parent::end(); ++itr )
		{
			ret += *itr;
		}

	}

	return ret;
}

double
ZIntArray::average( bool useOpenMP ) const
{
	double ret = 0.0;

	const int N = (int)parent::size();
	if( !N ) { return (int)ret; }

	const double _N = 1/(double)N;
	const int* data = pointer();

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

		std::vector<int>::const_iterator itr;
		for( itr=parent::begin(); itr!=parent::end(); ++itr )
		{
			ret += (*itr) * _N;
		}

	}

	return ret;
}

void
ZIntArray::getAccumulated( ZIntArray& accumulated ) const
{
	const int N = (int)parent::size();
	if( !N ) { accumulated.clear(); return; }

	const int* data = pointer();

	accumulated.setLength( N );

	accumulated[0] = data[0];

	FOR( i, 1, N )
	{
		accumulated[i] = data[i] + accumulated[i-1];
	}
}

void
ZIntArray::setMask( const ZIntArray& indices, bool value )
{
	const int len0 = (int)parent::size();
	const int len1 = indices.length();

	if( value ) { // mark true

		parent::zeroize();

		FOR( i, 0, len1 )
		{
			const int& idx = indices[i];
			if( idx < 0 || idx >= len0 ) { continue; }

			parent::at(idx) = 1;
		}

	} else { // mark false

		parent::fill( 1 );

		FOR( i, 0, len1 )
		{
			const int& idx = indices[i];
			if( idx < 0 || idx >= len0 ) { continue; }

			parent::at(idx) = 0;
		}

	}
}

ostream&
operator<<( ostream& os, const ZIntArray& object )
{
	os << "<ZIntArray>" << endl;
	const int size     = object.size();
	const int capacity = object.capacity();

	double bytes = size * sizeof(int);
	os << " size    : " << size << " (";
	     if( bytes <       1024.0  ) { os << ( bytes                 ) << " bytes)" << endl; }
	else if( bytes < ZPow2(1024.0) ) { os << ( bytes /       1024.0  ) << " kb)"    << endl; }
	else if( bytes < ZPow3(1024.0) ) { os << ( bytes / ZPow2(1024.0) ) << " mb)"    << endl; }
	else if( bytes < ZPow4(1024.0) ) { os << ( bytes / ZPow3(1024.0) ) << " gb)"    << endl; }

	bytes = capacity * sizeof(int);
	os << " capacity: " << size << " (";
	     if( bytes <       1024.0  ) { os << ( bytes                 ) << " bytes)" << endl; }
	else if( bytes < ZPow2(1024.0) ) { os << ( bytes /       1024.0  ) << " kb)"    << endl; }
	else if( bytes < ZPow3(1024.0) ) { os << ( bytes / ZPow2(1024.0) ) << " mb)"    << endl; }
	else if( bytes < ZPow4(1024.0) ) { os << ( bytes / ZPow3(1024.0) ) << " gb)"    << endl; }

	os << endl;
	return os;
}

ZELOS_NAMESPACE_END

