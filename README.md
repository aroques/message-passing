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

### Program Hanging
This program will randomly hang after oss forks a child and updates the clock that is in shared memory. It hangs as if there are no children processes waiting to receive a message at the clock that is in shared memory. If the program hangs press ctrl-c to interrupt the program and try running it again.  

Below is my git log:

6ccc75e 2018-03-05 19:08:10 -0600 Add git log and program hanging note to README  
6c54adc 2018-03-01 15:02:18 -0600 Check if process exists before sending SIGTERM to it  
f2409b0 2018-02-28 18:14:18 -0600 Count waiting to receive message as time 'worked'  
424e078 2018-02-27 15:43:46 -0600 Add bullet points to README  
f1904ea 2018-02-27 15:42:40 -0600 Add newlines to README  
3f2b1bb 2018-02-27 15:40:43 -0600 Add program and cmd line descriptions to README  
09d2b1f 2018-02-27 15:31:26 -0600 Untrack .project  
7388ab0 2018-02-27 15:27:21 -0600 Add variable filename from cmd line  
8f0661a 2018-02-27 15:22:20 -0600 Add command line argument options  
cdc1e29 2018-02-27 14:48:43 -0600 Add writing output to logfile  
56fe72c 2018-02-27 10:39:45 -0600 Change idx var name in fork_child()  
a7e5d35 2018-02-27 10:34:54 -0600 Add bitwise pid to srand(), modify increment_sysclock() logic()  
f4c1004 2018-02-27 10:21:01 -0600 Improve console output, change 1 billion to float  
e6e9071 2018-02-27 10:00:26 -0600 Implement logging as printfs and cleanup unneeded code  
e8e7f3d 2018-02-25 20:53:54 -0600 Add termination log id  
01ce4ab 2018-02-25 20:47:28 -0600 Rename msgbuf to sysclock  
0282a73 2018-02-25 20:45:41 -0600 Rename snd, rcv, to update_clock(), read_clock()  
0385c31 2018-02-25 20:38:09 -0600 Add termlog stuct  
a8032c9 2018-02-25 18:34:32 -0600 Cleanup unneeded code  
348003c 2018-02-25 18:30:32 -0600 Received incremented clock from user in oss  
7fa7276 2018-02-25 15:25:29 -0600 Add timer  
cdac2e6 2018-02-25 15:12:21 -0600 Add cleanup_and_exit()  
cf27071 2018-02-25 15:10:00 -0600 Add terminate_children()  
4f45312 2018-02-25 14:59:00 -0600 Replace signal with sigaction  
f0b35e8 2018-02-25 12:30:33 -0600 Add remove_message_queue() and sigint handler  
b2c69e5 2018-02-25 12:10:59 -0600 Rename if guard to meesage queue  
a40d9fe 2018-02-25 12:09:56 -0600 Move .project to new line  
a914455 2018-02-25 12:08:26 -0600 Rename shared memory to message queue  
cc734b5 2018-02-25 12:05:56 -0600 Add variables and childpid array to oss.c  
6ea1279 2018-02-25 10:55:31 -0600 Remove unneeded code/comments  
5708fdd 2018-02-25 10:45:27 -0600 Cleanup printfs in user.c  
b002ad3 2018-02-25 10:43:52 -0600 Condense sbuf initialization  
e95d49e 2018-02-25 10:37:11 -0600 Swap message buff char array with clock struct  
143c72e 2018-02-25 10:27:31 -0600 Remove unneeded code from oss.c  
2a06778 2018-02-25 10:25:42 -0600 Remove key from get_shared_memory()  
9fd11c6 2018-02-25 10:23:31 -0600 Pass qid instead of key  
8a18663 2018-02-25 10:19:50 -0600 Successfully received a message  
e559cc2 2018-02-25 09:50:26 -0600 Cleanup code  
9f84e18 2018-02-25 09:31:23 -0600 Test message passing  
d5e2467 2018-02-25 09:30:50 -0600 Test message passing  
7459594 2018-02-23 14:21:54 -0600 Fix if guard  
41222cb 2018-02-23 14:20:08 -0600 Initial commit  