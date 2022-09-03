#! /bin/bash


# ---- edge & alg & memory ----
for sys in "aspen" "teseo" "terrace" "teseo" "risgraph" "livegraph" "llama" "pcsr" "graphone";
do
  for data in "livejournal" "orkut" "graph500-24" "uniform-24" "twitter" "friendster";
    do
      s="../build/sys/${sys}/${sys}_edge -gname ${data} -core 16 -f ../data/ADJgraph/${data}.adj -log ../log/${sys}/edge.log"
      echo ${s}
      $s
      s="../build/sys/${sys}/${sys}_alg -gname ${data} -core 16 -f ../data/ADJgraph/${data}.adj -log ../log/${sys}/alg.log"
      echo ${s}
      $s
      s="../build/sys/teseo/teseo_mem -gname ${data} -core 16 -f ../data/ADJgraph/${data}.adj -log ../log/teseo/mem.log"
      echo ${s}
      $s
    done
done


## ---- scalability ----
for sys in "aspen" "teseo" "terrace" "teseo" "risgraph" "livegraph" "llama" "pcsr" "graphone";
do
  for data in "livejournal" "orkut" "graph500-24" "uniform-24";
  do
    s="../build/sys/${sys}/${sys}_scala -thread -gname ${data} -f ../data/ADJgraph/${data}.adj -log ../log/${sys}/scala.log"
    echo ${s}
    $s
  done
done

for sys in "aspen" "teseo" "terrace" "teseo" "risgraph" "livegraph" "llama" "pcsr" "graphone";
do
  for v in 20 21 22 23 24 25 26;
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



