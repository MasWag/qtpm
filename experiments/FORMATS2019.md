Experiments Data for FORMATS 2019
=============================

This is a document on the experiment data for the FORMATS 2019 paper [Waga19].
We implemented and evaluated our tool SyMon. We compiled SyMon using GCC 4.9.3. See also [READMD.md](../README.md).

Experimental raw data
---------------------

We conducted the experiments on an Amazon EC2 c4.large instance (2.9 GHz Intel Xeon E5-2666 v3, 2 vCPUs, and 3.75 GiB RAM) that runs Ubuntu 18.04 LTS (64 bit).

The DOT and signature files are in this directory.

Files
-----

The input logs can be downloaded from [HERE](https://1drv.ws/u/s!AgmwSTftArYRjTWEsDTD1PTS75bS?e=fqmdUk).

- `overshoot.dot`: TSA for `Overshoot` benchmark
- `ringing.dot`: TSA for `Ringing` benchmark
- `overshoot_unbounded.dot`: TSA for `Overshoot (Unbounded)` benchmark

Description of the Benchmarks
-----------------------------

All the benchmarks are inspired by the properties in [KJD+16] for automotive specifications.
The input signals are generated from [tprasadtp/cruise-control-simulink](https://github.com/tprasadtp/cruise-control-simulink).

### Overshoot

The TSA is for the settling when the reference value of the velocity is changed from vref < 35 to vref > 35. The duration of the matching signals is less than 150 time units.

### Ringing

The TSA is for the frequent rise and fall of the signal in 80 time units. The constraints rise and fall are rise = v(t) − v(t − 10) > 10 and fall = v(t) − v(t − 10) < −10.

### Overshoot (Unbounded)

The TSA is almost the same as that in Overshoot, but the time-bound (c < 150) is removed.

Calling qtpm
-------------

For all case studies, the following command was used:

>  ./build/qtpm -i [signal.tsv] -f [overshoot.dot] --abs

References
----------

<dl>
<dt>[Waga19]</dt>
<dd>Masaki Waga, 
Online Quantitative Timed Pattern Matching with Semiring-Valued Weighted Automata. In Proc. FORMATS 2019, LNCS 11750, pp. 3-22.</dd>
<dt>[KJD+16]</dt>
<dd>James Kapinski, Xiaoqing Jin, Jyotirmoy Deshmukh, Alexandre Donze, To- moya Yamaguchi, Hisahiro Ito, Tomoyuki Kaga, Shunsuke Kobuna, and Sanjit Seshia. St-lib: a library for specifying and classifying model behav- iors. Technical report, SAE Technical Paper, 2016.</dd>
</dl>
