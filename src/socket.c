#include <fox.h>

int ip_connect(char* host,int port,char** err){
	int ret=socket(AF_INET,SOCK_STREAM, 0);
	if(!ret){ return err_msg("Socket creation failed",err); };
	struct hostent *server=gethostbyname(host);
	if(!server){ return err_msg("fox_error: no such host",err); };
	struct sockaddr_in serv_addr={0};
	serv_addr.sin_family = AF_INET;
	memcpy(&serv_addr.sin_addr.s_addr, server->h_addr,server->h_length);
	serv_addr.sin_port = htons(port);
	if(connect(ret,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){ return err_msg("fox_error connecting",err); };
	return ret;
};
int ip_close(int sock){ close(sock);return 0; };
int ip_send(char* msg, int sock){ return write(sock,msg,mem_size(msg)); };
char* ip_receive(int sock,char* terminator,int size){
	char* ret=NULL;
	return ret;
};
char* remote_ip(int con){
	struct sockaddr_in addr={0};
	socklen_t len=sizeof(struct sockaddr_in);
	getpeername(con,(struct sockaddr*)&addr,&len);
	return inet_ntoa(addr.sin_addr);
};
