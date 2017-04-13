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
#include<malloc.h>
#include<sys/signal.h>
#include<pthread.h>
#include<time.h>
int get_ip(char *  , char *);
char *str2md5(const char *str, int length);
int main()
{
	char input[100];
	int counter;
	int portnumber=0,cmp, sock,addr_len,client_sock;
	struct sockaddr_in server,client;//creating struct sockaddr_in variables server and client
	void *thread_handler();
	printf("Enter Portnumber to run Webproxy server\n");
	scanf( " %[^\n]s", input);
	printf("%s\n",input);
	// printf("enter counter for cache");
	// scanf("%d",counter);

	cmp = sscanf(input, "webproxy %d", &portnumber);
	if(cmp ==1)
	{
		printf("portnumber : %d\n", portnumber);
	}
	else
	{
		printf("sscanf failed\n");
	}
	
	//socket creation
	if((sock = socket(AF_INET,SOCK_STREAM, 0)) == -1) 
		{

			perror("Socket: ");
			exit(-1);
		}
	server.sin_family = AF_INET;
	server.sin_port = htons(portnumber);
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
			//creating thread using pthread
			// index_pg = strtok(index_page,"\n");			
			// doc_path = strtok(document_path,"\n");
			pthread_t thread_id;
		printf("socket no main:%d\n",client_sock);
	//creating thread
			if ((pthread_create(&thread_id,NULL,thread_handler,(void *)&client_sock))<0)
			{
				perror("Thread:");
				exit(-1);
			}

			
			 }//while
			 return 1;
   
}

