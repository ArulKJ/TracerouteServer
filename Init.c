
#include "TraceRoute.h"



/**************************************************
 * Name : InitServerParams
 * In	: int argc,char* argv[]
 * Out	: int (Success or Failure)
 * Initial function to set server paramters such as
 * STRICTDEST,MAXUSERS etc
 * Input variables are the command line arguments
 * sent to main
 ***************************************************/
int InitServerParams(int argc, char* argv[]) {

	int idx;
	int bPortPresent = FALSE,iPortLocation, iRateValue;
	int bRatePresent= FALSE,iRateLocation, iRateRequest, iRateDuration;
	int bMaxUserPresent=FALSE,iMaxUserLocation,iMaxUserValue;
	int bStrictDestPresent = FALSE,iStrictDestLocation;
	char logText[BUFSIZE] = "PARAMS :: ";
	char tmp[30] = "";

	WriteToLog("Initializing Server Params");

	// Variable to check if all the parameters in argv are explored
	int bFlag = FALSE;
	int aExploredParameter[argc];
	for (idx=0;idx <argc;idx++) {
		aExploredParameter[idx] = FALSE;
	}


	strcpy(gPort,"1216");
	gMaxUsers = 2;
	gStrictDest = 0;
	gRateDuration = 60;
	gRateRequest = 4;

	for (idx=1; idx <argc; idx++) {

		if(strcmp(argv[idx],"PORT")== 0) {

			// PORT Value should ideally follow the PORT parameter
			iPortLocation = idx+1;

			// Mark the parameters as explored
			aExploredParameter[idx] = TRUE;
			aExploredParameter[idx+1] = TRUE;

			// Check if the PORT Value is permissible and store in the variable
			if (iPortLocation < argc) {

				iRateValue = atoi(argv[iPortLocation]);

				if (iRateValue >= MIN_PORTNO && iRateValue <= MAX_PORTNO) {

					strcpy(gPort,argv[iPortLocation]);
					bPortPresent = TRUE;

				} else {

					WriteToLog("Error in PORT Value. Either it is non-existent"
							" or invalid value");
					return 0;
				}
			}

		} else if(strcmp(argv[idx],"RATE") == 0) {

			// RATE would be entered as a tuple X,Y folowing the RATE parameter
			// Extract number of requests per duration and
			// check if values are valid
			iRateLocation = idx+1;
			char sRateDuration[10],sRateRequest[10];
			int iIdxCnt = 0, iCnt1 = 0, iCnt2 = 0;
			aExploredParameter[idx] = TRUE;
			aExploredParameter[idx+1] = TRUE;

			// Parse the parameter following the RATE and extract
			// no of requests which preceeds the comma
			if (iRateLocation < argc) {

				for (iCnt1 = 0; iCnt1<strlen(argv[iRateLocation]); iCnt1++) {

					if(argv[iRateLocation][iCnt1] == COMMA) {

						sRateRequest[iIdxCnt] = EOL;
						break;

					} else {

						sRateRequest[iIdxCnt++] = argv[iRateLocation][iCnt1];
					}
				}

				// Parse the remainder and add to the Duration variable
				iIdxCnt = 0;
				for (iCnt2=iCnt1+1 ;
					iCnt2 < strlen(argv[iRateLocation]); iCnt2++) {

					if (argv[iRateLocation][iCnt2] >= ZERO &&
						argv[iRateLocation][iCnt2] <= NINE) {

						sRateDuration[iIdxCnt++] =  argv[iRateLocation][iCnt2];

					} else {

						WriteToLog("Error in value for number of requests in"
							"RATE parameter");
						return 0;
					}
				}

				// Check if the rate values are sane and set the variables
				sRateDuration[iIdxCnt]='\0';
				if (((iRateRequest = atoi(sRateRequest)) > 0) &&
					((iRateDuration = atoi(sRateDuration)) > 0)) {

					gRateRequest = iRateRequest;
					gRateDuration = iRateDuration;
					bRatePresent = TRUE;


				} else {

					WriteToLog("Error with value specified for RATE\n");
					return 0;
				}
			}

		}  else if(strcmp(argv[idx],"MAX_USERS") == 0) {

			// Check if MAX_USERS is specified and parse the
			// value to check if it is valid

			iMaxUserLocation = idx+1;
			aExploredParameter[idx] = TRUE;
			aExploredParameter[idx+1] = TRUE;

			if (iMaxUserLocation < argc) {

				if ((iMaxUserValue = atoi(argv[iMaxUserLocation])) > 0) {

					gMaxUsers = iMaxUserValue;
					bMaxUserPresent=TRUE;

				} else {

					WriteToLog("Error in value specified for MAX_USERS"
							" or it is non-existent");
					return 0;

				}
			}

		} else if(strcmp(argv[idx],"STRICT_DEST") == 0) {

			// Check if STRICT_DEST is provided and parse the next parameter
			// to validate it.
			iStrictDestLocation = idx+1;
			aExploredParameter[idx] = TRUE;
			aExploredParameter[idx+1] = TRUE;

			if (iStrictDestLocation < argc) {

				// Since STRICT_DEST can be 0, we need to verify if it is a
				// number before converting from string to int as atoi
				// returns 0 when it fails to convert.
				if((strlen(argv[iStrictDestLocation]) <= 1) &&
					(argv[iStrictDestLocation][0] == 48 ||
					argv[iStrictDestLocation][0] == 49)) {

					gStrictDest = atoi(argv[iStrictDestLocation]);
					bStrictDestPresent = TRUE;

				} else {

					WriteToLog("Error in value specified for STRICT_DEST"
							" or it is non-existent");
					return 0;
				}
			}
		}
	}

	// Sanity checking to see if all params are parsed
	if (argc >= 2) {

		// None valid parameter exist but number of parameters is more than
		// one
		if (!(bPortPresent || bRatePresent || bMaxUserPresent ||
				bStrictDestPresent )) {

			WriteToLog("Some invalid Parameters exist");
			return 0;
		}

		// Some spurious parameters exist that is not mapped to any
		// of the four parameter above
		bFlag = FALSE;
		for (idx=1;idx<argc;idx++) {
			if(aExploredParameter[idx] == FALSE) {

				bFlag = TRUE;
			}
		}
		if (bFlag){

			WriteToLog("ERROR : CHECK PARAMETERS\n");
			return 0;
		}
	}



	snprintf(tmp,29,"PORT:%s, ",gPort);
	strcat(logText,tmp);
	snprintf(tmp,29,"STRICTDEST:%d, ",gStrictDest);
	strcat(logText,tmp);
	snprintf(tmp,29,"MAXUSERS:%d, ",gMaxUsers);
	strcat(logText,tmp);
	snprintf(tmp,29,"RATE DURATION:%d, ",gRateDuration);
	strcat(logText,tmp);
	snprintf(tmp,29,"RATE:%d, ",gRateRequest);
	strcat(logText,tmp);
	snprintf(tmp,29,"TIMEOUT:%d ",TIMEOUT);
	strcat(logText,tmp);
	WriteToLog(logText);
	return 1;
}




