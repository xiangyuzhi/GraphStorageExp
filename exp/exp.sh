#!/bin/sh


../build/sys/terrace/terrace_alg -src 9 -maxiters 10 -gname LiveJournal -core 4 -f ../data/ADJgraph/LiveJournal.adj
../build/sys/terrace/terrace_alg -src 9 -maxiters 10 -gname LiveJournal -core 8 -f ../data/ADJgraph/LiveJournal.adj
../build/sys/terrace/terrace_alg -src 9 -maxiters 10 -gname LiveJournal -core 12 -f ../data/ADJgraph/LiveJournal.adj
../build/sys/terrace/terrace_alg -src 9 -maxiters 10 -gname LiveJournal -core 16 -f ../data/ADJgraph/LiveJournal.adj


