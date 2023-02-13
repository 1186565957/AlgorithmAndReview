
#include "../lib_source/yyjson/yyjson.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
