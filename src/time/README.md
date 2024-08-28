# Time

## Scheduler

It runs on even intervals between execution. Like the main loop in game engines - no matter how long it takes to process all components in the hiaerarchy in a single moment in time, the next moment will happen exactly after the scheduled delay.

### Main loop

And this is useless for now but precise worker that repeats tasks at moments separated by the delay, skipping time required to execute tasks. So no matter how long tasks will take (if it less then the delay), the starting moment for tasks is exact

```cpp
{{#include ../../time/scheduler.cpp:29:55}}
```