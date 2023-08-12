//
//  push_server_handler.cpp
//  my_push_server
//
//  Created by luoning on 14-11-11.
//  Copyright (c) 2014年 luoning. All rights reserved.
//

#include "push_server_handler.h"
#include "push_define.h"
#include "push_session.h"
#include "push_app.h"
#include "session_manager.h"

void CPushServerHandler::OnClose(uint32_t nsockid)
{
    SPDLOG_INFO("push server closed, sockid: {}.", nsockid);
}

void CPushServerHandler::OnAccept(uint32_t nsockid, S_SOCKET sock, const char *szIP,int32_t nPort)
{
    push_server_ptr pServer = CSessionManager::GetInstance()->GetPushServer();
    push_session_ptr pSession(new CPushSession(pServer->GetIOLoop(), sock));
    CSessionManager::GetInstance()->AddPushSessionBySockID(pSession->GetSocketID(), pSession);
    SPDLOG_INFO("push server accept session, remote ip: {}, port: {}, sockid: {}, real socket: {}.", szIP, nPort, pSession->GetSocketID(), sock);
    pSession->Start();
}