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
[bar] Sleeping for 99978us twice

function                                                    line	calls	   tottime	avgtime

main........................................................ 57       1            1.01s      1.01s 
 foo for loop............................................... 60       1          585.59ms   585.59ms
  double foo(int)........................................... 10       10         585.59ms    58.56ms
   nop...................................................... 17       1000000     18.47ms    18.47ns
 bar wait................................................... 66       1          200.41ms   200.41ms
  bar....................................................... 23       1          100.21ms   100.21ms
 fibb....................................................... 71       1          196.97ms   196.97ms
  fibb...................................................... 31       177        196.74ms     1.11ms
 rec1 + rec2................................................ 76       1           27.82ms    27.82ms
  rec1...................................................... 41       89          27.77ms   312.06us
   rec2..................................................... 49       88          26.19ms   297.62us

function                                                    line	calls	   tottime	avgtime

main........................................................ 57       0            0.00ns     -nans 
 foo for loop............................................... 60       0            0.00ns     -nans 
  double foo(int)........................................... 10       0            0.00ns     -nans 
   nop...................................................... 17       0            0.00ns     -nans 
 bar wait................................................... 66       0            0.00ns     -nans 
  bar....................................................... 23       0            0.00ns     -nans 
 fibb....................................................... 71       0            0.00ns     -nans 
 fibb....................................................... 31       15          16.89ms     1.13ms
 rec1 + rec2................................................ 76       0            0.00ns     -nans 
  rec1...................................................... 41       0            0.00ns     -nans 
   rec2..................................................... 49       0            0.00ns     -nans 
fibb........................................................ 87       1           16.91ms    16.91ms

```
