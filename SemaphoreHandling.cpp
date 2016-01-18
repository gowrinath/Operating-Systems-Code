/***************************************************************
Pipe Communication and semapohore handling
***************************************************************/
#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<iomanip>
#include<fstream>
#include<sys/shm.h>
#include<cstdlib>
#include<sstream>                           
#include<unistd.h>
#include<cstring>
#include<sys/utsname.h>             //header files
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<cstdio>
#include<string>
#include<algorithm>
#include<semaphore.h>
#include<boost/regex.hpp>


using namespace std;

//protoypes
void parentWrite(string bit, int n);
string childRead(int n);


int main()
{
	int n;
	int fd[2];
	cout << unitbuf;
	ifstream inFile;
	string osType;								//variables to store the information to retrieve the file information
	string hostName;
	string osRelease;
	string version;
	struct utsname uName;
	inFile.open("/proc/sys/kernel/ostype");	
	getline(inFile, osType);
	inFile.close();
	inFile.open("/proc/sys/kernel/hostname");
	getline(inFile, hostName);
	inFile.close();										//The files containing the system information are retrieved
	inFile.open("/proc/sys/kernel/osrelease");
	getline(inFile, osRelease);
	inFile.close();
	inFile.open("/proc/sys/kernel/version");
	getline(inFile, version);
	inFile.close(); 
	uname(&uName);
	cout << setw(10)<<left<<"Field"<<setw(35)<<"File inFile"<<setw(35)<<"API Data"<<endl;  
	cout << setw(10)<<"ostype"<<setw(35)<<osType<<setw(35)<<uName.sysname<<endl;
	cout << setw(10)<<"hostname"<<setw(35)<<hostName<<setw(35)<<uName.nodename<<endl;				//printing out in the stipulated format
	cout << setw(10)<<"osrelease"<<setw(35)<<osRelease<<setw(35)<<uName.release<<endl;
	cout << setw(10)<<"version"<<setw(35)<<version<<setw(35)<<uName.version<<endl; 
	cout << setw(10)<<"machine"<<setw(35)<<" "<<setw(35)<<uName.machine<<endl<<endl; 
	
	int SID; 
	sem_t *semlock;
	int rt; 
	SID = shmget(IPC_PRIVATE, sizeof(sem_t), (S_IRUSR|S_IWUSR));						//creatting the memory segment
	if (SID == -1)
    {
      cout << "Failed to create shared memory";						
      return 1;
    }
    semlock = (sem_t *)shmat(SID, NULL, 0); 
    if (semlock == (void *)-1)
    {
      cout << "Failed to attach memory";
      return 1;
    }
	rt = sem_init(semlock, 1, 1); 
  	if (rt == -1)
    {
      cout << "Failed to initialize semaphore";
    }
	
	int PID;	
	n = pipe(fd);								//creating the pipe
	
	if( n < 0)						
	{
		cout << "Error in pipe";
		exit(-1);	
	}
	if((PID = fork()) == 0)     				//calling the fork statement to create child and parent processes
	{
		sem_wait(semlock);
		close(fd[1]); 
		string value;
		int group=0;
		int previous = 0; 
		int total = 0;
		int subTotal = 0;
		value = childRead(fd[0]);
		while(strcmp(value.c_str(),"invalid") != 0)				//verifying if it matches with no data 
		{
			group = value[0] - '0';
			if(previous != group)								//if previous group and current group are not same printing out the subtotals
			{
				if(previous !=0)
				{
					cout << "*** "<< previous << "            "<<subTotal<< endl;
				}
				subTotal=0;
				previous = group;
				cout << endl;	
			}
			cout << setw(10) << left << value;
			subTotal++;
			total++;
			value = childRead(fd[0]);							
			if(strcmp(value.c_str(),"invalid") == 0)
			{
				cout << "Child read error\n";
				exit(-1);
			}
			else
			{
				cout << setw(50) << left << value << endl;
			}			
			value = childRead(fd[0]);
		}
		cout<<"*** "<< previous << "            "<<subTotal<<endl;					//printing out the grand total at the end
		cout<<"GRAND TOTAL  "<<total<<endl;
		close(fd[0]);
		sem_post(semlock);
		exit(0);		
	}	
	
	else if( PID > 0)						//parent process
	{
	
		close(fd[0]);	
		sem_wait(semlock);
		ifstream my_file, temp_file;
		string fileData; 
		string fileName;
		string input;
		string PIDLength;
		int calculate = 1;
		stringstream ss;		
		bool regular;
		static const boost::regex numFile("\\d+");						//regular expression
		system("ls -1 /proc | sort > hwData.txt");
		my_file.open("hwData.txt");		
		my_file >> fileData;											//writing into the file
		while(my_file)
		{
			if((regular = boost::regex_match(fileData, numFile)))
			{
				
				cout << setw(3) << right << calculate<<"  ";
				calculate++;
				fileName = "/proc/"+fileData+"/cmdline";
				ss << setw(5) <<left <<  fileData;
				fileData = ss.str();
				ss.clear();
				ss.str("");
				parentWrite(fileData,fd[1]);
				cout << left << setw(30) << fileName;				
				temp_file.open(fileName.c_str());
				
				if(temp_file.is_open())
				{			
					getline(temp_file, input);
					if(input.length() == 0)
					input = "(none)";	
					else if(input.length() > 50)							//truncating the string to 50 size
						input = input.substr(0,50);
					std::replace(input.begin(), input.end(), '\x0', ' ');
					cout << input << endl;
					parentWrite(input,fd[1]);
					temp_file.close();
				}
				else
				{
					input = "(gone)";
					cout << input << endl;
					parentWrite(input, fd[1]);
				}
			}
			my_file >> fileData;
		}		
		close(fd[1]);
		sem_post(semlock);
		wait(NULL);
		my_file.close();
		system("rm hwData.txt");
	}
	else
	{
		cout << "Error creating process child\n";
		exit(0);	
	}
	rt = shmdt((void *) semlock);
	if (rt == -1)
    {
    	cout << "Failed to detach shared memory bit";					//detaching the shared memory
      	return 1;
    }
	rt = shmctl(SID, IPC_RMID, NULL);
  	if (rt == -1)
    {
      	cout << "Failed to free the shared memory"; 
      	return 1;
    }
	exit(0);
}
/*********************************************************************************************
Function name : childRead
parameters    : int
Returns		  :  returns a string which is the appropriate message
Purpose		  :  It reads the data from the pipe sent by the parent process and displays appropriate
				messaging accordingly
*********************************************************************************************/
string childRead(int n)
{	
	char readData[50];	
	int elementLength;	
	string output;	
	if(!read(n, readData, 4))
	{
		output = "invalid";
		return output;
	}	
	elementLength = atoi(readData);	
	if(!read(n, readData, elementLength+1))	
	{
		output = "invalid";
		return output;
	}	
	output = string(readData);	
	return output;	
}
/*********************************************************************************************
Function name : parentWrite
parameters    :  string and int
Returns		  :  returns nothing. return type is void
Purpose		  :  It writes the size of the message and then the message to the pipe for the child process
				to read
*********************************************************************************************/
void parentWrite(string bit, int n)
{
	int length;	
	stringstream ss;
	string elementLength;
	char arrival_length[4];
	char writeData[50];
	length = bit.length();
	ss << length;
	elementLength = ss.str();      
	ss.clear();
	ss.str("");	
	ss << setfill('0') << setw(3) << elementLength;
	elementLength = ss.str();
	ss.clear();
	ss.str("");
	strcpy(arrival_length,elementLength.c_str());
	arrival_length[3] = '\0';
	write(n, arrival_length, 4);		
	strcpy(writeData,bit.c_str());
	writeData[length] = '\0'; 
	write(n, writeData, length+1);				
}



	
		
