//----------//
// ZPtc.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
//         Jaegwang Lim @ Dexter Studios                 //
//         Nayoung Kim @ Dexter Studios                  //
// last update: 2016.09.20                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

ZPtc::ZPtc()
{
	reset();
}

void
ZPtc::reset()
{
	name = "";
	lIdx = 0;
	gUid = -1;
	gClr = ZColor::white();
	aabb = ZBoundingBox();
	tScl = 1.f;

	uid.clear(); uid.shrink_to_fit();
	pos.clear(); pos.shrink_to_fit();
	vel.clear(); vel.shrink_to_fit();
	rad.clear(); rad.shrink_to_fit();
	clr.clear(); clr.shrink_to_fit();
	nrm.clear(); nrm.shrink_to_fit();
	vrt.clear(); vrt.shrink_to_fit();
	dst.clear(); dst.shrink_to_fit();
	sdt.clear(); sdt.shrink_to_fit();
	uvw.clear(); uvw.shrink_to_fit();
	age.clear(); age.shrink_to_fit();
	lfs.clear(); lfs.shrink_to_fit();
	sts.clear(); sts.shrink_to_fit();
	typ.clear(); typ.shrink_to_fit();

}

int
ZPtc::count() const
{
	return pos.length();
}

int 
ZPtc::numAttributes() const
{
	int num = 0;
	
	if(uid.length()) num++;
	if(pos.length()) num++;
	if(vel.length()) num++;
	if(rad.length()) num++;
	if(clr.length()) num++;
	if(nrm.length()) num++;
	if(vrt.length()) num++;
	if(dst.length()) num++;
	if(sdt.length()) num++;
	if(uvw.length()) num++;
	if(age.length()) num++;
	if(lfs.length()) num++;
	if(sts.length()) num++;
	if(typ.length()) num++;
	
	return num;
}

ZPtc&
ZPtc::operator=( const ZPtc& other )
{
	name = other.name;
	gUid = other.gUid;
	gClr = other.gClr;
	aabb = other.aabb;
	tScl = other.tScl;

	uid = other.uid;
	pos = other.pos;
	vel = other.vel;
	rad = other.rad;
	clr = other.clr;
	nrm = other.nrm;
	vrt = other.vrt;
	dst = other.dst;
	sdt = other.sdt;
	uvw = other.uvw;
	age = other.age;
	lfs = other.lfs;
	sts = other.sts;
	typ = other.typ;

	return (*this);
}

void
ZPtc::remove( const ZIntArray& delList )
{
	uid.remove( delList );
	pos.remove( delList );
	vel.remove( delList );
	rad.remove( delList );
	clr.remove( delList );
	nrm.remove( delList );
	vrt.remove( delList );
	dst.remove( delList );
	sdt.remove( delList );
	uvw.remove( delList );
	age.remove( delList );
	lfs.remove( delList );
	sts.remove( delList );
	typ.remove( delList );
}

double
ZPtc::usedMemorySize( ZDataUnit::DataUnit dataUnit ) const
{
	double size = 0.0;

	size += uid.usedMemorySize( dataUnit );
	size += pos.usedMemorySize( dataUnit );
	size += vel.usedMemorySize( dataUnit );
	size += rad.usedMemorySize( dataUnit );
	size += clr.usedMemorySize( dataUnit );
	size += nrm.usedMemorySize( dataUnit );
	size += vrt.usedMemorySize( dataUnit );
	size += dst.usedMemorySize( dataUnit );
	size += sdt.usedMemorySize( dataUnit );
	size += uvw.usedMemorySize( dataUnit );
	size += age.usedMemorySize( dataUnit );
	size += lfs.usedMemorySize( dataUnit );
	size += sts.usedMemorySize( dataUnit );
	size += typ.usedMemorySize( dataUnit );

	return size;
}

