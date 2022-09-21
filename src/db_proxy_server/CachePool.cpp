/*
 * CachePool.cpp
 *
 *  Created on: 2014年7月24日
 *      Author: ziteng
 *  Modify By ZhangYuanhao
 *  2015-01-13
 *  Add some redis command
 * Modify By ZhangYuanhao
 * 2015-01-14
 * Add comment for function
 *
 */

#include "CachePool.h"
#include "yaml-cpp/yaml.h"
#include "base/slog.h"
#include "algorithm"

#define MIN_CACHE_CONN_CNT	2

 
CacheConn::CacheConn(CachePool* pCachePool)
{
	m_pCachePool = pCachePool;
	m_pContext = NULL;
	m_last_connect_time = 0;
}

CacheConn::~CacheConn()
{
	if (m_pContext) {
		redisFree(m_pContext);
		m_pContext = NULL;
	}
}

/*
 * redis初始化连接和重连操作，类似mysql_ping()
 */
int CacheConn::Init()
{
	if (m_pContext) {
		return 0;
	}

	// 4s 尝试重连一次
	uint64_t cur_time = (uint64_t)time(NULL);
	if (cur_time < m_last_connect_time + 4) {
		return 1;
	}

	m_last_connect_time = cur_time;

	// 200ms超时
	struct timeval timeout = {0, 200000};
	m_pContext = redisConnectWithTimeout(m_pCachePool->GetServerIP(), m_pCachePool->GetServerPort(), timeout);
	if (!m_pContext || m_pContext->err) {
		if (m_pContext) {
            SPDLOG_ERROR("redisConnect failed: {}", m_pContext->errstr);
			redisFree(m_pContext);
			m_pContext = NULL;
		} else {
            SPDLOG_ERROR("redisConnect failed");
		}

		return 1;
	}

	redisReply* reply = (redisReply *)redisCommand(m_pContext, "SELECT %d", m_pCachePool->GetDBNum());
	if (reply && (reply->type == REDIS_REPLY_STATUS) && (strncmp(reply->str, "OK", 2) == 0)) {
		freeReplyObject(reply);
		return 0;
	} else {
		SPDLOG_ERROR("select cache db failed");
		return 2;
	}
}


const char* CacheConn::GetPoolName()
{
	return m_pCachePool->GetPoolName();
}


string CacheConn::get(string key)
{
	string value;

	if (Init()) {
		return value;
	}

	redisReply* reply = (redisReply *)redisCommand(m_pContext, "GET %s", key.c_str());
	if (!reply) {
        SPDLOG_ERROR("redisCommand failed:{}", m_pContext->errstr);
		redisFree(m_pContext);
		m_pContext = NULL;
		return value;
	}

	if (reply->type == REDIS_REPLY_STRING) {
		value.append(reply->str, reply->len);
	}

	freeReplyObject(reply);
	return value;
}

string CacheConn::setex(string key, int timeout, string value)
{
	string ret_value;

	if (Init()) {
		return ret_value;
	}

	redisReply* reply = (redisReply *)redisCommand(m_pContext, "SETEX %s %d %s", key.c_str(), timeout, value.c_str());
	if (!reply) {
        SPDLOG_ERROR("redisCommand failed:{}", m_pContext->errstr);
		redisFree(m_pContext);
		m_pContext = NULL;
		return ret_value;
	}

	ret_value.append(reply->str, reply->len);
	freeReplyObject(reply);
	return ret_value;
}

string CacheConn::set(string key, string &value)
{
    string ret_value;
    
    if (Init()) {
        return ret_value;
    }
    
    redisReply* reply = (redisReply *)redisCommand(m_pContext, "SET %s %s", key.c_str(), value.c_str());
    if (!reply) {
        SPDLOG_ERROR("redisCommand failed:{}", m_pContext->errstr);
        redisFree(m_pContext);
        m_pContext = NULL;
        return ret_value;
    }
    
    ret_value.append(reply->str, reply->len);
    freeReplyObject(reply);
    return ret_value;
}

