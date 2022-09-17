#ifndef _TOOLS_REPO_H_
#define _TOOLS_REPO_H_

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include <random>

//字节的拷贝函数
//要考虑这之间的内存要是重叠了怎么办
/*
	不重叠：随便怎么拷贝都行
	源内存再后面：从头像后进行拷贝
	源内存再前面：从后往前拷贝
*/
void *memmove(void *dst, const void *src, size_t n)
{

	assert(dst && src);
	char *dst_t = (char *)dst;
	char *src_t = (char *)src_t;

	if (src_t > dst_t)
	{
		while (n--)
			*dst_t++ = *src_t++;
	}
	else if (src_t < dst_t)
	{
		dst_t += n - 1;
		src_t += n - 1;
		while (n--)
			*dst_t-- = *src_t--;
	}
	return dst;
}

//实现字符向整形类型转换的程序
int atoi(char *nptr)
{
	int Iterger_sin = 1;
	int Result_Interger = 0;

	assert(nptr);
	while (isspace(*nptr))
	{
		nptr++;
	}
	if (*nptr == '-')
		Iterger_sin = -1;
	if (*nptr == '-' || *nptr == '+')
		nptr++;

	while (*nptr >= '0' && *nptr <= '9')
	{
		Result_Interger += Result_Interger * 10 + *nptr - '0';
		nptr++;
	}
	Result_Interger *= Iterger_sin;
	return Result_Interger;
}

//字符分割函数
std::vector<std::string> Split(std::string &str, std::string flag)
{
	std::vector<std::string> vec;
	size_t last = 0;
	//返回的是第一个字符的下标
	size_t index = str.find(flag, last);
	// npos是-1，是结束也就是找不到了的时候
	//查找失败返回2^64-1
	while (index != std::string::npos)
	{
		if (index == last)
		{
			last = index + flag.size();
			index = str.find(flag, last);
			continue;
		}
		vec.push_back(str.substr(last, index - last));
		last = index + flag.size();
		index = str.find(flag, last);
	}
	//末端没有标记函数的情况
	if (index > last && index != last)
	{
		vec.push_back(str.substr(last, str.size() - last));
	}
	return vec;
}

// sort and dedumplicate the element with linux c
int sort_dedup(uint32_t *iplist, int *ipcnt)
{
#define MAX_IP_CNT 65535
	if (*ipcnt <= 0 || iplist == NULL)
		return -1;

	// sort
	int i, j, cnt = *ipcnt;
	uint32_t key = 0;
	for (j = 1; j < *ipcnt; j++)
	{
		i = j - 1;
		key = iplist[j];
		while (i >= 0 && iplist[i] > key)
		{
			iplist[i + 1] = iplist[i];
			i--;
		}
		iplist[i + 1] = key;
	}

	// deduplicate
	i = 0, j = 1;
	while (j < cnt)
	{
		if (iplist[j] == iplist[i])
		{
			j++;
			(*ipcnt)--;
		}
		else
		{
			iplist[++i] = iplist[j];
			j++;
		}
	}

	bzero(&iplist[++i], (MAX_IP_CNT - j + 1) * sizeof(uint32_t));
	return 0;
}

// Resolving the problem of buffer/cache increasing instantly
//  due to callback fwriet() frequently

/**
 * @brief the interface of linux c KAFKA, usring the open-source library of librdkafka
 *
 *
 */
#include "lib_source/librdkafka/rdkafka.h"

namespace kafka
{
	struct kfk_hdr
	{
		rd_kafka_t *rk;
		rd_kafka_topic_t *rk_topic;
	};

