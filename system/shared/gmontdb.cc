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
    Implementacion de lectura y escriura en la configuracion de los servidores
    Datos de las colas
    Procedimientos
*/
#include <gmontdb.h>
#include <gmstring.h>

#include <iostream>
#include <cstdio>
using namespace std;

#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

#define SAVE_RETRY 10

#define INDEX_SERVER( a ) (sizeof(CSystemInfo) + ( a * sizeof(SH_SERVER)))
#define INDEX_FUNCTION( a ) (sizeof(CSystemInfo) + ( a * sizeof(SH_FUNCION)) + (m_max_servers * sizeof(SH_SERVER) ))

CGMTdb::CGMTdb(string& cfgpath, int max_servers, int max_services, CGLog* plog)
{
  m_config_path = cfgpath;
  m_max_servers = max_servers;
  m_max_services = max_services;
  m_pLog = plog;
  m_pShMem = NULL;
}

CGMTdb::CGMTdb(const char* cfgpath, int max_servers, int max_services, CGLog* plog)
{
  if(cfgpath) m_config_path = cfgpath;
  else m_config_path = ".";
  m_max_servers = max_servers;
  m_max_services = max_services;
  m_pLog = plog;
  m_pShMem = NULL;
}

CGMTdb::~CGMTdb()
{
  Close();
}

/*
  Crea e inicializa el area de memoria global para mantener la configuracion.
  Lo deber�a llamar solamente el proceso server de configuraci�n.
*/
int CGMTdb::Create(CGLog* plog)
{
  m_pShMem = new CSincMem(GM_CONFIG_KEY);
  /* creo un espacio de memoria suficientemente
  grande para contener todos los servers y todos los servicios */
  if(m_pShMem->Create(  sizeof(CSystemInfo) +
                        (sizeof(SH_SERVER)*m_max_servers) +
                        (sizeof(SH_FUNCION)*m_max_services)
                      ) != 0) return -1;
  if(plog && !m_pLog) m_pLog = plog;
  if(m_pLog) m_pLog->Add(1, "Area de configuración creada para %i Servers y %i Servicios", m_max_servers, m_max_services);
  return Load();
}

int CGMTdb::Reload()
{
  if(m_pShMem->IsOwner())
  {
    return LoadSrv();
  }
  return -1;
}

/*
  Se agarra al area de memoria ya creada es para utilizar en los demas servers.
*/
int CGMTdb::Open(CGLog* plog)
{
  m_pShMem = new CSincMem(GM_CONFIG_KEY);
  /* creo un espacio de memoria suficientemente
  grande para contener todos los servers y todos los servicios */
  if(m_pShMem->Open(  sizeof(CSystemInfo) +
                      (sizeof(SH_SERVER)*m_max_servers) +
                      (sizeof(SH_FUNCION)*m_max_services)
                    ) != 0) return -1;
  if(plog && !m_pLog) m_pLog = plog;
  return 0;
}

void CGMTdb::Close()
{
  if(m_pShMem)
  {
    m_pShMem->Close();
    delete m_pShMem;
    m_pShMem = NULL;
  }
}

/* busca el server por el nombre y llena el resto de la estructura */
int CGMTdb::Server(CSrvTab& s)
{
  int i;
  SH_SERVER server;

  for(i = 0; i < m_max_servers; i++)
  {
    if(m_pShMem->GetAt(INDEX_SERVER(i), &server, sizeof(SH_SERVER)) != 0)
    {
      if(m_pLog)
      {
        m_pLog->Add(1, "ERROR en acceso de lectura a memoria compartida en CGMTdb::Server(CSrvTab)");
      }
      return -1;
    }
    if(strlen(server.nombre))
    {
      if(m_pLog)
      {
        m_pLog->Add(50, "Buscando %s -> Indice: %i Server: %s",
          s.nombre.c_str(), i, server.nombre);
      }
      if(s.nombre == server.nombre)
      {
        if(m_pLog)
        {
          m_pLog->Add(50, "%s encontrado -> Indice: %i", s.nombre.c_str(), i);
        }
        s.descripcion = server.descripcion;
        s.modo = server.modo;
        s.path = server.path;
        return 0;
      }
    }
  }
  return -1;
}


/* devuelve la lista de parametros para el server */
/*
int CGMTdb::SrvParams(string srv, char** param_list)
{
  param_list[j] = (char*)malloc( m_vServerParametro[i].parametro.length() + 1 );
  strcpy(param_list[j++], m_vServerParametro[i].parametro.c_str());
  if(m_vServerParametro[i].valor.length())
  {
    param_list[j] = (char*)malloc( m_vServerParametro[i].valor.length() + 1 );
    strcpy(param_list[j++], m_vServerParametro[i].valor.c_str());
  }
  return 0;
}
*/

