#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "sec.h"

int main(int argc, char** argv)
{
  int i;
  char data[4096];
  int  datalen;
  char key[16];
  char out[4096];
  int action = 0;

  memset(data,0,4096);
  memset(key,0,16);

  for(i = 1; i < argc; i++)
  {
    if( !strcmp(argv[i],"-k"))
    {
      i++;
      memcpy(key, argv[i], strlen(argv[i]));
    }
    else if( !strcmp(argv[i],"-d"))
    {
      action = 1;
    }
    else if( !strcmp(argv[i],"-e"))
    {
      action = 2;
    }
    else
    {
      datalen = (int)strlen(argv[i]);
      if(datalen > 4096) return -1;
      memcpy(data, argv[i], datalen);
      break;
    }
  }
  if( !strlen(key) || !strlen(data))
  {
    printf("Use %s [-e|-d] -k key data\n", argv[0]);
    printf("    -e: indica encriptar\n");
    printf("    -d: indica desencriptar\n");
    printf("    -k key: donde key es la clave a utilizar\n");
    printf("    data: es el dato a encriptar/desencriptar\n");
    printf("  Ejemplo: encriptar y desencriptar 049562FFFFFFFFFF\n");
    printf("           con la clave 0123456789ABCDEF\n");
    printf("    #> gmcrypt -e -k 0123456789ABCDEF 049562FFFFFFFFFF\n");
    printf("    2EFB6C8B6A435728\n");
    printf("    #> gmcrypt -d -k 0123456789ABCDEF 2EFB6C8B6A435728\n");
    printf("    049562FFFFFFFFFF\n");

    return 0;
  }
  if(action == 1)
  {
    Des_Decrypt(data, datalen, key, 16, out);
  }
  else if(action == 2)
  {
    Des_Encrypt(data, datalen, key, 16, out);
  }
  else return 0;

  printf("%-*.*s\n",datalen,datalen,out);

  return 0;
}
