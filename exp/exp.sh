#!/bin/sh

# ================== edge update for all dataset ====================

../build/sys/terrace/terrace_edge -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/terrace/edge.log
../build/sys/graphone/graphone_edge -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/graphone/edge.log
../build/sys/livegraph/livegraph_edge -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/livegraph/edge.log
../build/sys/risgraph/risgraph_edge -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/risgraph/edge.log
../build/sys/stinger/stinger_edge -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/stinger/edge.log
../build/sys/llama/llama_edge -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/llama/edge.log
../build/sys/teseo/teseo_edge -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/teseo/edge.log


../build/sys/terrace/terrace_edge -gname twitter -core 16 -f ../data/ADJgraph/twitter.adj -log ../log/terrace/edge.log
../build/sys/graphone/graphone_edge -gname twitter -core 16 -f ../data/ADJgraph/twitter.adj -log ../log/graphone/edge.log
../build/sys/livegraph/livegraph_edge -gname twitter -core 16 -f ../data/ADJgraph/twitter.adj -log ../log/livegraph/edge.log
../build/sys/risgraph/risgraph_edge -gname twitter -core 16 -f ../data/ADJgraph/twitter.adj -log ../log/risgraph/edge.log
../build/sys/stinger/stinger_edge -gname twitter -core 16 -f ../data/ADJgraph/twitter.adj -log ../log/stinger/edge.log
../build/sys/llama/llama_edge -gname twitter -core 16 -f ../data/ADJgraph/twitter.adj -log ../log/llama/edge.log
../build/sys/teseo/teseo_edge -gname twitter -core 16 -f ../data/ADJgraph/twitter.adj -log ../log/teseo/edge.log

../build/sys/terrace/terrace_edge -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/terrace/edge.log
../build/sys/graphone/graphone_edge -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/graphone/edge.log
../build/sys/livegraph/livegraph_edge -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/livegraph/edge.log
../build/sys/risgraph/risgraph_edge -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/risgraph/edge.log
../build/sys/stinger/stinger_edge -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/stinger/edge.log
../build/sys/llama/llama_edge -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/llama/edge.log
../build/sys/teseo/teseo_edge -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/teseo/edge.log


../build/sys/terrace/terrace_edge -gname graph500-24 -core 16 -f ../data/ADJgraph/graph500-24.adj -log ../log/terrace/edge.log
../build/sys/graphone/graphone_edge -gname graph500-24 -core 16 -f ../data/ADJgraph/graph500-24.adj -log ../log/graphone/edge.log
../build/sys/livegraph/livegraph_edge -gname graph500-24 -core 16 -f ../data/ADJgraph/graph500-24.adj -log ../log/livegraph/edge.log
../build/sys/risgraph/risgraph_edge -gname graph500-24 -core 16 -f ../data/ADJgraph/graph500-24.adj -log ../log/risgraph/edge.log
../build/sys/stinger/stinger_edge -gname graph500-24 -core 16 -f ../data/ADJgraph/graph500-24.adj -log ../log/stinger/edge.log
../build/sys/llama/llama_edge -gname graph500-24 -core 16 -f ../data/ADJgraph/graph500-24.adj -log ../log/llama/edge.log
../build/sys/teseo/teseo_edge -gname graph500-24 -core 16 -f ../data/ADJgraph/graph500-24.adj -log ../log/teseo/edge.log


../build/sys/terrace/terrace_edge -gname friendster -core 16 -f ../data/ADJgraph/friendster.adj -log ../log/terrace/edge.log
../build/sys/graphone/graphone_edge -gname friendster -core 16 -f ../data/ADJgraph/friendster.adj -log ../log/graphone/edge.log
../build/sys/livegraph/livegraph_edge -gname friendster -core 16 -f ../data/ADJgraph/friendster.adj -log ../log/livegraph/edge.log
../build/sys/risgraph/risgraph_edge -gname friendster -core 16 -f ../data/ADJgraph/friendster.adj -log ../log/risgraph/edge.log
../build/sys/stinger/stinger_edge -gname friendster -core 16 -f ../data/ADJgraph/friendster.adj -log ../log/stinger/edge.log
../build/sys/llama/llama_edge -gname friendster -core 16 -f ../data/ADJgraph/friendster.adj -log ../log/llama/edge.log
../build/sys/teseo/teseo_edge -gname friendster -core 16 -f ../data/ADJgraph/friendster.adj -log ../log/teseo/edge.log