/* devuelve la lista de parametros para el server */
/*
int CGMTdb::SrvParams(const char* srv, char** param_list)
{
  return SrvParams(string(srv), param_list);
}
*/

/* devuelve la lista de parametros del servicio */
/*
int CGMTdb::FcnParams(string fcn, char** param_list)
{
  param_list[j] = (char*)malloc( m_vFuncionParametro[i].parametro.length() + 1 );
  strcpy(param_list[j++], m_vFuncionParametro[i].parametro.c_str());
  if(m_vFuncionParametro[i].valor.length())
  {
    param_list[j] = (char*)malloc( m_vFuncionParametro[i].valor.length() + 1 );
    strcpy(param_list[j++], m_vFuncionParametro[i].valor.c_str());
  }
  return 0;
}
*/
/*
int CGMTdb::FcnParams(const char* fcn, char** param_list)
{
  return FcnParams(string(fcn), param_list);
}
*/

vector <int> CGMTdb::Cola(string& servicio, char tipo_mensaje)
{
  return Cola(servicio.c_str(), tipo_mensaje);
}

vector <int> CGMTdb::Cola(const char* servicio, char tipo_mensaje)
{
  int i, j, k;
  vector <int> cola_list;
  SH_FUNCION funcion;
  SH_SERVER server;

  cola_list.clear();
  /* busco los servicios para obtener los servers */
  for(i = 0; i < m_max_services; i++)
  {
    if(m_pShMem->GetAt(INDEX_FUNCTION(i), &funcion, sizeof(SH_FUNCION)) != 0)
    {
      if(m_pLog)
      {
        m_pLog->Add(1, "ERROR en acceso de lectura a memoria compartida en INDEX_FUNCTION CGMTdb::Cola");
      }
      return cola_list;
    }
    if(strlen(funcion.nombre))
    {
      if( !strcmp(funcion.nombre, servicio) && funcion.tipo_mensaje == tipo_mensaje )
      {
        if(m_pLog)
        {
          m_pLog->Add(50, "Servicio %s(%c) encontrado en server %s",
            funcion.nombre, funcion.tipo_mensaje, funcion.server);
        }
        /* cada uno que encuentro busco el server para obtener las colas */
        for(j = 0; j < m_max_servers; j++)
        {
          if(m_pShMem->GetAt(INDEX_SERVER(j), &server, sizeof(SH_SERVER)) != 0)
          {
            if(m_pLog)
            {
              m_pLog->Add(1, "ERROR en acceso de lectura a memoria "
                "compartida en INDEX_SERVER CGMTdb::Cola");
            }
            return cola_list;
          }
          if(strlen(server.nombre))
          {
            if( !strcmp(funcion.server, server.nombre))
            {
              /* por cada server que atienda al servicio me cargo las colas */
              for(k = 0; k < MAX_SERVER_INSTANCES; k++)
              {
                if(server.cola[k] > 0)
                {
                  if(m_pLog)
                  {
                    m_pLog->Add(50, "    Cola 0x%08X",
                      server.cola[k]);
                  }
                  cola_list.push_back(server.cola[k]);
                }
              }
            }
          }
        }
      }
    }
  }
  return cola_list;
}

int CGMTdb::AddSrv(string& server, int pid, int cola, int indice)
{
  return AddSrv(server.c_str(), pid, cola, indice);
}

int CGMTdb::RemoveSrv(string& server, int indice)
{
  return RemoveSrv(server.c_str(), indice);
}

/* Registra la cola de un server generada con un objeto CMsg */
/* el parametro cola es la clave y se obtiene con GetKey  */
/* el parametro indice es para abrir varia colas de un mismo proceso y se obtiene con GetIndex */
int CGMTdb::AddSrv(const char* nombre_server, int pid, int cola, int indice)
{
  int i;
  SH_SERVER server;

  if(indice < 0 || indice > 255) return -1;

  for(i = 0; i < m_max_servers; i++)
  {
    if(m_pShMem->GetAt(INDEX_SERVER(i), &server, sizeof(SH_SERVER)) != 0)
    {
      if(m_pLog) m_pLog->Add(1, "ERROR en acceso de lectura a memoria compartida en CGMTdb::AddSrv");
      return -1;
    }
    if(strlen(server.nombre))
    {
      if( !strcmp(nombre_server, server.nombre) )
      {
        server.cola[indice] = cola;
        server.pid[indice] = pid;
        if(m_pShMem->SetAt(INDEX_SERVER(i), &server, sizeof(SH_SERVER)) != 0)
        {
          if(m_pLog) m_pLog->Add(1, "ERROR en acceso de escritura a memoria compartida en CGMTdb::AddSrv");
          return -1;
        }
        else
        {
          return 0 ;
        }
      }
    }
  }
  if(m_pLog) m_pLog->Add(1, "ERROR se alcanzo el limite de servidores en CGMTdb::AddSrv");
  return (-1);
}

