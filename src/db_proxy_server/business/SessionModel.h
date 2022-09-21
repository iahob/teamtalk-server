/*================================================================
*     Copyright (c) 2015年 lanhu. All rights reserved.
*   
*   文件名称：SessionModel.h
*   创 建 者：Zhang Yuanhao
*   邮    箱：bluefoxah@gmail.com
*   创建日期：2015年03月16日
*   描    述：
*
#pragma once
================================================================*/
#ifndef __SESSIONMODEL_H__
#define __SESSIONMODEL_H__

#include "base/ImPduBase.h"
#include "IM.BaseDefine.pb.h"

class CSessionModel
{
public:
    static CSessionModel* getInstance(){
        static CSessionModel m_pInstance;
        return &m_pInstance;
    };
    ~CSessionModel() {}

    void getRecentSession(uint32_t userId, uint32_t lastTime, list<IM::BaseDefine::ContactSessionInfo>& lsContact);
    uint32_t getSessionId(uint32_t nUserId, uint32_t nPeerId, uint32_t nType, bool isAll);
    bool updateSession(uint32_t nSessionId, uint32_t nUpdateTime);
    bool removeSession(uint32_t nSessionId);
    uint32_t addSession(uint32_t nUserId, uint32_t nPeerId, uint32_t nType);
    
private:
    CSessionModel ()= default;
    CSessionModel (const CSessionModel &)= delete;
    CSessionModel & operator=(const CSessionModel &)= delete;
    void fillSessionMsg(uint32_t nUserId, list<IM::BaseDefine::ContactSessionInfo>& lsContact);
};

#endif /*defined(__SESSIONMODEL_H__) */
