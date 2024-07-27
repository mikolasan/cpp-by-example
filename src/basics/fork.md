# Fork

<div class="warning">
Linux only
</div>

Execution sequence is interesting in this case

```cpp
{{#include ../../basics/fork/fork.cpp}}
```

```sh
{{#include ../../basics/fork/long_process.sh}}
```

You should be carefull when call functions like `system` or `fork` itself in applications that consume a lot of memory (like games) because fork will clone the application process first which wil require free space equal at least to current needs of the application.