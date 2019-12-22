//------------------//
// ZParticleSet.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
//         Jaegwang Lim @ Dexter Studios                 //
// last update: 2017.06.01                               //
//-------------------------------------------------------//

#include <ZelosBase.h>
#include <atomic>

ZELOS_NAMESPACE_BEGIN

ZParticleSet::ZParticleSet()
{
}

ostream&
operator<<( ostream& os, const ZParticleSet& ptc )
{
	os << endl;

	return os;
}

ZELOS_NAMESPACE_END

