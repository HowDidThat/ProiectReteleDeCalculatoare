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


int numberDisplayed = 5;
int poz = 0;
int maxTrains;
void listInfo();
void UpdateInfo(int poz);
struct RequestedData requestedData;
bool run = true;
struct info
{
  char ID[15];
  char startStation[20];
  char endStation[20];
  char delay[8];
  char duration[10];
  char timeStart[10];
  char whereTrain[40];
  char expectedTime[8];
} Trains[50];
int nr;
void makeEstimate(int k)
{
 char t[10],l[8],d[8],exp[8];
 int time=0;
 strcpy(t,Trains[k].timeStart);
 strcpy(l,Trains[k].delay);
 strcpy(d,Trains[k].duration);

 time = time + (t[3] - '0');

 time = time + (t[2] - '0')*10;

 time = time + (t[1] - '0')*60;

 time = time + (t[0] - '0')*600;

 if (l[1] <='9' && l[1]>='0')
    time = time + (l[1] - '0');

 time = time + ((l[0] - '0') *10);

 if (d[1] <='9' && d[1]>='0')
 time = time + (d[1] - '0');

 time = time + ((d[0] - '0') *10);

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
 Trains[k].timeStart[len+1] = Trains[k].timeStart[len];
 Trains[k].timeStart[len] = Trains[k].timeStart[len-1];
 Trains[k].timeStart[len-1] = Trains[k].timeStart[len-2];
 Trains[k].timeStart[len-2] = ':';
}


void sort()
{
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
{
  pthread_mutex_lock(&mlock);
  char msg[100];
  bzero(msg,100);
  makeMessage(msg,requestedData);
  if (write (3,msg,sizeof(msg)) <= 0)
    {
      perror ("[client]Eroare la write() spre server.\n");
      exit (errno);
    }

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
    printf("%s\n",recieved);

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

static void requestData (GtkWidget *widget, gpointer  data)
{

  const char *text1,*text2,*text3;
  text1 = gtk_editable_get_text(GTK_EDITABLE(entry1));
  text2 = gtk_editable_get_text(GTK_EDITABLE(entry2));
  text3 = gtk_editable_get_text(GTK_EDITABLE(entry3));
  if (text1[0] == 0)
    strcpy(requestedData.ID,"0");
  else
    memcpy(requestedData.ID,text1,strlen(text1)+1);
  if (text2[0] == 0)
      strcpy(requestedData.startStation,"0");
    else
      memcpy(requestedData.startStation,text2,strlen(text2)+1);
  if (text3[0] == 0)
    strcpy(requestedData.timeStart,"0");
  else
    memcpy(requestedData.timeStart,text3,strlen(text3)+1);

  GetData();

}


static void Down (GtkWidget *widget, gpointer  data)
{
  g_print("UP\n");
  if (poz < maxTrains + 5)
    poz++;
  UpdateInfo(poz);
}

static void Up (GtkWidget *widget, gpointer  data)
{
  g_print("DOWN\n");
  if (poz > 0 )
    poz--;
  UpdateInfo(poz);
}

static void EndProgram(GtkWidget *widget, gpointer  data)
{
  printf("quit\n");
  write(3,"quit",sizeof("quit"));
  //destroy(window);
  run = 0;
}



static void SendDelay(GtkWidget *widget, gpointer data)
{
  const char *text1,*text2;
  text1 = gtk_editable_get_text(GTK_EDITABLE(entry4));
  text2 = gtk_editable_get_text(GTK_EDITABLE(entry5));
  g_print ("%s\t%s\n",text1,text2);
  char delayData[100];
  bzero(delayData,100);
  strcpy(delayData,"#");
  if (strcmp(text1,"") != 0)
    strcat(delayData,text1);
  else
    strcat(delayData,"0");
  strcat(delayData,"#");
  if (strcmp(text2,"") != 0)
    strcat(delayData,text2);
  else
    strcat(delayData,"0");
  strcat(delayData,"#");
  write(3,delayData,sizeof(delayData));

}

void makeButtons(gpointer user_data)
{

  button3 = gtk_button_new_with_label ("Request Data ");
  g_signal_connect (button3, "clicked", G_CALLBACK (requestData),entry3);
  gtk_grid_attach (GTK_GRID (grid), button3, 0, 3, 2, 1);

  buttonDelay = gtk_button_new_with_label ("SendDelay");
  g_signal_connect (buttonDelay,"clicked", G_CALLBACK (SendDelay),user_data);
  gtk_grid_attach (GTK_GRID (grid), buttonDelay, 0, 6, 2, 1);

  buttonUp = gtk_button_new_with_label("Up");

  g_signal_connect(buttonUp,"clicked",G_CALLBACK(Up),window);
  gtk_grid_attach (GTK_GRID (grid), buttonUp, 2, 0, 20, 1);

  buttonDown = gtk_button_new_with_label( "Down" );
  g_signal_connect(buttonDown,"clicked",G_CALLBACK(Down),window);
  gtk_grid_attach (GTK_GRID (grid), buttonDown, 2, 8, 20, 1);
}

void makeEntry()
{

    entry1  = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry1),"TrainID");
    gtk_grid_attach (GTK_GRID (grid),entry1,0,0,2,1);

    entry2  = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry2),"Start Station");
    gtk_grid_attach (GTK_GRID (grid),entry2,0,1,2,1);

    entry3  = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry3),"Start Hour");
    gtk_grid_attach (GTK_GRID (grid),entry3,0,2,2,1);

    entry4  = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry4),"TrainID(delay)");
    gtk_grid_attach (GTK_GRID (grid),entry4,0,4,2,1);

    entry5  = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry5),"Delay(minutes)");
    gtk_grid_attach (GTK_GRID (grid),entry5,0,5,2,1);




}
void makeLabels()
{ label0 = gtk_label_new("      ID tren     ");
  label1 = gtk_label_new("   Statie Plecare  ");
  label2 = gtk_label_new("   Statie Sosir    ");
  label3 = gtk_label_new("   Timp plecare    ");
  label4 = gtk_label_new("     Intarziere    ");
  label5 = gtk_label_new("       Durata      ");
  label6 = gtk_label_new("    Timp sosire    ");
  label7 = gtk_label_new("    Locatie Tren   ");
gtk_grid_attach (GTK_GRID (grid), label0, 3, 1, 1, 1);
gtk_grid_attach (GTK_GRID (grid), label1, 4, 1, 1, 1);
gtk_grid_attach (GTK_GRID (grid), label2, 5, 1, 1, 1);
gtk_grid_attach (GTK_GRID (grid), label3, 6, 1, 1, 1);
gtk_grid_attach (GTK_GRID (grid), label4, 7, 1, 1, 1);
gtk_grid_attach (GTK_GRID (grid), label5, 8, 1, 1, 1);
gtk_grid_attach (GTK_GRID (grid), label6,10, 1, 1, 1);
gtk_grid_attach (GTK_GRID (grid), label7, 9, 1, 1, 1);

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
{
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
{
  GtkWidget *space = gtk_label_new("   ");

  /* create a new window, and set its title */
  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Do you even lift?");

  /* Here we construct the container that is going pack our buttons */
  grid = gtk_grid_new ();

  /* Pack the container in the window */
  gtk_window_set_child (GTK_WINDOW (window), grid);

  makeButtons(user_data);
  makeEntry();
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
    strcpy(requestedData.ID,"0");
      strcpy(requestedData.startStation,"0");
        strcpy(requestedData.timeStart,"0");

  app = gtk_application_new (NULL, G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc,argcv);
  g_object_unref (app);
  return status;
}
