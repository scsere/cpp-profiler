# Simple manual cpp profiler

[Fork of uranix/cpp-profiler](https://github.com/uranix/cpp-profiler) to use c++11 chrono and allow to reset the profiler.

Procs:
* Single header
* Profile only what you need
* Profiling arbitrary scope
* Early profiling scope ending

Cons:
* Overhead ~300ns per call
* You need to explicitely mark what you want to profile
* May break in multithreaded environments
* Combines calls to the same function from different contexts, e.g. if `foo() -> baz(), bar() -> baz()` then `baz()` will be shown only after `foo()`
* Complex recursion (e.g. `rec1` calls `rec2` and `rec2` calls `rec1`) may lead to incorrect total time for both `rec1` and `rec2`.

## Demo output
```
function                                                    line	calls	   tottime	avgtime

main:demo.cpp............................................... 57       1            1.01s      1.01s 
 foo for loop:demo.cpp...................................... 60       1          586.26ms   586.26ms
  double foo(int):demo.cpp.................................. 10       10         586.26ms    58.63ms
   nop:demo.cpp............................................. 17       1000000     18.41ms    18.41ns
 bar wait:demo.cpp.......................................... 66       1          200.43ms   200.43ms
  bar:demo.cpp.............................................. 23       1          100.23ms   100.23ms
 fibb:demo.cpp.............................................. 71       1          196.30ms   196.30ms
  fibb:demo.cpp............................................. 31       177        196.02ms     1.11ms
 rec1 + rec2:demo.cpp....................................... 76       1           27.78ms    27.78ms
  rec1:demo.cpp............................................. 41       89          27.75ms   311.76us
   rec2:demo.cpp............................................ 49       88          26.17ms   297.38us

function                                                    line	calls	   tottime	avgtime

main:demo.cpp............................................... 57       0            0.00ns     -nans 
 foo for loop:demo.cpp...................................... 60       0            0.00ns     -nans 
  double foo(int):demo.cpp.................................. 10       0            0.00ns     -nans 
   nop:demo.cpp............................................. 17       0            0.00ns     -nans 
 bar wait:demo.cpp.......................................... 66       0            0.00ns     -nans 
  bar:demo.cpp.............................................. 23       0            0.00ns     -nans 
 fibb:demo.cpp.............................................. 71       0            0.00ns     -nans 
 fibb:demo.cpp.............................................. 31       15          16.26ms     1.08ms
 rec1 + rec2:demo.cpp....................................... 76       0            0.00ns     -nans 
  rec1:demo.cpp............................................. 41       0            0.00ns     -nans 
   rec2:demo.cpp............................................ 49       0            0.00ns     -nans 
fibb:demo.cpp............................................... 87       1           16.29ms    16.29ms
```
