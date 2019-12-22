//-----------------//
// ProgressBar.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#include <ZelosBaseForMaya.h>

ZELOS_NAMESPACE_BEGIN

void
StartProgressBar( MString title, int count )
{
	if( MGlobal::mayaState() == MGlobal::kInteractive )
	{
		MString cmd = "progressBar -e -bp -ii true -st \"";
		cmd += title;
		cmd += "\" -max ";
		cmd += count;
		cmd += " $gMainProgressBar;";

		MGlobal::executeCommand( cmd );
	}
}

void
StepProgressBar( int step )
{
	if( MGlobal::mayaState() == MGlobal::kInteractive )
	{
		MString cmd = "progressBar -e -s ";
		cmd += step;
		cmd += " $gMainProgressBar;";

		MGlobal::executeCommand( cmd );
	}
}

bool
ProgressBarCancelled()
{
	if( MGlobal::mayaState() == MGlobal::kInteractive )
	{
		int cmdResult = 0;
		MGlobal::executeCommand( "progressBar -q -isCancelled $gMainProgressBar", cmdResult );

		return ( cmdResult != 0 );
	}

	return false;
}

void
EndProgressBar()
{
	if( MGlobal::mayaState() == MGlobal::kInteractive )
	{
		MGlobal::executeCommand( "progressBar -e -ep $gMainProgressBar;" );
	}
}

ZELOS_NAMESPACE_END

