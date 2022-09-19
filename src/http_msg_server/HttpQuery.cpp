/*
 * HttpQuery.cpp
 *
 *  Created on: 2013-10-22
 *      Author: ziteng@mogujie.com
 */
#include <sstream>

#include "HttpQuery.h"
#include "RouteServConn.h"
#include "DBServConn.h"
#include "HttpPdu.h"
#include "base/public_define.h"
#include "AttachData.h"
#include "IM.Message.pb.h"
#include "IM.Buddy.pb.h"
#include "IM.SwitchService.pb.h"
#include "IM.Group.pb.h"
#include "base/EncDec.h"
#include "base/slog.h"
static uint32_t g_total_query = 0;
static uint32_t g_last_year = 0;
static uint32_t g_last_month = 0;
static uint32_t g_last_mday = 0;

CHttpQuery* CHttpQuery::m_query_instance = NULL;

std::unordered_map<string, auth_struct*> g_hm_http_auth;
extern CAes *pAes;

static bool checkValueIsNullForJson(Json::Value &json_obj, string key)
{
    bool result = json_obj[key].isNull();
    if(result) {
        SPDLOG_ERROR("not value for key:%s", key.c_str());
    }
    return result;
}

void http_query_timer_callback(void* callback_data, uint8_t msg, uint32_t handle, void* pParam)
{
	struct tm* tm;
	time_t currTime;

	time(&currTime);
	tm = localtime(&currTime);

	uint32_t year = tm->tm_year + 1900;
	uint32_t mon = tm->tm_mon + 1;
	uint32_t mday = tm->tm_mday;
	if (year != g_last_year || mon != g_last_month || mday != g_last_mday) {
		// a new day begin, clear the count
		SPDLOG_ERROR("a new day begin, g_total_query={} ", g_total_query);
		g_total_query = 0;
		g_last_year = year;
		g_last_month = mon;
		g_last_mday = mday;
	}
}

CHttpQuery* CHttpQuery::GetInstance()
{
	if (!m_query_instance) {
		m_query_instance = new CHttpQuery();
		netlib_register_timer(http_query_timer_callback, NULL, 1000);
	}

	return m_query_instance;
}

void CHttpQuery::DispatchQuery(std::string& url, std::string& post_data, CHttpConn* pHttpConn)
{
	++g_total_query;
    
    /* 在用
     /query/SendMessageByXiaoT
     /query/GroupP2PMessage
     
     */
	SPDLOG_ERROR("DispatchQuery, url=%s, content=%s ", url.c_str(), post_data.c_str());

    Json::Reader reader;
    Json::Value value;
    Json::Value root;

	if ( !reader.parse(post_data, value) ) {
		SPDLOG_ERROR("json parse failed, post_data=%s ", post_data.c_str());
		pHttpConn->Close();
		return;
	}
    
    string strErrorMsg;
    string strAppKey;
    HTTP_ERROR_CODE nRet = HTTP_ERROR_SUCCESS;
    try
    {
        string strInterface(url.c_str() + strlen("/query/"));
        strAppKey = value["app_key"].asString();
        string strIp = pHttpConn->GetPeerIP();
        uint32_t nUserId = value["req_user_id"].asUInt();
        nRet = _CheckAuth(strAppKey, nUserId, strInterface, strIp);
    }
    catch ( std::runtime_error msg)
    {
        nRet = HTTP_ERROR_INTERFACE;
    }
    
    if(HTTP_ERROR_SUCCESS != nRet)
    {
        if(nRet < HTTP_ERROR_MAX)
        {
            root["error_code"] = nRet;
            root["error_msg"] = HTTP_ERROR_MSG[nRet];
        }
        else
        {
            root["error_code"] = -1;
            root["error_msg"] = "未知错误";
        }
        string strResponse = root.toStyledString();
        pHttpConn->Send((void*)strResponse.c_str(), strResponse.length());
        return;
    }
    
    // process post request with post content
    if (strcmp(url.c_str(), "/query/CreateGroup") == 0)
    {
        _QueryCreateGroup(strAppKey, value, pHttpConn);
    }
    else if (strcmp(url.c_str(), "/query/ChangeMembers") == 0)
    {
        _QueryChangeMember(strAppKey, value, pHttpConn);
    }else if(strcmp(url.c_str(), "/query/SendP2PSingleMessage") == 0)
    {
        _SendP2PSingleMessage(strAppKey, value, pHttpConn);    
    }
    else {
        SPDLOG_ERROR("url not support ");
        pHttpConn->Close();
        return;
    }
}

