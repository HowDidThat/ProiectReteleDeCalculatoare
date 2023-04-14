#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <thread>
#include <csignal>
#include "TrainStructure.h"
#include "DataManipulation.h"
#include "Classes.h"

/* portul folosit */

#define PORT 2728

extern int errno;		/* eroarea returnata de unele apeluri */

void *resolver(void *arg);
// lock pt threaduri la punerea comenzii in coada
pthread_mutex_t mlock=PTHREAD_MUTEX_INITIALIZER;
// daca comenzile sunt gata de procesare 0= terminata soketDesc = de executat
int commandStatus[100];
// vector pentru comenzi
char commands[100][30];

//functia de plasare a comenzilor in coada
int PutCommnadInList(int fd);
//structura  de memorare a informatiilor trenurilor
Train fleet[200];
//numarul total de trenuri
int numberTrains;


/* functie de convertire a adresei IP a clientului in sir de caractere */
char * conv_addr (struct sockaddr_in address)
{
  static char str[25];
  char port[7];

  /* adresa IP a clientului */
  strcpy (str, inet_ntoa (address.sin_addr));
  /* portul utilizat de client */
  bzero (port, 7);
  sprintf (port, ":%d", ntohs (address.sin_port));
  strcat (str, port);
  return (str);
}
//pthread_mutex_t mlock=PTHREAD_MUTEX_INITIALIZER;
/* programul */
int main ()
{
  struct sockaddr_in server;	/* structurile pentru server si clienti */
  struct sockaddr_in from;
  fd_set readfds;		/* multimea descriptorilor de citire */
  fd_set actfds;		/* multimea descriptorilor activi */
  struct timeval tv;		/* structura de timp pentru select() */
  int sd, client;		/* descriptori de socket */
  int optval=1; 			/* optiune folosita pentru setsockopt()*/
  int fd;			/* descriptor folosit pentru
				   parcurgerea listelor de descriptori */
  int nfds;			/* numarul maxim de descriptori */
  unsigned int len;			/* lungimea structurii sockaddr_in */
  pthread_t idResolverThread;

  numberTrains = grabDataFromFile(fleet);

  /* creare socket */
  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("[server] Eroare la socket().\n");
      return errno;
    }

  /*setam pentru socket optiunea SO_REUSEADDR */
  setsockopt(sd, SOL_SOCKET, SO_REUSEADDR,&optval,sizeof(optval));

  /* pregatim structurile de date */
  bzero (&server, sizeof (server));

  /* umplem structura folosita de server */
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl (INADDR_ANY);
  server.sin_port = htons (PORT);

  /* atasam socketul */
  if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1)
    {
      perror ("[server] Eroare la bind().\n");
      return errno;
    }

  /* punem serverul sa asculte daca vin clienti sa se conecteze */
  if (listen (sd, 5) == -1)
    {
      perror ("[server] Eroare la listen().\n");
      return errno;
    }

  /* completam multimea de descriptori de citire */
  FD_ZERO (&actfds);		/* initial, multimea este vida */
  FD_SET (sd, &actfds);		/* includem in multime socketul creat */

  tv.tv_sec = 1;		/* se va astepta un timp de 1 sec. */
  tv.tv_usec = 0;

  /* valoarea maxima a descriptorilor folositi */
  nfds = sd;

  printf ("[server] Asteptam la portul %d...\n", PORT);
  fflush (stdout);

  pthread_create(&idResolverThread,NULL,&resolver,NULL);

  /* servim in mod concurent clientii... */
  while (1)
    {
      /* ajustam multimea descriptorilor activi (efectiv utilizati) */
      bcopy ((char *) &actfds, (char *) &readfds, sizeof (readfds));

      /* apelul select() */
      if (select (nfds+1, &readfds, NULL, NULL, &tv) < 0)
	{
	  perror ("[server] Eroare la select().\n");
	  return errno;
	}
      /* vedem daca e pregatit socketul pentru a-i accepta pe clienti */
      if (FD_ISSET (sd, &readfds))
	{
	  /* pregatirea structurii client */
	  len = sizeof (from);
	  bzero (&from, sizeof (from));

	  /* a venit un client, acceptam conexiunea */
	  client = accept (sd, (struct sockaddr *) &from, &len);

	  /* eroare la acceptarea conexiunii de la un client */
	  if (client < 0)
	    {
	      perror ("[server] Eroare la accept().\n");
	      continue;
	    }

          if (nfds < client) /* ajusteaza valoarea maximului */
            nfds = client;

	  /* includem in lista de descriptori activi si acest socket */
	  FD_SET (client, &actfds);

	  printf("[server] S-a conectat clientul cu descriptorul %d, de la adresa %s.\n",client, conv_addr (from));
	  fflush (stdout);
	}
      /* vedem daca e pregatit vreun socket client pentru a trimite raspunsul */
      for (fd = 0; fd <= nfds; fd++)	/* parcurgem multimea de descriptori */
	{
	  /* este un socket de citire pregatit? */
	  if (fd != sd && FD_ISSET (fd, &readfds))
	    { //blocam threadurile cand citim comanda/scriem in coada
        pthread_mutex_lock(&mlock);
        int q = PutCommnadInList(fd);
        pthread_mutex_unlock(&mlock);
	      if (q > 0)
		        {

		        }
        else if (q == -1)
          { //inchidem conexiunea cu serverul
             printf ("[server] S-a deconectat clientul cu descriptorul %d.\n",fd);
		         fflush (stdout);
		         close (fd);		/* inchidem conexiunea cu clientul */
		         FD_CLR (fd, &actfds);/* scoatem si din multime */

          }

	    }
	}			/* for */
    }				/* while */
}				/* main */

