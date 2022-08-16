#! /bin/bash

#../build/sys/teseo/teseo_edge -gname uniform-24 -core 12 -f ../data/ADJgraph/uniform-24.adj -log ../log/teseo/edge.log

# ---- mem ----

#../build/sys/teseo/teseo_mem -gname graph500-24 -core 16 -f ../data/ADJgraph/graph500-24.adj -log ../log/teseo/mem.log
#../build/sys/teseo/teseo_mem -gname livejournal -core 16 -f ../data/ADJgraph/livejournal.adj -log ../log/teseo/mem.log


#for data in "livejournal" "orkut" "uniform-24" "graph500-24" "twitter";
#  do
#    s="../build/sys/teseo/teseo_mem -gname ${data} -core 16 -f ../data/ADJgraph/${data}.adj -log ../log/teseo/mem.log"
#    echo ${s}
#    $s
#  done



#  ----   alg ----
#for sys in "pcsr" "ligra" "risgraph";
#do
#  for data in "uniform-24" "orkut";
#  do
#    s="../build/sys/${sys}/${sys}_alg -gname ${data} -core 16 -f ../data/ADJgraph/${data}.adj -log ../log/${sys}/alg.log"
#    echo ${s}
#    $s
#  done
#done
#
#
#for sys in "pcsr" "ligra" "risgraph";
#do
#  for data in "uniform-24" "orkut";
#  do
#    s="../build/sys/${sys}/${sys}_scala -thread -gname ${data} -core 16 -f ../data/ADJgraph/${data}.adj -log ../log/${sys}/scala.log"
#    echo ${s}
#    $s
#  done
#  s="../build/sys/${sys}/${sys}_scala -log ../log/${sys}/scala.log"
#  echo ${s}
#  $s
#done


## ---- scala ----

for sys in "teseo";
do
  for ((i=0;i<1;i++))
  do
    for v in 21 22 23 24 25 26;
    do
      s="../build/sys/${sys}/${sys}_scala -v ${v} -e 30 -log ../log/${sys}/scala.log"
      echo ${s}
      $s
    done
    for e in 10 20 30 40 50 60 70;
    do
      s="../build/sys/${sys}/${sys}_scala -v 23 -e ${e} -log ../log/${sys}/scala.log"
      echo ${s}
      $s
    done
  done
done



for sys in "teseo";
do
  for data in "orkut" "uniform-24";
  do
    for thd in 1 4 8 12 16;
    do
      s="../build/sys/${sys}/${sys}_scala -thread -core ${thd} -gname ${data} -f ../data/ADJgraph/${data}.adj -log ../log/${sys}/scala.log"
      echo ${s}
      $s
    done
  done
done


#for thd in 1 4 8 12 16;
#  do
#      s="../build/sys/graphone/graphone_scala -gname ${data} -thread -core ${thd} -f ../data/ADJgraph/${data}.adj -log ../log/graphone/scala.log"
#      echo ${s}
#      $s
#  done
# "livegraph" "graphone" "stinger"  "llama" "terrace" "aspen" ;
#for sys in "risgraph";
#do
#  for data in "orkut" "uniform-24";
#  do
#    for ((i=0;i<3;i++))
#    do
#      s="../build/sys/${sys}/${sys}_edge -gname ${data} -core 16 -f ../data/ADJgraph/${data}.adj -log ../log/${sys}/edge.log"
#      echo ${s}
#      $s
#    done
#  done
#done









#for thd in 1 4 8 12 16;
#  do
#    for data in "orkut" "uniform-24";
#    do
#      s="../build/sys/pcsr/pcsr_edge -gname ${data} -core ${thd} -f ../data/ADJgraph/${data}.adj -log ../log/pcsr/scala.log"
#      echo ${s}
#      $s
#    done
#  done
#
#for thd in 4 8 12;
#  do
#    for data in "orkut";
#    do
#      for sys in "llama" "teseo";
#      do
#        s="../build/sys/${sys}/${sys}_edge -gname ${data} -core ${thd} -f ../data/ADJgraph/${data}.adj -log ../log/${sys}/scala.log"
#        echo ${s}
#        $s
#      done
#    done
#  done