/*********************************************************
 * Name	: InitServer
 * In	: -
 * Out	: int
 * Primary function to initialize the server. Returns file
 * descriptor of bound socket. Error message displayed on
 * failure
 *********************************************************/
int InitServer()
{
	struct addrinfo systemAddrInfo;
	struct addrinfo *serverInfo, *iterator;
	struct timeval timeOut;
	int serverConnectionStatus;
	int socketFileDesc;
	int noOfConnections = 2;

	WriteToLog("Server is being Initialized...");

	// Fill out the details of the system
	memset(&systemAddrInfo,0,sizeof(systemAddrInfo));
	systemAddrInfo.ai_family = AF_UNSPEC;		// IPV4 or IPV6 Independent
	systemAddrInfo.ai_socktype = SOCK_STREAM;	// For TCP Connection
	systemAddrInfo.ai_flags = AI_PASSIVE;		// Fix the system IP

	serverConnectionStatus =
			getaddrinfo(NULL, gPort, &systemAddrInfo, &serverInfo);

	if (serverConnectionStatus != 0) {
		WriteToLog ("Cannot intialize the addrinfo struct");
		return -1;
	}

	// iterate through results and get the first valid one

	for (iterator = serverInfo; iterator != NULL;
		iterator = iterator->ai_next)
	{
		if ((socketFileDesc = socket(iterator->ai_family,
			iterator->ai_socktype, iterator->ai_protocol)) == -1) {
			
			WriteToLog("Cannot get a Socket File Descriptor");
			continue;
		}

		int yes=1;
		if (setsockopt(socketFileDesc, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1) {
			return -1;
		}


		timeOut.tv_sec = TIMEOUT;
		timeOut.tv_usec = 0;
		if (setsockopt(socketFileDesc,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeOut,sizeof(struct timeval)) != 0)
			return -1;

		if (bind(socketFileDesc, iterator->ai_addr,
			iterator->ai_addrlen) == -1) {
			
			WriteToLog("Cannot bind File Desc");
			continue;
		}
		break;
	}

	// Check if bind was successful
	if (iterator == NULL) {
		
		WriteToLog("Bind was unsuccessful");
		return -1;
	}

	// Deallocate serverInfo
	freeaddrinfo(serverInfo);


	if (listen(socketFileDesc,noOfConnections) == -1) {
		
		WriteToLog("Error in listening");
	}

	WriteToLog("Initialization Complete...");
	return socketFileDesc;
}




/*********************************************************
 * Name : WriteToLog
 * In	: char *msg
 * Out	: void
 * Writes input text in 'msg' to log file and prints the
 * same text on server screen. Places a mutext lock on file
 * before writing and destroys it on completion.
 *********************************************************/
void WriteToLog(char *msg)
{
	FILE *log;
	char strLine[BUFSIZE];
	struct tm *sys_time = NULL;

	//lock file
	pthread_mutex_lock(&lock);

	time_t tNow = time(0);
	sys_time = localtime(&tNow);

	//get timestamp for each entry
	snprintf(strLine,BUFSIZE-1,"%d/%d/%d %d:%d:%d :%s",
			sys_time->tm_mon+1,sys_time->tm_mday,sys_time->tm_year-100,
			sys_time->tm_hour,sys_time->tm_min,sys_time->tm_sec,msg);

	printf("%s\n",strLine);

	//writing to file
	log = fopen(LOG_FILE,"a+");
	fprintf(log,"%s\n",strLine);
	fclose(log);

	//unlock and dispose lock
	pthread_mutex_unlock(&lock);
	pthread_mutex_destroy(&lock);

}




/**********************************************************
 * Name	: InitLog
 * In	: -
 * Out	: void
 * Initializes the log file for successive write operations
 * during the duration of the program.
 **********************************************************/
void InitLog()
{
	char strTime[8];
	char *prefix = "LOG";
	char *mode = "w";
	struct tm *sys_time = NULL;

	printf("Initializing logs...\n");

	time_t tNow = time(0);
	sys_time = localtime(&tNow);
	FILE *tmp;

	//get datetime for filename
	sprintf(strTime,"%d%d%d",sys_time->tm_mday,sys_time->tm_mon+1,sys_time->tm_year);
	sprintf(LOG_FILE,"%s_%s",prefix,strTime);

	tmp = fopen(LOG_FILE,"r");

	if(tmp != NULL)
	{
		fclose(tmp);
		mode = "a+";
	}

	//append to log
	tmp = NULL;
	tmp = fopen(LOG_FILE,mode);
	fprintf(tmp,"\n\n\t\t***APPLICATION BEGIN***\n");
	fclose(tmp);

	printf("Done...\n");
}

