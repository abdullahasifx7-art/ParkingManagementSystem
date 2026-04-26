// ================================================================
//  ParkingSlot.h
//  Project  : Parking Lot Management System (C++ / OOP)
//  Module   : ParkingSlot — single slot model & status management
//  Member   : Abdul Wadood  |  Roll: 25L-3052
//  Section  : BSE-2A  |  Spring 2026
// ================================================================

#ifndef PARKINGSLOT_H
#define PARKINGSLOT_H

#include <string>
#include "Vehicle.h"

using namespace std;

class ParkingSlot {
private:
    int      slotId;         // Unique slot identifier
    string   type;           // "Compact" | "Large" | "Motorcycle" | "Electric"
    string   status;         // "available" | "occupied" | "maintenance"
    Vehicle* parkedVehicle;  // Pointer to currently parked vehicle (nullptr if empty)

public:
    // Constructor
    ParkingSlot(int id, string type);

    // Destructor
    ~ParkingSlot();

    // Core operations
    bool     park(Vehicle* v);   // Returns true if parked successfully
    Vehicle* vacate();           // Frees slot and returns vehicle pointer for billing

    // Status queries
    bool isAvailable()                       const;
    bool canAccommodate(const string& vType) const;

    // Getters
    int      getSlotId()        const;
    string   getType()          const;
    string   getStatus()        const;
    Vehicle* getParkedVehicle() const;

    // Setter (Admin use — e.g. mark as maintenance)
    bool setStatus(const string& s);   // Returns false if invalid status given

    // Display
    void displayInfo() const;
};

#endif // PARKINGSLOT_H
