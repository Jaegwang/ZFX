//---------------//
// ZImageMap.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
//         Jaegwang Lim @ Dexter Studios                 //
// last update: 2018.04.24                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

#include <half.h>
#include <tiffio.h>
//#include <jpeglib.h>

#include <OpenImageIO/imageio.h>
OIIO_NAMESPACE_USING

ZELOS_NAMESPACE_BEGIN

float ZImageMap::_255 = 1 / 255.f;

ZImageMap::ZImageMap()
{
	ZImageMap::_init();
}

ZImageMap::ZImageMap( const ZImageMap& img )
{
	ZImageMap::_init();
	*this = img;
}

ZImageMap::ZImageMap( const char* filePathName )
{
	ZImageMap::_init();
	ZImageMap::load( filePathName );
}

ZImageMap::ZImageMap( int Width, int Height, int NumChannels )
{
	ZImageMap::_init();
	ZImageMap::set( Width, Height, NumChannels );
}

ZImageMap::~ZImageMap()
{
	reset();
}

void
ZImageMap::_init()
{
	_width       = 0;
	_height      = 0;
	_numChannels = 0;
	_data        = (float*)NULL;
	_format      = ZImageFormat::zNone;
}

void
ZImageMap::reset()
{
	ZFree( _data, true );
	_init();
	_filePathName.clear();
}

bool
ZImageMap::set( int Width, int Height, int NumChannels )
{
	if( Width<1 || Height<1 || NumChannels<1 )
	{
		cout << "Error@ZImageMap::set(): Invalid dimension." << endl;
		reset();
		return false;
	}

	const int oldSize = size();

	_width       = Width;
	_height      = Height;
	_numChannels = NumChannels;

	const int newSize = size();

	if( oldSize != newSize )
	{
		ZFree( _data, true );
		ZAlloc( _data, newSize );
	}

	memset( (char*)_data, 0, newSize*sizeof(float) );

	return true;
}

ZImageMap&
ZImageMap::operator=( const ZImageMap& img )
{
	const int oldSize = size();

	_width        = img._width;
	_height       = img._height;
	_numChannels  = img._numChannels;
	_format       = img._format;
	_filePathName.clear();

	const int newSize = size();

	if( oldSize != newSize )
	{
		if( _data ) { delete[] _data; }
	}

	memcpy( (char*)_data, (char*)img._data, newSize*sizeof(float) );

	return (*this);
}

float
ZImageMap::min( const int& channelIndex ) const
{
	const int imgSize = _width * _height * _numChannels;
    const int nChannels = ZImageMap::numChannels();
    const int k = ZClamp( channelIndex, 0, nChannels );

    float ret = Z_FLTMAX;
 
    for( int i=k; i<imgSize; i+=_numChannels )
    {
        ret = ZMin( ret, _data[i] );
    }

    return ret;
}

float
ZImageMap::max( const int& channelIndex ) const
{
	const int imgSize = _width * _height * _numChannels;
    const int nChannels = ZImageMap::numChannels();
    const int k = ZClamp( channelIndex, 0, nChannels );

    float ret = -Z_FLTMAX;

    for( int i=k; i<imgSize; i+=_numChannels )
    {
        ret = ZMax( ret, _data[i] );
    }

    return ret;
}

float
ZImageMap::average( const int& channelIndex ) const
{
	const int imgSize = _width * _height * _numChannels;
    const int nChannels = ZImageMap::numChannels();
    const int k = ZClamp( channelIndex, 0, nChannels );

    double ret = 0.0;

	const int pixelCount = _width * _height;
    double denom = 1.0 / (double)pixelCount;

    for( int i=k; i<imgSize; i+=_numChannels )
    {
        ret += _data[i] * denom;
    }

    return (float)ret;
}

