#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>


//standard HTTP port
#define SERVER_PORT 15000

void strcut(char *ans,char*line){
        char* p1;
        char* p2;
        char *qtr=ans;
        char deli1[] ={"\r\n\r\n"};
        char deli2[] ={"\r\n"};
        int i = 0;
        p1 = strstr(line,deli1);
        p1 += 4;
        p2 = strstr(p1,deli2);
        --p2;

        while(p1 != p2){
                *qtr++=*p1++;
        }
        *qtr='\0';
}


int getlen(char* line){
        char* p1 = NULL;
        char* p2 = NULL;
        char strnum[64];
        int num;
        p2 = strnum;
        p1 = strstr(line,"Content-Length:");
        //printf("%c\n",*p1);
        while(!isspace(*p1)){
                p1++;
        }
        p1++;//跳空格
        //printf("%c\n",*p1);
        while(isdigit(*p1)){
                *p2 = *p1;
                p2++;
                p1++;
        }
        *p2 = '\0';

        num = atoi(strnum);
        return num;
};


int main(int argc,char **argv){

	if(argc != 2){
		printf("input error!\n");
	}

	int connfd;
	int web_fd;
	pid_t	childpid;
	

	/*make socket*/
	int sockfd;
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	//1.ipv4 2.tcp 3.normal
	//printf("sockfd = %d\n",sockfd);
	
	/*make server_addr*/
	struct sockaddr_in server_addr;
        struct sockaddr_in client_addr;
	bzero(&server_addr,sizeof(server_addr));	
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	
	//convert string to number
	inet_pton(AF_INET,argv[1],&server_addr.sin_addr.s_addr);
	
	/*bind*/
	bind(sockfd,(struct sockaddr*)&server_addr,sizeof(server_addr));
	printf("bind success!\n");

	/*listen*/
	listen(sockfd,20);
	printf("listen success!\n");
	
	signal(SIGCHLD, SIG_IGN);

	/*client
	int clifd;
	socklen_t cli_len = sizeof(client_addr);
	clifd = accept(sockfd,(struct sockaddr*)&client_addr,&cli_len);
	printf("accept success!\n");

	char buf[8192];
	int n,i;
	

	while(1){
		n = read(clifd,buf,sizeof(buf));
		for(i = 0;i <= n;i++){
			buf[i] = toupper(buf[i]);
		}
		write(clifd,buf,n);
		printf("%s\n",buf);
	
	}
	*/
	
	/*open html*/
	char web_buff[4096];
	web_fd = open("web.html",O_RDONLY);
	read(web_fd,web_buff,sizeof(web_buff));

	int n,fd,i,len;
	char buff[40000];
	char line[40000];
	char raw_data[40000];
	char binary_data[40000];

	char* string,*p1,*p2;
	int start;
	char deli[] = "\r\n\r\n";
	for( ; ; ){
		//struct sockaddr_in addr;
		//socklen_t addr_len;

		printf("waitting for a connection on port %d\n",SERVER_PORT);
		fflush(stdout);

		connfd = accept(sockfd,(struct sockaddr*) NULL,NULL);
		memset(line,0,40000);	

		//n = read(connfd, line,4096-1);
		len = recv(connfd,line,40000,0);
		printf("recv = %d\n",len);
		fprintf(stdout, "this is line:\n%s\n",line);
		printf("***************************\n");
		
				
		if ( (childpid = fork()) == 0) {	/* child process */
			close(sockfd);	/* close listening socket */
			
			if(line[0] == 'P'){

				len = getlen(line);
				printf("len = %d\n",len);
				p1 = strstr(line,deli);
				p1 += 4;

				strncpy(raw_data,p1,len);
				printf("!!!rawdata =\n%s\n",raw_data);

				memset(binary_data,0,40000);	
				strcut(binary_data,raw_data);
				printf("@@@binarydata =\n%s\n",binary_data);
				
				//fd= open("/home/fred2949/Desktop/network_hw1/my.jpg",O_RDWR|O_CREAT,S_IRWXU);
				fd= open("/home/fred2949/Desktop/network_hw1/my.txt",O_RDWR|O_CREAT,S_IRWXU);
				

				write(fd,binary_data,strlen(binary_data));
				printf("\nfinish");
			}
			close(fd);
			exit(0);
		}
		snprintf((char*)buff,sizeof(buff),"HTTP/1.0 200 OK\r\n\r\n"); 
		

		write(connfd, (char*)buff, strlen((char*)buff));
		write(connfd, (char*)web_buff, strlen((char*)web_buff));
		close(connfd);

	}
	return 0;
}