bool CacheConn::mget(const vector<string>& keys, map<string, string>& ret_value)
{
    if(Init())
    {
        return false;
    }
    if(keys.empty())
    {
        return false;
    }
    
    string strKey;
    bool bFirst = true;
    for (vector<string>::const_iterator it=keys.begin(); it!=keys.end(); ++it) {
        if(bFirst)
        {
            bFirst = false;
            strKey = *it;
        }
        else
        {
            strKey += " " + *it;
        }
    }
    
    if(strKey.empty())
    {
        return false;
    }
    strKey = "MGET " + strKey;
    redisReply* reply = (redisReply*) redisCommand(m_pContext, strKey.c_str());
    if (!reply) {
        SPDLOG_ERROR("redisCommand failed:{}", m_pContext->errstr);
        redisFree(m_pContext);
        m_pContext = NULL;
        return false;
    }
    if(reply->type == REDIS_REPLY_ARRAY)
    {
        for(size_t i=0; i<reply->elements; ++i)
        {
            redisReply* child_reply = reply->element[i];
            if (child_reply->type == REDIS_REPLY_STRING) {
                ret_value[keys[i]] = child_reply->str;
            }
        }
    }
    freeReplyObject(reply);
    return true;
}

bool CacheConn::isExists(string &key)
{
    if (Init()) {
        return false;
    }
    
    redisReply* reply = (redisReply*) redisCommand(m_pContext, "EXISTS %s", key.c_str());
    if(!reply)
    {
        SPDLOG_ERROR("redisCommand failed:{}", m_pContext->errstr);
        redisFree(m_pContext);
        return false;
    }
    long ret_value = reply->integer;
    freeReplyObject(reply);
    if(0 == ret_value)
    {
        return false;
    }
    else
    {
        return true;
    }
}
long CacheConn::hdel(string key, string field)
{
	if (Init()) {
		return 0;
	}

	redisReply* reply = (redisReply *)redisCommand(m_pContext, "HDEL %s %s", key.c_str(), field.c_str());
	if (!reply) {
		SPDLOG_ERROR("redisCommand failed:{}", m_pContext->errstr);
		redisFree(m_pContext);
		m_pContext = NULL;
		return 0;
	}

	long ret_value = reply->integer;
	freeReplyObject(reply);
	return ret_value;
}

string CacheConn::hget(string key, string field)
{
	string ret_value;
	if (Init()) {
		return ret_value;
	}

	redisReply* reply = (redisReply *)redisCommand(m_pContext, "HGET %s %s", key.c_str(), field.c_str());
	if (!reply) {
		SPDLOG_ERROR("redisCommand failed:{}", m_pContext->errstr);
		redisFree(m_pContext);
		m_pContext = NULL;
		return ret_value;
	}

	if (reply->type == REDIS_REPLY_STRING) {
		ret_value.append(reply->str, reply->len);
	}

	freeReplyObject(reply);
	return ret_value;
}

bool CacheConn::hgetAll(string key, map<string, string>& ret_value)
{
	if (Init()) {
		return false;
	}

	redisReply* reply = (redisReply *)redisCommand(m_pContext, "HGETALL %s", key.c_str());
	if (!reply) {
		SPDLOG_ERROR("redisCommand failed:{}", m_pContext->errstr);
		redisFree(m_pContext);
		m_pContext = NULL;
		return false;
	}

	if ( (reply->type == REDIS_REPLY_ARRAY) && (reply->elements % 2 == 0) ) {
		for (size_t i = 0; i < reply->elements; i += 2) {
			redisReply* field_reply = reply->element[i];
			redisReply* value_reply = reply->element[i + 1];

			string field(field_reply->str, field_reply->len);
			string value(value_reply->str, value_reply->len);
			ret_value.insert(make_pair(field, value));
		}
	}

	freeReplyObject(reply);
	return true;
}

