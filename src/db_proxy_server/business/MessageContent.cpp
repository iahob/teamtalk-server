/*================================================================
 *   Copyright (C) 2014 All rights reserved.
 *
 *   文件名称：MessageContent.cpp
 *   创 建 者：Zhang Yuanhao
 *   邮    箱：bluefoxah@gmail.com
 *   创建日期：2014年12月15日
 *   描    述：
 *
 ================================================================*/

#include "../ProxyConn.h"
#include "../CachePool.h"
#include "../DBPool.h"
#include "MessageContent.h"
#include "MessageModel.h"
#include "GroupMessageModel.h"
#include "Common.h"
#include "GroupModel.h"
#include "base/ImPduBase.h"
#include "IM.Message.pb.h"
#include "SessionModel.h"
#include "RelationModel.h"
#include "base/slog.h"


namespace DB_PROXY {

    void getMessage(CImPdu* pPdu, uint32_t conn_uuid)
    {
        IM::Message::IMGetMsgListReq msg;
  if(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()))
        {
            uint32_t nUserId = msg.user_id();
            uint32_t nPeerId = msg.session_id();
            uint32_t nMsgId = msg.msg_id_begin();
            uint32_t nMsgCnt = msg.msg_cnt();
            IM::BaseDefine::SessionType nSessionType = msg.session_type();
            if(IM::BaseDefine::SessionType_IsValid(nSessionType))
            {
                CImPdu* pPduResp = new CImPdu;
                IM::Message::IMGetMsgListRsp msgResp;

                list<IM::BaseDefine::MsgInfo> lsMsg;

                if(nSessionType == IM::BaseDefine::SESSION_TYPE_SINGLE)//获取个人消息
                {
                    CMessageModel::getInstance()->getMessage(nUserId, nPeerId, nMsgId, nMsgCnt, lsMsg);
                }
                else if(nSessionType == IM::BaseDefine::SESSION_TYPE_GROUP)//获取群消息
                {
                    if(CGroupModel::getInstance()->isInGroup(nUserId, nPeerId))
                    {
                        CGroupMessageModel::getInstance()->getMessage(nUserId, nPeerId, nMsgId, nMsgCnt, lsMsg);
                    }
                }

                msgResp.set_user_id(nUserId);
                msgResp.set_session_id(nPeerId);
                msgResp.set_msg_id_begin(nMsgId);
                msgResp.set_session_type(nSessionType);
                for(auto it=lsMsg.begin(); it!=lsMsg.end();++it)
                {
                    IM::BaseDefine::MsgInfo* pMsg = msgResp.add_msg_list();
        //            *pMsg = *it;
                    pMsg->set_msg_id(it->msg_id());
                    pMsg->set_from_session_id(it->from_session_id());
                    pMsg->set_create_time(it->create_time());
                    pMsg->set_msg_type(it->msg_type());
                    pMsg->set_msg_data(it->msg_data());
//                    SPDLOG_ERROR("userId={}, peerId={}, msgId={}", nUserId, nPeerId, it->msg_id());
                }

                SPDLOG_ERROR("userId={}, peerId={}, msgId={}, msgCnt={}, count={}", nUserId, nPeerId, nMsgId, nMsgCnt, msgResp.msg_list_size());
                msgResp.set_attach_data(msg.attach_data());
                pPduResp->SetPBMsg(&msgResp);
                pPduResp->SetSeqNum(pPdu->GetSeqNum());
                pPduResp->SetServiceId(IM::BaseDefine::SID_MSG);
                pPduResp->SetCommandId(IM::BaseDefine::CID_MSG_LIST_RESPONSE);
                CProxyConn::AddResponsePdu(conn_uuid, pPduResp);
            }
            else
            {
                SPDLOG_ERROR("invalid sessionType. userId={}, peerId={}, msgId={}, msgCnt={}, sessionType={}",
                    nUserId, nPeerId, nMsgId, nMsgCnt, nSessionType);
            }
        }
        else
        {
            SPDLOG_ERROR("parse pb failed");
        }
    }

    void sendMessage(CImPdu* pPdu, uint32_t conn_uuid)
    {
        IM::Message::IMMsgData msg;
        if(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()))
        {
            uint32_t nFromId = msg.from_user_id();
            uint32_t nToId = msg.to_session_id();
            uint32_t nCreateTime = msg.create_time();
            IM::BaseDefine::MsgType nMsgType = msg.msg_type();
            uint32_t nMsgLen = msg.msg_data().length();
            
            uint32_t nNow = (uint32_t)time(NULL);
            if (IM::BaseDefine::MsgType_IsValid(nMsgType))
            {
                if(nMsgLen != 0)
                {
                    CImPdu* pPduResp = new CImPdu;

                    uint32_t nMsgId = INVALID_VALUE;
                    uint32_t nSessionId = INVALID_VALUE;
                    uint32_t nPeerSessionId = INVALID_VALUE;

                    CMessageModel* pMsgModel = CMessageModel::getInstance();
                    CGroupMessageModel* pGroupMsgModel = CGroupMessageModel::getInstance();
                    if(nMsgType == IM::BaseDefine::MSG_TYPE_GROUP_TEXT) {
                        CGroupModel* pGroupModel = CGroupModel::getInstance();
                        if (pGroupModel->isValidateGroupId(nToId) && pGroupModel->isInGroup(nFromId, nToId))
                        {
                            nSessionId = CSessionModel::getInstance()->getSessionId(nFromId, nToId, IM::BaseDefine::SESSION_TYPE_GROUP, false);
                            if (INVALID_VALUE == nSessionId) {
                                nSessionId = CSessionModel::getInstance()->addSession(nFromId, nToId, IM::BaseDefine::SESSION_TYPE_GROUP);
                            }
                            if(nSessionId != INVALID_VALUE)
                            {
                                nMsgId = pGroupMsgModel->getMsgId(nToId);
                                if (nMsgId != INVALID_VALUE) {
                                    pGroupMsgModel->sendMessage(nFromId, nToId, nMsgType, nCreateTime, nMsgId, (string&)msg.msg_data());
                                    CSessionModel::getInstance()->updateSession(nSessionId, nNow);
                                }
                            }
                        }
                        else
                        {
                            SPDLOG_ERROR("invalid groupId. fromId={}, groupId={}", nFromId, nToId);
                            delete pPduResp;
                            return;
                        }
                    } else if (nMsgType == IM::BaseDefine::MSG_TYPE_GROUP_AUDIO) {
                        CGroupModel* pGroupModel = CGroupModel::getInstance();
                        if (pGroupModel->isValidateGroupId(nToId)&& pGroupModel->isInGroup(nFromId, nToId))
                        {
                            nSessionId = CSessionModel::getInstance()->getSessionId(nFromId, nToId, IM::BaseDefine::SESSION_TYPE_GROUP, false);
                            if (INVALID_VALUE == nSessionId) {
                                nSessionId = CSessionModel::getInstance()->addSession(nFromId, nToId, IM::BaseDefine::SESSION_TYPE_GROUP);
                            }
                            if(nSessionId != INVALID_VALUE)
                            {
                                nMsgId = pGroupMsgModel->getMsgId(nToId);
                                if(nMsgId != INVALID_VALUE)
                                {
                                    pGroupMsgModel->sendAudioMessage(nFromId, nToId, nMsgType, nCreateTime, nMsgId, msg.msg_data().c_str(), nMsgLen);
                                    CSessionModel::getInstance()->updateSession(nSessionId, nNow);
                                }
                            }
                        }
                        else
                        {
                            SPDLOG_ERROR("invalid groupId. fromId={}, groupId={}", nFromId, nToId);
                            delete pPduResp;
                            return;
                        }
                    } else if(nMsgType== IM::BaseDefine::MSG_TYPE_SINGLE_TEXT) {
                        if (nFromId != nToId) {
                            nSessionId = CSessionModel::getInstance()->getSessionId(nFromId, nToId, IM::BaseDefine::SESSION_TYPE_SINGLE, false);
                            if (INVALID_VALUE == nSessionId) {
                                nSessionId = CSessionModel::getInstance()->addSession(nFromId, nToId, IM::BaseDefine::SESSION_TYPE_SINGLE);
                            }
                            nPeerSessionId = CSessionModel::getInstance()->getSessionId(nToId, nFromId, IM::BaseDefine::SESSION_TYPE_SINGLE, false);
                            if(INVALID_VALUE ==  nPeerSessionId)
                            {
                                nSessionId = CSessionModel::getInstance()->addSession(nToId, nFromId, IM::BaseDefine::SESSION_TYPE_SINGLE);
                            }
                            uint32_t nRelateId = CRelationModel::getInstance()->getRelationId(nFromId, nToId, true);
                            if(nSessionId != INVALID_VALUE && nRelateId != INVALID_VALUE)
                            {
                                nMsgId = pMsgModel->getMsgId(nRelateId);
                                if(nMsgId != INVALID_VALUE)
                                {
                                    pMsgModel->sendMessage(nRelateId, nFromId, nToId, nMsgType, nCreateTime, nMsgId, (string&)msg.msg_data());
                                    CSessionModel::getInstance()->updateSession(nSessionId, nNow);
                                    CSessionModel::getInstance()->updateSession(nPeerSessionId, nNow);
                                }
                                else
                                {
                                    SPDLOG_ERROR("msgId is invalid. fromId={}, toId={}, nRelateId={}, nSessionId={}, nMsgType={}", nFromId, nToId, nRelateId, nSessionId, nMsgType);
                                }
                            }
                            else{
                                SPDLOG_ERROR("sessionId or relateId is invalid. fromId={}, toId={}, nRelateId={}, nSessionId={}, nMsgType={}", nFromId, nToId, nRelateId, nSessionId, nMsgType);
                            }
                        }
                        else
                        {
                            SPDLOG_ERROR("send msg to self. fromId={}, toId={}, msgType={}", nFromId, nToId, nMsgType);
                        }
                        
                    } else if(nMsgType == IM::BaseDefine::MSG_TYPE_SINGLE_AUDIO) {
                        
                        if(nFromId != nToId)
                        {
                            nSessionId = CSessionModel::getInstance()->getSessionId(nFromId, nToId, IM::BaseDefine::SESSION_TYPE_SINGLE, false);
                            if (INVALID_VALUE == nSessionId) {
                                nSessionId = CSessionModel::getInstance()->addSession(nFromId, nToId, IM::BaseDefine::SESSION_TYPE_SINGLE);
                            }
                            nPeerSessionId = CSessionModel::getInstance()->getSessionId(nToId, nFromId, IM::BaseDefine::SESSION_TYPE_SINGLE, false);
                            if(INVALID_VALUE ==  nPeerSessionId)
                            {
                                nSessionId = CSessionModel::getInstance()->addSession(nToId, nFromId, IM::BaseDefine::SESSION_TYPE_SINGLE);
                            }
                            uint32_t nRelateId = CRelationModel::getInstance()->getRelationId(nFromId, nToId, true);
                            if(nSessionId != INVALID_VALUE && nRelateId != INVALID_VALUE)
                            {
                                nMsgId = pMsgModel->getMsgId(nRelateId);
                                if(nMsgId != INVALID_VALUE) {
                                    pMsgModel->sendAudioMessage(nRelateId, nFromId, nToId, nMsgType, nCreateTime, nMsgId, msg.msg_data().c_str(), nMsgLen);
                                    CSessionModel::getInstance()->updateSession(nSessionId, nNow);
                                    CSessionModel::getInstance()->updateSession(nPeerSessionId, nNow);
                                }
                                else {
                                    SPDLOG_ERROR("msgId is invalid. fromId={}, toId={}, nRelateId={}, nSessionId={}, nMsgType={}", nFromId, nToId, nRelateId, nSessionId, nMsgType);
                                }
                            }
                            else {
                                SPDLOG_ERROR("sessionId or relateId is invalid. fromId={}, toId={}, nRelateId={}, nSessionId={}, nMsgType={}", nFromId, nToId, nRelateId, nSessionId, nMsgType);
                            }
                        }
                        else
                        {
                            SPDLOG_ERROR("send msg to self. fromId={}, toId={}, msgType={}", nFromId, nToId, nMsgType);
                        }
                    }

                    SPDLOG_ERROR("fromId={}, toId={}, type={}, msgId={}, sessionId={}", nFromId, nToId, nMsgType, nMsgId, nSessionId);

                    msg.set_msg_id(nMsgId);
                    pPduResp->SetPBMsg(&msg);
                    pPduResp->SetSeqNum(pPdu->GetSeqNum());
                    pPduResp->SetServiceId(IM::BaseDefine::SID_MSG);
                    pPduResp->SetCommandId(IM::BaseDefine::CID_MSG_DATA);
                    CProxyConn::AddResponsePdu(conn_uuid, pPduResp);
                }
                else
                {
                    SPDLOG_ERROR("msgLen error. fromId={}, toId={}, msgType={}", nFromId, nToId, nMsgType);
                }
            }
            else
            {
                SPDLOG_ERROR("invalid msgType.fromId={}, toId={}, msgType={}", nFromId, nToId, nMsgType);
            }
        }
        else
        {
            SPDLOG_ERROR("parse pb failed");
        }
    }

    void getMessageById(CImPdu* pPdu, uint32_t conn_uuid)
    {
        IM::Message::IMGetMsgByIdReq msg;
        if(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()))
        {
            uint32_t nUserId = msg.user_id();
            IM::BaseDefine::SessionType nType = msg.session_type();
            uint32_t nPeerId = msg.session_id();
            list<uint32_t> lsMsgId;
            uint32_t nCnt = msg.msg_id_list_size();
            for(uint32_t i=0; i<nCnt; ++i)
            {
                lsMsgId.push_back(msg.msg_id_list(i));
            }
            if (IM::BaseDefine::SessionType_IsValid(nType))
            {
                CImPdu* pPduResp = new CImPdu;
                IM::Message::IMGetMsgByIdRsp msgResp;

                list<IM::BaseDefine::MsgInfo> lsMsg;
                if(IM::BaseDefine::SESSION_TYPE_SINGLE == nType)
                {
                    CMessageModel::getInstance()->getMsgByMsgId(nUserId, nPeerId, lsMsgId, lsMsg);
                }
                else if(IM::BaseDefine::SESSION_TYPE_GROUP)
                {
                    CGroupMessageModel::getInstance()->getMsgByMsgId(nUserId, nPeerId, lsMsgId, lsMsg);
                }
                msgResp.set_user_id(nUserId);
                msgResp.set_session_id(nPeerId);
                msgResp.set_session_type(nType);
                for(auto it=lsMsg.begin(); it!=lsMsg.end(); ++it)
                {
                    IM::BaseDefine::MsgInfo* pMsg = msgResp.add_msg_list();
                    pMsg->set_msg_id(it->msg_id());
                    pMsg->set_from_session_id(it->from_session_id());
                    pMsg->set_create_time(it->create_time());
                    pMsg->set_msg_type(it->msg_type());
                    pMsg->set_msg_data(it->msg_data());
                }
                SPDLOG_ERROR("userId={}, peerId={}, sessionType={}, reqMsgCnt={}, resMsgCnt={}", nUserId, nPeerId, nType, msg.msg_id_list_size(), msgResp.msg_list_size());
                msgResp.set_attach_data(msg.attach_data());
                pPduResp->SetPBMsg(&msgResp);
                pPduResp->SetSeqNum(pPdu->GetSeqNum());
                pPduResp->SetServiceId(IM::BaseDefine::SID_MSG);
                pPduResp->SetCommandId(IM::BaseDefine::CID_MSG_GET_BY_MSG_ID_RES);
                CProxyConn::AddResponsePdu(conn_uuid, pPduResp);
            }
            else
            {
                SPDLOG_ERROR("invalid sessionType. fromId={}, toId={}, sessionType={}, msgCnt={}", nUserId, nPeerId, nType, nCnt);
            }
        }
        else
        {
            SPDLOG_ERROR("parse pb failed");
        }
    }

    void getLatestMsgId(CImPdu* pPdu, uint32_t conn_uuid)
    {
        IM::Message::IMGetLatestMsgIdReq msg;
        if(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()))
        {
            uint32_t nUserId = msg.user_id();
            IM::BaseDefine::SessionType nType = msg.session_type();
            uint32_t nPeerId = msg.session_id();
            if (IM::BaseDefine::SessionType_IsValid(nType)) {
                CImPdu* pPduResp = new CImPdu;
                IM::Message::IMGetLatestMsgIdRsp msgResp;
                msgResp.set_user_id(nUserId);
                msgResp.set_session_type(nType);
                msgResp.set_session_id(nPeerId);
                uint32_t nMsgId = INVALID_VALUE;
                if(IM::BaseDefine::SESSION_TYPE_SINGLE == nType)
                {
                    string strMsg;
                    IM::BaseDefine::MsgType nMsgType;
                    CMessageModel::getInstance()->getLastMsg(nUserId, nPeerId, nMsgId, strMsg, nMsgType, 1);
                }
                else
                {
                    string strMsg;
                    IM::BaseDefine::MsgType nMsgType;
                    uint32_t nFromId = INVALID_VALUE;
                    CGroupMessageModel::getInstance()->getLastMsg(nPeerId, nMsgId, strMsg, nMsgType, nFromId);
                }
                msgResp.set_latest_msg_id(nMsgId);
                SPDLOG_ERROR("userId={}, peerId={}, sessionType={}, msgId={}", nUserId, nPeerId, nType,nMsgId);
                msgResp.set_attach_data(msg.attach_data());
                pPduResp->SetPBMsg(&msgResp);
                pPduResp->SetSeqNum(pPdu->GetSeqNum());
                pPduResp->SetServiceId(IM::BaseDefine::SID_MSG);
                pPduResp->SetCommandId(IM::BaseDefine::CID_MSG_GET_LATEST_MSG_ID_RSP);
                CProxyConn::AddResponsePdu(conn_uuid, pPduResp);

            }
            else
            {
                SPDLOG_ERROR("invalid sessionType. userId={}, peerId={}, sessionType={}", nUserId, nPeerId, nType);
            }
        }
        else
        {
            SPDLOG_ERROR("parse pb failed");
        }
    }
};
