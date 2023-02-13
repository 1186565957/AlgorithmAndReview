/**
 * @brief udp keep-alive funtiong
 * 
 */
//Init the keep-alive server
#include <net/if.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/socket.h>

void init_server()
{
	int aliveoff = 1;
	int udpservfd = socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in serv_addr;
	struct ifreq ifr;
	memset(&ifr, 0, sizeof(struct ifreq));

	//get the local ip-address
	{
		uint32_t name_len = strlen(config->netflow_ethname);
		memcpy(ifr.ifr_name, config->netflow_ethname, name_len);
		ifr.ifr_name[name_len] = '\0';
		int ret = 0;
		if ((ret = ioctl(ntfl->udpservfd, SIOCGIFADDR, &ifr)) < 0)
		{
			return -1;
		}
	}
	memcpy(&serv_addr, &ifr.ifr_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(config->netflow_liveport);
	bind(ntfl->udpservfd, (struct sockaddr *)&serv_addr, (socklen_t)sizeof(struct sockaddr));
}

//implement with the select
void server_task()
{
	fd_set *fds = &ntfl->fds;
	FD_ZERO(fds);
	FD_SET(ntfl->udpservfd, fds);

	timeout.tv_sec = 1;
	timeout.tv_usec = 0;

	if ((ret = select(ntfl->udpservfd + 1, &ntfl->fds, NULL, NULL, &timeout)) > 0)
	{
		while ((ret = recvfrom(ntfl->udpservfd, recv_buf, 64, 0, (struct sockaddr *)&addr, (socklen_t *)&addr_len)) > 0)
		{
			uint32_t magic_code = 0;
			uint16_t opt_code = 0;
			uint16_t opt_id = 0;
			uint16_t status_code = 0;

			uint8_t *pt = recv_buf;
			magic_code = *((uint32_t *)pt);
			if (magic_code != KEEP_ALIVE_TYPE)
				return;
			pt += sizeof(uint32_t);
			opt_code = *((uint16_t *)pt);
			if (opt_code != 0)
				return;
			pt += sizeof(uint16_t);
			opt_id = *((uint16_t *)pt);

			status_code = find_usockj(&addr);

			//Splice udp-packet
			int msg_len = sizeof(uint32_t) + sizeof(uint16_t) * 3;
			uint8_t *ptr = send_buf;
			*((uint32_t *)ptr) = magic_code;
			ptr += sizeof(uint32_t);
			*((uint16_t *)ptr) = 1;
			ptr += sizeof(uint16_t);
			*((uint16_t *)ptr) = opt_id;
			//the sock status, 0 is right, 1 is error
			ptr += sizeof(uint16_t);
			*((uint16_t *)ptr) = status_code;

			ret = sendto(ntfl->udpservfd, send_buf, msg_len, 0, (struct sockaddr *)&addr, (socklen_t)sizeof(addr));
			if (ret < 0)
			{
				printf("%s, %d, keep-alive send response pkt error", __FILE__, __LINE__);
				return;
			}
		}
	}
}