#include <assert.h>
//#include <libc.h>
#include <stdio.h>
#include <windows.h>
#include "nanomsg/nn.h"
#include "nanomsg/survey.h"
#include <string.h>
#include <time.h>
#include "../include/client.h"

using namespace std;
using namespace poac;
using namespace poac::net;

#define SERVER "server"
#define CLIENT "client"
#define DATE   "DATE"

int server (const char *url)
{
	int sock = nn_socket (AF_SP, NN_SURVEYOR);
	int millis = (int)(1000);
	int rc = nn_setsockopt (sock, NN_SOL_SOCKET, NN_RCVTIMEO,
		&millis, sizeof (millis));
	if (rc != 0)
	{
		printf("Can't set recv timeout");
		return -1;
	}
	//assert (rc == 0, "Can't set recv timeout");
	//assert (sock >= 0);
	if (sock < 0)
	{
		return -1;
	}
	assert (nn_bind (sock, url) >= 0);
	
	// 输出服务器已启动
	printf("SERVER: Started and listening on %s\n", url);
	fflush(stdout);
	
	while(1)
	{
		// 发送心跳请求
		int sz_d = strlen(DATE) + 1; // '\0' too
		printf ("SERVER: SENDING DATE SURVEY REQUEST\n");
		fflush(stdout);
		
		int bytes = nn_send (sock, DATE, sz_d, 0);
		if (bytes < 0) {
			// 发送失败（可能没有客户端连接），但继续循环
			//printf("SERVER: Could not send survey (no clients connected?)\n");
			//fflush(stdout);
		}
		else {
			assert (bytes == sz_d);
		}

		// 等待1秒钟，同时检查是否有响应到达（非阻塞接收）
		DWORD startTime = GetTickCount();
		while((GetTickCount() - startTime) < 1000)
		{
			// 非阻塞接收响应
			char *buf = NULL;
			int recv_bytes = nn_recv (sock, &buf, NN_MSG, NN_DONTWAIT);
			if (recv_bytes >= 0)
			{
				printf ("SERVER: RECEIVED \"%s\" SURVEY RESPONSE; SUM:%d\n", buf, nn_get_statistic(sock, NN_STAT_CURRENT_CONNECTIONS));
				fflush(stdout);
				nn_freemsg (buf);
			}
			else
			{
				// 没有消息到达，短暂休眠以避免过度消耗CPU
				Sleep(10);  // 休眠10毫秒
			}
		}
	}
	return nn_shutdown (sock, 0);
}

int main (const int argc, const char **argv)
{
	if (argc >= 2 && strncmp (SERVER, argv[1], strlen (SERVER)) == 0 )
		return server (argv[2]);
	else if (argc >= 3 && strncmp (CLIENT, argv[1], strlen (CLIENT)) == 0 )
	{
		CHeartBeatClient hbc;
		return hbc.Run(argv[2], argv[3]);
		//return client (argv[2], argv[3]);
	}
	else
	{
		fprintf (stderr, "Usage: heartbeat %s|%s <URL> <ARG> ...\n",
			SERVER, CLIENT);
		return 1;
	}
}