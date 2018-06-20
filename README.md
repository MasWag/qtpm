# Quantitative Timed Pattern Matching [![wercker status](https://app.wercker.com/status/f1121fad1e4e172db519adf0da6d07f4/s/master "wercker status")](https://app.wercker.com/project/byKey/f1121fad1e4e172db519adf0da6d07f4)

This is our experimental implementation of *Quantitative Timed Pattern Matching* (Paper is not yet).

Usage
-----

### Synopsis

    qtpm [OPTIONS] -f FILE [FILE]

### Options

**-h**, **--help** Print a help message. <br />
**-q**, **--quiet** Quiet mode. Causes any results to be suppressed. <br />
**-V**, **--version** Print the version <br />
**-i** *file*, **--input** *file* Read a signal from *file*. <br />
**-f** *file*, **--automaton** *file* Read a timed automaton from *file*. <br />
**-a**, **--abs** absolute time mode
**--maxmin**  Use max-min semiring robust semantics (default). <br />
**--minplus**  Use min-plus semiring robust semantics . <br />

Installation
------------

This software is tested on Arch Linux and Mac OSX 10.13.5

### Requirements

* C++ compiler supporting C++14 and the corresponding libraries.
* Boost (>= 1.59)
* Eigen
* CMake

### Instructions

```sh
mkdir build
cd build && cmake -DCMAKE_BUILD_TYPE=Release .. && make && make install
```

Input Format
------------

### Signal

A signal is a sequence of the following line.

    v(x1) v(x2) v(x3) ... v(xn) time