void
ZImageMap::histogram( ZIntArray& counts, const int& numSegments, const int& channelIndex, const float& min, const float& max ) const
{
    if( numSegments < 1 )
    {
        counts.reset();
        return;
    }

    counts.setLength( numSegments );

	const int imgSize = _width * _height * _numChannels;
    const int nChannels = ZImageMap::numChannels();
    const int k = ZClamp( channelIndex, 0, nChannels );

    const float diff = ZAbs( max - min );
    const float dx = diff / (float)numSegments;

    for( int i=k; i<imgSize; i+=_numChannels )
    {
        const float& value = _data[i];

        if( value < min ) { continue; }
        if( value > max ) { continue; }

        const int idx = ( value / dx );

        counts[idx]++;
    }
}

bool
ZImageMap::save( const char* filePathName, ZImageFormat::ImageFormat imageFormat ) const
{
	TypeDesc iFormat = TypeDesc::UNKNOWN;
	switch( imageFormat )
	{
		case ZImageFormat::zInt8:   { iFormat=TypeDesc::INT8;   break; }
		case ZImageFormat::zUInt8:  { iFormat=TypeDesc::UINT8;  break; }
		case ZImageFormat::zInt16:  { iFormat=TypeDesc::INT16;  break; }
		case ZImageFormat::zUInt16: { iFormat=TypeDesc::UINT16; break; }
		case ZImageFormat::zInt32:  { iFormat=TypeDesc::INT32;  break; }
		case ZImageFormat::zUInt32: { iFormat=TypeDesc::UINT32; break; }
		case ZImageFormat::zInt64:  { iFormat=TypeDesc::INT64;  break; }
		case ZImageFormat::zUInt64: { iFormat=TypeDesc::UINT64; break; }
		case ZImageFormat::zHalf:   { iFormat=TypeDesc::HALF;   break; }
		case ZImageFormat::zFloat:  { iFormat=TypeDesc::FLOAT;  break; }
		case ZImageFormat::zDouble: { iFormat=TypeDesc::DOUBLE; break; }
		default: { cout << "Error@ZImageMap::save(): Invalid image format." << endl; return false; }
	}

	ImageOutput* img = ImageOutput::create( filePathName );
	ImageSpec imgSpec( _width, _height, _numChannels, iFormat );

	if( !img->open( filePathName, imgSpec ) )
	{
		cout << "Error@ZImageMap::save(): Failed to open file: " << filePathName << endl;
		return false;
	}

	if( !img->write_image( TypeDesc::FLOAT, _data ) )
	{
		cout << "Error@ZImageMap::save(): Failed to write file: " << filePathName << endl;
		return false;
	}

	img->close();
	delete img;

	return true;
}

bool
ZImageMap::load( const char* filePathName, bool skipIfSameFile )
{
	if( !ZFileExist( filePathName ) )
	{
		// Don't print error message!
		//cout << "Error@ZImageMap::load(): Failed to open file: " << filePathName << endl;
		return false;
	}

	if( skipIfSameFile && ( _filePathName == filePathName ) ) { return true; }

	ZImageMap::reset();

	const ZString extension( ZFileExtension(filePathName) );

	if( extension == "tiff" || extension == "tif" )
    {
		_load_tif( filePathName );
//	}
//	else if( extension == "jpg" )
//	{
//		_load_jpg( filePathName );
	}
    else if( extension == "tga" )
    {
		_load_tga( filePathName );

    } else if( extension == "tex" )
    {
		cout << "Error@ZImageMap::load(): .tex file format is not supported." << endl;
        return false;
	}
    else
    {
		ImageInput* img = ImageInput::create( filePathName );

		ImageSpec imgSpec;
		if( !img->open( filePathName, imgSpec ) )
		{
			cout << "Error@ZImageMap::load(): Failed to open file: " << filePathName << endl;
			return false;
		}

		int imageFormat = Z_NONE;
		switch( imgSpec.format.basetype )
		{
			case TypeDesc::INT8:   { imageFormat=ZImageFormat::zInt8;   break; }
			case TypeDesc::UINT8:  { imageFormat=ZImageFormat::zUInt8;  break; }
			case TypeDesc::INT16:  { imageFormat=ZImageFormat::zInt16;  break; }
			case TypeDesc::UINT16: { imageFormat=ZImageFormat::zUInt16; break; }
			case TypeDesc::INT32:  { imageFormat=ZImageFormat::zInt32;  break; }
			case TypeDesc::UINT32: { imageFormat=ZImageFormat::zUInt32; break; }
			case TypeDesc::INT64:  { imageFormat=ZImageFormat::zInt64;  break; }
			case TypeDesc::UINT64: { imageFormat=ZImageFormat::zUInt64; break; }
			case TypeDesc::HALF:   { imageFormat=ZImageFormat::zHalf;   break; }
			case TypeDesc::FLOAT:  { imageFormat=ZImageFormat::zFloat;  break; }
			case TypeDesc::DOUBLE: { imageFormat=ZImageFormat::zDouble; break; }
			default: { cout << "Error@ZImageMap::load(): Invalid image format." << endl; return false; }
		}

		_width        = imgSpec.width;
		_height       = imgSpec.height;
		_numChannels  = imgSpec.nchannels;
		_format       = ZImageFormat::ImageFormat(imageFormat);

		const int _size = size();

		_data = new float[_size];

		if( !img->read_image( TypeDesc::FLOAT, _data ) )
		{
			cout << "Error@ZImageMap::load(): Failed to read file: " << filePathName << endl;
			return false;
		}

		img->close();
		delete img;

	}

	_filePathName = filePathName;

	return true;
}

