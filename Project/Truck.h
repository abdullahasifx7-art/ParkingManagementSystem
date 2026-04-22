#ifndef TRUCK_H
#define TRUCK_H
#include<iostream>
using namespace std;
class Truck : public Vehicle {
public:
    Truck(string vNo, string owner);
    float getRate() const override;
};

#endif 
