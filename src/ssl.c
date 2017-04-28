#include <openssl/ssl.h>
#include <openssl/err.h>
#include <stdio.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <sys/socket.h>
#include <resolv.h>


/**
 * \fn SSL_CTX* InitServerCTX(void)
 * \brief    initialisation SSL 
 * \details  chargement de la librairie SSL
 *      
*/
SSL_CTX* InitServerCTX(void){
  SSL_METHOD *method = NULL;
  SSL_CTX *ctx = NULL;
  /* Chargement des algos cryptographique */
  OpenSSL_add_all_algorithms();
  /* Chargement erreur Manager */
  SSL_load_error_strings();
  /* initialisation de la librairie */
  SSL_library_init();
  /* creation instance methode serveur */
  method = TLSv1_2_server_method();  
  /* creation du context */
  ctx = SSL_CTX_new(method);
  if (ctx == NULL ){
    ERR_print_errors_fp(stderr);
    abort();
  }
  return ctx;
}



  
/**
 * \fn void chargementCertificats(SSL_CTX* ctx, char* CertFile, char* KeyFile)
 * \brief    chargement du Certificat 
 * \details  le certificat est chargé en entrée avec la cle privée (dans le cas
 *  ou la clé privée et le certificat ne correspondent pas le chargement est annulé
 *  
 * \param  SSL_CTX* ctx SSL contexte  
 * \param  char* CertFile chemin vers le certificat
 * \param  char*  chemin vers la clé priveé
 * \return 
 */
void chargementCertificats(SSL_CTX* ctx, char* CertFile, char* KeyFile){
  /* initialisation du certificat dans le context */
  if ( SSL_CTX_use_certificate_file(ctx, CertFile, SSL_FILETYPE_PEM) <= 0 ){
    ERR_print_errors_fp(stderr);
    abort();
  }
  /*initialistion du context avec la cle privee */
  if ( SSL_CTX_use_PrivateKey_file(ctx, KeyFile, SSL_FILETYPE_PEM) <= 0 ){
    ERR_print_errors_fp(stderr);
    abort();
  }
  /* verification de la cle privee */
  if ( !SSL_CTX_check_private_key(ctx) ){
    fprintf(stderr, "Private key does not match the public certificate\n");
    abort();
  }
}