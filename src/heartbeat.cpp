#include <assert.h>
//#include <libc.h>
#include <stdio.h>
#include "nanomsg/nn.h"
#include "nanomsg/survey.h"
#include "nanomsg/utils/sleep.h"
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
	while(1)
	{
		nn_sleep(1000); // wait for connections
		int sz_d = strlen(DATE) + 1; // '\0' too
		printf ("SERVER: SENDING DATE SURVEY REQUEST\n");
		int bytes = nn_send (sock, DATE, sz_d, 0);
		assert (bytes == sz_d);
		int count = 0;
		while (1)
		{
			char *buf = NULL;
			int bytes = nn_recv (sock, &buf, NN_MSG, 0);
			if (bytes == -ETIMEDOUT) break;
			if (bytes >= 0)
			{
				printf ("SERVER: RECEIVED \"%s\" SURVEY RESPONSE; SUM:%d\n", buf, nn_get_statistic(sock, NN_STAT_CURRENT_CONNECTIONS));
				count++;
				nn_freemsg (buf);
			}
			else
			{
				if (errno == ETIMEDOUT)
				{
					printf("SERVER: RECEIVED %d RESPONSES\n", count); 
					break;
				}
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