#../build/sys/terrace/terrace_edge -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/terrace/edge.log
#../build/sys/terrace/terrace_edge -gname twitter -core 16 -f ../data/ADJgraph/twitter.adj -log ../log/terrace/edge.log
#../build/sys/terrace/terrace_edge -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/terrace/edge.log
#../build/sys/terrace/terrace_edge -gname graph500-24 -core 16 -f ../data/ADJgraph/graph500-24.adj -log ../log/terrace/edge.log
#../build/sys/terrace/terrace_edge -gname friendster -core 16 -f ../data/ADJgraph/friendster.adj -log ../log/terrace/edge.log
#
#../build/sys/graphone/graphone_edge -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/graphone/edge.log
#../build/sys/graphone/graphone_edge -gname twitter -core 16 -f ../data/ADJgraph/twitter.adj -log ../log/graphone/edge.log
#../build/sys/graphone/graphone_edge -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/graphone/edge.log
#../build/sys/graphone/graphone_edge -gname graph500-24 -core 16 -f ../data/ADJgraph/graph500-24.adj -log ../log/graphone/edge.log
#../build/sys/graphone/graphone_edge -gname friendster -core 16 -f ../data/ADJgraph/friendster.adj -log ../log/graphone/edge.log
#
#
#../build/sys/livegraph/livegraph_edge -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/livegraph/edge.log
#../build/sys/livegraph/livegraph_edge -gname twitter -core 16 -f ../data/ADJgraph/twitter.adj -log ../log/livegraph/edge.log
#../build/sys/livegraph/livegraph_edge -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/livegraph/edge.log
#../build/sys/livegraph/livegraph_edge -gname graph500-24 -core 16 -f ../data/ADJgraph/graph500-24.adj -log ../log/livegraph/edge.log
#../build/sys/livegraph/livegraph_edge -gname friendster -core 16 -f ../data/ADJgraph/friendster.adj -log ../log/livegraph/edge.log
#
#
#../build/sys/risgraph/risgraph_edge -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/risgraph/edge.log
#../build/sys/risgraph/risgraph_edge -gname twitter -core 16 -f ../data/ADJgraph/twitter.adj -log ../log/risgraph/edge.log
#../build/sys/risgraph/risgraph_edge -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/risgraph/edge.log
#../build/sys/risgraph/risgraph_edge -gname graph500-24 -core 16 -f ../data/ADJgraph/graph500-24.adj -log ../log/risgraph/edge.log
#../build/sys/risgraph/risgraph_edge -gname friendster -core 16 -f ../data/ADJgraph/friendster.adj -log ../log/risgraph/edge.log
#
#
#../build/sys/stinger/stinger_edge -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/stinger/edge.log
#../build/sys/stinger/stinger_edge -gname twitter -core 16 -f ../data/ADJgraph/twitter.adj -log ../log/stinger/edge.log
#../build/sys/stinger/stinger_edge -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/stinger/edge.log
#../build/sys/stinger/stinger_edge -gname graph500-24 -core 16 -f ../data/ADJgraph/graph500-24.adj -log ../log/stinger/edge.log
#../build/sys/stinger/stinger_edge -gname friendster -core 16 -f ../data/ADJgraph/friendster.adj -log ../log/stinger/edge.log
#
#../build/sys/llama/llama_edge -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/llama/edge.log
#../build/sys/llama/llama_edge -gname twitter -core 16 -f ../data/ADJgraph/twitter.adj -log ../log/llama/edge.log
#../build/sys/llama/llama_edge -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/llama/edge.log
#../build/sys/llama/llama_edge -gname graph500-24 -core 16 -f ../data/ADJgraph/graph500-24.adj -log ../log/llama/edge.log
#../build/sys/llama/llama_edge -gname friendster -core 16 -f ../data/ADJgraph/friendster.adj -log ../log/llama/edge.log
#
#../build/sys/teseo/teseo_edge -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/teseo/edge.log
#../build/sys/teseo/teseo_edge -gname twitter -core 16 -f ../data/ADJgraph/twitter.adj -log ../log/teseo/edge.log
#../build/sys/teseo/teseo_edge -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/teseo/edge.log
#../build/sys/teseo/teseo_edge -gname graph500-24 -core 16 -f ../data/ADJgraph/graph500-24.adj -log ../log/teseo/edge.log
#../build/sys/teseo/teseo_edge -gname friendster -core 16 -f ../data/ADJgraph/friendster.adj -log ../log/teseo/edge.log




# ================== aspen ====================