int CGMTdb::RemoveSrv(const char* nombre_server, int indice)
{
  int i;
  SH_SERVER server;

  if(indice < 0 || indice > 255) return -1;

  for(i = 0; i < m_max_servers; i++)
  {
    if(m_pShMem->GetAt(INDEX_SERVER(i), &server, sizeof(SH_SERVER)) != 0)
    {
      if(m_pLog) m_pLog->Add(1, "ERROR en acceso de lectura a memoria compartida en CGMTdb::RemoveSrv");
      return -1;
    }
    if(strlen(server.nombre))
    {
      if( !strcmp(nombre_server, server.nombre) )
      {
        server.cola[indice] = 0;
        server.pid[indice] = 0;
        if(m_pShMem->SetAt(INDEX_SERVER(i), &server, sizeof(SH_SERVER)) != 0)
        {
          if(m_pLog) m_pLog->Add(1, "ERROR en acceso de escritura a memoria compartida en CGMTdb::RemoveSrv");
          return -1;
        }
        else
        {
          return 0 ;
        }
      }
    }
  }
  if(m_pLog) m_pLog->Add(1, "ERROR se alcanzo el limite de servidores en CGMTdb::AddSrv");
  return (-1);
}

/* Agrego un relacion server/servicio */
int CGMTdb::AddSvc(const char* servicio, char tipo_mensaje, const char* server)
{
  CFcnTab t;

  t.nombre = servicio;
  t.tipo_mensaje = tipo_mensaje;
  t.server = server;
  return Add(t);
}

/* Agrego un relacion server/servicio */
int CGMTdb::AddSvc(string& servicio, char tipo_mensaje, string& server)
{
  return AddSvc(servicio.c_str(), tipo_mensaje, server.c_str());
}

/* Elimino un relacion server/servicio */
int CGMTdb::RemoveSvc(const char* servicio, char tipo_mensaje, const char* server)
{
  CFcnTab t;

  t.nombre = servicio;
  t.tipo_mensaje = tipo_mensaje;
  t.server = server;
  return Remove(t);
}

/* Elimino un relacion server/servicio */
int CGMTdb::RemoveSvc(string& servicio, char tipo_mensaje, string& server)
{
  return RemoveSvc(servicio.c_str(), tipo_mensaje, server.c_str());
}

int CGMTdb::Load()
{
  int rc;

  rc = LoadSrv();
  if(rc) return rc;
  rc = LoadSrvPar();
  if(rc) return rc;
  rc = LoadFcn();
  if(rc) return rc;
  rc = LoadFcnPar();
  if(rc) return rc;
  return 0;
}

int CGMTdb::LoadSrv()
{
  CSrvTab t;
  ifstream in;
  string s;
  vector<string> vs;

  in.open((m_config_path + "/" + GM_FILENAME_SERVER_DB).c_str());
  if(! in.is_open())
  {
    return -1;
  }
  if(m_pLog)
  {
    m_pLog->Add(50, "CGMTdb::LoadSrv");
  }
  while(getline(in, s))
  {
    if(s.length() > 0 && s[0] != '#') /* salteo comentarios y lineas vacias */
    {
      vs = split(s, '|');
      if(vs.size() < 4)
      {
        in.close();
        m_pLog->Add(1, "ERROR al parsear configuracion de server");
        return -1;
      }
      t.nombre = vs[0];
      t.descripcion = vs[1];
      t.modo = atoi(vs[2].c_str());
      t.path = vs[3];
      if(m_pLog)
      {
        m_pLog->Add(50, "Nombre:      %s", t.nombre.c_str());
        m_pLog->Add(50, "Descripcion: %s", t.descripcion.c_str());
        m_pLog->Add(50, "Modo:        %i", t.modo);
        m_pLog->Add(50, "Path:        %s", t.path.c_str());
      }
      if(Add(t) != 0 && m_pLog)
      {
        m_pLog->Add(1, "ERROR al cargar servers en memoria");
        m_pLog->Add(1, "ERROR Nombre:      %s", t.nombre.c_str());
        m_pLog->Add(1, "ERROR Descripcion: %s", t.descripcion.c_str());
        m_pLog->Add(1, "ERROR Modo:        %i", t.modo);
        m_pLog->Add(1, "ERROR Path:        %s", t.path.c_str());
      }
      if(m_pLog)
      {
        m_pLog->Add(50, "------------------------------------------------------------");
      }
    }
  }
  in.close();
  return 0;
}

