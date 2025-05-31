#include "strategy.h"

#include <iostream>

using namespace btra::strategy;

class MyStrategy : public Strategy {
public:
    MyStrategy() { printf("MyStrategy\n"); }
};

REGISTER_STRATEGY(MyStrategy)
