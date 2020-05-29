#!/bin/bash

#########################################################################################################
# Config parameters 
#########################################################################################################
CPU1=1
CPU2=19
NODEBIND=0

SOURCE=$(readlink -f "`dirname $(readlink -f "$0")`")
MACHINE=$(cat /proc/cpuinfo | grep "model name" | head -1 | awk '{print $7}')

NUMACTLEXTRA=""

BENCHMARK_CPU1=$SOURCE/binaries/bin_sharedmem-set
BENCHMARK_CPU2=$SOURCE/binaries/bin_sharedmem-get

PERF=/home/akash/perf_5.4
#########################################################################################################
# Error Functions
#########################################################################################################
print_cpu_error()
{
    echo "Error: CPU $1 not supportted for script"
    exit
}

print_error()
{
    echo "Error: $1 $2"
    exit
}
#########################################################################################################

if [ $MACHINE = "E7-4850" ]; then
    PERF_EVENT_LOCAL_DRAM="mem_load_uops_l3_miss_retired.local_dram:u"
    PERF_EVENT_REMOTE_DRAM="mem_load_uops_l3_miss_retired.remote_dram:u"
    PERF_EVENTS="$PERF_EVENT_LOCAL_DRAM,$PERF_EVENT_REMOTE_DRAM"
elif [ $MACHINE = "6140" ] || [ $MACHINE = "6252" ]; then
    PERF_EVENT_LOCAL_DRAM="mem_load_l3_miss_retired.local_dram:u"
    PERF_EVENT_REMOTE_DRAM="mem_load_l3_miss_retired.remote_dram:u"
    PERF_EVENTS="$PERF_EVENT_LOCAL_DRAM,$PERF_EVENT_REMOTE_DRAM"
else
    print_cpu_error $MACHINE
fi

run_benchmarks()
{
	# --- launch first
    numactl -C $CPU1 -m $NODEBIND $BENCHMARK_CPU1 &
	PID1=$!
	echo "PID1: $PID1"
	OUTFILE1=$SOURCE/logs/$PID1.log
	$PERF stat -a -C $CPU1 -x, -o $OUTFILE1 --append -e $PERF_EVENTS -p $PID1 -I 5000 &
	PERF_PID1=$!
    if [ $RUN_SINGLE_PROCESS -eq 0 ]
    then
        sleep 10
        # --- launch second
        numactl -C $CPU2 $BENCHMARK_CPU2 &
        PID2=$!
        echo "PID2: $PID2"
        OUTFILE2=$SOURCE/logs/$PID2.log
        $PERF stat -a -C $CPU2 -x, -o $OUTFILE2 --append -e $PERF_EVENTS -p $PID2 -I 5000 &
        PERF_PID2=$!
    fi
	echo "Waiting for benchmarks to be done."
	wait $PID1
    if [ $RUN_SINGLE_PROCESS -eq 0 ]
    then 
    	wait $PID2
    fi
	kill -INT $PERF_PID1
    if [ $RUN_SINGLE_PROCESS -eq 0 ]
    then
    	kill -INT $PERF_PID2
    fi
}

RUN_SINGLE_PROCESS=0
run_benchmarks 
echo "Done."