int
ZPtc::deadList( ZIntArray& list )
{
	list.reserve( typ.length()/3 );

	for( int n=0; n<typ.length(); ++n )
	{
		if( typ[n] == PTC_DEL ) list.push_back( n );
	}

	return list.length();
}

bool
ZPtc::save( const char* filePathName ) const
{
	ofstream fout( filePathName, ios::out|ios::binary|ios::trunc );

	if( fout.fail() || !fout.is_open() )
	{
		cout << "Error@ZPtc::save(): Failed to save file: " << filePathName << endl;
		return false;
	}

	const int n = count();
	fout.write( (char*)&n, sizeof(int) );

	name.write( fout );
	fout.write( (char*)&lIdx, sizeof(int) );
	fout.write( (char*)&gUid, sizeof(int) );
	gClr.write( fout );
	aabb.write( fout );
	fout.write( (char*)&tScl, sizeof(float) );

	if( uid.size() )
	{
		ZString("uid").write( fout, true );
		uid.write( fout, false );
	}

	if( pos.size() )
	{
		ZString("pos").write( fout, true );
		pos.write( fout, false );
	}

	if( vel.size() )
	{
		ZString("vel").write( fout, true );
		vel.write( fout, false );
	}

	if( rad.size() )
	{
		ZString("rad").write( fout, true );
		rad.write( fout, false );
	}

	if( clr.size() )
	{
		ZString("clr").write( fout, true );
		clr.write( fout, false );
	}

	if( nrm.size() )
	{
		ZString("nrm").write( fout, true );
		nrm.write( fout, false );
	}

	if( vrt.size() )
	{
		ZString("vrt").write( fout, true );
		vrt.write( fout, false );
	}

	if( dst.size() )
	{
		ZString("dst").write( fout, true );
		dst.write( fout, false );
	}

	if( sdt.size() )
	{
		ZString("sdt").write( fout, true );
		sdt.write( fout, false );
	}

	if( uvw.size() )
	{
		ZString("uvw").write( fout, true );
		uvw.write( fout, false );
	}

	if( age.size() )
	{
		ZString("age").write( fout, true );
		age.write( fout, false );
	}

	if( lfs.size() )
	{
		ZString("lfs").write( fout, true );
		lfs.write( fout, false );
	}

	fout.close();

	return true;
}

bool
ZPtc::load( const char* filePathName )
{
	ifstream fin( filePathName, ios::in|ios::binary );

	if( fin.fail() )
	{
		cout << "Error@ZPtc::load(): Failed to load file." << endl;
		reset();
		return false;
	}

	int n = 0;
	fin.read( (char*)&n, sizeof(int) );

	name.read( fin );
	fin.read( (char*)&lIdx, sizeof(int) );
	fin.read( (char*)&gUid, sizeof(int) );
	gClr.read( fin );
	aabb.read( fin );
	fin.read( (char*)&tScl, sizeof(float) );

	bool uid_exist = false;
	bool pos_exist = false;
	bool vel_exist = false;
	bool rad_exist = false;
	bool clr_exist = false;
	bool nrm_exist = false;
	bool vrt_exist = false;
	bool dst_exist = false;
	bool sdt_exist = false;
	bool uvw_exist = false;
	bool age_exist = false;
	bool lfs_exist = false;

	while( !fin.eof() )
	{
		ZString attr;
		attr.read( fin, true );

		if( attr == "uid" ) { uid.setLength(n); uid.read(fin,false ); uid_exist=true; }
		if( attr == "pos" ) { pos.setLength(n); pos.read(fin,false ); pos_exist=true; }
		if( attr == "vel" ) { vel.setLength(n); vel.read(fin,false ); vel_exist=true; }
		if( attr == "rad" ) { rad.setLength(n); rad.read(fin,false ); rad_exist=true; }
		if( attr == "clr" ) { clr.setLength(n); clr.read(fin,false ); clr_exist=true; }
		if( attr == "nrm" ) { nrm.setLength(n); nrm.read(fin,false ); nrm_exist=true; }
		if( attr == "vrt" ) { vrt.setLength(n); vrt.read(fin,false ); vrt_exist=true; }
		if( attr == "dst" ) { dst.setLength(n); dst.read(fin,false ); dst_exist=true; }
		if( attr == "sdt" ) { sdt.setLength(n); sdt.read(fin,false ); sdt_exist=true; }
		if( attr == "uvw" ) { uvw.setLength(n); uvw.read(fin,false ); uvw_exist=true; }
		if( attr == "age" ) { age.setLength(n); age.read(fin,false ); age_exist=true; }
		if( attr == "lfs" ) { lfs.setLength(n); lfs.read(fin,false ); lfs_exist=true; }
	}

	if( !uid_exist ) { uid.clear(); };
	if( !pos_exist ) { pos.clear(); };
	if( !vel_exist ) { vel.clear(); };
	if( !rad_exist ) { rad.clear(); };
	if( !clr_exist ) { clr.clear(); };
	if( !nrm_exist ) { nrm.clear(); };
	if( !vrt_exist ) { vrt.clear(); };
	if( !dst_exist ) { dst.clear(); };
	if( !sdt_exist ) { sdt.clear(); };
	if( !uvw_exist ) { uvw.clear(); };
	if( !age_exist ) { age.clear(); };
	if( !lfs_exist ) { lfs.clear(); };

	fin.close();

	return false;
}

