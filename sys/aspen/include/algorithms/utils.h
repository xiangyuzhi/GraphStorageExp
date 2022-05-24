//
// Created by zxy on 5/4/22.
//

#ifndef EXP_UTILS_H
#define EXP_UTILS_H

#include<iostream>

void print_time_elapsed(std::string desc, struct timeval* start, struct
        timeval* end)
{
    struct timeval elapsed;
    if (start->tv_usec > end->tv_usec) {
        end->tv_usec += 1000000;
        end->tv_sec--;
    }
    elapsed.tv_usec = end->tv_usec - start->tv_usec;
    elapsed.tv_sec = end->tv_sec - start->tv_sec;
    float time_elapsed = (elapsed.tv_sec * 1000000 + elapsed.tv_usec)/1000000.f;
    std::cout << desc << "Total Time Elapsed: " << std::to_string(time_elapsed) <<
              "seconds" << std::endl;
}




#endif //EXP_UTILS_H
