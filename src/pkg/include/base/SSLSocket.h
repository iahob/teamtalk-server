
#ifndef __SSLSOCKET_H__
#define __SSLSOCKET_H__
#include "BaseSocket.h"
#include <resolv.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

class CSSLSocket : public CBaseSocket 
{
public:
    virtual int Send(void* buf, int len);
    virtual int Recv(void* buf, int len);
    virtual void _AcceptNewSocket();
    void setSSL(SSL *ssl){m_ssl = ssl;}
    SSL *getSSL(){return m_ssl;}

public:
    BIO  *buf_io;
    BIO  *ssl_bio;
private:
    SSL *m_ssl;

};


void initSSLConfig(char* CertFile, char* KeyFile);

#endif