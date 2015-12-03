#!/bin/bash


# CSV1
g++ *.cpp -o csv1_run
./csv1_run > csv1_run.txt

# CSV2
find . -name "memControl.cpp" -exec sed -i "s|CSV1|CSV2|g" {} +
find . -name "memControl.cpp" -exec sed -i "s|csv_1|csv_2|g" {} +
g++ *.cpp -o csv2_run
./csv2_run > csv2_run.txt

# CSV3
find . -name "memControl.cpp" -exec sed -i "s|CSV2|CSV3|g" {} +
find . -name "memControl.cpp" -exec sed -i "s|csv_2|csv_3|g" {} +
g++ *.cpp -o csv3_run
./csv3_run > csv3_run.txt

# Reset to CSV1
find . -name "memControl.cpp" -exec sed -i "s|CSV3|CSV1|g" {} +
find . -name "memControl.cpp" -exec sed -i "s|csv_3|csv_1|g" {} +
