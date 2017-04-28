#ifndef _SSL_
#define _SSL_

SSL_CTX* InitServerCTX(void);

void chargementCertificats(SSL_CTX* ctx, char* CertFile, char* KeyFile);
#endif