bool
ZPtc::savePtc( const char* filename ) const
{
	std::fstream file( filename, std::fstream::out | std::fstream::binary | std::fstream::trunc );
	
	if( file.fail() || !file.is_open() )
	{
		cout << "Error@ZPtc::save(): Failed to save file: " << filename << endl;
		return false;
	}	
	
	const char* ffName = "ZPtc";		
	int size = strlen(ffName);
	
	file.write( (char*)&size,  sizeof(size)   );
	file.write( (char*)ffName, strlen(ffName) );		
	
	float version = 1.0;
	file.write( (char*)&version, sizeof(version) );
	int ptcCount = count();
	file.write( (char*)&ptcCount, sizeof(ptcCount) );
	int attrCount = numAttributes();
	file.write( (char*)&attrCount, sizeof(attrCount) );
	
	char* type = 0;
	char* name = 0;
	int  point = -1;		
	std::vector<int> buff;
	
	if( uid.length() ) // uid
	{
		name = (char*)"uid";
		file.write( (char*)name, strlen(name) );
		type = (char*)"i1";
		file.write( (char*)type, strlen(type) );
		
		buff.push_back(file.tellg());
		file.write( (char*)&point, sizeof(point) );		
	}
	if( pos.length() ) // pos
	{
		name = (char*)"pos";
		file.write( (char*)name, strlen(name) );		
		type = (char*)"f3";
		file.write( (char*)type, strlen(type) );
				
		buff.push_back(file.tellg());
		file.write( (char*)&point, sizeof(point) );		
	}
	if( vel.length() ) // vel
	{
		name = (char*)"vel";
		file.write( (char*)name, strlen(name) );		
		type = (char*)"f3";
		file.write( (char*)type, strlen(type) );		
		
		buff.push_back(file.tellg());
		file.write( (char*)&point, sizeof(point) );		
	}
	if( rad.length() ) // rad
	{
		name = (char*)"rad";
		file.write( (char*)name, strlen(name) );
		type = (char*)"f1";
		file.write( (char*)type, strlen(type) );
		
		buff.push_back(file.tellg());
		file.write( (char*)&point, sizeof(point) );		
	}
	if( clr.length() ) // clr
	{	
		name = (char*)"clr";
		file.write( (char*)name, strlen(name) );		
		type = (char*)"f4";
		file.write( (char*)type, strlen(type) );
		
		buff.push_back(file.tellg());
		file.write( (char*)&point, sizeof(point) );			
	}
	if( nrm.length() ) // nrm
	{	
		name = (char*)"nrm";
		file.write( (char*)name, strlen(name) );		
		type = (char*)"f3";
		file.write( (char*)type, strlen(type) );
		
		buff.push_back(file.tellg());
		file.write( (char*)&point, sizeof(point) );			
	}
	if( vrt.length() ) // vrt
	{	
		name = (char*)"vrt";
		file.write( (char*)name, strlen(name) );		
		type = (char*)"f3";
		file.write( (char*)type, strlen(type) );
		
		buff.push_back(file.tellg());
		file.write( (char*)&point, sizeof(point) );			
	}	
	if( dst.length() ) // dst
	{	
		name = (char*)"dst";
		file.write( (char*)name, strlen(name) );		
		type = (char*)"f1";
		file.write( (char*)type, strlen(type) );
		
		buff.push_back(file.tellg());
		file.write( (char*)&point, sizeof(point) );			
	}	
	if( sdt.length() ) // sdt
	{	
		name = (char*)"sdt";
		file.write( (char*)name, strlen(name) );		
		type = (char*)"f1";
		file.write( (char*)type, strlen(type) );
		
		buff.push_back(file.tellg());
		file.write( (char*)&point, sizeof(point) );			
	}
	if( uvw.length() ) // uvw
	{	
		name = (char*)"uvw";
		file.write( (char*)name, strlen(name) );		
		type = (char*)"f3";
		file.write( (char*)type, strlen(type) );
		
		buff.push_back(file.tellg());
		file.write( (char*)&point, sizeof(point) );			
	}	
	if( age.length() ) // age
	{	
		name = (char*)"age";
		file.write( (char*)name, strlen(name) );		
		type = (char*)"f1";
		file.write( (char*)type, strlen(type) );
		
		buff.push_back(file.tellg());
		file.write( (char*)&point, sizeof(point) );			
	}
	if( lfs.length() ) // lfs
	{	
		name = (char*)"lfs";
		file.write( (char*)name, strlen(name) );		
		type = (char*)"f1";
		file.write( (char*)type, strlen(type) );
		
		buff.push_back(file.tellg());
		file.write( (char*)&point, sizeof(point) );			
	}
	if( sts.length() ) // sts
	{	
		name = (char*)"sts";
		file.write( (char*)name, strlen(name) );		
		type = (char*)"i1";
		file.write( (char*)type, strlen(type) );
		
		buff.push_back(file.tellg());
		file.write( (char*)&point, sizeof(point) );			
	}		
	if( typ.length() ) // typ
	{	
		name = (char*)"sts";
		file.write( (char*)name, strlen(name) );		
		type = (char*)"i1";
		file.write( (char*)type, strlen(type) );
		
		buff.push_back(file.tellg());
		file.write( (char*)&point, sizeof(point) );			
	}				
	
	int globalId = -1;
	ZColor globalColor(-1.0, -1.0, -1.0, -1.0);
	
	file.write( (char*)&globalId, sizeof(globalId) );
	file.write( (char*)&globalColor, sizeof(globalColor) );
	
	//char* boxType = (char*)"f6";
	//file.write( (char*)boxType, strlen(type) );
	//file.write( (char*)&aabb  , sizeof(aabb) );
	
	// TODO : write meta data block	
	{	
		
		
	}
	
	// Write body block	   
	int ix = 0;
	if( uid.length() )
	{
		int ptr = file.tellg();
		file.seekg(buff[ix++], ios::beg);
		file.write( (char*)&ptr, sizeof(ptr) );		
		file.seekg(ptr, ios::beg);
		file.write( (char*)&uid.at(0), sizeof(int)*uid.length() );
	}		
	if( pos.length() )
	{
		int ptr = file.tellg();
		file.seekg(buff[ix++], ios::beg);
		file.write( (char*)&ptr, sizeof(ptr) );		
		file.seekg(ptr, ios::beg);				
		file.write( (char*)&pos.at(0), sizeof(float)*3*pos.length() );
	}             
	if( vel.length() )
	{
		int ptr = file.tellg();
		file.seekg(buff[ix++], ios::beg);
		file.write( (char*)&ptr, sizeof(ptr) );		
		file.seekg(ptr, ios::beg);				
		file.write( (char*)&vel.at(0), sizeof(float)*3*vel.length() );		
	}
	if( rad.length() )
	{
		int ptr = file.tellg();
		file.seekg(buff[ix++], ios::beg);
		file.write( (char*)&ptr, sizeof(ptr) );		
		file.seekg(ptr, ios::beg);				
		file.write( (char*)&rad.at(0), sizeof(float)*rad.length() );
	}
	if( clr.length() )
	{
		int ptr = file.tellg();
		file.seekg(buff[ix++], ios::beg);
		file.write( (char*)&ptr, sizeof(ptr) );		
		file.seekg(ptr, ios::beg);				
		file.write( (char*)&clr.at(0), sizeof(float)*4*clr.length() );		
	}	
	if( nrm.length() )
	{
		int ptr = file.tellg();
		file.seekg(buff[ix++], ios::beg);
		file.write( (char*)&ptr, sizeof(ptr) );		
		file.seekg(ptr, ios::beg);				
		file.write( (char*)&nrm.at(0), sizeof(float)*3*nrm.length() );		
	}		
	if( vrt.length() )
	{
		int ptr = file.tellg();
		file.seekg(buff[ix++], ios::beg);
		file.write( (char*)&ptr, sizeof(ptr) );		
		file.seekg(ptr, ios::beg);				
		file.write( (char*)&vrt.at(0), sizeof(float)*3*vrt.length() );		
	}			
	if( dst.length() )
	{
		int ptr = file.tellg();
		file.seekg(buff[ix++], ios::beg);
		file.write( (char*)&ptr, sizeof(ptr) );		
		file.seekg(ptr, ios::beg);				
		file.write( (char*)&dst.at(0), sizeof(float)*dst.length() );		
	}				
	if( sdt.length() )
	{
		int ptr = file.tellg();
		file.seekg(buff[ix++], ios::beg);
		file.write( (char*)&ptr, sizeof(ptr) );		
		file.seekg(ptr, ios::beg);				
		file.write( (char*)&sdt.at(0), sizeof(float)*sdt.length() );		
	}					
	if( uvw.length() )
	{
		int ptr = file.tellg();
		file.seekg(buff[ix++], ios::beg);
		file.write( (char*)&ptr, sizeof(ptr) );		
		file.seekg(ptr, ios::beg);				
		file.write( (char*)&uvw.at(0), sizeof(float)*3*uvw.length() );		
	}						
	if( age.length() )
	{
		int ptr = file.tellg();
		file.seekg(buff[ix++], ios::beg);
		file.write( (char*)&ptr, sizeof(ptr) );		
		file.seekg(ptr, ios::beg);				
		file.write( (char*)&age.at(0), sizeof(float)*age.length() );		
	}							
	if( lfs.length() )
	{
		int ptr = file.tellg();
		file.seekg(buff[ix++], ios::beg);
		file.write( (char*)&ptr, sizeof(ptr) );		
		file.seekg(ptr, ios::beg);				
		file.write( (char*)&lfs.at(0), sizeof(float)*lfs.length() );		
	}								
	if( sts.length() )
	{
		int ptr = file.tellg();
		file.seekg(buff[ix++], ios::beg);
		file.write( (char*)&ptr, sizeof(ptr) );		
		file.seekg(ptr, ios::beg);				
		file.write( (char*)&sts.at(0), sizeof(int)*sts.length() );		
	}									
	if( typ.length() )
	{
		int ptr = file.tellg();
		file.seekg(buff[ix++], ios::beg);
		file.write( (char*)&ptr, sizeof(ptr) );		
		file.seekg(ptr, ios::beg);				
		file.write( (char*)&typ.at(0), sizeof(int)*typ.length() );		
	}									
	
	file.close();
	return true;
}