void CHttpQuery::_QueryCreateGroup(const string& strAppKey, Json::Value &post_json_obj, CHttpConn *pHttpConn)
{
    HTTP::CDBServConn *pConn = HTTP::get_db_serv_conn();
    if (!pConn) {
        SPDLOG_ERROR("no connection to DBProxy ");
        char* response_buf = PackSendResult(HTTP_ERROR_SERVER_EXCEPTION , HTTP_ERROR_MSG[9].c_str());
        pHttpConn->Send(response_buf, (uint32_t)strlen(response_buf));
        pHttpConn->Close();
        return;
    }
    
    if ( post_json_obj["req_user_id"].isNull()) {
        SPDLOG_ERROR("no user id ");
        char* response_buf = PackSendResult(HTTP_ERROR_PARMENT, HTTP_ERROR_MSG[1].c_str());
        pHttpConn->Send(response_buf, (uint32_t)strlen(response_buf));
        pHttpConn->Close();
        return;

    }
    
    if (post_json_obj["group_name"].isNull()) {
        SPDLOG_ERROR("no group name ");
        char* response_buf = PackSendResult(HTTP_ERROR_PARMENT, HTTP_ERROR_MSG[1].c_str());
        pHttpConn->Send(response_buf, (uint32_t)strlen(response_buf));
        pHttpConn->Close();
        return;
    }
    
    if (post_json_obj["group_type"].isNull()) {
        SPDLOG_ERROR("no group type ");
        char* response_buf = PackSendResult(HTTP_ERROR_PARMENT, HTTP_ERROR_MSG[1].c_str());
        pHttpConn->Send(response_buf, (uint32_t)strlen(response_buf));
        pHttpConn->Close();
        return;
    }
    
    if (post_json_obj["group_avatar"].isNull()) {
        SPDLOG_ERROR("no group avatar ");
        char* response_buf = PackSendResult(HTTP_ERROR_PARMENT, HTTP_ERROR_MSG[1].c_str());
        pHttpConn->Send(response_buf, (uint32_t)strlen(response_buf));
        pHttpConn->Close();
        return;
    }
    
    if (post_json_obj["user_id_list"].isNull()) {
        SPDLOG_ERROR("no user list ");
        char* response_buf = PackSendResult(HTTP_ERROR_PARMENT, HTTP_ERROR_MSG[1].c_str());
        pHttpConn->Send(response_buf, (uint32_t)strlen(response_buf));
        pHttpConn->Close();
        return;
    }
    
    try
    {
        uint32_t user_id = post_json_obj["req_user_id"].asUInt();
        string group_name = post_json_obj["group_name"].asString();
        uint32_t group_type = post_json_obj["group_type"].asUInt();
        string group_avatar = post_json_obj["group_avatar"].asString();
        uint32_t user_cnt = post_json_obj["user_id_list"].size();
        SPDLOG_ERROR("QueryCreateGroup, user_id: {}, group_name: %s, group_type: {}, user_cnt: {}. ",
            user_id, group_name.c_str(), group_type, user_cnt);
        if (!IM::BaseDefine::GroupType_IsValid(group_type)) {
            SPDLOG_ERROR("QueryCreateGroup, unvalid group_type");
            char* response_buf = PackSendResult(HTTP_ERROR_PARMENT, HTTP_ERROR_MSG[1].c_str());
            pHttpConn->Send(response_buf, (uint32_t)strlen(response_buf));
            pHttpConn->Close();
            return;
        }
        
        CDbAttachData attach_data(ATTACH_TYPE_HANDLE, pHttpConn->GetConnHandle());
        IM::Group::IMGroupCreateReq msg;
        msg.set_user_id(0);
        msg.set_group_name(group_name);
        msg.set_group_avatar(group_avatar);
        msg.set_group_type((::IM::BaseDefine::GroupType)group_type);
        for (uint32_t i = 0; i < user_cnt; i++) {
            uint32_t member_id = post_json_obj["user_id_list"][i].asUInt();
            msg.add_member_id_list(member_id);
        }
        msg.set_attach_data(attach_data.GetBuffer(), attach_data.GetLength());
        
        CImPdu pdu;
        pdu.SetPBMsg(&msg);
        pdu.SetServiceId(IM::BaseDefine::SID_GROUP);
        pdu.SetCommandId(IM::BaseDefine::CID_GROUP_CREATE_REQUEST);
        pConn->SendPdu(&pdu);

    }
    catch (std::runtime_error msg)
    {
        SPDLOG_ERROR("parse json data failed.");
        char* response_buf = PackSendResult(HTTP_ERROR_PARMENT, HTTP_ERROR_MSG[1].c_str());
        pHttpConn->Send(response_buf, (uint32_t)strlen(response_buf));
        pHttpConn->Close();
    }
    
}