#for data in  "orkut";
#do
#  for sys in "llama";
#  # "stinger" "uniform-24"
#  do
#    for thd in 1 4 8 16;
#    do
#      echo ${data} ${sys}
#      s="../build/sys/${sys}/${sys}_scala -thread -core ${thd} -gname ${data} -f ../data/ADJgraph/${data}.adj -log ../log/${sys}/scala.log"
#      echo ${s}
#      $s
#    done
#  done
#done
#
#thd=(4 8 12)
#data=("uniform-24" "graph500-24")
#for ((j=0;j<3;j++))
#  do
#    for ((i=0;i<2;i++))
#    do
#      s="../build/sys/llama/llama_edge -gname ${data[i]} -core ${thd[j]} -f ../data/ADJgraph/${data[i]}.adj -log ../log/llama/scala.log"
#      s="../build/sys/teseo/teseo_edge -gname ${data[i]} -core ${thd[j]} -f ../data/ADJgraph/${data[i]}.adj -log ../log/teseo/scala.log"
#      echo ${s}
#      $s
#    done
#  done






#../build/sys/teseo/teseo_mem -gname twitter -core 16 -f ../data/ADJgraph/twitter.adj -log ../log/teseo/mem.log










#data=("uniform-24" "orkut")
## "livejournal" "graph500-24"
#sys=("aspen")
##  "teseo" "aspen"
#
#for ((i=0;i<2;i++))
#do
#  for ((j=0;j<1;j++))
#  do
#    echo ${data[i]} ${sys[j]}
#    s="../build/sys/${sys[j]}/${sys[j]}_alg -gname ${data[i]} -core 16 -f ../data/ADJgraph/${data[i]}.adj -log ../log/${sys[j]}/alg.log"
#    echo ${s}
#    $s
#  done
#done


#for ((i=0;i<2;i++))
#do
#  for ((j=0;j<1;j++))
#  do
#      echo ${data[i]} ${sys[j]}
#      s="../build/sys/${sys[j]}/${sys[j]}_scala -gname ${data[i]} -f ../data/ADJgraph/${data[i]}.adj -log ../log/${sys[j]}/scala.log"
#      echo ${s}
#      $s
#  done
#done


# ================= edge insert/delete for dataset orkut & uniform-24 ===================
#../build/sys/aspen/aspen_edge -gname orkut -core 1 -f ../data/ADJgraph/orkut.adj -log ../log/aspen/edge.log
#../build/sys/terrace/terrace_edge -gname orkut -core 1 -f ../data/ADJgraph/orkut.adj -log ../log/terrace/edge.log
#../build/sys/graphone/graphone_edge -gname orkut -core 1 -f ../data/ADJgraph/orkut.adj -log ../log/graphone/edge.log
#../build/sys/livegraph/livegraph_edge -gname orkut -core 1 -f ../data/ADJgraph/orkut.adj -log ../log/livegraph/edge.log

#../build/sys/risgraph/risgraph_edge -gname orkut -core 1 -f ../data/ADJgraph/orkut.adj -log ../log/risgraph/edge.log

