#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <sys/queue.h>
#include <mtx.h>


static uint8_t hasht_level = 4;

static inline uint32_t m_hash_hash(const uint32_t key, int table_size)
{
	return key % table_size;
}

//Multi-thread
/**
 * mhash create
 * 
 * @param hash_size
 *		hash size
 * @return
 *		On success return a pointer of mhash
 *		On error return NULL
 */
void* m_hash_create(int hash_size)
{
	if (hash_size <= 0)
		return NULL;

	struct mhash* p = (struct mhash*)pf_plug_malloc(global_plugid, sizeof(struct mhash));
	if (p == NULL)
		return NULL;
	memset(p, 0, sizeof(struct mhash));

	if (!(MHS_INITIALIZE(p, hash_size)))
	{
		pf_plug_free(global_plugid, p);
		return NULL;
	}

	p->total_num = hash_size;
	p->table_size = p->total_num / hasht_level + 1;
	if (pthread_rwlock_init(&p->lock, NULL))
	{
		//rwlock init error
		return NULL;
	}

	return p;
}

void m_hash_free(void* mhash)
{
	if (mhash == NULL)
		return;

	struct mhash* p = (struct mhash*)mhash;

	while (pthread_rwlock_wrlock(&p->lock))
	{
		//sleep if any other thread is writing/reading this hash table
		usleep(MHASH_CLOCK_INTERVAL);
	}

	//clean data in this table
	struct mhashj* j = p->head;
	int i;
	for (i = 0; i < p->total_num; i++)
	{
		if (j == NULL)
		{
			//already has no any node on this table
			return;
		}
		
		j->pf_free_data(j->data);

		j = j->next;
	}

	MHS_CLEAN(p);

	pthread_rwlock_unlock(&p->lock);
	pthread_rwlock_destroy(&p->lock);

	pf_plug_free(global_plugid, p);
}

void m_hash_clean(void* mhash)
{
	struct mhash* p = (struct mhash*)mhash;

	while (pthread_rwlock_wrlock(&p->lock))
	{
		//sleep if any other thread is writing/reading this hash table
		usleep(MHASH_CLOCK_INTERVAL);
	}

	int i;
	for (i = 0; i < p->total_num; i++)
	{
		struct mhashj* j = p->head;

		if (j == NULL)
		{
			//already has no any node on this table
			pthread_rwlock_unlock(&p->lock);

			return;
		}
		
		j->pf_free_data(j->data);

		MHS_DELETE(p, j);
	}
}

/**
 * mhash add
 * 
 * @param mhash
 *		The mhash object.
 * @param buf
 *		add string
 * @param buf_len
 *		string length
 * @param pf_free_data
 *		function of free
 * @param data
 *		private data
 * @return
 *		On success return a pointer of mhash
 *		On error return NULL
 */
int m_hash_add(void* mhash, uint8_t* buf, int buf_len, PF_MHASH_FREE_DATA pf_free_data, void* data)
{
	assert(data != NULL || mhash != NULL);

	struct mhash* p = (struct mhash*)mhash;

	uint32_t key = pf_crc32(buf, buf_len);
	uint32_t position = m_hash_hash(key, p->table_size);

	while (pthread_rwlock_trywrlock(&p->lock))
	{
		//sleep if any other thread is writing/reading this hash table
		usleep(MHASH_CLOCK_INTERVAL);
	}

	struct mhashj* j = p->hash[position];
	while (j != NULL)
	{
		if (j->key == key)
		{
			//already exit in hash, ignore it
			pthread_rwlock_unlock(&p->lock);

			return 1;
		}

		j = j->next_colli;
	}

	//not find in hash, add it
	j = MHS_ADD(p, position);

	//in current mtx, oldest node will be erased to ensure newer add
	//thus, never return NULL even memory insufficient.
	assert(j != NULL);

	j->key = key;
	j->data = data;
	j->pf_free_data = pf_free_data;

	pthread_rwlock_unlock(&p->lock);

	return 1;
}