bool
ZPtc::loadPtc( const char* filename )
{
	std::fstream file( filename, std::fstream::in | std::fstream::binary );
	
	if( file.fail() || !file.is_open() )
	{
		return false;
	}
	
	int size;
	file.read( (char*)&size, sizeof(size) );
	
	char buff[32];
	file.read( (char*)buff, size ); buff[size] = 0;
	
	float version;
	file.read( (char*)&version, sizeof(version) );
	int ptcCount;
	file.read( (char*)&ptcCount, sizeof(ptcCount) );
	int attrCount;
	file.read( (char*)&attrCount, sizeof(attrCount) );
	
	reset();
	
	char name[32];
	char type[32];
	
	for(int i=0; i<attrCount; i++)
	{
		file.read( (char*)name, 3 ); name[3] = 0;		
		file.read( (char*)type, 2 ); type[2] = 0;
		
		int ptr;		
		file.read( (char*)&ptr, sizeof(ptr) );		
		
		if(ptr > 0)
		{			
			int cur = file.tellg();
			file.seekg(ptr, ios::beg);			
								
			if( strcmp(name, "uid") == 0 && strcmp(type, "i1") == 0 )
			{	
				uid.setLength(ptcCount);			
				file.read( (char*)&uid.at(0), sizeof(int)*ptcCount );
			}	
			else if( strcmp(name, "pos") == 0 && strcmp(type, "f3") == 0 )
			{
				pos.setLength(ptcCount);
				file.read( (char*)&pos.at(0), sizeof(float)*3*ptcCount );				
			}
			else if( strcmp(name, "vel") == 0 && strcmp(type, "f3") == 0 )
			{				
				vel.setLength(ptcCount);
				file.read( (char*)&vel.at(0), sizeof(float)*3*ptcCount );
			}
			else if( strcmp(name, "rad") == 0 && strcmp(type, "f3") == 0 )
			{				
				rad.setLength(ptcCount);
				file.read( (char*)&rad.at(0), sizeof(float)*ptcCount );
			}			
			else if( strcmp(name, "clr") == 0 && strcmp(type, "f4") == 0 )
			{				
				clr.setLength(ptcCount);
				file.read( (char*)&clr.at(0), sizeof(float)*4*ptcCount );
			}		
			else if( strcmp(name, "nrm") == 0 && strcmp(type, "f3") == 0 )
			{				
				nrm.setLength(ptcCount);
				file.read( (char*)&nrm.at(0), sizeof(float)*3*ptcCount );
			}		
			else if( strcmp(name, "vrt") == 0 && strcmp(type, "f3") == 0 )
			{				
				vrt.setLength(ptcCount);
				file.read( (char*)&vrt.at(0), sizeof(float)*3*ptcCount );
			}												
			else if( strcmp(name, "dst") == 0 && strcmp(type, "f1") == 0 )
			{				
				dst.setLength(ptcCount);
				file.read( (char*)&dst.at(0), sizeof(float)*ptcCount );
			}
			else if( strcmp(name, "sdt") == 0 && strcmp(type, "f1") == 0 )
			{				
				sdt.setLength(ptcCount);
				file.read( (char*)&sdt.at(0), sizeof(float)*ptcCount );
			}						
			else if( strcmp(name, "uvw") == 0 && strcmp(type, "f3") == 0 )
			{				
				uvw.setLength(ptcCount);
				file.read( (char*)&uvw.at(0), sizeof(float)*ptcCount );
			}									
			else if( strcmp(name, "age") == 0 && strcmp(type, "f1") == 0 )
			{				
				age.setLength(ptcCount);
				file.read( (char*)&age.at(0), sizeof(float)*ptcCount );
			}						
			else if( strcmp(name, "lfs") == 0 && strcmp(type, "f1") == 0 )
			{				
				lfs.setLength(ptcCount);
				file.read( (char*)&lfs.at(0), sizeof(float)*ptcCount );
			}		
			else if( strcmp(name, "sts") == 0 && strcmp(type, "i1") == 0 )
			{				
				sts.setLength(ptcCount);
				file.read( (char*)&sts.at(0), sizeof(int)*ptcCount );
			}		
			else if( strcmp(name, "typ") == 0 && strcmp(type, "i1") == 0 )
			{				
				typ.setLength(ptcCount);
				file.read( (char*)&typ.at(0), sizeof(int)*ptcCount );
			}																																													
			
			file.seekg(cur, ios::beg);					
		}
				
	}
	
	file.read( (char*)&gUid, sizeof(gUid) );
	file.read( (char*)&gClr, sizeof(gClr) );
	
	//file.read( (char*)type, 2 );
	//file.read( (char*)&aabb, sizeof(aabb) );	
		
	file.close();
	return true;	
}

