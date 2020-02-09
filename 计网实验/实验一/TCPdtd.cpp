#define _CRT_SECURE_NO_WARNINGS//在vs2017上运行时要加，否则会认为strcpy函数不安全而报错 
#include<stdlib.h>
#include <winsock2.h>
#include<process.h>
#include<time.h>
#include<iostream>
#include<string.h>
#include<sstream>
using namespace std;
//void errexit(const char*, ...);
//SOCKET passiveTCP(const char*, int);
//SOCKET passivesock(const char *, const char *, int);

#define QLEN 5
#define WSVERS MAKEWORD(2,0)
void errexit(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
	WSACleanup();
	exit(1);
}
SOCKET passivesock(const char*service, const char*ipnum, const char*transport, int qlen)
{
	struct servent*pse;
	struct protoent*ppe;
	struct sockaddr_in sin;
	SOCKET s;
	int type;

	memset(&sin, 0, sizeof(sin));//将s中当前位置后面的n个字节 （typedef unsigned int size_t ）用 0 替换并返回 s 
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr(ipnum);

	/* Map service name to port number */
	if (pse = getservbyname(service, transport)) {
		sin.sin_port = (u_short)pse->s_port;
	}
	else if ((sin.sin_port = htons((u_short)atoi(service))) == 0) {
		errexit("can't get\"%s\"service entry\n", service);
	}
	/* Map protocol name to protocol number */
	if ((ppe = getprotobyname(transport)) == 0)
		errexit("can't get\"%s\"protocol entry\n", transport);
	if (strcmp(transport, "udp") == 0)
		type = SOCK_DGRAM;
	else
		type = SOCK_STREAM;
	s = socket(PF_INET, type, ppe->p_proto);
	if (s == INVALID_SOCKET)
		errexit("can't create socket:%d\n", GetLastError());
	/* Bind the socket */
	if (bind(s, (struct sockaddr*)&sin, sizeof(sin)) == SOCKET_ERROR)
		errexit("can't bind to %s port: %d\n", service, GetLastError());
	if (type == SOCK_STREAM && listen(s, qlen) == SOCKET_ERROR)
		errexit("can't listen on %s port: %d\n", service, GetLastError());
	//const auto &s = sin.sin_addr.S_un.S_un_b;
	//printf("客户端IP地址 %d.%d.%d.%d\n", sin.sin_addr.S_un.S_un_b.s_b1, sin.sin_addr.S_un.S_un_b.s_b2, sin.sin_addr.S_un.S_un_b.s_b3, sin.sin_addr.S_un.S_un_b.s_b4);
	//printf("客户端端口号 %d\n", htons(sin.sin_port));
	return s;
}
SOCKET passiveTCP(const char *service,const char *ipnum, int qlen)
{ 
	return passivesock(service, ipnum,"tcp", qlen);
}
void TCPdaytimed(SOCKET s) {
	string request = "";
	char buf[1024];

	
	int n = recv(s, buf, 1023, 0);
	for (int i = 0; i < 1024; i++) {
		request.push_back(buf[i]);
	}
	if (n != SOCKET_ERROR && n >= 0)
	{
		std::cout << "\n接收到的报文:\n";
		buf[n] = '\0';
		puts(buf);
		//std::cout << request << endl;
	}
	if (n == SOCKET_ERROR)
	{
		errexit("can't recieve %d\n", GetLastError());
	}
	stringstream str(request);
	string method, url, protocal;
	str >> method >> url >> protocal;
	//std::cout << url << endl;
	if (!strcmp(url.c_str() ,"jpg") ){
		char sendbuff[1000000] = ("HTTP/1.1 200 OK\nConnection: close\nContent-Length: 690488\nContent-Type: image/jpg\n\n");
		FILE *in;
		if ((in = fopen("panda.jpg", "rb")) == NULL)
		{
			printf("404!\n\n");  
			FILE*in1 = fopen("message.txt", "rb");
			char sendbuff[1000] = ("HTTP/1.1 200 OK\nConnection: close\nContent-Length: 14\nContent-Type: text/html\n\n");
			puts(sendbuff);
			

			int length = strlen(sendbuff);
			fread(&sendbuff[length], sizeof(char), 1000, in1);
			if (send(s, sendbuff, 1000, 0) == SOCKET_ERROR)
				errexit("can't recieve %d\n", GetLastError());

			closesocket(s);
			return;
		}
		//puts(sendbuff);
		
		fseek(in, 0L, SEEK_END);
		int len = ftell(in);
		std::cout<<"HTTP/1.1 200 OK\nConnection: close\nContent-Length: ";
		char how_long[100];
		itoa(len, how_long, 10);
		puts(how_long);
		std::cout << ("\nContent-Type: image/jpg\n\n");
		//std::cout << len << endl;
		std::cout << "成功发送文件D: \\vs2017 code\\Service\\Service\\panda.jpg\n\n";
		fclose(in);
		in = fopen("panda.jpg", "rb");
		int length = strlen(sendbuff);
		fread(&sendbuff[length], sizeof(char), len, in);
		send(s, sendbuff, 1000000, 0);
		
		closesocket(s);
		return;
	}
	else if (!strcmp(url.c_str(), "mp3")) {
		char sendbuff[100000] = ("HTTP/1.1 200 OK\nConnection: close\nContent-Length: 1000000\nContent-Type: image/gif\n\n");
		FILE *in;
		if ((in = fopen("1.gif", "rb")) == NULL)
		{
			printf("404!\n");  exit(-1);
		}
		puts(sendbuff);
		fseek(in, 0L, SEEK_END);
		int len = ftell(in);
		std::cout << len << endl;
		fclose(in);
		in = fopen("1.gif", "rb");
		int length = strlen(sendbuff);
		fread(&sendbuff[length], sizeof(char), len, in);
		send(s, sendbuff, 100000, 0);
		
		closesocket(s);
		return;
	}




	
	char sendbuff[1000] = ("HTTP/1.1 200 OK\nConnection: close\nContent-Length: 216\nContent-Type: text/html\n\n");
	FILE *in;
	if ((in = fopen("index.html", "rb")) == NULL)
	{
		printf("404!\n\n");  
		FILE*in1 = fopen("message.txt", "rb");
		char sendbuff[1000] = ("HTTP/1.1 200 OK\nConnection: close\nContent-Length: 14\nContent-Type: text/html\n\n");
		puts(sendbuff);
		int length = strlen(sendbuff);
		fread(&sendbuff[length], sizeof(char), 1000, in1);
		if (send(s, sendbuff, 1000, 0) == SOCKET_ERROR)
			errexit("can't recieve %d\n", GetLastError());
		
		closesocket(s);
		return;
	}
	fseek(in, 0L, SEEK_END);
	int len = ftell(in);
	std::cout << "HTTP/1.1 200 OK\nConnection: close\nContent-Length: ";
	char how_long[100];
	itoa(len, how_long, 10);
	puts(how_long);
	std::cout << ("\nContent-Type: text/html\n\n");
	fclose(in);
	in = fopen("index.html", "rb");
	//std::cout << len << endl;
	//puts(sendbuff);
	std::cout << "成功发送D: \\vs2017 code\\Service\\Service\\index.html\n\n";
	int length = strlen(sendbuff);
	fread(&sendbuff[length],sizeof(char),1000,in);
	send(s, sendbuff, 1000, 0);
	
	closesocket(s);
}
void esc(void *)
{
	while (1)
	{
		if (GetKeyState(0x1B) & 0x8000)
		{
			cout << "成功关闭服务器\n" << endl;
			system("pause");
			exit(0);
		}
	}
}
int main(int argc, char*argv[]) {
	struct sockaddr_in fsin; /* the from address of a client */  
	SOCKET msock, ssock; /* master & slave sockets */ 
	int alen;   /* from-address length  */ 
	WSADATA wsadata;
	char ch;
	string ipnum;
	string service;
	if (_beginthread(esc, 0, 0) == -1L)
		throw runtime_error("create thread error");
	while (1) {
		std::cout << "输入'esc'结束程序，输入's'启动服务器：\n";
		std::cin >> ch;
		if (ch == 's') break;
		else if (ch == 'e') {
			std::cout << "程序已结束！\n";
			system("pause");
			return 0;
		}
		else std::cout << "无效输入！\n";
	}
	//char c;
	std::cout << "是否配置监听地址？1：yes 0：no\n";
	while (1) {
		std::cin >> ch;
		if (ch == '1') {
			std::cout << "请输入待配置的IP地址：  ";
			std::cin >> ipnum;
			break;
		}
		else if (ch == '0') {
			string request = "";
			char buf[15] = "127.0.0.1";


			for (int i = 0; i < 15; i++) {
				request.push_back(buf[i]);
			}
			ipnum = request;
			break;
		}
		else std::cout << "无效输入！\n";
	}
	//std::cout << "请输入待配置的IP地址：  ";
	//std::cin >> ipnum;
	
	//std::cout << ipnum;
	std::cout << "请输入待配置的端口号： ";
	std::cin >> service;

	if (WSAStartup(WSVERS, &wsadata) != 0)  
		errexit("WSAStartup failed\n");
	msock = passiveTCP(service.c_str(),ipnum.c_str(), QLEN);//bind & listen
	while (1) { 
		alen = sizeof(struct sockaddr);  
		ssock = accept(msock, (struct sockaddr *)&fsin, &alen); 
		printf("客户端地址：%s\n", inet_ntoa(fsin.sin_addr));
		std::cout << "客户端端口号：" << fsin.sin_port << endl;
		if (ssock == INVALID_SOCKET)   
			errexit("accept failed: error number %d\n", GetLastError());  
		if (_beginthread((void(*)(void *)) TCPdaytimed, 0, (void *)ssock) < 0) {
			errexit("_beginthread: %s\n", strerror(errno));
		} 
	}  
	return 1; /* not reached */
}