//-----------------//
// ZDoubleList.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZDoubleList::ZDoubleList()
{}

ZDoubleList::ZDoubleList( const ZDoubleList& a )
: ZList<double>::ZList( a )
{}

double
ZDoubleList::min() const
{
	double ret = Z_DBLMAX;
	std::list<double>::const_iterator itr = parent::begin();
	for( ; itr!=parent::end(); ++itr )
	{
		ret = ZMin( ret, *itr );
	}
	return ret;
}

double
ZDoubleList::max() const
{
	double ret = -Z_DBLMAX;
	std::list<double>::const_iterator itr = parent::begin();
	for( ; itr!=parent::end(); ++itr )
	{
		ret = ZMax( ret, *itr );
	}
	return ret;
}

double
ZDoubleList::absMax() const
{
	double ret = 0;
	std::list<double>::const_iterator itr = parent::begin();
	for( ; itr!=parent::end(); ++itr )
	{
		ret = ZAbsMax( ret, *itr );
	}
	return ret;
}

ostream&
operator<<( ostream& os, const ZDoubleList& object )
{
	os << "<ZDoubleList>" << endl;
	os << " size  : " << object.size() << endl;
	os << " memory: " << ZString::commify(object.size()*sizeof(double)) << " bytes" << endl;
	os << endl;

	return os;
}

ZELOS_NAMESPACE_END

