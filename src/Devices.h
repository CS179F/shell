
// This code allows the user to turn any C++ stream into a unix-style
// device and gives a template for turing any hardware device attached
// to the system into a similar device that is properly linked into the 
// systems interrupt structure.

// It is important to understand the inheritance structure of C++
// streams: 1) every stream is either an istream, an ostream, or both
// as is every iostream.  All fstreams and and stringstreams are
// iostreams.  cin is an istream and not an fstream.  cout, cerr, and
// clog are ostreams and not fstreams.  Of coursse, any pointer or
// reference to anything automatically is also a pointer to anything
// that is further up the inheritance heirarcy.

// This code is for illustration only.  It complies, but I've not yet
// tested it.

#include <string>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <mutex>
#include <condition_variable>
#include <limits>
#include <climits>
#include <map>
#include <queue>
#include <iomanip>
#include <signal.h>
#include <sys/time.h>
#include <errno.h>
#include "thread.h"
using namespace std;


// Below, I've incorporated some of the interrupt-blockage code from
// my threads implementation.

//=========================== interrupts ======================

class InterruptSystem {
public:       // man sigsetops for details on signal operations.
  //static void handler(int sig);
  // exported pseudo constants
  static sigset_t on;                    
  static sigset_t alrmoff;    
  static sigset_t alloff;     
  InterruptSystem() {  
    //signal( SIGALRM, InterruptSystem::handler ); 
    sigemptyset( &on );                    // none gets blocked.
    sigfillset( &alloff );                  // all gets blocked.
    sigdelset( &alloff, SIGINT );
    sigemptyset( &alrmoff );      
    sigaddset( &alrmoff, SIGALRM ); //only SIGALRM gets blocked.
    set( alloff );        // the set() service is defined below.
    struct itimerval time;
    time.it_interval.tv_sec  = 0;
    time.it_interval.tv_usec = 400000;
    time.it_value.tv_sec     = 0;
    time.it_value.tv_usec    = 400000;
    cerr << "\nstarting timer\n";
    setitimer(ITIMER_REAL, &time, NULL);
  }
  sigset_t set( sigset_t mask ) {
    sigset_t oldstatus;
    pthread_sigmask( SIG_SETMASK, &mask, &oldstatus );
    return oldstatus;
  } // sets signal/interrupt blockage and returns former status.
  sigset_t block( sigset_t mask ) {
    sigset_t oldstatus;
    pthread_sigmask( SIG_BLOCK, &mask, &oldstatus );
    return oldstatus;
  } //like set but leaves blocked those signals already blocked.
};

// signal mask pseudo constants
sigset_t InterruptSystem::on;   
sigset_t InterruptSystem::alrmoff; 
sigset_t InterruptSystem::alloff;  

InterruptSystem interrupts;               // singleton instance.


//============================================================

// Here I've adapted my thread-coordination stuff to C++14.

class Monitor : public mutex {
public:
  sigset_t mask;
public:
  Monitor( sigset_t mask = InterruptSystem::alrmoff )   
    : mask(mask) 
  {}
};


class Sentry {  // To automatically block and restore interrupts.
  const sigset_t old;   // old stores the prior interrupt status.
public:
  Sentry( Monitor* m ) 
    : old( interrupts.block(m->mask) )                 
  {}
  ~Sentry() {
    interrupts.set( old );                              
  }
};


// Here is the translation of my coordination primitives to C++14's.
// The only thing that C++14 lacks is prioritized waiting, which is
// very important for scheduling but not for this project.

// The following version of EXCLUSION works correctly because C++ 
// invokes destructors in the opposite order of the constructors.
//#define EXCLUSION Sentry snt_(this); unique_lock<Monitor::mutex> lck(*this);
#define CONDITION condition_variable
#define WAIT      wait(lck)
#define SIGNAL    notify_one()


class Semaphore : public Monitor {  // A signal-safe implementation 
// of semaphores.  The common C++ version of semaphore is not
// async-signal safe in the sense defined by POSIX.
private:
  CONDITION available;
  int count;
public:
  Semaphore( int count = 0 )
    : count(count)
  {}
  void release() {
    EXCLUSION
    ++count;
    available.SIGNAL;
  }
  void acquire() {
    EXCLUSION
    while(count == 0) available.WAIT;
    --count;
  }
};

//==================================================================


template< typename Item >
class iDevice : public Monitor {

  istream& stream;

public:
 
  iDevice( istream& stream )
    : stream(stream)
  {}

  CONDITION ok2read;
  bool readCompleted;

  int input( Item* buffer, int n ) {
    EXCLUSION
    int i = 0;
    for ( ; i != n; ++i ) {
      if ( !stream ) break;
      stream >> buffer[i];                        // read a byte
      readCompleted = false;
      readCompleted = true;  // This line is for testing purposes
      // only.  Delete it when interrupts are working.
      while( ! readCompleted ) ok2read.WAIT;
    }
    return i;
  }

  void completeRead() {
    EXCLUSION
    readCompleted = true;
    ok2read.SIGNAL;
  }

};


template< typename Item >
class oDevice : public Monitor {

  ostream& stream;

public:
 
  oDevice( ostream& stream )
    : stream(stream)
  {}

  CONDITION ok2write;
  bool writeCompleted;

  int output( Item* buffer, int n ) {
    EXCLUSION
    int i = 0;
    for ( ; i != n; ++i ) {
      if ( !stream ) break;
      stream << buffer[i];                       // write a byte
      writeCompleted = false;
      writeCompleted = true;  // This line is for testing purposes
      // only.  Delete it when interrupts are working.
      while( ! writeCompleted ) ok2write.WAIT;
    }
    return i;
  }

  void completeWrite() {
    EXCLUSION
    writeCompleted = true;
    ok2write.SIGNAL;
  }

};


template< typename Item >
class ioDevice : public iDevice<Item>, public oDevice<Item> {

  iostream& stream;

  ioDevice( iostream& stream )  
    : iDevice<Item>(stream),
      oDevice<Item>(stream)
  {}

};



// Here is some stuff showing how to hook up interrupt handlers, etc.

vector<ioDevice<char>*> v;  

void readCompletionHandler() {  // When IO-completion interrupts
  // are available, this handler should be installed to be directly 
  // invoked by the hardawre's interrupt mechanism.
  v[0]->completeRead();
}

void writeCompletionHandler() {  // When IO-completion interrupts
  // are available, this handler should be installed to be directly 
  // invoked by the hardawre's interrupt mechanism.
  v[0]->completeWrite();
}


int main() {}  // Just to make this file compile.
