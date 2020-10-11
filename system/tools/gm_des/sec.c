/* ************************************************************************ */
/*
Encripcion por Software (Analisis de la PROCESS^DES^PIN de ACIUTILS)
--------------------------------------------------------------------

PROCESS-TYPE 1: (Validacion de PIN)
-----------------------------------

PROCESS-TYPE 2: (Calculo de OFFSET)
-------------------------------------

PROCESS-TYPE 3: (Calculo de PIN)
-------------------------------------
PAN[PAN-OFFSET] + PAN-PAD             /-> xxxxxxxxxxxxFFFF
                                  DES
ENCR-KEY                              \-> 0123456789ABCDEF
                                      -----------------------
                                      /-> 0123456789ABCDEF       PAN-ENCRIPTADO
                       DECIMALIZACION
DEC-TBL                               \-> 0123456789012345
                                      -----------------------
                                      /-> 0123456789012345       PIN-NATURAL
                     SUMA SIN ACARREO
PIN-OFFSET                            \-> 0123456789012345
                                      -----------------------
                                          0123456789012345       PIN


Encripcion del PIN
------------------

PIN-LEN + PIN + PIN-PAD               /-> 04xxxxFFFFFFFFFF
                                  XOR
PAN[PAN-OFFSET] + PAN-PAD             \-> ppppppppppppFFFF
                                      -----------------------
                                      /-> aaaaaaaaaaaaaaaa            ANSI PIN BLOCK
                                  DES
ENCR-KEY                              \-> 0123456789ABCDEF
                                      -----------------------
                                      /-> 0123456789ABCDEF
                       DECIMALIZACION
DEC-TBL                               \-> 0123456789012345
                                      -----------------------
                                      /-> 0123456789012345            PIN-NATURAL
                     SUMA SIN ACARREO
OFFSET                                \-> 0123456789012345
                                      -----------------------
                                          0123456789012345            PIN
*/
/* ************************************************************************ */
#include "sec.h"

#include "desmat.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifndef min
#	define min(a, b)     ((a) < (b) ? (a) : (b))
#endif /* min */

int decimalizacion(const char* in, int len, const char* tab, char* out);
int suma_mod10(const char* in, const char* ofst, char* out);
int IsHexa(const char *data, int len);
/* ************************************************************************ */
/* *             Funciones utilizadas por los algoritmos de DES           * */
/* ************************************************************************ */
void transpone(int *dato, int *transp, int cant);
int expo(int n, int p);
void rotizq(int *clave);
void rotder(int *clave);
void f(int numero, int *clave, int *m_input, int *m_output, int flag);
/* ************************************************************************ */
int DES_GenPin(char* pin, int pin_len,
                 const char* pan, int pan_ofst, int pan_len, char pad,
                 const char* dec_tab, const char* key,
                 const char* ofst, int ofst_len)
{
  char pan_pad[16];
  char pan_pad_cript[16];
  char pan_pad_cript_deci[16];
  char pan_pad_cript_deci_ofst[16];
  char ofst_16[16];

  /* controles previos */
  if( !IsHexa(dec_tab, 16)) return -1;
  if( !IsHexa(key, 16)) return -1;
  if( !IsHexa(ofst, ofst_len)) return -1;
  /* me copio el pan desde el offset y padeo el resto, en realidad hago
     al revez que es mas facil */
  memset(pan_pad, pad, 16);
  memcpy(pan_pad, &pan[pan_ofst], min(16, pan_len));
  /* se encripta con la clave de encripcion */
  Des_Encrypt(pan_pad, 16, key, 16, pan_pad_cript);
  /* Decimalizo el pan pedeado */
  decimalizacion(pan_pad_cript, 16, dec_tab, pan_pad_cript_deci);
  /* padeo el ofsset con 0s al final */
  memset(ofst_16, '0', 16);
  memcpy(ofst_16, ofst, ofst_len);
  suma_mod10(pan_pad_cript_deci, ofst_16, pan_pad_cript_deci_ofst);
  memcpy(pin, pan_pad_cript_deci_ofst, pin_len);
  if( !IsHexa(pin, pin_len)) return -1;
  return 0;
}

int DES_GenOffset(const char* pin, int pin_len,
                   int min_pin_len, int max_pin_len,
                   const char* pan, int pan_ofst, int pan_len, char pad,
                   const char* dec_tab, const char* key,
                   char* ofst, int ofst_len)
{

  return -1;
}

