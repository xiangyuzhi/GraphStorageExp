# Dynamic Graph Storage Survey

This repository provides source code and script in our experiments for reproducing
the experiment in paper, _Dynamic Graph Storage: An Experimental Survey_.

We conduct an extensive survey and evaluation of existing graph storage
systems, focusing on how the storage structures affect different
aspects of performance, e.g., update throughput, lookup efficiency,
algorithm execution time, and memory consumption.

All the experiment systems are open-source, and you can fetch them by the following
links :

**Aspen**: https://github.com/ldhulipala/aspen

**LLAMA**: https://github.com/goatdb/llama

**Teseo**: https://github.com/cwida/teseo

**Terrace**: https://github.com/PASSIONLab/terrace

**Stinger**: https://github.com/the-data-lab/GraphOne

**GraphOne**: https://github.com/the-data-lab/GraphOne

**LiveGraph**: https://github.com/thu-pacman/LiveGraph

**RisGraph**: https://github.com/thu-pacman/RisGraph

**PCSR**: https://github.com/wheatman/Packed-Compressed-Sparse-Row/

## Build

### Environment
- CMake & C++ 17
- G++ 9.4.0 & gcc 7.5.0
- OpenMP
- Clang 10

### Preconfig

```
git clone https://github.com/xiangyuzhi/GraphStorageExp.git --recursive
cd GraphStorageExp & mkdir build 
cd build
cmake ..
make
```

### Dataset

The dataset can be download form: http://konect.cc/networks/ use:

```
cd data/LDBCgraph
wget http://konect.cc/networks/{exp_data} 
```

Before run experiment, please transform data from LDBC format to CSR format by use:

```
cd data/LDBCgraph
wget http://konect.cc/networks/{exp_data} 
cd ../../build
./transfer  ../data/LDBCgraph/{exp_data}.txt  ../data/ADJgraph/{exp_data}.adj
```

### Run

The experiment for all system can be run following the script:

```
cd exp
./exp.sh
```

### Plot
The figure plot in paper can be reproduced by running the python script in 
exp/plot/exp_data_proc.ipynb

## Authors

- Xiangyu Zhi [zhixy2021@mail.sustech.edu.cn](zhixy2021@mail.sustech.edu.cn)
- Xiao Yan [yanx@sustech.edu.cn](yanx@sustech.edu.cn)
- Keming Li [likm2020@mail.sustech.edu.cn](likm2020@mail.sustech.edu.cn)
- Bo Tang [tangb3@sustech.edu.cn](tangb3@sustech.edu.cn)
- Yanchao Zhu [zhuyanchao2@huawei.com](zhuyanchao2@huawei.com)
- Minqi Zhou [zhouminqi@huawei.com](zhouminqi@huawei.com)



