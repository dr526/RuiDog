#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/statfs.h>
#include <signal.h>
#include <wait.h>
#include <errno.h>

#define upload_port 8000
#define download_port 8001


struct sockInfo {
    int fd; // 通信的文件描述符
    struct sockaddr_in addr;
    pthread_t tid;  // 线程号
};
struct sockInfo upload_sockinfos[128];
struct sockInfo download_sockinfos[128];

void recyleChild(int arg) {
    while(1) {
        int ret = waitpid(-1, NULL, WNOHANG);
        if(ret == -1) {
            // 所有的子进程都回收了
            break;
        }else if(ret == 0) {
            // 还有子进程活着
            break;
        } else if(ret > 0){
            // 被回收了
            printf("子进程 %d 被回收了\n", ret);
        }
    }
}

void * upload(void * arg) {//获取客户端上传的文件
    // 子线程和客户端通信   cfd 客户端的信息 线程号
    // 获取客户端的信息
    struct sockInfo * pinfo = (struct sockInfo *)arg;

    char cliIp[16];
    inet_ntop(AF_INET, &pinfo->addr.sin_addr.s_addr, cliIp, sizeof(cliIp));
    unsigned short cliPort = ntohs(pinfo->addr.sin_port);
    printf("client ip is : %s, prot is %d\n", cliIp, cliPort);

    // 接收客户端发来的数据
	for (;;){
		char up_down_choose[BUFSIZ];
		char filename[BUFSIZ];
		char new_filename[BUFSIZ + 5];
		bzero(up_down_choose, sizeof(up_down_choose));
		bzero(filename, sizeof(filename));
		printf("接收客户端%d连接...\n",pinfo->fd);
		int rc=recv(pinfo->fd, up_down_choose, sizeof(up_down_choose),0);
		if(rc<=0){
			//close(pinfo->fd);
			printf("当前无数据!\n");
			continue;
		}
		printf("服务端接收到命令：%s\n",up_down_choose);
		// 获得文件名
		int i = 3, j = 0;
		while (up_down_choose[i] != '\0'){
			filename[j] = up_down_choose[i];
			i++;
			j++;
		}
		filename[strlen(filename)] = '\0';
		if (up_down_choose[1] == 'u'){
			char buf[1024]={0};
			FILE *fp=fopen(filename,"w");
			if(!fp){
				printf("打开文件失败\n");
			}
			else{
				memset(buf,0,sizeof(buf));
				strcpy(buf,"OK");
				if(send(pinfo->fd,buf,strlen(buf),0)<=0){
					printf("send() failed!\n");
				}
				else{
					printf("cfd 为 %d 的客户端想要上传文件\n", pinfo->fd);
					while((rc=recv(pinfo->fd,buf,sizeof(buf),0))>0){
						fwrite(buf,1,rc,fp);
						printf("*");
					}
					printf("\n文件上传完毕\n");
				}
			}
			fclose(fp);
		}
	}
	close(pinfo->fd);
	return NULL;
}

void * download(void * arg) {//客户端下载文件
    // 子线程和客户端通信   cfd 客户端的信息 线程号
    // 获取客户端的信息
    struct sockInfo * pinfo = (struct sockInfo *)arg;

    char cliIp[16];
    inet_ntop(AF_INET, &pinfo->addr.sin_addr.s_addr, cliIp, sizeof(cliIp));
    unsigned short cliPort = ntohs(pinfo->addr.sin_port);
    printf("client ip is : %s, prot is %d\n", cliIp, cliPort);

    // 向客户端发送数据
	for (;;){
		char up_down_choose[BUFSIZ];
		char filename[BUFSIZ];
		char new_filename[BUFSIZ + 5];
		bzero(up_down_choose, sizeof(up_down_choose));
		bzero(filename, sizeof(filename));
		printf("接收客户端%d连接...\n",pinfo->fd);
		int rc=recv(pinfo->fd, up_down_choose, sizeof(up_down_choose),0);
		if(rc<=0){
			//close(pinfo->fd);
			printf("当前无数据!\n");
			continue;
		}
		printf("服务端接收到命令：%s\n",up_down_choose);
		// 获得文件名
		int i = 3, j = 0;
		while (up_down_choose[i] != '\0'){
			filename[j] = up_down_choose[i];
			i++;
			j++;
		}
		filename[strlen(filename)] = '\0';
		if (up_down_choose[1] == 'd'){
			FILE* fp=fopen(filename,"rb");
			if(fp==NULL){
				printf("打开文件失败\n");
			}
			else{
				fseek(fp,0,SEEK_END);
				int fileSize=ftell(fp);
				fseek(fp,0,SEEK_SET);
				printf("文件大小为%d\n",fileSize);
				unsigned long fuck=0;
				struct stat statbuf;
				if(stat(filename,&statbuf)<0){
					fuck=0;
				}
				else{
					fuck=statbuf.st_size;
					printf("fuck=%ld\n",fuck);
					char number[65];
					sprintf(number,"%d",fileSize);
					if(send(pinfo->fd,number,strlen(number),0)<=0){
						printf("send fileSize failed\n");
					}
					char buffer[1024]={0};
					memset(buffer,0,sizeof(buffer));
					if(recv(pinfo->fd,buffer,2,0)<=0){
						printf("接收命令失败\n");
					}
					if(strncmp(buffer,"OK",2)==0){
						printf("接收到ok,准备发送数据\n");
					}
					int n=0;
					memset(buffer,0,sizeof(buffer));
					while(!feof(fp)){
						memset(buffer,0,sizeof(buffer));
						n=fread(buffer,1,sizeof(buffer),fp);
						if(n<0)break;
						if(n==0)continue;
						if(write(pinfo->fd,buffer,n)<0){
							printf("发送文件失败\n");
						}
						printf("=");
					}
					printf("文件发送完毕\n");
				}
				fclose(fp);
			}
		}
	}
	close(pinfo->fd);
	return NULL;
}

