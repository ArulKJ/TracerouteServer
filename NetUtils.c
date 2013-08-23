#include "TraceRoute.h"



void SanitizeInput(char *inStr, char *outStr)
{
	// Input received from user is generally terminated by a CLRF
	// Extract actual input srting from the Received input
	int idx = 0;

	WriteToLog("Sanitizing client input...");

	for (idx=1; idx <strlen(inStr);idx++)
	{
		// Checking for CL followed by CF
		if (inStr[idx] == 10 && inStr[idx-1] == 13)
		{
			outStr[idx-1]= '\0';
			break;
		}
		else
			outStr[idx-1] = inStr[idx-1];
	}

	// NULL Terminating the output string
	outStr[idx-1] = '\0';

	WriteToLog("Sanitization complete...");
}


int GetCmdRate(int sockfd)
{
	time_t tNow = time(0);
	localtime(&tNow);
	int idx;
 	for(idx = 0;idx<gMaxUsers;idx++)
 	{
 		if(siUsers[idx].sockfd == sockfd)
 		{
 			if(siUsers[idx].window_time == 0) {
 				siUsers[idx].window_time = tNow;
 				siUsers[idx].rateNo++;
 				return siUsers[idx].rateNo;
 			}
 			else
 			{
 				if(tNow-siUsers[idx].window_time > gRateDuration)
 				{
 					siUsers[idx].window_time = tNow;
 					siUsers[idx].rateNo = 1;
 					return siUsers[idx].rateNo;
 				}
 				else
 				{
 					siUsers[idx].rateNo++;
 					return siUsers[idx].rateNo;
 				}
 			}
 		} 
 		
 	}
	return -1;
		
}


void RemoveUserInfo(int sockfd)
{
	int idx = 0,pos=0;

	for(idx = 0;idx<gMaxUsers;idx++)
	{
		if(siUsers[idx].sockfd == sockfd)
		{
			pos = idx;
			break;
		}
	}

	for(idx = pos;idx<gMaxUsers;idx++)
		siUsers[idx] = siUsers[idx+1];

	siUsers[gMaxUsers-1].ip = '\0';
	siUsers[gMaxUsers-1].sockfd = -1;
	siUsers[gMaxUsers-1].window_time = 0;
	siUsers[gMaxUsers-1].rateNo=0;
}




void TraceRoute(char dest[], int sockfd)
{
    FILE *pipeFile;
    char outBuff[BUFSIZE];
    char cmd[256],logText[256];
    char endTxt[] = "\t\t*****TRACE COMPLETE*****\n";

    snprintf(logText,255,
    		"Trace destination >> %s",dest);
    WriteToLog(logText);

    snprintf(cmd,255,"traceroute %s",dest);
    pipeFile = popen(cmd, "r");

    if (pipeFile == NULL)
    {
    	send(sockfd,"Error",255,0);
    }

    while(fgets(outBuff, BUFSIZE-1, pipeFile))
    {
    	if (send(sockfd,outBuff,strlen(outBuff),0) != -1)
   		{/*...*/}
    }

    if (send(sockfd,endTxt,strlen(endTxt),0) != -1)
    {/*...*/}

    pclose(pipeFile);
    WriteToLog("Traceroute complete...");
}





