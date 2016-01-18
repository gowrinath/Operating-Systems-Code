/********************************************************************
The program handles the process handling along the entry queue, ready 
queue and the IO queue. Process that needs CPU time enters the entry queue. 
Once they are pushed to the entry queue they are never returned to the entry queue.
The process may move from entry queue to the IO queue when the process requires I/O time
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
#include <queue>

using namespace std;

const int MAX_TIME=500;
const int TIME_SLICE=5;
const int IN_USE=5;							//Declaration of constants
const int QUEUE_SIZE=20;
const int ARRAY_SIZE=12;
const int HOW_OFTEN=25;

int terminateCount=0;
int totalIdleTime=0;						//global variables
bool terminateIndicator=false;

class Process								//class process to store the information of the process
{
	public:
	string processName;
	int processID;
	int array[ARRAY_SIZE];
	int dupArray[ARRAY_SIZE];				//data members
	int sub;
	int CPUTimer;
	int IOTimer;
	int CPUTotal;
	int IOTotal;
	int CPUCount;
	int IOCount;
	int endSub;
	int entryTime;
	
	Process()								//default constructor to set the values to zeroes
	{
		sub=0;
		CPUTimer=0;
		IOTimer=0;
		CPUTotal=0;
		IOTotal=0;
		CPUCount=0;
		IOCount=0;
		endSub=0;
		entryTime=0;
	}
};

queue<Process*> entryQueue;
queue<Process*> readyQueue;				//queues to handle the process lists
queue<Process*> IOQueue;


void terminate(Process*, int);			//prototpye

int main()
{
	int counter=0;
	
	int processNumber=101;				//variables
	bool completeIndicator=true;
	Process processes[ARRAY_SIZE];
	ifstream infile;
	infile.open("/home/turing/t90hch1/csci480/Assign4/data4.txt");							
	if(infile.fail())							//checking fail conditon to open file
	{
		cout<<"Unable to open the file"<<endl;			//error message
		exit(0);							//exiting from the file
	}
	for(counter=0;counter<ARRAY_SIZE;counter++,processNumber++)			//loop to copy the contents of the file
	{
		processes[counter].processID=processNumber;
		infile>>processes[counter].processName;
		bool indicator=true;
		for(int i=0;i<12;i++)
		{
			infile>>processes[counter].array[i];
			processes[counter].dupArray[i]=processes[counter].array[i];				//looping the total information into the array
			if(processes[counter].array[i]==0 && indicator)
			{
				indicator=false;
				processes[counter].endSub=i;
			}
		}
		entryQueue.push(&processes[counter]);					//pushing into the entry queue
	}
	for(counter=0;counter<5;counter++)
	{
		Process *temp=entryQueue.front();					//pushing the first five entry queue elements into the ready queue
		readyQueue.push(temp);
		entryQueue.pop();
	}
	int time=0;
	int currentActiveTime=0;
	Process *active=NULL;
	Process *IOActive=NULL;								//initializing the active and IOactive
	bool idleIndicator;
	//active=readyQueue.front();
	//readyQueue.pop();
	for(time=0;time<MAX_TIME &&completeIndicator;time++)					//looping the time 
	{
			if((time%HOW_OFTEN==0))
		{
			cout<<"Top of the main loop at time "<<time;				//printing the contents for every 25 time instances
			int readysize=readyQueue.size();
			int iosize=IOQueue.size();
			if(active==NULL)
				cout<<endl<<"Active is Null";
			else														//printing active is null
				cout<<endl<<"Active is "<<active->processID;			//printing the active process id
			if(IOActive==NULL)
				cout<<endl<<"IOActive is Null";							//printing ioactive is null
			else
				cout<<endl<<"IOActive is "<<IOActive->processID;			//printing the io active process id
			cout<<endl<<"Contents of the Ready Queue:\n";
			Process *temporary;
			if(readysize==0)
				cout<<"(Empty)";										
			for(int j=0;j<readysize;j++)								
			{
				temporary=readyQueue.front();							//printing the ready queue contents
				readyQueue.pop();
				cout<<"Q["<<j<<"]:"<<temporary->processID<<endl;
				readyQueue.push(temporary);
			}
			cout<<endl<<"Contents of the IO Queue\n";
			if(iosize==0)
				cout<<"(Empty)";
			for(int j=0;j<iosize;j++)								//looping till the end of the queue
			{
				temporary=IOQueue.front();
				IOQueue.pop();
				cout<<"Q["<<j<<"]:"<<temporary->processID<<endl;	//printing the IO queue contents
				IOQueue.push(temporary);
			}
			
		}
		bool justChanged=false;
		if(terminateIndicator==true)			//checking if a process just ended
		{
			if(entryQueue.size()!=0)				//checking if the entry has still elements in it
			{	
				Process *temp1=entryQueue.front();
				readyQueue.push(temp1);
				entryQueue.pop();					//pushing from the entry queue to ready queue
				cout<<endl<<endl<<"Process "<<temp1->processID<<" moved from the Entry Queue into the Ready Queue and then became Active at time "<<time<<endl<<endl;
				temp1->entryTime=time;
			}
			terminateIndicator=false;
		}
		if(active==NULL)				//checking if the active is null
		{
			if(readyQueue.size()==0)		//if ready queue is empty wati till something comes in
			{
				if(idleIndicator==true)
				{
					idleIndicator=false;
					cout<<"\nAt time "<<time<<", Active is 0, so we have idle time for a while\n"<<endl;
				}		
				totalIdleTime++;				//incrementing the idle time counter
			}
			else
			{
				idleIndicator=true;
				currentActiveTime=0;					//if the ready queue has elements in it
				active=readyQueue.front();				//pushing from the ready queue
				readyQueue.pop();
				active->array[active->sub]=active->array[active->sub]-1;		//decrementing cpu burst
				currentActiveTime++;
				if(active->array[active->sub]==0)		//if the cpu burst is zero cheking if the process is done
				{
					currentActiveTime=0;
					active->sub++;
					if(active->endSub==active->sub)
					{
						terminate(active,time);			//terminating the process
						active=NULL;
					}
					else
					{
						IOQueue.push(active);			//pushing the process to IO queue if it has Io bursts
						justChanged=true;
						active=NULL;
					}
				}
			}	
		}
		else if(active!=NULL)			//if active has process already
		{
			if(currentActiveTime<TIME_SLICE)		//checking it doesnt exceed time slice
			{
				active->array[active->sub]=active->array[active->sub]-1;		//decrementing the CPU burst
				currentActiveTime++;
				if(active->array[active->sub]==0)		//checking if the cpu burst ended
				{
					currentActiveTime=0;
					active->sub++;
					if(active->endSub==active->sub)
					{
						
						terminate(active,time);		//terminating the process
						active=NULL;
					}
					else
					{
						IOQueue.push(active);			//moving to IO queue
						justChanged=true;
						active=NULL;
					}
				}
			}
			else					//if cpu bursts are exceeded
			{
				currentActiveTime=0;
				if(readyQueue.size()==0)		//if nothing is present in the ready queue continuing the loop
				{
					active->array[active->sub]=active->array[active->sub]-1;
					currentActiveTime++;
					if(active->array[active->sub]==0)		//if cpu burst ends
					{
						currentActiveTime=0;
						active->sub++;
						if(active->endSub==active->sub)		
						{
							
							terminate(active,time);		//terminating
							active=NULL;
						}
						else
						{
							IOQueue.push(active);		//pushing to the IO queue
							justChanged=true;
							active=NULL;
						}
					}
					
				}
				else
				{
					readyQueue.push(active);			//pushhing the active to ready queue
					active=readyQueue.front();
					readyQueue.pop();					//retrieving the front of the ready queue
					active->array[active->sub]=active->array[active->sub]-1;
					currentActiveTime++;
					if(active->array[active->sub]==0)			//checking if cpu bursts ends
					{
						currentActiveTime=0;
						active->sub++;
						if(active->endSub==active->sub)
						{
							
							terminate(active,time);			//termination
							active=NULL;
						}
						else
						{
							IOQueue.push(active);			//pushing to the queue
							justChanged=true;
							active=NULL;
							if(justChanged)
							{
							}
						}
					}
					
				}
			}
		}
		if(IOActive==NULL)			//if IO active is null
		{
			if(IOQueue.size()==0)
			{
											//do nothing
			}
			else
			{
				IOActive=IOQueue.front();				//retrieving the front of the Io queue
				IOQueue.pop();
				IOActive->array[IOActive->sub]=IOActive->array[IOActive->sub]+1;	//implementing the Io burst
				if(IOActive->array[IOActive->sub]==0)			//if Io burst is done
				{
					IOActive->sub++;
					readyQueue.push(IOActive);			//pushing the IO active to ready queue
					IOActive=NULL;
					if(IOQueue.size()>0)
					{
						IOActive=IOQueue.front();				//if Io queue has element in it attaching to IO aactiv
						IOQueue.pop();
					}
				}
			}
			
		}
		else			//if Io active is not null
		{
			IOActive->array[IOActive->sub]=IOActive->array[IOActive->sub]+1;		//implementing the IO burst
			if(IOActive->array[IOActive->sub]==0)
			{
				IOActive->sub++;
				readyQueue.push(IOActive);
				IOActive=NULL;
				if(IOQueue.size()>0)					//handking the process in Io queue
					{
						IOActive=IOQueue.front();
						IOQueue.pop();
					}
			}
		}
		if(entryQueue.size()==0 && readyQueue.size()==0 && IOQueue.size()==0 &&active==NULL &&IOActive==NULL)
			completeIndicator=false;				//checking if all the tasks are done
		//if(terminateCount==1)		
		//break;
		
	
	}
	cout<<"The run has ended."<<endl;
	cout<<"The final value of the time was: "<<time<<endl;
	cout<<"The amount of time spent idle was: "<<totalIdleTime<<endl;
	cout<<"Number of terminated processes = "<<terminateCount<<endl;			//printing out the stacks after processes are done
	cout<<"Contents of the Entry Queue:"<<endl;
	if(entryQueue.size()==0)
		cout<<"(Empty)"<<endl;
	cout<<"Contents of the Ready Queue:"<<endl;
	if(readyQueue.size()==0)
		cout<<"(Empty)"<<endl;
	cout<<"Contents of the IO Queue:"<<endl;
	if(IOQueue.size()==0)
		cout<<"(Empty)"<<endl;
	return 0;					//end main
	
	
}

/***************************************************************
Function:  void terminate(Process*, int)

Use: this function prints out the value of the process that is terminated

Arguments: bprocess id and time

Returns: It returns nothing
***************************************************************/
void terminate(Process* printer,int time)
{
	int total=0;
	int waitTime=0;				//variables
	terminateCount++;
	terminateIndicator=true;
	cout<<"Process "<<printer->processID<<" has ended at "<<time<<"\n";			//printing out the process Id and name
	cout<<"Name:  "<<printer->processName<<endl;
	for(int i=0;i<printer->sub;i++)
	{
		if(i%2==0)
		{
			printer->CPUTotal+=printer->dupArray[i];				//counting the cpu bursts 
			if(printer->dupArray[i]>5)
				printer->CPUCount=printer->CPUCount+2;
			else
				printer->CPUCount++;					//counting number of times
		}
		else
		{
			printer->IOTotal-=printer->dupArray[i];				//counting the Io bursts
			printer->IOCount++;
		}
	}		
	total=printer->CPUTotal+printer->IOTotal;			//calculating the total time
	waitTime=time-total-printer->entryTime;				//calculating the wait time
	if(waitTime<0)
		waitTime=0;
	cout<<"Started at time "<<printer->entryTime<<" and ended at time "<<time<<endl;
	cout<<"Total CPU time = "<<printer->CPUTotal<<" in "<<printer->CPUCount<<" CPU bursts"<<endl;		//printing out the bursts details and wait times
	cout<<"Total IO time = "<<printer->IOTotal<<" in "<<printer->IOCount<<" IO bursts"<<endl;
	cout<<"Time spent in waiting: "<<waitTime<<endl<<endl;
}