long CacheConn::hset(string key, string field, string value)
{
	if (Init()) {
		return -1;
	}

	redisReply* reply = (redisReply *)redisCommand(m_pContext, "HSET %s %s %s", key.c_str(), field.c_str(), value.c_str());
	if (!reply) {
		SPDLOG_ERROR("redisCommand failed:{}", m_pContext->errstr);
		redisFree(m_pContext);
		m_pContext = NULL;
		return -1;
	}

	long ret_value = reply->integer;
	freeReplyObject(reply);
	return ret_value;
}

long CacheConn::hincrBy(string key, string field, long value)
{
	if (Init()) {
		return -1;
	}

	redisReply* reply = (redisReply *)redisCommand(m_pContext, "HINCRBY %s %s %ld", key.c_str(), field.c_str(), value);
	if (!reply) {
		SPDLOG_ERROR("redisCommand failed:{}", m_pContext->errstr);
		redisFree(m_pContext);
		m_pContext = NULL;
		return -1;
	}

	long ret_value = reply->integer;
	freeReplyObject(reply);
	return ret_value;
}

long CacheConn::incrBy(string key, long value)
{
    if(Init())
    {
        return -1;
    }
    
    redisReply* reply = (redisReply*)redisCommand(m_pContext, "INCRBY %s %ld", key.c_str(), value);
    if(!reply)
    {
        SPDLOG_ERROR("redis Command failed:{}", m_pContext->errstr);
        redisFree(m_pContext);
        m_pContext = NULL;
        return -1;
    }
    long ret_value = reply->integer;
    freeReplyObject(reply);
    return ret_value;
}

string CacheConn::hmset(string key, map<string, string>& hash)
{
	string ret_value;

	if (Init()) {
		return ret_value;
	}

	int argc = hash.size() * 2 + 2;
	const char** argv = new const char* [argc];
	if (!argv) {
		return ret_value;
	}

	argv[0] = "HMSET";
	argv[1] = key.c_str();
	int i = 2;
	for (map<string, string>::iterator it = hash.begin(); it != hash.end(); it++) {
		argv[i++] = it->first.c_str();
		argv[i++] = it->second.c_str();
	}

	redisReply* reply = (redisReply *)redisCommandArgv(m_pContext, argc, argv, NULL);
	if (!reply) {
		SPDLOG_ERROR("redisCommand failed:{}", m_pContext->errstr);
		delete [] argv;

		redisFree(m_pContext);
		m_pContext = NULL;
		return ret_value;
	}

	ret_value.append(reply->str, reply->len);

	delete [] argv;
	freeReplyObject(reply);
	return ret_value;
}

bool CacheConn::hmget(string key, list<string>& fields, list<string>& ret_value)
{
	if (Init()) {
		return false;
	}

	int argc = fields.size() + 2;
	const char** argv = new const char* [argc];
	if (!argv) {
		return false;
	}

	argv[0] = "HMGET";
	argv[1] = key.c_str();
	int i = 2;
	for (list<string>::iterator it = fields.begin(); it != fields.end(); it++) {
		argv[i++] = it->c_str();
	}

	redisReply* reply = (redisReply *)redisCommandArgv(m_pContext, argc, (const char**)argv, NULL);
	if (!reply) {
		SPDLOG_ERROR("redisCommand failed:{}", m_pContext->errstr);
		delete [] argv;

		redisFree(m_pContext);
		m_pContext = NULL;

		return false;
	}

	if (reply->type == REDIS_REPLY_ARRAY) {
		for (size_t i = 0; i < reply->elements; i++) {
			redisReply* value_reply = reply->element[i];
			string value(value_reply->str, value_reply->len);
			ret_value.push_back(value);
		}
	}

	delete [] argv;
	freeReplyObject(reply);
	return true;
}

