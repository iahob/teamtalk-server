/*
 * loginConn.h
 *
 *  Created on: 2013-6-21
 *      Author: jianqingdu
 */

#ifndef logINCONN_H_
#define logINCONN_H_

#include "base/imconn.h"

enum {
	logIN_CONN_TYPE_CLIENT = 1,
	logIN_CONN_TYPE_MSG_SERV
};

 struct msg_serv_info_t {
    string		ip_addr1;	// 电信IP
    string		ip_addr2;	// 网通IP
    uint16_t	port;
    uint32_t	max_conn_cnt;
    uint32_t	cur_conn_cnt;
    string 		hostname;	// 消息服务器的主机名
} ;


class CloginConn : public CImConn
{
public:
	CloginConn();
	virtual ~CloginConn();

	virtual void Close();

	void OnConnect2(net_handle_t handle, int conn_type);
	virtual void OnClose();
	virtual void OnTimer(uint64_t curr_tick);

	virtual void HandlePdu(CImPdu* pPdu);
private:
	void _HandleMsgServInfo(CImPdu* pPdu);
	void _HandleUserCntUpdate(CImPdu* pPdu);
	void _HandleMsgServRequest(CImPdu* pPdu);

private:
	int	m_conn_type;
};

void init_login_conn();

#endif /* logINCONN_H_ */