	void *kafka_prdcr_create(char *broker, char *topic)
	{
		struct kfk_hdr *t = (struct kfk_hdr *)pf_plug_malloc(global_plugid, sizeof(struct kfk_hdr));
		if (t == NULL)
		{
			pf_rtmsg("%s %d::kafka_create, malloc rec Error.\n", __FILE__, __LINE__);
		}
		memset(t, 0, sizeof(struct kfk_hdr));
		rd_kafka_t *rk = NULL;
		rd_kafka_topic_t *rkt = NULL;
		rd_kafka_conf_t *rd_conf;

		char errstr[512];
		// create a kafka config
		rd_conf = rd_kafka_conf_new();

		// create kafka broker cluster
		if (rd_kafka_conf_set(rd_conf, "bootstrap.servers", broker, errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK)
		{
			pf_rtmsg("%s %d::kafka_init rd_kafka_conf_new Fail.\n", __FILE__, __LINE__);
			pf_plug_free(global_plugid, t);
			return NULL;
		}

		// create producer living example
		rk = rd_kafka_new(RD_KAFKA_PRODUCER, rd_conf, errstr, sizeof(errstr));
		if (!rk)
		{
			pf_rtmsg("%s %d::kafka_init rd_kafka_new Fail, errstr:%s\n", errstr);
			pf_plug_free(global_plugid, t);
			return NULL;
		}

		// instantiation kafka topic
		rkt = rd_kafka_topic_new(rk, topic, NULL);
		if (rkt == NULL)
		{
			pf_rtmsg("%s %d::kafka_init rd_kafka_topic_new Fail.\n", __FILE__, __LINE__);
			pf_plug_free(global_plugid, t);
			return NULL;
		}

		t->rk = rk;
		t->rk_topic = rkt;

		return (void *)t;
	}

	void *kafka_consume_create(char *broker, char *group_id, char *topic)
	{

		struct kfk_hdr *t = (struct kfk_hdr *)pf_plug_malloc(global_plugid, sizeof(struct kfk_hdr));
		if (t == NULL)
		{
			pf_rtmsg("%s %d::kafka_create, malloc rec Error.\n", __FILE__, __LINE__);
		}
		memset(t, 0, sizeof(struct kfk_hdr));

		char errstr[512];

		rd_kafka_resp_err_t err;
		rd_kafka_t *rk = t->rk;
		rd_kafka_topic_t *rkt = t->rk_topic;
		rd_kafka_conf_t *rd_conf;
		rd_kafka_topic_partition_list_t *subscription;

		rd_conf = rd_kafka_conf_new();
		if (rd_kafka_conf_set(rd_conf, "bootstrap.servers", broker, errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK)
		{
			printf("%s %d::kafka_init rd_kafka_conf_new Fail.\n", __FILE__, __LINE__);
			pf_plug_free(global_plugid, t);
			return NULL;
		}
		if (rd_kafka_conf_set(rd_conf, "group.id", group_id, errstr, sizeof(errstr) != RD_KAFKA_CONF_OK))
		{
			printf("%s %d::kafka_init rd_kafka_conf group id set Failed.\n", __FILE__, __LINE__);
			pf_plug_free(global_plugid, t);
			return NULL;
		}
		if (rd_kafka_conf_set(rd_conf, "auto.offset.reset", "earliest", errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK)
		{
			printf("%s %d::kafka_init offset model set Failed.\n", __FILE__, __LINE__);
			pf_plug_free(global_plugid, t);
			return NULL;
		}
		rk = rd_kafka_new(RD_KAFKA_CONSUMER, rd_conf, errstr, sizeof(errstr));
		if (!rk)
		{
			printf("%s %d::kafka_init create consumer rk Failed.\n", __FILE__, __LINE__);
			pf_plug_free(global_plugid, t);
			return NULL;
		}

		rkt = rd_kafka_topic_new(rk, topic, NULL);
		if (rkt == NULL)
		{
			printf("%s %d::kafka_init rd_kafka_topic_new Fail.\n", __FILE__, __LINE__);
			pf_plug_free(global_plugid, t);
			return NULL;
		}

		rd_kafka_poll_set_consumer(rk);
		subscription = rd_kafka_topic_partition_list_new(1);
		rd_kafka_topic_partition_list_add(subscription, topic, -1);
		if ((err = rd_kafka_subscribe(rk, subscription)))
		{
			printf("%% Failed to start consuming topics: %s", rd_kafka_err2str(err));
			pf_plug_free(global_plugid, t);
			return NULL;
		}

		return (void *)t;
	}

	int kafka_prdcr(void *hdr, uint8_t *data, uint32_t len)
	{
		struct kfk_hdr *t = (struct kfk_hdr *)hdr;
		rd_kafka_t *rk = t->rk;
		rd_kafka_topic_t *rkt = t->rk_topic;

		// produce date to kafka broker
		if (rd_kafka_produce(rkt, RD_KAFKA_PARTITION_UA, RD_KAFKA_MSG_F_COPY, data, len, NULL, 0, NULL) == -1)
		{
			return -1;
		}

		rd_kafka_poll(rk, 0);

		return 0;
	}

	int kafka_consume(void *hdr, uint8_t *data, uint32_t max_len)
	{
		struct kfk_hdr *t = (struct kfk_hdr *)hdr;
		rd_kafka_t *rk = t->rk;
		rd_kafka_message_t *rkmessage = NULL;

		rkmessage = rd_kafka_consumer_poll(rk, 100);
		if (rkmessage && rkmessage->err == RD_KAFKA_RESP_ERR_NO_ERROR)
		{
			if (max_len < rkmessage->len)
			{
				rd_kafka_message_destroy(rkmessage);
				return -1;
			}

			memcpy(data, rkmessage->payload, rkmessage->len);
			rd_kafka_message_destroy(rkmessage);

			return rkmessage->len;
			;
		}

		return 0;
	}

	void kafka_clean(void *hdr)
	{
		struct kfk_hdr *t = (struct kfk_hdr *)hdr;
		rd_kafka_t *rk = t->rk;
		rd_kafka_topic_t *rkt = t->rk_topic;

		rd_kafka_flush(rk, 10 * 1000);
		rd_kafka_topic_destroy(rkt);
		rd_kafka_destroy(rk);

		if (t != NULL)
		{
			pf_plug_free(global_plugid, t);
		}
	}
}
/**
 * @brief  YYJSON lib example
 *
 */
#include "lib_source/yyjson/yyjson.h"

namespace yyjson
{
	// usring the array
	int compile_json_result(char *filename, void *res)
	{
		struct json_result *result = (struct json_result *)res;
		struct file_result *node = NULL;

		if (filename == NULL)
			return -1;

		FILE *fd = fopen(filename, "w");
		if (fd == NULL)
			return -1;

		yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);
		yyjson_mut_val *root = yyjson_mut_obj(doc);
		yyjson_mut_doc_set_root(doc, root);

		yyjson_mut_obj_add_int(doc, root, "fileID", result->file_id);
		yyjson_mut_obj_add_int(doc, root, "status", result->status);
		yyjson_mut_obj_add_int(doc, root, "sonFileNum", result->son_file_num);
		yyjson_mut_obj_add_str(doc, root, "test1", result->test1);
		yyjson_mut_obj_add_str(doc, root, "test2", result->test2);
		yyjson_mut_obj_add_str(doc, root, "test3", result->test3);

		yyjson_mut_val *result_array = yyjson_mut_arr(doc);

		int i;
		for (i = 0; i < result->cnt; i++)
		{
			node = result->item[i];
			yyjson_mut_val *result_node = yyjson_mut_obj(doc);
			yyjson_mut_obj_add_str(doc, result_node, "fileName", node->file_name);
			yyjson_mut_obj_add_str(doc, result_node, "fileMD5", node->file_md5);
			yyjson_mut_obj_add_str(doc, result_node, "startTime", node->start_time);
			yyjson_mut_obj_add_str(doc, result_node, "endTime", node->end_time);
			yyjson_mut_obj_add_str(doc, result_node, "appName", node->app_name);
			yyjson_mut_obj_add_int(doc, result_node, "status", node->status);
			yyjson_mut_obj_add_str(doc, result_node, "msg", node->msg);

			yyjson_mut_arr_add_val(result_array, result_node);
		}

		yyjson_mut_obj_add_val(doc, root, "result", result_array);

		const char *json = yyjson_mut_write(doc, 0, NULL);
		if (json)
		{
			printf("json: %s\n", json);
			fwrite(json, 1, strlen(json), fd);
			free((void *)json);
		}

		str_free(res);
		fclose(fd);
		yyjson_mut_doc_free(doc);
	}

	// mutiple level
	int fill_event_json(void *hdr, uint8_t *buffer, uint32_t buffer_size)
	{
		yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);
		yyjson_mut_val *root = yyjson_mut_obj(doc);
		yyjson_mut_doc_set_root(doc, root);

		char *json = NULL;
		char *log_type = "pcap_hit";

		char timestamp[32];
		time_t t = *global_sys_time;
		struct tm *lt;
		lt = localtime(&t);
		memset(timestamp, 0, 32);
		strftime(timestamp, 32, "%Y-%m-%d %H:%M:%S", lt);

		yyjson_mut_obj_add_str(doc, root, "type", log_type);
		yyjson_mut_obj_add_str(doc, root, "timestamp", timestamp);
		yyjson_mut_obj_add_uint(doc, root, "ruleId", event_id);

		yyjson_mut_val *result = yyjson_mut_obj(doc);

		char srcip[32] = {0};
		char dstip[32] = {0};
		yyjson_mut_val *ipv4 = yyjson_mut_obj(doc);
		struct pms_stream_v4 p;
		memset(srcip, 0, 32);
		memset(srcip, 0, 32);
		sym->pf_ip2str(srcip, p.sip);
		sym->pf_ip2str(dstip, p.dip);
		yyjson_mut_obj_add_str(doc, ipv4, "srcIP", srcip);
		yyjson_mut_obj_add_str(doc, ipv4, "dstIP", dstip);
		yyjson_mut_obj_add_int(doc, ipv4, "srcPort", p.sip);
		yyjson_mut_obj_add_int(doc, ipv4, "dstPort", p.sport);

		yyjson_mut_val *ipv6 = yyjson_mut_obj(doc);
		struct pms_stream_v6 stream_v6;
		yyjson_mut_obj_add_str(doc, ipv6, "srcIP", stream_v6.sip);
		yyjson_mut_obj_add_str(doc, ipv6, "dstIP", stream_v6.dip);
		yyjson_mut_obj_add_int(doc, ipv6, "srcPort", stream_v6.sport);
		yyjson_mut_obj_add_int(doc, ipv6, "dstPort", stream_v6.dport);

		yyjson_mut_obj_add_val(doc, result, "IPv4", ipv4);
		yyjson_mut_obj_add_val(doc, result, "IPv6", ipv6);

		int return_info_len = 0;
		uint8_t return_info[65535];
		// vsos_b64_encode(return_info, return_info_len, buffer, buffer_size);

		yyjson_mut_obj_add_strn(doc, result, "msg", buffer, return_info_len);

		yyjson_mut_obj_add_val(doc, root, "result", result);

		json = yyjson_mut_write(doc, 0, NULL);
		uint64_t json_len = strlen(json);
		if (json_len > buffer_size /*buffer_size*/)
		{
			printf("compile_json_advice faild, json string is biger than buffer size\n");
			return -1;
		}

		if (json)
		{
			memcpy(buffer, json, json_len);
			free((void *)json);
		}

		yyjson_mut_doc_free(doc);

		return json_len;
	}

}
/**
 * @brief A simple memeory-pool structure, which is without the memalign and the memory uncontiguous
 *  just a single demo  wrote in my university
 *
 */

/*
	环形缓冲区，用以克服TCP读取和接受写入的速度问题。

*/

#pragma once

#include <memory>
#include <mutex>
#include <stdint.h>

namespace circle_buffer
{
	class CircleBuffer
	{
	public:
		enum
		{
			enmDefaultBufferLen = 72560,
		};