int Des_Encrypt( const char *in, int inlen,
                 const char *key, int keylen, char *out)
{
  int *inb, keyb[64], cripto[64];
  int a[64], b[64], x[64];
  int i, j, k, fl;

  if(inlen%16) return -1;
  if(keylen!=16) return -1;

  inb = (int*)calloc(inlen, sizeof(int)*4);
  for (i = 0; i < inlen / 2; i++)
  {
    Asc2Bin(inb + (i * 8), in + (i * 2));
  }
  for (i = 0; i < keylen / 2; i++)
  {
    Asc2Bin(keyb + (i * 8), key + (i * 2));
  }
  fl = 1;  /* indica encripcion */

  transpone(keyb, kt1, 56); /* transpone la clave y reduce a 56 bits */
  for(k = 0; k < inlen/16; k++)
  {
    memcpy(a, &inb[k*64], sizeof(a));
    transpone(a, itr, 64);  /* transposicion inicial */
    for (i = 0; i < 16; i++)
    {
      memcpy(b, a, sizeof(a));  /* a contiene el ultimo encriptado */
      for (j = 0; j < 32; j++)
      {
        a[j] = b[j + 32];        /* Ln = Rn-1 */
      }
      f(i+1, keyb, a, x, fl);        /* x = f( Ri-1, Ki)     */
      for (j = 0; j < 32; j++)
      {
        a[j + 32] = b[j] ^ x[j];  /* Rn = Ln-1  XOR f(Rn-1, Kn) */
      }
    }
    transpone(a, swap, 64);         /* intercambio Ln y Rn */
    transpone(a, ftr, 64);          /* transposicion final */
    memcpy(cripto, a, sizeof(a));
    for (i = 0; i < 8; i++)
    {
      Bin2Asc(cripto + (i * 8), &out[k*16] + (i * 2));
    }
  }
  return 0;
}

int Des3Encrypt(const char *in, int inlen,
                const char *clave1, const char *clave2, int keylen,
                char *out)
{
  char *tmp1, *tmp2;

  tmp1 = (char*)calloc(inlen, sizeof(char));
  tmp2 = (char*)calloc(inlen, sizeof(char));

  Des_Encrypt( in,   inlen, clave1, keylen, tmp1);
  Des_Decrypt( tmp1, inlen, clave2, keylen, tmp2);
  Des_Encrypt( tmp2, inlen, clave1, keylen, out );

  free(tmp1);
  free(tmp2);

  return 0;
}

int Des_Decrypt(const char *in, int inlen,
                const char *key, int keylen, char *out)
{
  int *inb, keyb[64], cripto[64];
  int a[64], b[64], x[64];
  int i, j, k, fl;

  if(inlen%16) return -1;
  if(keylen!=16) return -1;

  inb = (int*)calloc(inlen, sizeof(int)*4);
  for (i = 0; i < inlen / 2; i++)
  {
    Asc2Bin(inb + (i * 8), in + (i * 2));
  }
  for (i = 0; i < keylen / 2; i++)
  {
    Asc2Bin(keyb + (i * 8), key + (i * 2));
  }
  fl = 2;         /* flag de desencripcion */
  transpone(keyb, kt1, 56); /* transpone la clave y reduce a 56 bits */
  for(k = 0; k < inlen/16; k++)
  {
    memcpy(a, &inb[k*64], sizeof(a));
    transpone(a, itr, 64);  /* transposicion inicial */
    transpone(a, swap, 64);
    for (i = 16; i > 0; i--)
    {
      memcpy(b, a, sizeof(a));  /* a contiene el ultimo desencriptado */
      for (j = 0; j < 32; j++)
      {
        a[j + 32] = b[j];        /* Rn-1 = Ln */
      }
      f(i, keyb, a, x, fl);        /* x = f( Li , Ki)     */
      for (j = 0; j < 32; j++)
      {
        a[j] = b[j + 32] ^ x[j];  /* Ln-1 = Rn  XOR f(Ln , Kn) */
      }
    }
    transpone(a, ftr, 64);          /* transposicion final */
    memcpy(cripto, a, sizeof(a));
    for (i = 0; i < 8; i++)
    {
      Bin2Asc(cripto + (i * 8), &out[k*16] + (i * 2));
    }
  }
  return 0;
}

int Des3Decrypt(const char *in, int inlen,
                const char *clave1, const char *clave2, int keylen,
                char *out)
{
  char *tmp1, *tmp2;

  tmp1 = (char*)calloc(inlen, sizeof(char));
  tmp2 = (char*)calloc(inlen, sizeof(char));

  Des_Decrypt( in,   inlen, clave1, keylen, tmp1);
  Des_Encrypt( tmp1, inlen, clave2, keylen, tmp2);
  Des_Decrypt( tmp2, inlen, clave1, keylen, out );

  free(tmp1);
  free(tmp2);

  return 0;
}

