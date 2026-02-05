# Userspace driver 
There is no need to talk about differences between kernel drivers and userspace drivers, so let's dive into what is the alternatives and why they aren't selected.
## eBPF
Currently, biggest alternative was eBPF (e extension is for marketing. You can't see this extension in any source code.) and it's great with CO-RE, Compile Once - Run Everywhere, JIT, just in time compiler (provides almost the same performance as the main kernel code) and prevents kpanic with it's verifier.   
### Why not selected? 
In HFT, your rival company can also use this available extensions for optimization, and you can't pass no one with it. Since the industry is competing on microseconds/nanoseconds, this alternatives are totally bloat. 
## DPDK
Same goes for DPDK. Latency may be low enough on other workloads such on infrastructures, but not on here.
# Polling 
CPU's are going for sleep known as C states immediately their job is done. This provides efficiency when it comes to cooling & electricity bill, but waiting for your CPU to wake up will going to cost more than electricity. So, CPU will pool the descriptor done bit until it becomes 1 ( see [82599][82599] for descriptor data structures. Title 7.1.6 , Advanced Receive Descriptors.). As can be seen on the name of the document, this optimizations are like "low latency 101" class.
# Happy path is fast, error path is not.
In this implementation, happy path's are implemented as likely for static branch prediction. With this implementation, CPU's can fill the pipeline and continue from the if statement quickly. For those who are not familiar with branch prediction, CPU's will try to predict which way the if statement goes ( if you think in assembly, this means jumping or not) dynamicly. If implemented as likely/unlikely, it was manually provided to CPU branch predictor. See more on [wikipedia][branch_prediction].
# Static inlining
If a function is static inlined while developing, it's only affecting developers as clean and modular code. It was compiled like it's not a split function, ( since it's not.) but shouldn't used on one time init functions since it'll bloat main function & polluate caches. For %99 use cases, calling a function is nearly free, but HFT means trimming for nanoseconds. 
# Debugging 
For debugging, instead of print, counters or write(in one-shot init functions) is used. print is totally a bloat ( can be proven with a basic trace-cmd. It calls more than actual logic sometimes. ), can be used in one-shot init functions but not used because of my habits. In future implementations, another code can be used for reading this counters. With that implementation, observability can be provided without affecting data path.

[branch_prediction]: https://en.wikipedia.org/wiki/Branch_predictor
[82599]: https://www.intel.com/content/www/us/en/content-details/331520/intel-82599-10-gigabit-ethernet-controller-datasheet.html