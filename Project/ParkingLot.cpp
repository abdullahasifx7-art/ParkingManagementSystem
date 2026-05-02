#include <iostream>
#include "ParkingLot.h"
#include "ParkingSlot.h"
#include "Vehicle.h"
using namespace std;

ParkingLot::ParkingLot(int total) {
    totalSlots = total;
    slots = new ParkingSlot * [totalSlots];
    for (int i = 0; i < totalSlots; i++) {
        // Create actual slot objects
        if (i % 5 == 0)
            slots[i] = new ParkingSlot(i, "Large");
        else if (i % 3 == 0)
            slots[i] = new ParkingSlot(i, "Motorcycle");
        else
            slots[i] = new ParkingSlot(i, "Compact");
    }
    rates[0] = 2.0f; // Car
    rates[1] = 1.0f; // Bike
    rates[2] = 3.0f; // Truck
}

ParkingLot::~ParkingLot() {
    for (int i = 0; i < totalSlots; i++) {
        if (slots[i] != nullptr) {
            Vehicle* v = slots[i]->vacate();
            if (v != nullptr) {
                delete v;
            }
            delete slots[i];
        }
    }
    delete[] slots;
}

ParkingSlot* ParkingLot::assignSlot(Vehicle* v) {
    if (v == nullptr) return nullptr;

    string vehicleType = v->getType();
    for (int i = 0; i < totalSlots; i++) {
        // Remove the nullptr check — slots now always exist
        if (slots[i]->isAvailable() && slots[i]->canAccommodate(vehicleType)) {
            if (slots[i]->park(v)) return slots[i];
        }
    }
    return nullptr;
}

bool ParkingLot::freeSlot(string entryId) {
    for (int i = 0; i < totalSlots; i++) {
        if (slots[i] != nullptr) {
            Vehicle* v = slots[i]->getParkedVehicle();
            if (v != nullptr && v->getEntryId() == entryId) {
                slots[i]->vacate();
                return true;
            }
        }
    }
    return false;
}

int ParkingLot::getAvailableCount() const {
    int count = 0;
    for (int i = 0; i < totalSlots; i++) {
        if (slots[i]->isAvailable()) {
            count++;
        }
    }
    return count;
}

void ParkingLot::displaySlotGrid() const {
    cout << endl;
    cout << " PARKING LOT GRID " << endl;
    for (int i = 0; i < totalSlots; i++) {
        if (i > 0 && i % 5 == 0) cout << endl;
        cout << "[ID:" << slots[i]->getSlotId()
            << ":" << slots[i]->getType()
            << ":" << slots[i]->getStatus() << "] ";
    }
    cout << "\n========================\n" << endl;
}

ParkingSlot* ParkingLot::getSlot(int id) const {
    for (int i = 0; i < totalSlots; i++) {
        if (slots[i] != nullptr && slots[i]->getSlotId() == id)
            return slots[i];
    }
    return nullptr;
}

int ParkingLot::getTotalSlots() const {
    return totalSlots;
}

void ParkingLot::updateRate(string type, float rate) {
    if (type == "Car") {
        rates[0] = rate;
    }
    else if (type == "Bike") {
        rates[1] = rate;
    }
    else if (type == "Truck") {
        rates[2] = rate;
    }
}

float ParkingLot::getRate(string type) const {
    if (type == "Car") {
        return rates[0];
    }
    else if (type == "Bike") {
        return rates[1];
    }
    else if (type == "Truck") {
        return rates[2];
    }
    return 0.0f;
}

ParkingSlot* ParkingLot::findVehicleByNumber(const string& vehicleNo) const {
    if (vehicleNo.empty()) {
        cout << "[ParkingLot] Error: Vehicle number cannot be empty.\n";
        return nullptr;
    }

    for (int i = 0; i < totalSlots; i++) {
        if (slots[i] != nullptr) {
            Vehicle* v = slots[i]->getParkedVehicle();
            if (v != nullptr && v->getVehicleNo() == vehicleNo) {
                return slots[i];
            }
        }
    }
    return nullptr;
}

// Adds a new slot at the end. New slot is created and available.
// Returns true on success.
bool ParkingLot::addSlot(const string& type) {
    // allocate new array with one extra slot
    ParkingSlot** newSlots = new ParkingSlot*[totalSlots + 1];
    for (int i = 0; i < totalSlots; ++i) {
        newSlots[i] = slots[i];
    }
    // new slot index will be totalSlots
    newSlots[totalSlots] = new ParkingSlot(totalSlots, type);
    // swap arrays
    delete[] slots;
    slots = newSlots;
    totalSlots += 1;
    return true;
}

// Removes a slot by id. To avoid reindexing existing ParkingSlot objects (no slotId setter),
// only allow removing the last slot. The slot must be available (or null) to be removed.
bool ParkingLot::removeSlot(int slotId) {
    if (slotId < 0 || slotId >= totalSlots) {
        cout << "[ParkingLot] Error: slotId out of range.\n";
        return false;
    }

    if (slotId != totalSlots - 1) {
        cout << "[ParkingLot] Error: only the last slot (id=" << (totalSlots - 1) << ") can be removed to avoid reindexing.\n";
        return false;
    }

    if (slots[slotId] != nullptr) {
        if (!slots[slotId]->isAvailable()) {
            cout << "[ParkingLot] Error: slot is occupied, cannot remove.\n";
            return false;
        }
        delete slots[slotId];
        slots[slotId] = nullptr;
    }

    // shrink array by one
    if (totalSlots - 1 == 0) {
        delete[] slots;
        slots = nullptr;
        totalSlots = 0;
        return true;
    }

    ParkingSlot** newSlots = new ParkingSlot*[totalSlots - 1];
    for (int i = 0; i < totalSlots - 1; ++i) {
        newSlots[i] = slots[i];
    }
    delete[] slots;
    slots = newSlots;
    totalSlots -= 1;
    return true;
}
bool ParkingLot::setSlotStatus(int slotId, string status) {
    ParkingSlot* s = getSlot(slotId);
    if (!s) return false;
    if (status != "available" && status != "maintenance") return false;
    if (status == "maintenance" && s->getStatus() == "occupied") return false;
    return s->setStatus(status);
}
ParkingSlot* ParkingLot::getSlotByIndex(int index) const {
    if (index >= 0 && index < totalSlots)
        return slots[index];
    return nullptr;
}

bool ParkingLot::setSlotStatusByIndex(int index, string status) {
    ParkingSlot* s = getSlotByIndex(index);
    if (!s) return false;
    if (status == "maintenance" && s->getStatus() == "occupied") return false;
    return s->setStatus(status);
}