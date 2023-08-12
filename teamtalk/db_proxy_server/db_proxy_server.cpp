/*
 * db_proxy_server.cpp
 *
 *  Created on: 2014年7月21日
 *      Author: ziteng
 */

#include "base/netlib.h"
#include "base/ConfigFileReader.h"
#include "base/version.h"
#include "base/ThreadPool.h"
#include "DBPool.h"
#include "CachePool.h"
#include "ProxyConn.h"
#include "base/HttpClient.h"
#include "base/EncDec.h"
#include "business/AudioModel.h"
#include "business/MessageModel.h"
#include "business/SessionModel.h"
#include "business/RelationModel.h"
#include "business/UserModel.h"
#include "business/GroupModel.h"
#include "business/GroupMessageModel.h"
#include "business/FileModel.h"
#include "SyncCenter.h"
#include "base/slog.h"
#include "yaml-cpp/yaml.h"

string strAudioEnc;
// this callback will be replaced by imconn_callback() in OnConnect()
void proxy_serv_callback(void* callback_data, uint8_t msg, uint32_t handle, void* pParam)
{
	if (msg == NETLIB_MSG_CONNECT)
	{
		CProxyConn* pConn = new CProxyConn();
		pConn->OnConnect(handle);
	}
	else
	{
		SPDLOG_ERROR("!!!error msg: {}", msg);
	}
}

int main(int argc, char* argv[])
{
	if ((argc == 2) && (strcmp(argv[1], "-v") == 0)) {
		printf("Server Version: DBProxyServer/%s\n", VERSION);
		printf("Server Build: %s %s\n", __DATE__, __TIME__);
		return 0;
	}

	signal(SIGPIPE, SIG_IGN);
	srand(time(NULL));
	
	if (CacheManager::getInstance()->Init() != 0 ){
		return -1;
	}
	if (CDBManager::getInstance()->Init() != 0 ){
		return -1;
	}
	
	 
	puts("db init success");
	 
	YAML::Node config_file = YAML::LoadFile("dbproxyserver.yaml");
	std::string listenIp = config_file["ListenIP"].as<std::string>();
	uint16_t listenPort = config_file["ListenPort"].as<uint16_t>();
	uint32_t thread_num = config_file["ThreadNum"].as<uint32_t>();
	string strFileSite = config_file["MsfsSite"].as<std::string>();
	string str_aes_key = config_file["aesKey"].as<std::string>();

	if (listenIp.empty() || listenPort == 0 || thread_num == 0 || strFileSite.empty() || str_aes_key.empty()) {
		SPDLOG_ERROR("missing ListenIP/ListenPort/ThreadNum/MsfsSite/aesKey, exit...");
		return -1;
	}
    
    if(str_aes_key.length() != 32)
    {
        SPDLOG_ERROR("aes key is invalied");
        return -2;
    }
    CAes cAes = CAes(str_aes_key);
    string strAudio = "[语音]";
    char* pAudioEnc;
    uint32_t nOutLen;
    if(cAes.Encrypt(strAudio.c_str(), strAudio.length(), &pAudioEnc, nOutLen) == 0)
    {
        strAudioEnc.clear();
        strAudioEnc.append(pAudioEnc, nOutLen);
        cAes.Free(pAudioEnc);
    }
 

    CAudioModel::getInstance()->setUrl(strFileSite);

	int ret = netlib_init();

	if (ret == NETLIB_ERROR)
		return ret;
    
    /// yunfan add 2014.9.28
    // for 603 push
    curl_global_init(CURL_GLOBAL_ALL);
    /// yunfan add end

	init_proxy_conn(thread_num);
    CSyncCenter::getInstance()->init();
    CSyncCenter::getInstance()->startSync();

	CStrExplode listen_ip_list((char *)listenIp.c_str(), ';');
 	for (uint32_t i = 0; i < listen_ip_list.GetItemCnt(); i++) {
		ret = netlib_listen(listen_ip_list.GetItem(i), listenPort, proxy_serv_callback, NULL);
		if (ret == NETLIB_ERROR)
			return ret;
	}

	printf("server start listen on: %s:%d \n", listenIp.c_str(),  listenPort);
	printf("now enter the event loop...\n");
    writePid();
	netlib_eventloop(10);

	return 0;
}


