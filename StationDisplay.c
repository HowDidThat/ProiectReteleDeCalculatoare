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

// toate campurile care vor fi folosite la afisare
GtkWidget *window;
GtkWidget *grid;
GtkWidget *button1, *button2, *button3, *button;
GtkWidget *entry1, *entry2, *entry3;
GtkWidget *view1, *view2, *view3;
GtkTextBuffer *buffer1, *buffer2, *buffer3;
GtkWidget *buttonDelay;
GtkWidget *entry4, *entry5;
GtkWidget *buttonUp, *buttonDown;
GtkWidget *spaceUp, *spaceDown;
GtkWidget *label0, *label1, *label2, *label3, *label4, *label5, *label6, *label7;
struct thing
{
GtkWidget *train0, *train1, *train2, *train3, *train4, *train5, *train6, *train7;
} Info[5];


int poz = 0; //poz la care se incepe afisarea
int maxTrains;
void listInfo();
void UpdateInfo(int poz);
struct RequestedData requestedData;
bool run = true;
struct info{
  char ID[15];
  char startStation[20];
  char endStation[20];
  char delay[8];
  char duration[10];
  char timeStart[10];
  char whereTrain[40];
  char expectedTime[8];
} Trains[50]; // datele despre fiecare tren
int nr = 0;


void makeEstimate(int nr)
{//adauga la ora_start + intarziere + durata
 char t[10],l[8],d[8],exp[8];
 int time=0;
 strcpy(t,Trains[nr].timeStart);
 strcpy(l,Trains[nr].delay);
 strcpy(d,Trains[nr].duration);
 time = time + (t[3] - '0');
 time = time + (t[2] - '0')*10;
 time = time + (t[1] - '0')*60;
 time = time + (t[0] - '0')*600;
 if (l[1] <='9' && l[1]>='0')
    time = time + (l[1] - '0');
 time = time + (l[0] - '0') *10;
 if (d[1] <='9' && d[1]>='0')
    time = time + (d[1] - '0');
 time = time + (d[0] - '0') *10;

 time = time % 14400;
 bzero(exp,8);
 exp[0] = time / 600 + '0';
 exp[1] = time % 600 / 60 + '0';
 exp[2] = ':';
 exp[3] = time % 60 / 10 + '0';
 exp[4] = time % 60 % 10 + '0';
 exp[5] = '\0';
 strcpy(Trains[nr].expectedTime,exp);
 int len = strlen(Trains[nr].timeStart);
 Trains[nr].timeStart[len+1] = Trains[nr].timeStart[len];
 Trains[nr].timeStart[len] = Trains[nr].timeStart[len-1];
 Trains[nr].timeStart[len-1] = Trains[nr].timeStart[len-2];
 Trains[nr].timeStart[len-2] = ':';
}


void sort()
{ //sorteaza instantele dupa ora plecare
  printf("enteringSort\n");
  struct info aux;
  for (int i=0;i<nr-1;i++)
    for (int j=i+1;j<nr;j++)
  {
    if (strcmp(Trains[i].timeStart,Trains[j].timeStart) > 0 && Trains[i].timeStart[0] != '-'&& Trains[i].timeStart[0] != '-'&& Trains[j].timeStart[0] != '-')
      {
        aux = Trains[i];
        Trains[i] = Trains[j];
        Trains[j] = aux;
      }
  }
}









pthread_mutex_t mlock=PTHREAD_MUTEX_INITIALIZER;
void GetData()
{ //cere de la server informatii
  pthread_mutex_lock(&mlock);
  char msg[100];
  bzero(msg,100);
  makeMessageInfotaiment(msg,requestedData);
  if (write (3,msg,sizeof(msg)) <= 0)
    {
      perror ("[client]Eroare la write() spre server.\n");
      exit (errno);
    }
  //printf("%s\n",msg);
  char recieved[100];
  bzero(recieved,100);
  nr = 0;

  while(1)
  {
    if (read (3,recieved,sizeof(recieved)) < 0)
      {
        perror ("[client]Eroare la read() de la server data.\n");
        exit (errno);
      }
    if (strcmp(recieved,"EOT") == 0)
      break;
    //punem informatiile in structura fiecaruia
    char *s;
    s = strtok(recieved,"$");
    strcpy(Trains[nr].ID,s);

    s=strtok(NULL,"$");
    strcpy(Trains[nr].startStation,s);

    s = strtok(NULL,"$");
    strcpy(Trains[nr].endStation,s);

    s = strtok(NULL,"$");
    strcpy(Trains[nr].delay,s);

    s = strtok(NULL,"$");
    strcpy(Trains[nr].timeStart,s);

    s = strtok(NULL,"$");
    strcpy(Trains[nr].duration,s);

    s = strtok(NULL,"$");
    strcpy(Trains[nr].whereTrain,s);
    makeEstimate(nr);

    nr++;
  }
  //restul structurii este lasata goala
  for (nr;nr<50;nr++)
    {
    strcpy(Trains[nr].ID,"-");
    strcpy(Trains[nr].startStation,"-");
    strcpy(Trains[nr].endStation,"-");
    strcpy(Trains[nr].delay,"-");
    strcpy(Trains[nr].timeStart,"-");
    strcpy(Trains[nr].duration,"-");
    strcpy(Trains[nr].whereTrain,"-");
    }

  pthread_mutex_unlock(&mlock);
  sort();
  UpdateInfo(poz);
}

static void EndProgram(GtkWidget *widget, gpointer  data)
{ //inchidem programul
  printf("quit\n");
  write(3,"quit",sizeof("quit"));
  //destroy(window);
  run = 0;
}

