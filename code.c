#include<stdio.h>
#include<openssl/md5.h>
#include<sys/types.h> /*sys/socket.h uses sys/types.h*/
#include<sys/socket.h>/*defines constants such as SOCK_DGRAM used in socket based I/O*/
#include<netinet/in.h> /*defines structures such as sockaddr_in*/
#include<netdb.h>/*defines host ip addresses*/
#include<string.h>
#include<sys/errno.h>
#include<sys/time.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<unistd.h>
#include <sys/stat.h> 
#include<malloc.h>
#include<sys/signal.h>
#include<pthread.h>
#include <fcntl.h> 
#define SO_ORIGINAL_DST 80
FILE *file ;
int main()
{
char input[100];

int portnumber=0,cmp, sock,addr_len,client_sock;
struct sockaddr_in server,client;//creating struct sockaddr_in variables server and client
void *thread_handler();
//socket creation
if((sock = socket(AF_INET,SOCK_STREAM, 0)) == -1) 
{

perror("Socket: ");
exit(-1);
}
server.sin_family = AF_INET;
server.sin_port = htons(17000);
server.sin_addr.s_addr = INADDR_ANY;
bzero(&server.sin_zero, 8);
addr_len = sizeof(struct sockaddr_in);	
printf("\nSocket creation successfull\n");
//error checking to see if socket binding occurred or not
if((bind(sock,(struct sockaddr *)&server, addr_len)) == -1)
{ 
perror("bind");
exit(-1);
}
//listen for request
if((listen(sock, 5)) == -1)
{
perror("listen");
exit(-1);
}
printf("listening\n");	
while(1){
if((client_sock = accept(sock, (struct sockaddr *)&client, &addr_len)) == -1)
{
perror("accept");
exit(-1);
}

pthread_t thread_id;
printf("socket no main:%d\n",client_sock);
file= fopen("log.txt","rw");
//creating thread
if ((pthread_create(&thread_id,NULL,thread_handler,(void *)&client_sock))<0)
{
perror("Thread:");
exit(-1);
}
}//while
fclose(file);
return 1;
   
}

//function for thread handling
void *thread_handler(void *new_sock)
{	
int file_read,addr_len,n=1,n1=1;
int ser_sock = *(int *)new_sock, sockfd;
int size1;
struct sockaddr_in serv_addr_web,dest_addr,proxy_addr;

char ip[100];
char host_name[200];

//int dest_addr;
int size,tmp,line_counter =1;
char rec_buff[13000],rec_buff2[130],request[2000],filename[500],write_buff[200];
char iptable_command[1300];
int bytes_sent, bytes_received;
file_read =1;
time_t timestamp;

//int f_d = 0; 
     
char *status = (char *)malloc(500*sizeof(char));
char *req_ver = (char *)malloc(30*sizeof(char));
char *rec_buff1 = (char *)malloc(30*sizeof(char));
char *path = (char *)malloc(30*sizeof(char));
char *req_type = (char *)malloc(50*sizeof(char));
char *req_meth = (char *)malloc(30*sizeof(char));


//connect to the server requested.

if((sockfd = socket(AF_INET,SOCK_STREAM, 0)) == -1) 
{

perror("Socket: ");
exit(-1);
}
serv_addr_web.sin_family = AF_INET;
serv_addr_web.sin_port = 0;
serv_addr_web.sin_addr.s_addr=inet_addr("192.168.0.2");
bzero(&serv_addr_web.sin_zero, 8);
//addr_len = sizeof(struct sockaddr_in);	
printf("\nSocket creation successfull\n");
if((bind(sockfd,(struct sockaddr *)&serv_addr_web, sizeof(serv_addr_web))) == -1)
{ 
perror("bind");
exit(-1);
}
socklen_t proxy_len = sizeof(proxy_addr);
if (getsockname(sockfd, (struct sockaddr *)&proxy_addr, &proxy_len) == -1) {
    perror("getsockname");
    exit(-1);
}

printf("port number %d\n", ntohs(proxy_addr.sin_port));
printf("set iptables rule\n");
sprintf(iptable_command,"iptables -t nat -A POSTROUTING -p tcp -j SNAT --sport %hu --to-source 10.0.0.1",ntohs(proxy_addr.sin_port));
printf("command %s\n",iptable_command);
system(iptable_command);
bzero(iptable_command,sizeof(iptable_command));
socklen_t dest_len=sizeof(dest_addr);
getsockopt( ser_sock,SOL_IP,SO_ORIGINAL_DST,&dest_addr,&dest_len); 
printf("port number %d\n", ntohs(dest_addr.sin_port));
printf("ip address %s\n",inet_ntoa(dest_addr.sin_addr));
//error checking to see if socket binding occurred or not
dest_addr.sin_family=AF_INET;
    if( connect(sockfd, (struct sockaddr *)&dest_addr, dest_len) < 0)
    {
       	printf("\n Error : Connect Failed \n");
       	exit(-1);
} 
   
//printf("connection successfull\n");	
if (ntohs(dest_addr.sin_port) == 80)
{
n=recv(ser_sock,rec_buff,sizeof(rec_buff),0);
bytes_received=n+bytes_received;
n =send(sockfd,rec_buff,sizeof(rec_buff),0);
bytes_sent = n+ bytes_sent;
do 
{
//	printf("reached here in do while loop\n");
//bzero(rec_buff2,sizeof(rec_buff2));
n = recv(sockfd,rec_buff2,sizeof(rec_buff2),0);
bytes_received=n+ bytes_received;
//fwrite(rec_buff2,sizeof(rec_buff2),1,file);
//puts("reached after reading");
//printf("n :%d\n",n);
if(!(n<=0))
{
n=send(ser_sock,rec_buff2,n,0);
bytes_sent=n+bytes_sent;
}
//printf("%s\n",rec_buff2);
//	puts(rec_buff2);
}while(n>0);
}
if (ntohs(dest_addr.sin_port) == 22)
{
printf("reached in ssh\n");
fcntl(ser_sock,F_SETFL,O_NONBLOCK);
fcntl(sockfd,F_SETFL,O_NONBLOCK);
while(1)
{

if((n=recv(sockfd,rec_buff2,sizeof(rec_buff2),0))>0)
{
bytes_received= n+bytes_received;
printf("************%s\n",rec_buff2);
send(ser_sock,rec_buff2,n,0);
bytes_sent =n+bytes_sent;
bzero(rec_buff2,sizeof(rec_buff2));
}

if((n1=recv(ser_sock,write_buff,sizeof(write_buff),0))>0)
{
bytes_received= n1+bytes_received;
printf("############%s\n",write_buff);
send(sockfd,write_buff,n1,0);
bytes_sent =n1+bytes_sent;
bzero(write_buff,sizeof(write_buff));
}

}


}
timestamp = time(NULL);
sprintf(write_buff,"<%d> <10.0.0.1> <%d> <%s> <%d> <%d> <%d>\n", timestamp, ntohs(proxy_addr.sin_port), inet_ntoa(dest_addr.sin_addr), ntohs(dest_addr.sin_port),bytes_sent,bytes_received);
printf("write_buff %s\n",write_buff);
fwrite(write_buff,1,sizeof(write_buff),file);

close(ser_sock);

}//function


