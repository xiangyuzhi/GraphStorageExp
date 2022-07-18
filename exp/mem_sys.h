//
// Created by 15743 on 2022/6/21.
//

#ifndef EXP_MEM_SYS_H
#define EXP_MEM_SYS_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

double get_memory_usage() {
    FILE*file=fopen("/proc/meminfo","r");
    if(file == NULL){
        fprintf(stderr,"cannot open /proc/meminfo\n");
        return -1;
    }
    char keyword[20];
    char valuech[20];
    double mem        =0;
    double free_mem   =0;
    fscanf(file,"MemTotal: %s kB\n",keyword);
    mem=double(atol(keyword))/(1024*1024);
    fscanf(file,"MemFree: %s kB\n",valuech);
    fscanf(file,"MemAvailable: %s kB\n",valuech);
    free_mem=double(atol(valuech))/(1024*1024);
    fclose(file);
    fprintf(stderr,"Memory %.3f GB \\ %.3f GB.\n", mem, free_mem);
    return free_mem;
}

#endif //EXP_MEM_SYS_H
