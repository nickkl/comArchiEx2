//
// Created by nick on 5/3/19.
//

#ifndef CACHESIM_MEMUNIT_H
#define CACHESIM_MEMUNIT_H

#include <vector>
#include <list>
#include "LRU.h"

class memUnit {
    unsigned int size = 0, ways =0, cycle = 0, writeAllocate =0, blockSize =0;
    unsigned int realRows=0, realBlock=0, realWays=0, realSize=0;
    std::vector<std::vector<std::vector<unsigned int > >> tags;
    std::vector<std::vector<bool >> dirty;
    std::vector<std::vector<bool >> valid;
    std::list<class::LRU> LRU;
    unsigned access=0;
    //std::vector<std::vector<std::vector<unsigned int > >> data;
public:
    memUnit(unsigned int size, unsigned int ways, unsigned int cyc, unsigned
    int wr, unsigned int block);
    memUnit() = default;
    bool isTagExist(unsigned long int pc);
    bool isFull();
    class::LRU findFirstEmpty();
    //bool isTagExist(unsigned long int pc,;
};


#endif //CACHESIM_MEMUNIT_H