int main(){
	struct sigaction act;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);
	act.sa_handler = recyleChild;
	// 注册信号捕捉
	sigaction(SIGCHLD, &act, NULL);
	
	
	pid_t pid = fork();
	if(pid == 0) {// 子进程，用于接收客户端的文件
		int lfd = socket(PF_INET, SOCK_STREAM, 0);
		if(lfd == -1){
			perror("socket");
			exit(-1);
		}
		struct sockaddr_in saddr;
		saddr.sin_family = AF_INET;
		saddr.sin_port = htons(upload_port);
		saddr.sin_addr.s_addr = INADDR_ANY;

		// 绑定
		int ret = bind(lfd,(struct sockaddr *)&saddr, sizeof(saddr));
		if(ret == -1) {
			perror("bind");
			exit(-1);
		}

		// 监听
		ret = listen(lfd, 128);
		if(ret == -1) {
			perror("listen");
			exit(-1);
		}
		// 初始化数据
		int max = sizeof(upload_sockinfos) / sizeof(upload_sockinfos[0]);
		for(int i = 0; i < max; i++) {
			bzero(&upload_sockinfos[i], sizeof(upload_sockinfos[i]));
			upload_sockinfos[i].fd = -1;
			upload_sockinfos[i].tid = -1;
		}
		
		// 循环等待客户端连接，一旦一个客户端连接进来，就创建一个子线程进行通信
		while(1) {
			struct sockaddr_in cliaddr;
			int len = sizeof(cliaddr);
			// 接受连接
			int cfd = accept(lfd, (struct sockaddr*)&cliaddr, &len);
			struct sockInfo * pinfo;
			for(int i = 0; i < max; i++) {
				// 从这个数组中找到一个可以用的sockInfo元素
				if(upload_sockinfos[i].fd == -1) {
					pinfo = &upload_sockinfos[i];
					break;
				}
				if(i == max - 1) {
					sleep(1);
					i--;
				}
			}
			pinfo->fd = cfd;
			memcpy(&pinfo->addr, &cliaddr, len);

			// 创建子线程
			pthread_create(&pinfo->tid, NULL, upload, pinfo);
			pthread_detach(pinfo->tid);
		}
		close(lfd);
	}
	if(pid>0){
		int lfd = socket(PF_INET, SOCK_STREAM, 0);
		if(lfd == -1){
			perror("socket");
			exit(-1);
		}
		struct sockaddr_in saddr;
		saddr.sin_family = AF_INET;
		saddr.sin_port = htons(download_port);
		saddr.sin_addr.s_addr = INADDR_ANY;

		// 绑定
		int ret = bind(lfd,(struct sockaddr *)&saddr, sizeof(saddr));
		if(ret == -1) {
			perror("bind");
			exit(-1);
		}

		// 监听
		ret = listen(lfd, 128);
		if(ret == -1) {
			perror("listen");
			exit(-1);
		}
		// 初始化数据
		int max = sizeof(download_sockinfos) / sizeof(download_sockinfos[0]);
		for(int i = 0; i < max; i++) {
			bzero(&download_sockinfos[i], sizeof(download_sockinfos[i]));
			download_sockinfos[i].fd = -1;
			download_sockinfos[i].tid = -1;
		}
		
		// 循环等待客户端连接，一旦一个客户端连接进来，就创建一个子线程进行通信
		while(1) {
			struct sockaddr_in cliaddr;
			int len = sizeof(cliaddr);
			// 接受连接
			int cfd = accept(lfd, (struct sockaddr*)&cliaddr, &len);
			struct sockInfo * pinfo;
			for(int i = 0; i < max; i++) {
				// 从这个数组中找到一个可以用的sockInfo元素
				if(download_sockinfos[i].fd == -1) {
					pinfo = &download_sockinfos[i];
					break;
				}
				if(i == max - 1) {
					sleep(1);
					i--;
				}
			}
			pinfo->fd = cfd;
			memcpy(&pinfo->addr, &cliaddr, len);

			// 创建子线程
			pthread_create(&pinfo->tid, NULL, download, pinfo);
			pthread_detach(pinfo->tid);
		}
		close(lfd);
	}
	return 0;
}
