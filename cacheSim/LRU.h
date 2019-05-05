//
// Created by nick on 5/3/19.
//

#ifndef CACHESIM_LRU_H
#define CACHESIM_LRU_H


class LRU {
public:
    int way = 0;
    int row =0;

    LRU(int way, int row){
        this->way=way;
        this->row=row;
    };
    LRU() = default;
    bool operator==(const LRU& lru) const{
        return (this->row == lru.row)&&(this->way==lru.way);
    }

};


#endif //CACHESIM_LRU_H