	public:
		CircleBuffer(const uint32_t size = enmDefaultBufferLen);
		~CircleBuffer();

		uint32_t Put(const char *buff, const uint32_t buffLen);
		uint32_t Get(char buf[], const uint32_t maxBuffLen);
		uint32_t Size()
		{
			return size;
		}
		uint32_t Abandoned(const uint32_t buffLen);

		uint32_t EmptySize();
		uint32_t UsedSize();
		const char *Buffer()
		{
			return buffer;
		}

		bool isEmpty()
		{
			return out >= in ? true : false;
		}

	private:
		uint32_t size;

		//两个缓冲区标识符量
		volatile uint32_t in;
		volatile uint32_t out;
		char *buffer;
	};

	CircleBuffer::CircleBuffer(const uint32_t size)
		: in(0), out(0), size(0)
	{
		buffer = new (std::nothrow) char[size];
		if (!buffer)
		{
			return;
		}
		memset(buffer, 0, size);
		this->size = size;
	}

	CircleBuffer::~CircleBuffer()
	{
		if (buffer)
		{
			delete[] buffer;
		}
	}

	uint32_t CircleBuffer::Put(const char *buff, const uint32_t bufferLen)
	{
		uint32_t lengthToPut = min(bufferLen, EmptySize());

		// Starting puts data from labol of in
		uint32_t len = min(lengthToPut, size - (in % size));
		memcpy(buffer + (in % size), buff, len);

		// Moveing the rest data to the beginning of the buffer
		memcpy(buffer, buff + len, lengthToPut - len);
		in += lengthToPut;
		return lengthToPut;
	}
	uint32_t CircleBuffer::Get(char buf[], const uint32_t maxBuffLen)
	{
		uint32_t lengthToGet = min(maxBuffLen, UsedSize());

		uint32_t len = min(lengthToGet, size - (out % size));
		memcpy(buf, buffer + out % size, len);

		memcpy(buf + len, buffer, lengthToGet - len);
		out += lengthToGet;
		return lengthToGet;
	}

	uint32_t CircleBuffer::Abandoned(const uint32_t bufferLen)
	{

		char tempBuff[65535] = {0};

		uint32_t lengthToAbandon = min(bufferLen, UsedSize());
		uint32_t len = min(lengthToAbandon, size - (out % size));
		memcpy(tempBuff, buffer + out % size, len);
		memcpy(tempBuff + len, buffer, lengthToAbandon - len);
		out += lengthToAbandon;
		return lengthToAbandon;
	}

	uint32_t CircleBuffer::EmptySize()
	{
		return size - in + out;
	}
	uint32_t CircleBuffer::UsedSize()
	{
		return in - out;
	}
}

/**
 * @brief udp keep-alive funtiong
 * 
 */
//Init the keep-alive server
#include <net/if.h>
#include <sys/ioctl.h>

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

//implement with epoll

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

    uint8_t *ip = argv[1];
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



#endif // !_TOOLS_REPO_H_