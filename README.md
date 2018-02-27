## Message Passing and Operating System Simulator

**oss (operating system similator)** is the main process who forks off children user processes. All processes have access to a simulated system clock that is protected by critical sections implemented with message queues. A termination log is also implemented with message queues so that children user processes can send oss a message containing information when they terminate.

Each child user process will run for a random duration and during that time will continuously update the simulated system clock. When they are done running they will send a message to oss who will then fork off another child process.

The program (by default) will generate a log file called oss.log.

Possible command line arguments are:  
- -h [help]  
- -s x  
- -l filename  
- -t z  
where x is the maximum number of slave processes spawned (default 5) and filename is the log file used. The parameter z is the time in seconds when the master will terminate itself and all children (default 20).

To build this program run:
```
make
```

To run this program with default configurations:
```    
./oss
```

For possible command line options run:
```    
./oss -h
```

To clean up run:
```
make clean
```

### Description of problems encountered:

Below is my git log (in oneline format):
