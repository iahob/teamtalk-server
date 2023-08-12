/*
 * login_server.cpp
 *
 *  Created on: 2013-6-21
 *      Author: ziteng@mogujie.com
 */

#include "loginConn.h"
#include "base/netlib.h"
#include "base/ConfigFileReader.h"
#include "base/version.h"
#include "HttpConn.h"
#include "ipparser.h"
#include "base/slog.h"
#include "yaml-cpp/yaml.h"

IpParser* pIpParser = NULL;
string strMsfsUrl;
string strDiscovery;//发现获取地址
void client_callback(void* callback_data, uint8_t msg, uint32_t handle, void* pParam)
{
	if (msg == NETLIB_MSG_CONNECT)
	{
		CloginConn* pConn = new CloginConn();
		pConn->OnConnect2(handle, logIN_CONN_TYPE_CLIENT);
	}
	else
	{
		SPDLOG_ERROR("!!!error msg: {} ", msg);
	}
}

// this callback will be replaced by imconn_callback() in OnConnect()
void msg_serv_callback(void* callback_data, uint8_t msg, uint32_t handle, void* pParam)
{
    SPDLOG_ERROR("msg_server come in");

	if (msg == NETLIB_MSG_CONNECT)
	{
		CloginConn* pConn = new CloginConn();
		pConn->OnConnect2(handle, logIN_CONN_TYPE_MSG_SERV);
	}
	else
	{
		SPDLOG_ERROR("!!!error msg: {} ", msg);
	}
}


void http_callback(void* callback_data, uint8_t msg, uint32_t handle, void* pParam)
{
    if (msg == NETLIB_MSG_CONNECT)
    {
        CHttpConn* pConn = new CHttpConn();
        pConn->OnConnect(handle);
    }
    else
    {
        SPDLOG_ERROR("!!!error msg: {} ", msg);
    }
}

int main(int argc, char* argv[])
{
	if ((argc == 2) && (strcmp(argv[1], "-v") == 0)) {
		printf("Server Version: loginServer/{}\n", VERSION);
		printf("Server Build: {} {}\n", __DATE__, __TIME__);
		return 0;
	}

	signal(SIGPIPE, SIG_IGN);

	YAML::Node root = YAML::LoadFile("loginserver.yaml");
 	std::string clientIP = root["ClientListenIP"].as<std::string>();
	uint16_t clientPort = root["ClientPort"].as<uint16_t>();
 	std::string httpIP = root["HttpListenIP"].as<std::string>();
	uint16_t httpPort = root["HttpPort"].as<uint16_t>();
	std::string msgIP = root["MsgServerListenIP"].as<std::string>();
	uint16_t msgPort = root["MsgServerPort"].as<uint16_t>();
	strMsfsUrl = root["msfs"].as<std::string>();
	strDiscovery = root["discovery"].as<std::string>();
 
    pIpParser = new IpParser();
    
	int ret = netlib_init();

	if (ret == NETLIB_ERROR)
		return ret;
	ret = netlib_listen(clientIP.c_str(), clientPort, client_callback, NULL);
	if (ret == NETLIB_ERROR)
		return ret;
	 
	ret = netlib_listen(httpIP.c_str(), httpPort, msg_serv_callback, NULL);
	if (ret == NETLIB_ERROR)
		return ret;
	 
    ret = netlib_listen(msgIP.c_str(), msgPort, http_callback, NULL);
    if (ret == NETLIB_ERROR)
        return ret;
    
    SPDLOG_INFO("server start listen on:\nFor client {}:{}\nFor MsgServer: {}:{}\nFor http:{}:{}\n",
           clientIP, clientPort, msgIP, msgPort, httpIP, httpPort);
	
	init_login_conn();
    init_http_conn();

	printf("now enter the event loop...\n");
    
    writePid();

	netlib_eventloop();

	return 0;
}