#../build/sys/llama/llama_edge -gname orkut -core 1 -f ../data/ADJgraph/orkut.adj -log ../log/llama/edge.log
#../build/sys/teseo/teseo_edge -gname orkut -core 1 -f ../data/ADJgraph/orkut.adj -log ../log/teseo/edge.log
#../build/sys/pcsr/pcsr_edge -gname orkut -core 1 -f ../data/ADJgraph/orkut.adj -log ../log/pcsr/edge.log
#
#../build/sys/aspen/aspen_edge -gname uniform-24 -core 1 -f ../data/ADJgraph/uniform-24.adj -log ../log/aspen/edge.log
#../build/sys/terrace/terrace_edge -gname uniform-24 -core 1 -f ../data/ADJgraph/uniform-24.adj -log ../log/terrace/edge.log
#../build/sys/graphone/graphone_edge -gname uniform-24 -core 1 -f ../data/ADJgraph/uniform-24.adj -log ../log/graphone/edge.log
#../build/sys/livegraph/livegraph_edge -gname uniform-24 -core 1 -f ../data/ADJgraph/uniform-24.adj -log ../log/livegraph/edge.log
#../build/sys/risgraph/risgraph_edge -gname uniform-24 -core 1 -f ../data/ADJgraph/uniform-24.adj -log ../log/risgraph/edge.log
#../build/sys/stinger/stinger_edge -gname uniform-24 -core 1 -f ../data/ADJgraph/uniform-24.adj -log ../log/stinger/edge.log
#../build/sys/llama/llama_edge -gname uniform-24 -core 1 -f ../data/ADJgraph/uniform-24.adj -log ../log/llama/edge.log
#../build/sys/teseo/teseo_edge -gname uniform-24 -core 1 -f ../data/ADJgraph/uniform-24.adj -log ../log/teseo/edge.log
#../build/sys/teseo/teseo_edge -gname orkut -core 1 -f ../data/ADJgraph/orkut.adj -log ../log/teseo/edge.log
#../build/sys/teseo/teseo_edge -gname uniform-24 -core 1 -f ../data/ADJgraph/uniform-24.adj -log ../log/teseo/edge.log
#../build/sys/pcsr/pcsr_edge -gname uniform-24 -core 1 -f ../data/ADJgraph/uniform-24.adj -log ../log/pcsr/edge.log
#../build/sys/stinger/stinger_edge -gname orkut -core 1 -f ../data/ADJgraph/orkut.adj -log ../log/stinger/edge.log



# === for alg ===
#../build/sys/terrace/terrace_alg -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/terrace/alg.log
#../build/sys/terrace/terrace_alg -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/terrace/alg.log
#../build/sys/pcsr/pcsr_alg -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/pcsr/alg.log
#../build/sys/pcsr/pcsr_alg -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/pcsr/alg.log
#../build/sys/stinger/stinger_alg -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/stinger/alg.log
#../build/sys/ligra/ligra_alg -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/ligra/alg.log
#../build/sys/ligra/ligra_alg -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/ligra/alg.log


# == for mem ===
#../build/sys/terrace/terrace_mem -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/terrace/mem.log
#../build/sys/graphone/graphone_mem -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/graphone/mem.log
#../build/sys/livegraph/livegraph_mem -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/livegraph/mem.log
#../build/sys/risgraph/risgraph_mem -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/risgraph/mem.log
#../build/sys/stinger/stinger_mem -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/stinger/mem.log
#../build/sys/llama/llama_mem -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/llama/mem.log
#../build/sys/teseo/teseo_mem -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/teseo/mem.log
#../build/sys/ligra/ligra_mem -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/ligra/mem.log
#../build/sys/pcsr/pcsr_mem -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/pcsr/mem.log

#../build/sys/terrace/terrace_mem -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/terrace/mem.log
#../build/sys/graphone/graphone_mem -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/graphone/mem.log
#../build/sys/livegraph/livegraph_mem -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/livegraph/mem.log
#../build/sys/risgraph/risgraph_mem -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/risgraph/mem.log
#../build/sys/stinger/stinger_mem -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/stinger/mem.log
#../build/sys/llama/llama_mem -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/llama/mem.log
#../build/sys/teseo/teseo_mem -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/teseo/mem.log
#../build/sys/ligra/ligra_mem -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/ligra/mem.log
#../build/sys/pcsr/pcsr_mem -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/pcsr/mem.log

# ====== for medium read ======
#../build/sys/terrace/terrace_alg -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/terrace/alg.log
#../build/sys/graphone/graphone_alg -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/graphone/alg.log
#../build/sys/livegraph/livegraph_alg -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/livegraph/alg.log
#../build/sys/risgraph/risgraph_alg -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/risgraph/alg.log
#../build/sys/stinger/stinger_alg -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/stinger/alg.log
#../build/sys/llama/llama_alg -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/llama/alg.log
#../build/sys/teseo/teseo_alg -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/teseo/alg.log
#../build/sys/ligra/ligra_alg -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/ligra/alg.log
#../build/sys/pcsr/pcsr_alg -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/pcsr/alg.log
#
#../build/sys/terrace/terrace_alg -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/terrace/alg.log
#../build/sys/graphone/graphone_alg -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/graphone/alg.log
#../build/sys/livegraph/livegraph_alg -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/livegraph/alg.log
#../build/sys/risgraph/risgraph_alg -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/risgraph/alg.log
#../build/sys/stinger/stinger_alg -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/stinger/alg.log
#../build/sys/llama/llama_alg -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/llama/alg.log
#../build/sys/teseo/teseo_alg -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/teseo/alg.log
#../build/sys/ligra/ligra_alg -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/ligra/alg.log
#../build/sys/pcsr/pcsr_alg -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/pcsr/alg.log

