//
// Created by eli on 18-12-29.
//

#ifndef ERENDER_AGGREGATE_H
#define ERENDER_AGGREGATE_H

#include "primitive.h"

class Aggregate: public Primitive{
public:
    std::vector<Primitive*> primitives;
};
#endif //ERENDER_AGGREGATE_H
