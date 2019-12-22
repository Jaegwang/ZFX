//----------------------//
// ZAlembicProperty.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2017.08.22                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZAlembicProperty::ZAlembicProperty()
{
	ZAlembicProperty::reset();
}

ZAlembicProperty::ZAlembicProperty( const ZAlembicProperty& prp )
{
	this->operator=( prp );
}

void
ZAlembicProperty::reset()
{
	_cmpPrp      = Alembic::Abc::ICompoundProperty();
	_header      = Alembic::AbcCoreAbstract::PropertyHeader();

	_name        = "";
	_typeId      = 0;
	_numChildren = 0;
	_dataTypeId  = 0;
	_extent      = 0;

	_objectPtr   = (ZAlembicObject*)NULL;
}

void
ZAlembicProperty::setAsTop( const ZAlembicObject& ownerObject )
{
	const Alembic::Abc::IObject& object = ownerObject.object();

	_cmpPrp      = object.getProperties();
	_header      = Alembic::AbcCoreAbstract::PropertyHeader();

	_name        = "topProperty";
	_typeId      = 1; // compound type
	_numChildren = (int)_cmpPrp.getNumProperties();
	_dataTypeId  = -1;
	_extent      = 0;

	_objectPtr   = const_cast<ZAlembicObject*>( &ownerObject );
}

bool
ZAlembicProperty::getMetaData( ZStringArray& keys, ZStringArray& values ) const
{
	const Alembic::Abc::MetaData md = _header.getMetaData();

	return GetMetaData( md, keys, values );
}

bool
ZAlembicProperty::getChild( int i, ZAlembicProperty& childProperty ) const
{
	childProperty.reset();

	if( i < 0 || i >= _numChildren )
	{
		cout << "Error@ZAlembicProperty::getChild(): Invalid index." << endl;
		return false;
	}

	Alembic::Abc::ICompoundProperty& cCmpPrp = childProperty._cmpPrp;
	Alembic::AbcCoreAbstract::PropertyHeader& cHeader      = childProperty._header;

	ZString&           cName        = childProperty._name;
	int&               cTypeId      = childProperty._typeId;
	int&               cNumChildren = childProperty._numChildren;
	int&               cDataTypeId  = childProperty._dataTypeId;
	int&               cExtent      = childProperty._extent;
	ZAlembicObject*&   cObjectPtr   = childProperty._objectPtr;

	cCmpPrp = _cmpPrp;
	cHeader = _cmpPrp.getPropertyHeader( i );
	cName   = cHeader.getName();

	switch( cHeader.getPropertyType() )
	{
		case Alembic::Abc::kCompoundProperty: { cTypeId = 1; break; }
		case Alembic::Abc::kScalarProperty:   { cTypeId = 2; break; }
		case Alembic::Abc::kArrayProperty:    { cTypeId = 3; break; }
		defaul:                               { cTypeId = 0; break; }
	}

	if( cTypeId == 1 ) { // if compound type

		cCmpPrp = Alembic::Abc::ICompoundProperty( _cmpPrp, cName );

		cNumChildren = (int)cCmpPrp.getNumProperties();
		cDataTypeId  = -1;
		cExtent      = 0;

	} else {

		cNumChildren = 0;
		cDataTypeId  = (int)cHeader.getDataType().getPod();
		cExtent      = (int)cHeader.getDataType().getExtent();

	}

	cObjectPtr = _objectPtr;

	return true;
}

ZAlembicObject&
ZAlembicProperty::ownerObject()
{
	return (*_objectPtr);
}

const ZAlembicObject&
ZAlembicProperty::ownerObject() const
{
	return (*_objectPtr);
}

ZAlembicProperty&
ZAlembicProperty::operator=( const ZAlembicProperty& prp )
{
	_cmpPrp      = prp._cmpPrp;
	_header      = prp._header;

	_name        = prp._name;
	_typeId      = prp._typeId;
	_numChildren = prp._numChildren;
	_dataTypeId  = prp._dataTypeId;
	_extent      = prp._extent;

	_objectPtr   = prp._objectPtr;

	return (*this);
}

ZString
ZAlembicProperty::name() const
{
	return _name;
}

int
ZAlembicProperty::typeId() const
{
	return _typeId;
}

ZString
ZAlembicProperty::typeStr() const
{
	if( _typeId == 1 ) { return "compound";      }
	if( _typeId == 2 ) { return "simple_scalar"; }
	if( _typeId == 3 ) { return "simple_array";  }

	return "none";
}

int
ZAlembicProperty::numChildren() const
{
	return _numChildren;
}

int
ZAlembicProperty::dataTypeId() const
{
	return _dataTypeId;
}

