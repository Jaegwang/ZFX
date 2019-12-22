//----------------//
// ZFloatList.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZFloatList::ZFloatList()
{}

ZFloatList::ZFloatList( const ZFloatList& l )
: ZList<float>::ZList( l )
{}

float
ZFloatList::min() const
{
	float ret = Z_FLTMAX;
	std::list<float>::const_iterator itr = parent::begin();
	for( ; itr!=parent::end(); ++itr )
	{
		ret = ZMin( ret, *itr );
	}
	return ret;
}

float
ZFloatList::max() const
{
	float ret = -Z_FLTMAX;
	std::list<float>::const_iterator itr = parent::begin();
	for( ; itr!=parent::end(); ++itr )
	{
		ret = ZMax( ret, *itr );
	}
	return ret;
}

float
ZFloatList::absMax() const
{
	float ret = 0;
	std::list<float>::const_iterator itr = parent::begin();
	for( ; itr!=parent::end(); ++itr )
	{
		ret = ZAbsMax( ret, *itr );
	}
	return ret;
}

ostream&
operator<<( ostream& os, const ZFloatList& object )
{
	os << "<ZFloatList>" << endl;
	os << " data type: " << object.dataType() << endl;
	os << " size     : " << object.size() << endl;
	os << " memory   : " << ZString::commify(object.size()*sizeof(float)) << " bytes" << endl;
	os << endl;

	return os;
}

ZELOS_NAMESPACE_END

