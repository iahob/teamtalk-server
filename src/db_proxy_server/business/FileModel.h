/*================================================================
*     Copyright (c) 2014年 lanhu. All rights reserved.
*   
*   文件名称：FileModel.h
*   创 建 者：Zhang Yuanhao
*   邮    箱：bluefoxah@gmail.com
*   创建日期：2014年12月31日
*   描    述：
*
#pragma once
================================================================*/
#ifndef __FILEMODEL_H__
#define __FILEMODEL_H__
#include "IM.File.pb.h"
#include "base/ImPduBase.h"

class CFileModel
{
public:
    virtual ~CFileModel()= default;
    static CFileModel* getInstance(){
        static CFileModel m_pInstance;
        return &m_pInstance;
    };
    
    void getOfflineFile(uint32_t userId, list<IM::BaseDefine::OfflineFileInfo>& lsOffline);
    void addOfflineFile(uint32_t fromId, uint32_t toId, string& taskId, string& fileName, uint32_t fileSize);
    void delOfflineFile(uint32_t fromId, uint32_t toId, string& taskId);
    
private:
    CFileModel ()= default;
    CFileModel (const CFileModel &)= delete;
    CFileModel & operator=(const CFileModel &)= delete;
};

#endif /*defined(__FILEMODEL_H__) */
