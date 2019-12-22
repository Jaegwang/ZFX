//----------//
// Time.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2016.12.28                               //
//-------------------------------------------------------//

#include <ZelosBaseForMaya.h>

ZELOS_NAMESPACE_BEGIN

float
ZCurrentTime()
{
	return MAnimControl::currentTime().as( MTime::uiUnit() );
}

float
ZCurrentTime( MDataBlock& block, MObject& timeObj )
{
	MTime::Unit timeUnit = MTime::uiUnit();
	return (float)block.inputValue( timeObj ).asTime().as( timeUnit );
}

float
ZCurrentTimeStepSize()
{
	MTime::Unit timeUnit = MTime::uiUnit();

	switch( timeUnit )
	{
		case MTime::kFilm:			{ return 1/  24.f; } //   24 frames per second
		case MTime::kNTSCFrame:		{ return 1/  30.f; } //   30 frames per second
		case MTime::kHours:			{ return   3600.f; } // 3600 seconds
		case MTime::kMinutes:  		{ return     60.f; } //   60 seconds
		case MTime::kSeconds:  		{ return      1.f; } //    1 seconds
		case MTime::kMilliseconds:	{ return 1/1000.f; } // 1/1000 of a seconds
		case MTime::kGames:			{ return 1/  15.f; } //   15 frames per second
		case MTime::kPALFrame:		{ return 1/  25.f; } //   25 frames per second
		case MTime::kShowScan:		{ return 1/  48.f; } // twice the speed of film (48 frames per second)
		case MTime::kPALField:		{ return 1/  50.f; } // twice the speed of PAL  (50 frames per second)
		case MTime::kNTSCField:		{ return 1/  60.f; } // twice the speed of NTSC (60 frames per second)
		case MTime::k2FPS:			{ return 1/   2.f; } //    2 frames per second
		case MTime::k3FPS:			{ return 1/   3.f; } //    3 frames per second
		case MTime::k4FPS:			{ return 1/   4.f; } //    4 frames per second
		case MTime::k5FPS:			{ return 1/   5.f; } //    5 frames per second
		case MTime::k6FPS:			{ return 1/   6.f; } //    6 frames per second
		case MTime::k8FPS:			{ return 1/   8.f; } //    8 frames per second
		case MTime::k10FPS:			{ return 1/  10.f; } //   10 frames per second
		case MTime::k12FPS:			{ return 1/  12.f; } //   12 frames per second
		case MTime::k16FPS:			{ return 1/  16.f; } //   16 frames per second
		case MTime::k20FPS:			{ return 1/  20.f; } //   20 frames per second
		case MTime::k40FPS:			{ return 1/  40.f; } //   40 frames per second
		case MTime::k75FPS:			{ return 1/  75.f; } //   75 frames per second
		case MTime::k80FPS:			{ return 1/  80.f; } //   80 frames per second
		case MTime::k100FPS:		{ return 1/ 100.f; } //  100 frames per second
		case MTime::k120FPS:		{ return 1/ 120.f; } //  120 frames per second
		case MTime::k125FPS:		{ return 1/ 125.f; } //  125 frames per second
		case MTime::k150FPS:		{ return 1/ 150.f; } //  150 frames per second
		case MTime::k200FPS:		{ return 1/ 200.f; } //  200 frames per second
		case MTime::k240FPS:		{ return 1/ 240.f; } //  240 frames per second
		case MTime::k250FPS:		{ return 1/ 250.f; } //  250 frames per second
		case MTime::k300FPS:		{ return 1/ 300.f; } //  300 frames per second
		case MTime::k375FPS:		{ return 1/ 375.f; } //  375 frames per second
		case MTime::k400FPS:		{ return 1/ 400.f; } //  400 frames per second
		case MTime::k500FPS:		{ return 1/ 500.f; } //  500 frames per second
		case MTime::k600FPS:		{ return 1/ 600.f; } //  600 frames per second
		case MTime::k750FPS:		{ return 1/ 750.f; } //  750 frames per second
		case MTime::k1200FPS:		{ return 1/1200.f; } // 1200 frames per second
		case MTime::k1500FPS:		{ return 1/1500.f; } // 1500 frames per second
		case MTime::k2000FPS:		{ return 1/2000.f; } // 2000 frames per second
		case MTime::k3000FPS:		{ return 1/3000.f; } // 3000 frames per second
		case MTime::k6000FPS:		{ return 1/6000.f; } // 6000 frames per second
		default:      				{ return      1.f; }
	}
}

float
ZFPS()
{
	float fps = 24.0f;

	MTime::Unit unit = MTime::uiUnit();

	if( unit != MTime::kInvalid )
	{
		MTime time( 1.0, MTime::kSeconds );
		fps = static_cast<float>( time.as(unit) );
	}

	if( fps <= 0.f )
	{
		fps = 24.0f;
	}

	return fps;
}

void
SetPlayback( double min, double max )
{
	MStatus status = MS::kSuccess;

	MAnimControl anim;
	MTime minTime, maxTime, curTime;
	minTime.setValue( min );
	maxTime.setValue( max );

	anim.setMinTime( minTime );
	anim.setAnimationStartTime( minTime );

	anim.setMaxTime( maxTime );
	anim.setAnimationEndTime( maxTime );
}

ZELOS_NAMESPACE_END

