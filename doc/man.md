% QTPM(1)
% Masaki Waga
% June 2018

# NAME

qtpm - An experimental implementation of Quantitative Timed Pattern Matching with weighted automata

# SYNOPSIS

    qtpm [OPTIONS] -f FILE [FILE]

# DESCRIPTION

The tool **qtpm** reads signal from standard input or a file and execute quantitative timed pattern matching.

The options are as follows:

**-h**, **--help** 
: Print a help message.

**-q**, **--quiet**
: Quiet mode. Causes any results to be suppressed.

**-V**, **--version**
: Print the version.

**-i** *file*, **--input** *file*
: Read a signal from *file*.

**-f** *file*, **--automaton** *file*
: Read a (timed) automaton from *file*.

**-u**, **--untimed** (default)
: Untimed filtering mode.

**-t**, **--timed**
: Timed filtering mode.

**-a**, **--abs**:
absolute time mode

**--maxmin** (default)
: Use max-min semiring robust semantics.

**--minplus** 
: Use min-plus semiring robust semantics.


# Examples

The command:

    qtpm -f ta.dot < signal.txt
    
or 

    qtpm -f ta.dot -i signal.txt

read the word from *signal.txt* and write the result of quantitative timed pattern matching masked word to standard output. The semantics is the space robustness over max-min semiring.
