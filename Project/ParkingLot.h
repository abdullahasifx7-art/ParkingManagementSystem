#ifndef PARKINGLOT_H
#define PARKINGLOT_H
#include<iostream>
using namespace std;
class ParkingLot {
private:
    ParkingSlot** slots;
    int           totalSlots;
    float         rates[3];    // [0]=Car  [1]=Bike  [2]=Truck
public:
    ParkingLot(int total);
    ~ParkingLot();
    ParkingSlot* assignSlot(Vehicle* v);
    bool         freeSlot(string entryId);
    int          getAvailableCount()  const;
    void         displaySlotGrid()    const;
    ParkingSlot* getSlot(int id)      const;
    int          getTotalSlots()      const;
    void         updateRate(string type, float rate);
    float        getRate(string type) const;
    ParkingSlot* findVehicleByNumber(string vehicleNo) const;
};

#endif 
