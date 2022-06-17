/*
 * ============================================================================
 *
 *         Author:  Prashant Pandey (), ppandey2@cs.cmu.edu
 *   Organization:  Carnegie Mellon University
 *
 * ============================================================================
 */

#ifndef _UTIL_H_
#define _UTIL_H_

#include <iostream>
#include <cstring>
#include <vector>
#include <cassert>
#include <fstream>
#include <unordered_map>

//#include "spdlog/spdlog.h"

#ifdef DEBUG_MODE
#define PRINT_DEBUG 1
#else
#define PRINT_DEBUG 0
#endif

#define SDSL_BITVECTOR_BLOCK_SIZE 127
//extern std::shared_ptr<spdlog::logger> console;

#define DEBUG(x) do { \
	if (PRINT_DEBUG) { std::cerr << x << std::endl; } \
} while (0)

#define ERROR(x) do { \
	{ std::cerr << x << std::endl; } \
} while (0)

#define PRINT(x) do { \
	{ std::cout << x << std::endl; } \
} while (0)

#if PRINT_DEBUG == 1
//#define dprintf(fmt, args...) fprintf(stderr, fmt, ##args)
#define dprintf(fmt, args...) /* Don't do anything in release builds */
#else
#define dprintf(fmt, args...) /* Don't do anything in release builds */
#endif

// find index of first 1-bit (least significant bit)
static inline int bsf_word(int word) {
  int result;
  __asm__ volatile("bsf %1, %0" : "=r"(result) : "r"(word));
  return result;
}

static inline int bsr_word(int word) {
  int result;
  __asm__ volatile("bsr %1, %0" : "=r"(result) : "r"(word));
  return result;
}
static inline uint32_t bsr_word(uint32_t word) {
  uint32_t result;
  __asm__ volatile("bsr %1, %0" : "=r"(result) : "r"(word));
  return result;
}
static inline uint64_t bsr_word(uint64_t word) {
  uint64_t result;
  __asm__ volatile("bsr %1, %0" : "=r"(result) : "r"(word));
  return result;
}

typedef struct _pair_int {
  uint32_t x; // length in array
  uint32_t y; // depth
} pair_int;
typedef struct _pair_uint {
  uint32_t x; // length in array
  uint32_t y; // depth
} pair_uint;

// three tuple uint
struct trip_uint {
  uint32_t x;
  uint32_t y;
  uint32_t z;
} ;


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

float cal_time_elapsed(struct timeval* start, struct timeval* end)
{
    struct timeval elapsed;
    if (start->tv_usec > end->tv_usec) {
        end->tv_usec += 1000000;
        end->tv_sec--;
    }
    elapsed.tv_usec = end->tv_usec - start->tv_usec;
    elapsed.tv_sec = end->tv_sec - start->tv_sec;
    return (elapsed.tv_sec * 1000000 + elapsed.tv_usec)/1000000.f;
}

std::vector<uint32_t> get_random_permutation(uint32_t num) {
    std::vector<uint32_t> perm(num);
    std::vector<uint32_t> vec(num);

    for (uint32_t i = 0; i < num; i++)
        vec[i] = i;

    uint32_t cnt{0};
    while (vec.size()) {
        uint32_t n = vec.size();
        srand(time(NULL));
        uint32_t idx = rand() % n;
        uint32_t val = vec[idx];
        std::swap(vec[idx], vec[n-1]);
        vec.pop_back();
        perm[cnt++] = val;
    }
    return perm;
}


double cal_time(std::vector<double> timelist){
    if(timelist.size() == 1) return timelist[0];
    sort(timelist.begin(),timelist.end());
    double st = 0.0;
    for(uint32_t i = 1 ;i < timelist.size()-1;i++)
        st += timelist[i];
    return st/double(timelist.size()-2);
}

static std::string getCurrentTime0() {
    std::time_t result = std::time(nullptr);
    std::string ret;
    ret.resize(64);
    int wsize = sprintf((char *)&ret[0], "%s", std::ctime(&result));
    ret.resize(wsize-1);
    return ret;
}

#endif
