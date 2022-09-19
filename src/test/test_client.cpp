/*================================================================
 *   Copyright (C) 2014 All rights reserved.
 *
 *   文件名称：test_client.cpp
 *   创 建 者：Zhang Yuanhao
 *   邮    箱：bluefoxah@gmail.com
 *   创建日期：2014年12月30日
 *   描    述：
 *
 ================================================================*/

#include <vector>
#include <iostream>
#include <string>
#include "ClientConn.h"
#include "netlib.h"
#include "TokenValidator.h"
#include "Thread.h"
#include "IM.BaseDefine.pb.h"
#include "IM.Buddy.pb.h"
#include "playsound.h"
#include "Common.h"
#include "Client.h"
#include "EncDec.h"
using namespace std;

#define MAX_LINE_LEN	1024
string g_login_domain = "http://xiaominfc.com:8080";
string g_cmd_string[10];
int g_cmd_num;
CClient* g_pClient = NULL;
CAes* pAes;

void split_cmd(char* buf)
{
	int len = strlen(buf);
	string element;

	g_cmd_num = 0;
	for (int i = 0; i < len; i++) {
		if (buf[i] == ' ' || buf[i] == '\t') {
			if (!element.empty()) {
				g_cmd_string[g_cmd_num++] = element;
				element.clear();
			}
		} else {
			element += buf[i];
		}
	}

	// put the last one
	if (!element.empty()) {
		g_cmd_string[g_cmd_num++] = element;
	}
}

void print_help()
{
	printf("Usage:\n");
    printf("login user_name user_pass\n");
    /*
	printf("connect serv_ip serv_port user_name user_pass\n");
    printf("getuserinfo\n");
    printf("send toId msg\n");
    printf("unreadcnt\n");
     */
	printf("close\n");
	printf("quit\n");
}


void sendTextMsg(const string& id, const string& msg)
{
    if(g_pClient){
        g_pClient->sendMsg(atoi(id.c_str()),IM::BaseDefine::MSG_TYPE_SINGLE_TEXT,msg);
    }
}

void doLogin(const string& strName, const string& strPass)
{
    try
    {
        g_pClient = new CClient(strName, strPass, g_login_domain);
    }
    catch(...)
    {
        printf("get error while alloc memory\n");
        PROMPTION;
        return;
    }
    g_pClient->connect();
}

void getOfflineFile() {

    printf("get offlinefile\n");
    if(g_pClient) {
        g_pClient->getOfflineFile();
    }
}


void exec_cmd()
{
	if (g_cmd_num == 0) {
		return;
	}

    printf("cmd:%s\n",g_cmd_string[0].c_str());
    
    if(g_cmd_string[0] == "login")
    {
        if(g_cmd_num == 3)
        {
            doLogin(g_cmd_string[1], g_cmd_string[2]);
        }
        else
        {
            print_help();
        }
    }else if(strcmp(g_cmd_string[0].c_str(), "send") == 0){
        if(g_cmd_num == 3) {
            sendTextMsg(g_cmd_string[1], g_cmd_string[2]);
        }else {
            print_help();
        }
    }else if(strcmp(g_cmd_string[0].c_str(), "offlinefile") == 0) {
        getOfflineFile();
    }
    else if (strcmp(g_cmd_string[0].c_str(), "close") == 0) {
        g_pClient->close();
    }
    else if (strcmp(g_cmd_string[0].c_str(), "quit") == 0) {
		exit(0);

    }
    else if(strcmp(g_cmd_string[0].c_str(), "list") == 0)
    {
        printf("+---------------------+\n");
        printf("|        用户名        |\n");
        printf("+---------------------+\n");
        CMapNick2User_t mapUser = g_pClient->getNick2UserMap();
        auto it = mapUser.begin();
        for(;it!=mapUser.end();++it)
        {
            uint32_t nLen = 21 - it->first.length();
            printf("|");
            for(uint32_t i=0; i<nLen/2; ++i)
            {
                printf(" ");
            }
            printf("%s:%d", it->first.c_str(),((IM::BaseDefine::UserInfo*)it->second)->user_id());
            for(uint32_t i=0; i<nLen/2; ++i)
            {
                printf(" ");
            }
            printf("|\n");
            printf("+---------------------+\n");
        }
    }
    else {
		print_help();
	}
}


class CmdThread : public CThread
{
public:
	void OnThreadRun()
	{
		while (true)
		{
			fprintf(stderr, "%s", PROMPT);	// print to error will not buffer the printed message

			if (fgets(m_buf, MAX_LINE_LEN - 1, stdin) == NULL)
			{
                if(errno == 0)  {
                    exit(0);
                    break;
                }
				fprintf(stderr, "fgets failed: %d\n", errno);
				continue;
			}

			m_buf[strlen(m_buf) - 1] = '\0';	// remove newline character

			split_cmd(m_buf);

			exec_cmd();
		}
	}
private:
	char	m_buf[MAX_LINE_LEN];
};

CmdThread g_cmd_thread;

int main(int argc, char* argv[])
{
//    play("message.wav");
    pAes = new CAes("12345678901234567890123456789012");
    
    if(argc == 3){
        doLogin(argv[1],argv[2]);
    }
    
	g_cmd_thread.StartThread();
	signal(SIGPIPE, SIG_IGN);
	int ret = netlib_init();

	if (ret == NETLIB_ERROR)
		return ret;
    
	netlib_eventloop();

	return 0;
}
