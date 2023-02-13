/**
 * @brief the interface of linux c KAFKA, usring the open-source library of librdkafka
 *
 *
 */
#include "../lib_source/librdkafka/rdkafka.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

namespace kafka
{
	struct kfk_hdr
	{
		rd_kafka_t *rk;
		rd_kafka_topic_t *rk_topic;
	};

	void *kafka_prdcr_create(char *broker, char *topic)
	{
		struct kfk_hdr *t = (struct kfk_hdr *)malloc(sizeof(struct kfk_hdr));
		if (t == NULL)
		{
			printf("%s %d::kafka_create, malloc rec Error.\n", __FILE__, __LINE__);
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
			printf("%s %d::kafka_init rd_kafka_conf_new Fail.\n", __FILE__, __LINE__);
			free( t);
			return NULL;
		}

		// create producer living example
		rk = rd_kafka_new(RD_KAFKA_PRODUCER, rd_conf, errstr, sizeof(errstr));
		if (!rk)
		{
			printf("%s %d::kafka_init rd_kafka_new Fail, errstr:%s\n", errstr);
			free( t);
			return NULL;
		}

		// instantiation kafka topic
		rkt = rd_kafka_topic_new(rk, topic, NULL);
		if (rkt == NULL)
		{
			printf("%s %d::kafka_init rd_kafka_topic_new Fail.\n", __FILE__, __LINE__);
			free( t);
			return NULL;
		}

		t->rk = rk;
		t->rk_topic = rkt;

		return (void *)t;
	}

	void *kafka_consume_create(char *broker, char *group_id, char *topic)
	{

		struct kfk_hdr *t = (struct kfk_hdr *)malloc( sizeof(struct kfk_hdr));
		if (t == NULL)
		{
			printf("%s %d::kafka_create, malloc rec Error.\n", __FILE__, __LINE__);
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
			free( t);
			return NULL;
		}
		if (rd_kafka_conf_set(rd_conf, "group.id", group_id, errstr, sizeof(errstr) != RD_KAFKA_CONF_OK))
		{
			printf("%s %d::kafka_init rd_kafka_conf group id set Failed.\n", __FILE__, __LINE__);
			free( t);
			return NULL;
		}
		if (rd_kafka_conf_set(rd_conf, "auto.offset.reset", "earliest", errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK)
		{
			printf("%s %d::kafka_init offset model set Failed.\n", __FILE__, __LINE__);
			free( t);
			return NULL;
		}
		rk = rd_kafka_new(RD_KAFKA_CONSUMER, rd_conf, errstr, sizeof(errstr));
		if (!rk)
		{
			printf("%s %d::kafka_init create consumer rk Failed.\n", __FILE__, __LINE__);
			free( t);
			return NULL;
		}

		rkt = rd_kafka_topic_new(rk, topic, NULL);
		if (rkt == NULL)
		{
			printf("%s %d::kafka_init rd_kafka_topic_new Fail.\n", __FILE__, __LINE__);
			free( t);
			return NULL;
		}

		rd_kafka_poll_set_consumer(rk);
		subscription = rd_kafka_topic_partition_list_new(1);
		rd_kafka_topic_partition_list_add(subscription, topic, -1);
		if ((err = rd_kafka_subscribe(rk, subscription)))
		{
			printf("%% Failed to start consuming topics: %s", rd_kafka_err2str(err));
			free( t);
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
			free( t);
		}
	}
}
/**
 * @brief  YYJSON lib example
 *
 */
