/**************************************************
 * Primary header file that includes all necessary
 * libraries. Maintains all global variables and
 * function prototypes defined in other files
***************************************************/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include<pthread.h>
#include <netdb.h>
#include<time.h>
#include<unistd.h>
#include<ctype.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#ifndef TRACEROUTE_H_
#define TRACEROUTE_H_

#define TIMEOUT 30
#define STRICT_DEST 0
#define MIN_PORTNO 1025
#define MAX_PORTNO 65536
#define BUFSIZE 1024

#define COMMA ','
#define EOL '\0'
#define ZERO 48
#define NINE 57
#define MAX_USER 255
#define TRUE 1
#define FALSE 0


char LOG_FILE[12];
int active_sessions;
char gPort[6];
int gMaxUsers;
int gStrictDest;
int gRateDuration;
int gRateRequest;
pthread_mutex_t lock;


/*Custom data structure*/
typedef struct {

	int sockfd; //socket descriptor
	char *ip;	//client ip
	int rateNo; //rate
	time_t window_time; //rate window
}SessionInfo;
SessionInfo siUsers[MAX_USER]; // need to change this




/*--Init.c--*/
int InitServer();
int InitServerParams(int argc, char* argv[]);
void InitLog();
void WriteToLog(char *msg);
/*--Help.c--*/
void DisplayHelp(int sockfd);
void DisplayHelpServer();
/*--NetUtils.c--*/
int ParseUserInput(char *cmd, int sockfd);
void TraceRoute(char dest[],int sockfd);
void SanitizeInput(char *inStr, char *outStr);
void *WaitForUserInput(void *accDesc);
void CreateNewSession(int accDesc);


#endif /* TRACEROUTE_H_ */
