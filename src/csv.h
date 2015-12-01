#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>
using namespace std;

//Length of each csv file
#define CSV1 24
#define CSV2 43
#define CSV3 21

//Structs
struct Info
{
    int time;
    int device;
    int operation;
    int ts;
    int tr_data_tag;
};

//Prototypes
int csv_1(Info Array1[CSV1]);
int csv_2(Info Array2[CSV2]);
int csv_3(Info Array3[CSV3]);
int parse_line(string, Info*, int);