int m_hash_add2(void* mhash, uint8_t* buf, int buf_len, uint8_t* key, PF_MHASH_FREE_DATA pf_free_data, void* data)
{
	assert(data != NULL || mhash != NULL);

	struct mhash* p = (struct mhash*)mhash;

	if (buf != NULL && buf_len > 0)
	{
		*key = pf_crc32(buf, buf_len);
	}
	uint32_t position = m_hash_hash(*key, p->table_size);

	while (pthread_rwlock_trywrlock(&p->lock))
	{
		//sleep if any other thread is writing/reading this hash table
		usleep(MHASH_CLOCK_INTERVAL);
	}

	struct mhashj* j = p->hash[position];
	while (j != NULL)
	{
		if (j->key == *key)
		{
			//already exit in hash, ignore it
			pthread_rwlock_unlock(&p->lock);

			return 1;
		}

		j = j->next_colli;
	}

	//not find in hash, add it
	j = MHS_ADD(p, position);

	//in current mtx, oldest node will be erased to ensure newer add
	//thus, never return NULL even memory insufficient.
	assert(j != NULL);

	j->key = *key;
	j->data = data;
	j->pf_free_data = pf_free_data;

	pthread_rwlock_unlock(&p->lock);

	return 1;
}

int m_hash_del(void* mhash, uint8_t* buf, int buf_len)
{	
	assert(mhash != NULL);

	struct mhash* p = (struct mhash*)mhash;

	uint32_t key = pf_crc32(buf, buf_len);
	uint32_t position = m_hash_hash(key, p->table_size);

	while (pthread_rwlock_trywrlock(&p->lock))
	{
		//sleep if any other thread is writing/reading this hash table
		usleep(MHASH_CLOCK_INTERVAL);
	}

	struct mhashj* j = p->hash[position];
	while (j != NULL)
	{
		if (j->key == key)
		{
			//node exit in hash table, delete it
			j->pf_free_data(j->data);
			MHS_DELETE(p, j);

			pthread_rwlock_unlock(&p->lock);

			return 1;
		}

		j = j->next_colli;
	}

	//no find
	pthread_rwlock_unlock(&p->lock);
	return -1;
}

int m_hash_lookup(void* mhash, uint8_t* buf, int buf_len, void** data)
{
	assert(data != NULL || mhash != NULL);

	struct mhash* p = (struct mhash*)mhash;

	uint32_t key = pf_crc32(buf, buf_len);
	uint32_t position = m_hash_hash(key, p->table_size);

	while (pthread_rwlock_tryrdlock(&p->lock))
	{
		//sleep if any other threads is writing this hash table
		usleep(MHASH_CLOCK_INTERVAL);
	}

	struct mhashj* j = p->hash[position];
	while (j != NULL)
	{
		if (j->key == key)
		{
			*data = j->data;

			pthread_rwlock_unlock(&p->lock);
			return 1;
		}

		j = j->next_colli;
	}

	//no find
	pthread_rwlock_unlock(&p->lock);

	return -1;
}

int m_hash_print(void* mhash, FILE* fp, void* callback)
{
	struct mhash* p = (struct mhash*)mhash;

	PF_MHASH_PRINT pf_mhash_print = (PF_MHASH_PRINT)callback;	

    int count = 0;

	while (pthread_rwlock_tryrdlock(&p->lock))
	{
		//sleep if any other threads is writing this hash table
		usleep(MHASH_CLOCK_INTERVAL);
	}

	struct mhashj* j = p->head;

	//item
	int i;
	for(i = 0; i < p->total_num; i++)
	{
		if (j == NULL)
		{
			//no exist
			break;
		}

		fprintf(fp, "item [%d]\t", i);

		pf_mhash_print(fp, j->data);
		
		fprintf(fp, "\n");

		count++;

		j = j->next;
	}

	pthread_rwlock_unlock(&p->lock);

	//handle 
	fprintf(fp, "%-32s%d\n%-32s%d\n","total num:", p->total_num, "current num:", count);

	return 1;
}

uint64_t m_hash_size(void* mhash)
{
	return MHS_USED_MEMORY((struct mhash*)mhash);
}