# ================== edge update for all dataset ====================

#../build/sys/terrace/terrace_edge -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/terrace/edge.log
#../build/sys/graphone/graphone_edge -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/graphone/edge.log
#../build/sys/livegraph/livegraph_edge -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/livegraph/edge.log
#../build/sys/risgraph/risgraph_edge -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/risgraph/edge.log
#../build/sys/stinger/stinger_edge -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/stinger/edge.log
#../build/sys/llama/llama_edge -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/llama/edge.log
#../build/sys/teseo/teseo_edge -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/teseo/edge.log
#
#
#../build/sys/terrace/terrace_edge -gname twitter -core 16 -f ../data/ADJgraph/twitter.adj -log ../log/terrace/edge.log
#../build/sys/graphone/graphone_edge -gname twitter -core 16 -f ../data/ADJgraph/twitter.adj -log ../log/graphone/edge.log
#../build/sys/livegraph/livegraph_edge -gname twitter -core 16 -f ../data/ADJgraph/twitter.adj -log ../log/livegraph/edge.log
#../build/sys/risgraph/risgraph_edge -gname twitter -core 16 -f ../data/ADJgraph/twitter.adj -log ../log/risgraph/edge.log
#../build/sys/stinger/stinger_edge -gname twitter -core 16 -f ../data/ADJgraph/twitter.adj -log ../log/stinger/edge.log
#../build/sys/llama/llama_edge -gname twitter -core 16 -f ../data/ADJgraph/twitter.adj -log ../log/llama/edge.log
#../build/sys/teseo/teseo_edge -gname twitter -core 16 -f ../data/ADJgraph/twitter.adj -log ../log/teseo/edge.log
#
#../build/sys/terrace/terrace_edge -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/terrace/edge.log
#../build/sys/graphone/graphone_edge -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/graphone/edge.log
#../build/sys/livegraph/livegraph_edge -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/livegraph/edge.log
#../build/sys/risgraph/risgraph_edge -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/risgraph/edge.log
#../build/sys/stinger/stinger_edge -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/stinger/edge.log
#../build/sys/llama/llama_edge -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/llama/edge.log
#../build/sys/teseo/teseo_edge -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/teseo/edge.log
#
#
#../build/sys/terrace/terrace_edge -gname graph500-24 -core 16 -f ../data/ADJgraph/graph500-24.adj -log ../log/terrace/edge.log
#../build/sys/graphone/graphone_edge -gname graph500-24 -core 16 -f ../data/ADJgraph/graph500-24.adj -log ../log/graphone/edge.log
#../build/sys/livegraph/livegraph_edge -gname graph500-24 -core 16 -f ../data/ADJgraph/graph500-24.adj -log ../log/livegraph/edge.log
#../build/sys/risgraph/risgraph_edge -gname graph500-24 -core 16 -f ../data/ADJgraph/graph500-24.adj -log ../log/risgraph/edge.log
#../build/sys/stinger/stinger_edge -gname graph500-24 -core 16 -f ../data/ADJgraph/graph500-24.adj -log ../log/stinger/edge.log
#../build/sys/llama/llama_edge -gname graph500-24 -core 16 -f ../data/ADJgraph/graph500-24.adj -log ../log/llama/edge.log
#../build/sys/teseo/teseo_edge -gname graph500-24 -core 16 -f ../data/ADJgraph/graph500-24.adj -log ../log/teseo/edge.log
#
#
#../build/sys/terrace/terrace_edge -gname friendster -core 16 -f ../data/ADJgraph/friendster.adj -log ../log/terrace/edge.log
#../build/sys/graphone/graphone_edge -gname friendster -core 16 -f ../data/ADJgraph/friendster.adj -log ../log/graphone/edge.log
#../build/sys/livegraph/livegraph_edge -gname friendster -core 16 -f ../data/ADJgraph/friendster.adj -log ../log/livegraph/edge.log
#../build/sys/risgraph/risgraph_edge -gname friendster -core 16 -f ../data/ADJgraph/friendster.adj -log ../log/risgraph/edge.log
#../build/sys/stinger/stinger_edge -gname friendster -core 16 -f ../data/ADJgraph/friendster.adj -log ../log/stinger/edge.log
#../build/sys/llama/llama_edge -gname friendster -core 16 -f ../data/ADJgraph/friendster.adj -log ../log/llama/edge.log
#../build/sys/teseo/teseo_edge -gname friendster -core 16 -f ../data/ADJgraph/friendster.adj -log ../log/teseo/edge.log