//function for thread handling
void *thread_handler(void *new_sock)
{	
	int file_read,addr_len,n=1;
	int ser_sock = *(int *)new_sock, sockfd;
	int size1;
	struct sockaddr_in serv_addr_web;
	FILE *file;
	// unsigned char check[MD5_DIGEST_LENGTH];

	// MD5_CTX mdCon ;
	char ip[100];
	char host_name[200];
	int size,line_counter =1;
	char rec_buff[13000],rec_buff2[130],request[2000],filename[500],write_buff[200],*temp_name;
	// char index_page[20]={0};
//	char *error = "/?";
	char delim[4] =" ";
	bzero(filename,500);
	file_read =1;

	char *status = (char *)malloc(500*sizeof(char));
	char *req_ver = (char *)malloc(30*sizeof(char));
	char *rec_buff1 = (char *)malloc(30*sizeof(char));
	char *path = (char *)malloc(30*sizeof(char));
	char *req_type = (char *)malloc(50*sizeof(char));
	char *req_meth = (char *)malloc(30*sizeof(char));
	
	
	//signal(SIGALRM,time_handle);
	
	//while(break_condition==0){
//receiving from client		
		size =read(ser_sock,rec_buff,sizeof(rec_buff));
		
		printf("size of request buff:%d\n",size);
		printf("request:%s",rec_buff);
		printf("socket no:%d\n",ser_sock);
		line_counter=1;
			rec_buff1 = strtok(rec_buff,"\n");
//taking GET /image.png HTTP/1.1 into three different strings req_meth,req_type,req_ver
			while(line_counter==1){
				req_meth = strtok(rec_buff1,delim);
				req_type = strtok(NULL,delim);
				printf("\n request type: %s",req_type);
				req_ver = strtok(NULL,delim);
				
			line_counter++;
			}
			printf("\n request method: %s",req_meth);
//error handling (facing issue in strcmp couldnot compare strings so the part is commented)
			// if(strstr(req_type,error)==0)
			// {
			// 	status = "HTTP/1.0 400 Bad Request:Invalid URI:";
			// 	//strcat(status,req_type);
			// 	printf("in if statement");
			// 	send(ser_sock,status,strlen(status),0);
			// }
			 if(strstr(req_meth,"GET")==NULL)
			{
				status = "HTTP/1.0 400 Bad Request:Invalid Method:\r\n\r\n";
				send(ser_sock,status,strlen(status),0);
			}
			else if((strstr(req_ver,"HTTP/1.0")==NULL) && (strstr(req_ver,"HTTP/1.1")==NULL))
			{
				status = "HTTP/1.0 400 Bad Request:Invalid Version:\r\n\r\n";
				send(ser_sock,status,strlen(status),0);
			}
			else{
			temp_name=str2md5(req_type,strlen(req_type));
			printf("hash: %s",temp_name);
			sprintf(filename,"cache/%s",temp_name);
			FILE *temp_file = fopen(filename,"rb");
			if(temp_file == NULL)
			{
			FILE *file = fopen(filename,"wb");	
			printf("filename : %s\n",filename);
			//connect to the server requested.
			sscanf(req_type,"http://%[^/]s/",host_name);
			sscanf(req_type,"http://%*[^/]%s",path);
			printf("\n name: %s",host_name);
			printf("\n path: %s",path);
			get_ip(host_name,ip);
			printf("\n ip address is: %s",ip);
				if((sockfd = socket(AF_INET,SOCK_STREAM, 0)) == -1) 
				{

					perror("Socket: ");
						exit(-1);
				}
				serv_addr_web.sin_family = AF_INET;
				serv_addr_web.sin_port = htons(80);
				serv_addr_web.sin_addr.s_addr = inet_addr(ip);
				bzero(&serv_addr_web.sin_zero, 8);
				addr_len = sizeof(struct sockaddr_in);	
				printf("\nSocket creation successfull\n");
		//error checking to see if socket binding occurred or not
    			if( connect(sockfd, (struct sockaddr *)&serv_addr_web, addr_len) < 0)
    			{
       				printf("\n Error : Connect Failed \n");
       				exit(-1);
				 } 
    
						printf("connection successfull\n");	
				if (getsockname(sockfd, (struct sockaddr *)&serv_addr_web, &addr_len) == -1) {
    				perror("getsockname");
    				exit(-1);
					}

				printf("port number %d\n", ntohs(serv_addr_web.sin_port));
				sprintf(request,"GET / %s\r\n\r\n",req_ver);
				size1=send(sockfd,request,sizeof(request),0);
				printf("bytes send %d\n",size1);
				 do 
	 			{
	 				printf("reached here in do while loop\n");
	 					//bzero(rec_buff2,sizeof(rec_buff2));
	 					n = recv(sockfd,rec_buff2,sizeof(rec_buff2),0);
	 					fwrite(rec_buff2,sizeof(rec_buff2),1,file);
	 						puts("reached after reading");
	 						printf("n :%d\n",n);
	 						if(!(n<=0))
	 							{
	 								send(ser_sock,rec_buff2,n,0);
	 								}
		
	 	//printf("%s\n",rec_buff2);
									puts(rec_buff2);
								}while(n>0);
	fclose(file);				
			}
		else
		{
			while(file_read>0)
			{
				file_read = fread(write_buff,1,sizeof(write_buff),temp_file);
				write(ser_sock,write_buff,sizeof(write_buff));
			
			}
		fclose(temp_file);	
		}
	//	fclose(temp_file);

	
	
	
			
			
}//else



	close(ser_sock);
	//return 0;
}//function

int get_ip(char * host_name , char* ip)
{
    struct hostent *host;
    struct in_addr **list;
    int i;
         
    if ( (host = gethostbyname( host_name ) ) == NULL) 
    {
        // get the host info
        herror("gethostbyname");
        return 1;
    }
 
    list = (struct in_addr **) host->h_addr_list;
     
    for(i = 0; list[i] != NULL; i++) 
    {
        //Return the first one;
        strcpy(ip , inet_ntoa(*list[i]) );
        return 0;
    }
     
    return 1;
}
char *str2md5(const char *str, int length) {
    int n;
    MD5_CTX c;
    unsigned char digest[16];
    char *out = (char*)malloc(33);

    MD5_Init(&c);

    while (length > 0) {
        if (length > 512) {
            MD5_Update(&c, str, 512);
        } else {
            MD5_Update(&c, str, length);
        }
        length -= 512;
        str += 512;
    }

    MD5_Final(digest, &c);

    for (n = 0; n < 16; ++n) {
        snprintf(&(out[n*2]), 16*2, "%02x", (unsigned int)digest[n]);
    }

    return out;
}