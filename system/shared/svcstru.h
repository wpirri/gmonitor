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
#ifndef _SERVER_STRUCT_H_
#define _SERVER_STRUCT_H_

/* Estructura para consultas y respuestas de los servicios:
  .new_buffer
  .add_buffer
  .del_buffer
  .get_buffer */
typedef union _ST_SBUFFER
{
  struct
  {
    unsigned int id;
    unsigned long len;
    char data[1];
  } new_buffer;
  struct
  {
    unsigned int id;
    unsigned long len;
    char data[1];
  } add_buffer;
  struct
  {
    unsigned int id;
  } del_buffer;
  struct
  {
    unsigned int id;       /* IN  - identificador del buffera devolver */
    unsigned long offset;  /* IN  - offset del inicio a devolver */
    unsigned long maxlen;  /* IN  - tam max a devolver */
    unsigned long len;     /* OUT - tam devuelto */
    unsigned long totlen;  /* OUT - tam total del buffer almacenado */
    char data[1];
  } get_buffer;
} ST_SBUFFER;

/*
  Estructura para los servicios
  .settimer
  .killtimer
*/
typedef union _ST_STIMER
{
  struct
  {
    unsigned int id;
    char servicio[32];
    char modo_servicio;
    unsigned int delay; /* Para setar un timer por tiempo */
    char tipo_timer;     /* Repetitivo / Unico */
    unsigned long at;    /* Para setear un timer en una hora determinada */
    unsigned long len;
    char data[1];
  } set_timer;
  struct
  {
    unsigned int id;
  } kill_timer;
} ST_STIMER;

/*
  Estructuras para los servicios encolados
  .create-queue
  .enqueue
  .dequeue
  .list-queue
  .info-queue
  .clear-queue
  .clear-all-queue
*/

typedef struct _ST_SQUEUE
{
  char saf_name[32];
  unsigned long len;
  char data[1];
} ST_SQUEUE;


#endif /* _SERVER_STRUCT_H_ */
