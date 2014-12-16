#include <string>
#include <iostream>
#include <vector>
#include <list>
#include <sstream>
#include <sys/wait.h>
#include <errno.h>                       // man errno for information
#include <cassert>
//#include <thread>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <stdlib.h>
#include <stdlib.h>
#include <cstring>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "thread.h"
#include <mutex>
#include "filesystem.h"


using namespace filesystem;
using namespace std;

#define each(I) for( typeof((I).begin()) it=(I).begin(); it!=(I).end(); ++it )

int doit( vector<string> tok );
/*
struct Devices{
  int deviceNumber;
  string driverName;
};

struct openFileTable{
  Devices *ptr; //pointer to device
  bool write;
  bool read;
};


struct processTable{
    pid_t pid;
    pid_t *ppid;
    openFileTable opfile[32];
   
    void setid(pid_t p, pid_t pp){pid = p; ppid = &pp;}
    void print(){
        cout << "[PID: " << pid << "][PPID: " << *ppid << "]" << endl;
    }
};

thread_local processTable tmp;
*/
int doit( vector<string> tok );
class shellThread : public Thread {
    vector<string> tok;
    int priority() {return Thread::priority(); }
    void action (){
		
    
        string progname = tok[0];
        //tmp.setid(getpid(), getppid());
        //testCompleteMe();
        char* arglist[ 1 + tok.size() ];   // "1+" for a terminating null ptr.
        int argct = 0;
        for ( int i = 0; i != tok.size(); ++i ) {
     
      string progname = tok[0];
      char* arglist[ 1 + tok.size() ];   // "1+" for a terminating null ptr.
      int argct = 0;
      for ( int i = 0; i != tok.size(); ++i ) {
            if      ( tok[i] == "&" || tok[i] == ";" ) break;   // arglist done.
            else if ( tok[i] == "<"  ) freopen( tok[++i].c_str(), "r", stdin  );
            else if ( tok[i] == ">"  ) freopen( tok[++i].c_str(), "w", stdout );
            else if ( tok[i] == ">>" ) freopen( tok[++i].c_str(), "a", stdout );
            else if ( tok[i] == "2>" ) freopen( tok[++i].c_str(), "w", stderr );
            else if ( tok[i] == "|"  ) {                   // create a pipeline.
              int mypipe[2];  
              int& pipe_out = mypipe[0];
              int& pipe_in  = mypipe[1];
              // Find two available ports and create a pipe between them, and
              // store output portuntitled folder# into pipe_out and input port# to pipe_in.
              if ( pipe( mypipe ) ) {     // All that is done here by pipe().
                //cerr << "myshell: " << strerror(errno) << endl; // report err
                return;
              } else if ( fork() ) {  // you're the parent and consumer here.
            dup2( pipe_out, STDIN_FILENO ); // connect pipe_out to stdin.
                close( pipe_out );        // close original pipe connections.
                close( pipe_in );  
                while ( tok.front() != "|" ) tok.erase( tok.begin() );
                tok.erase(tok.begin());                    // get rid of "|".
                exit( doit( tok ) );        // recurse on what's left of tok.
              } else {                 // you're the child and producer here.
                dup2( pipe_in, STDOUT_FILENO ); // connect pipe_in to stdout.
                close( pipe_out );        // close original pipe connections.
                close( pipe_in );
                break;                      // exec with the current arglist.
              }
            } 
            else {           // add this token a C-style argv for execvp().
              // Append tok[i].c_str() to arglist
              arglist[argct] = new char[1+tok[i].size()]; 
              strcpy( arglist[argct], tok[i].c_str() );
              // arglist[argct] = const_cast<char*>( tok[i].c_str() );
              // Per C++2003, Section 21.3.7: "Nor shall the program treat
              // the returned value [ of .c_str() ] as a valid pointer value
              // after any subsequent call to a non-const member function of
              // basic_string that designates the same object as this."
              // And, there are no subsequent operations on these strings.
              arglist[++argct] = 0; // C-lists of strings end w null pointer.
            }
          }
         

          // tilde expansion
          if ( progname[0] == '~' ) progname = getenv("HOME")+progname.substr(1);
//          execvp( progname.c_str(), arglist );         // execute the command.

		  Inode<App>* junk = static_cast<Inode<App>*>((dynamic_cast<Inode<Directory>*>(root->file->theMap["bin"])->file->theMap)[tok[0]] ); //Update to put apps in a directory

		    if ( ! junk ) {
			  (dynamic_cast<Inode<Directory>*>(root->file->theMap["bin"])->file->theMap).erase(tok[0]);
			  cerr << "shell: " << tok[0] << " command not found\n";
			  continue;
			}
			App* thisApp = static_cast<App*>(junk->file);
			if ( thisApp != 0 ) {
			  thisApp(tok);          // if possible, apply cmd to its args.
			  return;
			} else { 
			  cerr << "Instruction " << tok[0] << " not implemented.\n";
			}


          // If we get here, an error occurred in the child's attempt to exec.
          //cerr << "myshell: " << strerror(errno) << endl;     // report error.
          //exit(0);                  // child must not return, so must die now.
        }
    }
   
   
 
   
    public:
        shellThread(string name, int priority, vector<string> v)
        :tok(v), Thread(name, priority)
        {}
       
};

