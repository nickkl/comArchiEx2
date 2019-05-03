//
// Created by nick on 4/30/19.
//

#ifndef CACHESIM_CACHE_H
#define CACHESIM_CACHE_H

#include "memUnit.h"

extern double L1MissRate;
extern double L2MissRate;
extern double avgAccTime;


class cache{
    unsigned int MemCyc = 0, BSize = 0, L1Size = 0, L2Size = 0, L1Assoc = 0, L2Assoc = 0, L1Cyc = 0, L2Cyc = 0, WrAlloc = 0, VicCache = 0;
    memUnit L1;
    memUnit L2;
    memUnit victim;
    unsigned long int numberOfcommands=0, accessL1=0, accessL2 =0,
    accessVictim=0, accessMem=0;
    int hitL1=0, hitL2=0;
public:
    cache(unsigned int memCyc, unsigned int Bsize, unsigned int L1size,
          unsigned int L2size, unsigned int L1Assoc, unsigned int L2Assoc,
          unsigned int L1Cyc, unsigned int L2Cyc, unsigned int wrAlloc,
          unsigned int VicCache);
    ~cache()= default;
    void execute(unsigned long int pc, char operation);
};
#endif //CACHESIM_CACHE_H
