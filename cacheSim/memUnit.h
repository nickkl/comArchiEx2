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
    std::list<class::LRU > LRU;
    unsigned access=0;
    //std::vector<std::vector<std::vector<unsigned int > >> data;
public:
    memUnit(unsigned int size, unsigned int ways, unsigned int cyc, unsigned
    int wr, unsigned int block);
    memUnit() = default;
    bool isTagExist(unsigned long int pc,class::LRU& lru);
    bool isTagExistVictim(unsigned long int pc,class::LRU& lru);
    bool isFull();
    class::LRU findFirstEmpty();
    void updateDirty(class::LRU lru, bool dirty);
    bool isDirty(class::LRU& lru);
    void updateMemory(unsigned long int pc, class::LRU& lru);
    class::LRU popLRU();
    std::vector<unsigned int> getTag(class::LRU& lru);
    class::LRU findTag(std::vector<unsigned int>);
    void updateLRU(class::LRU& lru);
    void updateValid(class::LRU& lru, bool valid);
    void updateTag(std::vector<unsigned int>, class::LRU &lru);
    void updateLRUVictim(class::LRU& lru);
    void updateMemoryVictim(unsigned long int pc, class ::LRU &lru);
    void updateRow (std::vector<unsigned int>, class::LRU& lru);
};


#endif //CACHESIM_MEMUNIT_H
