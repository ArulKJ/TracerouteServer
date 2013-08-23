#include "TraceRoute.h"


void DisplayHelp(int sockfd)
{
	char sHelpMessage[] ="\n\n**************************************"
			"*****************************************\n"
			"Traceroute Application\n"
			"**************************************"
			"*****************************************\n"
			"The various options available to you are:\n"
			"1) traceroute [destination_machine] - destination_machine=DNS"
			"hostname/IP Address\n"
			"2) traceroute [filename] - filename= File containing a list of"
			" hostname/IP Adresses\n"
			"3) traceroute me - A traceroute to your machine\n"
			"4) help - Display this help text\n"
			"5) quit - Exit the application\n\n\n";

	if (send(sockfd,sHelpMessage,strlen(sHelpMessage),0) != -1)
	{/*..*/}

}

void DisplayHelpServer() {
	printf("---------------------------------------------------------\n"
			"To invoke the Traceroute server please enter:\n"
			"./traceroute PORT <port_no> RATE <no_requests,duration>"
			" MAX_USERS <max_user> STRICT_DEST <0/1>\n"
			"Any or all of these parameters are optional\n"
			"<port_no> should be between 1025 and 65536\n"
			"--------------------------------------------------------\n");
}
