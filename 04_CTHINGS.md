# C/CPP Things

## Type Definitions

Use clear types.

```cpp
#include <stdint.h>

uint8_t var1; // unsigned 8-bit int
int32_t var2; // signed 32-bit int
// ...
```

## Return Conventions

Functions _often_ return a positive integer value on success and a negative
value on failure. Some returned objects can also be used as similar to booleans.

```cpp
int32_t ret = some_function_call();
if (ret != 0) {
    // Error
}

SomeObject ret = some_function_call();
if (!ret) {
    // Error
}
```

## Inifinite Loops

```cpp
// Variant 1
while (1) { /* ... */ }

// Variant 2
for (;;) { /* ... */ }
```

## The `using` Keyword

Include something from the standard library.

```cpp
// BAD: imports the entire namespace, can potentially lead to naming conflicts.
using namespace std;

// BETTER: Just import functions that are needed a lot.
using std::printf;

// WITHOUT using
std::printf("Hello, World\n");

// WITH using
printf("Hello, World\n");
```

## Memory Management.

Generally `new` should be pared with `delete` and `malloc` with `free`.

```cpp
T* var = new T // Constructor called.
delete var;    // Destructor called.

void* var = malloc(5); // Raw memory allocated.
free(var);             // Memory freed.
```

Some background:

See [Stackoverflow](https://stackoverflow.com/a/6816851).

Object lifetime in C++ follows: Allocate, Construct, Deconstruct, Deallocate.

`new` allocates and constructs, `delete` destructs and deallocates.
(Technically, `delete` only deallocates, but the compiler usually automatically
inserts a call to the deconstructor.)

```cpp
T* var = (T*)::operator new(sizeof(T)); // Allocate raw memory.
var = new (var) T;                      // Call the constructor.

var->~T();                              // Deconstruct the object.
::operator delete(var);                 // Deallocate the memory.
```

## Function Definitions

In C:

```c
void foo(void) // A function taking _no_ arguments.
void foo()     // An unspecified number of arguments of unspecified type.
```

In C++:

```cpp
void foo(void) // A function taking _no_ arguments. (Same as in C.)
void foo()     // Also a function taking _no_ arguments.
```
