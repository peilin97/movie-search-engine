Is there a difference between running DirectoryParser_MT with one thread, vs the original single-threaded Directory Parser?

When parsing the data_small directory, DirectoryParser_MT with one thread is faster than/ or actually similar with the original single-threaded Directory Parser. There is a slight difference between these two parsers.
The time used by DirectoryParser_MT with one thread is 0.106321.
The time used by the original single-threaded parser is 0.112696.

What may explain what you observe?

DirectoryParser_MT with more threads took more time to finish the work and they are slower than the original single-threaded parser. Multiple-threading didn't help to faster the work.

When using more cores, DP_MT with multiple threads (I tried 5 and 10) took more time than just one core. Running benchmarker with one core is faster than multiple cores.

time stats for parsing the data_small directory
One core
original DP: 0.125607
DP_MT with one thread: 0.126325
DP_MT with five threads: 0.357353
DP_MT with ten threads: 0.665914

Two cores
original DP: 0.124774
DP_MT with one thread: 0.130740
DP_MT with five threads: 0.420067
DP_MT with ten threads: 0.710189

Four cores
original DP: 0.109742
DP_MT with one thread: 0.114390
DP_MT with five threads: 0.613335
DP_MT with ten threads: 0.985481

Eight cores
original DP: 0.120195
DP_MT with one thread: 0.131486
DP_MT with five threads: 0.766533
DP_MT with ten threads: 1.875243