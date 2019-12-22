//----------------//
// pluginMain.cpp //
//-------------------------------------------------------//
// author: Jaegwang lim @ Dexter Studios                 //
// last update: 2017.08.16                               //
//-------------------------------------------------------//

#include <maya/MFnPlugin.h>
#include <ZWeb.h>

MStatus
initializePlugin( MObject obj )
{
	MStatus stat = MS::kSuccess;
	MFnPlugin pluginFn( obj, "Dexter Studios", "1.0", "Any" );

	// GLEW Initialization
	if( ( MGlobal::mayaState() != MGlobal::kBatch ) && ( MGlobal::mayaState() != MGlobal::kLibraryApp ) )
	{
		glewInit();

		GLenum status = glCheckFramebufferStatusEXT( GL_FRAMEBUFFER_EXT );

		if( status == GL_FRAMEBUFFER_UNSUPPORTED_EXT )
		{
			cout << "Error: GL_FRAMEBUFFER_UNSUPPORTED_EXT" << endl;
		}
	}

	// Data
	RegisterData( ZWebCurvesData );	

	// DGs
	RegisterNode( ZWebCurves );
	RegisterNode( ZWStitchCurves );
	RegisterNode( ZWebCurvesControl );
	RegisterNode( ZWStitchCurvesControl );

	// commands
	RegisterCommand( ZWExportComm );
	RegisterCommand( ZWEnableComm );

	// .mel scripts
	MGlobal::sourceFile( "ZWebCustomShelf.mel" ); 

	// menu
	//pluginFn.registerUI( "CreateZelosMenu", "DeleteZelosMenu" );

	return MS::kSuccess;
}

MStatus
uninitializePlugin( MObject obj )
{
	MStatus stat = MS::kSuccess;
	MFnPlugin pluginFn( obj );

	// Data
	DeregisterData( ZWebCurvesData );

	// DGs
	DeregisterNode( ZWebCurves );
	DeregisterNode( ZWStitchCurves );
	DeregisterNode( ZWebCurvesControl );	
	DeregisterNode( ZWStitchCurvesControl );

	// commands
	DeregisterCommand( ZWExportComm );
	DeregisterCommand( ZWEnableComm );

	return MS::kSuccess;
}

