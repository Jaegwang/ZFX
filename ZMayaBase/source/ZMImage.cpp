//-------------//
// ZMImage.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#include <ZelosBaseForMaya.h>

ZELOS_NAMESPACE_BEGIN

ZMImage::ZMImage()
{
	init();
}

ZMImage::ZMImage( const char* filePathName )
{
	init();
	readFromFile( filePathName );
}

ZMImage::~ZMImage()
{
	reset();
}

void
ZMImage::reset()
{
	if( toReleaseCPixel ) { ZFree( m_cPixel, true ); }
	if( toReleaseFPixel ) { ZFree( m_fPixel, true ); }

	init();
}

void
ZMImage::init()
{
	toReleaseCPixel = false;
	toReleaseFPixel = false;

	m_width  = 0;
	m_height = 0;
	m_depth  = 0;
	m_size   = 0;

	m_cPixel = (unsigned char*)NULL;
	m_fPixel = (float*)NULL;
}

bool
ZMImage::readFromFile( const char* filePathName )
{
	MImage img;

	if( !img.readFromFile( filePathName ) ) { return false; }

	// image size
	img.getSize( m_width, m_height );
	m_depth = img.depth();
	m_size = m_width * m_height * m_depth;

	if( !( m_depth == 3 || m_depth == 4 ) )
	{
		MGlobal::displayError( MString( "Invalid depth." ) );
		return false;
	}

	switch( img.pixelType() )
	{
		//case MImage::MPixelType::kByte:
		case MImage::kByte:

			m_cPixel = img.pixels();
			ZAlloc( m_fPixel, m_size );

			#pragma omp parallel for
			FOR( i, 0, m_size )
			{
				m_fPixel[i] = m_cPixel[i] / 255.f;
			}

			toReleaseCPixel = false;
			toReleaseFPixel = true;

			break;

		//case MImage::MPixelType::kFloat:
		case MImage::kFloat:

			ZAlloc( m_cPixel, m_size );
			m_fPixel = img.floatPixels();

			#pragma omp parallel for
			FOR( i, 0, m_size )
			{
				m_cPixel[i] = (unsigned char)( 255 * m_fPixel[i] );
			}

			toReleaseCPixel = true;
			toReleaseFPixel = false;

			break;

		default:

			MGlobal::displayError( MString( "Invalid pixel type" ) );
			return false;
	}

	return true;
}

ZELOS_NAMESPACE_END

