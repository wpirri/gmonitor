#ifndef _SEC_H_
#define _SEC_H_

void Asc2Bin(int *entero, const char *carac);
void Bin2Asc(const int *entero, char *carac);

int DES_GenPin(   char* pin, int pin_len,
                       const char* pan, int pan_ofst, int pan_len, char pad,
                       const char* dec_tab, const char* key,
                       const char* ofst, int ofst_len);

int Des_Encrypt( const char *in, int inlen,
                 const char *key, int keylen, char *out);

int Des3Encrypt(const char *in, int inlen,
                      const char *clave1, const char *clave2, int keylen,
                      char *out);
int Des_Decrypt(const char *in, int inlen,
                const char *key, int keylen, char *out);

int Des3Decrypt(const char *in, int inlen,
                      const char *clave1, const char *clave2, int keylen,
                      char *out);

#endif /* _SEC_H_ */
