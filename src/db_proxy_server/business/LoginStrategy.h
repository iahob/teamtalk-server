/*================================================================
*     Copyright (c) 2015年 lanhu. All rights reserved.
*   
*   文件名称：loginStrategy.h
*   创 建 者：Zhang Yuanhao
*   邮    箱：bluefoxah@gmail.com
*   创建日期：2015年03月09日
*   描    述：
*
#pragma once
================================================================*/
#ifndef __logINSTRATEGY_H__
#define __logINSTRATEGY_H__

#include <iostream>

#include "IM.BaseDefine.pb.h"

class CloginStrategy
{
public:
    virtual bool dologin(const std::string& strName, const std::string& strPass, IM::BaseDefine::UserInfo& user) = 0;
};

#endif /*defined(__logINSTRATEGY_H__) */