// See "/usr/local/include/Alembic/Util/PlainOldDataType.".
ZString
ZAlembicProperty::dataTypeStr() const
{
	if( _dataTypeId == (int)Alembic::Abc::kBooleanPOD           ) { return "bool";                }
	if( _dataTypeId == (int)Alembic::Abc::kInt8POD              ) { return "char";                }
	if( _dataTypeId == (int)Alembic::Abc::kUint8POD             ) { return "unsigned char";       }
	if( _dataTypeId == (int)Alembic::Abc::kInt16POD             ) { return "short";               }
	if( _dataTypeId == (int)Alembic::Abc::kUint16POD            ) { return "unsigned short";      }
	if( _dataTypeId == (int)Alembic::Abc::kInt32POD             ) { return "int";                 }
	if( _dataTypeId == (int)Alembic::Abc::kUint32POD            ) { return "unsigned int";        }
	if( _dataTypeId == (int)Alembic::Abc::kInt64POD             ) { return "long";                }
	if( _dataTypeId == (int)Alembic::Abc::kUint64POD            ) { return "unsigned long";       }
	if( _dataTypeId == (int)Alembic::Abc::kFloat16POD           ) { return "half";                }
	if( _dataTypeId == (int)Alembic::Abc::kFloat32POD           ) { return "float";               }
	if( _dataTypeId == (int)Alembic::Abc::kFloat64POD           ) { return "double";              }
	if( _dataTypeId == (int)Alembic::Abc::kStringPOD            ) { return "string pointer";      }
	if( _dataTypeId == (int)Alembic::Abc::kWstringPOD           ) { return "wide string pointer"; }
	if( _dataTypeId == (int)Alembic::Abc::kNumPlainOldDataTypes ) { return "number of POD";       }
	if( _dataTypeId == (int)Alembic::Abc::kUnknownPOD           ) { return "unknown";             }

	return "none";
}

int
ZAlembicProperty::extent() const
{
	return _extent;
}

