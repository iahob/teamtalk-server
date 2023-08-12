/*================================================================
 *   Copyright (C) 2014 All rights reserved.
 *
 *   文件名称：login.cpp
 *   创 建 者：Zhang Yuanhao
 *   邮    箱：bluefoxah@gmail.com
 *   创建日期：2014年12月15日
 *   描    述：
 *
 ================================================================*/

#include <list>
#include "../ProxyConn.h"
#include "base/HttpClient.h"
#include "../SyncCenter.h"
#include "login.h"
#include "UserModel.h"
#include "base/TokenValidator.h"
#include "json/json.h"
#include "Common.h"
#include "IM.Server.pb.h"
#include "base/Base64.h"
#include "Interlogin.h"
#include "Exterlogin.h"
#include "base/slog.h"

CInterloginStrategy g_loginStrategy;

unordered_map<string, list<uint32_t> > g_hmLimits;
CLock g_cLimitLock;
namespace DB_PROXY {
    
    
void dologin(CImPdu* pPdu, uint32_t conn_uuid)
{
    
    CImPdu* pPduResp = new CImPdu;
    
    IM::Server::IMValidateReq msg;
    IM::Server::IMValidateRsp msgResp;
    if(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()))
    {
        
        string strDomain = msg.user_name();
        string strPass = msg.password();
        
        msgResp.set_user_name(strDomain);
        msgResp.set_attach_data(msg.attach_data());
	//清理超过5分钟的错误时间点记录
	/*
		清理放在这里还是放在密码错误后添加的时候呢？
		放在这里，每次都要遍历，会有一点点性能的损失。
		放在后面，可能会造成30分钟之前有10次错的，但是本次是对的就没办法再访问了。
	*/
	{
	    CAutoLock cAutoLock(&g_cLimitLock);
	    list<uint32_t>& lsErrorTime = g_hmLimits[strDomain];
	    int kTimeLimit = 5 * 60;
	    list<uint32_t> temp_list;
	    uint32_t tmNow = time(NULL);

	    // 判断5分钟内密码错误次数是否大于10
	    if (lsErrorTime.size() > 10) {
                for (auto itTime = lsErrorTime.begin(); itTime != lsErrorTime.end(); ++itTime)
		{
		    if ((tmNow - *itTime) < kTimeLimit) // 淘汰早于5分钟前的
		    {
		        temp_list.push_back(*itTime);
		    }
		}
			
		uint32_t last_error_time = temp_list.front(); // 倒序
		if (tmNow - last_error_time <= kTimeLimit) {
		    msgResp.set_result_code(6);
		    msgResp.set_result_string("用户名/密码错误次数太多");
		    pPduResp->SetPBMsg(&msgResp);
		    pPduResp->SetSeqNum(pPdu->GetSeqNum());
		    pPduResp->SetServiceId(IM::BaseDefine::SID_OTHER);
		    pPduResp->SetCommandId(IM::BaseDefine::CID_OTHER_VALIDATE_RSP);
		    CProxyConn::AddResponsePdu(conn_uuid, pPduResp);

		    // 加入
		    if(temp_list.size() > 10)
		        temp_list.pop_back();		// 删除最早时候的错误时间
		    temp_list.push_front(tmNow);	// 更新这次错误时间
		    lsErrorTime.clear();
		    for (auto itTime = temp_list.begin(); itTime != temp_list.end(); ++itTime) {
		       lsErrorTime.push_back(*itTime);
		    }
		    return;
               }
               else {
                   lsErrorTime.clear();
               }
           }
	}	        

	SPDLOG_INFO("{} request login.", strDomain.c_str());

	IM::BaseDefine::UserInfo cUser;

	if(g_loginStrategy.dologin(strDomain, strPass, cUser))
	{
		IM::BaseDefine::UserInfo* pUser = msgResp.mutable_user_info();
		pUser->set_user_id(cUser.user_id());
		pUser->set_user_gender(cUser.user_gender());
		pUser->set_department_id(cUser.department_id());
		pUser->set_user_nick_name(cUser.user_nick_name());
		pUser->set_user_domain(cUser.user_domain());
		pUser->set_avatar_url(cUser.avatar_url());

		pUser->set_email(cUser.email());
		pUser->set_user_tel(cUser.user_tel());
		pUser->set_user_real_name(cUser.user_real_name());
		pUser->set_status(0);

		pUser->set_sign_info(cUser.sign_info());

		msgResp.set_result_code(0);
		msgResp.set_result_string("成功");

		//如果登陆成功，则清除错误尝试限制
		CAutoLock cAutoLock(&g_cLimitLock);
		list<uint32_t>& lsErrorTime = g_hmLimits[strDomain];
		lsErrorTime.clear();
	}
	else
	{
		//密码错误，记录一次登陆失败
		uint32_t tmCurrent = time(NULL);
		CAutoLock cAutoLock(&g_cLimitLock);
		list<uint32_t>& lsErrorTime = g_hmLimits[strDomain];
		lsErrorTime.push_front(tmCurrent);

		SPDLOG_ERROR("get result false");
		msgResp.set_result_code(1);
		msgResp.set_result_string("用户名/密码错误");
	}
    }
    else
    {
	    msgResp.set_result_code(2);
	    msgResp.set_result_string("服务端内部错误");
    }


    pPduResp->SetPBMsg(&msgResp);
    pPduResp->SetSeqNum(pPdu->GetSeqNum());
    pPduResp->SetServiceId(IM::BaseDefine::SID_OTHER);
    pPduResp->SetCommandId(IM::BaseDefine::CID_OTHER_VALIDATE_RSP);
    CProxyConn::AddResponsePdu(conn_uuid, pPduResp);
}

};

