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
#include <gtk/gtk.h>

#include "TrainStructure.h"
#include "ClientDataManipulation.h"
#include "GUIClient.h"
/* codul de eroare returnat de anumite apeluri */
extern int errno;

/* portul de conectare la server*/
int port;

pthread_t threads[2];

int sd;
int updateTimer = 4;


void *GUIClient(void *sda)
{
  char **arr=malloc(2);
  char *sdstring = malloc(2);
  sdstring[0]=sd-'0';
  sdstring[1]='\0';
  arr[0]=sdstring;
  StartGUI(1,arr);
  pthread_exit(NULL);
}



int main (int argc, char *argv[])
{
  // descriptorul de socket
  struct sockaddr_in server;	// structura folosita pentru conectare
  		// mesajul trimis
  int nr=0;
  char buf[10];

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
    


    int rc;
    rc = pthread_create(&threads[0],NULL,GUIClient,(void*)sd);
    if (rc) {
       printf("Error:unable to create thread2, %d", rc);
       exit(-1);
    }
  printf("endProgram\n");
  int sec=0;
  while (run)
  {
    sleep(updateTimer);
    if (run)
      GetData();

  }
  return 0;
}
