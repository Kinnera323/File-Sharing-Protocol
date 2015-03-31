#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

void tcp_server(int server_port);
void tcp_client(int client_port);
void udp_client(int client_port);
void udp_server(int server_port);

int main(int argc,int *argv[]){
	int server_port;
	printf("Enter the port no. to which you want to bind\n");
	scanf("%d",&server_port);
	int protocol;
	printf("Please select the type: '0' for TCP and '1' for UDP\n");
	scanf("%d",&protocol);

	int pid;
	pid = fork();
	if(pid == 0){
		printf("This part is client\n");
		printf("Please enter the port to which to want to connect\n");
		int client_port;
		scanf("%d",&client_port);
		char c;
		scanf("%c",&c); /// This character eliminates \n after taking input ////
		if(protocol == 0)
			tcp_client(client_port);
		else
			udp_client(client_port);
	}
	else{
		if(protocol == 0 )
			tcp_server(server_port);
		else
			udp_server(server_port);
	}
	return 0;
}
void tcp_server(int server_port){

	//printf("went into server\n");

	struct sockaddr_in server_addr,client_addr; 
	int connectionSocket,sock;

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
	{
		perror("Socket");
		exit(1);
	}

	server_addr.sin_family = AF_INET;         
	server_addr.sin_port = htons(server_port);     
	server_addr.sin_addr.s_addr = INADDR_ANY; 
	bzero(&(server_addr.sin_zero),8); 


	if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr))== -1) 
	{
		perror("Unable to bind");
		exit(1);
	}

	// Listening to connections
	if (listen(sock, 5) == -1) //maximum connections listening to 5
	{
		perror("Listen");
		exit(1);
	}
	fflush(stdout);
	// Accepting connections
	while((connectionSocket=accept(sock , (struct sockaddr*)NULL,NULL))<0);
	// NULL will get filled in by the client's sockaddr once a connection is establised

	printf("[CONNECTED]>>>>>> server\n");

	while(1){
		int bytes_received;
		char received_data[1024],send_data[1024];
		char *token1;

		//printf("*\n");

		bytes_received=recv(connectionSocket,received_data,1024,0);
		received_data[bytes_received]='\0';
		//printf("%s\n",received_data );

		if( received_data[0]=='i' && received_data[1]=='l')
		{     
			system("ls -l > outfile_long");
			//system("find . -printf '%p %TY%Tm%Td%TH%Tm%Tm %k \n' > outfile_long");
			//file to be sent here longlist
			FILE *fp = fopen("outfile_long","r");
			if(fp == NULL)
			{
				printf("wrong file\n");
				continue;
			}
			memset(send_data,0,1024);
			int bytes_read,bytes_sent;
			while(!feof(fp))
			{ //memset(send_data,0,1024);
				bytes_read = fread(send_data,1,1024,fp);
				send_data[bytes_read] = '\0';
				bytes_sent = write(connectionSocket,send_data,1024);
				//printf("%s",send_data);
				//printf("read %d sent %d\n",bytes_sent,bytes_read);
				//===============================file sent=================================================
			}
			printf("End file\n");
			memset(send_data,0,1024);
			char end[]= "End Of File";
			strcpy(send_data,end);
			write(connectionSocket,send_data,1024);
			fclose(fp);
			continue;
		}
		else if(received_data[0]=='i' && received_data[1]=='s'){
			printf("%s\n",received_data );
			char start_time[1024],end_time[1024];

			token1 = strtok(received_data," ");
			//printf("%s\n",token1 );        // this gives 'is'
			token1 = strtok(NULL," ");
			strcpy(start_time,token1);
			printf("%s\n",start_time );
			
			token1 = strtok(NULL," ");
			strcpy(end_time,token1);
			printf("%s\n",start_time );
			

			system("ls -l --time-style=+%Y%m%d%H%M%S -t > outfile_total_short ");
			FILE *fp = fopen("outfile_total_short","r");
			if(fp == NULL)
			{
				printf("wrong file\n");
				continue;
			}
			char one_file[1024],found_time[1024],one_file_original[1024];
			int bytes_sent;
			int count=0;
			while(fgets(one_file,1024,fp)){
				if(count == 0){
					//send(connectionSocket,one_file,1024);
					count = 1;
				}
				else{
				printf("%s\n",one_file );
				int i;
				strcpy(one_file_original,one_file);
				token1 = strtok(one_file," ");
				printf("%s\n",token1 );
				for (i = 0; i < 5; i++)
				{
					if(token1 != NULL) 
						token1 = strtok(NULL," ");
					printf("%s\n",token1 );
				}
				strcpy(found_time,token1);
				strcat(found_time,"\n");
				strcat(start_time,"\n");
				if(strcmp(found_time,start_time)>0 && strcmp(found_time,end_time)<0){
					//bytes_read = fread(one_file_original,1,1024,fp);
				 	//send_data[bytes_read] = '\0';
				 	//strcpy(one_file_original,"\0");
				 	send(connectionSocket,one_file_original,strlen(one_file_original),0);
				 	printf("%s\n",one_file_original );
				}
				memset(one_file_original,0,1024);
				}
			}
			printf("End File\n");
			
			memset(found_time,0,1024);
			memset(start_time,0,1024);
			memset(end_time,0,1024);
			char end[]="End Of File";
			strcpy(send_data,end);
			write(connectionSocket,send_data,1024);
			fclose(fp);
			continue;

		}
		else if(received_data[0]=='i' && received_data[1]=='r'){

			//printf("%s\n",received_data );
			token1 = strtok(received_data," ");
			token1 = strtok(NULL," ");
			char token2[1024];
			strncpy(token2,token1,strlen(token1)-1);
			char proc_command[1024];
			strcpy(proc_command,"find . -name ");
			strcat(proc_command,token2);
			strcat(proc_command," > out_regex");
			//printf("%s\n",proc_command );
			system(proc_command);

			FILE *fp = fopen("out_regex","r");
			if(fp == NULL)
			{
				printf("wrong file\n");
				continue;
			}
			memset(send_data,0,1024);
			int bytes_read,bytes_sent;
			while(!feof(fp))
			{ 
				//memset(send_data,0,1024);
				bytes_read = fread(send_data,1,1024,fp);
				send_data[bytes_read] = '\0';
				bytes_sent = write(connectionSocket,send_data,1024);
			}
			printf("End file\n");
			memset(send_data,0,1024);
			char end[]= "End Of File";
			strcpy(send_data,end);
			write(connectionSocket,send_data,1024);
			fclose(fp);
			continue;
		}
		else if(received_data[0]=='f' && received_data[1]=='v'){

				printf("%s\n",received_data );
				token1 = strtok(received_data," ");
				token1 = strtok(NULL," ");

				char commanded[1024],command1[1024];
				token1[strlen(token1)-1]='\0';

				//printf("*******\n");
				strcpy(commanded,"md5sum ");
				//strcat(commanded," ");
				strcat(commanded,token1);
				//printf("%s\n",commanded );
				strcat(commanded," | awk '{print $1}' > out1");
				//printf("%s\n",commanded );
				//strcat(command," > out ");
				system(commanded);
				//printf("%s\n",commanded );

				strcpy(command1,"stat");
				strcat(command1," ");
				strcat(command1,token1);
				strcat(command1," | grep Modify | awk '{print $3}' >> out1");
				system(command1);
				printf("%s\n",command1 );

				FILE *f=fopen("out1","r");
				char data[300];
				fread(data,1,300,f);
				fclose(f);
				data[strlen(data)-1]='\0';
				printf("%s\n",data );
				write(connectionSocket,data,300);

				// f=fopen("out2","r");
				// char data1[300];
				// fgets(data1,sizeof(data1),f);
				// fclose(f);
				// data1[strlen(data1)-1]='\0';
				// printf("%s\n",data1 );
				// write(connectionSocket,data1,1024);


		}
		else if(received_data[0]=='f' && received_data[1]=='c'){


		}
		else if(received_data[0] == 'D'){
			printf("%s\n",received_data );
			token1 = strtok(received_data," ");
			token1 = strtok(NULL," ");
			token1[strlen(token1)-1]='\0';

			memset(send_data,0,1024);
			int bytes_read,bytes_sent;

			char tim[1024];char str[1024];
			struct stat st;
			stat(token1, &st);
			int size = st.st_size;
			sprintf(str, "%d", size);
			sprintf(tim,"%d",ctime(&st.st_mtime));
			write(connectionSocket,str,1024);
			write(connectionSocket,tim,1024);

			char md5[1024];
			strcpy(md5,"md5sum");
			strcat(md5," ");
			strcat(md5,token1);
			strcat(md5," | awk '{print $1}' > checksum " );
			system(md5);

			printf("%s\n",md5 );
			FILE *checkfp = fopen("checksum","r");
			while(!feof(checkfp)){
				int bytes = fread(send_data,1,1024,checkfp);
				send_data[bytes]='\0';
				write(connectionSocket,send_data,1024);
				printf("%s\n",send_data );
			}
			char end[]="END";
			write(connectionSocket,end,1024);
			fclose(checkfp);
			//sleep(1);
			FILE *fop = fopen(token1,"r");
			if(fop == NULL)
				printf("Error opening file\n");
			while(!feof(fop)){
				bytes_read = fread(send_data,1,1024,fop);
				send_data[bytes_read] = '\0';
				bytes_sent = write(connectionSocket,send_data,1024);
				//printf("%s\n",send_data );
			}
			//char end[]="END";
			strcpy(end,"END");
			write(connectionSocket,end,1024);
			fclose(fop);
			printf("file sent\n");
			continue;
		}
		else if(received_data[0] == 'U'){
			//printf("Please enter y or Y to accept the fileupload\n");
			token1 = strtok(received_data," ");
			token1 = strtok(NULL," ");
			
			//token1[strlen(token1)-1]='\0';
			
			char status[1024];char token2[1024];int bytes;
			//printf("**\n");
			int c;
			//scanf("%d",&c);
			//scanf("%d",&status);
			//gets(status);
			//fgets(status,1023,stdin);
			//printf("*****\n");
			//printf("%d\n",c );
			//send(connectionSocket,status,strlen(status),0);
			//			sleep(1);
						int dog=0;
						FILE *fp = fopen(token1,"w");
   						if(fp == NULL){
   							printf("Error opening the file\n");
   						}
   						while(1){
   							bytes = recv(connectionSocket,received_data,1024,0);
   							received_data[bytes] = '\0';
   							if(strcmp(received_data,"END")==0 ){ 
   								if(dog==0)
   								{
   									strcpy(token2,"rm");
   									strcat(token2," ");
   									strcat(token2,token1);
   									system(token2);
   								}
   								break;
   							}
   							fprintf(fp, "%s", received_data);
   							dog++;
   						}
   						fclose(fp);
   						printf("File Uploaded succesfully\n");
   						continue;
		}


	}
	close(connectionSocket);


}
void tcp_client(int client_port){
	int ClientSocket;
	struct sockaddr_in serv_addr;
	//struct hostent *host;
	//host = gethostbyname("127.0.0.1");

	ClientSocket = socket(AF_INET,SOCK_STREAM,0);
	if(ClientSocket<0)
	{
		printf("ERROR WHILE CREATING A SOCKET\n");
		return;
	}
	else
		printf("[CLIENT] Socket created \n");

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(client_port);
	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	bzero(&(serv_addr.sin_zero),8);

	while (connect(ClientSocket, (struct sockaddr *)&serv_addr,sizeof(struct sockaddr)) < 0);

	printf("[CONNECTED]\n");

	while(1){

		char *got_data_1,*send_command_1,*received_data_1;
		char *token1;
		int bytes_received,bytes;
		char buffer[1024],send_command[1024],got_data[1024],received_data[1024];
		//printf("\n SEND (q or Q to quit) : ");
		//gets(got_data);
		fgets(buffer,1023,stdin);
		strcpy(got_data,buffer);


		//printf("*\n");

		if (strcmp(got_data , "q") == 0 || strcmp(got_data , "Q") == 0)
		{
			send(ClientSocket, got_data,strlen(got_data), 0); 
			close(ClientSocket);
			break;
		}
		else{
			//printf("**\n");
			/* get the first token */
			token1 = strtok(got_data," ");
			if(token1 != NULL){
				if(strcmp(token1,"IndexGet") == 0){
					//printf("***\n");
					token1 = strtok(NULL," ");
					//printf("%s",token1 );
					if(strcmp(token1,"longlist\n") == 0){
						//printf(">>>>\n");
						strcpy(send_command,"il");
						//printf("<<<\n");
						//printf("%s\n",send_command );
						if( send(ClientSocket,send_command,strlen(send_command),0) < 0 ) 
							printf("error while sending\n");

						while(1)
						{
							bytes_received=recv(ClientSocket,received_data,1024,0);
							if(bytes_received < 0)
							{
								printf("ERROR while reading from the socket\n");
							}
							else
							{	   
								received_data[bytes_received] = '\0';
								if(strcmp(received_data,"End Of File") == 0)
									break;
								fwrite(received_data,1,bytes_received,stdout);
							}
						}
						continue;
					}
					else if(strcmp(token1,"shortlist") == 0){

						strcpy(send_command,"is");
						token1 = strtok(NULL," ");
						//printf("%s\n",token1 );
						if(token1 != NULL){
							strcat(send_command," ");
							strcat(send_command,token1);
							token1 = strtok(NULL," ");
							if(token1 != NULL){
								strcat(send_command," ");
								strcat(send_command,token1);
								if(send(ClientSocket,send_command,strlen(send_command),0)<0)
									printf("ERROR while writing to the socket\n");

								memset(received_data,0,1024);
								while(1)
								{
									bytes_received=recv(ClientSocket,received_data,1024,0);
									received_data[bytes_received] = '\0';
									if(strcmp(received_data,"End Of File") == 0)break;
									fwrite(received_data,1,bytes_received,stdout);

								}
								printf("\ncommand succesfully completed now\n");
								continue;
							}

						}	
						else printf("Please Type \"IndexGet --shortlist <start-time> <end-time>\"	\n");	
					}
					else if(strcmp(token1,"regex") == 0){
						memset(send_command,0,1024);
   					strcpy(send_command,"ir");
   					token1 = strtok(NULL," ");
   						if(token1 != NULL){
   							strcat(send_command," ");
   							strcat(send_command,token1);
   							printf("%s\n",send_command );
   							if(send(ClientSocket,send_command,strlen(send_command),0)<0)
   									printf("ERROR while writing to the socket\n");

							while(1)
							{
								bytes_received=recv(ClientSocket,received_data,1024,0);
								received_data[bytes_received] = '\0';
								if(strcmp(received_data,"End Of File") == 0)break;
								fwrite(received_data,1,bytes_received,stdout);
							
							}
							printf("\ncommand succesfully completed now\n");
							continue;
   						}
   						else printf("Please Type \"IndexGet --regex <regex-argument>\"	\n");

   					}

				}
				else if(strcmp(token1,"FileHash") == 0){
					//printf("*\n");
   					token1 = strtok(NULL," ");
   					if(token1 != NULL){
   					if(strcmp(token1,"verify") == 0){
   					//	printf("**\n");
   						strcpy(send_command,"fv");
   						token1 = strtok (NULL, " ");
						if(token1!=NULL){
							strcat(send_command," ");
							strcat(send_command,token1);
							printf("%s\n",send_command );
							send(ClientSocket, send_command,strlen(send_command), 0);
							printf("Command sent\n");
								//while(1)
								{
									bytes_received=recv(ClientSocket,received_data,1024,0);
									received_data[bytes_received] = '\0';
									if(strcmp(received_data,"End Of File") == 0)break;
									fwrite(received_data,1,bytes_received,stdout);

									// bytes_received=recv(ClientSocket,received_data,1024,0);
									// received_data[bytes_received] = '\0';
									// if(strcmp(received_data,"End Of File") == 0)break;
									// fwrite(received_data,1,bytes_received,stdout);
									
								}
								printf("\ncommand execution completed now\n");
								continue;
						}
						else printf("Please Type \"FileHash --verify <file-name>\"	\n");

   					}
   					else if(strcmp(token1,"checkall") == 0 ){
   						strcpy(send_command,"fc");
   						send(ClientSocket,send_command,strlen(send_command),0);
   						while(1){
   							bytes_received = recv(ClientSocket,received_data,1024,0);
   							received_data[bytes_received] = '\0';
   							if(strcmp(received_data,"End Of File") == 0)break;
   							fwrite(received_data,1,bytes_received,stdout);
   						}
   						printf("\ncommand execution done\n");
   						continue;

   					}
   					}
   					else printf("Please enter proper command \" FileHash --verify <file-name> or --checkall \" \n");

   				}
   				else if(strcmp(token1,"FileDownload") == 0){
   					token1 = strtok(NULL," ");
   					if(token1 != NULL){
   						memset(send_command,0,1024);
   						strcpy(send_command,"D");
   						strcat(send_command," ");
   						strcat(send_command,token1);

   						token1[strlen(token1)-1]='\0';

   						//printf("%s\n",send_command );
   						if( send(ClientSocket, send_command,strlen(send_command), 0) < 0)
   							printf("Error while passing command to server\n");


   						// recv(ClientSocket,received_data,1024,0);
   						// printf("%s %s ",token1,received_data );
   						// memset(received_data,0,1024);
   						// recv(ClientSocket,received_data,1024,0);
   						// printf("%s ",received_data );
   						// memset(received_data,0,1024);
   						// recv(ClientSocket,received_data,1024,0);
   						// printf("%s\n",received_data );
   						printf(" %s ",token1 );
   						while(1){
   							bytes = recv(ClientSocket,received_data,1024,0);
   							received_data[bytes] = '\0';
   							if(strcmp(received_data,"END")==0 ) break;
   							printf("%s ", received_data);
   						}
   						printf("\n");
   						FILE *fp = fopen(token1,"w");
   						if(fp == NULL){
   							printf("Error opening the file\n");
   						}
   						while(1){
   							bytes = recv(ClientSocket,received_data,1024,0);
   							received_data[bytes] = '\0';
   							if(strcmp(received_data,"END")==0 ) break;
   							fprintf(fp, "%s", received_data);
   						}
   						fclose(fp);
   						printf("File Downloaded succesfully\n");
   						continue;
   					}
   				}
	   			else if(strcmp(token1,"FileUpload") == 0){
	   				token1 = strtok(NULL," ");
	   				if(token1 != NULL){
	   					strcpy(send_command,"U");
	   					strcat(send_command," ");
	   					strcat(send_command,token1);
	   					send_command[strlen(send_command)-1]='\0';
	   					token1[strlen(token1)-1]='\0';

   						printf("%s\n",send_command );
   						if( send(ClientSocket, send_command,strlen(send_command), 0) < 0)
   							printf("Error while passing command to server\n");

   						//recv(ClientSocket,received_data,1024,0);
   						//if( received_data[0]=='y' || received_data[0]=='Y'){
   							FILE *fp = fopen(token1,"r");
							if(fp == NULL)
							{
							printf("wrong file\n");
							continue;
							}
							char send_data[1024];
							memset(send_data,0,1024);
							int bytes_read,bytes_sent;
							while(!feof(fp))
							{ 	
								memset(send_data,0,1024);
								bytes_read = fread(send_data,1,1024,fp);
								send_data[bytes_read] = '\0';
								bytes_sent = write(ClientSocket,send_data,1024);
				
							}
							char end[]="END";
							write(ClientSocket,end,1024);
							fclose(fp);

   						//}
   						//else
   						//	continue;
						
						char tim[1024];char str[1024];
						struct stat st;
						stat(token1, &st);
						int size = st.st_size;
						sprintf(str, "%d", size);
						sprintf(tim,"%d",ctime(&st.st_mtime));
						//write(connectionSocket,str,1024);
						//write(connectionSocket,tim,1024);
						printf("------ %s %s %s",token1,str,tim );
						char md5[1024];
						strcpy(md5,"md5sum");
						strcat(md5," ");
						strcat(md5,token1);
						strcat(md5," | awk '{print $1}' > checksum " );
						system(md5);

						//printf("%s\n",md5 );
						FILE *checkfp = fopen("checksum","r");
						while(!feof(checkfp)){
							int bytes = fread(send_data,1,1024,checkfp);
							send_data[bytes]='\0';
							//write(connectionSocket,send_data,1024);
							printf("%s-----\n",send_data );
						}
						printf("File Uploaded\n");	
						fclose(checkfp);

	   				}
   				}
			}
		}

	}
}
void udp_server(int server_port){

}
void udp_client(int client_port){

}