template<typename T>
bool
ZImageMap_readTiff( ZImageMap& img, TIFF* tiff )
{
	const int w = img.width();
	const int h = img.height();
	const int c = img.numChannels();
	float* data = img.pointer();

	T* buffer = (T*)NULL;
	ZAlloc( buffer, c*w );

	bool stat = true;
	int idx = 0;

	// Don't use OpenMP here.
	FOR( j, 0, h )
	{
		stat = TIFFReadScanline( tiff, buffer, j, 1 );
		if( !stat ) { break; }

		FOR( i, 0, w )
		{
			FOR( k, 0, c )
			{
				data[idx++] = float(buffer[i*c+k]);
			}
		}
	}

	ZFree( buffer );

	return stat;
}

bool
ZImageMap::_load_tif( const char* filePathName )
{
	reset();

	TIFF* tiff = TIFFOpen( filePathName, "rb" );

	if( !tiff )
	{
		cout << "Error@ZImageMap::load(): Failed to open file: " << filePathName << endl;
		return false;
	}

	bool stat = true;

	// width
	int w = 0;
	stat = TIFFGetField( tiff, TIFFTAG_IMAGEWIDTH, &w );
	if( !stat || w<1 )
	{
		cout << "Error@ZImageMap::load(): " << filePathName << " has invalid width." << endl;
		return false;
	}

	// height
	int h = 0;
	stat = TIFFGetField( tiff, TIFFTAG_IMAGELENGTH, &h );
	if( !stat || h<1 )
	{
		cout << "Error@ZImageMap::load(): " << filePathName << " has invalid height." << endl;
		return false;
	}

	// # of channels
	int c = 0;
	stat = TIFFGetField( tiff, TIFFTAG_SAMPLESPERPIXEL, &c );
	if( !stat || c<1 )
	{
		cout << "Error@ZImageMap::load(): " << filePathName << " has invalid number of channels." << endl;
		return false;
	}

	// orientation
	int o = 0;
	stat = TIFFGetField( tiff, TIFFTAG_ORIENTATION, &o );
	if( !stat || o!=1 ) // ORIENTATION_TOPLEFT = 1
	{
		cout << "Error@ZImageMap::load(): " << filePathName << " has invalid orientation." << endl;
		return false;
	}

	// sample format
	int s = 0;
	TIFFGetField( tiff, TIFFTAG_SAMPLEFORMAT, &s );

	// Some .tiff file has zero s value.
	//stat = TIFFGetField( tiff, TIFFTAG_SAMPLEFORMAT, &s );
	//if( !stat )
	//{
	//	cout << "Error@ZImageMap::load(): " << filePathName << " has invalid sample format." << endl;
	//	return false;
	//}

	// bits per sample
	int b = 0;
	stat = TIFFGetField( tiff, TIFFTAG_BITSPERSAMPLE, &b );
	if( !stat || b<1 )
	{
		cout << "Error@ZImageMap::load(): " << filePathName << " has invalid bits per sample." << endl;
		return false;
	}
	const int bytesPerSample = b / 8;

	if( (c*w*bytesPerSample) != TIFFScanlineSize(tiff) )
	{
		cout << "Error@ZImageMap::load(): " << filePathName << " has unsupported scanline format." << endl;
		return false;
	}

	_width       = w;
	_height      = h;
	_numChannels = c;

			// Some .tiff file has zero s value.
	     if( b==8  && s==SAMPLEFORMAT_UINT   ) { _format = ZImageFormat::zUInt8;  }
	else if( b==8  && s==0                   ) { _format = ZImageFormat::zUInt8;  }
	else if( b==16 && s==SAMPLEFORMAT_UINT   ) { _format = ZImageFormat::zUInt16; }
	else if( b==16 && s==SAMPLEFORMAT_IEEEFP ) { _format = ZImageFormat::zHalf;   }
	else if( b==16 && s==0                   ) { _format = ZImageFormat::zHalf;   }
	else if( b==32 && s==SAMPLEFORMAT_IEEEFP ) { _format = ZImageFormat::zFloat;  }
	else if( b==64 && s==SAMPLEFORMAT_IEEEFP ) { _format = ZImageFormat::zDouble; }
	else {
		cout << "Error@ZImageMap::load(): " << filePathName << " has unsupported format." << endl;
		return false;
	}

	const int imgSize = _width * _height * _numChannels;
	ZAlloc( _data, imgSize );

	if( _format==ZImageFormat::zUInt8 || _format==ZImageFormat::zUInt16 ) {

		uint32* buffer = (uint32*)NULL;
		ZAlloc( buffer, _width*_height );

		stat = TIFFReadRGBAImage( tiff, _width, _height, buffer, 0 );

		if( stat )
		{
			// Don't use OpenMP here.
			FOR( j, 0, _height )
			FOR( i, 0, _width  )
			{{
				const int index = (_height-1-j)*_width + i; // flip vertically
				int idx = _numChannels*( j*_width + i );

				if( _numChannels == 3 ) {

					_data[idx++] = static_cast<float>( TIFFGetR( buffer[index] ) * _255 );
					_data[idx++] = static_cast<float>( TIFFGetG( buffer[index] ) * _255 );
					_data[idx++] = static_cast<float>( TIFFGetB( buffer[index] ) * _255 );

				} else if( _numChannels == 4 ) {

					_data[idx++] = static_cast<float>( TIFFGetR( buffer[index] ) * _255 );
					_data[idx++] = static_cast<float>( TIFFGetG( buffer[index] ) * _255 );
					_data[idx++] = static_cast<float>( TIFFGetB( buffer[index] ) * _255 );
					_data[idx++] = static_cast<float>( TIFFGetA( buffer[index] ) * _255 );

				}
			}}
		}

		ZFree( buffer );
		TIFFClose( tiff );

	} else if( _format == ZImageFormat::zHalf ) {

		stat = ZImageMap_readTiff<half>( *this, tiff );

	} else if( _format == ZImageFormat::zFloat ) {

		stat = ZImageMap_readTiff<float>( *this, tiff );

	} else if( _format == ZImageFormat::zDouble ) {

		stat = ZImageMap_readTiff<double>( *this, tiff );

	} else {

		stat = false;

	}

	if( !stat )
	{
		reset();
		cout << "Error@ZImageMap::load(): Failed to read image: " << filePathName << endl;
		return false;
	}

	return true;
}

