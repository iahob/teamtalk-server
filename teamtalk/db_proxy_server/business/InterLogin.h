/*================================================================
*     Copyright (c) 2015年 lanhu. All rights reserved.
*   
*   文件名称：InternalAuth.h
*   创 建 者：Zhang Yuanhao
*   邮    箱：bluefoxah@gmail.com
*   创建日期：2015年03月09日
*   描    述：内部数据库验证策略
*
#pragma once
================================================================*/
#ifndef __INTERlogIN_H__
#define __INTERlogIN_H__

#include "loginStrategy.h"

class CInterloginStrategy :public CloginStrategy
{
public:
    virtual bool dologin(const std::string& strName, const std::string& strPass, IM::BaseDefine::UserInfo& user);
};

#endif /*defined(__INTERlogIN_H__) */
