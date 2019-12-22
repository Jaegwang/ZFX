//----------------------------//
// ZTriMeshConnectionInfo.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2018.10.24                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZTriMeshConnectionInfo::ZTriMeshConnectionInfo()
{}

ZTriMeshConnectionInfo::ZTriMeshConnectionInfo( const ZTriMesh& mesh )
{
	set( mesh );
}

void
ZTriMeshConnectionInfo::reset()
{
	_numVertices  = 0;
	_numTriangles = 0;

	v2v.reset();
	e2v.reset();
	t2v.reset();

	v2e.reset();
	e2e.reset();
	t2e.reset();

	v2t.reset();
	e2t.reset();
	t2t.reset();
}

void
ZTriMeshConnectionInfo::set( const ZTriMesh& mesh )
{
	reset();

	_numVertices  = mesh.numVertices();
	_numTriangles = mesh.numTriangles();

	v2t = mesh.v012;
}

void
ZTriMeshConnectionInfo::calculate_v2v()
{
    if( v2v.length() > 0 ) { return; } // already done

    v2v.reset();
    v2v.resize( _numVertices );

    FOR( iTriangle, 0, _numTriangles )
    {
        const int& v0 = v2t[iTriangle][0];
        const int& v1 = v2t[iTriangle][1];
        const int& v2 = v2t[iTriangle][2];

        ZIntArray& list0 = v2v[v0];
        if( list0.findTheFirstIndex( v1 ) < 0 ) { list0.append( v1 ); }
        if( list0.findTheFirstIndex( v2 ) < 0 ) { list0.append( v2 ); }

        ZIntArray& list1 = v2v[v1];
        if( list1.findTheFirstIndex( v2 ) < 0 ) { list1.append( v2 ); }
        if( list1.findTheFirstIndex( v0 ) < 0 ) { list1.append( v0 ); }

        ZIntArray& list2 = v2v[v2];
        if( list2.findTheFirstIndex( v0 ) < 0 ) { list2.append( v0 ); }
        if( list2.findTheFirstIndex( v1 ) < 0 ) { list2.append( v1 ); }
    }
}

void
ZTriMeshConnectionInfo::calculate_e2v()
{
    if( e2v.length() > 0 ) { return; } // already done

    ZTriMeshConnectionInfo::calculate_v2e();

    e2v.reset();
    e2v.resize( _numVertices );

    const int numEdges = v2e.length();

    FOR( iEdge, 0, numEdges )
    {
        const int& v0 = v2e[iEdge][0];
        const int& v1 = v2e[iEdge][1];

        e2v[v0].append( iEdge );
        e2v[v1].append( iEdge );
    }
}

void
ZTriMeshConnectionInfo::calculate_t2v()
{
    if( t2v.length() > 0 ) { return; } // already done

    ZTriMeshConnectionInfo::calculate_v2t();

    t2v.reset();
    t2v.resize( _numVertices );

    FOR( iTriangle, 0, _numTriangles )
    {
        const int& v0 = v2t[iTriangle][0];
        const int& v1 = v2t[iTriangle][1];
        const int& v2 = v2t[iTriangle][2];

        ZIntArray& list0 = t2v[v0];
        if( list0.findTheFirstIndex( iTriangle ) < 0 ) { list0.append( iTriangle ); }

        ZIntArray& list1 = t2v[v1];
        if( list1.findTheFirstIndex( iTriangle ) < 0 ) { list1.append( iTriangle ); }

        ZIntArray& list2 = t2v[v2];
        if( list2.findTheFirstIndex( iTriangle ) < 0 ) { list2.append( iTriangle ); }
    }
}

void
ZTriMeshConnectionInfo::calculate_v2e()
{
    if( v2e.length() > 0 ) { return; } // already done

    ZTriMeshConnectionInfo::calculate_v2v();

    v2e.reset();
    v2e.reserve( _numTriangles*2 );

    FOR( iVertex, 0, _numVertices )
    {
        const ZIntArray& list = v2v[iVertex];

        FOR( j, 0, list.length() )
        {
            const int& jVertex = list[j];

            if( iVertex < jVertex ) { v2e.append( ZInt2( iVertex, jVertex ) ); }
        }
    }
}