void EvalTraceOpts(char * pOptions, int sockfd)
{
	int idx,idx1,iFileNameCounter=0;
	char fileName[200]; 
	int bInvalidInput=FALSE;
	int bFlag=FALSE;
	int rate;
	FILE *fp;
	char sLine[200]; 
	char cmd[200];
	char logText[BUFSIZE] = "";

	for (idx=0; idx <strlen(pOptions); idx++)
	{
		if (pOptions[idx] == ' ')
		{
			fileName[iFileNameCounter] = '\0';
			bFlag=TRUE;
			break;
		}
		else
			fileName[iFileNameCounter++] = pOptions[idx];
	}


	if (!bFlag)
		fileName[iFileNameCounter] = '\0';


	for (idx1=idx; idx1 < strlen(pOptions); idx1++)
	{
		if(pOptions[idx1] == ' ')
			continue;
		else
			bInvalidInput = TRUE;
	}


	if (bInvalidInput)
	{
		WriteToLog("Invalid traceroute options");
		char message[] = "\nSome Extra characters in Traceroute command\n"
							"Use 'HELP' to get assistance\n";
								  
		if (send(sockfd,message,strlen(message),0) != -1)
		{}

		//DisplayHelp(sockfd);

		return;
	}

	if (strcmp(fileName,"me") == 0) {

		for(idx=0;idx<active_sessions;idx++)
		{
			if(siUsers[idx].sockfd == sockfd)
			{
				rate = GetCmdRate(sockfd);
				if(rate >=1 && rate <= gRateRequest ) {
					TraceRoute(siUsers[idx].ip,siUsers[idx].sockfd);
				}
				else
				{
					snprintf(logText,BUFSIZE-1,
							"Traceroute exceeded limit of %d per %d",gRateRequest,gRateDuration);
					WriteToLog(logText);
					char message[] = "\nMaximum number of requests exceeded."
									  " Please Wait for some time and try again\n";
					if (send(sockfd,message,strlen(message),0) != -1)
					{/*...*/}
												
				}
				return;
			}
		}

	}
	else
	{
		if((fp =fopen(fileName,"r")) == NULL)
		{
			bFlag = FALSE;
			for (idx=0; idx<strlen(fileName); idx++) {
				if (!(fileName[idx] == '.' || fileName[idx] == '-'
					|| (fileName[idx] >= 48 && fileName[idx] <=57)
					|| (fileName[idx] >= 65 && fileName[idx] <=90)
					|| (fileName[idx] >= 97 && fileName[idx] <=122))) {
					bFlag = TRUE;
				}
			}

			if (bFlag)
			{

				char message[]="\nInvalid DNS/IP entered\n"
								"Use 'HELP' to get assistance\n";
											
				if (send(sockfd,message,strlen(message),0) != -1) {}
				return;
			}

			bFlag = 1;
			if (gStrictDest){
				for(idx=0;idx<active_sessions;idx++)
				{
					if(siUsers[idx].sockfd == sockfd)
					{
						if (strcmp(fileName,siUsers[idx].ip) != 0) {
							bFlag = 0;
							break;
						}
					}
				}		
			}
			
			if(bFlag) {
				rate = GetCmdRate(sockfd);
				if(rate >=1 && rate <= gRateRequest ) {
					TraceRoute(fileName, sockfd);
				}
				else
				{
					snprintf(logText,BUFSIZE-1,
										"Traceroute per minute exceeded limit of %d",gRateRequest);
					WriteToLog(logText);
					char message[] = "\nMaximum number of requests exceeded."
									  " Please Wait for some time and try again\n";
					if (send(sockfd,message,strlen(message),0) != -1)
					{}
												
				}
				
			} else {

				WriteToLog("STRICT_DEST Violation, alerting user");
				char message[]="\nSrict Destination enabled by Server\n"
						"Can traceroute to your ip only\n";
				if (send(sockfd,message,strlen(message),0) != -1) {}
			}
		}
		else
		{

			while (fscanf(fp,"%s %s",sLine,cmd) != EOF) {
				printf("%s: %s\n",sLine,cmd);
				
				
				rate = GetCmdRate(sockfd);
				if(rate >=1 && rate <= gRateRequest ) 
				{
					TraceRoute(cmd, sockfd);
									
				}
				else
				{
					snprintf(logText,BUFSIZE-1,
						"Traceroute per minute exceeded limit of %d",gRateRequest);
					WriteToLog(logText);
										
					char message[] = "\nMaximum number of requests exceeded."
				  				     " Please Wait for some time and try again\n";
					if (send(sockfd,message,strlen(message),0) != -1)
					{}
														
				}
			}
			return;
		}
	}
}




