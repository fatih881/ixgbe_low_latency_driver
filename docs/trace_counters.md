# Summary 
Some functions have counters in enter-happy exit path, since the processes can exit without implemented returns. (e.g, kpanic)
Within this traces, this possibility can be traced on hexdump.  
Since increasing counters is a atomic operation, this implementation found logical especially on init functions. In functions which will be going to run on data path, it can be optional / never implemented.