int CGMTdb::LoadSrvPar()
{
  CSrvParTab t;
  ifstream in;
  string s;
  vector<string> vs;

  in.open((m_config_path + "/" + GM_FILENAME_SERVER_PARAMETRO_DB).c_str());
  if(! in.is_open())
  {
    return -1;
  }
  if(m_pLog)
  {
    m_pLog->Add(50, "CGMTdb::LoadSrvPar");
  }
  while(getline(in, s))
  {
    if(s.length() > 0 && s[0] != '#') /* salteo comentarios y lineas vacias */
    {
      vs = split(s, '|');
      if(vs.size() != 3)
      {
        in.close();
        return -1;
      }
      t.server = vs[0];
      t.parametro = vs[1];
      t.valor = vs[2];
      if(m_pLog)
      {
        m_pLog->Add(50, "Server:    %s", t.server.c_str());
        m_pLog->Add(50, "Parametro: %s", t.parametro.c_str());
        m_pLog->Add(50, "Valor:     %s", t.valor.c_str());
      }
      m_pLog->Add(1, "INFO **** Falta cargar parametros de servers en memoria****");
      /*
      if(Add(t) != 0 && m_pLog)
      {
        m_pLog->Add(1, "ERROR al cargar parametros de servers en memoria");
        m_pLog->Add(1, "ERROR Server:    %s", t.server.c_str());
        m_pLog->Add(1, "ERROR Parametro: %s", t.parametro.c_str());
        m_pLog->Add(1, "ERROR Valor:     %s", t.valor.c_str());
      }
      */
      if(m_pLog)
      {
        m_pLog->Add(50, "------------------------------------------------------------");
      }
    }
  }
  in.close();

  return 0;
}

int CGMTdb::LoadFcn()
{
  CFcnTab t;
  ifstream in;
  string s;
  vector<string> vs;

  in.open((m_config_path + "/" + GM_FILENAME_FUNCION_DB).c_str());
  if(! in.is_open())
  {
    return -1;
  }
  if(m_pLog)
  {
    m_pLog->Add(50, "CGMTdb::LoadFcn");
  }
  while(getline(in, s))
  {
    if(s.length() > 0 && s[0] != '#') /* salteo comentarios y lineas vacias */
    {
      vs = split(s, '|');
      if(vs.size() != 5)
      {
        in.close();
        return -1;
      }
      t.nombre = vs[0];
      t.descripcion = vs[1];
      t.tipo_mensaje = vs[2].c_str()[0];
      t.server = vs[3];
      if(m_pLog)
      {
        m_pLog->Add(50, "Nombre:      %s", t.nombre.c_str());
        m_pLog->Add(50, "Descripcion: %s", t.descripcion.c_str());
        m_pLog->Add(50, "T Mensaje:   %c", t.tipo_mensaje);
        m_pLog->Add(50, "Server:      %s", t.server.c_str());
      }
      if(Add(t) != 0 && m_pLog)
      {
        m_pLog->Add(1, "ERROR al cargar servicios en memoria");
        m_pLog->Add(1, "ERROR Nombre:      %s", t.nombre.c_str());
        m_pLog->Add(1, "ERROR Descripcion: %s", t.descripcion.c_str());
        m_pLog->Add(1, "ERROR T Mensaje:   %c", t.tipo_mensaje);
        m_pLog->Add(1, "ERROR Server:      %s", t.server.c_str());
      }
      if(m_pLog)
      {
        m_pLog->Add(50, "------------------------------------------------------------");
      }
    }
  }
  return 0;
}

int CGMTdb::LoadFcnPar()
{
  CFcnParTab t;
  ifstream in;
  string s;
  vector<string> vs;

  in.open((m_config_path + "/" + GM_FILENAME_FUNCION_PARAMETRO_DB).c_str());
  if(! in.is_open())
  {
    return -1;
  }
  if(m_pLog)
  {
    m_pLog->Add(50, "CGMTdb::LoadFcnPar");
  }
  while(getline(in, s))
  {
    if(s.length() > 0 && s[0] != '#') /* salteo comentarios y lineas vacias */
    {
      vs = split(s, '|');
      if(vs.size() != 3)
      {
        in.close();
        return -1;
      }
      t.funcion = vs[0];
      t.parametro = vs[1];
      t.valor = vs[2];
      if(m_pLog)
      {
        m_pLog->Add(50, "Server:    %s", t.funcion.c_str());
        m_pLog->Add(50, "Parametro: %s", t.parametro.c_str());
        m_pLog->Add(50, "Valor:     %s", t.valor.c_str());
      }
      m_pLog->Add(1, "INFO **** Falta cargar parametros de servicios en memoria****");
      /*
      if(Add(t) != 0 && m_pLog)
      {
        m_pLog->Add(1, "ERROR al cargar parametros de servicios en memoria");
        m_pLog->Add(1, "ERROR Server:    %s", t.funcion.c_str());
        m_pLog->Add(1, "ERROR Parametro: %s", t.parametro.c_str());
        m_pLog->Add(1, "ERROR Valor:     %s", t.valor.c_str());
      }
      */
      if(m_pLog)
      {
        m_pLog->Add(50, "------------------------------------------------------------");
      }
    }
  }
  in.close();
  return 0;
}

