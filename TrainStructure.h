
/*
structure to keep data for each individual train
*/
struct Trip
{
  char startStation[20]; // the start station of the train up to 20 characters long
  char endStation[20]; // the end station of the train up to 20 characters long
  char delay[4]; // from 0 to 999 minutes
  char timeStart[5]; // from 0 to 24 the hour from 0 to 59 the minute at witch the train leaves
  char duration[5]; // expected duration of the trip
};

struct Train
{
 char ID[10]; //keep an unique id of the train
 struct Trip trips[10]; // the information about the train stations
 int numberStations; //number of stations that the train passes trough
 int whereTrain; // where is the train at the momment
};

struct RequestedData
{
  char ID[10];
  char startStation [20];
  char timeStart [5];

};

struct UpdateData
{
  char ID[10];
  char delay[4];
  char startStation[20];
};