/* realizeaza primirea si retrimiterea unui mesaj unui client */
int PutCommnadInList(int fd)
{
  char buffer[100];		/* mesajul */
  int bytes;			/* numarul de octeti cititi numar intors de functie si folosit
  pentru verificarea terminarii conexiunii */
  char msg[100];		//mesajul primit de la client
  char msgrasp[100]=" ";        //mesaj de raspuns pentru client

  bytes = read (fd, msg, sizeof (buffer)); //citim comanda din buffer
  if (bytes < 0)
    {
      perror ("Eroare la read() de la client.\n");
      return 0;
    }
  printf ("[server]Mesajul a fost receptionat...%s\n", msg);

  /*pregatim mesajul de raspuns */

  if (strcmp(msg,"quit") == 0)
    {
      return -1; //returnam -1 in caz de deconectare de la client
    }
  if (msg[0]=='&' || msg[0]=='#' || msg[0]=='^' || msg[0] =='*')
    {
      int i=0;
      while (commandStatus[i] != 0 )
          i++; //cautam o pozitie in coada care sa nu fie ocupata
      strcpy(commands[i],msg); //punem comanda in coada
      commandStatus[i] = fd; //punem in coada de control descriptorul (pt trimitere date)
      return bytes; //intoarcem nr de biti cititi

    }
  return -1;

}

void executeCommand(int i)
{
  //cream un nou Invoker contine toate comenzile
  Invoker *newInvoker = new Invoker(commandStatus[i]);
  //decidem ce tip de comanda este
  if (commands[i][0] == '&')
      newInvoker->request->execute(commands[i],fleet,numberTrains);
  else if (commands[i][0] == '#')
      newInvoker->update ->execute(commands[i],fleet,numberTrains);
  else if (commands[i][0] == '*')
      newInvoker->panel -> execute(commands[i],fleet,numberTrains);
  else
      newInvoker->goTrain->execute(commands[i],fleet,numberTrains);
  commandStatus[i] = 0;

}
pthread_t trd;
void *resolver(void *arg)
/*o functie care citeste din coada comenzi si creeaza noi threaduri pentru fiecare
comanda*/
{
  while (1)
  {
    for (int i=0;i<100;i++)
    {
      if (commandStatus[i] != 0)
      {
        std::thread th(executeCommand,i); //cream noi threaduri
        th.detach(); //detasam threadurile de firul principal pentru a nu ne opri programul
      }
  }
    sleep(1); // timer intre verificari ale cozii pt noi comenzi
  }

}
