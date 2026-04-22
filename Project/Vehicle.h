#ifndef VEHICLE_H
#define VEHICLE_H
#include<iostream>
using namespace std;
class Vehicle {
protected:
    string vehicleNo;
    string ownerName;
    string type;
    string entryId;
    time_t entryTime;
public:
    Vehicle(string vNo, string owner, string type);
    virtual float getRate() const = 0;
    string getVehicleNo() const;
    string getOwnerName() const;
    string getType() const;
    string getEntryId() const;
    time_t getEntryTime() const;
    void generateEntryId();
    virtual ~Vehicle() {}
};

#endif // !VEHICLE_H
