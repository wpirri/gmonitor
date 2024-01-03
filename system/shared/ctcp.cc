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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

/*
    https://www.codeproject.com/Tips/168704/How-to-set-a-socket-connection-timeout

    Implementacion de comunicacion a travez de TCP
    Soporte SSL/TLS
      Fuente:
        https://fm4dd.com/openssl/sslconnect.shtm 
        https://wiki.openssl.org/index.php/Simple_TLS_Server
      Instalar:
        apt install libssl-dev
      Incluir:
        #include <openssl/bio.h>
        #include <openssl/ssl.h>
        #include <openssl/err.h>
        #include <openssl/pem.h>
        #include <openssl/x509.h>
        #include <openssl/x509_vfy.h>
      Linkear:
        -lssl -lcrypto
        IMPORTANTE: En la compilación final el parámetro -O debe preceder a -lssl y -lcrypto

*/

#include <ctcp.h>

#include <string>
#include <iostream>
#include <cstdio>
#include <cerrno>
using namespace std;

#include <unistd.h> /* close() */
#include <fcntl.h>
#include <sys/time.h> /* struct timeval */
#include <sys/poll.h> /* poll() */

/* includes para los sockets */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <stdarg.h>
#include <syslog.h>
#ifdef DEBUG_TCP
#include <errno.h>
#endif

CTcp::CTcp()
{
#ifdef DEBUG_TCP
  syslog(LOG_INFO, "[CTcp::CTcp()]");
#endif
  m_sock = (-1);
#ifdef SOPORTE_SSL_TLS
#ifdef DEBUG_TCP
  syslog(LOG_INFO, "[CTcp::CTcp()] - Soporte TLS habilitado");
#endif
  m_p_ssl = NULL;
  m_p_ctx = NULL;
  m_ssl_tls = 0;
  m_ssl_tls_ver = 0;
  m_cert_pem = "";
  m_key_pem = "";
#endif
  m_iLastError = 0;
  strcpy(m_strLastError, "");
  strcpy(m_strLocalAddress, "");
  strcpy(m_strRemoteAddress, "");
}

CTcp::CTcp(int sock)
{
#ifdef DEBUG_TCP
  syslog(LOG_INFO, "[CTcp::CTcp(sock = %i)]", sock);
#endif
  m_sock = sock;
#ifdef SOPORTE_SSL_TLS
#ifdef DEBUG_TCP
  syslog(LOG_INFO, "[CTcp::CTcp()] - Soporte TLS habilitado");
#endif
  m_p_ssl = NULL;
  m_p_ctx = NULL;
  m_ssl_tls = 0;
  m_ssl_tls_ver = 0;
  m_cert_pem = "";
  m_key_pem = "";
#endif
  m_iLastError = 0;
  strcpy(m_strLastError, "");
}

#ifdef SOPORTE_SSL_TLS
CTcp::CTcp(int ssl_tls, int ssl_tls_v)
{
#ifdef DEBUG_TCP
  syslog(LOG_INFO, "[CTcp::CTcp(ssl_tls = %i, ssl_tls_v = %i)]", ssl_tls, ssl_tls_v);
#endif
  m_p_ssl = NULL;
  m_p_ctx = NULL;
  m_ssl_tls = ssl_tls;
  m_ssl_tls_ver = ssl_tls_v;
  m_cert_pem = "";
  m_key_pem = "";

  m_sock = (-1);
  m_iLastError = 0;
  strcpy(m_strLastError, "");
  strcpy(m_strLocalAddress, "");
  strcpy(m_strRemoteAddress, "");

  if(m_ssl_tls != 0)
  {
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    if(SSL_library_init() < 0)
    {
#ifdef DEBUG_TCP
      syslog(LOG_INFO, "[CTcp::CTcp(ssl_tls = %i, ssl_tls_v = %i)] - TLS lib error", ssl_tls, ssl_tls_v);
#endif
      m_ssl_tls = (-1);
    }
  }
}

