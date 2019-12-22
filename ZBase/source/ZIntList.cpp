//--------------//
// ZIntList.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZIntList::ZIntList()
{}

ZIntList::ZIntList( const ZIntList& l )
: ZList<int>::ZList( l )
{}

int
ZIntList::min() const
{
	int ret = Z_INTMAX;
	std::list<int>::const_iterator itr = parent::begin();
	for( ; itr!=parent::end(); ++itr )
	{
		ret = ZMin( ret, *itr );
	}
	return ret;
}

int
ZIntList::max() const
{
	int ret = -Z_INTMAX;
	std::list<int>::const_iterator itr = parent::begin();
	for( ; itr!=parent::end(); ++itr )
	{
		ret = ZMax( ret, *itr );
	}
	return ret;
}

int
ZIntList::absMax() const
{
	int ret = 0;
	std::list<int>::const_iterator itr = parent::begin();
	for( ; itr!=parent::end(); ++itr )
	{
		ret = ZAbsMax( ret, *itr );
	}
	return ret;
}

ostream&
operator<<( ostream& os, const ZIntList& object )
{
	os << "<ZIntList>" << endl;
	os << " data type: " << object.dataType() << endl;
	os << " size     : " << object.size() << endl;
	os << " memory   : " << ZString::commify(object.size()*sizeof(int)) << " bytes" << endl;
	os << endl;

	return os;
}

ZELOS_NAMESPACE_END

