//---------------//
// ZMaya_Print.h //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2016.01.27                               //
//-------------------------------------------------------//

#ifndef _ZMaya_Print_h_
#define _ZMaya_Print_h_

#include <ZelosBaseForMaya.h>

ZELOS_NAMESPACE_BEGIN

#define mPrintInfo(x)				MGlobal::displayInfo(x)
#define mPrintError(x) 				MGlobal::displayError(x)
#define mPrintWarning(x)			MGlobal::displayWarning(x)
#define ZCheckError(status,message) if(status!=MS::kSuccess) { MGlobal::displayError(message); return status; }

template <class T>
void mPrint( const T& a )
{ stringstream ss; ss<<a; mPrintInfo(ss.str().c_str()); }

template <class T1, class T2>
void mPrint( const T1& x, const T2& y )
{ stringstream ss; ss<<x<<" "<<y; mPrintInfo(ss.str().c_str()); }

template <class T1, class T2, class T3>
void mPrint( const T1& x, const T2& y, const T3& z )
{ stringstream ss; ss<< x<<" "<<y<<" "<<z; mPrintInfo(ss.str().c_str()); }

template <class T1, class T2, class T3, class T4>
void mPrint( const T1& x, const T2& y, const T3& z, const T4& w )
{ stringstream ss; ss<<x<<" "<<y<<" "<<z<<" "<<w; mPrintInfo(ss.str().c_str()); }

template <class T>
void mPrintLocator( const T& a )
{ mPrintInfo(MString("spaceLocator -p ")+a[0]+" "+a[1]+" "+a[2]+";"); }

template <class T>
void mPrintArray( const T& a )
{ FOR(i,0,a.size()) { stringstream ss; ss<<a[i]; mPrintInfo(ss.str().c_str()); } }

template <class T>
void PrintLocator( const T& a )
{ cout<<"spaceLocator -p "<<a.x<<" "<<a.y<<" "<<a.z<<";"<<endl; }

ZELOS_NAMESPACE_END

#endif

