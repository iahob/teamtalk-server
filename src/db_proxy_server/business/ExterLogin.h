/*================================================================
*     Copyright (c) 2015年 lanhu. All rights reserved.
*   
*   文件名称：ExterAuth.h
*   创 建 者：Zhang Yuanhao
*   邮    箱：bluefoxah@gmail.com
*   创建日期：2015年03月09日
*   描    述：
*
#pragma once
================================================================*/
#ifndef __EXTERlogIN_H__
#define __EXTERlogIN_H__

#include "loginStrategy.h"

class CExterloginStrategy:public CloginStrategy
{
public:
    virtual bool dologin(const std::string& strName, const std::string& strPass, IM::BaseDefine::UserInfo& user);
};
#endif /*defined(__EXTERlogIN_H__) */