void CHttpQuery::_SendP2PSingleMessage(const string& strAppKey,Json::Value& post_json_obj, CHttpConn* pHttpConn)
{
    HTTP::CDBServConn *pConn = HTTP::get_db_serv_conn();
    if (!pConn) {
        SPDLOG_ERROR("no connection to MsgServConn ");
        char* response_buf = PackSendResult(HTTP_ERROR_SERVER_EXCEPTION, HTTP_ERROR_MSG[9].c_str());
        pHttpConn->Send(response_buf, (uint32_t)strlen(response_buf));
        pHttpConn->Close();
        return;
    }

    if(checkValueIsNullForJson(post_json_obj, "from_user_id") ||
       checkValueIsNullForJson(post_json_obj, "to_user_id")   ||
       checkValueIsNullForJson(post_json_obj, "create_time")  ||
       checkValueIsNullForJson(post_json_obj, "msg") ) {
        char* response_buf = PackSendResult(HTTP_ERROR_PARMENT, HTTP_ERROR_MSG[1].c_str());
        pHttpConn->Send(response_buf, (uint32_t)strlen(response_buf));
        pHttpConn->Close();
        return;
    }

    try
    {
        uint32_t user_id = post_json_obj["from_user_id"].asUInt();
        uint32_t to_id = post_json_obj["to_user_id"].asUInt();
        uint32_t create_time = post_json_obj["create_time"].asUInt();

    uint32_t msg_type = IM::BaseDefine::MSG_TYPE_SINGLE_TEXT;

    if(!post_json_obj["msg_type"].isNull()) {
        msg_type = post_json_obj["msg_type"].asUInt();
    }

        string msgContent;
        if(post_json_obj["msg"].isString())
        {
            msgContent = post_json_obj["msg"].asString();
        }else if(post_json_obj["msg"].isObject())
        {
            msgContent = post_json_obj["msg"].toStyledString();
        }   
        char* msg_out = NULL;
        uint32_t msg_out_len = 0;
        if(pAes->Encrypt(msgContent.c_str(),msgContent.length(),&msg_out,msg_out_len) == 0)
        {
             msgContent = string(msg_out, msg_out_len);
        }
        pAes->Free(msg_out);

        create_time = time(NULL);
        CDbAttachData attach_data(ATTACH_TYPE_HANDLE, pHttpConn->GetConnHandle());
        IM::Message::IMMsgData msg;
        msg.set_from_user_id(user_id);
        msg.set_to_session_id(to_id);
        msg.set_msg_data(msgContent);
        msg.set_msg_id(1);
        //msg.set_msg_type(IM::BaseDefine::MSG_TYPE_SINGLE_TEXT);
 //      msg.set_msg_type(IM::BaseDefine::MSG_TYPE_SINGLE_OTHER);
        msg.set_msg_type((IM::BaseDefine::MsgType)msg_type);
        msg.set_create_time(create_time);
        msg.set_attach_data(attach_data.GetBuffer(), attach_data.GetLength());
        CImPdu pdu;
        pdu.SetPBMsg(&msg);
        pdu.SetSeqNum(pHttpConn->GetConnHandle());
        pdu.SetServiceId(IM::BaseDefine::SID_MSG);
        pdu.SetCommandId(IM::BaseDefine::CID_MSG_DATA);
        pConn->SendPdu(&pdu);
    }
    catch (std::runtime_error msg)
    {
        SPDLOG_ERROR("parse json data failed.");
        char* response_buf = PackSendResult(HTTP_ERROR_PARMENT, HTTP_ERROR_MSG[1].c_str());
        pHttpConn->Send(response_buf, (uint32_t)strlen(response_buf));
        pHttpConn->Close();
    }   
}

