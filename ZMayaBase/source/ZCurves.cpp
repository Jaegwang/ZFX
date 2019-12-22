//-------------//
// ZCurves.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2018.08.22                               //
//-------------------------------------------------------//

#include <ZelosBaseForMaya.h>

ZELOS_NAMESPACE_BEGIN

bool
Convert( ZCurves& curves, const MObjectArray& curveObjs, const MMatrixArray& worldMats, bool toResample )
{
	MStatus stat = MS::kSuccess;

	if( curveObjs.length() != worldMats.length() )
	{
		cout << "Error@Convert(): Invalid input data." << endl;
		return false;
	}

	int numCurves = (int)curveObjs.length();

	vector<MPointArray> cvs;
	{
        FOR( i, 0, numCurves )
        {
            MFnNurbsCurve curveFn( curveObjs[i], &stat );

            if( stat != MS::kSuccess ) { continue; }
            if( curveFn.isIntermediateObject() ) { continue; }

            const int numCVs = curveFn.numCVs();

            if( numCVs < 2 ) { continue; }

            MPointArray crv( numCVs );

            if( toResample )
            {
                // Without this line, MFnNurbsCurve::findParamFromLength() may go into a deadlock!
                curveFn.updateCurve();

                const double curveLength = curveFn.length();
                const double dl = curveLength / double(numCVs-1);
                double l=0.0, t=0.0;

                FOR( j, 0, numCVs )
                {
                    t = curveFn.findParamFromLength( l );
                    curveFn.getPointAtParam( t, crv[j], MSpace::kObject );
                    l += dl;
                }
            }
            else
            {
				curveFn.getCVs( crv, MSpace::kObject );
            }

            cvs.push_back( crv );
        }
	}

	MMatrixArray wMats;
	{
		FOR( i, 0, numCurves )
		{
			MFnNurbsCurve curveFn( curveObjs[i], &stat );

			if( stat != MS::kSuccess ) { continue; }
			if( curveFn.isIntermediateObject() ) { continue; }

			const int numCVs = curveFn.numCVs();

			if( numCVs < 2 ) { continue; }

			wMats.append( worldMats[i] );
		}
	}

	numCurves = (int)cvs.size();

	// Don't use OpenMP here!
	FOR( i, 0, numCurves )
	{
		MPointArray& crv = cvs[i];
		const MMatrix& wMat = wMats[i];
		const int nCVs = (int)crv.length();

		FOR( j, 0, nCVs )
		{
			ApplyXForm( crv[j], wMat );
		}
	}

	bool toReAlloc = false;
	{
		if( curves.numCurves() != numCurves ) {

			toReAlloc = true;

		} else {

			const ZIntArray& nCVs = curves.numCVs();

			FOR( i, 0, numCurves )
			{
				if( nCVs[i] != (int)cvs[i].length() )
				{
					toReAlloc = true;
					break;
				}
			}

		}

	}

	if( toReAlloc )
	{
		curves.reset();

		ZIntArray numCVs;
		{
			numCVs.setLength( numCurves );

			FOR( i, 0, numCurves )
			{
				numCVs[i] = (int)cvs[i].length();
			}
		}

		curves.set( numCVs );
	}

	// Don't use OpenMP here!
	FOR( i, 0, numCurves )
	{
		FOR( j, 0, curves.numCVs(i) )
		{
			Copy( curves.cv(i,j), cvs[i][j] );
		}
	}

	return true;
}

bool
Convert( ZCurves& curves, const MObjectArray& curveObjs, const MMatrixArray& worldMats, bool toResample, const ZCharArray& onOff )
{
	MStatus stat = MS::kSuccess;

	if( curveObjs.length() != worldMats.length() )
	{
		cout << "Error@Convert(): Invalid input data." << endl;
		return false;
	}

	int numCurves = (int)curveObjs.length();

	if( numCurves != onOff.length() )
	{
		cout << "Error@Convert(): Invalid onOff data." << endl;
		return false;
	}

	vector<MPointArray> cvs;
	{
        FOR( i, 0, numCurves )
        {
            if( !onOff[i] ) { continue; }

            MFnNurbsCurve curveFn( curveObjs[i], &stat );

            if( stat != MS::kSuccess ) { continue; }
            if( curveFn.isIntermediateObject() ) { continue; }

            const int numCVs = curveFn.numCVs();

            if( numCVs < 2 ) { continue; }

            MPointArray crv( numCVs );

            if( toResample )
            {
                // Without this line, MFnNurbsCurve::findParamFromLength() may go into a deadlock!
                curveFn.updateCurve();

				const double curveLength = curveFn.length();
				const double dl = curveLength / double(numCVs-1);
				double l=0.0, t=0.0;

				FOR( j, 0, numCVs )
				{
					t = curveFn.findParamFromLength( l );
					curveFn.getPointAtParam( t, crv[j], MSpace::kObject );
					l += dl;
				}
            }
            else
            {
				curveFn.getCVs( crv, MSpace::kObject );
            }

			cvs.push_back( crv );
		}
	}

	MMatrixArray wMats;
	{
		FOR( i, 0, numCurves )
		{
			if( !onOff[i] ) { continue; }

			MFnNurbsCurve curveFn( curveObjs[i], &stat );

			if( stat != MS::kSuccess ) { continue; }
			if( curveFn.isIntermediateObject() ) { continue; }

			const int numCVs = curveFn.numCVs();

			if( numCVs < 2 ) { continue; }

			wMats.append( worldMats[i] );
		}
	}

	numCurves = (int)cvs.size();

	// Don't use OpenMP here!
	FOR( i, 0, numCurves )
	{
		MPointArray& crv = cvs[i];
		const MMatrix& wMat = wMats[i];
		const int nCVs = (int)crv.length();

		FOR( j, 0, nCVs )
		{
			ApplyXForm( crv[j], wMat );
		}
	}

	bool toReAlloc = false;
	{
		if( curves.numCurves() != numCurves ) {

			toReAlloc = true;

		} else {

			const ZIntArray& nCVs = curves.numCVs();

			FOR( i, 0, numCurves )
			{
				if( nCVs[i] != (int)cvs[i].length() )
				{
					toReAlloc = true;
					break;
				}
			}

		}

	}

	if( toReAlloc )
	{
		curves.reset();

		ZIntArray numCVs;
		{
			numCVs.setLength( numCurves );

			FOR( i, 0, numCurves )
			{
				numCVs[i] = (int)cvs[i].length();
			}
		}

		curves.set( numCVs );
	}

	// Don't use OpenMP here!
	FOR( i, 0, numCurves )
	{
		FOR( j, 0, curves.numCVs(i) )
		{
			Copy( curves.cv(i,j), cvs[i][j] );
		}
	}

	return true;
}

ZELOS_NAMESPACE_END

