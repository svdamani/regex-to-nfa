# regex-to-nfa
Convert a Regular Expression to Nondeterministic finite automaton with ε-moves

Build it with `make`, and visualize a simple regular expression, such as
`((a|b|c)*|(d|e)+)f`.

    make
    ./generate_nfa '((a|b|c)*|(d|e)+)f'

 - nfa.dot : ε-NFA constructed directly from the regular expression

*Attention*: Graphviz must be installed

## Dependencies

    sudo apt-get install gcc graphviz xdot

## Overview

`generate_nfa` takes a simple regular expression from commandline and generates DOT
file named nfa.dot in current working directory, which is the non-deterministic
finite automaton constructed from that regular expression. This dot file
can be converted to various image formats by graphviz.

    dot -Tpdf nfa.dot -o nfa.pdf
    dot -Tps nfa.dot -o nfa.ps

The "regular expression" this program accepts can have only these symbols :

 - Alternative         '|'
 - Kleene star         '*'
 - Optional mark       '?'
 - Positive closure    '+'

The vocabulary is also restricted (only alnums as input characters).