void cpyData(char data[100],char *line,int readSize)
{
  for (int i=0;i<readSize-1;i++)
  {
    //transformam char* in char[]
    data[i]=*line;
    line++;
  }
  //ending the string
  data[readSize-1]='\0';
}
/*
  stergem tagurile pentru a ramane doar cu datele relevante
*/
void detag(char data[100])
{
  //stergem tagul de inceput
  strcpy(data,strchr(data,'>')+1);
  int i=0;
  //stergem tagul de final
  while (data[i] != '/')
    i++;
  data[i-1] = '\0';
}

/*
umplem structura fleet cu datele trenurilor din fisierul xml
*/
int grabDataFromFile (struct Train fleet[200])
{
  int numberTrain = 0;
  int numberStations = 0;
  //file descriptor and using fopen to open the file for reading
  FILE* fptr;
  fptr = fopen("Trains.xml","r");
  if (fptr  == NULL)
  {
    //error in case fopen error
    perror ("error at fopen");
    return 0;
  }
  char *line= NULL;
  size_t length= 0; //marimea citita
  int readSize; // marimea fiecarui rand citit
  while ((readSize = getline(&line,&length,fptr)) != -1)
  {

    char data[100];
    //char* ->char[]
    cpyData(data,line,readSize);
    if (strstr(data,"<ID>"))
      {
        detag(data);
        strcpy(fleet[numberTrain].ID,data);
        fleet[numberTrain].numberStations = 0;
        fleet[numberTrain].whereTrain = 0;
      }
    else if (strstr(data,"<timestart>"))
      {
        detag(data);
        strcpy(fleet[numberTrain].trips[numberStations].timeStart,data);
        strcpy(fleet[numberTrain].trips[numberStations].delay,"0");
      }
    else if (strstr(data,"<startStation>"))
      {
        detag(data);
        strcpy(fleet[numberTrain].trips[numberStations].startStation,data);
      }
    else if (strstr(data,"<endStation>"))
      {
          detag(data);
          strcpy(fleet[numberTrain].trips[numberStations].endStation,data);
        }
    else if (strstr(data,"<duration>"))
      {
          detag(data);
          strcpy(fleet[numberTrain].trips[numberStations].duration,data);
      }
    else if (strstr(data,"</trip>"))
      {

        fleet[numberTrain].numberStations++;
        numberStations++;
      }
    else if (strstr(data,"</train>"))
      {
        numberTrain ++;
        numberStations = 0;
      }

    }

return numberTrain;
}
