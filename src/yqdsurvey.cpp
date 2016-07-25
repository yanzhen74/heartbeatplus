#include <assert.h>
//#include <libc.h>
#include <stdio.h>
#include "nanomsg/nn.h"
#include "nanomsg/survey.h"
#include "nanomsg/utils/sleep.h"
#include <string.h>
#include <time.h>

using namespace std;


#define SERVER "server"
#define CLIENT "client"
#define DATE   "DATE"

char *date ()
{
	time_t raw = time (&raw);
	struct tm *info = localtime (&raw);
	char *text = asctime (info);
	text[strlen(text)-1] = '\0'; // remove '\n'
	return text;
}

int server (const char *url)
{
	int sock = nn_socket (AF_SP, NN_SURVEYOR);
	int millis = (int)(1000);
	int rc = nn_setsockopt (sock, NN_SOL_SOCKET, NN_RCVTIMEO,
		&millis, sizeof (millis));
	//assert (rc == 0, "Can't set recv timeout");
	//assert (sock >= 0);
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

int client (const char *url, const char *name)
{
	int sock = nn_socket (AF_SP, NN_RESPONDENT);
	int millis = (int)(3000);
	int rc = nn_setsockopt (sock, NN_SOL_SOCKET, NN_RCVTIMEO,
		&millis, sizeof (millis));
	//assert (rc == 0, "Can't set recv timeout");
	char *msg = new char[100];
	assert (sock >= 0);
	assert (nn_connect (sock, url) >= 0);
	bool isConnected = (1 == nn_get_statistic(sock, NN_STAT_CURRENT_CONNECTIONS));
	int count = 0;
	while (1)
	{
		char *buf = NULL;
		int bytes = nn_recv (sock, &buf, NN_MSG, 0);
		if (bytes >= 0)
		{
			printf ("CLIENT (%s): RECEIVED \"%s\" SURVEY REQUEST\n", name, buf);
			nn_freemsg (buf);
			char *d = date();
			sprintf(msg, "%s:%s", name, d);
			int sz_d = strlen(msg) + 1; // '\0' too
			printf ("CLIENT (%s): SENDING DATE SURVEY RESPONSE %d; SERVER:%d\n", name, count++, nn_get_statistic(sock, NN_STAT_CURRENT_CONNECTIONS));
			int bytes = nn_send (sock, msg, sz_d, 0);
			assert (bytes == sz_d);
		}
		else if (isConnected)
		{
			printf ("CLIENT (%s): RECEIVE Timed out; SERVER:%d\n", name, nn_get_statistic(sock, NN_STAT_CURRENT_CONNECTIONS));
		}
		if (isConnected != (1 == nn_get_statistic(sock, NN_STAT_CURRENT_CONNECTIONS)))
		{
			printf ("CLIENT (%s): Connected Status:%d\n", name, nn_get_statistic(sock, NN_STAT_CURRENT_CONNECTIONS));
		}
		isConnected = (1 == nn_get_statistic(sock, NN_STAT_CURRENT_CONNECTIONS));
	}
	return nn_shutdown (sock, 0);
}

int main (const int argc, const char **argv)
{
	if (strncmp (SERVER, argv[1], strlen (SERVER)) == 0 && argc >= 2)
		return server (argv[2]);
	else if (strncmp (CLIENT, argv[1], strlen (CLIENT)) == 0 && argc >= 3)
		return client (argv[2], argv[3]);
	else
	{
		fprintf (stderr, "Usage: survey %s|%s <URL> <ARG> ...\n",
			SERVER, CLIENT);
		return 1;
	}
}
