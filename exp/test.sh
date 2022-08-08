#! /bin/bash

data=("uniform-24" "orkut")
sys=("teseo")

for ((j=0;j<1;j++))
do
  for ((i=0;i<2;i++))
  do
    echo ${data[i]} ${sys[j]}
    s="../build/sys/${sys[j]}/${sys[j]}_edge  -gname ${data[i]} -core 12 -f ../data/ADJgraph/${data[i]}.adj -log ../log/${sys[j]}/edge.log"
    echo ${s}
    $s
  done
done



sys=("terrace"  "graphone" "livegraph" "risgraph" "stinger" "llama" "pcsr")
#  "teseo" "aspen"
data=("orkut" "uniform-24")
# "livejournal" "graph500-24"
for ((i=0;i<2;i++))
do
  for ((j=0;j<7;j++))
  do
    echo ${data[i]} ${sys[j]}
    s="../build/sys/${sys[j]}/${sys[j]}_edge -gname ${data[i]} -core 16 -f ../data/ADJgraph/${data[i]}.adj -log ../log/${sys[j]}/edge.log"
    echo ${s}
    $s
  done
done




#sys2=("risgraph" "stinger" "llama" "ligra" "pcsr")
#data2="graph500-24"
#
#for ((j=0;j<8;j++))
#  do
#    echo ${data2} ${sys2[j]}
#    s="../build/sys/${sys2[j]}/${sys2[j]}_mem -gname ${data2} -core 16 -f ../data/ADJgraph/${data2}.adj -log ../log/${sys2[j]}/mem.log"
#    echo ${s}
#    $s
#  done

#sys=("terrace" "graphone" "livegraph" "risgraph" "stinger" "llama" "ligra" "pcsr")
##  "teseo" "aspen"
#data=("twitter" "friendster")
## "livejournal" "graph500-24"
#for ((i=0;i<2;i++))
#do
#  for ((j=0;j<8;j++))
#  do
#    echo ${data[i]} ${sys[j]}
#    s="../build/sys/${sys[j]}/${sys[j]}_mem -gname ${data[i]} -core 16 -f ../data/ADJgraph/${data[i]}.adj -log ../log/${sys[j]}/mem.log"
#    echo ${s}
#    $s
#  done
#done