//Single-thread
/**
 * shash create
 * 
 * @param hash_size
 *		hash size
 * @return
 *		On success return a pointer of mhash
 *		On error return NULL
 */
void* s_hash_create(int hash_size)
{
	if (hash_size <= 0)
		return NULL;

	struct mhash* p = (struct mhash*)pf_plug_malloc(global_plugid, sizeof(struct mhash));
	if (p == NULL)
		return NULL;
	memset(p, 0, sizeof(struct mhash));

	if (!(MHS_INITIALIZE(p, hash_size)))
	{
		pf_plug_free(global_plugid, p);
		return NULL;
	}

	p->total_num = hash_size;
	p->table_size = p->total_num / hasht_level + 1;

	return p;
}

void s_hash_free(void* mhash)
{
	if (mhash == NULL)
		return;

	struct mhash* p = (struct mhash*)mhash;

	//clean data in this table
	struct mhashj* j = p->head;
	int i;
	for (i = 0; i < p->total_num; i++)
	{
		if (j == NULL)
		{
			//already has no any node on this table
			return;
		}
		
		j->pf_free_data(j->data);

		j = j->next;
	}

	MHS_CLEAN(p);

	pf_plug_free(global_plugid, p);
}

void s_hash_clean(void* mhash)
{
	struct mhash* p = (struct mhash*)mhash;

	int i;
	for (i = 0; i < p->total_num; i++)
	{
		struct mhashj* j = p->head;

		if (j == NULL)
		{
			//already has no any node on this table
			return;
		}
		
		j->pf_free_data(j->data);

		MHS_DELETE(p, j);
	}
}

int s_hash_add(void* mhash, uint8_t* buf, int buf_len, PF_MHASH_FREE_DATA pf_free_data, void* data)
{
	assert(data != NULL || mhash != NULL);

	struct mhash* p = (struct mhash*)mhash;

	uint32_t key = pf_crc32(buf, buf_len);
	uint32_t position = m_hash_hash(key, p->table_size);

	struct mhashj* j = p->hash[position];
	while (j != NULL)
	{
		if (j->key == key)
		{
			//already exit in hash, ignore it
			return 1;
		}

		j = j->next_colli;
	}

	//not find in hash, add it
	j = MHS_ADD(p, position);

	//in current mtx, oldest node will be erased to ensure newer add
	//thus, never return NULL even memory insufficient.
	assert(j != NULL);

	j->key = key;
	j->data = data;
	j->pf_free_data = pf_free_data;

	return 1;
}

int s_hash_add2(void* mhash, uint8_t* buf, int buf_len, uint8_t* key, PF_MHASH_FREE_DATA pf_free_data, void* data)
{
	assert(data != NULL || mhash != NULL);

	struct mhash* p = (struct mhash*)mhash;

	if (buf != NULL && buf_len > 0)
	{
		*key = pf_crc32(buf, buf_len);
	}
	uint32_t position = m_hash_hash(*key, p->table_size);

	struct mhashj* j = p->hash[position];
	while (j != NULL)
	{
		if (j->key == *key)
		{
			//already exit in hash, ignore it
			return 1;
		}

		j = j->next_colli;
	}

	//not find in hash, add it
	j = MHS_ADD(p, position);

	//in current mtx, oldest node will be erased to ensure newer add
	//thus, never return NULL even memory insufficient.
	assert(j != NULL);

	j->key = *key;
	j->data = data;
	j->pf_free_data = pf_free_data;

	return 1;
}

int s_hash_del(void* mhash, uint8_t* buf, int buf_len)
{	
	assert(mhash != NULL);

	struct mhash* p = (struct mhash*)mhash;

	uint32_t key = pf_crc32(buf, buf_len);
	uint32_t position = m_hash_hash(key, p->table_size);

	struct mhashj* j = p->hash[position];
	while (j != NULL)
	{
		if (j->key == key)
		{
			//node exit in hash table, delete it
			j->pf_free_data(j->data);
			MHS_DELETE(p, j);

			return 1;
		}

		j = j->next_colli;
	}

	//no find
	return -1;
}

