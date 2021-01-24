#include<iostream>
#include<fstream>
using namespace std;

class node
{
    private:
        string processName = "Free";
        int startAddress = 0;
        int endAddress = 0;
        int size = 0;
        node* nextNode = NULL;
        node* prevNode = NULL;
        bool allocated = false;
    
    public:
    // Constructors
        node(string n, int s)
        {
            this->processName = n;
            this->size = s;
        }

        // Setters
        void setName(string n)
        {
            this->processName = n;
        }

        void setStart (int n)
        {
            this->startAddress = n;
        }

        void setEnd (int n)
        {
            this->endAddress = n;
        }
        void setSize(int n)
        {
            this->size = n;
        }

        void setAllocate(bool a)
        {
         this->allocated = a;   
        }

        void setNext(node* n)
        {
            this->nextNode = n;
        }

        void setPrev(node* n)
        {
            this->prevNode = n;
        }

        // Getters
        string getName()
        {
            return this->processName;
        }

        int getStart()
        {
            return this->startAddress;
        }

        int getEnd()
        {
            return this->endAddress;
        }

        int getSize()
        {
            return this->size;
        }

        node* getNext()
        {
            return this->nextNode;
        }

        node* getPrev()
        {
            return this->prevNode;
        }

        bool isAllocated()
        {
            return this->allocated;
        }

        // Methods
        // Using addresses compute the size of the process
        void computeSize()
        {
            this->size = (this->endAddress + 1) - this->startAddress;
        }

        // Base on parameter X shift the nodes address
        void shift(int x)
        {
            this->startAddress = this->startAddress + x;
            this->endAddress = this->endAddress + x;
        }





};
class LList
{
    private:
    node* listHead = NULL;
    int largestNode = 0;
    

    public:
    LList(int size)
    {
        node* dummy = new node("Free",size);
        dummy->setStart(0);
        dummy->setEnd(size);
        dummy->setAllocate(false);
        this->listHead = dummy; 
        this->largestNode = size;
    }

    // Getters
    node* getHead()
    {
        return this->listHead;
    }

    // Methods
    // Find next free spot based base worst fit allocation
    node* findSpot(node* newNode,bool& output)
    {
        node * spot;
        node * runner = this->listHead;
        int largestSpace = 0;
        int space = newNode->getSize();
		while((runner != NULL))
        {

        if((runner->isAllocated() == false) && (runner->getSize() >= space) && runner->getSize() > largestSpace)
        {
            spot = runner;
            largestSpace = runner->getSize();
            output = true;
        }
        runner = runner->getNext();	
		}
	return spot;
    }

    // Insert new freespace into head node, used for compaction
    void insertNewHead()
    {
        node* n = new node("Free",0);
        n->setNext(this->listHead);
        n->getNext()->setPrev(n);
        n->setPrev(NULL);
        n->setAllocate(false);
        this->listHead = n;
    }
    
    // Part 2 of RQ function, to insert new process into memory
    void partition(node* spot, node* n)
    {
        if(spot->getSize() == n->getSize())
        {
            spot->setName(n->getName());
            spot->setAllocate(true);
        }
        else
        {    
        n->setEnd(spot->getEnd());
        n->setStart(n->getEnd()-n->getSize());
        n->setPrev(spot);
        spot->setEnd(n->getStart() - 1);
        spot->computeSize();
        n->setNext(spot->getNext());
        spot->setNext(n);
        if(n->getNext() != NULL)
        {
            n->getNext()->setPrev(n);
        }
        n->setAllocate(true);
        }
    }

    // RQ function implementation
    void allocate(string name, int size)
    {
        node* n = new node(name, size);
        node* spot;
        bool foundSpot = false; 
        spot = findSpot(n,foundSpot);

        if(foundSpot)
        {
            partition(spot,n);
        }
    }

    // RL function implementation
    void release(string n)
    {
        //Condtion 1: Target process is at the start of memory
        if(this->listHead->getName() == n)
        {
            this->listHead->setName("Free");
            this->listHead->setAllocate(false);
            node* spot = this->listHead;
            node* runner = spot->getNext();
            while(runner != NULL && runner->isAllocated() == false)
            {
                spot->setEnd(runner->getEnd());
            }
            spot->computeSize();
        }
        else 
        {
            // Condition 2: target is after the start of memory
            node* runner = this->listHead;
            while(runner != NULL && runner->getName() != n)
            {
                runner = runner->getNext();
            }

            if(runner == NULL)
            {
                cout<<"Process does not exist in memory\n";
            }
            else
            {
                // Condense continous free memory behind the target
                runner->setName("Free");
                runner->setAllocate(false);
                if(runner->getPrev() != NULL && runner->getPrev()->isAllocated() == false)
                {
                    node* spot = runner;
                    spot->setName("Free");
                    while(runner->getPrev()!=NULL && runner->getPrev()->isAllocated() == false)
                    {
                        runner = runner->getPrev();
                       
                    }
                    spot->setStart(runner->getStart());
                    spot->setPrev(NULL); 
                    if(runner->getPrev()!= NULL)
                    {
                        runner->getPrev()->setNext(spot);
                        spot->getPrev()->setNext(spot);
                    }
                    if(this->listHead == runner)
                    {
                        this->listHead = spot;
                        spot->computeSize();
                        spot->setSize(spot->getSize()-1);
                    } 
                    else
                    {
                        spot->computeSize();
                    }
                    
                }

                // Condense continous memory after the target
                if(runner->getNext() != NULL && runner->getNext()->isAllocated() == false)
                {
                    node* spot = runner;
                    while(runner->getNext() != NULL && runner->getNext()->isAllocated() == false)
                    {
                        runner = runner->getNext();
                    }

                    spot->setEnd(runner->getEnd());
                    spot->setNext(runner->getNext()); 
                    if(runner->getNext()!= NULL)
                    {
                        runner->getNext()->setPrev(spot);
                    }
                }

            }
        } 
    }

