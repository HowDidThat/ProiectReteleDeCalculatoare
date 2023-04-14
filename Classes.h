//o comanda pur virtuala
class BaseCommand
{
public:
  virtual void execute(char str[100],Train *fleet,int numberTrains){};

};
//comanda care se ocupa de display din Statie
class Panel : public BaseCommand
{
  private:
  int socketDescriptor;
  RequestedData request;
public:
  Panel(int sd) {socketDescriptor = sd;};
  void execute(char str[100],Train *fleet,int numberTrains)
  {
    char *data;
    data = strtok(str,"*");
    strcpy(request.ID,data);
    data = strtok(NULL,"*");
    strcpy(request.startStation,data);
    data = strtok(NULL,"*");
    strcpy(request.timeStart,data);
    char dataToSend[100];
      printf ("Request by Station \n");
      for (int i=0;i<=numberTrains;i++)
        for (int j=0;j<=fleet[i].numberStations;j++)
          {
            if (strcmp(fleet[i].trips[j].startStation,request.startStation) == 0 && fleet[i].whereTrain/2 <= j
                ||strcmp(fleet[i].trips[j].endStation,request.startStation) == 0 && (fleet[i].whereTrain+1)/2 <= j)
              {
                bzero(dataToSend,100);
                dataToSend[0] = '\0';
                strcpy(dataToSend,"$");
                strcat(dataToSend,fleet[i].ID);
                strcat(dataToSend,"$");
                strcat(dataToSend,fleet[i].trips[j].startStation);
                strcat(dataToSend,"$");
                strcat(dataToSend,fleet[i].trips[j].endStation);
                strcat(dataToSend,"$");
                strcat(dataToSend,fleet[i].trips[j].delay);
                strcat(dataToSend,"$");
                strcat(dataToSend,fleet[i].trips[j].timeStart);
                strcat(dataToSend,"$");
                strcat(dataToSend,fleet[i].trips[j].duration);
                strcat(dataToSend,"$");
                int d = fleet[i].whereTrain;
                if (d % 2 == 0 && d < fleet[i].numberStations*2)
                  {strcat(dataToSend,fleet[i].trips[d/2].startStation);}
                else if (d < fleet[i].numberStations*2)
                  {
                    strcat(dataToSend,fleet[i].trips[d/2].startStation);
                    strcat(dataToSend,"-");
                    strcat(dataToSend,fleet[i].trips[d/2].endStation);
                  }
                else if (d == fleet[i].numberStations*2)
                  {
                    strcat(dataToSend,fleet[i].trips[fleet[i].numberStations-1].endStation);
                  }
                else
                  {
                    strcat(dataToSend,"End of track");
                  }
                if (write(socketDescriptor,dataToSend,sizeof(dataToSend))<0)
                {
                  printf("Write Error\n");

                }
              }

          }
      if (write(socketDescriptor,"EOT",sizeof("EOT"))<0)
      {
        printf("Write Error\n");

      }
  };

};
//comanda care se ocupa de cerere de informatii
class Request : public BaseCommand
{
private:
  RequestedData request;
  int socketDescriptor;
public:

  Request(int sd) {socketDescriptor = sd;};
  void execute(char str[100],Train *fleet,int numberTrains)
  {

    //  printf("Requst %s\n",str);
      char *data;
      data = strtok(str," &");
      strcpy(request.ID,data);
      data = strtok(NULL," &");
      strcpy(request.startStation,data);
      data = strtok(NULL," &");
      strcpy(request.timeStart,data);
      if (strcmp(request.ID,"0") != 0)
        {
          Id(fleet,numberTrains);
        }
      else if(strcmp(request.startStation,"0") != 0)
        {
          Station(fleet,numberTrains);
        }
      else
        {
          Hour(fleet,numberTrains);
        }
  };

