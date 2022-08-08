///非阻塞connect的写法
///其实内部设置的应该是SOCKET的方式
#include<iostream>
#include<sys/types.h>
#include<ws2tcpip.h>
#include<fcntl.h>
using namespace std;

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
    }else if(n<0 && errno != EINPROGRESS){//不再连接进度中
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