/* Para cuando se implemente completa la configuracion dinamica */
int CGMTdb::SaveSrv()
{
  int i, j;
  ofstream out;
  SH_SERVER server;

  for(i = 0; i < SAVE_RETRY; i++)
  {
    out.open((m_config_path + "/" + GM_FILENAME_SERVER_DB).c_str());
    if(out.is_open())
    {
      for(j = 0; j < m_max_servers; j++)
      {
        if(m_pShMem->GetAt(INDEX_SERVER(j), &server, sizeof(SH_SERVER)) == 0)
        {
          if(strlen(server.nombre))
          {
            out << server.nombre << "|";
            out << server.descripcion  << "|";
            out << server.modo << "|";
            out << server.path << endl;
          }
        }
      }
      out.close();
      break;
    }
    else
    {
      sleep(1); /* si no puedo abrir el archivo espero y reintento */
    }
  }
  return (i < SAVE_RETRY)?0:-1;
}

int CGMTdb::SaveSrvPar()
{

  return 0;
}

/*
int CGMTdb::SaveFcn()
{
  unsigned int i, vlen;
  CFcnParTab t;
  ofstream out;

  vlen = m_vFuncion.size();
  if(vlen > 0)
  {
    out.open((m_config_path + "/" + GM_FILENAME_FUNCION_DB).c_str());
    if(out.is_open())
    {
      out << "# Tabla: funcion.tab" << endl;
      out << "# nombre|descripcion|server" << endl;
      for(i = 0; i < vlen; i++)
      {
        out << m_vFuncion[i].nombre << "|";
        out << m_vFuncion[i].descripcion << "|";
        out << m_vFuncion[i].tipo_mensaje << "|";
        out << m_vFuncion[i].server << "|";
        out << m_vFuncion[i].borrar << endl;
      }
      out.close();
    }
  }
  return 0;
}
*/

/*
int CGMTdb::SaveFcnPar()
{

  return 0;
}
*/

/* Carga de la tabla de servers */
/* Carga en memoria los datos a medida que los levanta del archivo de configuraci�n */
int CGMTdb::Add(CSrvTab t)
{
  int i;
  int idx = m_max_servers;
  SH_SERVER server;
  bool data_update = false;

  /* busco un lugarcito libre y ademas me fijo que no exista de antes */
  for(i = 0; i < m_max_servers; i++)
  {
    if(m_pShMem->GetAt(INDEX_SERVER(i), &server, sizeof(SH_SERVER)) != 0)
    {
      if(m_pLog) m_pLog->Add(1, "ERROR en acceso de lectura a memoria compartida en CGMTdb::Add CSrvTab");
      return -1;
    }
    if(strlen(server.nombre))
    {
      if(t.nombre == server.nombre) /* ya existe */
      {
        idx = i; /* uso este asi reemplazo el de memoria con el nuevo  */
        data_update = true;
        break;
      }
    }
    else if( idx == m_max_servers)
    {
      /* encontr� un lugar y todav�a no habia encontrado ninguno */
      idx = i;
    }
  }
  if(idx < m_max_servers)
  {
    if(data_update)
    {
      /* limpio solamente estas variables para no pisar las colas si existen */
      memset(server.nombre, '\0', sizeof(server.nombre));
      memset(server.descripcion, '\0', sizeof(server.descripcion));
      memset(server.path, '\0', sizeof(server.path));
    }
    else
    {
      /* blanqueo toda la estructura antes de copiar */
      memset(&server, '\0', sizeof(SH_SERVER));
    }
    /* copio los datos del server */
    strncpy(server.nombre, t.nombre.c_str(), sizeof(server.nombre) - 1);
                strncpy(server.descripcion, t.descripcion.c_str(), sizeof(server.descripcion) - 1);
                server.modo = t.modo;
                strncpy(server.path, t.path.c_str(), sizeof(server.path) - 1);
    /* lo plancho en memoria */
    if(m_pShMem->SetAt(INDEX_SERVER(idx), &server, sizeof(SH_SERVER)) != 0)
    {
      if(m_pLog)
      {
        m_pLog->Add(1, "ERROR en acceso de escritura a memoria compartida en CGMTdb::AddSrv CSrvTab");
      }
      return -1;
    }
    else
    {
      return 0 ;
    }
  }
  return -1;
}

