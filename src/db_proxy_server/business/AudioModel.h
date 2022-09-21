/*================================================================
 *   Copyright (C) 2014 All rights reserved.
 *
 *   文件名称：AudioModel.h
 *   创 建 者：Zhang Yuanhao
 *   邮    箱：bluefoxah@gmail.com
 *   创建日期：2014年12月15日
 *   描    述：
 *
 ================================================================*/

#ifndef AUDIO_MODEL_H_
#define AUDIO_MODEL_H_


#include <list>
#include <map>
#include "base/public_define.h"
#include "base/util.h"
#include "IM.BaseDefine.pb.h"

using namespace std;

class CAudioModel {

public:
	static CAudioModel* getInstance(){
	    static CAudioModel m_pInstance;
        return &m_pInstance;
    };
    virtual ~CAudioModel ()= default;

    void setUrl(string& strFileUrl);
    
    bool readAudios(list<IM::BaseDefine::MsgInfo>& lsMsg);
    
    int saveAudioInfo(uint32_t nFromId, uint32_t nToId, uint32_t nCreateTime, const char* pAudioData, uint32_t nAudioLen);
    
private:

    string m_strFileSite;
    CAudioModel ()= default;
    CAudioModel (const CAudioModel &)= delete;
    CAudioModel & operator=(const CAudioModel &)= delete;
    bool readAudioContent(uint32_t nCostTime, uint32_t nSize, const string& strPath, IM::BaseDefine::MsgInfo& msg);

};



#endif /* AUDIO_MODEL_H_ */
