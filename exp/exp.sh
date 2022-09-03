#! /bin/bash


# ---- edge ----
#for sys in "pcsr" ;
##"graph500-24" "livejournal" "twitter"
#do
#  for data in "livejournal" "graph500-24";
#    do
#      s="../build/sys/${sys}/${sys}_edge -gname ${data} -core 16 -f ../data/ADJgraph/${data}.adj -log ../log/${sys}/edge.log"
#      echo ${s}
#      $s
#    done
#done
#
#for sys in "stinger" "livegraph";
##"graph500-24" "livejournal" "twitter"
#do
#  for data in "twitter";
#    do
#      s="../build/sys/${sys}/${sys}_edge -gname ${data} -core 16 -f ../data/ADJgraph/${data}.adj -log ../log/${sys}/edge.log"
#      echo ${s}
#      $s
#    done
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
#for sys in "llama";
#do
#  for data in "graph500-24";
#  do
#    s="../build/sys/${sys}/${sys}_alg -gname ${data} -core 16 -f ../data/ADJgraph/${data}.adj -log ../log/${sys}/alg.log"
#    echo ${s}
#    $s
#  done
#done

#
## ---- scala ----
#for sys in "livegraph" "llama" "pcsr";
#do
#  for data in "livejournal" "graph500-24";
#  do
#    s="../build/sys/${sys}/${sys}_scala -thread -gname ${data}  -f ../data/ADJgraph/${data}.adj -log ../log/${sys}/scala.log"
#    echo ${s}
#    $s
#  done
#done
#
#for sys in "graphone";
#do
#  for data in "graph500-24";
#  do
#    for thd in 1 4 8 12 16;
#    do
#      s="../build/sys/${sys}/${sys}_scala -thread -core ${thd} -gname ${data} -f ../data/ADJgraph/${data}.adj -log ../log/${sys}/scala.log"
#      echo ${s}
#      $s
#    done
#  done
#done


#  ----   alg ----
for sys in "teseo";
do
  for data in "friendster";
  do
    s="../build/sys/${sys}/${sys}_alg -gname ${data} -core 16 -f ../data/ADJgraph/${data}.adj -log ../log/${sys}/alg.log"
    echo ${s}
    $s
  done
done

for sys in "pcsr" "llama";
#
do
  for data in "twitter";
  do
    s="../build/sys/${sys}/${sys}_alg -gname ${data} -core 16 -f ../data/ADJgraph/${data}.adj -log ../log/${sys}/alg.log"
    echo ${s}
    $s
  done
done


# ---- edge ----
for sys in "pcsr" "graphone";
#"graph500-24" "livejournal" "twitter"
do
  for data in "twitter";
    do
      s="../build/sys/${sys}/${sys}_edge -gname ${data} -core 16 -f ../data/ADJgraph/${data}.adj -log ../log/${sys}/edge.log"
      echo ${s}
      $s
    done
done


for sys in "llama";
#
do
  for data in "friendster";
  do
    s="../build/sys/${sys}/${sys}_alg -gname ${data} -core 16 -f ../data/ADJgraph/${data}.adj -log ../log/${sys}/alg.log"
    echo ${s}
    $s
  done
done




#for sys in "aspen";
#do
#  for data in "orkut";
#  do
#    s="../build/sys/${sys}/${sys}_scala -thread -gname ${data} -core 16 -f ../data/ADJgraph/${data}.adj -log ../log/${sys}/scala.log"
#    echo ${s}
#    $s
#  done
##  s="../build/sys/${sys}/${sys}_scala -log ../log/${sys}/scala.log"
##  echo ${s}
##  $s
#done


#for sys in "ligra";
#do
#  for v in 20 21 22 23 24 25 26;
#  do
#    s="../build/sys/${sys}/${sys}_scala -v ${v} -e 30 -log ../log/${sys}/scala.log"
#    echo ${s}
#    $s
#  done
#  for e in 10 20 30 40 50 60 70;
#  do
#    s="../build/sys/${sys}/${sys}_scala -v 23 -e ${e} -log ../log/${sys}/scala.log"
#    echo ${s}
#    $s
#  done
#done


#for sys in "teseo";
#do
#  for data in "orkut" "uniform-24";
#  do
#    for thd in 1 4 8 12 16;
#    do
#      s="../build/sys/${sys}/${sys}_scala -thread -core ${thd} -gname ${data} -f ../data/ADJgraph/${data}.adj -log ../log/${sys}/scala.log"
#      echo ${s}
#      $s
#    done
#  done
#done


#for thd in 1 4 8 12 16;
#do
#  for data in "orkut" "uniform-24"
#  do
#      s="../build/sys/graphone/graphone_scala -gname ${data} -thread -core ${thd} -f ../data/ADJgraph/${data}.adj -log ../log/graphone/scala.log"
#      echo ${s}
#      $s
#  done
#done


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