/* Carga en memoria los datos a medida que los levanta del archivo de configuraci�n */
/*
int CGMTdb::Add(CSrvParTab t)
{
  if(m_pLog)
  {
    m_pLog->Add(1, "WARNING Funcionalidad no implementada CGMTdb::Add(CSrvParTab t)");
  }
  return -1;
}
*/

/* Carga de l tabla de servicios */
/* Carga en memoria los datos a medida que los levanta del archivo de configuraci�n */
int CGMTdb::Add(CFcnTab t)
{
  int i;
  SH_FUNCION funcion;
  int idx = m_max_services;

  /* busco si el servicio ya est� dado de alta con el mismo nombre, tipo de mensaje y server */
  /* si es as� listo,  */
  for(i = 0; i < m_max_services; i++)
  {
    if(m_pShMem->GetAt(INDEX_FUNCTION(i), &funcion, sizeof(SH_FUNCION)) != 0)
    {
      if(m_pLog) m_pLog->Add(1, "ERROR en acceso de lectura a emoria compartida en CGMTdb::Add CFcnTab");
      return -1;
    }
    if(strlen(funcion.nombre))
    {
      /*
        Para un mismo servicio se pueden dar de alta varias instancias.
        Las instancias pueden ser del mismo server o diferente.
        Tambien puede haber un mismo servicio pero con diferente tipo de mensaje, en
        ese caso se trata como un servicio diferente.
      */
      if( t.nombre == funcion.nombre &&
        t.tipo_mensaje == funcion.tipo_mensaje &&
        t.server == funcion.server)
      {
        /* si es el mismo server incremento el contador de suscripciones */
        funcion.suscripciones++;
        /* lo plancho en memoria */
        if(m_pShMem->SetAt(INDEX_FUNCTION(i), &funcion, sizeof(SH_FUNCION)) != 0)
        {
          if(m_pLog)
          {
            m_pLog->Add(1, "ERROR en acceso de escritura a memoria compartida "
              "en CGMTdb::Add CFcnTab");
          }
          return -1;
        }
        else
        {
          return 0 ;
        }
      }
    }
    else if(idx == m_max_services) /* si encuentro un lugar y todavia no habia encontrado */
    {
      /* me guardo el lugar libre por si termino insertando */
      idx = i;
    }
  }
  if(idx < m_max_services)
  {
    strncpy(funcion.nombre, t.nombre.c_str(), sizeof(funcion.nombre) - 1);
    strncpy(funcion.descripcion, t.descripcion.c_str(), sizeof(funcion.descripcion) - 1);
    funcion.tipo_mensaje = t.tipo_mensaje;
    strncpy(funcion.server, t.server.c_str(), sizeof(funcion.server) - 1);
    funcion.suscripciones = 1;
    /* lo plancho en memoria */
    if(m_pShMem->SetAt(INDEX_FUNCTION(idx), &funcion, sizeof(SH_FUNCION)) != 0)
    {
      if(m_pLog)
      {
        m_pLog->Add(1, "ERROR en acceso de escritura a memoria compartida en CGMTdb::Add CFcnTab");
      }
      return -1;
    }
    else
    {
      return 0 ;
    }
  }
  if(m_pLog) m_pLog->Add(1, "ERROR se alcanzo la maxima cantidad de servicios en CGMTdb::Add CFcnTab");
  return -1;
}

/* Carga en memoria los datos a medida que los levanta del archivo de configuraci�n */
/*
int CGMTdb::Add(CFcnParTab t)
{
  if(m_pLog)
  {
    m_pLog->Add(1, "WARNING Funcionalidad no implementada CGMTdb::Add(CFcnParTab t)");
  }
  return -1;
}
*/

