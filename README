# Custom Linux distribution for embedded systems using Buildroot

This has been developed for the Operating Systems Laboratory course at PUCRS. The following sections provide an overview of the modifications made. The target architecture is i686 and we emulate the target system on QEMU.

## Web server for basic system info

As a first experiment, we implemented a web server that runs on the target architecture and provides basic information on the target system, such as:

- Date & time;
- Uptime;
- Processor model & velocity;
- % of current CPU use;
- Available and total memory (RAM) of the system;
- System version;
- List of processes in execution;
- Disk unities and their capacities;
- And other data as well.

Those modifications are available in the [`custom-scripts/stats-server.py`](custom-scripts/stats-server.py) file.

## Kernel module for custom disk scheduling

We also implemented a kernel module that provides Shortest Seek Time First (SSTF) disk scheduling functionality. Those modifications are available in the [`modules/sstf_sched`](modules/sstf_sched) file.

## Custom CPU scheduling policy & profiler

Finally, we added a custom CPU scheduling policy in the Linux kernel called `SCHED_LOW_IDLE`, which has a lower priority than the existing `SCHED_IDLE` policy. Then, we added a profiler that allows the creation of processes/threads in the system with the existing scheduling policies, including the one we created, and provides an overview of their behavior while executing.

The profiler is available in the [`custom-scripts/sched-profiler`](custom-scripts/sched-profiler) directory, whereas the definition for the `SCHED_LOW_IDLE` policy has been added directly to the appropriate kernel files. PR #1 added them, so you might check it if you want some further details.