ZString
ZAlembicProperty::valuesAsString( int sampleIndex ) const
{
	ZString str;

	if( _typeId == 0 ) { str = "none"; }

	else if( _typeId == 1 ) { str = "compound"; }

	else if( _typeId == 2 ) { // scalar 

		FOR( i, 0, _extent )
		{
			if( _dataTypeId == (int)Alembic::Abc::kBooleanPOD ) { // bool

				Alembic::Abc::IBoolProperty property( _cmpPrp, _name );
				bool tmp = property.getValue( Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );
				str += tmp ? "true" : "false";

			} else if( _dataTypeId == (int)Alembic::Abc::kInt8POD ) { // char

				Alembic::Abc::ICharProperty property( _cmpPrp, _name );
				str += property.getValue( Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

			} else if( _dataTypeId == (int)Alembic::Abc::kUint8POD ) { // unsigned char

				Alembic::Abc::IUcharProperty property( _cmpPrp, _name );
				str += property.getValue( Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

			} else if( _dataTypeId == (int)Alembic::Abc::kInt16POD ) { // short

				Alembic::Abc::IInt16Property property( _cmpPrp, _name );
				str += property.getValue( Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

			} else if( _dataTypeId == (int)Alembic::Abc::kUint16POD ) { // unsigned short

				Alembic::Abc::IUInt16Property property( _cmpPrp, _name );
				str += property.getValue( Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

			} else if( _dataTypeId == (int)Alembic::Abc::kInt32POD ) { // int

				Alembic::Abc::IInt32Property property( _cmpPrp, _name );
				str += property.getValue( Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

			} else if( _dataTypeId == (int)Alembic::Abc::kUint32POD ) { // unsigned int

				Alembic::Abc::IUInt32Property property( _cmpPrp, _name );
				str += property.getValue( Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

			} else if( _dataTypeId == (int)Alembic::Abc::kInt64POD ) { // long int

				Alembic::Abc::IInt64Property property( _cmpPrp, _name );
				str += property.getValue( Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

			} else if( _dataTypeId == (int)Alembic::Abc::kUint64POD ) { // unsigned long int

				Alembic::Abc::IUInt64Property property( _cmpPrp, _name );
				str += property.getValue( Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

			} else if( _dataTypeId == (int)Alembic::Abc::kFloat16POD ) { // half

				Alembic::Abc::IHalfProperty property( _cmpPrp, _name );
				str += property.getValue( Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

			} else if( _dataTypeId == (int)Alembic::Abc::kFloat32POD ) { // float

				Alembic::Abc::IFloatProperty property( _cmpPrp, _name );
				str += property.getValue( Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

			} else if( _dataTypeId == (int)Alembic::Abc::kFloat64POD ) { // double

				Alembic::Abc::IDoubleProperty property( _cmpPrp, _name );
				str += property.getValue( Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

			} else if( _dataTypeId == (int)Alembic::Abc::kStringPOD ) { // string

				Alembic::Abc::IStringArrayProperty property( _cmpPrp, _name );
				str += property.getValue( Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

			} else {

				str += "not supported data type";

			}

			if( i != (_extent-1) )
			{
				str += " ";
			}
		}

	} else if( _typeId == 3 ) { // array

		if( _extent == 1 ) {

			if( _dataTypeId == (int)Alembic::Abc::kBooleanPOD ) { // bool

				Alembic::Abc::BoolArraySamplePtr arrayPtr;
				Alembic::Abc::IBoolArrayProperty property( _cmpPrp, _name );
				property.get( arrayPtr, Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

				const int len = (int)arrayPtr->size();

				if( len ) { str = (*arrayPtr)[0]; }
				else      { str = "empty";        }

				if( len > 1 ) { str += ", ..."; }

			} else if( _dataTypeId == (int)Alembic::Abc::kInt8POD ) { // char

				Alembic::Abc::CharArraySamplePtr arrayPtr;
				Alembic::Abc::ICharArrayProperty property( _cmpPrp, _name );
				property.get( arrayPtr, Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

				const int len = (int)arrayPtr->size();

				if( len ) { str = (*arrayPtr)[0]; }
				else      { str = "empty";        }

				if( len > 1 ) { str += ", ..."; }

			} else if( _dataTypeId == (int)Alembic::Abc::kUint8POD ) { // unsigned char

				Alembic::Abc::UcharArraySamplePtr arrayPtr;
				Alembic::Abc::IUcharArrayProperty property( _cmpPrp, _name );
				property.get( arrayPtr, Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

				const int len = (int)arrayPtr->size();

				if( len ) { str = (*arrayPtr)[0]; }
				else      { str = "empty";        }

				if( len > 1 ) { str += ", ..."; }

			} else if( _dataTypeId == (int)Alembic::Abc::kInt16POD ) { // short

				Alembic::Abc::Int16ArraySamplePtr arrayPtr;
				Alembic::Abc::IInt16ArrayProperty property( _cmpPrp, _name );
				property.get( arrayPtr, Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

				const int len = (int)arrayPtr->size();

				if( len ) { str = (*arrayPtr)[0]; }
				else      { str = "empty";        }

				if( len > 1 ) { str += ", ..."; }

			} else if( _dataTypeId == (int)Alembic::Abc::kUint16POD ) { // unsigned short

				Alembic::Abc::UInt16ArraySamplePtr arrayPtr;
				Alembic::Abc::IUInt16ArrayProperty property( _cmpPrp, _name );
				property.get( arrayPtr, Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

				const int len = (int)arrayPtr->size();

				if( len ) { str = (*arrayPtr)[0]; }
				else      { str = "empty";        }

				if( len > 1 ) { str += ", ..."; }

			} else if( _dataTypeId == (int)Alembic::Abc::kInt32POD ) { // int

				Alembic::Abc::Int32ArraySamplePtr arrayPtr;
				Alembic::Abc::IInt32ArrayProperty property( _cmpPrp, _name );
				property.get( arrayPtr, Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

				const int len = (int)arrayPtr->size();

				if( len ) { str = (*arrayPtr)[0]; }
				else      { str = "empty";        }

				if( len > 1 ) { str += ", ..."; }

			} else if( _dataTypeId == (int)Alembic::Abc::kUint32POD ) { // unsigned int

				Alembic::Abc::UInt32ArraySamplePtr arrayPtr;
				Alembic::Abc::IUInt32ArrayProperty property( _cmpPrp, _name );
				property.get( arrayPtr, Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

				const int len = (int)arrayPtr->size();

				if( len ) { str = (*arrayPtr)[0]; }
				else      { str = "empty";        }

				if( len > 1 ) { str += ", ..."; }

			} else if( _dataTypeId == (int)Alembic::Abc::kInt64POD ) { // long int

				Alembic::Abc::Int64ArraySamplePtr arrayPtr;
				Alembic::Abc::IInt64ArrayProperty property( _cmpPrp, _name );
				property.get( arrayPtr, Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

				const int len = (int)arrayPtr->size();

				if( len ) { str = (*arrayPtr)[0]; }
				else      { str = "empty";        }

				if( len > 1 ) { str += ", ..."; }

			} else if( _dataTypeId == (int)Alembic::Abc::kUint64POD ) { // unsigned long int

				Alembic::Abc::UInt64ArraySamplePtr arrayPtr;
				Alembic::Abc::IUInt64ArrayProperty property( _cmpPrp, _name );
				property.get( arrayPtr, Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

				const int len = (int)arrayPtr->size();

				if( len ) { str = (*arrayPtr)[0]; }
				else      { str = "empty";        }

				if( len > 1 ) { str += ", ..."; }

			} else if( _dataTypeId == (int)Alembic::Abc::kFloat16POD ) { // half

				Alembic::Abc::HalfArraySamplePtr arrayPtr;
				Alembic::Abc::IHalfArrayProperty property( _cmpPrp, _name );
				property.get( arrayPtr, Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

				const int len = (int)arrayPtr->size();

				if( len ) { str = (*arrayPtr)[0]; }
				else      { str = "empty";        }

				if( len > 1 ) { str += ", ..."; }

			} else if( _dataTypeId == (int)Alembic::Abc::kFloat32POD ) { // float

				Alembic::Abc::FloatArraySamplePtr arrayPtr;
				Alembic::Abc::IFloatArrayProperty property( _cmpPrp, _name );
				property.get( arrayPtr, Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

				const int len = (int)arrayPtr->size();

				if( len ) { str = (*arrayPtr)[0]; }
				else      { str = "empty";        }

				if( len > 1 ) { str += ", ..."; }

			} else if( _dataTypeId == (int)Alembic::Abc::kFloat64POD ) { // double

				Alembic::Abc::DoubleArraySamplePtr arrayPtr;
				Alembic::Abc::IDoubleArrayProperty property( _cmpPrp, _name );
				property.get( arrayPtr, Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

				const int len = (int)arrayPtr->size();

				if( len ) { str = (*arrayPtr)[0]; }
				else      { str = "empty";        }

				if( len > 1 ) { str += ", ..."; }

			} else if( _dataTypeId == (int)Alembic::Abc::kStringPOD ) { // string

				Alembic::Abc::StringArraySamplePtr arrayPtr;
				Alembic::Abc::IStringArrayProperty property( _cmpPrp, _name );
				property.get( arrayPtr, Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

				const int len = (int)arrayPtr->size();

				if( len ) { str = (*arrayPtr)[0]; }
				else      { str = "empty";        }

				if( len > 1 ) { str += ", ..."; }

			} else {

				str += "not supported data type";

			}

		} else if( _extent == 2 ) {

			if( _dataTypeId == (int)Alembic::Abc::kFloat32POD ) { // float

				Alembic::Abc::V2fArraySamplePtr arrayPtr;
				Alembic::Abc::IV2fArrayProperty property( _cmpPrp, _name );
				property.get( arrayPtr, Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

				const int len = (int)arrayPtr->size();

				if( len ) {

					str  = "(";
					str += (*arrayPtr)[0].x;
					str += ", ";
					str += (*arrayPtr)[0].y;
					str += ")";

				} else {

					str = "empty";

				}

				if( len > 1 ) { str += ", ..."; }

			} else if( _dataTypeId == (int)Alembic::Abc::kFloat64POD ) { // double

				Alembic::Abc::V2dArraySamplePtr arrayPtr;
				Alembic::Abc::IV2dArrayProperty property( _cmpPrp, _name );
				property.get( arrayPtr, Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

				const int len = (int)arrayPtr->size();

				if( len ) {

					str  = "(";
					str += (*arrayPtr)[0].x;
					str += ", ";
					str += (*arrayPtr)[0].y;
					str += ")";

				} else {

					str = "empty";

				}

				if( len > 1 ) { str += ", ..."; }

			} else {

				str += "not supported data type";

			}

		} else if( _extent == 3 ) {

			if( _dataTypeId == (int)Alembic::Abc::kFloat32POD ) { // float

				Alembic::Abc::V3fArraySamplePtr arrayPtr;
				Alembic::Abc::IV3fArrayProperty property( _cmpPrp, _name );
				property.get( arrayPtr, Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

				const int len = (int)arrayPtr->size();

				if( len ) {

					str  = "(";
					str += (*arrayPtr)[0].x;
					str += ", ";
					str += (*arrayPtr)[0].y;
					str += ", ";
					str += (*arrayPtr)[0].z;
					str += ")";

				} else {

					str = "empty";

				}

				if( len > 1 ) { str += ", ..."; }

			} else if( _dataTypeId == (int)Alembic::Abc::kFloat64POD ) { // double

				Alembic::Abc::V3dArraySamplePtr arrayPtr;
				Alembic::Abc::IV3dArrayProperty property( _cmpPrp, _name );
				property.get( arrayPtr, Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

				const int len = (int)arrayPtr->size();

				if( len ) {

					str  = "(";
					str += (*arrayPtr)[0].x;
					str += ", ";
					str += (*arrayPtr)[0].y;
					str += ", ";
					str += (*arrayPtr)[0].z;
					str += ")";

				} else {

					str = "empty";

				}

				if( len > 1 ) { str += ", ..."; }

			} else {

				str += "not supported data type";

			}

		} else {

			str += "not supported data type";

		}

	}

	return str;
}

// (unsigned) char
char
ZAlembicProperty::valueChar( int sampleIndex ) const
{
	if( !( _typeId == 2 && _extent == 1 ) ) { return ' '; }

	if( _dataTypeId == (int)Alembic::Abc::kInt8POD ) // char
	{
		Alembic::Abc::ICharProperty property( _cmpPrp, _name );
		return (char)property.getValue( Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );
	}

	if( _dataTypeId == (int)Alembic::Abc::kUint8POD ) // unsigned char
	{
		Alembic::Abc::IUcharProperty property( _cmpPrp, _name );
		return (char)property.getValue( Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );
	}

	return ' ';
}

bool
ZAlembicProperty::getValue( char& value, int sampleIndex ) const
{
	value = ' ';

	if( !( _typeId == 2 && _extent == 1 ) )
	{
		cout << "Error@ZAlembicProperty::getValue(): Invalid type." << endl;
		return false;
	}

	if( _dataTypeId == (int)Alembic::Abc::kInt8POD ) // char
	{
		Alembic::Abc::ICharProperty property( _cmpPrp, _name );
		value = (char)property.getValue( Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

		return true;
	}

	if( _dataTypeId == (int)Alembic::Abc::kUint8POD ) // unsigned char
	{
		Alembic::Abc::IUcharProperty property( _cmpPrp, _name );
		value = (char)property.getValue( Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

		return true;
	}

	return false;
}

// bool, (unsigned) short, (unsigned) int, (unsigned) long int
int
ZAlembicProperty::valueInt( int sampleIndex ) const
{
	if( !( _typeId == 2 && _extent == 1 ) ) { return 0; }

	if( _dataTypeId == (int)Alembic::Abc::kBooleanPOD ) // bool
	{
		Alembic::Abc::IBoolProperty property( _cmpPrp, _name );
		return (int)property.getValue( Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );
	}

	if( _dataTypeId == (int)Alembic::Abc::kInt16POD ) // short
	{
		Alembic::Abc::IInt16Property property( _cmpPrp, _name );
		return (int)property.getValue( Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );
	}

	if( _dataTypeId == (int)Alembic::Abc::kUint16POD ) // unsigned short
	{
		Alembic::Abc::IUInt16Property property( _cmpPrp, _name );
		return (int)property.getValue( Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );
	}

	if( _dataTypeId == (int)Alembic::Abc::kInt32POD ) // int
	{
		Alembic::Abc::IInt32Property property( _cmpPrp, _name );
		return (int)property.getValue( Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );
	}

	if( _dataTypeId == (int)Alembic::Abc::kUint32POD ) // unsigned int
	{
		Alembic::Abc::IUInt32Property property( _cmpPrp, _name );
		return (int)property.getValue( Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );
	}

	if( _dataTypeId == (int)Alembic::Abc::kInt64POD ) // long int
	{
		Alembic::Abc::IInt64Property property( _cmpPrp, _name );
		return (int)property.getValue( Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );
	}

	if( _dataTypeId == (int)Alembic::Abc::kUint64POD ) // unsigned long int
	{
		Alembic::Abc::IUInt64Property property( _cmpPrp, _name );
		return (int)property.getValue( Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );
	}

	return 0;
}

bool
ZAlembicProperty::getValue( int& value, int sampleIndex ) const
{
	value = 0;

	if( !( _typeId == 2 && _extent == 1 ) )
	{
		cout << "Error@ZAlembicProperty::getValue(): Invalid type." << endl;
		return false;
	}

	if( _dataTypeId == (int)Alembic::Abc::kBooleanPOD ) // bool
	{
		Alembic::Abc::IBoolProperty property( _cmpPrp, _name );
		value = (int)property.getValue( Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

		return true;
	}

	if( _dataTypeId == (int)Alembic::Abc::kInt16POD ) // short
	{
		Alembic::Abc::IInt16Property property( _cmpPrp, _name );
		value = (int)property.getValue( Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

		return true;
	}

	if( _dataTypeId == (int)Alembic::Abc::kUint16POD ) // unsigned short
	{
		Alembic::Abc::IUInt16Property property( _cmpPrp, _name );
		value = (int)property.getValue( Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

		return true;
	}

	if( _dataTypeId == (int)Alembic::Abc::kInt32POD ) // int
	{
		Alembic::Abc::IInt32Property property( _cmpPrp, _name );
		value = (int)property.getValue( Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

		return true;
	}

	if( _dataTypeId == (int)Alembic::Abc::kUint32POD ) // unsigned int
	{
		Alembic::Abc::IUInt32Property property( _cmpPrp, _name );
		value = (int)property.getValue( Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

		return true;
	}

	if( _dataTypeId == (int)Alembic::Abc::kInt64POD ) // long int
	{
		Alembic::Abc::IInt64Property property( _cmpPrp, _name );
		value = (int)property.getValue( Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

		return true;
	}

	if( _dataTypeId == (int)Alembic::Abc::kUint64POD ) // unsigned long int
	{
		Alembic::Abc::IUInt64Property property( _cmpPrp, _name );
		value = (int)property.getValue( Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

		return true;
	}

	return false;
}

// half, float
float
ZAlembicProperty::valueFloat( int sampleIndex ) const
{
	if( !( _typeId == 2 && _extent == 1 ) ) { return 0.f; }

	if( _dataTypeId == (int)Alembic::Abc::kFloat16POD ) // half
	{
		Alembic::Abc::IHalfProperty property( _cmpPrp, _name );
		return (float)property.getValue( Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );
	}

	if( _dataTypeId == (int)Alembic::Abc::kFloat32POD ) // float
	{
		Alembic::Abc::IFloatProperty property( _cmpPrp, _name );
		return (float)property.getValue( Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );
	}

	return 0.f;
}

bool
ZAlembicProperty::getValue( float& value, int sampleIndex ) const
{
	value = 0.f;

	if( !( _typeId == 2 && _extent == 1 ) )
	{
		cout << "Error@ZAlembicProperty::getValue(): Invalid type." << endl;
		return false;
	}

	if( _dataTypeId == (int)Alembic::Abc::kFloat16POD ) // half
	{
		Alembic::Abc::IHalfProperty property( _cmpPrp, _name );
		value = (float)property.getValue( Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

		return true;
	}

	if( _dataTypeId == (int)Alembic::Abc::kFloat32POD ) // float
	{
		Alembic::Abc::IFloatProperty property( _cmpPrp, _name );
		value = (float)property.getValue( Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

		return true;
	}

	return false;
}

// double
double
ZAlembicProperty::valueDouble( int sampleIndex ) const
{
	if( !( _typeId == 2 && _extent == 1 ) ) { return 0.0; }

	if( _dataTypeId == (int)Alembic::Abc::kFloat64POD ) // double
	{
		Alembic::Abc::IDoubleProperty property( _cmpPrp, _name );
		return (double)property.getValue( Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );
	}

	return 0.0;
}

bool
ZAlembicProperty::getValue( double& value, int sampleIndex ) const
{
	value = 0.0;

	if( !( _typeId == 2 && _extent == 1 ) )
	{
		cout << "Error@ZAlembicProperty::getValue(): Invalid type." << endl;
		return false;
	}

	if( _dataTypeId == (int)Alembic::Abc::kFloat64POD ) // double
	{
		Alembic::Abc::IDoubleProperty property( _cmpPrp, _name );
		value = (double)property.getValue( Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

		return true;
	}

	return false;
}

// string
ZString
ZAlembicProperty::valueString( int sampleIndex ) const
{
	if( !( _typeId == 2 && _extent == 1 ) )
	{
		cout << "Error@ZAlembicProperty::getValueString(): Invalid type." << endl;
		return false;
	}

	if( _dataTypeId == (int)Alembic::Abc::kStringPOD ) // string
	{
		Alembic::Abc::IStringArrayProperty property( _cmpPrp, _name );

		ostringstream oss;
		oss << property.getValue( Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );
		return oss.str();
	}

	return "";
}

bool
ZAlembicProperty::getValue( ZString& value, int sampleIndex ) const
{
	value = "";

	if( !( _typeId == 2 && _extent == 1 ) )
	{
		cout << "Error@ZAlembicProperty::getValue(): Invalid type." << endl;
		return false;
	}

	if( _dataTypeId == (int)Alembic::Abc::kStringPOD ) // string
	{
		Alembic::Abc::IStringArrayProperty property( _cmpPrp, _name );

		ostringstream oss;
		oss << property.getValue( Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );
		value = oss.str();

		return true;
	}

	return false;
}

// (unsigned) char
bool
ZAlembicProperty::getValues( ZCharArray& values, int sampleIndex ) const
{
	values.reset();

	if( !( _typeId == 3 && _extent == 1 ) )
	{
		cout << "Error@ZAlembicProperty::getValues(): Invalid type." << endl;
		return false;
	}

	if( _dataTypeId == (int)Alembic::Abc::kInt8POD ) // char
	{
		Alembic::Abc::CharArraySamplePtr arrayPtr;
		Alembic::Abc::ICharArrayProperty property( _cmpPrp, _name );
		property.get( arrayPtr, Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

		const int len = (int)arrayPtr->size();

		values.reserve( len );

		FOR( i, 0, len )
		{
			values.push_back( (char)(*arrayPtr)[i] );
		}

		return true;
	}

	if( _dataTypeId == (int)Alembic::Abc::kUint8POD ) // unsigned char
	{
		Alembic::Abc::UcharArraySamplePtr arrayPtr;
		Alembic::Abc::IUcharArrayProperty property( _cmpPrp, _name );
		property.get( arrayPtr, Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

		const int len = (int)arrayPtr->size();

		values.reserve( len );

		FOR( i, 0, len )
		{
			values.push_back( (char)(*arrayPtr)[i] );
		}

		return true;
	}

	return false;
}

// bool, (unsigned) short, (unsigned) int, (unsigned) long int
bool
ZAlembicProperty::getValues( ZIntArray& values, int sampleIndex ) const
{
	values.reset();

	if( !( _typeId == 3 && _extent == 1 ) )
	{
		cout << "Error@ZAlembicProperty::getValues(): Invalid type." << endl;
		return false;
	}

	if( _dataTypeId == (int)Alembic::Abc::kBooleanPOD ) // bool
	{
		Alembic::Abc::BoolArraySamplePtr arrayPtr;
		Alembic::Abc::IBoolArrayProperty property( _cmpPrp, _name );
		property.get( arrayPtr, Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

		const int len = (int)arrayPtr->size();

		values.reserve( len );

		FOR( i, 0, len )
		{
			values.push_back( (int)(*arrayPtr)[i] );
		}

		return true;
	}

	if( _dataTypeId == (int)Alembic::Abc::kInt16POD ) // short
	{
		Alembic::Abc::Int16ArraySamplePtr arrayPtr;
		Alembic::Abc::IInt16ArrayProperty property( _cmpPrp, _name );
		property.get( arrayPtr, Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

		const int len = (int)arrayPtr->size();

		values.reserve( len );

		FOR( i, 0, len )
		{
			values.push_back( (int)(*arrayPtr)[i] );
		}

		return true;
	}

	if( _dataTypeId == (int)Alembic::Abc::kUint16POD ) // unsigned short
	{
		Alembic::Abc::UInt16ArraySamplePtr arrayPtr;
		Alembic::Abc::IUInt16ArrayProperty property( _cmpPrp, _name );
		property.get( arrayPtr, Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

		const int len = (int)arrayPtr->size();

		values.reserve( len );

		FOR( i, 0, len )
		{
			values.push_back( (int)(*arrayPtr)[i] );
		}

		return true;
	}

	if( _dataTypeId == (int)Alembic::Abc::kInt32POD ) // int
	{
		Alembic::Abc::Int32ArraySamplePtr arrayPtr;
		Alembic::Abc::IInt32ArrayProperty property( _cmpPrp, _name );
		property.get( arrayPtr, Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

		const int len = (int)arrayPtr->size();

		values.reserve( len );

		FOR( i, 0, len )
		{
			values.push_back( (int)(*arrayPtr)[i] );
		}

		return true;
	}

	if( _dataTypeId == (int)Alembic::Abc::kUint32POD ) // unsigned int
	{
		Alembic::Abc::UInt32ArraySamplePtr arrayPtr;
		Alembic::Abc::IUInt32ArrayProperty property( _cmpPrp, _name );
		property.get( arrayPtr, Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

		const int len = (int)arrayPtr->size();

		values.reserve( len );

		FOR( i, 0, len )
		{
			values.push_back( (int)(*arrayPtr)[i] );
		}

		return true;
	}

	if( _dataTypeId == (int)Alembic::Abc::kInt64POD ) // long int
	{
		Alembic::Abc::Int64ArraySamplePtr arrayPtr;
		Alembic::Abc::IInt64ArrayProperty property( _cmpPrp, _name );
		property.get( arrayPtr, Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

		const int len = (int)arrayPtr->size();

		values.reserve( len );

		FOR( i, 0, len )
		{
			values.push_back( (int)(*arrayPtr)[i] );
		}

		return true;
	}

	if( _dataTypeId == (int)Alembic::Abc::kUint64POD ) // unsigned long int
	{
		Alembic::Abc::UInt64ArraySamplePtr arrayPtr;
		Alembic::Abc::IUInt64ArrayProperty property( _cmpPrp, _name );
		property.get( arrayPtr, Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

		const int len = (int)arrayPtr->size();

		values.reserve( len );

		FOR( i, 0, len )
		{
			values.push_back( (int)(*arrayPtr)[i] );
		}

		return true;
	}

	return false;
}

// half, float
bool
ZAlembicProperty::getValues( ZFloatArray& values, int sampleIndex ) const
{
	values.reset();

	if( !( _typeId == 3 && _extent == 1 ) )
	{
		cout << "Error@ZAlembicProperty::getValues(): Invalid type." << endl;
		return false;
	}

	if( _dataTypeId == (int)Alembic::Abc::kFloat16POD ) // half
	{
		Alembic::Abc::HalfArraySamplePtr arrayPtr;
		Alembic::Abc::IHalfArrayProperty property( _cmpPrp, _name );
		property.get( arrayPtr, Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

		const int len = (int)arrayPtr->size();

		values.reserve( len );

		FOR( i, 0, len )
		{
			const float& v = (*arrayPtr)[i];
			values.push_back( v );
		}

		return true;
	}

	if( _dataTypeId == (int)Alembic::Abc::kFloat32POD ) // float
	{
		Alembic::Abc::FloatArraySamplePtr arrayPtr;
		Alembic::Abc::IFloatArrayProperty property( _cmpPrp, _name );
		property.get( arrayPtr, Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

		const int len = (int)arrayPtr->size();

		values.reserve( len );

		FOR( i, 0, len )
		{
			const double& v = (*arrayPtr)[i];
			values.push_back( (float)v );
		}

		return true;
	}

	return false;
}

// double
bool
ZAlembicProperty::getValues( ZDoubleArray& values, int sampleIndex ) const
{
	values.reset();

	if( !( _typeId == 3 && _extent == 1 ) )
	{
		cout << "Error@ZAlembicProperty::getValues(): Invalid type." << endl;
		return false;
	}

	if( _dataTypeId == (int)Alembic::Abc::kFloat64POD ) // double
	{
		Alembic::Abc::DoubleArraySamplePtr arrayPtr;
		Alembic::Abc::IDoubleArrayProperty property( _cmpPrp, _name );
		property.get( arrayPtr, Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

		const int len = (int)arrayPtr->size();

		values.reserve( len );

		FOR( i, 0, len )
		{
			const double& v = (*arrayPtr)[i];
			values.push_back( v );
		}

		return true;
	}

	return false;
}

// string
bool
ZAlembicProperty::getValues( ZStringArray& values, int sampleIndex ) const
{
	values.reset();

	if( !( _typeId == 3 && _extent == 1 ) )
	{
		cout << "Error@ZAlembicProperty::getValues(): Invalid type." << endl;
		return false;
	}

	if( _dataTypeId == (int)Alembic::Abc::kStringPOD ) // string
	{
		Alembic::Abc::StringArraySamplePtr arrayPtr;
		Alembic::Abc::IStringArrayProperty property( _cmpPrp, _name );
		property.get( arrayPtr, Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

		const int len = (int)arrayPtr->size();

		values.reserve( len );

		FOR( i, 0, len )
		{
			values.push_back( (ZString)(*arrayPtr)[i] );
		}

		return true;
	}

	return false;
}

// float2
bool
ZAlembicProperty::getValues( ZFloat2Array& values, int sampleIndex ) const
{
	values.reset();

	if( !( _typeId == 3 && _extent == 2 ) )
	{
		cout << "Error@ZAlembicProperty::getValues(): Invalid type." << endl;
		return false;
	}

	if( _dataTypeId == (int)Alembic::Abc::kFloat32POD ) // float
	{
		Alembic::Abc::V2fArraySamplePtr arrayPtr;
		Alembic::Abc::IV2fArrayProperty property( _cmpPrp, _name );
		property.get( arrayPtr, Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

		const int len = (int)arrayPtr->size();

		values.reserve( len );

		FOR( i, 0, len )
		{
			const Imath::V2f& v = (*arrayPtr)[i];
			values.push_back( ZFloat2( v.x, v.y ) );
		}

		return true;
	}

	if( _dataTypeId == (int)Alembic::Abc::kFloat64POD ) // double
	{
		Alembic::Abc::V2dArraySamplePtr arrayPtr;
		Alembic::Abc::IV2dArrayProperty property( _cmpPrp, _name );
		property.get( arrayPtr, Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

		const int len = (int)arrayPtr->size();

		values.reserve( len );

		FOR( i, 0, len )
		{
			const Imath::V2d& v = (*arrayPtr)[i];
			values.push_back( ZFloat2( (float)v.x, (float)v.y ) );
		}

		return true;
	}

	return false;
}

// float3
bool
ZAlembicProperty::getValues( ZFloat3Array& values, int sampleIndex ) const
{
	values.reset();

	if( !( _typeId == 3 && _extent == 3 ) )
	{
		cout << "Error@ZAlembicProperty::getValues(): Invalid type." << endl;
		return false;
	}

	if( _dataTypeId == (int)Alembic::Abc::kFloat32POD ) // float
	{
		Alembic::Abc::V3fArraySamplePtr arrayPtr;
		Alembic::Abc::IV3fArrayProperty property( _cmpPrp, _name );
		property.get( arrayPtr, Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

		const int len = (int)arrayPtr->size();

		values.reserve( len );

		FOR( i, 0, len )
		{
			const Imath::V3f& v = (*arrayPtr)[i];
			values.push_back( ZFloat3( v.x, v.y, v.z ) );
		}

		return true;
	}

	if( _dataTypeId == (int)Alembic::Abc::kFloat64POD ) // double
	{
		Alembic::Abc::V3dArraySamplePtr arrayPtr;
		Alembic::Abc::IV3dArrayProperty property( _cmpPrp, _name );
		property.get( arrayPtr, Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

		const int len = (int)arrayPtr->size();

		values.reserve( len );

		FOR( i, 0, len )
		{
			const Imath::V3d& v = (*arrayPtr)[i];
			values.push_back( ZFloat3( (float)v.x, (float)v.y, (float)v.z ) );
		}

		return true;
	}

	return false;
}

// quaternion
bool
ZAlembicProperty::getValues( ZQuaternionArray& values, int sampleIndex ) const
{
	values.reset();

	if( !( _typeId == 3 && _extent == 4 ) )
	{
		cout << "Error@ZAlembicProperty::getValues(): Invalid type." << endl;
		return false;
	}

	if( _dataTypeId == (int)Alembic::Abc::kFloat32POD ) // float
	{
		Alembic::Abc::QuatfArraySamplePtr arrayPtr;
		Alembic::Abc::IQuatfArrayProperty property( _cmpPrp, _name );
		property.get( arrayPtr, Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

		const int len = (int)arrayPtr->size();

		values.reserve( len );

		FOR( i, 0, len )
		{
			const Imath::Quatf v = (*arrayPtr)[i];
			values.push_back( ZQuaternion( v.r, v.v[0], v.v[1], v.v[2] ) );
		}

		return true;
	}

	if( _dataTypeId == (int)Alembic::Abc::kFloat64POD ) // double
	{
		Alembic::Abc::QuatdArraySamplePtr arrayPtr;
		Alembic::Abc::IQuatdArrayProperty property( _cmpPrp, _name );
		property.get( arrayPtr, Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

		const int len = (int)arrayPtr->size();

		values.reserve( len );

		FOR( i, 0, len )
		{
			const Imath::Quatd v = (*arrayPtr)[i];
			values.push_back( ZQuaternion( (float)v.r, (float)v.v[0], (float)v.v[1], (float)v.v[2] ) );
		}

		return true;
	}

	return false;
}

// vector
bool
ZAlembicProperty::getValues( ZVectorArray& values, int sampleIndex ) const
{
	values.reset();

	if( !( _typeId == 3 && _extent == 3 ) )
	{
		cout << "Error@ZAlembicProperty::getValues(): Invalid type." << endl;
		return false;
	}

	if( _dataTypeId == (int)Alembic::Abc::kFloat32POD ) // float
	{
		Alembic::Abc::V3fArraySamplePtr arrayPtr;
		Alembic::Abc::IV3fArrayProperty property( _cmpPrp, _name );
		property.get( arrayPtr, Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

		const int len = (int)arrayPtr->size();

		values.reserve( len );

		FOR( i, 0, len )
		{
			const Imath::V3f& v = (*arrayPtr)[i];
			values.push_back( ZVector( v.x, v.y, v.z ) );
		}

		return true;
	}

	if( _dataTypeId == (int)Alembic::Abc::kFloat64POD ) // double
	{
		Alembic::Abc::V3dArraySamplePtr arrayPtr;
		Alembic::Abc::IV3dArrayProperty property( _cmpPrp, _name );
		property.get( arrayPtr, Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

		const int len = (int)arrayPtr->size();

		values.reserve( len );

		FOR( i, 0, len )
		{
			const Imath::V3d& v = (*arrayPtr)[i];
			values.push_back( ZVector( (float)v.x, (float)v.y, (float)v.z ) );
		}

		return true;
	}

	return false;
}

// color array
bool
ZAlembicProperty::getValues( ZColorArray& values, int sampleIndex ) const
{
	values.reset();

	if( !( _typeId == 3 && _extent == 4 ) )
	{
		cout << "Error@ZAlembicProperty::getValues(): Invalid type." << endl;
		return false;
	}

	if( _dataTypeId == (int)Alembic::Abc::kFloat32POD ) // float
	{
		Alembic::Abc::C4fArraySamplePtr arrayPtr;
		Alembic::Abc::IC4fArrayProperty property( _cmpPrp, _name );
		property.get( arrayPtr, Alembic::Abc::ISampleSelector( Alembic::Abc::index_t(sampleIndex) ) );

		const int len = (int)arrayPtr->size();

		values.reserve( len );

		FOR( i, 0, len )
		{
			const Imath::C4f& c = (*arrayPtr)[i];
			values.push_back( ZColor( c.r, c.g, c.b ) );
		}

		return true;
	}

	return false;
}

ostream&
operator<<( ostream& os, const ZAlembicProperty& object )
{
	os << "<ZAlembicProperty>" << endl;
	os << " Name           : " << object.name() << endl;
	os << " Type           : " << object.typeStr() << endl;
	os << " # of Children  : " << object.numChildren() << endl;
	os << " Data Type      : " << object.dataTypeStr() << endl;
	os << " Extent         : " << object.extent() << endl;

	os << endl;

	return os;
}

ZELOS_NAMESPACE_END

