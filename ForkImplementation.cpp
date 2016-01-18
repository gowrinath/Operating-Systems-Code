/********************************************************************


Purpose:   This program deals with how the fork command works.Several parent and child
			process along with grandchild processses are created
			 and the parent process call the ps command as a system command.
			

*********************************************************************/
#include<stdio.h>
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



int main()
{
	setbuf(stdout,NULL);
	int fork1;						// first fork pid
	int fork2;						// second fork pid
	cerr<<"I am the original process. My pid is "<<getpid()<<" and my parent's PID is "<<getppid()<<"\n";			//printing out the message from the initial process
	fork1 = fork();					// first fork call
	if(fork1<0)						//checking if the fork is success or not
	{
		cerr<<"Fork Failed\n";		//printing the fork fail condition
		exit(-1);					//exiting from the program
	}
	if(fork1==0)					//child process codition
	{
		cerr<<"I am the child. My PID is "<<getpid()<<" and my parent's PID is "<<getppid()<<"\n";     //printing out the child details
		fork2=fork();					//calling the fork for the second time
		if(fork2<0)						//checking if the fork is success or not
		{
			cerr<<"Fork Failed\n";		//printing the fork fail condition
			exit(-1);					//exiting from the program
		}
		if(fork2==0)					//Identifying the grandchild process
		{
			cerr<<"I am the grandchild. My PID is "<<getpid()<<" and my parent's PID is "<<getppid()<<"\n";     //printing out the child details
			cerr<<"I am the grandchild. About to exit.\n";					//printing the message Grandchild exiting the process
			exit(0);							//exiting from the grandchild
		}
		wait(0);								//waiting for the grandchild to be completed
		cerr<<"I am the child. About to exit.\n";			//printing that child it about to exit
		exit(0);							//exiting from the child
}
	cerr<<"I am the parent. My PID is "<<getpid()<<" and my parent's PID is "<<getppid()<<"\n"; 		//printing out the parent informantion
	cerr<<"I am the parent, about to call ps\n";					//printing that parent is going to call ps command
	system("ps");						//calling the ps command
	wait(0);							//waiting for the child to terminate
	cerr<<"I am the parent. About to exit.\n";			//printing that parent is about to exit
	exit(0);							//exiting from the parent
	
	return 0;							//returning zero and the program ends
	
}
