//------------------//
// ZWEXportComm.cpp //
//-------------------------------------------------------//
// author: Jaegwang Lim @ Dexter Studios                 //
// last update: 2017.09.01                               //
//-------------------------------------------------------//

#include <ZWExportComm.h>
#include <ZWebCurves.h>
#include <ZWStitchCurves.h>
#define STRINGER(s) #s

const MString ZWExportComm::name( "ZWebAbcExport" );

MStatus ZWExportComm::doIt( const MArgList& args )
{    
	MStatus stat = MS::kSuccess;

    int startFrame(0);
    int endFrame(0);
    MString filePath;

    args.get( 0, startFrame );
    args.get( 1, endFrame   );
    args.get( 2, filePath   );

    {
        std::stringstream ss;
        ss << "$__ZWExportStartFrame=" << startFrame << ";";
        ss << "$__ZWExportEndFrame=" << endFrame << ";";
        ss << "$__ZWExportFilePath=" << "\"" << filePath << "\"" << ";";
        MGlobal::executeCommand( ss.str().c_str() );        
    }

    // get ZWeb Curves
    {
        std::stringstream ss;
        ss << "string $__ZWebCurvesExportList[] = { ";

        std::map<std::string, ZWebCurves*>::iterator iter = ZWebCurves::instances.begin();
        const int num = ZWebCurves::instances.size();

        for( int n=1; iter != ZWebCurves::instances.end(); ++iter, ++n )
        {
            ss << "\"";
            std::string key = iter->first;
            ss << key;
            ss << "\"";

            if( n != num ) ss << ",";
        }

        ss << "};";
        MGlobal::executeCommand( ss.str().c_str() );
    }

    // get ZWStitch curves
    {
        std::stringstream ss;
        ss << "string $__ZWebStitchCurvesExportList[] = { ";

        std::map<std::string, ZWStitchCurves*>::iterator iter = ZWStitchCurves::instances.begin();
        const int num = ZWStitchCurves::instances.size();

        for( int n=1; iter != ZWStitchCurves::instances.end(); ++iter, ++n )
        {
            ss << "\"";
            std::string key = iter->first;
            ss << key;
            ss << "\"";            

            if( n != num ) ss << ",";
        }

        ss << "};";
        MGlobal::executeCommand( ss.str().c_str() );
    }

    const char* melComm = (const char*)STRINGER(

        \n/*maya mel head*/\n

        string $exportList;

        int $i=0;        
        for( ; $i<size($__ZWebCurvesExportList); $i++ )
        {            
            string $web = $__ZWebCurvesExportList[$i];            
            int $N = size( ($web+".outWebCurves") );
            
            int $j=0;
            for( ; $j<$N; $j++ )
            {             
                string $dest[] = `connectionInfo -dfs ($web+".outWebCurves"+"["+$j+"]")`;
                                
                string $names[];
                tokenize( $dest[0], ".", $names );
                
                if( $names[0] != "")
                {
                    string $pa[] = `listRelatives -parent -path $names[0]`;
                    $exportList += (" -root "+$pa[0]); 
                }
            }        
            
        }         
        
        $i=0;        
        for( ; $i<size($__ZWebStitchCurvesExportList); $i++ )
        {            
            string $web = $__ZWebStitchCurvesExportList[$i];            

            string $dests[] = `connectionInfo -dfs ($web+".outPivot")`;
            
            int $j=0;           
            for( ; $j<size($dests); $j++ )
            {
                string $names[];
                tokenize( $dests[$j], ".", $names );

                if( $names[0] != "")
                {
                    $exportList += (" -root "+$names[0]);
                }
            }
        }

        string $exportParam = (" -frameRange "+$__ZWExportStartFrame+" "+$__ZWExportEndFrame+" -file "+$__ZWExportFilePath+" ");
        AbcExport -j ( $exportParam + $exportList );

        \n/*maya mel foot*/\n
    );

    MGlobal::executeCommand( melComm );

    return stat;
}