# ================== algorithm for all dataset ====================

#../build/sys/terrace/terrace_edge -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/terrace/edge.log
#../build/sys/graphone/graphone_edge -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/graphone/edge.log
#../build/sys/livegraph/livegraph_edge -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/livegraph/edge.log
#../build/sys/risgraph/risgraph_edge -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/risgraph/edge.log
#../build/sys/stinger/stinger_edge -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/stinger/edge.log
#../build/sys/llama/llama_edge -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/llama/edge.log
#../build/sys/teseo/teseo_edge -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/teseo/edge.log
#
#
#../build/sys/terrace/terrace_edge -gname twitter -core 16 -f ../data/ADJgraph/twitter.adj -log ../log/terrace/edge.log
#../build/sys/graphone/graphone_edge -gname twitter -core 16 -f ../data/ADJgraph/twitter.adj -log ../log/graphone/edge.log
#../build/sys/livegraph/livegraph_edge -gname twitter -core 16 -f ../data/ADJgraph/twitter.adj -log ../log/livegraph/edge.log
#../build/sys/risgraph/risgraph_edge -gname twitter -core 16 -f ../data/ADJgraph/twitter.adj -log ../log/risgraph/edge.log
#../build/sys/stinger/stinger_edge -gname twitter -core 16 -f ../data/ADJgraph/twitter.adj -log ../log/stinger/edge.log
#../build/sys/llama/llama_edge -gname twitter -core 16 -f ../data/ADJgraph/twitter.adj -log ../log/llama/edge.log
#../build/sys/teseo/teseo_edge -gname twitter -core 16 -f ../data/ADJgraph/twitter.adj -log ../log/teseo/edge.log
#
#../build/sys/terrace/terrace_edge -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/terrace/edge.log
#../build/sys/graphone/graphone_edge -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/graphone/edge.log
#../build/sys/livegraph/livegraph_edge -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/livegraph/edge.log
#../build/sys/risgraph/risgraph_edge -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/risgraph/edge.log
#../build/sys/stinger/stinger_edge -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/stinger/edge.log
#../build/sys/llama/llama_edge -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/llama/edge.log
#../build/sys/teseo/teseo_edge -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/teseo/edge.log
#
#
#../build/sys/terrace/terrace_edge -gname graph500-24 -core 16 -f ../data/ADJgraph/graph500-24.adj -log ../log/terrace/edge.log
#../build/sys/graphone/graphone_edge -gname graph500-24 -core 16 -f ../data/ADJgraph/graph500-24.adj -log ../log/graphone/edge.log
#../build/sys/livegraph/livegraph_edge -gname graph500-24 -core 16 -f ../data/ADJgraph/graph500-24.adj -log ../log/livegraph/edge.log
#../build/sys/risgraph/risgraph_edge -gname graph500-24 -core 16 -f ../data/ADJgraph/graph500-24.adj -log ../log/risgraph/edge.log
#../build/sys/stinger/stinger_edge -gname graph500-24 -core 16 -f ../data/ADJgraph/graph500-24.adj -log ../log/stinger/edge.log
#../build/sys/llama/llama_edge -gname graph500-24 -core 16 -f ../data/ADJgraph/graph500-24.adj -log ../log/llama/edge.log
#../build/sys/teseo/teseo_edge -gname graph500-24 -core 16 -f ../data/ADJgraph/graph500-24.adj -log ../log/teseo/edge.log
#
#
#../build/sys/terrace/terrace_edge -gname friendster -core 16 -f ../data/ADJgraph/friendster.adj -log ../log/terrace/edge.log
#../build/sys/graphone/graphone_edge -gname friendster -core 16 -f ../data/ADJgraph/friendster.adj -log ../log/graphone/edge.log
#../build/sys/livegraph/livegraph_edge -gname friendster -core 16 -f ../data/ADJgraph/friendster.adj -log ../log/livegraph/edge.log
#../build/sys/risgraph/risgraph_edge -gname friendster -core 16 -f ../data/ADJgraph/friendster.adj -log ../log/risgraph/edge.log
#../build/sys/stinger/stinger_edge -gname friendster -core 16 -f ../data/ADJgraph/friendster.adj -log ../log/stinger/edge.log
#../build/sys/llama/llama_edge -gname friendster -core 16 -f ../data/ADJgraph/friendster.adj -log ../log/llama/edge.log
#../build/sys/teseo/teseo_edge -gname friendster -core 16 -f ../data/ADJgraph/friendster.adj -log ../log/teseo/edge.log