/* Borra una relacion server - servicio */
int CGMTdb::Remove(CFcnTab t)
{
  int i;
  SH_FUNCION funcion;

  /* busco los servicios para obtener los servers */
  for(i = 0; i < m_max_services; i++)
  {
    if(m_pShMem->GetAt(INDEX_FUNCTION(i), &funcion, sizeof(SH_FUNCION)) != 0)
    {
      if(m_pLog) m_pLog->Add(1, "ERROR en acceso de lectura a emoria compartida en CGMTdb::Remove CFcnTab");
      return -1;
    }
    if(strlen(funcion.nombre))
    {
      if(t.nombre == funcion.nombre &&
        t.tipo_mensaje == funcion.tipo_mensaje &&
        t.server == funcion.server)
      {
        /* decremento la cantidad de suscripciones */
        funcion.suscripciones--;
        if(funcion.suscripciones <= 0)
        {
          /* si era la ultima la borro, sino solamente se actualiza la cantidad de suscripciones */
          /* el borrado es simplemente limpiar la parte del nombre */
          memset(funcion.nombre, 0, sizeof(funcion.nombre));
        }
        if(m_pShMem->SetAt(INDEX_FUNCTION(i), &funcion, sizeof(SH_FUNCION)) != 0)
        {
          if(m_pLog)
          {
            m_pLog->Add(1, "ERROR en acceso de escritura a memoria compartida "
              "en CGMTdb::Remove CFcnTab");
          }
          return -1;
        }
        else
        {
          return 0 ;
        }
      }
    }
  }
  return -1;
}

/*
  Hace un vuelco al logger de los datos de configuracion de SHmem
*/
void CGMTdb::Dump()
{
  int i, j;
  int tot_svr = 0;
  int tot_svc = 0;
  SH_SERVER server;
  SH_FUNCION funcion;

  if(!m_pLog) return; /* Si no puedo acceder al log no tiene sentido seguir */

  m_pLog->Add(1, "Dump de configuracion");
  m_pLog->Add(1, "Servers:");
  for(i = 0; i < m_max_servers; i++)
  {
    if(m_pShMem->GetAt(INDEX_SERVER(i), &server, sizeof(SH_SERVER)) != 0)
    {
      m_pLog->Add(1, "ERROR en acceso de lectura a memoria compartida en CGMTdb::Dump INDEX_SERVER");
      break;
    }
    if(strlen(server.nombre))
    {
      m_pLog->Add(1, "  Nombre:      %s", server.nombre);
      m_pLog->Add(1, "  Descripcion: %s", server.descripcion);
      m_pLog->Add(1, "  Modo:        %i", server.modo);
      m_pLog->Add(1, "  Path:        %s", server.path);
      m_pLog->Add(1, "  Colas:");
      for(j = 0; j < MAX_SERVER_INSTANCES; j++)
      {
        if(server.cola[j] > 0)
        {
          m_pLog->Add(1, "         Key 0x%08X (%i)", server.cola[j], server.pid[j]);
        }
        tot_svr++;
      }
    }
  }
  m_pLog->Add(1, "---------------------------------------------------------");
  m_pLog->Add(1, "Servicios:");
  for(i = 0; i < m_max_services; i++)
  {
    if(m_pShMem->GetAt(INDEX_FUNCTION(i), &funcion, sizeof(SH_FUNCION)) != 0)
    {
      m_pLog->Add(1, "ERROR en acceso de lectura a memoria compartida CGMTdb::Dump INDEX_FUNCTION");
      break;
    }
    if(strlen(funcion.nombre))
    {
      m_pLog->Add(1, "  Nombre:       %s", funcion.nombre);
      m_pLog->Add(1, "  Descripcion:  %s", funcion.descripcion);
      m_pLog->Add(1, "  Tipo-Mensaje: %c", funcion.tipo_mensaje);
      m_pLog->Add(1, "  Server        %s", funcion.server);
    }
    tot_svc++;
  }
  m_pLog->Add(1, "---------------------------------------------------------");
  m_pLog->Add(1, "  Servers:   %i (max %i)", tot_svr, m_max_servers);
  m_pLog->Add(1, "  Servicios: %i (max %i)", tot_svc, m_max_services);
  m_pLog->Add(1, "---------------------------------------------------------");
}

/*
int CGMTdb::DumpSrv(ostream& std, ostream& err)
{
  return -1;
}
*/

/*
int CGMTdb::DumpSvc(ostream& std, ostream& err)
{
  return -1;
}
*/

int CGMTdb::DumpSrv(FILE* std, FILE* err)
{
  int i, j;
  int srv_count = 0;
  SH_SERVER server;

  fprintf(std, "- Servers ----------------------------------------------------------------------\n");
  for(i = 0; i < m_max_servers; i++)
  {
    if(m_pShMem->GetAt(INDEX_SERVER(i), &server, sizeof(SH_SERVER)) != 0)
    {
      fprintf(err, "ERROR: al intentar leer memoria compartida para ver servers");
      return -1;
    }
    if(strlen(server.nombre))
    {
      fprintf(std, " %-32.32s %i ", server.nombre, server.modo);
      for(j = 0; j < MAX_SERVER_INSTANCES; j++)
      {
        if(server.cola[j])
        {
          fprintf(std, "0x%08X (%i) ", server.cola[j], server.pid[j]);
        }
      }
      fprintf(std, "\n");
      srv_count++;
    }
  }
  fprintf(std, "--------------------------------------------------------------------------------\n");
  fprintf(std, "  Servers: %i / %i\n", srv_count, m_max_servers);
  fprintf(std, "--------------------------------------------------------------------------------\n");

  return 0;
}

