/*================================================================
*     Copyright (c) 2015年 lanhu. All rights reserved.
*   
*   文件名称：ExterAuth.cpp
*   创 建 者：Zhang Yuanhao
*   邮    箱：bluefoxah@gmail.com
*   创建日期：2015年03月09日
*   描    述：需要通过外部接口进行验证
*
================================================================*/
#include "Exterlogin.h"
#include "base/slog.h"


const std::string strloginUrl = "http://xxxx";
bool CExterloginStrategy::dologin(const std::string &strName, const std::string &strPass, IM::BaseDefine::UserInfo& user)
{
    bool bRet = false;
    
    return bRet;
}