#include <iostream>
#include <vector>
#include <map>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

#define DISK_SIZE 256
// Programmer's name: Evyatar Golan



void decToBinary(int n , char &c) 
{ 
   // array to store binary number 
    int binaryNum[8]; 
  
    // counter for binary array 
    int i = 0; 
    while (n > 0) { 
          // storing remainder in binary array 
        binaryNum[i] = n % 2; 
        n = n / 2; 
        i++; 
    } 
  
    // printing binary array in reverse order 
    for (int j = i - 1; j >= 0; j--) {
        if (binaryNum[j]==1)
            c = c | 1u << j;
    }
 }

void decToBinary2(int n , unsigned char &c) 
{ 
   // array to store binary number 
    int binaryNum[8]; 
  
    // counter for binary array 
    int i = 0; 
    while (n > 0) { 
          // storing remainder in binary array 
        binaryNum[i] = n % 2; 
        n = n / 2; 
        i++; 
    } 
  
    // printing binary array in reverse order 
    for (int j = i - 1; j >= 0; j--) {
        if (binaryNum[j]==1)
            c = c | 1u << j;
    }
 } 

// #define SYS_CALL


//-------------CLASSES START---------------


//-------------CLASS fsInode START---------------

class fsInode {
    int fileSize;
    int block_in_use;

    int *directBlocks;
    int singleInDirect;
    int num_of_direct_blocks;
    int block_size;

    public:
    fsInode(int _block_size, int _num_of_direct_blocks) {
        fileSize = 0; 
        block_in_use = 0; 
        block_size = _block_size;
        num_of_direct_blocks = _num_of_direct_blocks;
        directBlocks = new int[num_of_direct_blocks];
		assert(directBlocks);
        for (int i = 0 ; i < num_of_direct_blocks; i++) {   
            directBlocks[i] = -1;
        }
        singleInDirect = -1;

    }
    ~fsInode() { 
        delete [] directBlocks;
        block_size=0;

        //free indirect block if exists!!  WIP

    }

    //my functions
    int returnUsed() //retuns the amount of used data blocks
    {return block_in_use;}
    void changeUsed(int d) //sets the change to the amount of used blocks
    {block_in_use+=d;}

    int* returnDirect()
    {return directBlocks;}
    int returnIndirect()
    {return singleInDirect;}
    void changeIndirect(int i)
    {singleInDirect=i;}
};
//-------------CLASS fsInode END-----------------


//-------------CLASS FileDescriptor START---------------

class FileDescriptor {
    pair<string, fsInode*> file;
    bool inUse;
    bool dead;
    
    public:
    FileDescriptor(string FileName, fsInode* fsi) {
        file.first = FileName;
        file.second = fsi;
        inUse = true;
        dead=false;

    }
    ~FileDescriptor()
    {
    }
    bool isEmpty()
    {return file.second == nullptr;}
    void killString()
    {
        if(&file.first !=nullptr)
            free(&file.first);
    }
    void killInode()
    {
        if(&file.second !=nullptr)
            delete file.second;
    }

    //setters and getters
    string getFileName(){return file.first;}
    fsInode* getInode() {return file.second;}
    bool isInUse() {return (inUse);}
    void setInUse(bool _inUse) {inUse = _inUse;}
    void setDead() {dead=true;inUse=false;}
    bool getDead() {return dead;}
};
//-------------CLASS FileDescriptor END-----------------

#define DISK_SIM_FILE "DISK_SIM_FILE.txt"

//-------------CLASS fsDisk START---------------

class fsDisk {
    FILE *sim_disk_fd;
 
    bool is_formated;

	// BitVector - "bit" (int) vector, indicate which block in the disk is free
	//              or not.  (i.e. if BitVector[0] == 1 , means that the 
	//             first block is occupied. 
    int BitVectorSize;
    int *BitVector;
    int *UsedBitVector;
    int *indBitVector;

    // Unix directories are lists of association structures, 
    // each of which contains one filename and one inode number.
    map<string, fsInode*>  MainDir; 

    // OpenFileDescriptors --  when you open a file, 
	// the operating system creates an entry to represent that file
    // This entry number is the file descriptor. 
    vector< FileDescriptor > OpenFileDescriptors;

    int direct_enteris;
    int block_size;

