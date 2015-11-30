/* This program takes in the data from the three csv files supplied externally.
   The program will parse this data into five interger values and fill in the
   five fields that determine the time (in clock cycles) of the transaction,
   which device is making the transaction, whether a read or write needs to be
   performed, the size of the transaction, and the tag associated with the
   transaction.
*/

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
int csv_1();
int csv_2();
int csv_3();
int parse_line(string, Info*, int);

//Main
int main() {

    //First csv file
    csv_1();

    //Second csv file
    csv_2();

    //Third csv file
    csv_3();

    return 0;

}

//First csv file
int csv_1() {

    Info * Array1 = new Info[CSV1];	//Create array of structs
    string Line;			//Holds a line from the csv file
    ifstream csv_file;			//Create the file stream

    csv_file.open("~/ece485/src/csv_files/first_scenario.csv");

    if (csv_file.is_open()) {
	for (int i = 0; i < CSV1; i++) {
	    getline(csv_file, Line);	//Obtain current line
	    parse_line(Line, Array1, i);//Get data from line in csv file
	}
	csv_file.close();
    }

    return 0;
}

//Second csv file
int csv_2() {

    Info * Array2 = new Info[CSV2];	//Create array of structs
    string Line;                        //Holds a line from the csv file
    ifstream csv_file;                  //Create the file stream

    csv_file.open("~/ece485/src/csv_files/second_scenario.csv");

    if (csv_file.is_open()) {
	for (int i = 0; i < CSV2; i++) {
	    getline(csv_file, Line);	//Obtain current line
	    parse_line(Line, Array2, i);//Get data from line in csv file
	}
	csv_file.close();
    }

    return 0;
}

//Third csv file
int csv_3() {

    Info * Array3 = new Info[CSV3];	//Create array of structs
    string Line;                        //Holds a line from the csv file
    ifstream csv_file;                  //Create the file stream

    csv_file.open("~/ece485/src/csv_files/third_scenario.csv");

    if (csv_file.is_open()) {
	for (int i = 0; i < CSV3; i++) {
	    getline(csv_file, Line);	//Obtain current line
	    parse_line(Line, Array3, i);//Get data from line in csv file
	}
	csv_file.close();
    }

    return 0;
}

//This function splits the given string Line into multiple smaller strings
//that need to be converted to integer values so that they can be passed into
//the struct Info.
int parse_line(string Line, Info * Data, int Line_Number) {

    stringstream ss(Line);	//stream to obtain values separated by commas
    string Time;		//**
    string Device;		//
    string Operation;		// All of the fields in the struct
    string TS;			//
    string Tag;			//**

    //Separate the values in the string by comma
    getline(ss, Time, ',');
    getline(ss, Device, ',');
    getline(ss, Operation, ',');
    getline(ss, TS, ',');
    getline(ss, Tag);

    //Convert the split strings into numbers
    Data[Line_Number].time = atoi(Time.c_str());
    Data[Line_Number].device = atoi(Device.c_str());
    Data[Line_Number].operation = atoi(Operation.c_str());
    Data[Line_Number].ts = atoi(TS.c_str());
    Data[Line_Number].tr_data_tag = atoi(Tag.c_str());

    return 0;

}
