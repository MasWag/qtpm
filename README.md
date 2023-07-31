Quantitative Timed Pattern Matching
===================================

[![Boost.Test](https://github.com/MasWag/qtpm/actions/workflows/boosttest.yml/badge.svg?branch=master)](https://github.com/MasWag/qtpm/actions/workflows/boosttest.yml)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](./LICENSE)
![Doxygen](https://img.shields.io/badge/docs-Doxygen-deepgreen.svg)](https://maswag.github.io/qtpm/doxygen/index.html)

This is our experimental implementation of *Quantitative Timed Pattern Matching*.
See [FORMATS2019.md](./experiments/README.md) for the experiments in our FORMATS 2019 paper.

Demo on Google Colab is [HERE](https://colab.research.google.com/drive/1y1LU0pKQb5rMcuwwA9VwEb5Ec8I2qvlf)!!

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
**-a**, **--abs** absolute time mode. <br />
**--maxmin**  Use max-min semiring robust semantics (default). <br />
**--minplus**  Use min-plus semiring robust semantics. <br />
**--maxplus**  Use max-plus semiring robust semantics. <br />
**--boolean**  Use boolean semantics. <br />

Installation
------------

This software is tested on Ubuntu 20.04, Ubuntu 22.04, macOS 11, macOS 12, macOS 13, and Arch Linux.

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

     time v(x1) v(x2) v(x3) ... v(xn)

How to make compile_commands.json
---------------------------------

``` shell
mkdir build
cd build && cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
```

Reference
---------

- [Masaki Waga](http://group-mmm.org/~mwaga/), **Online Quantitative Timed Pattern Matching with Semiring-Valued Weighted Automata**, In *Proc. [FORMATS 2019](https://lipn.univ-paris13.fr/formats2019/)*, LNCS 11750, pp. 3-22 **(Best paper awarded)**. [arXiv version](https://arxiv.org/abs/1906.12133)