//bool
//ZImageMap::_load_jpg( const char* filePathName )
//{
//	reset();
//
//	FILE* fp = fopen( filePathName, "rb" );
//
//	if( !fp )
//	{
//		cout << "Error@ZImageMap::load(): Failed to open file: " << filePathName << endl;
//		return false;
//	}
//
//	jpeg_decompress_struct cinfo;
//	jpeg_error_mgr jerr;
//
//	cinfo.err = jpeg_std_error( &jerr );
//
//	jpeg_create_decompress( &cinfo );
//	jpeg_stdio_src( &cinfo, fp );
//
//	jpeg_read_header( &cinfo, true );
//	jpeg_start_decompress( &cinfo );
//
//	_width       = (int)cinfo.output_width;
//	_height      = (int)cinfo.output_height;
//	_numChannels = 3; // only RGB format
//	_format      = ZImageFormat::zUInt8;
//
//	const int imgSize = _width * _height * _numChannels;
//	if( imgSize <= 0 )
//	{
//		reset();
//		cout << "Error@ZImageMap::load(): " << filePathName << " has invalid dimension." << endl;
//		return false;
//	}
//
//	ZAlloc( _data, imgSize );
//
//	JSAMPLE* jpeg = (JSAMPLE*)NULL;
//	ZAlloc( jpeg, _width*_numChannels );
//
//	// Don't use OpenMP here.
//	FOR( j, 0, _height )
//	{
//		jpeg_read_scanlines( &cinfo, &jpeg, 1 );
//
//		const int jWidth = j*_width;
//
//		FOR( i, 0, _width )
//		{
//			int index = i*3;
//			int idx = _numChannels*(i+jWidth);
//
//			_data[  idx] = jpeg[  index] * _255;
//			_data[++idx] = jpeg[++index] * _255;
//			_data[++idx] = jpeg[++index] * _255;
//			_data[++idx] = 1;
//		}
//	}
//
//	ZFree( jpeg );
//
//	jpeg_finish_decompress( &cinfo );
//	jpeg_destroy_decompress( &cinfo );
//
//	fclose( fp );
//
//	return true;
//}

