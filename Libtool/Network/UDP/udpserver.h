
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

// the udp-client
char* get_localtime(time_t now)
{
	struct tm time;
	localtime_r(&now, &time);

	bzero(buffer, 64/*buf_size*/);
	sprintf(buffer, "%4.4d/%2.2d/%2.2d/%2.2d:%2.2d:%2.2d", time.tm_year+1900, time.tm_mon+1, 
			time.tm_mday, time.tm_hour, time.tm_min, time.tm_sec);

	return (char*)buffer;
}

int main_test(int argc, char **argv)
{
    if (argc < 3)
    {
        fprintf(stderr, "Usage: ./udpserver 127.0.0.1 7777\n");
        exit(1);
    }

    uint8_t *ip = (uint8_t *)argv[1];
    uint16_t port = atoi(argv[2]);

    int udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_socket < 0)
    {
        return -1;
    }

    int save_mode = fcntl(udp_socket, F_GETFL, 0);
    fcntl(udp_socket, F_SETFL, save_mode | O_NONBLOCK);

    uint8_t recv_buf[64];
    uint8_t send_buf[64];
    int daddrlen = sizeof(struct sockaddr_in);
    struct sockaddr_in saddr, daddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &saddr.sin_addr);

    int ret = 0;
    int i = 0;
    while (1)
    {
        //Splice udp-packet
        int msg_len = sizeof(uint32_t) + sizeof(uint16_t) * 3;
        uint8_t *ptr = send_buf;
        *((uint32_t *)ptr) = KEEP_ALIVE_TYPE;
        ptr += sizeof(uint32_t);
        *((uint16_t *)ptr) = 0;
        ptr += sizeof(uint16_t);
        *((uint16_t *)ptr) = i++;

        ret = sendto(udp_socket, send_buf, msg_len, 0, (struct sockaddr *)&saddr, (socklen_t)sizeof(saddr));
        if (ret < 0)
        {
            printf("%s, %d,netflow.so: keep-alive send response pkt error", __FILE__, __LINE__);
            //return;
        }

        int addr_len = 0;
        ret = recvfrom(udp_socket, recv_buf, 64, 0, (struct sockaddr *)&daddr, (socklen_t *)&addr_len);
        if (ret > 0)
        {
            uint8_t* pt = recv_buf;
            uint32_t magic_code = *((uint32_t *)pt);
            if (magic_code != KEEP_ALIVE_TYPE)
                return;
            pt += sizeof(uint32_t);
            uint16_t opt_code = *((uint16_t *)pt);
            if (opt_code != 1)
                return;
            pt += sizeof(uint16_t);
            uint16_t opt_id = *((uint16_t *)pt);
            pt += sizeof(uint16_t);
            uint16_t status_code = *((uint16_t *)pt);

            time_t now = time(NULL);
            printf("%s: opt_code: %d, opt_id: %d, opt_status: %d\n", get_localtime(now), opt_code, opt_id, status_code);
        }

        sleep(10);
    }
}