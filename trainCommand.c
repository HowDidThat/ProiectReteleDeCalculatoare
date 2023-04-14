/* cliTCPIt.c - Exemplu de client TCP
   Trimite un nume la server; primeste de la server "Hello nume".

   Autor: Lenuta Alboaie  <adria@infoiasi.ro> (c)2009
*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
/* codul de eroare returnat de anumite apeluri */
extern int errno;

/* portul de conectare la server*/
int port;

int sd;
int updateTimer = 4;

int main (int argc, char *argv[])
{
  // descriptorul de socket
  struct sockaddr_in server;	// structura folosita pentru conectare
  		// mesajul trimis

  /* exista toate argumentele in linia de comanda? */
  if (argc != 3)
    {
      printf ("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
      return -1;
    }

  /* stabilim portul */
  port = atoi (argv[2]);

  /* cream socketul */
  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("Eroare la socket().\n");
      return errno;
    }

  /* umplem structura folosita pentru realizarea conexiunii cu serverul */
  /* familia socket-ului */
  server.sin_family = AF_INET;
  /* adresa IP a serverului */
  server.sin_addr.s_addr = inet_addr(argv[1]);
  /* portul de conectare */
  server.sin_port = htons (port);;

  /* ne conectam la server */
  if (connect (sd, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1)
    {
      perror ("[client]Eroare la connect().\n");
      return errno;
    }
    printf("creare Threaduri\n");
    //conexiunea a fost stabilita, crearea threadurilor

    //defaultData();
  char buf[10]; // datele primite de la consola
  char msg[20]; // mesajul care va fi trimis la server dupa procesare
  while (1)
  {
  bzero (msg,20); // golim spatiul
  bzero (buf,10); // golim spatiul
  printf ("[client]Introduceti ID tren: ");
  fflush (stdout);
  read (0, buf, sizeof(buf)); //citim de la tastatura mesajul
  if (strcmp(buf,"quit\n") == 0)  //daca mesajul este de terminare il trimitem ca atare
      {if (write (sd,"quit",sizeof("quit")) <= 0)
      {
      perror ("[client]Eroare la write() spre server.\n");
      return errno;
    }
    return 0; //inchidem clientul
  }
  buf[strlen(buf)-1] = '\0';
  strcpy(msg,"^");
  strcat(msg,buf);
  strcat(msg,"^"); // configuram mesajul
  if (write (sd,msg,sizeof(buf)) <= 0) //trimitem mesajul
    {
      perror ("[client]Eroare la write() spre server.\n");
      return errno;
    }
  }
  return 0;
}
