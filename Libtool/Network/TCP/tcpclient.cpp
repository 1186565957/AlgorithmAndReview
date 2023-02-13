
#include "common.h"

namespace tcpclient{
#define SEND_SIZE 3072

#define TF_CTL_NONE			0
#define TF_CTL_RUNNING		1
#define TF_CTL_PASUE		2

struct tcp_ctl_config
{
	uint32_t ip;
	uint16_t port;
	uint32_t timeout;
};

struct tcp_ctl
{
	struct tcp_ctl_config config;

	int sockfd;

	int last_time;

	int status;
};

int tcpclient_init(void* tcp_hdr)
{
	struct tcp_ctl* hdr = (struct tcp_ctl*)tcp_hdr;

	uint32_t ip = hdr->config.ip;
	uint16_t port = hdr->config.port;
	int sock = socket(AF_INET, SOCK_STREAM, 0);			
	if ( sock< 0 )
	{
		printf("HeartDetected create client socket error");
		return -1;
	}

	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(struct sockaddr_in));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = ip;
	server_addr.sin_port = htons(port);

	//set the socket options
	int timeout = hdr->config.timeout;
	setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(int));
	
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(int));

	bool reuseaddr = true;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(bool));

	int keepalive = 1;
	setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &keepalive, sizeof(int));
	
	int ret;
	ret = connect(sock, (struct sockaddr*)&server_addr, sizeof(struct sockaddr));
	if(ret == 0)
	{
		hdr->sockfd = sock;
		return 0;
	}

	return ret;
}

int tcpclient_send_log(void* tcp_hdr, uint8_t* data, uint32_t len)
{
	struct tcp_ctl* hdr = (struct tcp_ctl*)tcp_hdr;

	//Doing compress is after splited the message-log

	int ret = 0;
	int have_remain_len = len;
	uint8_t* buf = data;

	if(hdr->status == TF_CTL_PASUE)
	{
		if(tcpclient_init(tcp_hdr))
			return -1;
	}

	while(have_remain_len > 0)
	{
		ret = send(hdr->sockfd, buf, have_remain_len < SEND_SIZE? have_remain_len: SEND_SIZE, 0);
		if(ret > 0)
		{
			have_remain_len -= ret;
			buf += ret;
		}
		else
		{
			return -1;
		}
	}
	return 0;
}

int tcpclient_timeout(void* tcp_hdr)
{
	struct tcp_ctl* hdr = (struct tcp_ctl*)tcp_hdr;
	
	if(hdr->sockfd > 0)
		close(hdr->sockfd);
}

int tcpclient_close(void* tcp_hdr)
{
	struct tcp_ctl* hdr = (struct tcp_ctl*)tcp_hdr;
	
	if(hdr->sockfd > 0)
		close(hdr->sockfd);
}


int conn_nonblock(int sockfd,const struct sockaddr_in *addr,socklen_t len,int nsec){

    int flags,mSocketNum,error,code;
    socklen_t mLen;
    fd_set wset;
    struct timeval tvla;
    //获取文件描述符状态信息
    flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd,F_SETFL,flags|O_NONBLOCK);

    error = 0;
    if((mSocketNum = connect(sockfd,(const sockaddr *)addr,len))==0){

        fcntl(sockfd, F_SETFL, flags);  /* restore file status flags */
        return (0);
    }else if(mSocketNum<0 && errno != EINPROGRESS){//不再连接进度中
        return -1;
    }

    FD_ZERO(&wset);
    FD_SET(sockfd,&wset);
    tvla.tv_sec = nsec;
    tvla.tv_usec = 0;
    //Linux中需要文件描述符+1,表示文件福描述集的总个数。在poll和epoll中则不需要。
    if(mSocketNum = select(sockfd+1,NULL,&wset,NULL,nsec?&tvla:NULL) == 0){

        close(sockfd);
        errno = ETIMEDOUT;
        return -1;
    }
    //判断是否在前面的集合中，也就是是不是要监听的对象
    if(FD_ISSET(sockfd,&wset)){
        mLen = sizeof(error);
        code = getsockopt(sockfd,SOL_SOCKET,SO_ERROR,&error,&mLen);

        /* 如果发生错误，Solaris实现的getsockopt返回-1，
         * 把pending error设置给errno. Berkeley实现的
         * getsockopt返回0, pending error返回给error. 
         * 我们需要处理这两种情况 */
        if (code < 0 || error) {
            close(sockfd);
            if (error) 
                errno = error;
            return (-1);
        } else {
            fprintf(stderr, "select error: sockfd not set");
            exit(0);
        }
    }
}

}