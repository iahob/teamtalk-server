//
//  apns_client_handler.cpp
//  my_push_server
//
//  Created by luoning on 14-11-11.
//  Copyright (c) 2014年 luoning. All rights reserved.
//

#include "apns_client_handler.h"
#include "push_define.h"
#include "session_manager.h"

void CAPNSClientHandler::OnClose(uint32_t nsockid)
{
    SPDLOG_WARN("apns gateway client closed, sockid: {}", nsockid);
    m_Msg.Clear();
    apns_client_ptr pClient = CSessionManager::GetInstance()->GetAPNSClient();
    if (pClient)
    {
        pClient->StartReConnectGateway();
        pClient->StartReConnectFeedback();
    }
}

void CAPNSClientHandler::OnException(uint32_t nsockid, int32_t nErrorCode)
{
    SPDLOG_WARN("apns gateway client has exception, sockid: {}, error code: {}.", nsockid, nErrorCode);
    apns_client_ptr pClient = CSessionManager::GetInstance()->GetAPNSClient();
    if (pClient)
    {
        pClient->StopGateWayClient();
    }
}

void CAPNSClientHandler::OnConnect(uint32_t nsockid)
{
}

void CAPNSClientHandler::OnSSLConnect(uint32_t nsockid)
{
    SPDLOG_INFO("apns gateway ssl connect successed.");
}

void CAPNSClientHandler::OnRecvData(const char* szBuf, int32_t nBufSize)
{
    m_Msg.Append(szBuf, nBufSize);
    while (m_Msg.CheckMsgAvailable())
    {
        CAPNSGateWayResMsg msg;
        if (msg.ParseFromArray(m_Msg.Data(), m_Msg.GetResMsgLength()))
        {
            SPDLOG_INFO("apns gateway client recv resp, cmd id: {}, status: {}, notification id: {}", (uint32_t)msg.GetCommandID(), (uint32_t)msg.GetStatus(), msg.GetNotificationID());
            //apns_client_ptr pClient = CSessionManager::GetInstance()->GetAPNSClient();
            //if (pClient)
            //{
            //    pClient->StopGateWayClient();
            //}
        }
        else
        {
            SPDLOG_ERROR("CAPNSGateWayResMsg, msg parse failed.");
            apns_client_ptr pClient = CSessionManager::GetInstance()->GetAPNSClient();
            if (pClient)
            {
                pClient->StopGateWayClient();
            }
        }
        m_Msg.Remove(m_Msg.GetResMsgLength());
    }
}