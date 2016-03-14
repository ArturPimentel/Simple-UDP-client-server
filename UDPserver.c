/*
*    Code created by Artur Pimentel de Oliveira
*    netid: apiment2
*    Based in codes from Prof. Dong Wang lectures
*
*/

#include  <stdio.h>
#include  <string.h>
#include  <stdlib.h>
#include  <sys/types.h>
#include  <sys/socket.h>
#include  <netinet/in.h>
#include  <netdb.h>
#include  <time.h>
#include  <sys/time.h>

#define  MAX_PENDING  5
#define  MAX_LINE  4096
#define	 MAX_TIMESTAMP_LEN 50

int main(int argc, char * argv[]) {  
	struct sockaddr_in sin, client_addr;  
	char buf[MAX_LINE]; 					/*Buffer that will store message from client*/
	char timestamp[MAX_TIMESTAMP_LEN];		/*String for timestamp making*/
	char *msg_buffer;						/*Buffer used to send message to cleint*/
	int len, addr_len, port_number;
	int s;
	time_t ltime;
	struct timeval tv;
	char *timeStr;
	char *token;

	if (argc==2) {
    	port_number = atoi(argv[1]);
	}
	else {
    	fprintf(stderr, "usage: simplex-talk host or null port number\n");
    	exit(1);
    }

	//* build address data structure */
	bzero((char *)&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(port_number);

	/* setup passive open */
	if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("simplex-talk: socket");
		exit(1);
	}

	if ((bind(s, (struct sockaddr *)&sin, sizeof(sin))) < 0) {
		perror("simplex-talk: bind");
		exit(1);
	}

	addr_len = sizeof (client_addr);
	bzero((char*)&buf, sizeof(buf)); //Clean buffer to make sure messages will not be corrupted inside the server

	while (1){
		// Start building Timestamp
		strcpy(timestamp, "Timestamp: ");

		// Wait for client message to arrive
		if( recvfrom(s, buf, sizeof(buf), 0,  (struct sockaddr *)&client_addr, &addr_len)==-1){
		    perror("Receive error!\n");
		    exit(1);
		}
		else{
			// Retrieving time according to Server's time zone
			ltime = time(NULL);
			gettimeofday(&tv, NULL);
			timeStr = asctime(localtime(&ltime));

			// Separating only the time of the day
			char microStr[8];
			char* token = strtok(timeStr, " ");
			int i;
			for (i = 0; i < 3; ++i){
			    token = strtok(NULL, " ");
			}

			// Building the timestamp
			strcat(timestamp, token);
			strcat(timestamp, ".");
			sprintf(microStr, "%ld", tv.tv_usec);
			strcat(timestamp, microStr);

			// Preparing the message to be encrypted
			msg_buffer = calloc(strlen(buf) + strlen(timestamp) + 3, 1);
			strcpy(msg_buffer, buf);
			strcat(msg_buffer, " ");
			strcat(msg_buffer, timestamp);
			// printf("\n-------------------------------------------\n");
			// printf("Mes. to send (unencrypted): \"%s\"\n", msg_buffer); //DEBUG
			// printf("\n-------------------------------------------\n");

			// Encrypt message
			int j;
			char temp;
			for (j = 0; j < strlen(msg_buffer)/2; ++j){
				if(j % 2 == 0){
					temp = msg_buffer[j];
				    msg_buffer[j] = msg_buffer[strlen(msg_buffer) - j - 1];
				    msg_buffer[strlen(msg_buffer) - j - 1] = temp;
				}
			}
			// printf("\n-------------------------------------------\n");
			// printf("Mes. to send (encrypted):   \"%s\"\n", msg_buffer); //DEBUG
			// printf("\n-------------------------------------------\n");

			// Send message to client
			if(sendto(s, msg_buffer, strlen(msg_buffer), 0, (struct sockaddr *)&client_addr, sizeof(struct sockaddr))==-1){
		    	perror ("Client Send Error!\n");
		    	exit(1);
		   }
		}
		
		// Clear buffers for next loop
		bzero((char*)&buf, sizeof(buf));
		bzero((char*)&timestamp, sizeof(timestamp));
		free(msg_buffer);
	}

	close(s);
}