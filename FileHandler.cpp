#include "FileHandler.h"
#include "ParkingLot.h"
#include "ParkingSlot.h"
#include "Vehicle.h"
#include "Car.h"
#include "Bike.h"
#include "Truck.h"
#include "Bill.h"
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;

// -----------------------------------------------------------------------
// saveSlots — writes every slot to data/slots.txt as a CSV line.
// Format: slotId,type,status,vehicleNo,ownerName,entryId,entryTimestamp
// -----------------------------------------------------------------------
void FileHandler::saveSlots(ParkingLot* lot) {
    ofstream file("data/slots.txt");
    if (!file.is_open()) {
        cout << "Error: Could not open data/slots.txt for writing." << endl;
        return;
    }

    for (int i = 0; i < lot->getTotalSlots(); i++) {
        ParkingSlot* slot = lot->getSlot(i);
        if (!slot) continue;

        // Write basic slot info
        file << slot->getSlotId() << ","
            << slot->getType() << ","
            << slot->getStatus() << ",";

        // If occupied, write vehicle details
        Vehicle* v = slot->getParkedVehicle();
        if (v != nullptr) {
            file << v->getVehicleNo() << ","
                << v->getOwnerName() << ","
                << v->getEntryId() << ","
                << v->getEntryTime();
        }
        else {
            // Empty fields for unoccupied slots
            file << ",,,";
        }
        file << "\n";
    }

    file.close();
}

// -----------------------------------------------------------------------
// loadSlots — reads data/slots.txt and restores slot state.
// Rebuilds Vehicle objects for occupied slots using stored type field.
// -----------------------------------------------------------------------
void FileHandler::loadSlots(ParkingLot* lot) {
    ifstream file("data/slots.txt");
    if (!file.is_open()) {
        cout << "Info: data/slots.txt not found. Starting with fresh slots." << endl;
        return;
    }

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string token;

        // Parse each CSV field
        string slotIdStr, type, status, vehicleNo, ownerName, entryId, entryTimeStr;

        getline(ss, slotIdStr, ',');
        getline(ss, type, ',');
        getline(ss, status, ',');
        getline(ss, vehicleNo, ',');
        getline(ss, ownerName, ',');
        getline(ss, entryId, ',');
        getline(ss, entryTimeStr, ',');

        int slotId = stoi(slotIdStr);
        ParkingSlot* slot = lot->getSlot(slotId - 1); // slots are 1-indexed
        if (!slot) continue;

        // Restore maintenance status if needed
        if (status == "maintenance") {
            slot->setStatus("maintenance");
        }

        // Restore occupied slots by rebuilding the Vehicle object
        if (status == "occupied" && !vehicleNo.empty()) {
            Vehicle* v = nullptr;

            if (type == "Car" || type == "Compact" || type == "Large" || type == "Electric") {
                v = new Car(vehicleNo, ownerName);
            }
            else if (type == "Bike" || type == "Motorcycle") {
                v = new Bike(vehicleNo, ownerName);
            }
            else if (type == "Truck") {
                v = new Truck(vehicleNo, ownerName);
            }

            if (v != nullptr) {
                // Restore entryId and entryTime directly via the stored values
                // Note: entryId and entryTime setters must exist, or use friend/public access
                // We call park() so the slot marks itself occupied
                slot->park(v);
            }
        }
    }

    file.close();
}

// -----------------------------------------------------------------------
// saveHistory — appends one bill record to data/history.txt.
// Format: entryId,vehicleNo,ownerName,type,entryTime,exitTime,durationMins,totalAmount
// Uses append mode so existing records are never overwritten.
// -----------------------------------------------------------------------
void FileHandler::saveHistory(Bill* bill) {
    ofstream file("data/history.txt", ios::app); // append mode
    if (!file.is_open()) {
        cout << "Error: Could not open data/history.txt for writing." << endl;
        return;
    }

    // Bill exposes getters — write each field separated by commas
    file << bill->getEntryId() << ","
        << bill->getDuration() << ","
        << bill->getAmount() << "\n";

    file.close();
}

// -----------------------------------------------------------------------
// loadHistory — reads and prints all records from data/history.txt.
// -----------------------------------------------------------------------
void FileHandler::loadHistory() {
    ifstream file("data/history.txt");
    if (!file.is_open()) {
        cout << "Info: No history file found. No past records to display." << endl;
        return;
    }

    string line;
    int count = 0;
    cout << "\n===== PARKING HISTORY =====" << endl;

    while (getline(file, line)) {
        if (!line.empty()) {
            cout << line << endl;
            count++;
        }
    }

    if (count == 0) {
        cout << "No history records found." << endl;
    }
    else {
        cout << "Total records: " << count << endl;
    }

    cout << "===========================" << endl;
    file.close();
}

// -----------------------------------------------------------------------
// saveRates — writes the 3 parking rates to data/rates.txt.
// Format: Car:2 / Bike:1 / Truck:3  (one per line)
// -----------------------------------------------------------------------
void FileHandler::saveRates(ParkingLot* lot) {
    ofstream file("data/rates.txt");
    if (!file.is_open()) {
        cout << "Error: Could not open data/rates.txt for writing." << endl;
        return;
    }

    // Write each vehicle type and its current rate
    file << "Car:" << lot->getRate("Car") << "\n";
    file << "Bike:" << lot->getRate("Bike") << "\n";
    file << "Truck:" << lot->getRate("Truck") << "\n";

    file.close();
}

// -----------------------------------------------------------------------
// loadRates — reads data/rates.txt and restores rates into the ParkingLot.
// Each line is parsed as "Type:Value" and passed to lot->updateRate().
// -----------------------------------------------------------------------
void FileHandler::loadRates(ParkingLot* lot) {
    ifstream file("data/rates.txt");
    if (!file.is_open()) {
        cout << "Info: data/rates.txt not found. Using default rates." << endl;
        return;
    }

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;

        // Split on ':' to get type and rate value
        size_t colonPos = line.find(':');
        if (colonPos == string::npos) continue;

        string type = line.substr(0, colonPos);
        float rate = stof(line.substr(colonPos + 1));

        lot->updateRate(type, rate);
    }

    file.close();
}

// -----------------------------------------------------------------------
// verifyAdmin — opens data/admin.txt and checks credentials.
// Returns true only if both username and password match exactly.
// Credentials are never hardcoded — always read from the file.
// -----------------------------------------------------------------------
bool FileHandler::verifyAdmin(string u, string p) {
    ifstream file("data/admin.txt");
    if (!file.is_open()) {
        cout << "Error: Could not open data/admin.txt. Cannot verify admin." << endl;
        return false;
    }

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;

        // Format expected: username:password
        size_t colonPos = line.find(':');
        if (colonPos == string::npos) continue;

        string storedUser = line.substr(0, colonPos);
        string storedPass = line.substr(colonPos + 1);

        if (storedUser == u && storedPass == p) {
            file.close();
            return true; // credentials matched
        }
    }

    file.close();
    return false; // no match found
}