int s_hash_lookup(void* mhash, uint8_t* buf, int buf_len, void** data)
{
	assert(data != NULL || mhash != NULL);

	struct mhash* p = (struct mhash*)mhash;

	uint32_t key = pf_crc32(buf, buf_len);
	uint32_t position = m_hash_hash(key, p->table_size);

	struct mhashj* j = p->hash[position];
	while (j != NULL)
	{
		if (j->key == key)
		{
			*data = j->data;

			return 1;
		}

		j = j->next_colli;
	}

	//no find
	return -1;
}

int s_hash_print(void* mhash, FILE* fp, void* callback)
{
	struct mhash* p = (struct mhash*)mhash;

	PF_MHASH_PRINT pf_mhash_print = (PF_MHASH_PRINT)callback;	

    int count = 0;
	struct mhashj* j = p->head;

	//item
	int i;
	for(i = 0; i < p->total_num; i++)
	{
		if (j == NULL)
		{
			//no exist
			break;
		}

		fprintf(fp, "item [%d]\t", i);

		pf_mhash_print(fp, j->data);
		
		fprintf(fp, "\n");

		count++;

		j = j->next;
	}

	//handle 
	fprintf(fp, "%-32s%d\n%-32s%d\n","total num:", p->total_num, "current num:", count);

	return 1;
}

uint64_t s_hash_size(void* mhash)
{
	return MHS_USED_MEMORY((struct mhash*)mhash);
}

//Multi-thread
/**
 * shash create
 * 
 * @param hash_size
 *		hash size
 * @return
 *		On success return a pointer of mhash
 *		On error return NULL
 */
void* map_hash_create(int hash_size)
{
	if (hash_size <= 0)
		return NULL;

	struct mhash* p = (struct mhash*)pf_plug_malloc(global_plugid, sizeof(struct mhash));
	if (p == NULL)
		return NULL;
	memset(p, 0, sizeof(struct mhash));

	if (!(MHS_INITIALIZE(p, hash_size)))
	{
		pf_plug_free(global_plugid, p);
		return NULL;
	}

	p->total_num = hash_size;
	p->table_size = p->total_num / hasht_level + 1;

	//init lock.
	pthread_mutex_init(&p->mutex, NULL);

	return p;
}

void map_hash_free(void* mhash)
{
	if (mhash == NULL)
		return;

	struct mhash* p = (struct mhash*)mhash;

	//clean data in this table
	struct mhashj* j = p->head;
	
	int i;
	for (i = 0; i < p->total_num; i++)
	{
		if (j == NULL)
		{
			//already has no any node on this table
			break;
		}
		
		//
		assert(j->data != NULL);
		
		j->pf_free_data(j->data);

		//
		j = j->next;
	}

	MHS_CLEAN(p);

	pthread_mutex_destroy(&p->mutex);
	
	pf_plug_free(global_plugid, p);
}

void map_hash_clean(void* mhash)
{
	struct mhash* p = (struct mhash*)mhash;

	int i;
	for (i = 0; i < p->total_num; i++)
	{
		struct mhashj* j = p->head;

		if (j == NULL)
		{
			//already has no any node on this table
			return;
		}
		
		//
		assert(j->data != NULL);
		
		j->pf_free_data(j->data);

		MHS_DELETE(p, j);
	}
}

int map_hash_add(void* mhash, PF_MHASH_FREE_DATA pf_free_data, void* data)
{
	assert(data != NULL || mhash != NULL);

	struct mhash* p = (struct mhash*)mhash;

	struct mvalue* t = (struct mvalue*)data;
	
	uint32_t key = 0;
	if (t->type)
	{
		key = t->ip;
	}
	else
	{
		key = t->hash2;
	}
	uint32_t position = m_hash_hash(key, p->table_size);
	
	//check current hash_tbl.
	pthread_mutex_lock(&p->mutex);
	struct mhashj* j = p->hash[position];
	
	while (j != NULL)
	{
		struct mvalue* t2 = (struct mvalue*)j->data;
		
		//
		assert(j->data != NULL);

		if (((uint64_t*)t2->mapkey)[0] == ((uint64_t*)t->mapkey)[0] &&
				((uint64_t*)t2->mapkey)[1] == ((uint64_t*)t->mapkey)[1])
		{
			if (memcmp(t2->keyword, t->keyword, t->keyword_len) == 0)
			{
				//already exit in hash, ignore it
				pthread_mutex_unlock(&p->mutex);
				return -1;
			}
		}

		j = j->next_colli;
	}

	//not find in hash, add it
	j = MHS_ADD(p, position);

	//in current mtx, oldest node will be erased to ensure newer add
	//thus, never return NULL even memory insufficient.
	assert(j != NULL);

	j->key = key;
	j->data = data;
	j->pf_free_data = pf_free_data;
	
	pthread_mutex_unlock(&p->mutex);

	return 1;
}

