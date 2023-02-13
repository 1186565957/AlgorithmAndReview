
/**
 * mhash_crc32.h
 * use for hash_crc32 compare 
 *   */

#ifndef _M_HASH_CRC_H
#define _M_HASH_CRC_H

#include <stdint.h>
#include <pthread.h>


//M-Hash technology
#undef MHS_JTYPE
#undef MHS_JCLOSE
#undef MHS_HSIZE
#undef MHS_HKEY

#define MHS_JTYPE		 struct mhashj
#define MHS_JCLOSE(P, J) pf_free_data((J)->data)
#define MHS_HSIZE		 hash_size
#define MHS_HKEY(P, J)	 m_hash_hash(((J)->key), (P->table_size))

//
#define MHASH_CLOCK_INTERVAL  100

#define MAX_KEYWORD_LEN	 32
#define MAX_MAPKEY		 16

// mvalue for setmap
struct mvalue
{
	//hash key.
	//mark setmap function the second paramer.
	bool type;				//global_param_type.
	union
	{
		struct 
		{
			//hash key.
			uint8_t mapkey[MAX_MAPKEY];
			//hash2
			uint32_t hash2;
		};
		
		//ip.src
		uint32_t ip;
	};

	//keyword as the first parammer setmap(). 
	uint8_t keyword[MAX_KEYWORD_LEN];
	uint32_t keyword_len;
	
	uint8_t* value1;
	uint32_t value1_len;
	uint8_t* value2;
	uint32_t value2_len;
	uint8_t* value3;	
	uint32_t value3_len;

	uint8_t* data;

	time_t timeout;
	time_t starttime;
};

struct mvalue_dec
{
	//hash key.
	uint8_t mapkey[16];
	
	//keyword as the first parammer setmap(). 
	uint8_t keyword[32];
	uint32_t keyword_len;
	
	//hash2
	uint32_t hash2;

	uint8_t* value1;
	uint32_t value1_len;
	uint8_t* value2;
	uint32_t value2_len;
	uint8_t* value3;	
	uint32_t value3_len;

	uint8_t* data;

	time_t timeout;
	time_t starttime;
};

//
struct mhashj
{
	//private crc32 key
	uint32_t key;

    //private data
	void* data; 

	//data free function
	PF_MHASH_FREE_DATA pf_free_data;

	MHS_JDATA_STUB;
};

struct mhash
{
	int total_num;

	//hash table length
	int table_size;

	//read/write lock 
	pthread_rwlock_t lock;
	pthread_mutex_t  mutex;

	MHS_TDATA_STUB;
};

//Multi-thread, include read/write lock
void* m_hash_create(int hash_size);
void m_hash_free(void* mhash);
void m_hash_clean(void* mhash);
int m_hash_add(void* mhash, uint8_t* buf, int buf_len, PF_MHASH_FREE_DATA pf_free_data, void* data);
int m_hash_add2(void* mhash, uint8_t* buf, int buf_len, uint8_t* key, PF_MHASH_FREE_DATA pf_free_data, void* data);
int m_hash_del(void* mhash, uint8_t* buf, int buf_len);
int m_hash_lookup(void* mhash, uint8_t* buf, int buf_len, void** data);
int m_hash_print(void* mhash, FILE* fp, void* callback);
uint64_t m_hash_size(void* mhash);

//Single-thread
void* s_hash_create(int hash_size);
void s_hash_free(void* mhash);
void s_hash_clean(void* mhash);
int s_hash_add(void* mhash, uint8_t* buf, int buf_len, PF_MHASH_FREE_DATA pf_free_data, void* data);
int s_hash_add2(void* mhash, uint8_t* buf, int buf_len, uint8_t* key, PF_MHASH_FREE_DATA pf_free_data, void* data);
int s_hash_del(void* mhash, uint8_t* buf, int buf_len);
int s_hash_lookup(void* mhash, uint8_t* buf, int buf_len, void** data);
int s_hash_print(void* mhash, FILE* fp, void* callback);
uint64_t s_hash_size(void* mhash);

//Multi-thread.
void* map_hash_create(int hash_size);
void  map_hash_free(void* mhash);
void  map_hash_clean(void* mhash);
int   map_hash_add(void* mhash, PF_MHASH_FREE_DATA pf_free_data, void* data);
int   map_hash_del(void* mhash,  void* data);
void*   map_hash_lookup(void* mhash, void* data);
int   map_hash_print(void* mhash, FILE* fp, void* callback);
int   map_hash_list(void* mhash, void* callback);
uint64_t   map_hash_size(void* mhash);

//only for mapkey
void* map_dec_hash_create(int hash_size);
void  map_dec_hash_free(void* mhash);
void  map_dec_hash_clean(void* mhash);
int   map_dec_hash_add(void* mhash, PF_MHASH_FREE_DATA pf_free_data, void* data);
int   map_dec_hash_del(void* mhash,  void* data);
void*   map_dec_hash_lookup(void* mhash, void* data);
int   map_dec_hash_print(void* mhash, FILE* fp, void* callback);
int   map_dec_hash_list(void* mhash, void* callback);
uint64_t map_dec_hash_size(void* mhash);

#endif

