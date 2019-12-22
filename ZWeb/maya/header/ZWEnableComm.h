//----------------//
// ZWEnableComm.h //
//-------------------------------------------------------//
// author: Jaegwang Lim @ Dexter Studios                 //
// last update: 2017.09.01                               //
//-------------------------------------------------------//

#ifndef _ZWEnableComm_h_
#define _ZWEnableComm_h_

#include <ZelosBaseForMaya.h>

class ZWEnableComm : public MPxCommand
{
    public:

        static const MString name;

        ZWEnableComm() {};
        virtual ~ZWEnableComm() {};

        virtual MStatus doIt( const MArgList& args );
		virtual bool isUndoable() const { return false; }

        MStatus undoIt() { return MS::kSuccess; }     

        static void* creator() { return new ZWEnableComm(); }
        static MSyntax newSyntax() { MSyntax syntax; return syntax; }

};

#endif