    public:
    //-----CONSTRUCTOR & DESTRUCTOR START ------
    fsDisk() 
    {
        sim_disk_fd = fopen( DISK_SIM_FILE , "wr+" );
        assert(sim_disk_fd);
        for (int i=0; i < DISK_SIZE ; i++) {
            int ret_val = fseek ( sim_disk_fd , i , SEEK_SET );
            ret_val = fwrite( "\0" ,  1 , 1, sim_disk_fd );
            assert(ret_val == 1);
        }
        fflush(sim_disk_fd);
        is_formated=false;
    }
    ~fsDisk()
    {
        fclose(sim_disk_fd);
        remove(DISK_SIM_FILE); //delete the disk file
        // if(BitVector != NULL)
        if(is_formated)
        {
            delete [] BitVector;
            delete [] UsedBitVector;
            delete [] indBitVector;
            while(!OpenFileDescriptors.empty())
            {
                int s = OpenFileDescriptors.size()-1;
                if(!OpenFileDescriptors[s].getDead())
                    delete OpenFileDescriptors[s].getInode();
                OpenFileDescriptors.pop_back();
            }
        
        }
        
    }	
    //-----CONSTRUCTOR & DESTRUCTOR   END ------

    //-----ASSIGNMENT FUNCTIONS START ------

    //lists all the files currently in our MainDir
    void listAll() {
        int i = 0;    
        for ( auto it = begin (OpenFileDescriptors); it != end (OpenFileDescriptors); ++it) {
            cout << "index: " << i << ": FileName: " << it->getFileName() <<  " , isInUse: " << it->isInUse() << endl; 
            i++;
        }
        char bufy;
        cout << "Disk content: '" ;
        for (i=0; i < DISK_SIZE ; i++) {
            // if(indBitVector[i/block_size]==1) //if block is indirect block
            //     continue;                     //don't bother printing its contents
            int ret_val = fseek ( sim_disk_fd , i , SEEK_SET );
            ret_val = fread(  &bufy , 1 , 1, sim_disk_fd );
            cout << bufy;          
        }
        cout << "'" << endl;


    }
 
    //formats the fsdisk
    void fsFormat( int blockSize = 4, int direct_Enteris_ = 3 ) 
    {
        if(blockSize>DISK_SIZE)
        {
            cerr << "ERROR: cannot initialize large blocks (>" << DISK_SIZE <<" bytes).";
            return;
        }
        else if(blockSize<=0)
        {
            cerr << "ERROR: cannot initialize blocks with no indirect blocks." << endl;
            return;
        }
        else if(direct_Enteris_<0)
        {
            cerr << "ERROR: quantity are a scalar property, thus cannot be negative." << endl;
            return;
        }

        this->direct_enteris=direct_Enteris_;
        this->block_size=blockSize;
        this->is_formated=true;
        this->BitVectorSize = DISK_SIZE / block_size;
        this->BitVector = new int[BitVectorSize];
        this->UsedBitVector = new int[BitVectorSize];
        this->indBitVector = new int[BitVectorSize];
        for(int i=0;i<BitVectorSize;i++)
        {
            BitVector[i]=0;
            UsedBitVector[i]=0;
            indBitVector[i]=0;
        }

        int numOfBlocks = (DISK_SIZE)/(block_size);
        cout << "FORMAT DISK: number of blocks: " << numOfBlocks << endl;
        if(numOfBlocks == 1)
            cerr << "WARNING: disk size suboptimal, consider smaller (<" << (DISK_SIZE)/2 << " bytes) block size." << endl;
    }

    //creates the file and places its pointer in mainDir and OpenFileDescriptors
    int CreateFile(string fileName) 
    {
        //prevent double entries and return 
        if(MainDir.count(fileName)>0)
        {
            cerr << "ERROR: filename already exists, please select another." << endl;
            return -1;
        }
        
        fsInode* newOne = new fsInode (this->block_size,this->direct_enteris); //new fsInode!
        MainDir.emplace(fileName,newOne);
        FileDescriptor new2 = FileDescriptor(fileName,newOne);
        OpenFileDescriptors.push_back(new2); //put in OFD
        return this->OpenFileDescriptors.size() - 1;


    }

    //open the file -- i.e. turn on its inUse boolean
    int OpenFile(string fileName) 
    {
        int i = 0; bool found=false;
        for ( auto it = begin (OpenFileDescriptors); it != end (OpenFileDescriptors); ++it) {
            if(it->getFileName() == fileName) 
            {found=true;break;}
            i++;
        }
        if(!(found))
        {
            cerr << "ERROR: no such filename exists" << endl;
            return -1;
        } else if(OpenFileDescriptors[i].getDead())
            return -1;
        OpenFileDescriptors[i].setInUse(true);
        return i;
    }  

