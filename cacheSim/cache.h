//
// Created by nick on 4/30/19.
//

#ifndef CACHESIM_CACHE_H
#define CACHESIM_CACHE_H

#include <cmath>
#include "memUnit.h"

class cache {
    unsigned int MemCyc = 0, BSize = 0, L1Size = 0, L2Size = 0, L1Assoc = 0, L2Assoc = 0, L1Cyc = 0, L2Cyc = 0, WrAlloc = 0, VicCache = 0;
    memUnit L1;
    memUnit L2;
    memUnit victim;
    unsigned long int numberOfcommands = 0, accessL1 = 0, accessL2 = 0,
            accessVictim = 0, accessMem = 0;
    int hitL1 = 0, hitL2 = 0;
    double miss1 =0, miss2 =0;
    double avg =0;
public:
    cache(unsigned int memCyc, unsigned int Bsize, unsigned int L1size,
          unsigned int L2size, unsigned int L1Assoc, unsigned int L2Assoc,
          unsigned int L1Cyc, unsigned int L2Cyc, unsigned int wrAlloc,
          unsigned int VicCache);

    ~cache() = default;

    void execute(unsigned long int pc, char operation);
    void calc(){
        accessL1 = L1.getAcc();
        accessL2 = L2.getAcc();
        accessVictim = victim.getAcc();
        this->miss1 = 1 - ((double)hitL1/accessL1);
        this->miss2 = 1 - ((double)hitL2/accessL2);
        this->avg = double((accessL1*L1Cyc)+(accessL2*L2Cyc)+accessVictim+(accessMem*MemCyc))/numberOfcommands;
    }
    double getL1(){
        return miss1;
    }
    double getL2(){
        return miss2;
    }
    double getAvg(){
        return avg;
    }
};

#endif //CACHESIM_CACHE_H