#../build/sys/terrace/terrace_alg -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/terrace/alg.log
#../build/sys/terrace/terrace_sssp -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/terrace/alg.log
#../build/sys/graphone/graphone_alg -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/graphone/alg.log
#../build/sys/livegraph/livegraph_alg -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/livegraph/alg.log
#../build/sys/risgraph/risgraph_alg -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/risgraph/alg.log
#../build/sys/stinger/stinger_alg -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/stinger/alg.log
#../build/sys/llama/llama_alg -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/llama/alg.log
#../build/sys/teseo/teseo_alg -gname orkut -core 16 -f ../data/ADJgraph/orkut.adj -log ../log/teseo/alg.log


#../build/sys/terrace/terrace_alg -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/terrace/alg.log
#../build/sys/terrace/terrace_sssp -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/terrace/alg.log
#../build/sys/livegraph/livegraph_alg -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/livegraph/alg.log
#../build/sys/risgraph/risgraph_alg -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/risgraph/alg.log
#../build/sys/stinger/stinger_alg -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/stinger/alg.log
#../build/sys/llama/llama_alg -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/llama/alg.log
#../build/sys/teseo/teseo_alg -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/teseo/alg.log
#../build/sys/graphone/graphone_alg -gname uniform-24 -core 16 -f ../data/ADJgraph/uniform-24.adj -log ../log/graphone/alg.log
#
#../build/sys/terrace/terrace_alg -gname graph500-24 -core 16 -f ../data/ADJgraph/graph500-24.adj -log ../log/terrace/alg.log
#../build/sys/terrace/terrace_sssp -gname graph500-24 -core 16 -f ../data/ADJgraph/graph500-24.adj -log ../log/terrace/alg.log
#../build/sys/livegraph/livegraph_alg -gname graph500-24 -core 16 -f ../data/ADJgraph/graph500-24.adj -log ../log/livegraph/alg.log
#../build/sys/risgraph/risgraph_alg -gname graph500-24 -core 16 -f ../data/ADJgraph/graph500-24.adj -log ../log/risgraph/alg.log
#../build/sys/stinger/stinger_alg -gname graph500-24 -core 16 -f ../data/ADJgraph/graph500-24.adj -log ../log/stinger/alg.log
#../build/sys/llama/llama_alg -gname graph500-24 -core 16 -f ../data/ADJgraph/graph500-24.adj -log ../log/llama/alg.log
#../build/sys/teseo/teseo_alg -gname graph500-24 -core 16 -f ../data/ADJgraph/graph500-24.adj -log ../log/teseo/alg.log
#../build/sys/graphone/graphone_alg -gname graph500-24 -core 16 -f ../data/ADJgraph/graph500-24.adj -log ../log/graphone/alg.log

#///

