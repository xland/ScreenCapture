#pragma once
#include <stdint.h>
#include <random>
#include <iostream>

namespace Util {
    int MakeRndNum() {
        std::random_device rd;
        std::mt19937 engine(rd());
        std::uniform_int_distribution<> dist(0, MAXINT);
        int random_number = dist(engine);
        return random_number;
    }
}