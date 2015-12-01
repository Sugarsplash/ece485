#include "csv.h"

//First csv file
int csv_1(Info Array1[CSV1]) {

    string Line;			//Holds a line from the csv file
    ifstream csv_file;			//Create the file stream

    csv_file.open("./csv_files/first_scenario.csv");

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
int csv_2(Info Array2[CSV2]) {

    string Line;                        //Holds a line from the csv file
    ifstream csv_file;                  //Create the file stream

    csv_file.open("./csv_files/second_scenario.csv");

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
int csv_3(Info Array3[CSV3]) {

    string Line;                        //Holds a line from the csv file
    ifstream csv_file;                  //Create the file stream

    csv_file.open("./csv_files/third_scenario.csv");

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

/*Debugging

    printf("%d, %d, %d, %d, %d.\n", Data[Line_Number].time, 
                                    Data[Line_Number].device,
                                    Data[Line_Number].operation,                                        
                                    Data[Line_Number].ts, 
                                    Data[Line_Number].tr_data_tag);

*/
    return 0;

}