#../build/sys/aspen/aspen_edge -gname livejournal -core 16 -f ../data/ADJgraph/livejournal.adj -log ../log/aspen/edge.log
#../build/sys/aspen/aspen_edge -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/aspen/edge.log
#../build/sys/aspen/aspen_edge -gname twitter -core 16 -f ../data/ADJgraph/twitter.adj -log ../log/aspen/edge.log
#../build/sys/aspen/aspen_edge -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/aspen/edge.log
#../build/sys/aspen/aspen_edge -gname graph500-24 -core 16 -f ../data/ADJgraph/graph500-24.adj -log ../log/aspen/edge.log
#../build/sys/aspen/aspen_edge -gname friendster -core 16 -f ../data/ADJgraph/friendster.adj -log ../log/aspen/edge.log

#../build/sys/aspen/aspen_alg -gname livejournal -core 16 -f ../data/ADJgraph/livejournal.adj -log ../log/aspen/alg.log
#../build/sys/aspen/aspen_alg -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/aspen/alg.log
#../build/sys/aspen/aspen_alg -gname twitter -core 16 -f ../data/ADJgraph/twitter.adj -log ../log/aspen/alg.log
#../build/sys/aspen/aspen_alg -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/aspen/alg.log
#../build/sys/aspen/aspen_alg -gname graph500-24 -core 16 -f ../data/ADJgraph/graph500-24.adj -log ../log/aspen/alg.log
#../build/sys/aspen/aspen_alg -gname friendster -core 16 -f ../data/ADJgraph/friendster.adj -log ../log/aspen/alg.log

#../build/sys/aspen/aspen_edge -gname livejournal -core 1 -f ../data/ADJgraph/livejournal.adj -log ../log/aspen/edge.log
#../build/sys/aspen/aspen_edge -gname livejournal -core 4 -f ../data/ADJgraph/livejournal.adj -log ../log/aspen/edge.log
#../build/sys/aspen/aspen_edge -gname livejournal -core 8 -f ../data/ADJgraph/livejournal.adj -log ../log/aspen/edge.log
#../build/sys/aspen/aspen_edge -gname livejournal -core 12 -f ../data/ADJgraph/livejournal.adj -log ../log/aspen/edge.log


# ================== teseo ====================

#../build/sys/teseo/teseo_edge -gname livejournal -core 16 -f ../data/ADJgraph/livejournal.adj -log ../log/teseo/edge.log
#../build/sys/teseo/teseo_edge -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/teseo/edge.log
#../build/sys/teseo/teseo_edge -gname twitter -core 16 -f ../data/ADJgraph/twitter.adj -log ../log/teseo/edge.log
#../build/sys/teseo/teseo_edge -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/teseo/edge.log
#../build/sys/teseo/teseo_edge -gname graph500-24 -core 16 -f ../data/ADJgraph/graph500-24.adj -log ../log/teseo/edge.log
#../build/sys/teseo/teseo_edge -gname friendster -core 16 -f ../data/ADJgraph/friendster.adj -log ../log/teseo/edge.log

#../build/sys/teseo/teseo_alg -gname livejournal -core 16 -f ../data/ADJgraph/livejournal.adj -log ../log/teseo/alg.log
#../build/sys/teseo/teseo_alg -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/teseo/alg.log
#../build/sys/teseo/teseo_alg -gname twitter -core 16 -f ../data/ADJgraph/twitter.adj -log ../log/teseo/alg.log
#../build/sys/teseo/teseo_alg -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/teseo/alg.log
#../build/sys/teseo/teseo_alg -gname graph500-24 -core 16 -f ../data/ADJgraph/graph500-24.adj -log ../log/teseo/alg.log
#../build/sys/teseo/teseo_alg -gname friendster -core 16 -f ../data/ADJgraph/friendster.adj -log ../log/teseo/alg.log

#../build/sys/teseo/teseo_edge -gname livejournal -core 1 -f ../data/ADJgraph/livejournal.adj -log ../log/teseo/edge.log
#../build/sys/teseo/teseo_edge -gname livejournal -core 4 -f ../data/ADJgraph/livejournal.adj -log ../log/teseo/edge.log
#../build/sys/teseo/teseo_edge -gname livejournal -core 8 -f ../data/ADJgraph/livejournal.adj -log ../log/teseo/edge.log
#../build/sys/teseo/teseo_edge -gname livejournal -core 12 -f ../data/ADJgraph/livejournal.adj -log ../log/teseo/edge.log