/* ************************************************************************ */
void Asc2Bin(int *entero, const char *carac)
{
  int i,j;

  if(carac[0] > '9') { j = (carac[0] - 55) * 16; }
  else { j = (carac[0] - 0x30) * 16; }
  if(carac[1] > '9') { j += carac[1] - 55; }
  else { j += carac[1] - 0x30; }
  for(i = 0; i < 8; i++)
  {
    entero[i] = j & mascara[i];
    if(entero[i] != 0) entero[i] = 1;
  }
}

void Bin2Asc(const int *entero, char *carac)
{
  int j, l;

  l = 0;
  for(j = 0; j < 8; j++)
  {
    l += entero[j] * expo(2, 7 - j);
  }
  carac[0] = (unsigned char)(l / 16);
  if(carac[0] > 9) { carac[0] += 55; }
  else { carac[0] += 0x30; }
  carac[1] = (unsigned char)(l % 16);
  if(carac[1] > 9) { carac[1] += 55; }
  else { carac[1] += 0x30; }
}

void transpone(int *dato, int *transp, int cant)
{
  int x[64];
  int i;

  memcpy(x, dato, sizeof(x));
  for (i = 0; i < cant; i++)
  {
   dato[i] = x[transp[i]-1];
  }
}

void rotizq(int *clave)
{
    int x[64];
    int i;

    memcpy(x, clave, sizeof(x));
    for (i = 0; i < 55; i++)
    {
      x[i] = x[i+1];
    }
    x[27] = clave[0];
    x[55] = clave[28];
    memcpy(clave, x, sizeof(x));
}

void rotder(int *clave)
{
  int x[64];
  int i;

  memcpy(x, clave, sizeof(x));
  for (i = 55; i > 0; i--)
  {
    x[i] = x[i-1];
  }
  x[0]  = clave[27];
  x[28] = clave[55];
  memcpy(clave, x, sizeof(x));
}

int expo(int n, int p)
{
    if(p == 0)        { return 1; }
    else if (p == 1)  { return n; }
    else { return (n * expo(n, p - 1)); }
}

void f(int numero, int *clave, int *m_input, int *m_output, int flag)
{
  int e[64], claux[64], y[64], r, k, j;

  memcpy(e, m_input, sizeof(e));
  transpone(e, etr, 48);    /* expande e a 48 bits  */
  if (flag == 1)
  {
    for (j = 0; j < nrot[numero - 1]; j++)
    {
      rotizq(clave);
    }
  }
  memcpy(claux, clave, sizeof(claux));
  transpone(claux, kt2, 48);
  for (j = 0; j < 48; j++)
  {
    y[j] = e[j] ^ claux[j];  /* Rn-1  xor  Kn  */
  }
  for (k = 0; k < 8; k++)
  {
    r = 32 * y[6 * k] +
        16 * y[6 * k + 5] +
         8 * y[6 * k + 1] +
         4 * y[6 * k + 2] +
         2 * y[6 * k + 3] +
             y[6 * k + 4];

    m_output[4 * k]     = (caja_s[k][r] / 8) % 2;
    m_output[4 * k + 1] = (caja_s[k][r] / 4) % 2;
    m_output[4 * k + 2] = (caja_s[k][r] / 2) % 2;
    m_output[4 * k + 3]  = caja_s[k][r] % 2;
  }
  transpone(m_output, ptr, 32);
  if (flag == 2)
  {
    for (j = 0; j < nrot[numero - 1]; j++)
    {
      rotder(clave);
    }
  }
}

int decimalizacion(const char* in, int len, const char* tab, char* out)
{
  int idx;

  for(; len > 0; len--)
  {
    idx = (int)(in[len -1]);
    if(idx >= '0' && idx <= '9') { idx -= '0'; }
    else if(idx >= 'A' && idx <= 'F') { idx = (int)(idx - 'A' + 10); }
    else return -1;
    out[len-1] = tab[idx];
  }
  return 0;
}

int suma_mod10(const char* in, const char* ofst, char* out)
{
  int i;

  for(i = 0; i < 16; i++)
  {
    out[i] = (char)((((in[i]-'0') + (ofst[i]-'0')) % 10) + '0');
  }
  return 0;
}

int IsHexa(const char *data, int len)
{
  if(len <= 0) return -1;
  for(;len;len--)
  {
    if( (data[len-1] < '0' || data[len-1] > '9') &&
        (data[len-1] < 'A' || data[len-1] > 'F')  ) return -1;
  }
  return 0;
}
/* ************************************************************************ */