int doit( vector<string> tok ) { 
  // Executes a parsed command line returning command's exit status.

  if ( tok.size() == 0 ) return 0;             // nothing to be done.

  string progname = tok[0]; 
  assert( progname != "" );
  if(progname == "menu"){
    cout << "ls: list contents of the current directory" << endl; 
  cout << "mkdir: creates directory with specified name" << endl; 
  cout << "rmdir: removes directory with specified name" << endl; 
  cout << "exit: ends execution of the shell"<< endl; 
  cout <<  "rm: removes file with specified name"<< endl; 
  cout << "cd: changes current working directory to specified directory"<< endl;
 cout << "touch: access/change access and modification timestamps"<< endl;
  cout << "pwd: displays current working directory"<< endl;
cout << "tree: outputs list of directories and associated directories"<< endl;
cout << "echo: sends specified string to standard output"<< endl;
cout << "cat: concatenates specified string onto standard output or specified file" << endl;
cout << "wc: prints newline, word, and byte counts for each file" << endl;
cout << "write: writes to a file descriptor" << endl;
cout << "read: read specified amount of bytes from file descriptor into buffer"<< endl;
cout << "cp: copies specified file or directory"<< endl;
return -1;
  }
  if(progname == "pstree"){
    
    
  }
 
      
  // A child process can't cd for its parent.
  /*
  if ( progname == "cd" ) {                    // chdir() and return.
    chdir( tok.size() > 1 ? tok[1].c_str() : getenv("HOME") );
    if ( ! errno ) return 0;
    cerr << "myshell: cd: " << strerror(errno) << endl;
    return -1;
  }
	*/
  // fork.  And, wait if child to run in foreground.
  /*if ( pid_t kidpid = fork() )
  {      
    if ( errno || tok.back() == "&") return 0;
    int temp = 0;               
    waitpid( kidpid, &temp, 0 );
    return ( WIFEXITED(temp) ) ? WEXITSTATUS(temp) : -1;
  }*/
  // You're the child.
  //cerr << "Thread starting\n";
  shellThread thread1 ("Temp name", INT_MAX,tok);
  //cerr << "thread exiting\n";
  thread1.join();
  //cerr << "returning\n";
  return 0;
}




int main( int argc, char* argv[] ) {
///*
  FSInit("info.txt");
  while ( ! cin.eof() ) {
    cout << "? " ;                                         // prompt.
    //testCompleteMe();
    // testCompleteMe();
    string temp = "";
    getline( cin, temp );
    cout.flush();

    stringstream ss(temp);      // split temp at white spaces into v.
    while ( ss ) {
      vector<string> v;
      string s;
      while ( ss >> s ) {
        v.push_back(s);
        if ( s == "&" || s == ";" ) break;   
      }
     // thread t(do_work);
	  //cerr <<"Entering doit\n";
      int status = doit( v );           // FIX make status available.
      //cerr << "Exiting doit\n";
      //if ( errno ) cerr << "myshell: " << strerror(errno) << endl;
    }

  }
  //cerr << "exit" << endl;
  return 0;                                                  // exit.
  //*/
//    testCompleteMe();
}


///////////////// Diagnostic Tools /////////////////////////

   // cout.flush();
   // if ( WIFEXITED(status) ) { // reports exit status of proc.
   //   cout << "exit status = " << WEXITSTATUS(status) << endl;
   // }