int CGMTdb::DumpSvc(FILE* std, FILE* err)
{
  int i;
  int svc_count = 0;
  SH_FUNCION funcion;

  fprintf(std, "- Servicios --------------------------------------------------------------------\n");
  for(i = 0; i < m_max_services; i++)
  {
    if(m_pShMem->GetAt(INDEX_FUNCTION(i), &funcion, sizeof(SH_FUNCION)) != 0)
    {
      fprintf(err, "ERROR: al intentar leer memoria compartida para ver servicios");
      return -1;
    }
    if(strlen(funcion.nombre))
    {
      fprintf(std, " %-32.32s %c %s\n", funcion.nombre, funcion.tipo_mensaje, funcion.server);
      svc_count++;
    }
  }
  fprintf(std, "--------------------------------------------------------------------------------\n");
  fprintf(std, "  Servicios: %i / %i\n", svc_count, m_max_services);
  fprintf(std, "--------------------------------------------------------------------------------\n");

  return 0;
}

vector <CGMTdb::CSrvTab> CGMTdb::ServerList(string& servicio, char tipo_mensaje)
{
  return ServerList(servicio.c_str(), tipo_mensaje);
}

vector <CGMTdb::CSrvTab> CGMTdb::ServerList(const char*  servicio, char tipo_mensaje)
{
  vector <CGMTdb::CSrvTab> svr_list;
  int i, j;
  SH_SERVER server;
  CGMTdb::CSrvTab svr;
  SH_FUNCION funcion;
  CFcnTab fcn;

  if( !servicio || !strlen(servicio))
  {
    for(i = 0; i < m_max_servers; i++)
    {
      if(m_pShMem->GetAt(INDEX_SERVER(i), &server, sizeof(SH_SERVER)) != 0)
      {
        return svr_list;
      }
      if(strlen(server.nombre))
      {
        svr.nombre = server.nombre;
        svr.descripcion = server.descripcion;
        svr.modo = server.modo;
        svr.path = server.path;
        svr.cola.clear();
        svr.pid.clear();
        for(j = 0; j < MAX_SERVER_INSTANCES; j++)
        {
          if(server.cola[j] > 0 && server.pid[j] > 0)
          {
            svr.cola.push_back(server.cola[j]);
            svr.pid.push_back(server.pid[j]);
          }
        }
        svr_list.push_back(svr);
      }
    }
  }
  else /* tengo que obtener la lista de servers de la base de servicios */
  {
    for(i = 0; i < m_max_services; i++)
    {
      if(m_pShMem->GetAt(INDEX_FUNCTION(i), &funcion, sizeof(SH_FUNCION)) != 0)
      {
        return svr_list;
      }
      if( !strcmp(funcion.nombre, servicio) &&
        (funcion.tipo_mensaje == tipo_mensaje || tipo_mensaje == '\0') )
      {
        svr.nombre = funcion.server;
        if(Server(svr) == 0)
        {
          svr_list.push_back(svr);
        }
      }
    }
  }
  return svr_list;
}

vector <CGMTdb::CFcnTab> CGMTdb::ServicioList(string& server)
{
  return ServicioList(server.c_str());
}

/* TODO: que le de bola al parametro server */
vector <CGMTdb::CFcnTab> CGMTdb::ServicioList(const char* /*server*/)
{
  int i;
  SH_FUNCION funcion;
  vector <CGMTdb::CFcnTab> fcn_list;
  CFcnTab fcn;

  for(i = 0; i < m_max_services; i++)
  {
    if(m_pShMem->GetAt(INDEX_FUNCTION(i), &funcion, sizeof(SH_FUNCION)) != 0)
    {
      return fcn_list;
    }
    if(strlen(funcion.nombre))
    {
      fcn.nombre = funcion.nombre;
      fcn.descripcion = funcion.descripcion;
      fcn.tipo_mensaje = funcion.tipo_mensaje;
      fcn.server = funcion.server;
      fcn_list.push_back(fcn);
    }
  }
  return fcn_list;
}

int CGMTdb::GetSysInfo(CSystemInfo *si)
{
  return m_pShMem->GetAt(0, si, sizeof(CSystemInfo));
}

int CGMTdb::SetSysInfo(CSystemInfo &si)
{
  return SetSysInfo(&si);
}

int CGMTdb::SetSysInfo(CSystemInfo *si)
{
  return m_pShMem->SetAt(0, si, sizeof(CSystemInfo));
}
