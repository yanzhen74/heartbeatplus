#ifndef POAC_HEARTBEAT_CLIENT
#define POAC_HEARTBEAT_CLIENT


#include <iostream>
#include <string>

#if !defined( __GNUC__)

#include <tchar.h>
#endif

#include <exception>

namespace poac{namespace net
{
	class CHeartBeatClient
	{
	public:
		CHeartBeatClient();
		int Run(const char *url, const char *name);
		~CHeartBeatClient();
	public:
		bool IsTimedOut();
		bool IsConnected(int sock);
		bool IsConnectStatusChanged();
		void Reset();
	protected:
		volatile bool m_bTimedOut;
		volatile bool m_bConnectStatusChanged;
	private:
		void __Log(const char *msg);
		char* __date();
	};
}
}

#endif