CTcp::CTcp(int sock, SSL *ssl_sock, int ver)
{
#ifdef DEBUG_TCP
  syslog(LOG_INFO, "[CTcp::CTcp(sock = %i, SSL* = 0x%X, ver = %i)]", sock, ssl_sock, ver);
#endif
  m_p_ssl = ssl_sock;
  m_p_ctx = NULL;
  m_ssl_tls = 1;
  m_ssl_tls_ver = ver;
  m_cert_pem = "";
  m_key_pem = "";

  m_sock = sock;
  m_iLastError = 0;
  strcpy(m_strLastError, "");
}
#endif

CTcp::~CTcp()
{
#ifdef DEBUG_TCP
  syslog(LOG_INFO, "[CTcp::CTcp~()]");
#endif
  Close();
}

unsigned int CTcp::ResolvAddr(const char *addr)
{
  struct hostent *hostEnt;
  struct in_addr inaddr;

  if(!addr)
  {
#ifdef DEBUG_TCP
    syslog(LOG_INFO, "[CTcp::ResolvAddr(addr = INADDR_ANY)]");
#endif
    inaddr.s_addr = INADDR_ANY;
  }
  else
  {
    if(strlen(addr))
    {
#ifdef DEBUG_TCP
      syslog(LOG_INFO, "[CTcp::ResolvAddr(addr = %s)]", addr);
#endif
      /* trato de obtener la dirección del formato nnn.nnn.nnn.nnn */
      if(!inet_aton((char*)addr, &inaddr))
      {
        /* no estaba en ese formato -> lo busco por DNS */
#ifdef DEBUG_TCP
        syslog(LOG_INFO, "[CTcp::ResolvAddr(addr = %s)] - gethostbyname", addr);
#endif
        hostEnt = gethostbyname((char*)addr);
        if(!hostEnt)
        {
#ifdef DEBUG_TCP
          syslog(LOG_INFO, "[CTcp::ResolvAddr(addr = %s)] - gethostbyname -> INADDR_NONE", addr);
#endif
          inaddr.s_addr = INADDR_NONE;
        }
        else
        {
#ifdef DEBUG_TCP
          syslog(LOG_INFO, "[CTcp::ResolvAddr(addr = %s)] - gethostbyname -> %s", addr, hostEnt->h_addr_list[0]);
#endif
          memcpy(&inaddr, hostEnt->h_addr_list[0], hostEnt->h_length);
        }
      }
#ifdef DEBUG_TCP
      else
      {
        syslog(LOG_INFO, "[CTcp::ResolvAddr(addr = %s)] - inet_aton Ok", addr);
      }
#endif
    }
    else
    {
#ifdef DEBUG_TCP
      syslog(LOG_INFO, "[CTcp::ResolvAddr(addr = INADDR_ANY)]");
#endif
      inaddr.s_addr = INADDR_ANY;
    }
  }
  return inaddr.s_addr;
}