void CHttpQuery::_QueryChangeMember(const string& strAppKey, Json::Value &post_json_obj, CHttpConn *pHttpConn)
{
    HTTP::CDBServConn *pConn = HTTP::get_db_serv_conn();
    if (!pConn) {
        SPDLOG_ERROR("no connection to dbServConn ");
        char* response_buf = PackSendResult(HTTP_ERROR_SERVER_EXCEPTION, HTTP_ERROR_MSG[9].c_str());
        pHttpConn->Send(response_buf, (uint32_t)strlen(response_buf));
        pHttpConn->Close();
        return;
    }
    if ( post_json_obj["req_user_id"].isNull()) {
        SPDLOG_ERROR("no user id ");
        char* response_buf = PackSendResult(HTTP_ERROR_PARMENT, HTTP_ERROR_MSG[1].c_str());
        pHttpConn->Send(response_buf, (uint32_t)strlen(response_buf));
        pHttpConn->Close();
        return;
    }
    
    if ( post_json_obj["group_id"].isNull() ) {
        SPDLOG_ERROR("no group id ");
        char* response_buf = PackSendResult(HTTP_ERROR_PARMENT, HTTP_ERROR_MSG[1].c_str());
        pHttpConn->Send(response_buf, (uint32_t)strlen(response_buf));
        pHttpConn->Close();
        return;
    }
    
    if ( post_json_obj["modify_type"].isNull() ) {
        SPDLOG_ERROR("no modify_type ");
        char* response_buf = PackSendResult(HTTP_ERROR_PARMENT, HTTP_ERROR_MSG[1].c_str());
        pHttpConn->Send(response_buf, (uint32_t)strlen(response_buf));
        pHttpConn->Close();
        return;
    }
    
    if (post_json_obj["user_id_list"].isNull()) {
        SPDLOG_ERROR("no user list ");
        char* response_buf = PackSendResult(HTTP_ERROR_PARMENT, HTTP_ERROR_MSG[1].c_str());
        pHttpConn->Send(response_buf, (uint32_t)strlen(response_buf));
        pHttpConn->Close();
        return;
    }
    
    try
    {
        uint32_t user_id = post_json_obj["req_user_id"].asUInt();
        uint32_t group_id = post_json_obj["group_id"].asUInt();
        uint32_t modify_type = post_json_obj["modify_type"].asUInt();
        uint32_t user_cnt =  post_json_obj["user_id_list"].size();
        uint32_t handle = pHttpConn->GetConnHandle();

        SPDLOG_ERROR("QueryChangeMember, user_id: {}, group_id: {}, modify type: {}, user_cnt: {} , handle: {}",
            user_id, group_id, modify_type, user_cnt, handle);
        if (!IM::BaseDefine::GroupModifyType_IsValid(modify_type)) {
            SPDLOG_ERROR("QueryChangeMember, unvalid modify_type");
            char* response_buf = PackSendResult(HTTP_ERROR_PARMENT, HTTP_ERROR_MSG[1].c_str());
            pHttpConn->Send(response_buf, (uint32_t)strlen(response_buf));
            pHttpConn->Close();
            return;
        }
        CDbAttachData attach_data(ATTACH_TYPE_HANDLE, handle);
        IM::Group::IMGroupChangeMemberReq msg;
        msg.set_user_id(0);
        msg.set_change_type((::IM::BaseDefine::GroupModifyType)modify_type);
        msg.set_group_id(group_id);
        for (uint32_t i = 0; i < user_cnt; i++) {
            uint32_t member_id = post_json_obj["user_id_list"][i].asUInt();
            msg.add_member_id_list(member_id);
        }
        msg.set_attach_data(attach_data.GetBuffer(), attach_data.GetLength());
        CImPdu pdu;
        pdu.SetPBMsg(&msg);
        pdu.SetServiceId(IM::BaseDefine::SID_GROUP);
        pdu.SetCommandId(IM::BaseDefine::CID_GROUP_CHANGE_MEMBER_REQUEST);
        pConn->SendPdu(&pdu);
    }
    catch (std::runtime_error msg)
    {
        SPDLOG_ERROR("parse json data failed.");
        char* response_buf = PackSendResult(HTTP_ERROR_PARMENT, HTTP_ERROR_MSG[1].c_str());
        pHttpConn->Send(response_buf, (uint32_t)strlen(response_buf));
        pHttpConn->Close();
    }
}

HTTP_ERROR_CODE CHttpQuery::_CheckAuth(const string& strAppKey, const uint32_t userId, const string& strInterface, const string& strIp)
{
    return HTTP_ERROR_SUCCESS;
}

HTTP_ERROR_CODE CHttpQuery::_CheckPermission(const string& strAppKey, uint8_t nType, const list<uint32_t>& lsToId, string strMsg)
{
    strMsg.clear();
    return HTTP_ERROR_SUCCESS;
}