    //close the file -- i.e. turn off its inUse boolean
    string CloseFile(int fd) 
    {
        if(fd>OpenFileDescriptors.size() || OpenFileDescriptors[fd].isEmpty())
        {
            cerr << "ERROR: no such fd exists" << endl;
            return "-1";
        } else if(OpenFileDescriptors[fd].getDead())
            return "";
        OpenFileDescriptors[fd].setInUse(false);
        return OpenFileDescriptors[fd].getFileName();
    }
    
    //write information to file
    int WriteToFile(int fd, char *buf, int len ) 
    {
        if(fd>this->OpenFileDescriptors.size() || this->OpenFileDescriptors[fd].isEmpty())
        {
            cerr << "ERROR: there is no fd in spot " << fd <<"." << endl;
            return -1;
        }
        else if(!OpenFileDescriptors[fd].isInUse())
        {
            cerr << "ERROR: file is not open, please open file first!" << endl;
            return -1;
        }
       
       fsInode *toUse = OpenFileDescriptors[fd].getInode();
       //all the error checks
       {
       int greatestSize = ((direct_enteris + block_size) * block_size);
       if(greatestSize < len)
       {   //check if string length is too long
            cerr << "ERROR: string length greater than allowed (>="<<greatestSize <<")." << endl;
           return -1;
       }
        else if(fd>this->OpenFileDescriptors.size() || this->OpenFileDescriptors[fd].isEmpty())
        {   //check if fd exists
            cerr << "ERROR: There is no fd in spot " << fd <<"." << endl;
            return -1;
        }
        else if(!OpenFileDescriptors[fd].isInUse())
        {   //check if the file is open
            cerr << "ERROR: file is not open, please open file first!" << endl;
            return -1;
        } else if(OpenFileDescriptors[fd].getDead())
        {
            cerr << "ERROR: file is deleted!";
            return -1;
        }
        int available = greatestSize - toUse->returnUsed();
        if(available < len)
        {
            cerr << "ERROR: not enough room available." << endl;
            return -1;
        }
        }
        
        //if we passed all the error checks, start assiging letters to blocks!
        int indA = 0; //indA can be used to indicate how many letters we've added
        int* arr = toUse->returnDirect();
        
        //add to direct entries if possible
        for(int i=0 ; i<direct_enteris && indA < len && toUse->returnUsed() < direct_enteris*block_size ; i++)
        {
            
            int j=arr[i];
            if(j==-1)       //if we still didn't assign a direct block
            {
                j=returnBitV();
                if(j==-1)   //if returnBitV didn't find a block to allocate its space to the file
                {           //then simply end this function :(
                    cerr << "There are no blocks available for allocation, \nplease delete some blocks first." <<endl;
                    return -1;
                }
                arr[i]=j;   //otherwise notify the correct places that you took this block
                BitVector[j]=1;
                UsedBitVector[j]=1;
            }
            char* arr2=returnBlock(j);
            for(int k=0;k<block_size && indA < len;k++)
            {
                if(arr2[k]=='\0')
                {
                    this->rewriteCell(j*block_size+k,buf[indA]);
                    indA++;
                }
            }
            delete [] arr2;
        }
        // delete [] arr;
        
        //add to indirect entries if needed AND possible
            if(indA < len) // i.e. - if we have more letters to commit to the fsInode's indirect!
        {
            //if an indirect block still haven't been given to the file
            if(toUse->returnIndirect() == -1)
            {//we will try assigning one ourselves!
                int i=returnBitV(); 
                if(i == -1)
                {
                    cerr << "ERROR: no space available for full string storage." << endl;
                    return -1;
                }
                toUse->changeIndirect(i);
                BitVector[i]=1;
                UsedBitVector[i]=1;
                indBitVector[i]=1;                
            }

            //for passing over the file
            int blockP = (block_size * toUse->returnIndirect());
            char* indArr = returnBlock(toUse->returnIndirect());
            for(int i=0;i<block_size && indA < len;i++)//can take up to block_size blocks at most
            {
                char dInd = indArr[i];
                int nInd=(int) dInd;
                // cout << "";
                if(dInd == 0) //if the i place in the indirect block doesn't lead anywhere
                {
                    int j=returnBitV();
                    if(j==-1)   //if returnBitV didn't find a block to allocate its space to the file
                    {           //then simply end this function :(
                        cerr << "There are no blocks available for allocation, \nplease delete some blocks first." <<endl;
                        return -1;
                    }
                    BitVector[j]=1;
                    UsedBitVector[j]=1;
                    unsigned char c=0;decToBinary2(j,c);
                    rewriteCell(blockP+i,c); //put the binary value of j in place in the indirect block
                    nInd=j;
                }
                char *arr = returnBlock(nInd);
                for(int k=0;k<block_size && indA < len;k++)
                {
                    if(arr[k]=='\0')
                    {
                        rewriteCell(nInd*block_size+k,buf[indA]);
                        indA++;
                    }

                }
                delete [] arr;

                

            }
            delete [] indArr;


        }

        
        //once done with that, we will check if we assigned all the letters in the string
        if(indA!=len)
            cerr << "WARNING: there was not enough availble space for your entire string." << endl;
        return 0;


    }
    