  void Id(Train *fleet,int numberTrains)
  {
    printf("request by ID to  %d\n",socketDescriptor);
    char dataToSend[100];
    for (int i=0;i<numberTrains;i++)
      {
        if (strcmp(fleet[i].ID,request.ID) == 0)
          {for (int j = 0;j<fleet[i].numberStations;j++)
            {
              bzero(dataToSend,100);
              dataToSend[0] = '\0';
              strcpy(dataToSend,"$");
              strcat(dataToSend,fleet[i].ID);
              strcat(dataToSend,"$");
              strcat(dataToSend,fleet[i].trips[j].startStation);
              strcat(dataToSend,"$");
              strcat(dataToSend,fleet[i].trips[j].endStation);
              strcat(dataToSend,"$");
              strcat(dataToSend,fleet[i].trips[j].delay);
              strcat(dataToSend,"$");
              strcat(dataToSend,fleet[i].trips[j].timeStart);
              strcat(dataToSend,"$");
              strcat(dataToSend,fleet[i].trips[j].duration);
              strcat(dataToSend,"$");
              int d = fleet[i].whereTrain;
              if (d % 2 == 0 && d < fleet[i].numberStations*2)
                {strcat(dataToSend,fleet[i].trips[d/2].startStation);}
              else if (d < fleet[i].numberStations*2)
                {
                  strcat(dataToSend,fleet[i].trips[d/2].startStation);
                  strcat(dataToSend,"-");
                  strcat(dataToSend,fleet[i].trips[d/2].endStation);
                }
              else if (d == fleet[i].numberStations*2)
                {
                  strcat(dataToSend,fleet[i].trips[fleet[i].numberStations-1].endStation);
                }
              else
                {
                  strcat(dataToSend,"End of track");
                }
              strcat(dataToSend,"$");
              if (write(socketDescriptor,dataToSend,sizeof(dataToSend))<0)
              {
                printf("Write Error\n");

              }
            }
            i = numberTrains;
          }
      }
    if (write(socketDescriptor,"EOT",sizeof("EOT"))<0)
    {
      printf("Write Error\n");

    }
  };
  void Station(Train *fleet,int numberTrains)
  { char dataToSend[100];
    printf ("Request by Station \n");
    for (int i=0;i<=numberTrains;i++)
      for (int j=0;j<=fleet[i].numberStations;j++)
        {
          if (strcmp(fleet[i].trips[j].startStation,request.startStation) == 0)
            {
              bzero(dataToSend,100);
              dataToSend[0] = '\0';
              strcpy(dataToSend,"$");
              strcat(dataToSend,fleet[i].ID);
              strcat(dataToSend,"$");
              strcat(dataToSend,fleet[i].trips[j].startStation);
              strcat(dataToSend,"$");
              strcat(dataToSend,fleet[i].trips[j].endStation);
              strcat(dataToSend,"$");
              strcat(dataToSend,fleet[i].trips[j].delay);
              strcat(dataToSend,"$");
              strcat(dataToSend,fleet[i].trips[j].timeStart);
              strcat(dataToSend,"$");
              strcat(dataToSend,fleet[i].trips[j].duration);
              strcat(dataToSend,"$");
              int d = fleet[i].whereTrain;
              if (d % 2 == 0 && d < fleet[i].numberStations*2)
                {strcat(dataToSend,fleet[i].trips[d/2].startStation);}
              else if (d < fleet[i].numberStations*2)
                {
                  strcat(dataToSend,fleet[i].trips[d/2].startStation);
                  strcat(dataToSend,"-");
                  strcat(dataToSend,fleet[i].trips[d/2].endStation);
                }
              else if (d == fleet[i].numberStations*2)
                {
                  strcat(dataToSend,fleet[i].trips[fleet[i].numberStations-1].endStation);
                }
              else
                {
                  strcat(dataToSend,"End of track");
                }
              if (write(socketDescriptor,dataToSend,sizeof(dataToSend))<0)
              {
                printf("Write Error\n");

              }
            }

        }
    if (write(socketDescriptor,"EOT",sizeof("EOT")) < 0)
    {
      printf("Write Error\n");

    }

  };
  void Hour(Train *fleet,int numberTrains)
  {

    printf("Request by hour \n");
    char dataToSend[100];
     for (int i=0;i<=numberTrains;i++)
       for (int j=0;j<=fleet[i].numberStations;j++)
         {
           if (fleet[i].trips[j].timeStart[0] == request.timeStart[0] &&
               fleet[i].trips[j].timeStart[1] == request.timeStart[1])
             {
               bzero(dataToSend,100);
               dataToSend[0] = '\0';
               strcpy(dataToSend,"$");
               strcat(dataToSend,fleet[i].ID);
               strcat(dataToSend,"$");
               strcat(dataToSend,fleet[i].trips[j].startStation);
               strcat(dataToSend,"$");
               strcat(dataToSend,fleet[i].trips[j].endStation);
               strcat(dataToSend,"$");
               strcat(dataToSend,fleet[i].trips[j].delay);
               strcat(dataToSend,"$");
               strcat(dataToSend,fleet[i].trips[j].timeStart);
               strcat(dataToSend,"$");
               strcat(dataToSend,fleet[i].trips[j].duration);
               strcat(dataToSend,"$");
               int d = fleet[i].whereTrain;
               if (d % 2 == 0 && d < fleet[i].numberStations*2)
                 {strcat(dataToSend,fleet[i].trips[d/2].startStation);}
               else if (d < fleet[i].numberStations*2)
                 {
                   strcat(dataToSend,fleet[i].trips[d/2].startStation);
                   strcat(dataToSend,"-");
                   strcat(dataToSend,fleet[i].trips[d/2].endStation);
                 }
               else if (d == fleet[i].numberStations*2)
                 {
                   strcat(dataToSend,fleet[i].trips[fleet[i].numberStations-1].endStation);
                 }
               else
                 {
                   strcat(dataToSend,"End of track");
                 }
               if (write(socketDescriptor,dataToSend,sizeof(dataToSend)) < 0)
                {
                  printf("Write Error\n");
                  
                }
             }
         }
      if (write(socketDescriptor,"EOT",sizeof("EOT"))<0)
      {
        printf("Write Error\n");

      }
   };

};
//comanda care se ocupa de introducerea intarzierilor in baza de date
class Update : public BaseCommand
{
private:
  int socketDescriptor;
public:
    Update(int sd) {socketDescriptor = sd;};

