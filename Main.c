#include "TraceRoute.h"



int main(int argc,char** argv)
{
	int socketFileDesc = 0;
	char logText[] = "Connection established with ";

	InitLog();

	if(!InitServerParams(argc, argv))
	{
		DisplayHelpServer();
		return 0;
	}
		
	socketFileDesc = InitServer();


	if (socketFileDesc >= 1)
	{
			struct sockaddr_storage client_address;
			struct sockaddr_in* netAddr;
			socklen_t address_size;
			int acceptingFileDesc,ipAddr;
			char clientIP[INET6_ADDRSTRLEN];


			address_size = sizeof(client_address);

			WriteToLog("Waiting for connections...");

			while(1)
			{

				acceptingFileDesc = accept(socketFileDesc,
					(struct sockaddr *) &client_address, &address_size);

				netAddr = (struct sockaddr_in*)&client_address;
				ipAddr = netAddr->sin_addr.s_addr;
				inet_ntop( AF_INET, &ipAddr, clientIP, INET_ADDRSTRLEN );


				if (acceptingFileDesc != -1)
				{
					strcat(logText,clientIP);
					WriteToLog(logText);

					siUsers[active_sessions].ip = clientIP;
					siUsers[active_sessions].sockfd = acceptingFileDesc;
					siUsers[active_sessions].rateNo = 0;
					siUsers[active_sessions].window_time = 0;

					CreateNewSession(acceptingFileDesc);
					strcpy(logText, "Connection established with ");

				}

			}
		}
		else
			WriteToLog("Server Initialization failed!! Exiting now.");


	return 0;
}





