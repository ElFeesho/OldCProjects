/* TMSNC - Textbased MSN Client
 * Copyright (C) 2004 The IR Developer Group
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the IR Public Domain License as published by
 *  the IR Group; either version 1.6 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You should have received a copy of the IR Public Domain License
 * along with this program; if not, write to sanoix@gmail.com.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core_ssl.h"
#include <sys/socket.h>
#include <netdb.h>

BIO *bio_err=0;

char *_(char *str)
{
	return str;
}

char *split(char *str, char splits, int ret)
{
  char *ptr;
  int a, b, c=0;

  /* Find the first split-char */
  if(ret!=0)
  {
    for(a=0;a<=strlen(str);a++)
    {
      if(str[a]==splits && ++c==ret)
      {
        a++;
        break;
      }
      if(a==strlen(str))
        return NULL;
    }
  } else
  {
    a = 0;
  }
  /* Find the second split-char */
  for(b=a;b<=strlen(str);b++)
    if(str[b]==splits) break;

  ptr = (char *)malloc(sizeof(char)*(b-a+1));
  strncpy(ptr, &str[a], b-a);
  ptr[b-a]=0x0;

  return (char *)ptr;
}

char *MSN_url_encode(char *str, char *ret, int ret_len)
{
  int i, a;
  for(i=0, a=0;i<strlen(str) && a<ret_len;i++, a++)
  {
    if(!isalnum(str[i]))
    {
      snprintf(&ret[a], ret_len-i, "%%%x", str[i]);
      a+=2;
    }
    else
    {
      ret[a] = str[i];
    }
  }
  ret[a]=0x0;
  return ret;
}

int tcp_connect(char *host, short port)
{
	int rsock = socket(AF_INET, SOCK_STREAM, 0);
	
	struct hostent *hent = gethostbyname(host);
	struct sockaddr_in sock_dst;
	sock_dst.sin_family = AF_INET;
	sock_dst.sin_port = htons(port);
	sock_dst.sin_addr = *((struct in_addr*)hent->h_addr);
	
	connect(rsock, (struct sockaddr*)&sock_dst, sizeof(sock_dst));
	
	return rsock;
}

/* Print SSL errors and exit*/
int berr_exit(char *string)
{
  BIO_printf(bio_err,"%s\n",string);
  ERR_print_errors(bio_err);
  exit(0);
}

SSL_CTX *initialize_ctx(void)
{
  SSL_METHOD *meth;
  SSL_CTX *ctx;

  if(!bio_err)
  {
    /* Global system initialization*/
    SSL_library_init();
    SSL_load_error_strings();

    /* An error write context */
    bio_err=BIO_new_fp(stderr,BIO_NOCLOSE);
  }
    /* Create our context*/
  meth=TLSv1_client_method();
  ctx=SSL_CTX_new(meth);
  
  return ctx;
}

int SSL_receive(SSL *ssl, char *buf, int len)
{
  char recv_buf[len];
  int r, bytes_received=0;
  
  buf[0]=0x0;
  while(bytes_received<len)
  {
    memset(recv_buf, 0x0, len);
    r = SSL_read(ssl,recv_buf,len);
    if(r<0) return -1;
    if(r==0) return 0;
    
    bytes_received+=r;
    strncat(buf, recv_buf, len);
  }
  return 0;
}


int get_https(char *host, int port, char *request, char *buf, int len)
{
  SSL_CTX *ctx;
  SSL *ssl;
  BIO *sbio;
  int ret, sock;
  
  /* Build our SSL context*/
  ctx=initialize_ctx();
  
  /* Connect the TCP socket*/
  sock=tcp_connect(host, port);
  
  /* Connect the SSL socket */
  ssl=SSL_new(ctx);
  sbio=BIO_new_socket(sock,BIO_NOCLOSE);
  SSL_set_bio(ssl,sbio,sbio);
  
  if(SSL_connect(ssl)<=0)
    berr_exit(_("SSL connect error"));
  
  if(SSL_write(ssl,request,strlen(request))<1)
    berr_exit(_("SSL write problem"));
  
  if(SSL_receive(ssl,buf,len)<0)
    berr_exit(_("SSL read problem"));            
  
  if(SSL_shutdown(ssl)<0)
    berr_exit(_("SSL shutdown problem"));
  
  /*** If the document has moved ***/
  if(strncmp(buf, "HTTP/1.1 302", 12)==0)
    ret = -2;
  /*** If the server returned an error ***/
  else if(strncmp(buf, "HTTP/1.1 200", 12)!=0)
    ret = -1;
  else ret = 0;
  
  SSL_free(ssl);
  SSL_CTX_free(ctx);
  close(sock);
  
  return(ret);
}