int ParseUserInput(char *cmd, int sockfd)
{
	int idx;
	int bFlag=FALSE;
	char sCommand[100]; 
	char sOptions[200]; 
	char logText[] = "";
	
	sprintf(logText,"Client Sent :%s",cmd);
	WriteToLog(logText);

	WriteToLog("Parsing user input...");

	// Parse the user input and call functions appropriately
	for (idx=0;idx <strlen(cmd);idx++)
	{
		if (cmd[idx] == ' ')
		{
			sCommand[idx]='\0';
			bFlag=TRUE;
			break;
		}
		else
			sCommand[idx] = toupper(cmd[idx]);
	}


	if (!bFlag)
		sCommand[idx] = '\0';


	// Parse the options
	int idx1,idx2= 0;
	for (idx1=idx+1;idx1<strlen(cmd);idx1++)
		sOptions[idx2++] = cmd[idx1];


	sOptions[idx2] = '\0';


	// Parse the command and respective options
	if (strcmp(sCommand,"HELP") == 0)
	{
		WriteToLog("Sending HELP Information");
		DisplayHelp(sockfd);

	}
	else if (strcmp(sCommand,"QUIT") == 0)
	{
		WriteToLog("Attempting to release connection");

		char message[]="Goodbye!\n";
		if (send(sockfd,message,strlen(message),0) != -1)
		{/*...*/}
		return 1;
	}
	else if (strcmp(sCommand,"TRACEROUTE") == 0)
	{
	
		WriteToLog("Checking for Traceroute options...");
		EvalTraceOpts(sOptions, sockfd);
		
	}
	else
	{
		WriteToLog("Unknown command : Displaying HELP");
		char message[]="Unknown Command\nUse 'HELP' to get assistance\n";
		if (send(sockfd,message,strlen(message),0) != -1)
		{/*...*/}
	}

	WriteToLog("Parse/Processing complete...");
	return 0;

}




void *WaitForUserInput(void *accDesc)
{
	int sockfd  = (int)(intptr_t)accDesc;
	char strCmd[] = "";
	void *buffer = (void *)strCmd;
	char logText[BUFSIZE] = "";
	int retVal = 0;
	char timeoutMsg[] = "Session has expired due to inactivity...\n";

	while(1)
	{
		snprintf(logText,BUFSIZE-1,"Listening for User(%d) input...",sockfd);
		WriteToLog(logText);

		if (recv(sockfd, buffer, BUFSIZE-1, 0) != -1)
		{
			SanitizeInput((char*)buffer,strCmd);
			retVal = ParseUserInput(strCmd,sockfd);
		
			if(retVal == 1)
				break;
		}
		else
		{
			snprintf(logText,BUFSIZE-1,
					"TIMEOUT EXPIRED : User(%d) Inactive",sockfd);
			WriteToLog(logText);

			strcat(timeoutMsg,"Goodbye!\n");
			send(sockfd,timeoutMsg,(int)strlen(timeoutMsg),0);
			break;
		}

	}

	close(sockfd);
	RemoveUserInfo(sockfd);
	active_sessions--;

	snprintf(logText,BUFSIZE-1,
			"Connection(%d) closed, exiting thread...",sockfd);
	WriteToLog(logText);

	fflush(stdout);
	pthread_exit(0);
	return NULL;
}





void CreateNewSession(int accDesc)
{
	char *message = "\n\nWelcome to the Traceroute server\n";
	int len = strlen(message);
	pthread_t pth;

	if (active_sessions == gMaxUsers)
	{
		WriteToLog("MAX_USERS reached, connection refused");
		message = "Server busy, please try again later...\n";
		len = strlen(message);
		send(accDesc,message,len,0);
		close(accDesc);
		return;
	}
	else
	{
		send(accDesc,message,len,0);
		DisplayHelp(accDesc);
		pthread_create(&pth,NULL,WaitForUserInput,(void *)(intptr_t)accDesc);
		active_sessions++;
	}
}

