//------------------//
// ZWEnableComm.cpp //
//-------------------------------------------------------//
// author: Jaegwang Lim @ Dexter Studios                 //
// last update: 2017.09.01                               //
//-------------------------------------------------------//

#include <ZWEnableComm.h>
#include <ZWebCurves.h>
#include <ZWStitchCurves.h>

const MString ZWEnableComm::name( "ZWebEnable" );

MStatus ZWEnableComm::doIt( const MArgList& args )
{    
	MStatus stat = MS::kSuccess;
    if( args.length() == 0 )
    {
        setResult( ZWebCurves::enable );
        return stat;
    }

    bool enable(true);
    args.get( 0, enable );

    ZWebCurves::enable = enable;
    ZWStitchCurves::enable = enable;

    setResult( enable );

    return stat;
}

