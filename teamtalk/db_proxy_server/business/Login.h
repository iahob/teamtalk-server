/*================================================================
 *   Copyright (C) 2014 All rights reserved.
 *
 *   文件名称：login.h
 *   创 建 者：Zhang Yuanhao
 *   邮    箱：bluefoxah@gmail.com
 *   创建日期：2014年12月15日
 *   描    述：
 *
 ================================================================*/

#ifndef logIN_H_
#define logIN_H_

#include "base/ImPduBase.h"

namespace DB_PROXY {

void dologin(CImPdu* pPdu, uint32_t conn_uuid);

};


#endif /* logIN_H_ */