int CTcp::LocalConn(const char *addr, int port)
{
  int sock;
  struct sockaddr_in local;
  int localLen;

#ifdef DEBUG_TCP
      syslog(LOG_INFO, "[CTcp::LocalConn(addr = %s, port = %i)]", addr, port);
#endif
  /* abro el socket */
  if((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
  {
    /* error al crear el socket */
#ifdef DEBUG_TCP
    syslog(LOG_INFO, "[CTcp::LocalConn(addr = %s, port = %i) - socket error]", addr, port);
#endif
    GetErr("[CTCP] ERROR LocalConn()->socket() (%s:%i)", (addr)?addr:"", port);
    return CTCP_GENERAL_ERROR;
  }
  /* conecto el socket a la red */
#ifdef DEBUG_TCP
  syslog(LOG_INFO, "[CTcp::LocalConn(addr = %s, port = %i)] - socket ok", addr, port);
#endif
  memset(&(local.sin_zero), 0, sizeof(local.sin_zero));
  local.sin_family = AF_INET;
  local.sin_port = htons(port);
  if((local.sin_addr.s_addr = ResolvAddr(addr)) == INADDR_NONE)
  {
#ifdef DEBUG_TCP
    syslog(LOG_INFO, "[CTcp::LocalConn(addr = %s, port = %i)] - ResolvAddr error", addr, port);
#endif
    /* el DNS devolvió la dirección en un formato erróneo */
    GetErr("[CTCP] ERROR LocalConn()->ResolvAddr() (%s:%i)", (addr)?addr:"", port);
    close(sock);
    return CTCP_ADDRESS_NOT_FOUND;
  }
#ifdef DEBUG_TCP
    syslog(LOG_INFO, "[CTcp::LocalConn(addr = %s, port = %i)] - ResolvAddr ok", addr, port);
#endif
  localLen = sizeof(struct sockaddr_in);
  if(bind(sock, (struct sockaddr*)&local, localLen) < 0)
  {
#ifdef DEBUG_TCP
    syslog(LOG_INFO, "[CTcp::LocalConn(addr = %s, port = %i)] - bind error", addr, port);
#endif
    /* error al darle nombre al socket */
    GetErr("[CTCP] ERROR LocalConn()->bind() (%s:%i)", (addr)?addr:"", port);
    close(sock);
    return CTCP_HOST_NOT_FOUND;
  }
#ifdef DEBUG_TCP
    syslog(LOG_INFO, "[CTcp::LocalConn(addr = %s, port = %i)] - Ok ok", addr, port);
#endif
  return sock;
}

int CTcp::RemoteConn(const char *addr, int port, int to_ms)
{
  struct sockaddr_in remote;
  struct timeval tv;
  int fcntl_flags;
  fd_set fd_wr, fd_err;

  if(!addr || !port)
  {
    /* no se puede establecer una conexion remota sin estos datos */
    GetErr("[CTCP] ERROR RemoteConn()->[param error] (%s:%i)",
      (addr)?addr:"", port);
    return CTCP_INVALID_ADDRESS;
  }

#ifdef DEBUG_TCP
  syslog(LOG_INFO, "[CTcp::RemoteConn(addr = %s, port = %i)]", addr, port);
#endif
#ifdef SOPORTE_SSL_TLS
  if(m_ssl_tls < 0)
  {
#ifdef DEBUG_TCP
    syslog(LOG_INFO, "[CTcp::RemoteConn(addr = %s, port = %i)] - TLS lib error", addr, port);
#endif
    GetErr("[SSL/TLS] Library Error.");
    return CTCP_SSL_LIB_ERROR;
  }
  if(m_ssl_tls > 0)
  {
    m_p_method = TLS_client_method();
    if ( (m_p_ctx = SSL_CTX_new(m_p_method)) == NULL)
    {
#ifdef DEBUG_TCP
      syslog(LOG_INFO, "[CTcp::RemoteConn(addr = %s, port = %i)] - SSL_CTX_new error", addr, port);
#endif
      GetErr("[SSL/TLS] Unable to create a new SSL context structure..");
      return CTCP_SSL_LIB_ERROR;
    }
    if(m_ssl_tls_ver >= 3)
    {
      SSL_CTX_set_options(m_p_ctx, SSL_OP_NO_SSLv2);
    }
    m_p_ssl = SSL_new(m_p_ctx);
  }
#endif /* SOPORTE_SSL_TLS */

  /* conecto el socket a la red */
  memset(&(remote.sin_zero), 0, sizeof(remote.sin_zero));
  remote.sin_family = AF_INET;
  remote.sin_port = htons(port);
  if((remote.sin_addr.s_addr = ResolvAddr(addr)) == INADDR_NONE)
  {
#ifdef DEBUG_TCP
    syslog(LOG_INFO, "[CTcp::RemoteConn(addr = %s, port = %i)] - ResolvAddr error", addr, port);
#endif
    /* el DNS devolvi� la direcci�n en un formato err�neo */
    GetErr("[CTCP] ERROR RemoteConn()->ResolvAddr() (%s:%i)", (addr)?addr:"", port);
    return CTCP_ADDRESS_NOT_FOUND;
  }
#ifdef DEBUG_TCP
  syslog(LOG_INFO, "[CTcp::RemoteConn(addr = %s, port = %i)] - connect", addr, port);
#endif

  fcntl_flags = fcntl(m_sock, F_GETFL);
  /* pongo el socket no bloqueante */
  fcntl(m_sock, F_SETFL, fcntl_flags|O_NONBLOCK);
  /* me conecto con el server TCP */
  connect(m_sock, (struct sockaddr*)&remote, sizeof(struct sockaddr_in));
  /* lo vuelvo a poner bloqueante bloqueante */
  fcntl(m_sock, F_SETFL, fcntl_flags & ~O_NONBLOCK);
  /* Seteo el timeout en el socket */
  FD_ZERO(&fd_wr);
  FD_ZERO(&fd_err);
  FD_SET(m_sock, &fd_wr);
  FD_SET(m_sock, &fd_err);

  /* Seteo la estructura para el time-out */
  if(to_ms >= 1000)
  {
    tv.tv_sec = to_ms / 1000;
    tv.tv_usec = 0;
  }
  else
  {
    tv.tv_sec = 0;
    tv.tv_usec = to_ms * 1000;
  }
  /* hago un select para bloquear hasta que el socket esté disponible o time-out */
  select(0,NULL,&fd_wr,&fd_err,&tv);			
  if( !FD_ISSET(m_sock, &fd_wr)) 
  {	
#ifdef DEBUG_TCP
    syslog(LOG_INFO, "[CTcp::RemoteConn(addr = %s, port = %i)] - select error", addr, port);
#endif
    GetErr("[CTCP] ERROR RemoteConn()->select() (%s:%i)", (addr)?addr:"", port);
    return CTCP_HOST_NOT_FOUND;
  }

#ifdef SOPORTE_SSL_TLS
  if(m_ssl_tls > 0)
  {
    SSL_set_fd(m_p_ssl, m_sock);
    if( SSL_connect(m_p_ssl) != 1 )
    {
#ifdef DEBUG_TCP
      syslog(LOG_INFO, "[CTcp::RemoteConn(addr = %s, port = %i)] - SSL_connect error", addr, port);
#endif
      GetErr("[SSL/TLS] Could not build a SSL session.");
      return CTCP_SSL_SESSION_ERROR;
    }
  }
#endif /* SOPORTE_SSL_TLS */
#ifdef DEBUG_TCP
      syslog(LOG_INFO, "[CTcp::RemoteConn(addr = %s, port = %i)] - Ok", addr, port);
#endif

  return CTCP_OK;
}

CTcp* CTcp::Listen(const char *addr, int port)
{
  int newsock;
  int fcntl_flags;
#ifdef SOPORTE_SSL_TLS
  SSL *newssl;
#endif

#ifdef DEBUG_TCP
  syslog(LOG_INFO, "[CTcp::Listen(addr = %s, port = %i)] - Ok", addr, port);
#endif
  if(m_sock < 0)
  {
    /* si es el primer listen lo conecto */
    if((m_sock = LocalConn(addr, port)) < CTCP_OK)
    {
          return NULL;
    }
  }

#ifdef SOPORTE_SSL_TLS
  if(m_ssl_tls < 0)
  {
#ifdef DEBUG_TCP
    syslog(LOG_INFO, "[CTcp::Listen(addr = %s, port = %i)] - TLS lib error", addr, port);
#endif
    GetErr("[SSL/TLS] Library Error.");
    return NULL;
  }
  if(m_ssl_tls > 0)
  {
    m_p_method = TLS_server_method();
    if ( (m_p_ctx = SSL_CTX_new(m_p_method)) == NULL)
    {
#ifdef DEBUG_TCP
    syslog(LOG_INFO, "[CTcp::Listen(addr = %s, port = %i)] - SSL_CTX_new error", addr, port);
#endif
      GetErr("[SSL/TLS] Unable to create a new SSL context structure..");
      return NULL;
    }
    if(m_ssl_tls_ver >= 3)
    {
      SSL_CTX_set_options(m_p_ctx, SSL_OP_NO_SSLv2);
    }
  }

  if(m_cert_pem.length() && m_key_pem.length())
  {
    if(SSL_CTX_use_certificate_file(m_p_ctx, m_cert_pem.c_str(), SSL_FILETYPE_PEM) <= 0)
    {
#ifdef DEBUG_TCP
      syslog(LOG_INFO, "[CTcp::Listen(addr = %s, port = %i)] - SSL_CTX_use_certificate_file error", addr, port);
#endif
      GetErr("[SSL/TLS] Unable to load cert PEM.");
      return NULL;
    }
    if(SSL_CTX_use_PrivateKey_file(m_p_ctx, m_key_pem.c_str(), SSL_FILETYPE_PEM) <= 0)
    {
#ifdef DEBUG_TCP
      syslog(LOG_INFO, "[CTcp::Listen(addr = %s, port = %i)] - SSL_CTX_use_PrivateKey_file error", addr, port);
#endif
      GetErr("[SSL/TLS] Unable to load key PEM.");
      return NULL;
    }
  }

#endif /* SOPORTE_SSL_TLS */

  if(listen(m_sock, SOMAXCONN) < 0)
  {
#ifdef DEBUG_TCP
    syslog(LOG_INFO, "[CTcp::Listen(addr = %s, port = %i)] - listen error", addr, port);
#endif
    GetErr("[CTCP] ERROR Listen()->listen() (%s:%i)", (addr)?addr:"", port);
    return NULL;
  }
  if((newsock = accept(m_sock, 0, 0)) < 0)
  {
#ifdef DEBUG_TCP
    syslog(LOG_INFO, "[CTcp::Listen(addr = %s, port = %i)] - accept error", addr, port);
#endif
    GetErr("[CTCP] ERROR Listen()->accept() (%s:%i)", (addr)?addr:"", port);
    return NULL;
  }
#ifdef SOPORTE_SSL_TLS
  if(m_ssl_tls > 0)
  {
    newssl = SSL_new(m_p_ctx);
    SSL_set_fd(newssl, newsock);
    if(SSL_accept(newssl) <= 0)
    {
#ifdef DEBUG_TCP
      syslog(LOG_INFO, "[CTcp::Listen(addr = %s, port = %i)] - SSL_accept error", addr, port);
#endif
      SSL_shutdown(newssl);
      SSL_free(newssl);
      GetErr("[SSL/TLS] ERROR Listen()->accept() (%s:%i)", (addr)?addr:"", port);
      return NULL;
    }
    fcntl_flags = fcntl(newsock, F_GETFL);
    /* pongo el socket no bloqueante */
    fcntl(newsock, F_SETFL, fcntl_flags|O_NONBLOCK);
    return new CTcp(newsock, newssl, m_ssl_tls_ver);
  }
  else
  {
#endif /* SOPORTE_SSL_TLS */
    fcntl_flags = fcntl(newsock, F_GETFL);
    /* pongo el socket no bloqueante */
    fcntl(newsock, F_SETFL, fcntl_flags|O_NONBLOCK);
    return new CTcp(newsock);
#ifdef SOPORTE_SSL_TLS
  }
#endif /* SOPORTE_SSL_TLS */

}

CTcp* CTcp::Listen(string saddr, int port)
{
  return Listen(saddr.c_str(), port);
}

int CTcp::Connect(const char* addrloc, const char *addrrem, int port)
{
  int rc;
  int fcntl_flags;

#ifdef DEBUG_TCP
  syslog(LOG_INFO, "[CTcp::Connect(addrloc = %s, addrrem = %s, port = %i)]", addrloc, addrrem, port);
#endif
  if(m_sock >= 0)
  {
#ifdef DEBUG_TCP
    syslog(LOG_INFO, "[CTcp::Connect(addrloc = %s, addrrem = %s, port = %i)] - ya esta conectado", addrloc, addrrem, port);
#endif
    /* ya esta conectado */
    return CTCP_OK;
  }
  /* lo conecto localmente */
  if((rc = LocalConn(addrloc, 0)) < CTCP_OK)
  {
#ifdef DEBUG_TCP
    syslog(LOG_INFO, "[CTcp::Connect(addrloc = %s, addrrem = %s, port = %i)] - LocalConn error", addrloc, addrrem, port);
#endif
    return rc;
  }
  m_sock = rc;
  /* lo conecto al server */
  if((rc = RemoteConn(addrrem, port, 5)) != CTCP_OK)
  {
#ifdef DEBUG_TCP
    syslog(LOG_INFO, "[CTcp::Connect(addrloc = %s, addrrem = %s, port = %i)] - RemotelConn error", addrloc, addrrem, port);
#endif
    return rc;
  }
#ifdef DEBUG_TCP
  syslog(LOG_INFO, "[CTcp::Connect(addrloc = %s, addrrem = %s, port = %i)] - OK", addrloc, addrrem, port);
#endif
  fcntl_flags = fcntl(m_sock, F_GETFL);
  /* pongo el socket no bloqueante */
  fcntl(m_sock, F_SETFL, fcntl_flags|O_NONBLOCK);
  return CTCP_OK;
}

int CTcp::Connect(string saddrloc, string saddrrem, int port)
{
  return Connect(saddrloc.c_str(), saddrrem.c_str(), port);
}

int CTcp::LocalPort()
{
    struct sockaddr_in local;
    int localLen;

#ifdef DEBUG_TCP
    syslog(LOG_INFO, "[CTcp::LocalPort()]");
#endif
    /* obtengo el port al que esta conectado el socket */
    localLen = sizeof(struct sockaddr_in);
    if(getsockname(m_sock, (struct sockaddr*)&local, (socklen_t*)&localLen) == 0)
    {
      return ntohs(local.sin_port);
    }
    return 0;
}

int CTcp::RemotePort()
{
  struct sockaddr_in local;
  int localLen;

#ifdef DEBUG_TCP
    syslog(LOG_INFO, "[CTcp::RemotePort()]");
#endif
  /* obtengo el port al que esta conectado el socket */
  localLen = sizeof(struct sockaddr_in);
  if(getpeername(m_sock, (struct sockaddr*)&local, (socklen_t*)&localLen) == 0)
  {
    return ntohs(local.sin_port);
  }
  return 0;
}

char* CTcp::LocalAddress()
{
  struct sockaddr_in local;
  int localLen;

#ifdef DEBUG_TCP
    syslog(LOG_INFO, "[CTcp::LocalAddress()]");
#endif
  /* obtengo el port al que esta conectado el socket */
  localLen = sizeof(struct sockaddr_in);
  if(getsockname(m_sock, (struct sockaddr*)&local, (socklen_t*)&localLen) == 0)
  {
    sprintf(m_strLocalAddress, "%s",inet_ntoa(local.sin_addr));
    return (char*)&m_strLocalAddress;
  }
  return NULL;
}

char* CTcp::RemoteAddress()
{
  struct sockaddr_in remote;
  int remoteLen;

#ifdef DEBUG_TCP
    syslog(LOG_INFO, "[CTcp::RemoteAddress()]");
#endif
  /* obtengo el port al que esta conectado el socket */
  remoteLen = sizeof(struct sockaddr_in);
  if(getpeername(m_sock, (struct sockaddr*)&remote, (socklen_t*)&remoteLen) == 0)
  {
    sprintf(m_strRemoteAddress, "%s",inet_ntoa(remote.sin_addr));
    return (char*)&m_strRemoteAddress;
  }
  return NULL;
}

int CTcp::Close()
{

#ifdef DEBUG_TCP
    syslog(LOG_INFO, "[CTcp::Close()]");
#endif
#ifdef SOPORTE_SSL_TLS
  if(m_p_ssl)
  {
    SSL_free(m_p_ssl);
    m_p_ssl = NULL;
  }
  if(m_p_ctx)
  {
    SSL_CTX_free(m_p_ctx);
    m_p_ctx = NULL;
  }
#endif

  if(m_sock >= 0)
  {
    shutdown(m_sock, 2);
    Kill();
  }
  return CTCP_OK;
}

int CTcp::Kill()
{
#ifdef DEBUG_TCP
    syslog(LOG_INFO, "[CTcp::Kill()]");
#endif
  if(m_sock >= 0)
  {
    if(close(m_sock) < 0)
    {
      close(m_sock);
      close(m_sock);
      close(m_sock);
      /* por alguna razon no se puede cerrar el socket */
      GetErr("[CTCP] ERROR Kill()->close()");
    }
    m_sock = (-1);
  }
  return CTCP_OK;
}

int CTcp::Send(const void *msg, unsigned int msglen)
{
  struct pollfd pfd[1];
  int rc;

#ifdef DEBUG_TCP
    syslog(LOG_INFO, "[CTcp::Send()]");
#endif
  if(m_sock < 0)
  {
    /* el socket no esta abierto */
    GetErr("[CTCP] ERROR Send() El socket no esta conectado");
    return CTCP_NOT_CONNECTED;
  }
#ifdef SOPORTE_SSL_TLS
  if( m_ssl_tls > 0 && !m_p_ssl )
  {
    /* el socket no esta abierto */
    GetErr("[SSL/TLS] ERROR Send() El socket no esta conectado");
    return CTCP_NOT_CONNECTED;
  }
#endif /* SOPORTE_SSL_TLS */

#ifdef SOPORTE_SSL_TLS
  if(m_ssl_tls > 0)
  {
    if(SSL_write(m_p_ssl, (char*)msg, msglen) < 0)
    {
      GetErr("[SSL/TLS] ERROR Send()->send()");
      return CTCP_WRITE_ERROR;
    }
  }
  else
  {
#endif /* SOPORTE_SSL_TLS */

  /* para enviar tranquilo espero a que el socket esta dispnible */
  pfd[0].fd = m_sock;
  pfd[0].events = POLLOUT;
  if((rc = poll(pfd, 1, 1000)) < 0)
  {
    GetErr("[CTCP] ERROR Send()->poll()");
    return CTCP_READ_ERROR;
  }
  if(rc == 0 || (pfd[0].revents & POLLERR))
  {
    GetErr("[CTCP] ERROR Send() El socket estaba bloqueado para envio");
    return CTCP_WRITE_ERROR;
  }
  if(send(m_sock, (char*)msg, msglen, MSG_CONFIRM) < 0)
  {
    GetErr("[CTCP] ERROR Send()->send()");
    return CTCP_WRITE_ERROR;
  }

#ifdef SOPORTE_SSL_TLS
  }
#endif /* SOPORTE_SSL_TLS */

  return 0;
}

int CTcp::Receive(void *msg, unsigned int msglen, int to_ms)
{
  int recvlen;
  int rc;
  struct pollfd pfd[1];
  int to;

#ifdef DEBUG_TCP
    syslog(LOG_INFO, "[CTcp::Receive()]");
#endif
  if(m_sock < 0)
  {
    /* el socket no esta abierto */
    GetErr("[CTCP] ERROR Receive() El socket no esta conectado");
    return CTCP_NOT_CONNECTED;
  }
#ifdef SOPORTE_SSL_TLS
  if( m_ssl_tls > 0 && !m_p_ssl )
  {
    /* el socket no esta abierto */
    GetErr("[SSL/TLS] ERROR Send() El socket no esta conectado");
    return CTCP_NOT_CONNECTED;
  }

  if(m_ssl_tls > 0)
  {
    recvlen = 0;
    rc = 0;
    to = to_ms;

    do
    {
      rc = SSL_read(m_p_ssl, (char*)((char*)msg+recvlen), (msglen-recvlen) );
      if(rc < 0) break;
      if(rc == 0)
      {
        usleep(1000);
        to--;
      }
      else if(rc > 0)
      {
        recvlen += rc;
        to = to_ms;
      }
    } while(recvlen < (int)msglen && to);
  }
  else
  {
#endif /* SOPORTE_SSL_TLS */
  pfd[0].fd = m_sock;
  pfd[0].events = POLLIN;
  if((rc = poll(pfd, 1, to_ms)) < 0)
  {
    GetErr("[CTCP] ERROR Receive()->poll()");
    return CTCP_READ_ERROR;
  }
  /* verifico time-out */
  if(rc == 0)
  {
    if(to_ms > 0) GetErr("[CTCP] TIMEOUT Receive()->poll()");
    return 0;
  }
  else if(pfd[0].revents & POLLERR)
  {
    GetErr("[CTCP] ERROR Receive()->poll() Error en el socket");
    return CTCP_READ_ERROR;
  }
  else if(pfd[0].revents & POLLIN)
  {
    if((recvlen = recv(m_sock, (char*)msg, msglen, 0)) < 0)
    {
      GetErr("[CTCP] ERROR Receive()->recv()");
      if((m_iLastError) && (m_iLastError != 11))
      {
        /* error de recepcion */
        return CTCP_READ_ERROR;
      }
    }
  }
  else
  {
    /* se recibi� con tama�o 0 */
    GetErr("[CTCP] ERROR Receive()->recv() rcvlen = 0");
    return CTCP_READ_ERROR;
  }
#ifdef SOPORTE_SSL_TLS
  }
#endif /* SOPORTE_SSL_TLS */

  return recvlen;
}

void CTcp::GetErr(const char *msg, ...)
{
  va_list vl;

  m_iLastError = errno;
  va_start(vl, msg);
  vsprintf(m_strError, msg, vl);
  va_end(vl);
}

char* CTcp::GetErrorText()
{
  strcpy(m_strLastError, m_strError);
  if(m_iLastError)
  {
    strcat(m_strLastError, " - ");
    strcat(m_strLastError, strerror(m_iLastError));
  }
  return (char*)&m_strLastError;
}

int CTcp::GetErrorNumber()
{
  return m_iLastError;
}

int CTcp::GetFD()
{
  return m_sock;
}

#ifdef SOPORTE_SSL_TLS
SSL* CTcp::GetSSL()
{
  return m_p_ssl;
}
#endif

int CTcp::Query(const char* raddr, int rport, const char* snd, char* rcv, int rcv_max_len, int to_ms)
{
  int rc = 0; 

#ifdef DEBUG_TCP
  syslog(LOG_INFO, "[CTcp::Query()]");
#endif
  this->Connect(NULL, raddr, rport);
  if(rc != 0) 
  {
      return (-1);
  }

  rc = this->Send(snd, strlen(snd));
  if(rc != 0) 
  {
      return (-1);
  }

  rc = this->Receive(rcv, rcv_max_len, to_ms);
  *((char*)(rcv + rc)) = 0;

  return rc;
}

#ifdef ___COMMENT___

  /* ---------------------------------------------------------- *
   * Get the remote certificate into the X509 structure         *
   * ---------------------------------------------------------- */
  cert = SSL_get_peer_certificate(ssl);
  if (cert == NULL)
    BIO_printf(outbio, "Error: Could not get a certificate from: %s.\n", dest_url);
  else
    BIO_printf(outbio, "Retrieved the server's certificate from: %s.\n", dest_url);

  /* ---------------------------------------------------------- *
   * extract various certificate information                    *
   * -----------------------------------------------------------*/
  certname = X509_NAME_new();
  certname = X509_get_subject_name(cert);

  /* ---------------------------------------------------------- *
   * display the cert subject here                              *
   * -----------------------------------------------------------*/
  BIO_printf(outbio, "Displaying the certificate subject data:\n");
  X509_NAME_print_ex(outbio, certname, 0, 0);
  BIO_printf(outbio, "\n");

  /* ---------------------------------------------------------- *
   * Free the structures we don't need anymore                  *
   * -----------------------------------------------------------*/
  SSL_free(ssl);
  close(server);
  X509_free(cert);
  SSL_CTX_free(ctx);
  BIO_printf(outbio, "Finished SSL/TLS connection with server: %s.\n", dest_url);

#endif