#../build/sys/terrace/terrace_edge -gname livejournal -core 1 -f ../data/ADJgraph/livejournal.adj -log ../log/terrace/edge.log
#../build/sys/terrace/terrace_edge -gname livejournal -core 4 -f ../data/ADJgraph/livejournal.adj -log ../log/terrace/edge.log
#../build/sys/terrace/terrace_edge -gname livejournal -core 8 -f ../data/ADJgraph/livejournal.adj -log ../log/terrace/edge.log
#../build/sys/terrace/terrace_edge -gname livejournal -core 12 -f ../data/ADJgraph/livejournal.adj -log ../log/terrace/edge.log
#
#../build/sys/teseo/teseo_edge -gname livejournal -core 1 -f ../data/ADJgraph/livejournal.adj -log ../log/teseo/edge.log
#../build/sys/teseo/teseo_edge -gname livejournal -core 4 -f ../data/ADJgraph/livejournal.adj -log ../log/teseo/edge.log
#../build/sys/teseo/teseo_edge -gname livejournal -core 8 -f ../data/ADJgraph/livejournal.adj -log ../log/teseo/edge.log
#../build/sys/teseo/teseo_edge -gname livejournal -core 12 -f ../data/ADJgraph/livejournal.adj -log ../log/teseo/edge.log
#
#../build/sys/aspen/aspen_edge -gname livejournal -core 1 -f ../data/ADJgraph/livejournal.adj -log ../log/aspen/edge.log
#../build/sys/aspen/aspen_edge -gname livejournal -core 4 -f ../data/ADJgraph/livejournal.adj -log ../log/aspen/edge.log
#../build/sys/aspen/aspen_edge -gname livejournal -core 8 -f ../data/ADJgraph/livejournal.adj -log ../log/aspen/edge.log
#../build/sys/aspen/aspen_edge -gname livejournal -core 12 -f ../data/ADJgraph/livejournal.adj -log ../log/aspen/edge.log




#../build/sys/aspen/aspen_alg -gname twitter -core 16 -f ../data/ADJgraph/twitter.adj -log ../log/aspen/alg.log
#../build/sys/terrace/terrace_alg -gname twitter -core 16 -f ../data/ADJgraph/twitter.adj -log ../log/terrace/alg.log
#../build/sys/terrace/terrace_sssp -gname twitter -core 16 -f ../data/ADJgraph/twitter.adj -log ../log/terrace/alg.log
#../build/sys/graphone/graphone_alg -gname twitter -core 16 -f ../data/ADJgraph/twitter.adj -log ../log/graphone/alg.log
#../build/sys/livegraph/livegraph_alg -gname twitter -core 16 -f ../data/ADJgraph/twitter.adj -log ../log/livegraph/alg.log
#../build/sys/risgraph/risgraph_alg -gname twitter -core 16 -f ../data/ADJgraph/twitter.adj -log ../log/risgraph/alg.log
#../build/sys/stinger/stinger_alg -gname twitter -core 16 -f ../data/ADJgraph/twitter.adj -log ../log/stinger/alg.log
#../build/sys/llama/llama_alg -gname twitter -core 16 -f ../data/ADJgraph/twitter.adj -log ../log/llama/alg.log
#../build/sys/teseo/teseo_alg -gname twitter -core 16 -f ../data/ADJgraph/twitter.adj -log ../log/teseo/alg.log



#../build/sys/terrace/terrace_alg -gname friendster -core 16 -f ../data/ADJgraph/friendster.adj -log ../log/terrace/alg.log
#../build/sys/terrace/terrace_sssp -gname friendster -core 16 -f ../data/ADJgraph/friendster.adj -log ../log/terrace/alg.log
##../build/sys/graphone/graphone_alg -gname friendster -core 16 -f ../data/ADJgraph/friendster.adj -log ../log/graphone/alg.log
#../build/sys/livegraph/livegraph_alg -gname friendster -core 16 -f ../data/ADJgraph/friendster.adj -log ../log/livegraph/alg.log
#../build/sys/risgraph/risgraph_alg -gname friendster -core 16 -f ../data/ADJgraph/friendster.adj -log ../log/risgraph/alg.log
#../build/sys/stinger/stinger_alg -gname friendster -core 16 -f ../data/ADJgraph/friendster.adj -log ../log/stinger/alg.log
#../build/sys/llama/llama_alg -gname friendster -core 16 -f ../data/ADJgraph/friendster.adj -log ../log/llama/alg.log
#../build/sys/teseo/teseo_alg -gname friendster -core 16 -f ../data/ADJgraph/friendster.adj -log ../log/teseo/alg.log




