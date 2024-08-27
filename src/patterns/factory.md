# Factory

We would want to write a tool that can provide similar functionality between components with different implementations. It should be easily expandable with new components that doesn't require to change anything in the main app.

For this task we are going to use a factory pattern.

## Interface

**AbstractTime.h**

```cpp
{{#include ../../patterns/factory/AbstractTime.h:6:12}}
```

## Factory

Factory itself will be a singleton. And essentially it's a collection of build functions. And we will be calling `register_builder` in an interesting way (check for constructors in concrete objects).

**TimeFactory.h**

```cpp
{{#include ../../patterns/factory/TimeFactory.h:9:31}}
```

**TimeFactory.cpp**

```cpp
{{#include ../../patterns/factory/TimeFactory.cpp}}
```

## Define components

Implement the interface

```cpp
{{#include ../../patterns/factory/MotherboardRTC.h:7:14}}
```

And now just define a stati object. But this line actually calls `register_builder` in a static object of `TimeFactory` class. So this object will be available in the main function.

```cpp
{{#include ../../patterns/factory/MotherboardRTC.cpp:3}}
```

## Use components

```cpp
{{#include ../../patterns/factory/main.cpp:26:31}}
```

## Result

```bash
./time_factory list
# random
# system

./time_factory get system
# system: 08/26/24 20:43:02 Pacifique (heure dТete)
```