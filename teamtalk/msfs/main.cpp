/*================================================================
 *   Copyright (C) 2014 All rights reserved.
 *
 *   @       File              :  main.cpp
 *   @       Author      :  Zhang Yuanhao
 *   @       Email         :  bluefoxah@gmail.com
 *   @       Date           :  2014年7月29日
 *   @       Version     :   1.0
 *   @  Description	:
 *
 ================================================================*/

#include <iostream>
#include <signal.h>
#include "base/netlib.h"
#include "base/ConfigFileReader.h"
#include "HttpConn.h"
#include "FileManager.h"
#include "base/ThreadPool.h"
#include "base/slog.h"
#include "yaml-cpp/yaml.h"
using namespace std;
using namespace msfs;


FileManager* FileManager::m_instance = NULL;
FileManager* g_fileManager = NULL;
CThreadPool g_PostThreadPool;
CThreadPool g_GetThreadPool;

void closeall(int fd)
{
    int fdlimit = sysconf(_SC_OPEN_MAX);

    while (fd < fdlimit)
        close(fd++);
}

int daemon(int nochdir, int noclose, int asroot)
{
    switch (fork())
    {
        case 0:  break;
        case -1: return -1;
        default: _exit(0);          /* exit the original process */
    }

    if (setsid() < 0)               /* shoudn't fail */
        return -1;

    if ( !asroot && (setuid(1) < 0) )              /* shoudn't fail */
        return -1;

    /* dyke out this switch if you want to acquire a control tty in */
    /* the future -- not normally advisable for daemons */

    switch (fork())
    {
        case 0:  break;
        case -1: return -1;
        default: _exit(0);
    }

    if (!nochdir)
        chdir("/");

    if (!noclose)
    {
        closeall(0);
        dup(0); dup(0);
    }

    return 0;
}

// for client connect in
void http_callback(void* callback_data, uint8_t msg, uint32_t handle,
        void* pParam)
{
    if (msg == NETLIB_MSG_CONNECT)
    {
        CHttpConn* pConn = new CHttpConn();
//        CHttpTask* pTask = new CHttpTask(handle, pConn);
//        g_ThreadPool.AddTask(pTask);
        pConn->OnConnect(handle);
    } else
    {
        SPDLOG_ERROR("!!!error msg: {}", msg);
    }
}

void doQuitJob() {
    YAML::Node root = YAML::LoadFile("msfs.yaml");
    root["FileCnt"] = g_fileManager->getFileCntCurr();
    FileManager::destroyInstance();
    netlib_destroy();
    SPDLOG_ERROR("I'm ready quit...");
}
void Stop(int signo) {
    SPDLOG_ERROR("receive signal:{}", signo);
    switch (signo) {
        case SIGINT:
        case SIGTERM:
        case SIGQUIT:
            doQuitJob();
            _exit(0);
            break;
        default:
            SPDLOG_ERROR("unknown signal");
            _exit(0);
    }
}
int main(int argc, char* argv[])
{
    for(int i=0; i < argc; ++i)
       {
           if(strncmp(argv[i], "-d", 2) == 0)
           {
               if(daemon(1, 0, 1) < 0)
               {
                    SPDLOG_ERROR("daemon error");
                   return -1;
               }
               break;
           }
       }
    SPDLOG_ERROR("MsgServer max files can open: {}", getdtablesize());
    // CConfigFileReader config_file("msfs.conf");

    YAML::Node root = YAML::LoadFile("msfs.yaml");
    std::string listenIP = root["ListenIP"].as<std::string >();
    uint16_t listenPort = root["ListenPort"].as<uint16_t>();
    std::string baseDir = root["BaseDir"].as<std::string>();
    uint32_t fileCnt = root["FileCnt"].as<uint32_t>();
    uint32_t filesPerDir = root["FilesPerDir"].as<uint32_t>();
    uint32_t postThreadCnt = root["PostThreadCount"].as<uint32_t>();
    uint32_t getThreadCnt = root["GetThreadCount"].as<uint32_t>();

    if (listenIP.empty() || listenPort == 0 || baseDir.empty() || 
    fileCnt == 0 || filesPerDir == 0 || postThreadCnt == 0 || getThreadCnt == 0)
    {
        SPDLOG_ERROR("config file miss, exit...");
        return -1;
    }
    
    SPDLOG_ERROR("{},{}",listenIP, listenPort);
    
    g_PostThreadPool.Init(postThreadCnt);
    g_GetThreadPool.Init(getThreadCnt);

    g_fileManager = FileManager::getInstance(listenIP.c_str(), baseDir.c_str(), fileCnt, filesPerDir);
	int ret = g_fileManager->initDir();
	if (ret) {
        SPDLOG_ERROR("The BaseDir is set incorrectly :{} ",baseDir);
		return ret;
    }
	ret = netlib_init();
    if (ret == NETLIB_ERROR)
        return ret;

    
    ret = netlib_listen(listenIP.c_str(), listenPort,http_callback, NULL);
    if (ret == NETLIB_ERROR)
        return ret;


    signal(SIGINT, Stop);
    signal (SIGTERM, Stop);
    signal (SIGQUIT, Stop);
    signal(SIGPIPE, SIG_IGN);
    signal (SIGHUP, SIG_IGN);

    SPDLOG_INFO("server start listen on: {}:{} ", listenIP, listenPort);
    init_http_conn();
    SPDLOG_INFO("now enter the event loop... ");
    writePid();

    netlib_eventloop();
    return 0;
}