int map_hash_del(void* mhash, void* data)
{	
	assert(mhash != NULL);

	struct mhash* p = (struct mhash*)mhash;
	
	//node.
	struct mvalue* t = (struct mvalue*)data;
	
	uint32_t key = 0;
	if (t->type)
	{
		key = t->ip;
	}
	else
	{
		key = t->hash2;
	}

	//check
	uint32_t position = m_hash_hash(key, p->table_size);
	
	pthread_mutex_lock(&p->mutex);

	struct mhashj* j = p->hash[position];
	while (j != NULL)
	{
		struct mvalue* t2 = (struct mvalue*)j->data;
		//
		assert(j->data != NULL);

		if (((uint64_t*)t2->mapkey)[0] == ((uint64_t*)t->mapkey)[0] &&
				((uint64_t*)t2->mapkey)[1] == ((uint64_t*)t->mapkey)[1])
		{
			if (memcmp(t2->keyword, t->keyword, t->keyword_len) == 0)
			{
				//node exit in hash table, delete it
				j->pf_free_data(j->data);
				MHS_DELETE(p, j);

				pthread_mutex_unlock(&p->mutex);
				return 1;
			}
		}

		j = j->next_colli;
	}

	pthread_mutex_unlock(&p->mutex);
	//no find
	return -1;
}

void* map_hash_lookup(void* mhash,  void* data)
{
	assert(data != NULL || mhash != NULL);

	struct mhash* p = (struct mhash*)mhash;

	//node
	struct mvalue* t = (struct mvalue*)data;
	
	uint32_t key = 0;
	if (t->type)
	{
		key = t->ip;
	}
	else
	{
		key = t->hash2;
	}

	//check
	uint32_t position = m_hash_hash(key, p->table_size);
	struct mhashj* j = p->hash[position];
	while (j != NULL)
	{
		//
		struct mvalue* t2 = (struct mvalue*)j->data;
		//
		assert(j->data != NULL);

		if (((uint64_t*)t2->mapkey)[0] == ((uint64_t*)t->mapkey)[0] &&
				((uint64_t*)t2->mapkey)[1] == ((uint64_t*)t->mapkey)[1])
		{
			if (memcmp(t2->keyword, t->keyword, t->keyword_len) == 0)
			{
				return j->data;
			}
		}

		j = j->next_colli;
	}

	//no find
	return NULL;
}

int map_hash_print(void* mhash, FILE* fp, void* callback)
{
	struct mhash* p = (struct mhash*)mhash;

	PF_MHASH_PRINT pf_mhash_print = (PF_MHASH_PRINT)callback;	

    int count = 0;
	struct mhashj* j = p->head;

	//item
	int i;
	for(i = 0; i < p->total_num; i++)
	{
		if (j == NULL)
		{
			//no exist
			break;
		}

		fprintf(fp, "item [%d]\t", i);

		pf_mhash_print(fp, j->data);
		
		fprintf(fp, "\n");

		count++;

		j = j->next;
	}

	//handle 
	fprintf(fp, "%-32s%d\n%-32s%d\n","total num:", p->total_num, "current num:", count);

	return 1;
}

