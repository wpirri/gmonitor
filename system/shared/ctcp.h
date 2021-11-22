/***************************************************************************
 * Copyright (C) 2003 Walter Pirri
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 ***************************************************************************/
#ifndef _CTCP_H_
#define _CTCP_H_

#include <string>
using namespace std;

#ifndef WAIT_INFINITE
#define WAIT_INFINITE	-1
#endif

#define SOPORTE_SSL_TLS

#ifdef SOPORTE_SSL_TLS
#include <openssl/ssl.h>
#include <openssl/err.h>
#endif

/* Valores de errores */

#define CTCP_OK                   0
#define CTCP_GENERAL_ERROR        -1
#define CTCP_HOST_NOT_FOUND       -2
#define CTCP_ADDRESS_NOT_FOUND    -3
#define CTCP_READ_ERROR           -4
#define CTCP_WRITE_ERROR          -5
#define CTCP_INVALID_ADDRESS      -6
#define CTCP_NOT_CONNECTED        -7
#ifdef SOPORTE_SSL_TLS
#define CTCP_SSL_LIB_ERROR        -8
#define CTCP_SSL_SESSION_ERROR    -9
#endif
class CTcp
{
public:
    CTcp();
    CTcp(int sock);
#ifdef SOPORTE_SSL_TLS
    CTcp(int ssl_tls, int ssl_tls_v);
#endif
    virtual ~CTcp();
    /* Abre un socket SERVER y hace un Listen, cuando se establece la conexion devuelve u objeto conectado */
    CTcp* Listen(const char *addr, int port);
    CTcp* Listen(string saddr, int port);
    /* Abre un socket CLIENTE y lo conecta con el server */
    int Connect(const char *adrrloc, const char *addrrem, int port); /* Conexion cliente */
    int Connect(string sadrrloc, string saddrrem, int port); /* Conexion cliente */
    /* cierra una conexion */
    int Close();
    /* cierra la conex�on sun hacer el shutdown del socket */
    int Kill();
    /* devuelve el port que esta usando el socket */
    int LocalPort();
    int RemotePort();
    char* LocalAddress();
    char* RemoteAddress();
    int Send(const void *msg, unsigned int msglen);
    int Receive(void *msg, unsigned int msglen, int to_ms);
    int Query(const char* raddr, int rport, const char* snd, char* rcv, int rcv_max_len, int to_ms);

    char* GetErrorText();
    int GetErrorNumber();
    int GetFD();
#ifdef SOPORTE_SSL_TLS
    SSL* GetSSL();
#endif

protected:
    void GetErr(const char *msg, ...);
    int LocalConn(const char *addr, int port);
    int RemoteConn(const char *addr, int port);
    unsigned int ResolvAddr(const char *addr);

private:
    char m_strLastError[1025];
    char m_strError[255];
    int m_iLastError;
    int m_sock;
    int m_port;
    char m_strLocalAddress[21];
    char m_strRemoteAddress[21];
#ifdef SOPORTE_SSL_TLS
    int m_ssl_tls;
    int m_ssl_tls_ver;
    const SSL_METHOD *m_p_method;
    SSL_CTX *m_p_ctx;
    SSL *m_p_ssl;
#endif

};
#endif /*_CTCP_H_*/

