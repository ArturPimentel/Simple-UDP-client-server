/*
*    Code created by Artur Pimentel de Oliveira
*    netid: apiment2
*    Based in codes from Prof. Dong Wang lectures
*
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fnmatch.h>
#include <time.h>

#define MAX_LINE 256

int main(int argc, char * argv[]){
   FILE *fp;
   struct hostent *hp;
   struct sockaddr_in sin, server_addr;
   char *host;
   char buf[MAX_LINE];
   int s, len;
   int server_port;
   char *msg_buffer;
   clock_t t1, t2;
   
   if (argc==4) {
      host = argv[1];
   } else {
      fprintf(stderr, "usage: simplex-talk host or null message\n");
      exit(1);
   }

   /* translate host name into peer's IP address */
   hp = gethostbyname(host);
   
   if (!hp) {
      fprintf(stderr, "simplex-talk: unknown host: %s\n", host);
      exit(1);
   }

   /* build address data structure */
   bzero((char *)&sin, sizeof(sin));
   sin.sin_family = AF_INET;
   bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
   server_port = atoi(argv[2]);
   sin.sin_port = htons(server_port);
   
   /* active open */
   if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
      perror("simplex-talk: socket");
      exit(1);
   }

   /* get and send lines of text */
   if (fnmatch("*.txt", argv[3], FNM_PERIOD) == FNM_NOMATCH){
      msg_buffer = calloc(strlen(argv[3]), 1);
      strcpy(msg_buffer, argv[3]);
   }
   else{ /*if it is a file, read it*/
      FILE* file = fopen(argv[3],"r");

      if(file == NULL)
      {
         printf("null file\n");
      }
      else{
         fseek(file, 0, SEEK_END);
         long int size = ftell(file);
         rewind(file);

         msg_buffer = calloc(size + 1, 1);

         fread(msg_buffer,1,size,file);
      }
   }
   
   /*Send to server for encryption*/
   if(sendto(s, msg_buffer, strlen(msg_buffer), 0, (struct sockaddr *)&sin, sizeof(struct sockaddr))==-1){
      perror ("Client Send Error!\n");
      exit(1);
   }
   t1 = clock();

   /*Receive encrypted message*/
   int addr_len = sizeof(struct sockaddr);

   if( recvfrom(s, buf, sizeof(buf), 0,  (struct sockaddr *)&sin, &addr_len)==-1){
       perror("Receive error!\n");
       exit(1);
   }

   t2 = clock();

   /*Calculate RTT*/
   float diff = (((float)t2 - (float)t1) / CLOCKS_PER_SEC ) * 1000000;

   /*Decryption*/
   // 28 is the lenght of timestamp
   int outlenght = 28 + strlen(msg_buffer);
   char *outln;

   outln = calloc(outlenght + 1, 1);

   int i;
   for (i = 0; i < outlenght; ++i)
   {
      if(!(i % 2)){
         outln[i] = buf[outlenght - i - 1];
         outln[outlenght - i - 1] = buf[i];
      }
      else{
         outln[i] = buf[i];
      }
   }

   /*Print decrypted message*/
   printf("%s | RTT: %.2f us\n", outln, diff);
}