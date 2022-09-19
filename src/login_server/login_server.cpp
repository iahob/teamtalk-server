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

	CConfigFileReader config_file("loginserver.conf");

    char* client_listen_ip = config_file.GetConfigName("ClientListenIP");
    char* str_client_port = config_file.GetConfigName("ClientPort");
    char* http_listen_ip = config_file.GetConfigName("HttpListenIP");
    char* str_http_port = config_file.GetConfigName("HttpPort");
	char* msg_server_listen_ip = config_file.GetConfigName("MsgServerListenIP");
	char* str_msg_server_port = config_file.GetConfigName("MsgServerPort");
    char* str_msfs_url = config_file.GetConfigName("msfs");
    char* str_discovery = config_file.GetConfigName("discovery");

	if (!msg_server_listen_ip || !str_msg_server_port || !http_listen_ip
        || !str_http_port || !str_msfs_url || !str_discovery) {
		SPDLOG_ERROR("config item missing, exit... ");
		return -1;
	}

	uint16_t client_port = atoi(str_client_port);
	uint16_t msg_server_port = atoi(str_msg_server_port);
    uint16_t http_port = atoi(str_http_port);
    strMsfsUrl = str_msfs_url;
    strDiscovery = str_discovery;
    
    
    pIpParser = new IpParser();
    
	int ret = netlib_init();

	if (ret == NETLIB_ERROR)
		return ret;
	CStrExplode client_listen_ip_list(client_listen_ip, ';');
	for (uint32_t i = 0; i < client_listen_ip_list.GetItemCnt(); i++) {
		ret = netlib_listen(client_listen_ip_list.GetItem(i), client_port, client_callback, NULL);
		if (ret == NETLIB_ERROR)
			return ret;
	}

	CStrExplode msg_server_listen_ip_list(msg_server_listen_ip, ';');
	for (uint32_t i = 0; i < msg_server_listen_ip_list.GetItemCnt(); i++) {
		ret = netlib_listen(msg_server_listen_ip_list.GetItem(i), msg_server_port, msg_serv_callback, NULL);
		if (ret == NETLIB_ERROR)
			return ret;
	}
    
    CStrExplode http_listen_ip_list(http_listen_ip, ';');
    for (uint32_t i = 0; i < http_listen_ip_list.GetItemCnt(); i++) {
        ret = netlib_listen(http_listen_ip_list.GetItem(i), http_port, http_callback, NULL);
        if (ret == NETLIB_ERROR)
            return ret;
    }
    

    SPDLOG_INFO("server start listen on:\nFor client {}:{}\nFor MsgServer: {}:{}\nFor http:{}:{}\n",
           client_listen_ip, client_port, msg_server_listen_ip, msg_server_port, http_listen_ip, http_port);
	init_login_conn();
    init_http_conn();

	printf("now enter the event loop...\n");
    
    writePid();

	netlib_eventloop();

	return 0;
}