void
ZPtc::drawPos( bool useGroupColor ) const
{
	if( useGroupColor )
	{
		glColor( gClr );
	}

	pos.drawPoints();
}

void
ZPtc::drawVel( const float maxVel )
{
    if(pos.size() == 0) return;
    
    if(pos.size() != clr.size())
    {
        clr.clear();
        clr.setLength(pos.size());
    }
    
    #pragma omp parallel for
    for(int n=0; n<pos.size(); n++)
    {
        const float weight = vel[n].length()/maxVel;
        clr[n] = ZColor(weight, weight, 1.0, 1.0);
    }
    
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_COLOR_ARRAY  );
    
    glVertexPointer( 3, GL_FLOAT, 0, &pos.at(0) );
    glColorPointer ( 4, GL_FLOAT, 0, &clr.at(0) );
    
    glDrawArrays( GL_POINTS, 0, pos.size() );    
    
    glDisableClientState( GL_COLOR_ARRAY  );
    glDisableClientState( GL_VERTEX_ARRAY );
    
}

ostream&
operator<<( ostream& os, const ZPtc& ptc )
{
	os << "<ZPtc>" << endl;

    cout << " Count      : " << ptc.count() << endl;
	cout << " Group ID   : " << ptc.gUid << endl;
	cout << " Group Color: " << ptc.gClr << endl;
	cout << " AABB       : " << ptc.aabb << endl;
	cout << " Time Scale : " << ptc.tScl << endl;
	cout << "Attributes)" << endl;
	cout << "   uid: " << ( ptc.uid.size() ? "O" : "X" ) <<endl;
	cout << "   pos: " << ( ptc.pos.size() ? "O" : "X" ) <<endl;
	cout << "   vel: " << ( ptc.vel.size() ? "O" : "X" ) <<endl;
	cout << "   rad: " << ( ptc.rad.size() ? "O" : "X" ) <<endl;
	cout << "   clr: " << ( ptc.clr.size() ? "O" : "X" ) <<endl;
	cout << "   nrm: " << ( ptc.nrm.size() ? "O" : "X" ) <<endl;
	cout << "   vrt: " << ( ptc.vrt.size() ? "O" : "X" ) <<endl;
	cout << "   dst: " << ( ptc.dst.size() ? "O" : "X" ) <<endl;
	cout << "   sdt: " << ( ptc.sdt.size() ? "O" : "X" ) <<endl;
	cout << "   uvw: " << ( ptc.uvw.size() ? "O" : "X" ) <<endl;
	cout << "   age: " << ( ptc.age.size() ? "O" : "X" ) <<endl;
	cout << "   lfs: " << ( ptc.lfs.size() ? "O" : "X" ) <<endl;
	cout << " Memory: " << ptc.usedMemorySize( ZDataUnit::zMegabytes ) << endl;

	os << endl;

	return os;
}

ZELOS_NAMESPACE_END

