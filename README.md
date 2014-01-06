klog
====

An In-memory Kernel Logging Library

klog.c is an easy-to-use and efficient in-memory logging module in the Linux kernel. It leverages pre-allocated buffer to log messages during the kernel execution, for example, processing in the network protocol stack. In our example, we insert some profiling logic into net/ipv4/tcp_cong.c of the Linux 2.6.35.14 kernel to keep track of the change of congestion window controlled by RENO. 

To compile klog.c in the kernel, add "obj-m += klog.o" in net/ipv4/Makefile

To load the klog module into the kernel, run "insmod /lib/modules/$(uname -r)/kernel/net/ipv4/klog.ko" 

To unload the klog module from the kernel, run "rmmod klog" and the logging messages will be output into the file /etc/myconfig
