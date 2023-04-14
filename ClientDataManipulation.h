void makeMessage(char data[100],struct RequestedData requestedData)
{
  bzero(data,100);
  data[0]='\0';
  requestedData.ID[strlen(requestedData.ID)] = '\0';
  requestedData.startStation[strlen(requestedData.startStation)] = '\0';
  requestedData.timeStart[strlen(requestedData.timeStart)] = '\0';
  strcat(data,"&");
  strcat(data,requestedData.ID);
  strcat(data,"&");
  strcat(data,requestedData.startStation);
  strcat(data,"&");
  strcat(data,requestedData.timeStart);
  strcat(data,"&");
}

void makeMessageInfotaiment(char data[100],struct RequestedData requestedData)
{
  bzero(data,100);
  data[0]='\0';
  requestedData.ID[strlen(requestedData.ID)] = '\0';
  requestedData.startStation[strlen(requestedData.startStation)] = '\0';
  requestedData.timeStart[strlen(requestedData.timeStart)] = '\0';
  strcat(data,"*");
  strcat(data,requestedData.ID);
  strcat(data,"*");
  strcat(data,requestedData.startStation);
  strcat(data,"*");
  strcat(data,requestedData.timeStart);
  strcat(data,"*");
}
