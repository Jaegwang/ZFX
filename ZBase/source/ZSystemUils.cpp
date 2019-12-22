//------------------//
// ZSystemUtils.cpp //
//-------------------------------------------------------//
// author: Wanho Choi @ Dexter Studios                   //
// last update: 2015.10.07                               //
//-------------------------------------------------------//

#include <ZelosBase.h>

ZELOS_NAMESPACE_BEGIN

bool ZGetLocalIPAddress( ZString& ipAddress )
{
	#ifdef OS_LINUX
		struct ifaddrs *ifa = (ifaddrs*)NULL, *ifp = (ifaddrs*)NULL;
		if( getifaddrs(&ifp) < 0 )
		{
			cout << "Error@LocalIPAddress(): Failed to getifaddrs()." << endl;
			return false;
		}

		int count = 0;

		for( ifa=ifp; ifa; ifa=ifa->ifa_next )
		{
			char ip[256];
			socklen_t salen;
			     if( ifa->ifa_addr->sa_family==AF_INET  ) { salen = sizeof( struct sockaddr_in  ); }
			else if( ifa->ifa_addr->sa_family==AF_INET6 ) { salen = sizeof( struct sockaddr_in6 ); }
			else                                          { continue; }

			if( getnameinfo( ifa->ifa_addr, salen, ip, sizeof(ip), NULL, 0, NI_NUMERICHOST ) < 0 )
			{
				cout << "Error@LocalIPAddress(): Failed to getnameinfo()." << endl;
				return false;
			}

			if( count++ == 1 )
			{
				ipAddress = ip;
				break;
			}
		}
	#endif

	return true;
}

void ZGetSystemInfo( ZString& systemInfo )
{
	ostringstream oss;

	ZString ip;
	ZGetLocalIPAddress( ip );
	oss << "\t\"local_IP_address\":\"" << ip << "\"," << endl;

	#ifdef OS_LINUX
		time_t timeval;	(void)time( &timeval );
		char computer[256]; if( gethostname( computer, 256 ) ) { sprintf( computer, "Unknown_machine" ); }
		struct utsname uts;
		if( uname( &uts ) == 0 )
		{
			oss << "\t\"number_of_processors\":\"" << omp_get_num_procs() << "\"," << endl;
			oss << "\t\"computer_name\":\"" << computer << "\"," << endl;
			oss << "\t\"system_name\":\"" << uts.sysname << "\"," << endl;
			oss << "\t\"machine_name\":\"" << uts.machine << "\"," << endl;
			oss << "\t\"node_name\":\"" << uts.nodename << "\"," << endl;
			oss << "\t\"kernel_version\":\"" << uts.release << "\"," << endl;
			oss << "\t\"kernel_date\":\"" << uts.version << "\"," << endl;
			string ctimeStr = ctime(&timeval);
			ctimeStr.erase( std::remove( ctimeStr.begin(), ctimeStr.end(), '\n' ), ctimeStr.end() );
			oss << "\t\"time_data\":\"" << ctimeStr << "\"" << endl;
		}
	#endif

	#ifdef OS_WINDOWS
		time_t timeval;	(void)time( &timeval );
		char ctimeStr[256]; ctime_s( ctimeStr, 256, &timeval );
		char computer[256]; if( gethostname( computer, 256 ) ) { sprintf_s( computer, "Unknown_machine" ); }
		OSVERSIONINFO versionInfo;
		versionInfo.dwOSVersionInfoSize = sizeof( versionInfo );
		if( GetVersionEx( &versionInfo ) )
		{
			ostringstream oss;
			oss << "\t\"number_of_processorss\":\"" << omp_get_num_procs() << "\"," << endl;
			oss << "\t\"computer_names\":\"" << computer << "\", " << endl;
			oss << "\t\"major_version\":\"" << versionInfo.dwMajorVersion << "\"," << endl;
			oss << "\t\"minor_version\":\"" << versionInfo.dwMinorVersion << "\"," << endl;
			oss << "\t\"build_number\":\"" << versionInfo.dwBuildNumber << "\"," << endl;
			oss << "\t\"platform_ID\":\"" << versionInfo.dwPlatformId << "\"," << endl;
			oss << "\t\"CSD_version\":\"" << versionInfo.szCSDVersion << "\"," << endl;
			oss << "\t\"time_data\":\"" << ctimeStr << "\"" << endl;
		}
	#endif

	systemInfo = oss.str();
}

ZELOS_NAMESPACE_END