int https_auth(char *origHost,int port,char *origFile,char *msnLogin,char *msnPasswd,char *idString,char *ticket,int ticketLen, void (*callback)(int,void *))
{
  int r;
  pid_t pid;
  struct timeval tv;
  char buf[BUF_SIZE], request[BUF_SIZE], *host, *encodedPasswd, *file, *ptr;

  char requestTemplate[] = "GET /%s HTTP/1.1\r\nAuthorization: "
                           "Passport1.4 OrgVerb=GET,"
                           "OrgURL=http%%3A%%2F%%2Fmessenger%%2Emsn%%2Ecom,"
                           "sign-in=%s,pwd=%s,%sHost: "
                           //"%s:%d\r\n\r\n";
									"%s\r\n\r\n";

  /* Seed the random number generator */
  pid = getpid();
  RAND_add(&pid, sizeof(pid),0.01);
  pid = getpgrp();
  RAND_add(&pid, sizeof(pid),0.01);
  pid = getppid();
  RAND_add(&pid, sizeof(pid),0.01);
  pid = 0;
  gettimeofday(&tv,NULL);
  RAND_add(&tv, sizeof(tv),0.1);
  memset(&tv, '\0', sizeof(tv));

  /* URL-encode password and make copies of the host and file */
  MSN_url_encode(msnPasswd, buf, sizeof(buf)-1);
  encodedPasswd = strdup(buf);
  host = strdup(origHost);
  file = strdup(origFile);

  if(encodedPasswd==NULL || host==NULL || file==NULL)
  {
    if(callback!=NULL) callback(-1,_("Cannot allocate memory"));
    return -1;
  } 

	printf("MSN LOGIN: %s\n",msnLogin);

	char *escaped_msnLogin = malloc(strlen(msnLogin)*3+1);

	printf("Done!\n");

	memset(escaped_msnLogin,strlen(msnLogin)*3+1,'\0');

	printf("OT OH\n");
	MSN_url_encode(msnLogin, escaped_msnLogin, strlen(msnLogin)*3);
	printf("SPAGHETTIOHS!\n");
  /* form the first request */
  snprintf(request, BUF_SIZE-1, requestTemplate, file, escaped_msnLogin, encodedPasswd, idString, host);
	printf("Request: %s\n", request);
  /* request a new document until get_https returns 0 or -1 */
  while((r=get_https(host, port, request, buf, sizeof(buf)-1))==-2)
  {
    free(host);
    free(file);
    
    if((ptr = strstr(buf, "Location:"))==NULL)
    {
      printf("No 'Location:' in HTTP header\n");
      return -1;
    }
    ptr[strlen(ptr)-4]=0x0;
    host = split(ptr, '/', 2);
    file = split(ptr, '/', 3);

    if(host==NULL || file==NULL)
    {
      printf("Couldn't parse URL\n");
      return -1;
    }

    /* remove trailing '\r\n' from file */
    if((ptr = strstr(file, "\r\n")) != NULL)
      *ptr = 0x0;

    /* form a new request */
    snprintf(request, sizeof(request)-1, requestTemplate,file, escaped_msnLogin, encodedPasswd, idString, host /*, port */);
  }

  free(host);
  free(file);
  free(encodedPasswd);

  if(r==-1)
  {
    printf("Bad username or password!\n");
    return -1;	
  }
  
  if((ptr=split(buf, '\'',1))==NULL)
  {
    printf("Couldn't extract ticket\n");
    return -1;
  }
  strncpy(ticket, ptr, ticketLen);
  free(ptr);
  
  return 0;
}
