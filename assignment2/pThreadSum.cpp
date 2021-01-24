#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>  
#include <pthread.h>
#include <fcntl.h>
#include <cstdlib>

using namespace std;


// Custom data type for threads that have to harbor grand children threads
struct parentThreadData
{
    //What are the range of values this threads descendents must calculate
    long long startPnt; 
    long long threadRange;
    long long amtOfChildren;
    long long *output;
};

// Custom data type for threads calculating the sum
struct threadData
{
    long long startPnt; //Where does the thread start adding from
    long long base; // Where does it stop counting to
    pthread_t parent_t; //Who created this thread
    long long *output; // Where will the final computed value go
    boolean child = false; // Is this thread a child thread
};

void* summation(void* input)
{
    struct threadData *data;
    data = (struct threadData * ) input;
    long long range = data->startPnt + data->base;
    long long finalAmt = 0;
    long long* finalTotalAmt = data->output;


    for(long long i = data->startPnt; i <= range; i++)
    {
        finalAmt += i;
    }

    *finalTotalAmt += finalAmt;

    if(data->child)
    {
    cout <<"This thread is a grand child--"<< "Child thread number "<<" start= "<<data->startPnt<<" end= "<<data->startPnt + data->base<<" sum= "<<finalAmt<<endl;
    }
    else
    {
    //  printf("Child thread number %d:\t start=%lld\t end=%lld \t sum=%lld \n",pthread_self(),data->startPnt,data->startPnt + data->base,finalAmt);
    cout << "Child thread number "<<" start= "<<data->startPnt<<" end= "<<data->startPnt + data->base<<" sum= "<<finalAmt<<endl;
    }
    return NULL;
}

void* makeGrandChildren(void* input)
{
    struct parentThreadData* grandThread;
	grandThread = (struct parentThreadData * ) input;

   // Calculating values for thread
        long long rem = grandThread->threadRange % grandThread->amtOfChildren;
        long long baseValue = ((grandThread->threadRange - rem) / grandThread->amtOfChildren);

        // Start point saver
        long long startPoint = grandThread->startPnt-1;
        for(long long i = 0; i < grandThread->amtOfChildren; i++)
        {
            // Creating struct thread data type and adding data to it for each thread
            threadData td;
            td.child=true;
            td.output = grandThread->output;
            td.base = baseValue-1;
            startPoint += 1;
            td.startPnt = startPoint;
            startPoint += baseValue-1;

            if(rem > 0)
            {
                startPoint += 1;
                td.base += 1;
                rem--;            
            }

            

            
            // Creating thread
            pthread_t thread_id;
            pthread_attr_t attr;
            pthread_attr_init(&attr);

            pthread_create(&thread_id,&attr, summation, &td);
            pthread_join(thread_id,NULL);
        }
        
        return NULL;
}

int main (int argc, char **argv)
{

    // check for valid amount of arguements
    if(argc < 2) 
    {
        printf("Incorrect amount of arguments");
        exit(-1);
    }
    // Taking values from arguments passed in CLI
    long long total = atoll(argv[1]);
    long long children = atoll(argv[2]);
    long long grandChildren = atoll(argv[3]);

    // Create variable to hold the sum of data
    long long sum = 0;
    long long* sum_ptr = &sum;

    // Creating a result file for summation
    const char *fileName = "rslt.txt";
	freopen(fileName,"w",stdout);
    printf("parent:Start of Execution\n");

// If grandchildren were passed, grand children are to compute the value for the grandparent, grandparents then add up their values
  
    if(grandChildren > 0)
    {
        long long rem = total%children;
        cout << rem << endl;
        long long baseRange = ((total-rem)/children);
        cout << baseRange << endl;

        long long startPoint = 0;
        for(long long i = 0; i < children; i++){
            // Creating struct parent thread data type
            parentThreadData td;
            td.output = sum_ptr; 
            td.amtOfChildren = grandChildren;
            td.threadRange = baseRange;
            startPoint += 1;
            td.startPnt = startPoint;
            startPoint += baseRange-1;

            if (rem > 0){
                startPoint += 1;
                td.threadRange += 1;
                rem--;
            }
           
            

            // Creating grandparent thread
            pthread_t thread_id;
            pthread_attr_t attr;
            pthread_attr_init(&attr);

            pthread_create(&thread_id,&attr, makeGrandChildren, &td);
            pthread_join(thread_id,NULL);
        }
    }

// Else no grandchildren so first set of threads passed are to compute sum
    else
    {
        // Calculating values for thread
        // rem: extra value that needs to be distributed to the rest of the children
        // baseValue: the minimum value for each string
        long long rem = total%children;
        long long baseValue = ((total-rem)/children);


        // Start point 
        long long startPoint = 0;
        for(long long i = 0; i < children; i++)
        {
            // Creating struct thread data type and adding data to it for each thread
            threadData td;
            td.output = sum_ptr;
            td.base = baseValue-1;
            startPoint += 1;
            td.startPnt = startPoint;
            startPoint += baseValue-1;

            if(rem > 0)
            {
                startPoint += 1;
                td.base += 1;
                rem--;            
            }
            
            // Creating thread
            pthread_t thread_id;
            pthread_attr_t attr;
            pthread_attr_init(&attr);

            pthread_create(&thread_id,&attr, summation, &td);
            pthread_join(thread_id,NULL);
        }

    }

    	printf("Total sum is %lld\n",sum);
		printf("Parent: End of Execution");
}