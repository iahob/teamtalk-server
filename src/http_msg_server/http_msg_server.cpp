//
//  http_msg_server.cpp
//  http_msg_server
//
//  Created by jianqing.du on 13-9-29.
//  Copyright (c) 2013年 ziteng. All rights reserved.
//

#include "base/netlib.h"
#include "base/ConfigFileReader.h"
#include "RouteServConn.h"
#include "DBServConn.h"
#include "base/version.h"
#include "base/ServInfo.h"
#include "HttpConn.h"
#include "HttpQuery.h"
#include "base/util.h"
#include "base/EncDec.h"
#include "base/slog.h"
#include "yaml-cpp/yaml.h"

CAes* pAes;

#define DEFAULT_CONCURRENT_DB_CONN_CNT  2

// for client connect in
void http_callback(void* callback_data, uint8_t msg, uint32_t handle, void* pParam)
{
	if (msg == NETLIB_MSG_CONNECT)
	{
		CHttpConn* pConn = new CHttpConn();
		pConn->OnConnect(handle);
	}
	else
	{
		SPDLOG_ERROR("!!!error msg: %d ", msg);
	}
}


int main(int argc, char* argv[])
{
	if ((argc == 2) && (strcmp(argv[1], "-v") == 0)) {
		printf("Server Version: HttpMsgServer/%s\n", VERSION);
		printf("Server Build: %s %s\n", __DATE__, __TIME__);
		return 0;
	}
    
	signal(SIGPIPE, SIG_IGN);
	srand(time(NULL));
    
	SPDLOG_ERROR("MsgServer max files can open: {} ", getdtablesize());
    
	YAML::Node root = YAML::LoadFile("httpmsgserver.yaml");
	std::string serverListenIp = root["ListenIP"].as<std::string>();
    uint32_t serverPort = root["ListenPort"].as<uint32_t>();
	std::string aesKey = root["aesKey"].as<std::string>();
	if (aesKey.length() != 32 ) {
		SPDLOG_ERROR("aes key is invalied");
		return 0;
	}
    pAes = new CAes(aesKey);

	std::vector<serv_info_t> dbNodes;
	std::vector<serv_info_t> rtNodes;	
	for(size_t i = 0; i < root["DBServer"].size(); ++i) {
		YAML::Node node = root["DBServer"][i];
 		if(!node.IsMap()) { //判断是否是map结构， IsMap
            continue;
        }
		serv_info_t p ;
		p.server_name = node["name"].as<std::string>();
		p.server_ip = node["host"].as<std::string>();
		p.server_port = node["port"].as<int>();
		dbNodes.push_back(p);
	}
	for(size_t i = 0; i < root["RouterServer"].size(); ++i) {
		YAML::Node node = root["RouterServer"][i];
 		if(!node.IsMap()) { //判断是否是map结构， IsMap
            continue;
        }
		serv_info_t p ;
		p.server_name = node["name"].as<std::string>();
		p.server_ip = node["host"].as<std::string>();
		p.server_port = node["port"].as<int>();
		rtNodes.push_back(p);
	}
	 
	if (serverListenIp.empty() || serverPort == 0 ) {
		SPDLOG_ERROR("config file miss, exit... ");
		return -1;
	}
    
      
	int ret = netlib_init();
    
	if (ret == NETLIB_ERROR)
		return ret;
    
	ret = netlib_listen(serverListenIp.c_str(), serverPort, http_callback, NULL);
	if (ret == NETLIB_ERROR)
		return ret;

	printf("server start listen on: %s:%d\n", serverListenIp.c_str(), serverPort);
    
	init_http_conn();
    
	if (dbNodes.size() > 0) {
		HTTP::init_db_serv_conn(dbNodes);
	}

	if (rtNodes.size() > 0) {
		HTTP::init_route_serv_conn(rtNodes);
	}

	printf("now enter the event loop...\n");
    
    writePid();

	netlib_eventloop();
    
	return 0;
}
