/********************************************************************
Implementation of different process scheduling algorithms
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
#include <list>
#include <cstring>
#include <cstdio>
#include <queue>

using namespace std;

int main()
{
	char type[10];							//data type to store the scheduling type
	int pageInfo[10][2];					//page size and frame size
	int pageOrder[10][20];					
	int i=0;
	ifstream infile;
	infile.open("/home/turing/t90hch1/csci480/Assign6/data6.txt");
	if(infile.fail())							//checking fail conditon to open file
	{
		cout<<"Unable to open the file"<<endl;			//error message
		exit(0);							//exiting from the file
	}
	for(i=0;i<10;i++)
	{
		infile>>type[i];					//reading algo type
		infile>>pageInfo[i][0];				//reading page size
		infile>>pageInfo[i][1];				//reading frame size
		for(int j=0;j<20;j++)		
		{
			infile>>pageOrder[i][j];		//reading the order of pages
		}
	}

	for(i=0;i<10;i++)						//looping through the input file
	{
		if(type[i]=='F')					//if fifo is selected
		{
			int frameSize=pageInfo[i][1];
			//cout<<frameSize;
			int pageSize=pageInfo[i][0];
			//cout<<"  "<<pageSize;
			int removeCounter=0;
			int tempj;
			int pageFaultCount=0;
			bool indicator=false;
			int frame[frameSize];				//variables to store the info regarding the current page sequence
			int currentCounter=0;
			int stepCounter=0;
			cout<<endl<<endl<<"Simulation of FIFO algorithm for "<<pageSize<<" and "<<frameSize<<" frames.\n";
			cout<<endl<<"step"<<"   Next Page   ";				//printing the stipulate header
			for(int x=0;x<frameSize;x++)
				cout<<"Frame "<<x<<"   ";
			cout<<"Page Fault?"<<endl;
			for(int j=0;currentCounter<frameSize;j++)
			{
				cout<<setw(4)<<stepCounter;						//loop to iterate through the page order
				cout<<setw(12)<<pageOrder[i][j];
				//cout<<endl;
				indicator=false;
				for(int k=0;k<currentCounter;k++)
				{
					if(pageOrder[i][j]==frame[k])
					{
						indicator=true;						//checking if the page already exists in the frame
						break;
					}
				}
				if(indicator==true)
				{
				}
				else
				{
					frame[currentCounter]=pageOrder[i][j];				//chaging the frame
					currentCounter++;
				}
				stepCounter++;										//incrementing the step
				for(int k=0;k<currentCounter;k++)
				{
					cout<<setw(10)<<frame[k];						//printing out the frame
				}
				if(currentCounter<=frameSize)
				{
					if(indicator==true)
					{
						int size=((frameSize-currentCounter+1)*10)-5;			//printing out the page fault alerts
						cout<<setw(size)<<"No"<<endl;
					}
					else
					{
						int size=((frameSize-currentCounter+1)*10)+7;
						cout<<setw(size)<<"Yes, in frame "<<(currentCounter-1)<<endl; 			//printing out if there is a fault
						pageFaultCount++;
					}
				}
				tempj=j+1;
			}
			removeCounter=0;
			for(;pageOrder[i][tempj]!=-1;tempj++)						//looping through the rest of the page sequence
			{
				cout<<setw(4)<<stepCounter;
				cout<<setw(12)<<pageOrder[i][tempj];					//printing out the page and step number
				stepCounter++;
			
				
				indicator=false;
				for(int k=0;k<frameSize;k++)
				{
					if(pageOrder[i][tempj]==frame[k])						//checking if page already exists
					{
						indicator=true;
						break;
					}
				}
				if(indicator==false)										//if the page is not in the frame
				{
					frame[removeCounter]=pageOrder[i][tempj];				//replacing the pages in frame
					for(int x=0;x<frameSize;x++)
					{
						cout<<setw(10)<<frame[x];							//printing out the frame
					}
					cout<<setw(17)<<"Yes, in frame "<<removeCounter<<endl;
					pageFaultCount++;									//printing the page fault messsage
					removeCounter++;
					if(removeCounter==frameSize)
					{
						removeCounter=0;
					}
				}
				else
				{
					for(int x=0;x<frameSize;x++)
					{
						cout<<setw(10)<<frame[x];					//printing out the no page fault message
					}
					cout<<setw(5)<<"No"<<endl;
				}
				
			}
			
			cout<<endl<<"In "<<stepCounter<<" steps, we have "<<pageFaultCount<<" page faults"<<endl<<endl;			//printing out total steps and page faults
		}
		else
		{
			int frameSize=pageInfo[i][1];
			//cout<<frameSize;
			int pageSize=pageInfo[i][0];
			//cout<<"  "<<pageSize;
			//int removeCounter=0;
			int tempj;
			int pageFaultCount=0;						//variables to handle the sequence
			bool indicator=false;
			int frame[frameSize];
			int frameHistory[pageSize];
			//int history;
			int currentCounter=0;
			int stepCounter=0;
			cout<<endl<<endl<<"Simulation of LRU algorithm for "<<pageSize<<" and "<<frameSize<<" frames.\n";
			cout<<endl<<"step"<<"   Next Page   ";
			for(int x=0;x<frameSize;x++)
				cout<<"Frame "<<x<<"   ";
			cout<<"Page Fault?"<<endl;
			for(int j=0;currentCounter<frameSize;j++)								//looping to fill the initial frames
			{
				cout<<setw(4)<<stepCounter;
				cout<<setw(12)<<pageOrder[i][j];
				//cout<<endl;
				indicator=false;
				for(int k=0;k<currentCounter;k++)
				{
					if(pageOrder[i][j]==frame[k])
					{
						indicator=true;									//checking if the frame already exists
						frameHistory[frame[k]]=j;
						break;
					}
				}
				if(indicator==true)
				{
				}
				else
				{
					frame[currentCounter]=pageOrder[i][j];
					frameHistory[frame[currentCounter]]=j;			//checking if the history of the frame alligns with replacement
					currentCounter++;
				}
				stepCounter++;
				for(int k=0;k<currentCounter;k++)
				{
					cout<<setw(10)<<frame[k];					//printing out the frame
				}
				if(currentCounter<=frameSize)
				{
					if(indicator==true)
					{
						int size=((frameSize-currentCounter+1)*10)-5;
						cout<<setw(size)<<"No"<<endl;
					}
					else
					{
						int size=((frameSize-currentCounter+1)*10)+7;						//printing out the page fault messages
						cout<<setw(size)<<"Yes, in frame "<<(currentCounter-1)<<endl;
						pageFaultCount++;
					}
				}
				tempj=j+1;
			}
			
			
			for(;pageOrder[i][tempj]!=-1;tempj++)
			{
				
	
				cout<<setw(4)<<stepCounter;
				cout<<setw(12)<<pageOrder[i][tempj];
				stepCounter++;									//filling out the rest of the pages in the frame
			
				
				indicator=false;
				for(int k=0;k<frameSize;k++)
				{
					if(pageOrder[i][tempj]==frame[k])
					{
						indicator=true;
						frameHistory[frame[k]]=tempj;
						break;
					}
				}
				if(indicator==false)
				{
					int selectPlace=100;
					int selectPlaceValue=100;
					for(int y=0;y<frameSize;y++)
					{
						if(selectPlaceValue>frameHistory[frame[y]])						//selecting which frame to replace
						{
							selectPlaceValue=frameHistory[frame[y]];
							selectPlace=y;
						}
					}
					frame[selectPlace]=pageOrder[i][tempj];
					frameHistory[frame[selectPlace]]=tempj;
					for(int x=0;x<frameSize;x++)
					{
						cout<<setw(10)<<frame[x];
					}
					cout<<setw(17)<<"Yes, in frame "<<selectPlace<<endl;			//page fault occured
					pageFaultCount++;
				}
				else
				{
					for(int x=0;x<frameSize;x++)
					{
						cout<<setw(10)<<frame[x];						//printing out the frame
					}
					cout<<setw(5)<<"No"<<endl;
				}
				
				
				
			}
			
			cout<<endl<<"In "<<stepCounter<<" steps, we have "<<pageFaultCount<<" page faults"<<endl<<endl;			//printing out the step count and page fault count
		}
	}
}
