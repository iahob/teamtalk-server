/*
 * file_server.cpp
 *
 *  Created on: 2013-12-9
 *      Author: ziteng@mogujie.com
 */

// #include "FileConn.h"

#include "base/netlib.h"
#include "base/ConfigFileReader.h"
#include "base/version.h"
#include "IM.BaseDefine.pb.h"

#include "config_util.h"
#include "file_client_conn.h"
#include "file_msg_server_conn.h"
#include "base/slog.h"
#include "yaml-cpp/yaml.h"

int main(int argc, char* argv[])
{
#if 0
    pid_t pid = fork();
    if (pid < 0) {
        exit(-1);
    } else if (pid > 0) {
        exit(0);
    }
    setsid();
#endif
	if ((argc == 2) && (strcmp(argv[1], "-v") == 0)) {
		printf("Server Version: FileServer/%s\n", VERSION);
		printf("Server Build: %s %s\n", __DATE__, __TIME__);
		return 0;
	}

	signal(SIGPIPE, SIG_IGN);

    YAML::Node root = YAML::LoadFile("fileserver.yaml");
    std::string clientListenIp = root["ClientListenIP"].as<std::string>();
    uint32_t clientPort = root["ClientListenPort"].as<uint32_t>();
    std::string serverListenIp = root["MsgServerListenIP"].as<std::string>();
    uint32_t serverPort = root["MsgServerListenPort"].as<uint32_t>();
    uint32_t taskTimeout = root["TaskTimeout"].as<uint32_t>();
   
	if (clientListenIp.empty() || clientPort!=0 || serverListenIp.empty() || serverPort!=0 ) {
		SPDLOG_ERROR("config item missing, exit... ");
		return -1;
	}
    ConfigUtil::Instance()->AddAddress(clientListenIp.c_str(), clientPort);
    ConfigUtil::Instance()->SetTaskTimeout(taskTimeout);
    
    InitializeFileMsgServerConn();
	InitializeFileClientConn();

	int ret = netlib_init();

	if (ret == NETLIB_ERROR)
		return ret;

    ret = netlib_listen(clientListenIp.c_str(), clientPort, FileClientConnCallback, NULL);
    if (ret == NETLIB_ERROR) {
        SPDLOG_DEBUG("listen {}:{} error!!", clientListenIp.c_str(), clientPort);
        return ret;
    } else {
        SPDLOG_DEBUG("server start listen on {}:{}", clientListenIp.c_str().GetItem(i), clientPort);
    }   
	 

    ret = netlib_listen(serverListenIp.c_str(), serverPort, FileMsgServerConnCallback, NULL);
    if (ret == NETLIB_ERROR) {
        SPDLOG_DEBUG("listen {}:{} error!!\n", serverListenIp.c_str(), serverPort);
        return ret;
    } else {
        SPDLOG_DEBUG("server start listen on %s:%d\n", serverListenIp.c_str(), serverPort);
    }
	SPDLOG_DEBUG("now enter the event loop...\n");
    writePid();
	netlib_eventloop();
	SPDLOG_DEBUG("exiting.......\n");
	return 0;
}