    //delete the file from the mainDir and other stuff
    int DelFile( string FileName ) 
    {
       auto it=begin (OpenFileDescriptors);
       int toReturn=0;
       for ( ; it != end (OpenFileDescriptors); ++it) 
        {
            if(it->getFileName() == FileName)
                break;
            toReturn++;
        }
        if(it->getDead())
        {
            cerr << "ERROR: file already deleted." << endl;
            return -1;
        }
       
       if(MainDir.find(FileName)==MainDir.end()) //if you cannot find it in the 'directory'
       {
           cerr << "ERROR: no file found with this name." << endl;
           return -1;
       }
       fsInode *p = MainDir.at(FileName);
       MainDir.erase(FileName);
       
        for(int i=0;i<direct_enteris;i++)
        {
            if(p->returnDirect()[i]!=-1)
            {
                this->BitVector[p->returnDirect()[i]]=0;
            }
        }
        if(p->returnIndirect() != -1) //if any indirect blocks exist within the file!
        {
            int indI = p->returnIndirect();//where the indirect block starts!
            char* arrToDel = new char[block_size];

            int ret_val = fseek ( sim_disk_fd , (indI*block_size) , SEEK_SET );
            ret_val = fread(  arrToDel , 1 , block_size, sim_disk_fd);
            for(int i=0;i<block_size;i++)
            {
                int j=(int)arrToDel[i];
                if(j!=0)
                    BitVector[j]=0;
            }
            BitVector[indI]=0;
            indBitVector[indI]=0;
        }
        delete p;
        it->setDead();
        return toReturn;  
    }
    
    //read from the file
    int ReadFromFile(int fd, char *buf, int len ) 
    {

       //check for errors!
       {
       if(fd>this->OpenFileDescriptors.size() || this->OpenFileDescriptors[fd].isEmpty())
        {
            cerr << "ERROR: there is no fd in spot " << fd <<"." << endl;
            return -1;
        }
        else if(!OpenFileDescriptors[fd].isInUse())
        {
            cerr << "ERROR: file is not open, please open file first!" << endl;
            return -1;
        } else if(OpenFileDescriptors[fd].getDead())
        {
            cerr << "ERROR: file is deleted." << endl;
            return -1;
        }
        }
       
    
       int ind=0;
       fsInode *toUse = (this->OpenFileDescriptors[fd].getInode());
       //take as much as needed & possible from direct blocks
       {
       for(int i=0;i<this->direct_enteris && i<(len);i++)
        {
            int m = toUse->returnDirect()[i];
            if(m == -1)
            {
                continue;
            }
            char* arr = returnBlock(m);
            for(int j=0;j<block_size && ind < len;j++)
            {
                if(arr[j]!='\0')
                {
                    buf[ind]=arr[j];                    
                    ind++;
                }
            }
            delete [] arr;

        }
        }

        //if user requested more than the direct blocks, and fsInode has
        //chars on the 'disk', then add them to the string!
        if(len>direct_enteris && toUse->returnIndirect() != -1 && ind < len)
        {
            int indI = toUse->returnIndirect();//where the indirect block starts!
            char *arr=new char[block_size];
            char *indArr = returnBlock(indI);
            for(int i=0;i<block_size && ind < len;i++)
            {
                char bufy=indArr[i];
                //find out the location of the i block of data
                int lv2 = (int) bufy;
                if(lv2 == 0 || bufy == '\0') //if either one of these is true,
                    continue;               //then the block hasn't been assigned yet

                
                //copy block indicated by lv2 to arr
                lv2=lv2*block_size;
                /*arr=returnBlock(lv2);*/
                fseek(sim_disk_fd,lv2,SEEK_SET);
                fread(arr, 1 , block_size, sim_disk_fd);
                for(int j=0;j<block_size;j++)
                {
                    if(arr[j] != '\0')
                    {
                        buf[ind]=arr[j];
                        ind++;
                    }
                }
            }
        }

        buf[ind]='\0';
        return 0;
    }
    
