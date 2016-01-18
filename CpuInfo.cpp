/********************************************************************


Purpose:   The program reads from the /proc/cpuinfo file and writes into the
			output file named cpuinfo.txt



*********************************************************************/
#include <fstream>
#include <iostream>					//header files
#include <string>
#include <iomanip>

using namespace std;
int main()
{
	ifstream infile;
	ofstream outfile;								//Variable declaration
	string str;
	infile.open("/proc/cpuinfo");					//reading from the input file
	outfile.open("cpuinfo.txt");					//writing to the output file
	if(infile.fail())
	{
		cout << "Unable to open the file\n";				//Failure to open the input file
	  	return 0;	
	}
	while(true)										//looping till the end of file is reached
	{
	if(infile.eof())								//checking end of file
		{
			break;		// If we reach end of file break out of this loop
	  	}
	getline(infile, str);									//writing from the file to the string
	outfile<<str<<endl;										//writing to the output file
}
return 0;									//returning zero
}
