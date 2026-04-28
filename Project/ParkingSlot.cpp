// ================================================================
//  ParkingSlot.cpp
//  Project  : Parking Lot Management System (C++ / OOP)
//  Module   : ParkingSlot — single slot model & status management
//  Member   : Abdul Wadood  |  Roll: 25L-3052
//  Section  : BSE-2A  |  Spring 2026
// ================================================================

#include "ParkingSlot.h"
#include <iostream>
#include <iomanip>
using namespace std;

// ----------------------------------------------------------------
//  Valid slot types  (used for canAccommodate logic)
//
//   Slot Type   |  Accepts
//  -------------|---------------------------
//   Compact     |  Car
//   Large       |  Car, Truck
//   Motorcycle  |  Bike
//   Electric    |  Car  (has charging point)
// ----------------------------------------------------------------


// ================================================================
//  Constructor
//  Initialises slot with given id and type.
//  Status defaults to "available", no vehicle assigned.
// ================================================================
ParkingSlot::ParkingSlot(int id, string type) {
    this->slotId = id;
    this->type = type;
    this->status = "available";
    this->parkedVehicle = nullptr;
}


// ================================================================
//  Destructor
//  NOTE: ParkingSlot does NOT own the Vehicle object.
//        Vehicles are owned by ParkingLot / main.
//        We only null the pointer — never delete it here.
// ================================================================
ParkingSlot::~ParkingSlot() {
    parkedVehicle = nullptr;
}


// ================================================================
//  isAvailable()
//  Returns true only when status is "available".
//  Always call this before canAccommodate() and park().
// ================================================================
bool ParkingSlot::isAvailable() const {
    return (status == "available");
}


// ================================================================
//  canAccommodate()
//  Checks whether this slot's type supports the given vehicle type.
//
//  Usage in ParkingLot::assignSlot():
//    if (slots[i]->isAvailable() && slots[i]->canAccommodate(v->getType()))
//        slots[i]->park(v);
// ================================================================
bool ParkingSlot::canAccommodate(const string& vehicleType) const {
    if (type == "Compact" && vehicleType == "Car") return true;
    if (type == "Large" && (vehicleType == "Car" || vehicleType == "Truck")) return true;
    if (type == "Motorcycle" && vehicleType == "Bike") return true;
    if (type == "Electric" && vehicleType == "Car") return true;
    return false;
}


// ================================================================
//  park()
//  Parks a vehicle in this slot.
//  Returns true on success, false on failure.
//
//  Caller (ParkingLot) is responsible for checking
//  isAvailable() and canAccommodate() BEFORE calling park().
//  park() still has its own guards as a safety net.
// ================================================================
bool ParkingSlot::park(Vehicle* v) {
    if (v == nullptr) {
        cout << "[ParkingSlot] Error: Cannot park a null vehicle pointer.\n";
        return false;
    }
    if (!isAvailable()) {
        cout << "[ParkingSlot] Error: Slot " << slotId
            << " is not available (current status: " << status << ").\n";
        return false;
    }
    if (!canAccommodate(v->getType())) {
        cout << "[ParkingSlot] Error: Slot " << slotId << " (" << type
            << ") cannot accommodate a " << v->getType() << ".\n";
        return false;
    }

    parkedVehicle = v;
    status = "occupied";
    cout << "[ParkingSlot] Slot " << slotId << " (" << type
        << "): Vehicle " << v->getVehicleNo() << " parked successfully.\n";
    return true;
}


// ================================================================
//  vacate()
//  Removes the vehicle from this slot and returns its pointer.
//
//  The returned pointer is used by the Bill module to
//  calculate parking charges before the vehicle is released.
//
//  Returns nullptr if the slot was already empty.
// ================================================================
Vehicle* ParkingSlot::vacate() {
    if (parkedVehicle == nullptr) {
        cout << "[ParkingSlot] Warning: Slot " << slotId << " is already empty.\n";
        return nullptr;
    }
    Vehicle* temp = parkedVehicle;
    parkedVehicle = nullptr;
    status = "available";
    cout << "[ParkingSlot] Slot " << slotId
        << ": Vehicle " << temp->getVehicleNo() << " has left. Slot is now available.\n";
    return temp;
}


// ================================================================
//  setStatus()
//  Allows Admin module to manually override slot status.
//  Valid values: "available" | "occupied" | "maintenance"
//
//  When set to "maintenance":
//    - parkedVehicle pointer is cleared (prevents dangling pointer)
//  When set to "available" or "occupied":
//    - No automatic side effects — caller manages vehicle pointer.
//
//  Returns true if status was updated, false if invalid value.
// ================================================================
bool ParkingSlot::setStatus(const string& s) {
    if (s == "available" || s == "occupied" || s == "maintenance") {
        status = s;
        if (s == "maintenance") {
            parkedVehicle = nullptr;
        }
        cout << "[ParkingSlot] Slot " << slotId << " status updated to \"" << s << "\".\n";
        return true;
    }
    cout << "[ParkingSlot] Error: Invalid status \"" << s
        << "\". Accepted values: available | occupied | maintenance.\n";
    return false;
}


// ================================================================
//  Getters
// ================================================================
int ParkingSlot::getSlotId() const { return slotId; }
string ParkingSlot::getType() const { return type; }
string ParkingSlot::getStatus() const { return status; }
Vehicle* ParkingSlot::getParkedVehicle() const { return parkedVehicle; }


// ================================================================
//  displayInfo()
//  Prints a formatted slot summary to the console.
//  Used by Admin reports and ParkingLot display functions.
// ================================================================
void ParkingSlot::displayInfo() const {
    cout << "+-------------------------------+\n";
    cout << "| Slot ID  : " << left << setw(19) << slotId << "|\n";
    cout << "| Type     : " << left << setw(19) << type << "|\n";
    cout << "| Status   : " << left << setw(19) << status << "|\n";
    if (parkedVehicle != nullptr)
        cout << "| Vehicle  : " << left << setw(19) << parkedVehicle->getVehicleNo() << "|\n";
    else
        cout << "| Vehicle  : " << left << setw(19) << "None" << "|\n";
    cout << "+-------------------------------+\n";
}