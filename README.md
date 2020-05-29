# intel-perfcounter-bug
Inter Remote and Local loads performance Counters bug

Affected counters: mem_load_l3_miss_retired.remote_dram , mem_load_l3_miss_retired.local_dram

Description: 
  2 programs: 
1. programs/sharedmem-set.c <br>
  It will mmap an amount os memory as shared using and mmap and will fill data.
  Then it will start random access into this memory. 
2. programs/sharedmem-get.c <br>
  This program will get the memory allocated by the previous program using shm_open and mmap.
  It then will do random access in the area. 

Process: <br>
1. Run program 1 on CPU of one node with memory bindind to same node and launch program 2 on a CPU of another node. 

Expected Result : 
1. Almost all access to memory of program 1 should be local access as all its memory is allocted on the same node. 
2. Almost all access to memory of program 2 should be remote access as all its memory is allocted on the other node by process 1.

Result: 
>> I can see a significant amount of remote memory access as reported by the perf counter mem_load_l3_miss_retired.remote_dram.
