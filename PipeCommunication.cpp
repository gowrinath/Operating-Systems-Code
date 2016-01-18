/********************************************************************
Thre program succesfully implements the pipe coummnication between the
process which are achieved through forking of the parent process. 
Information is writted from the parent process to the child process.
*********************************************************************/
#include<stdio.h>
#include<stdlib.h>
#include<vector>
#include<iostream>
#include<iomanip>
#include<fstream>
#include<string>
#include<cstdlib>					//header files
#include<sstream>
#include<math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstring>
#include<sys/utsname.h>
#include <cstdio>

using namespace std;

void PWork(int PA[],int PB[]);				
void CWork(int PA[],int PB[]);			//function prototypes

int main()
{
	setbuf(stdout,NULL);
	int PA[2];				//pipe 1
	int PB[2];				//pipe 2
	int pid1;				//fork id to identify parent and child process
	if(pipe(PA)==-1)		//checking pipe fail condition
	{
		cerr<<"Pipe Error\n";			//printing that pipe failed
		exit(-5);						//exiting from the program
	}
	
	if(pipe(PB))			//chekcing piple fail condition
	{
		cerr<<"Pipe Error\n";			//printing that pipe failed
		exit(-5);						//exiting from the program
	}
	pid1=fork();				//callung fork command
	if(pid1==-1)				//checking fork fail condition
	{
		cerr<<"Pipe Error\n";			//printing that fork failed
		exit(-5);						//exiting from the program
	}
	if(pid1==0)					//child process
	{
		close(PA[1]);			//closing the read end
		close(PB[0]);			//closing the write end
		CWork(PA,PB);			//calling the funciton
		close(PA[0]);			//closing the write end
		close(PB[1]);			//closing the read end
		exit(0);				//exiting from the child process
	}
	close(PA[0]);				//closing the write end
	close(PB[1]);				//closing the read end
	PWork(PA,PB);				//calling the function
	close(PA[1]);				//closing the read end
	close(PB[0]);				//closing the write end
	wait(0);					//waiting for the child process to end
	exit(0);					//exiting from the parent process
	return 0;					//returing 0
}

/***************************************************************
Function:  void PWork(int PA[],int PB[])

Use: this function prints out the value that is passed to the parent process

Arguments: both the pipe attributes are passed

Returns: It returns nothing
***************************************************************/
void PWork(int PA[],int PB[])
{
	setbuf(stdout,NULL);
	char *Buffer = (char*)malloc(sizeof(char) * 10);  		//Buffer with size 10
	char *Value = (char*)malloc(sizeof(char) * 10);         //Buffer named Value with size 10
	Value[0] = '1';									//initializing the value
	Value[1] = '\0';						//terminating the string properly
	int j = 0;								//counter to read values from buffer
	int n;									//counter to identify the number of bytes written
	cerr<<"Parent value:"<<setw(10)<<Value<<"\n";				//printing out the value
	Value[0] = '4';
	Value[1] = '\0';
	int length=write(PA[1],Value,2);			//writing to the child

	char temp[10];					//temporary array to read from child
	while(length<=8)				//checking the size is less than or equal to 8
	{
		read(PB[0],temp,1);					//reading byte by byte
		for(j=0;temp[0]!='\0'&&j<=8;j++)		//repeating till null character encoutered
		{
			Value[j]=temp[0];
			read(PB[0],temp,1);						//writing to the vaule
			
		}
		Value[j] = '\0';				//temrinating the value
		int M = atoi(Value);			//converting to integer value
		M = 3*M + 1;
		n=sprintf(Buffer,"%d",M);			//writing into the buffer
		length=n;
		Buffer[n]='\0';					//terminating the buffer
		write(PA[1], Buffer,n+1);		//writing to the pipe
		usleep(2000);
		cerr<<"Parent Value:"<<setw(10)<<Value<<endl;			//priting out the parent value
	
	}
}

/***************************************************************
Function:  void CWork(int PA[],int PB[])

Use: this function prints out the value that is passed to the child process

Arguments: both the pipe attributes are passed

Returns: It returns nothing
***************************************************************/
void CWork(int PA[],int PB[])
{
	setbuf(stdout,NULL);
	char *Buffer = (char*)malloc(sizeof(char) * 10);  		//Buffer with size 10
	char *Value = (char*)malloc(sizeof(char) * 10);        //Buffer named Value with size 10
	int M;
	int j=0;								//counter to read values from buffer
	int n;									//counter to identify the number of bytes written
	char temp1[10];
	int length=2;
	
	read(PA[0],temp1,1);					//reading byte by byte
	while(length<=8)						//making sure that it does not go out of 8
	{
	
		for(j=0;temp1[0]!='\0'&&j<=8;j++)			//looping till null character encoutered
		{
			Value[j]=temp1[0];				//assigning it to the value
			read(PA[0],temp1,1);			//writing to the pipe
		}
		Value[j] = '\0';			//terminating the value
		M = atoi(Value);			//converting to integer value
		M = 3*M + 1;
		n=sprintf(Buffer,"%d",M);			//writing into the buffer
		length=n;
		Buffer[n]='\0';
		write(PB[1], Buffer,n+1);		//writing to the pipe
		read(PA[0],temp1,1);			//reading to the piple
		usleep(1000);
		cerr<<"Child Value: "<<setw(10)<<Value<<endl;		//priting out the child value
	}
}

