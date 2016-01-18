/***************************************************************************
The program handles dynamic memorry allocation that is required by the process
and this alotment is done either according to the first fit or best fit depending
upon the requirements of the user selection.
***************************************************************************/
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
//#include <sys/types.h>
//#include <sys/wait.h>
#include <cstring>
//#include<sys/utsname.h>
#include <cstdio>
#include <queue>

using namespace std;

#define HOWOFTEN 5
#define MB 1024

class Block
{
	public:
	int startAddress;
	int size;
	string processID;	
	string blockID;
	
};

class readFile
{
	public:
	char processType;
	int processID;
	int size;
	string programName;
};

list<Block> InUse;
list<Block> Avail;
readFile fileData[17];

void loadTransaction(int);
void allocateTransaction(int);
void deallocateTransaction(int);
void terminateTransaction(int);

int main()
{
	Block oneMB;
	Block twoMB1, twoMB2;
	Block fourMB1, fourMB2;
	oneMB.startAddress=3*MB*MB;
	twoMB1.startAddress=4*MB*MB;
	twoMB1.startAddress=6*MB*MB;
	fourMB1.startAddress=8*MB*MB;
	fourMB2.startAddress=12*MB*MB;
	oneMB.size=MB;
	twoMB1.size=2*MB;
	twoMB2.size=2*MB;
	fourMB1.size=4*MB;
	fourMB2.size=4*MB;
	Avail.push_back(oneMB);
	Avail.push_back(twoMB1);
	Avail.push_back(twoMB2);
	Avail.push_back(fourMB1);
	Avail.push_back(fourMB2);
	ifstream infile;
	infile.open("data.txt");
	if(infile.fail())							//checking fail conditon to open file
	{
		cout<<"Unable to open the file"<<endl;			//error message
		exit(0);							//exiting from the file
	}
	int i=-1;
	while(true)
	{
		i++;
		if(i==17)
			break;
		infile>>fileData[i].processType;
		infile>>fileData[i].processID;
		if(fileData[i].processType!='T')
		{
			if(fileData[i].processType!='D')
				infile>>fileData[i].size;
			infile>>fileData[i].programName;
		}
	}
	
	
	for(int i=0;i<17;i++)
	{
		if(fileData[i].processType=='L')
			loadTransaction(i);
	
	}
	
	return 0;
}

void loadTransaction(int counter)
{
	counter=false;
	list<Block>::iterator it;
//	int i=0;
	for(it=Avail.begin();it!=Avail.end();)
	{
		if(fileData[counter].size<=it->size)
		{
			counter=true;
			Block newBlock;
			newBlock.blockID=it->blockID;
			newBlock.startAddress=it->startAddress;
			newBlock.size=fileData[counter].size;
			newBlock.processID=fileData[counter].processID;
			it->startAddress=(it->startAddress)+fileData[counter].size;
			it->size=(it->size)-fileData[counter].size;
			InUse.push_back(newBlock);
			cout<<"Successful Insertion"<<endl;
			if(it->size==0)
			{
				it=Avail.erase(it);
			}
			break;
		}
		else
		 	++it;
	}
	if(counter==false)
	{
		cout<<"No sufficient block found"<<endl;
		return;
	}
}

void allocateTransaction(int counter)
{
		counter=false;
	list<Block>::iterator it;
	//int i=0;
	for(it=Avail.begin();it!=Avail.end();)
	{
		if(fileData[counter].size<=it->size)
		{
			counter=true;
			Block newBlock;
			newBlock.blockID=it->blockID;
			newBlock.startAddress=it->startAddress;
			newBlock.size=fileData[counter].size;
			newBlock.processID=fileData[counter].processID;
			it->startAddress=(it->startAddress)+fileData[counter].size;
			it->size=(it->size)-fileData[counter].size;
			InUse.push_back(newBlock);
			cout<<"Successful Insertion"<<endl;
			if(it->size==0)
			{
				it=Avail.erase(it);
			}
			break;
		}
		else
		 	++it;
	}
	if(counter==false)
	{
		cout<<"No sufficient block found"<<endl;
		return;
	}
}

void deallocateTransaction(int counter)
{
	list<Block>::iterator it;
}