    //-----ASSIGNMENT FUNCTIONS   END ------
    

    //------------------------------------
    //          MY FUNCTIONS
    //------------------------------------
    int returnAvB() //will tell us how many blocks are still available on the disk
    {
        int i=this->BitVectorSize;
        for(int j=0;j<BitVectorSize;j++)
            if(BitVector[j] == 1)
                i--;
        return i;
    }
    bool formatBool() //will return whether or not the fsDisk is 'formatted'
    {return is_formated;}
    int rewriteCell(int offset, char c) //will rewrite the letter in offset on file
    {
        
        int fd = open(DISK_SIM_FILE,O_RDWR ,0);
        lseek (fd, offset, SEEK_SET); 
        write (fd, &c, 1);
        close(fd);
        return 0;
    }
    char* returnBlock(int blocknum)
    {
        char *arr=new char[block_size];
        int fd = open(DISK_SIM_FILE,O_RDONLY,0);
        lseek(fd,blocknum*block_size,SEEK_SET);
        read(fd,arr,block_size);
        return arr;
    }
    int returnBitV()    //returns a block number that can be reused by the system
    {
        int i=0;
        for(;i<=BitVectorSize;i++)
        {
            if(i==BitVectorSize)
                break;
            else if(BitVector[i]==0)
                break;
        }
        if(i==BitVectorSize)
            return -1;
        if(UsedBitVector[i]==1) //if the block is pre-used,
        {                       //clean the cells in it (=make them '\0')
            for(int j=0;j<block_size;j++)
                rewriteCell(i*block_size+j,'\0');
        }
        return i;
    }
};

    //------------------------------------
    //        MY FUNCTIONS END
    //------------------------------------

//-------------CLASS fsDisk END-----------------

//-------------CLASSES END-----------------
    
int main() {
    int blockSize=0; 
	int direct_entries=0;
    string fileName="";
    char str_to_write[DISK_SIZE];
    char str_to_read[DISK_SIZE];
    int size_to_read=0; 
    int _fd=-1;		//assume no file was originally open
    fsDisk *fs = new fsDisk();
    int cmd_=-1;
    while(1) {
        cin >> cmd_;
        if(!fs->formatBool() && cmd_ != 0 && cmd_ != 2)  //check if fsDisk is formatted
        {
            cerr << "ERROR: FILE SYSTEM NOT FORMATTED, PLEASE FORMAT FIRST!" << endl;
            continue;
        } else if(fs->formatBool() && cmd_ == 2)
        {
            cerr << "ERROR: FILE SYSTEM ALREADY FORMATTED, CANNOT FORMAT TWICE." << endl;
            continue;
        }
        switch (cmd_)
        {
            case 0:   // exit   ***
				delete fs;
				exit(0);
                break;

            case 1:  // list-file   ***
                fs->listAll(); 
                break;
          
            case 2:    // format   ***
                cin >> blockSize;
				cin >> direct_entries;
                fs->fsFormat(blockSize, direct_entries);
                break;
          
            case 3:    // creat-file   ***
                cin >> fileName;
                _fd = fs->CreateFile(fileName);
                cout << "CreateFile: " << fileName << " with File Descriptor #: " << _fd << endl;
                break;
            
            case 4:  // open-file   ***
                cin >> fileName;
                _fd = fs->OpenFile(fileName);
                cout << "OpenFile: " << fileName << " with File Descriptor #: " << _fd << endl;
                break;
             
            case 5:  // close-file   ***
                cin >> _fd;
                fileName = fs->CloseFile(_fd); 
                cout << "CloseFile: " << fileName << " with File Descriptor #: " << _fd << endl;
                break;
           
            case 6:   // write-file
                cin >> _fd;
                cin >> str_to_write;
                fs->WriteToFile( _fd , str_to_write , strlen(str_to_write) );
                break;
          
            case 7:    // read-file  ***
                cin >> _fd;
                cin >> size_to_read ;
                fs->ReadFromFile( _fd , str_to_read , size_to_read );
                cout << "ReadFromFile: " << str_to_read << endl;
                break;
           
            case 8:   // delete file 
                 cin >> fileName;
                _fd = fs->DelFile(fileName);
                cout << "DeletedFile: " << fileName << " with File Descriptor #: " << _fd << endl;
                break;
            default:
                break;
        }
    }

} 