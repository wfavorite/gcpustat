# gcpu - A Linux graphical CPU monitor

This is a tool for viewing CPU utilization, statistics, and logical layout.

### Intent and Usage

In the *default* mode, gcpu allows you to view CPU utilization and scheduling in a way that is:
 + logically presented (Intel hyperthreads on the same physical CPU line and all cores on a socket grouped together)
 + color coded by utilization ("warmer" colours represent higher utilization, "cooler" colours represent lower utilization)
 + combined with alternative stats such as interrupts on that CPU and CPU speed

Running at one-second intervals with speed (-p) and IRQ (-i) stats enabled. (Note that cpu0/cpu2 and cpu1/cpu3 are hyperthreads on the same physical core(s).)
<pre>
./gcpu -pi 1
      User Nice  Sys Idle   MHz   IrqCnt         User Nice  Sys Idle   MHz   IrqCnt   
cpu0    1%   0%   1%  98%   799        3   cpu2    1%   0%   0%  99%   903        0
cpu1    1%   0%   0%  99%   999        2   cpu3    0%   0%   0% 100%   999        0
</pre>

Two less obvious modes are the "dump" modes that print varying levels of detail about the CPUs in the system.

The -d(etailed dump) and -D(dump) modes display CPU information.

<pre>
./gcpu -d
Total logical cores (threads in system): 4
Core width (threads per core): 2
Socket height (threads per socket): 4
Socket height (cores per socket): 2

SOCKET   0     Intel(R) Atom(TM) CPU D510   @ 1.66GHz
  CORE 0
    LOGICAL 0  1662 MHz
      CACHE level=1; type=Data; size=24K;
      CACHE level=1; type=Instruction; size=32K;
      CACHE level=2; type=Unified; size=512K;
    LOGICAL 2  1662 MHz
      CACHE level=1; type=Data; size=24K;
      CACHE level=1; type=Instruction; size=32K;
      CACHE level=2; type=Unified; size=512K;
  CORE 1
    LOGICAL 1  1662 MHz
      CACHE level=1; type=Data; size=24K;
      CACHE level=1; type=Instruction; size=32K;
      CACHE level=2; type=Unified; size=512K;
    LOGICAL 3  1662 MHz
      CACHE level=1; type=Data; size=24K;
      CACHE level=1; type=Instruction; size=32K;
      CACHE level=2; type=Unified; size=512K;
</pre>

### Development

The project is coded in C++ 11 with some design optimizations aimed at performance. (The tool should not *consume* the CPUs it is monitoring.) The code is free to use with BSD licensing.