    // C function implementation
    void compact()
    {
        int freeMem = 0;
        node* runner;
        node* shifter;
        // First: Check list head condition, is it free or allocated

        // Condition 1: List head is free
        if(this->listHead->isAllocated()== false)
        {
            node* spot = this->listHead;
            runner = spot->getNext(); 
            while(runner != NULL)
            {
                if(runner->isAllocated() == false)
                {
                    // Add space to front node
                    freeMem = freeMem + runner->getSize();
                    this->listHead->setEnd(this->listHead->getEnd() + runner->getSize());

                    // If runner is at end point
                    if(runner->getNext() == NULL)
                    {
                        runner->getPrev()->setNext(NULL);
                        shifter = this->listHead->getNext();
                        while(shifter != NULL)
                        {
                            shifter->shift(freeMem);
                            shifter = shifter->getNext();
                        }
                    }
                    else
                    {
                        shifter = this->listHead->getNext();
                        while(shifter != runner)
                        {
                            shifter->shift(freeMem);
                            shifter = shifter->getNext();
                        }
                        runner->getPrev()->setNext(runner->getNext());
                        runner->getNext()->setPrev(runner->getPrev());
                    }        
                }
                runner = runner->getNext();
            }     
        }
        else
        {
            // Condition 2: List head is filled meaning that memory might be filled and compaction is not possible
            node* runner = this->listHead;
            while(runner != NULL && runner->isAllocated() == false)
            {
                runner = runner->getNext();
            }

            // If the runner found some free memory
            if(runner != NULL)
            {
                insertNewHead();
                while(runner != NULL)
                {
                    freeMem = runner->getSize();
                     if(runner->getNext() == NULL)
                    {
                        runner->getPrev()->setNext(NULL);
                        this->listHead->setEnd(this->listHead->getEnd() + freeMem);
                    }
                    else
                    {
                        this->listHead->setEnd(this->listHead->getEnd() + freeMem);
                        shifter = this->listHead->getNext();
                        while(shifter != runner)
                        {
                            shifter->shift((runner->getEnd() - runner->getNext()->getEnd()) - 1);
                            shifter = shifter->getNext();
                        }
                        runner->getPrev()->setNext(runner->getNext());
                        runner->getNext()->setPrev(runner->getPrev());
                    }
                    runner = runner->getNext();
                }
            }
        
        }
        
    }

    // STAT function implementation
    void stat(ofstream& outfile)
    {
        node* runner = this->listHead;
        outfile << "allocator>STAT\n";
        while(runner != NULL)
        {
            if(runner->isAllocated() == false)
            {
            outfile << "Addresses [" << runner->getStart() << " : " << runner->getEnd() << "] " << runner->getName() << endl;
            runner = runner->getNext();
            }
            else
            {
            outfile << "Addresses [" << runner->getStart() << " : " << runner->getEnd() << "] " << "Process " << runner->getName() << endl;
            runner = runner->getNext();
            }
            
        }
    }
};


// Main methods

    // Gets next token in the command passed
    string getNextToken(string& str)
    {
        string token;
        string delim = " ";

        // Getting "token" of inputted string and consuming it from the input
        token = str.substr(0,str.find(delim));
        str.erase(0, str.find(delim) + delim.length()); 

        return token;
    }



int main(int argc, char** argv)
{
    int memSize = atoi(argv[1]);

    // Setting up variables to hold data for nodes
    string input;
    string name;
    string flag;
    int inSize;
    string token;
    ofstream file;
    bool active = true;

    file.open("rslt.txt");
    // Setting up memory
    LList* memory = new LList(memSize);
    


    // Program loop
    cout << "Enter command(s) " << endl;

    while(active)
    {
  
        getline(cin,input);
        

       string cmd = getNextToken(input);

        // QUIT command
       if(cmd == "QUIT")
       {
           file.close();
           active = false;
       }
       // RQ command
       else if(cmd == "RQ")
       {
           name = getNextToken(input);
           inSize = stoi(getNextToken(input));
           flag = getNextToken(input);
           
           memory->allocate(name,inSize);
       }
       // RL command
        else if(cmd == "RL")
       {
            name = getNextToken(input);
            memory->release(name);
       }
       // STAT command
       else if(cmd == "STAT")
       {
           memory->stat(file);
       }
       // C command
       else if(cmd == "C")
       {
            memory->compact();
       }
       else
       {
           cout<<"Improper format\n";
       }     
    }
    // End of program execution/QUIT command has been passed in
    cout<<"Ending current instance\n";
    file.close();
}