int map_hash_list(void* mhash, void* callback)
{
	struct mhash* p = (struct mhash*)mhash;

	//PF_MAP_TIMEOUT map_check_timeout = (PF_MAP_TIMEOUT)callback;
	
    int count = 0;
	struct mhashj* j = p->head;

	//item
	int i;
	for(i = 0; i < p->total_num; i++)
	{
		if (j == NULL)
		{
			//no exist
			break;
		}

		if (((PF_MAP_TIMEOUT)callback)(j->data))
		{
			struct mvalue* t = (struct mvalue*)j->data;
			
			//
			assert(j->data != NULL);
			
			//timeout
			j->pf_free_data(j->data);
			
			//
			MHS_DELETE(p, j);

			return 1;
		}

		count++;
		j = j->next;
	}

	return 1;
}

uint64_t map_hash_size(void* mhash)
{
	return MHS_USED_MEMORY((struct mhash*)mhash);
}

//Multi-thread
/**
 * shash create
 * 
 * @param hash_size
 *		hash size
 * @return
 *		On success return a pointer of mhash
 *		On error return NULL
 */
void* map_dec_hash_create(int hash_size)
{
	if (hash_size <= 0)
		return NULL;

	struct mhash* p = (struct mhash*)pf_plug_malloc(global_plugid, sizeof(struct mhash));
	if (p == NULL)
		return NULL;
	memset(p, 0, sizeof(struct mhash));

	if (!(MHS_INITIALIZE(p, hash_size)))
	{
		pf_plug_free(global_plugid, p);
		return NULL;
	}

	p->total_num = hash_size;
	p->table_size = p->total_num / hasht_level + 1;

	//init lock.
	pthread_mutex_init(&p->mutex, NULL);

	return p;
}

void map_dec_hash_free(void* mhash)
{
	if (mhash == NULL)
		return;

	struct mhash* p = (struct mhash*)mhash;

	//clean data in this table
	struct mhashj* j = p->head;
	
	int i;
	for (i = 0; i < p->total_num; i++)
	{
		if (j == NULL)
		{
			//already has no any node on this table
			break;
		}
		
		//
		assert(j->data != NULL);
		
		j->pf_free_data(j->data);

		//
		j = j->next;
	}

	MHS_CLEAN(p);

	pthread_mutex_destroy(&p->mutex);
	
	pf_plug_free(global_plugid, p);
}

void map_dec_hash_clean(void* mhash)
{
	struct mhash* p = (struct mhash*)mhash;

	int i;
	for (i = 0; i < p->total_num; i++)
	{
		struct mhashj* j = p->head;

		if (j == NULL)
		{
			//already has no any node on this table
			return;
		}
		
		//
		assert(j->data != NULL);
		
		j->pf_free_data(j->data);

		MHS_DELETE(p, j);
	}
}

int map_dec_hash_add(void* mhash, PF_MHASH_FREE_DATA pf_free_data, void* data)
{
	assert(data != NULL || mhash != NULL);

	struct mhash* p = (struct mhash*)mhash;

	struct mvalue_dec* t = (struct mvalue_dec*)data;
	uint32_t key = t->hash2;
	
	uint32_t position = m_hash_hash(key, p->table_size);
	
	//check current hash_tbl.
	pthread_mutex_lock(&p->mutex);
	struct mhashj* j = p->hash[position];
	
	while (j != NULL)
	{
		struct mvalue_dec* t2 = (struct mvalue_dec*)j->data;
		//
		assert(j->data != NULL);

		if (((uint64_t*)t2->mapkey)[0] == ((uint64_t*)t->mapkey)[0] &&
				((uint64_t*)t2->mapkey)[1] == ((uint64_t*)t->mapkey)[1])
		{
			if (memcmp(t2->keyword, t->keyword, t->keyword_len) == 0)
			{
				//already exit in hash, ignore it
				pthread_mutex_unlock(&p->mutex);
				return -1;
			}
		}

		j = j->next_colli;
	}

	//not find in hash, add it
	j = MHS_ADD(p, position);

	//in current mtx, oldest node will be erased to ensure newer add
	//thus, never return NULL even memory insufficient.
	assert(j != NULL);

	j->key = key;
	j->data = data;
	j->pf_free_data = pf_free_data;
	
	pthread_mutex_unlock(&p->mutex);

	return 1;
}

