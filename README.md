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


Instructions for vtune: 
Consideting Cpu 0 and 19 lie on different sockets/nodes. 
Run program 1 using vtune
>> vtune -collect  memory-access --cpu-mask=0 -r my_log_dir_2 -- numactl --physcpubind=0 --membind=0 binaries/bin_sharedmem-set
The after 10 seconds, run 
>> numactl -C 19 binaries/bin_sharedmem-get

Expected Result : 
1. Almost all access to memory of program 1 should be local access as all its memory is allocted on the same node. 
Actual : 
>> LLC Miss Count: 3,552,052,641
        Local DRAM Access Count: 1,967,896,200
        Remote DRAM Access Count: 1,771,106,580
        Remote Cache Access Count: 0


Expected vtune report is that most of the DRAM access should be local for the program 1 which is executed using vtune.