  void execute(char str[100],Train *fleet,int numberTrains)
  {
    char id[10],delay[10],*s;
    s = strtok(str,"#");
    strcpy(id,s);
    s = strtok(NULL,"#");
    strcpy(delay,s);
    printf("ID Is : %s\n",id);
    for (int i=0;i<numberTrains;i++)
      {
        if (strcmp(fleet[i].ID,id) == 0)
          {
            for (int j=fleet[i].whereTrain/2;j<fleet[i].numberStations;j++)
              strcpy(fleet[i].trips[j].delay,delay);
            printf("Deppends %s \n",fleet[i].ID);
          }
      }
  };
};
//comanda care muta trenul de la o statie la alta
class GoTrain : public BaseCommand
{
  public:
  void execute(char str[20],Train *fleet, int numberTrains)
    {
      char *trainID;
      trainID = strtok(str,"^");
      for (int i=0; i<numberTrains;i++)
      {
        if (strcmp(fleet[i].ID,trainID) == 0)
          {
              fleet[i].whereTrain ++;
              i = numberTrains;
          }
      }
    }
};

//comanda principala care gazduieste cate o instanta a fiecarei comenzi
class Invoker
{
private:
  int socketDescriptor;
public:
  Request *request ;
  Update  *update ;
  GoTrain *goTrain;
  Panel *panel;
  Invoker(int sd) {
  socketDescriptor = sd; printf("Invoker Socket %d \n",socketDescriptor);
  request = new Request(socketDescriptor);
  update = new Update(socketDescriptor);
  goTrain = new GoTrain();
  panel = new Panel(socketDescriptor);
};


};
