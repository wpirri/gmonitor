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
#ifndef _GMONTDB_H_
#define _GMONTDB_H_

#include <sincmem.h>
#include <glog.h>

#include <fstream>
#include <string>
#include <vector>
using namespace std;

#include <stdio.h>

#define GM_FILENAME_SERVER_DB             "server.tab"
#define GM_FILENAME_SERVER_PARAMETRO_DB   "server_parametro.tab"
#define GM_FILENAME_FUNCION_DB            "funcion.tab"
#define GM_FILENAME_FUNCION_PARAMETRO_DB  "funcion_parametro.tab"

// ahora definido por configure #define GM_CONFIG_KEY	0x131313

#define MAX_SERVER_INSTANCES 255

class CGMTdb
{
public:
	typedef struct _CSrvTab
	{
		string nombre;		/* Clave */
		string descripcion;
		int modo;
		string path;
		vector <int> cola;	/* para acelerar la busqueda de resolucion de servicios*/
	} CSrvTab;
	typedef struct _CSrvParTab
	{
		string server;		/* Clave */
		string parametro;	/* Clave */
		string valor;
	} CSrvParTab;
	typedef struct _CFcnTab
	{
		string nombre;		/* Clave */
		string descripcion;
		char tipo_mensaje;	/* Tipo de mensaje que va a responder C/R M G/L Q */
		string server;
		/*int borrar;*/
	} CFcnTab;
	typedef struct _CFcnParTab
	{
		string funcion;		/* Clave */
		string parametro;	/* Clave */
		string valor;
	} CFcnParTab;
  typedef struct _CSystemInfo
  {
    int log_level;
    time_t start_time;
    int load_tps;
    int max_tps;
  } CSystemInfo;

public:
	CGMTdb(string& cfgpath, int max_servers, int max_services, CGLog* plog = NULL);
	CGMTdb(const char* cfgpath, int max_servers, int max_services, CGLog* plog = NULL);
	virtual ~CGMTdb();

	/* Crea el area de configuracion (solo para el server de configuracion) */
	int Create(CGLog* plog = NULL);
	/* Actualiza la tabla de servers */
	int Reload();
	/* se asocia a un area de configuracion previmente creada */
	int Open(CGLog* plog = NULL);
	/* depende de si se cre� o se asoci� libera o se desasocia */
	void Close();
	/* Obtienen los datos asociados con un server o servicio */
	/*int Data(CSrvTab& s); Obsoleto */
	/*int Data(CFcnTab& f); Obsoleto */
	/* Completa la estructura con los datos del server */
	int Server(CSrvTab& s);
	/* obtiene la lista de servers que atienden un servicio o
	todos los srvers si no se le pasan parametros */
	vector <CSrvTab> ServerList(string& servicio, char tipo_mensaje);
	vector <CSrvTab> ServerList(const char*  servicio, char tipo_mensaje);
	/* obtiene la lista de servicios que atiende un server o
	todos los servicios si no se le pasan parametros */
	vector <CFcnTab> ServicioList(string& server);
	vector <CFcnTab> ServicioList(const char* server);
	/* Obtienen los parametros para server y funcion */
	/*
	En un server GM_MODO_ONLINE:
		Los parametros del server se pasan al programa al ser levantado por gmq.
		Los parametros de funcion no se usan.
	En un server GM_MODO_OFFLINE:
		Los parametros del server no se usan.
		Los parametros de funcion se pasan al programa al ser levantado por gmq.
	En un server GM_MODO_STANDALONE:
		Los parametros del server no se usan.
		Los parametros de funcion no se usan.
	*/
	int SrvParams(string srv, char** param_list);
	int FcnParams(string fcn, char** param_list);
	int SrvParams(const char* srv, char** param_list);
	int FcnParams(const char* fcn, char** param_list);
	/* Obtiene las colas asociadas al servicio */
	vector <int> Cola(string& servicio, char tipo_mensaje);
	vector <int> Cola(const char* servicio, char tipo_mensaje);
	/* Para parametrizacion on line */
	int AddSrv(string& nombre_server, int cola, int indice);
	int AddSrv(const char* server, int cola, int indice);
	int RemoveSrv(string& server, int indice);
	int RemoveSrv(const char* server, int indice);
	int AddSvc(const char* servicio, char tipo_mensaje, const char* server);
	int AddSvc(string& servicio, char tipo_mensaje, string& server);
	int RemoveSvc(const char* servicio, char tipo_mensaje, const char* server);
	int RemoveSvc(string& servicio, char tipo_mensaje, string& server);
	void Dump();
	int DumpSrv(ostream& std, ostream& err);
	int DumpSrv(FILE* std, FILE* err);
	int DumpSvc(ostream& std, ostream& err);
	int DumpSvc(FILE* std, FILE* err);
  int GetSysInfo(CSystemInfo *si);
  int SetSysInfo(CSystemInfo &si);
  int SetSysInfo(CSystemInfo *si);

protected:
	typedef struct _SH_SERVER
	{
		char	nombre[33];
		char	descripcion[256];
		int		modo;
		char	path[256];
		int		cola[MAX_SERVER_INSTANCES]; /*para distintas instancias de un mismo server*/
	} SH_SERVER;
	typedef struct _SH_FUNCION
	{
		char	nombre[33];
		char	descripcion[256];
		char	tipo_mensaje;
		char	server[33];
		int		suscripciones;
	} SH_FUNCION;

	int Load();
	/*int Save(); */
	/* int Clean();*/
	int LoadSrv();
	int LoadSrvPar();
	int LoadFcn();
	int LoadFcnPar();
	int SaveSrv();
	int SaveSrvPar();
	int SaveFcn();
	int SaveFcnPar();
	int Add(CSrvTab	t);
	int Add(CFcnTab t);
	int Add(CSrvParTab t);
	int Add(CFcnParTab t);
	int Remove(CFcnTab t);

	CGLog* m_pLog;
private:
	string m_config_path;
	int m_max_servers;
	int m_max_services;
	CSincMem* m_pShMem;
};
#endif /* _GMONTDB_H_ */