long CacheConn::incr(string key)
{
    if(Init())
    {
        return -1;
    }
    
    redisReply* reply = (redisReply*)redisCommand(m_pContext, "INCR %s", key.c_str());
    if(!reply)
    {
        SPDLOG_ERROR("redis Command failed:{}", m_pContext->errstr);
        redisFree(m_pContext);
        m_pContext = NULL;
        return -1;
    }
    long ret_value = reply->integer;
    freeReplyObject(reply);
    return ret_value;
}

long CacheConn::decr(string key)
{
    if(Init())
    {
        return -1;
    }
    
    redisReply* reply = (redisReply*)redisCommand(m_pContext, "DECR %s", key.c_str());
    if(!reply)
    {
        SPDLOG_ERROR("redis Command failed:{}", m_pContext->errstr);
        redisFree(m_pContext);
        m_pContext = NULL;
        return -1;
    }
    long ret_value = reply->integer;
    freeReplyObject(reply);
    return ret_value;
}

long CacheConn::lpush(string key, string value)
{
	if (Init()) {
		return -1;
	}

	redisReply* reply = (redisReply *)redisCommand(m_pContext, "LPUSH %s %s", key.c_str(), value.c_str());
	if (!reply) {
		SPDLOG_ERROR("redisCommand failed:{}", m_pContext->errstr);
		redisFree(m_pContext);
		m_pContext = NULL;
		return -1;
	}

	long ret_value = reply->integer;
	freeReplyObject(reply);
	return ret_value;
}

long CacheConn::rpush(string key, string value)
{
	if (Init()) {
		return -1;
	}

	redisReply* reply = (redisReply *)redisCommand(m_pContext, "RPUSH %s %s", key.c_str(), value.c_str());
	if (!reply) {
		SPDLOG_ERROR("redisCommand failed:{}", m_pContext->errstr);
		redisFree(m_pContext);
		m_pContext = NULL;
		return -1;
	}

	long ret_value = reply->integer;
	freeReplyObject(reply);
	return ret_value;
}

long CacheConn::llen(string key)
{
	if (Init()) {
		return -1;
	}

	redisReply* reply = (redisReply *)redisCommand(m_pContext, "LLEN %s", key.c_str());
	if (!reply) {
		SPDLOG_ERROR("redisCommand failed:{}", m_pContext->errstr);
		redisFree(m_pContext);
		m_pContext = NULL;
		return -1;
	}

	long ret_value = reply->integer;
	freeReplyObject(reply);
	return ret_value;
}

bool CacheConn::lrange(string key, long start, long end, list<string>& ret_value)
{
	if (Init()) {
		return false;
	}

	redisReply* reply = (redisReply *)redisCommand(m_pContext, "LRANGE %s %d %d", key.c_str(), start, end);
	if (!reply) {
		SPDLOG_ERROR("redisCommand failed:{}", m_pContext->errstr);
		redisFree(m_pContext);
		m_pContext = NULL;
		return false;
	}

	if (reply->type == REDIS_REPLY_ARRAY) {
		for (size_t i = 0; i < reply->elements; i++) {
			redisReply* value_reply = reply->element[i];
			string value(value_reply->str, value_reply->len);
			ret_value.push_back(value);
		}
	}

	freeReplyObject(reply);
	return true;
}

///////////////
CachePool::CachePool(const char* pool_name, const char* server_ip, int server_port, int db_num, int max_conn_cnt)
{
	m_pool_name = pool_name;
	m_server_ip = server_ip;
	m_server_port = server_port;
	m_db_num = db_num;
	m_max_conn_cnt = max_conn_cnt;
	m_cur_conn_cnt = MIN_CACHE_CONN_CNT;
}

CachePool::~CachePool()
{
	m_free_notify.Lock();
	for (list<CacheConn*>::iterator it = m_free_list.begin(); it != m_free_list.end(); it++) {
		CacheConn* pConn = *it;
		delete pConn;
	}

	m_free_list.clear();
	m_cur_conn_cnt = 0;
	m_free_notify.Unlock();
}