bool
ZImageMap::_load_tga( const char* filePathName )
{
	ifstream fin( filePathName, ios::in | ios::binary );

	if( fin.fail() )
	{
		cout << "Error@ZImageMap::load(): Failed to open file: " << filePathName << endl;
		return false;
	}

	static unsigned char uTGAheader[12] = { 0, 0,  2, 0, 0, 0, 0, 0, 0, 0, 0, 0 };	// Uncompressed TGA Header
	static unsigned char cTGAheader[12] = { 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0 };	// Compressed TGA Header

	unsigned char header[12];
	fin.read( (char*)&header, sizeof(header) );

	unsigned char infoHeader[6];
	fin.read( (char*)infoHeader, sizeof(infoHeader) );

	_width  = infoHeader[0] + infoHeader[1] * 256;	// = highbyte*256 + lowbyte
	_height = infoHeader[2] + infoHeader[3] * 256;	// = highbyte*256 + lowbyte
	if( _width <= 0 || _height <= 0 )
	{
		reset();
		cout << "Error@ZImageMap::load(): " << filePathName << " has invalid dimension." << endl;
		return false;
	}

	int bitsPerPixel = infoHeader[4];
	if( ( bitsPerPixel != 24 ) && ( bitsPerPixel !=32 ) )
	{
		reset();
		cout << "Error@ZImageMap::load(): " << filePathName << " has invalid format." << endl;
		return false;
	}
	if( bitsPerPixel == 24 ) { _numChannels = 3; }
	else                     { _numChannels = 4; }

	const int imgSize = _width * _height * _numChannels;
	ZAlloc( _data, imgSize );

	unsigned char* imgData = (unsigned char*)NULL;
	ZAlloc( imgData, imgSize );

	if( !memcmp( uTGAheader, &header, sizeof(header) ) ) {		// it is an uncompressed TGA image file

		fin.read( (char*)imgData, sizeof(unsigned char)*imgSize );

		for( int i=0; i<imgSize; i+=_numChannels )
		{
			ZSwap( imgData[i], imgData[i+2] );
		}

		// Origin: bottom left -> top left
		FOR( j, 0, _height )
		FOR( i, 0, _width  )
		{{
			const int idx0 = _numChannels*(i+j*_width);
			const int idx1 = _numChannels*(i+(_height-j-1)*_width);

			FOR( k, 0, _numChannels )
			{
				_data[k+idx0] =  imgData[k+idx1] * _255;
			}
		}}

	} else if( !memcmp( cTGAheader, &header, sizeof(header) )) {	// it is a compressed TGA image file

		const int bytesPerPixel = bitsPerPixel / 8;
		unsigned int pixelcount	= _width * _height;		// Nuber of pixels in the image
		unsigned int currentpixel	= 0;				// Current pixel being read
		unsigned int currentbyte	= 0;				// Current byte 

		unsigned char* buffer = (unsigned char*)NULL;
		ZAlloc( buffer, bytesPerPixel );				// Storage for 1 pixel

		unsigned char chunkHeader;
		unsigned int counter = 0;

		do {

			chunkHeader = 0;												// Storage for "chunk" header
			fin.read( (char*)&chunkHeader, sizeof(unsigned char) );

			if( chunkHeader < 128 ) {										// If the ehader is < 128, it means the that is the number of RAW color packets minus 1 that follow the header

				++chunkHeader;												// add 1 to get number of following color values

				FOR( counter, 0, chunkHeader )								// Read RAW color values
				{
					fin.read( (char*)buffer, sizeof(unsigned char)*bytesPerPixel );

					imgData[currentbyte  ] = buffer[2];						// Flip R and B vcolor values around in the process 
					imgData[currentbyte+1] = buffer[1];
					imgData[currentbyte+2] = buffer[0];

					if( bytesPerPixel == 4 )								// if its a 32 bpp image
					{
						imgData[currentbyte+3] = buffer[3];					// copy the 4th byte
					}

					currentbyte += bytesPerPixel;							// Increase thecurrent byte by the number of bytes per pixel
					++currentpixel;											// Increase current pixel by 1

					if( currentpixel > pixelcount )							// Make sure we havent read too many pixels
					{
						ZFree( buffer );
						ZFree( imgData );
						reset();
						cout << "Error@ZImageMap::load(): " << filePathName << " has invalid data." << endl;
						return false;
					}
				}

			} else {														// chunkHeader > 128 RLE imgData, next color reapeated chunkHeader - 127 times

				chunkHeader -= 127;											// Subteact 127 to get rid of the ID bit
				fin.read( (char*)buffer, sizeof(unsigned char)*bytesPerPixel );

				FOR( counter, 0, chunkHeader )								// Read RAW color values
				{
					imgData[currentbyte  ] = buffer[2];						// switch R and B bytes areound while copying
					imgData[currentbyte+1] = buffer[1];
					imgData[currentbyte+2] = buffer[0];

					if( bytesPerPixel == 4 )								// If TGA images is 32 bpp
					{
						imgData[currentbyte+3] = buffer[3];					// Copy 4th byte
					}

					currentbyte += bytesPerPixel;							// Increase current byte by the number of bytes per pixel
					++currentpixel;											// Increase pixel count by 1

					if( currentpixel > pixelcount )							// Make sure we havent written too many pixels
					{
						ZFree( buffer );
						ZFree( imgData );
						reset();
						cout << "Error@ZImageMap::load(): " << filePathName << " has invalid data." << endl;
						return false;
					}
				}

			}

		} while( currentpixel < pixelcount );									// Loop while there are still pixels left

		FOR( i, 0, imgSize )
		{
			_data[i] = imgData[i] * _255;
		}

		ZFree( buffer );
	}

	ZFree( imgData );

	fin.close();

	return true;
}

ostream&
operator<<( ostream& os, const ZImageMap& object )
{
	os << "<ZImageMap>" << endl;
	os << "width        : " << object.width() << endl;
	os << "height       : " << object.height() << endl;
	os << "# of channels: " << object.numChannels() << endl;
	os << "format       : " << ZImageFormat::name(object.format()) << endl;
	os << endl;

	return os;
}

ZELOS_NAMESPACE_END