void makeLabels()
{ //creem capurile de tabel
  label0 = gtk_label_new("      ID tren     ");
  label1 = gtk_label_new("   Statie Plecare  ");
  label2 = gtk_label_new("   Statie Sosire   ");
  label3 = gtk_label_new("   Timp plecare    ");
  label4 = gtk_label_new("     Intarziere    ");
  label5 = gtk_label_new("       Durata      ");
  label6 = gtk_label_new("    Timp sosire    ");
  label7 = gtk_label_new("    Locatie Tren   ");
  //pozitionam capurile de tabel
gtk_grid_attach (GTK_GRID (grid), label0, 3, 1, 1, 1);
gtk_grid_attach (GTK_GRID (grid), label1, 4, 1, 1, 1);
gtk_grid_attach (GTK_GRID (grid), label2, 5, 1, 1, 1);
gtk_grid_attach (GTK_GRID (grid), label3, 6, 1, 1, 1);
gtk_grid_attach (GTK_GRID (grid), label4, 7, 1, 1, 1);
gtk_grid_attach (GTK_GRID (grid), label5, 8, 1, 1, 1);
gtk_grid_attach (GTK_GRID (grid), label6,10, 1, 1, 1);
gtk_grid_attach (GTK_GRID (grid), label7, 9, 1, 1, 1);
//creem celulele tabelului
  for (int i=0;i<5;i++)
    {
      Info[i].train0 = gtk_label_new("-");
      Info[i].train1 = gtk_label_new("-");
      Info[i].train2 = gtk_label_new("-");
      Info[i].train3 = gtk_label_new("-");
      Info[i].train4 = gtk_label_new("-");
      Info[i].train5 = gtk_label_new("-");
      Info[i].train6 = gtk_label_new("-");
      Info[i].train7 = gtk_label_new("-");
    }
  //punem in tabel infor
  for (int i=0;i<5;i++)
    {int q=1;
     gtk_grid_attach (GTK_GRID (grid), Info[i].train0, 2+q, 2+i, 1, 1);
     q++;
     gtk_grid_attach (GTK_GRID (grid), Info[i].train1, 2+q, 2+i, 1, 1);
     q++;
     gtk_grid_attach (GTK_GRID (grid), Info[i].train2, 2+q, 2+i, 1, 1);
     q++;
     gtk_grid_attach (GTK_GRID (grid), Info[i].train3, 2+q, 2+i, 1, 1);
     q++;
     gtk_grid_attach (GTK_GRID (grid), Info[i].train4, 2+q, 2+i, 1, 1);
     q++;
     gtk_grid_attach (GTK_GRID (grid), Info[i].train5, 2+q, 2+i, 1, 1);
     q++;
     gtk_grid_attach (GTK_GRID (grid), Info[i].train6, 2+q, 2+i, 1, 1);
     q++;
     gtk_grid_attach (GTK_GRID (grid), Info[i].train7, 2+q, 2+i, 1, 1);
    }

}

void UpdateInfo(int poz)
{ // actualizam informatiile din tabel
for (int i=0;i<5;i++)
  {
    gtk_label_set_text(GTK_LABEL (Info[i].train0),Trains[poz + i].ID);
    gtk_label_set_text(GTK_LABEL (Info[i].train1),Trains[poz + i].startStation);
    gtk_label_set_text(GTK_LABEL (Info[i].train2),Trains[poz + i].endStation);
    gtk_label_set_text(GTK_LABEL (Info[i].train3),Trains[poz + i].timeStart);
    gtk_label_set_text(GTK_LABEL (Info[i].train5),Trains[poz + i].duration);
    gtk_label_set_text(GTK_LABEL (Info[i].train4),Trains[poz + i].delay);
    gtk_label_set_text(GTK_LABEL (Info[i].train6),Trains[poz + i].whereTrain);
    gtk_label_set_text(GTK_LABEL (Info[i].train7),Trains[poz + i].expectedTime);

  }
}

static void activate (GtkApplication *app,gpointer user_data)
{ //pornum programul
  GtkWidget *space = gtk_label_new("   ");

  //cream nou window
  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "What station ?");

  //construim un widget care pastreaza toate celelalte widgeturi
  grid = gtk_grid_new ();

  gtk_window_set_child (GTK_WINDOW (window), grid);

  makeLabels();

  button = gtk_button_new_with_label ("Quit");
  g_signal_connect_swapped (button, "clicked", G_CALLBACK (EndProgram), window);
  gtk_grid_attach (GTK_GRID (grid), button, 0, 7, 2, 2);


  gtk_grid_set_column_homogeneous(GTK_GRID(grid),FALSE);

  gtk_widget_show (window);

}

int StartGUI (int argc, char **argcv)
{
  GtkApplication *app;
  int status;


  app = gtk_application_new (NULL, G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc,argcv);
  g_object_unref (app);
  return status;
}



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


  if (argc != 4)
    {
      printf ("Sintaxa: %s <adresa_server> <port> <station>\n", argv[0]);
      return -1;
    }

  /* stabilim portul */
  port = atoi (argv[2]);
  //umplem structura cu date necesare (in functie de statie request)
  strcpy(requestedData.ID,"0");
  strcpy(requestedData.timeStart,"0");
  strcpy(requestedData.startStation,argv[3]);
  printf("%s\n",argv[3]);
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



    //threadul care este responsabil de gui
    int rc;
    rc = pthread_create(&threads[0],NULL,GUIClient,(void*)sd);
    if (rc) {
       printf("Error:unable to create thread2, %d", rc);
       exit(-1);
    }
  printf("endProgram\n");
  int sec=0;
  while (run)
  { //o lupa infinita care face update la informatii dupa un timp setat
    sleep(updateTimer);
    if (run)
      GetData();

  }
  return 0;
}