int CachePool::Init()
{
	for (int i = 0; i < m_cur_conn_cnt; i++) {
		CacheConn* pConn = new CacheConn(this);
		if (pConn->Init()) {
			delete pConn;
			return 1;
		}

		m_free_list.push_back(pConn);
	}

	SPDLOG_ERROR("cache pool: {}, list size: {}", m_pool_name.c_str(), m_free_list.size());
	return 0;
}

CacheConn* CachePool::GetCacheConn()
{
	m_free_notify.Lock();

	while (m_free_list.empty()) {
		if (m_cur_conn_cnt >= m_max_conn_cnt) {
			m_free_notify.Wait();
		} else {
			CacheConn* pCacheConn = new CacheConn(this);
			int ret = pCacheConn->Init();
			if (ret) {
				SPDLOG_ERROR("Init CacheConn failed");
				delete pCacheConn;
				m_free_notify.Unlock();
				return NULL;
			} else {
				m_free_list.push_back(pCacheConn);
				m_cur_conn_cnt++;
				SPDLOG_ERROR("new cache connection: %s, conn_cnt: %d", m_pool_name.c_str(), m_cur_conn_cnt);
			}
		}
	}

	CacheConn* pConn = m_free_list.front();
	m_free_list.pop_front();

	m_free_notify.Unlock();

	return pConn;
}

void CachePool::RelCacheConn(CacheConn* pCacheConn)
{
	m_free_notify.Lock();

	list<CacheConn*>::iterator it = m_free_list.begin();
	for (; it != m_free_list.end(); it++) {
		if (*it == pCacheConn) {
			break;
		}
	}

	if (it == m_free_list.end()) {
		m_free_list.push_back(pCacheConn);
	}

	m_free_notify.Signal();
	m_free_notify.Unlock();
}
 

int CacheManager::Init()
{
  	YAML::Node root = YAML::LoadFile("dbproxyserver.yaml");
	struct NodePoint {
		std::string name;
    	std::string host;
    	int port;
		int db;
		int maxConn;
	};
	
	if(!root["CacheInstances"].IsDefined() || !root["CacheInstances"].IsSequence()) { //IsDefined() 判断节点是否存在
        return 1;
    }
	for(size_t i = 0; i < root["CacheInstances"].size(); ++i) {
		YAML::Node node = root["CacheInstances"][i];
 		if(!node.IsMap()) { //判断是否是map结构， IsMap
            continue;
        }
		NodePoint p ;
		p.name = node["name"].as<std::string>();
		p.host = node["host"].as<std::string>();
		p.port = node["port"].as<int>();
		p.db   = node["db"].as<int>();
		p.maxConn = node["maxConn"].as<int>();
		SPDLOG_ERROR("Init cache manager name: {}, host:{} port:{} maxConn:{} ", p.name.c_str(),p.host.c_str(), p.port, p.maxConn );

		CachePool* pCachePool = new CachePool(p.name.c_str(), p.host.c_str(), p.port,p.db, p.maxConn);
		if (pCachePool->Init()) {
			SPDLOG_ERROR("Init cache pool failed");
			return 1;
		}
		m_cache_pool_map.insert(make_pair(p.name, pCachePool));
	}

	 
	return 0;
}

CacheConn* CacheManager::GetCacheConn(const char* pool_name)
{
	map<string, CachePool*>::iterator it = m_cache_pool_map.find(pool_name);
	if (it != m_cache_pool_map.end()) {
		return it->second->GetCacheConn();
	} else {
		return NULL;
	}
}

void CacheManager::RelCacheConn(CacheConn* pCacheConn)
{
	if (!pCacheConn) {
		return;
	}

	map<string, CachePool*>::iterator it = m_cache_pool_map.find(pCacheConn->GetPoolName());
	if (it != m_cache_pool_map.end()) {
		return it->second->RelCacheConn(pCacheConn);
	}
}