int map_dec_hash_del(void* mhash, void* data)
{	
	assert(mhash != NULL);

	struct mhash* p = (struct mhash*)mhash;
	
	//node.
	struct mvalue_dec* t = (struct mvalue_dec*)data;
	uint32_t key = t->hash2;
	
	//check
	uint32_t position = m_hash_hash(key, p->table_size);
	
	pthread_mutex_lock(&p->mutex);

	struct mhashj* j = p->hash[position];
	while (j != NULL)
	{
		struct mvalue_dec* t2 = (struct mvalue_dec*)j->data;
		//
		assert(j->data != NULL);
		
		if (((uint64_t*)t2->mapkey)[0] == ((uint64_t*)t->mapkey)[0] &&
				((uint64_t*)t2->mapkey)[1] == ((uint64_t*)t->mapkey)[1])
		{
			if (memcmp(t2->keyword, t->keyword, t->keyword_len) == 0)
			{
				//node exit in hash table, delete it
				j->pf_free_data(j->data);
				MHS_DELETE(p, j);

				pthread_mutex_unlock(&p->mutex);
				return 1;
			}
		}

		j = j->next_colli;
	}

	pthread_mutex_unlock(&p->mutex);
	//no find
	return -1;
}

void* map_dec_hash_lookup(void* mhash,  void* data)
{
	assert(data != NULL || mhash != NULL);

	struct mhash* p = (struct mhash*)mhash;

	//node
	struct mvalue_dec* t = (struct mvalue_dec*)data;
	uint32_t key = t->hash2;

	//check
	uint32_t position = m_hash_hash(key, p->table_size);
	struct mhashj* j = p->hash[position];
	while (j != NULL)
	{
		//
		struct mvalue_dec* t2 = (struct mvalue_dec*)j->data;
		//
		assert(j->data != NULL);
		
		if (((uint64_t*)t2->mapkey)[0] == ((uint64_t*)t->mapkey)[0] &&
				((uint64_t*)t2->mapkey)[1] == ((uint64_t*)t->mapkey)[1])
		{
			if(t2->keyword_len == t->keyword_len)
			{
				if (memcmp(t2->keyword, t->keyword, t->keyword_len) == 0)
				{
					return j->data;
				}
			}
		}

		j = j->next_colli;
	}

	//no find
	return NULL;
}

int map_dec_hash_print(void* mhash, FILE* fp, void* callback)
{
	struct mhash* p = (struct mhash*)mhash;

	PF_MHASH_PRINT pf_mhash_print = (PF_MHASH_PRINT)callback;	

    int count = 0;
	struct mhashj* j = p->head;

	//item
	int i;
	for(i = 0; i < p->total_num; i++)
	{
		if (j == NULL)
		{
			//no exist
			break;
		}

		fprintf(fp, "item [%d]\t", i);

		pf_mhash_print(fp, j->data);
		
		fprintf(fp, "\n");

		count++;

		j = j->next;
	}

	//handle 
	fprintf(fp, "%-32s%d\n%-32s%d\n","total num:", p->total_num, "current num:", count);

	return 1;
}

int map_dec_hash_list(void* mhash, void* callback)
{
	struct mhash* p = (struct mhash*)mhash;

	//PF_MAP_TIMEOUT map_check_timeout = (PF_MAP_TIMEOUT)callback;
	
    int count = 0;
	struct mhashj* j = p->head;

	//item
	int i;
	for(i = 0; i < p->total_num; i++)
	{
		if (j == NULL)
		{
			//no exist
			break;
		}

		if (((PF_MAP_TIMEOUT)callback)(j->data))
		{
			struct mvalue_dec* t = (struct mvalue_dec*)j->data;
			
			//
			assert(j->data != NULL);
			
			//timeout
			j->pf_free_data(j->data);
			
			//
			MHS_DELETE(p, j);

			return 1;
		}

		count++;
		j = j->next;
	}

	return 1;
}

uint64_t map_dec_hash_size(void* mhash)
{
	return MHS_USED_MEMORY((struct mhash*)mhash);
}