void
ZTriMeshConnectionInfo::calculate_e2e()
{
    // no thing to do (no meaning)
}

void
ZTriMeshConnectionInfo::calculate_t2e()
{
    if( t2e.length() > 0 ) { return; } // already done

    ZTriMeshConnectionInfo::calculate_e2v();

    const int numEdges = v2e.length();

    t2e.reset();
    t2e.setLength( numEdges );
    t2e.fill( ZInt2(-1) );

    FOR( iTriangle, 0, _numTriangles )
    {
        FOR( iVertex, 0, 3 )
        {
            const int& v0 = v2t[iTriangle][iVertex];
            const int& v1 = v2t[iTriangle][(iVertex+1)%3];

            const ZIntArray& list = e2v[v0];

            FOR( i, 0, list.length() )
            {
                const int& iEdge = list[i];

                const int& ev0 = v2e[iEdge][0];
                const int& ev1 = v2e[iEdge][1];

                if( ( ev0 == v1 ) || ( ev1 == v1 ) )
                {
                    if( t2e[iEdge][0] < 0 ) { t2e[iEdge][0] = iTriangle; }
                    else                    { t2e[iEdge][1] = iTriangle; }
                }
            }
        }
    }
}

void
ZTriMeshConnectionInfo::calculate_v2t()
{
    // v2t = mesh.triangles: already done
}

void
ZTriMeshConnectionInfo::calculate_e2t()
{
    if( e2t.length() > 0 ) { return; } // already done

    ZTriMeshConnectionInfo::calculate_e2v();

    e2t.reset();
    e2t.setLength( _numTriangles );
    e2t.fill( ZInt3(-1) );

    FOR( iTriangle, 0, _numTriangles )
    {
        FOR( iVertex, 0, 3 )
        {
            const int& v0 = v2t[iTriangle][iVertex];
            const int& v1 = v2t[iTriangle][(iVertex+1)%3];

            const ZIntArray& list = e2v[v0];

            FOR( i, 0, list.length() )
            {
                const int& iEdge = list[i];

                if( ( v2e[iEdge][0] == v1 ) || ( v2e[iEdge][1] == v1 ) )
                {
                    e2t[iTriangle][iVertex] = iEdge;
                    break;
                }
            }
        }
    }
}

void
ZTriMeshConnectionInfo::calculate_t2t()
{
    if( t2t.length() > 0 ) { return; } // already done

    ZTriMeshConnectionInfo::calculate_t2v();

    t2t.reset();
    t2t.setLength( _numTriangles );
    t2t.fill( ZInt3(-1) );

    FOR( iTriangle, 0, _numTriangles )
    {
        FOR( iVertex, 0, 3 )
        {
            const int& v0 = v2t[iTriangle][iVertex];
            const int& v1 = v2t[iTriangle][(iVertex+1)%3];

            const ZIntArray& list = t2v[v0];

            FOR( j, 0, list.length() )
            {
                const int& jTriangle = list[j];
                if( jTriangle == iTriangle ) { continue; }

                if( v2t[jTriangle][0] == v1 ) { t2t[iTriangle][iVertex]=jTriangle; break; }
                if( v2t[jTriangle][1] == v1 ) { t2t[iTriangle][iVertex]=jTriangle; break; }
                if( v2t[jTriangle][2] == v1 ) { t2t[iTriangle][iVertex]=jTriangle; break; }
            }
        }
    }
}

ostream&
operator<<( ostream& os, const ZTriMeshConnectionInfo& object )
{
	os << "<ZTriMeshConnectionInfo>" << endl;
	os << endl;

	return os;
}

ZELOS_NAMESPACE_END

