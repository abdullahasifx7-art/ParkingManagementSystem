#include <iostream>
#include <string>
#include <ctime>
#include <limits>

#include "ParkingLot.h"
#include "ParkingSlot.h"
#include "FileHandler.h"
#include "Admin.h"
#include "Car.h"
#include "Bike.h"
#include "Truck.h"
#include "Bill.h"

using namespace std;

static void pause() {
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

static string readLineNonEmpty(const string& prompt) {
    string s;
    while (true) {
        cout << prompt;
        getline(cin, s);
        if (!s.empty()) return s;
        cout << "Input cannot be empty. Try again.\n";
    }
}

static ParkingSlot* findSlotByEntryId(ParkingLot* lot, const string& entryId) {
    for (int i = 0; i < lot->getTotalSlots(); ++i) {
        ParkingSlot* slot = lot->getSlot(i);
        if (!slot) continue;
        Vehicle* v = slot->getParkedVehicle();
        if (v != nullptr && v->getEntryId() == entryId) {
            return slot;
        }
    }
    return nullptr;
}

int main() {
    // Program startup: create parking lot and admin, load data
    ParkingLot* lot = new ParkingLot(50);
    Admin admin;

    FileHandler::loadRates(lot);
    FileHandler::loadSlots(lot);

    bool running = true;
    while (running) {
        cout << "\n=== PARKING MANAGEMENT ===\n";
        cout << "1. Admin\n2. User\n3. Exit\nSelect role: ";
        int role = 0;
        if (!(cin >> role)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (role == 1) {
            // Admin role
            bool backToRole = false;
            while (!backToRole) {
                cout << "\n--- ADMIN ---\n";
                cout << "1. Login\n2. View Parked Vehicles\n3. View History\n4. Generate Report (day/week/month)\n5. Update Rate\n6. Export Report\n7. Logout\n8. Back\nChoose: ";
                int aopt = 0;
                if (!(cin >> aopt)) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    continue;
                }
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                switch (aopt) {
                case 1: {
                    string u = readLineNonEmpty("Username: ");
                    string p = readLineNonEmpty("Password: ");
                    if (admin.login(u, p)) cout << "Login successful.\n";
                    else cout << "Invalid credentials.\n";
                    pause();
                    break;
                }
                case 2:
                    admin.viewParkedVehicles(lot);
                    pause();
                    break;
                case 3: {
                    string filter;
                    cout << "Enter filter (empty for all): ";
                    getline(cin, filter);
                    admin.viewHistory(filter);
                    pause();
                    break;
                }
                case 4: {
                    string period;
                    cout << "Enter period (day/week/month): ";
                    getline(cin, period);
                    admin.generateReport(period);
                    pause();
                    break;
                }
                case 5: {
                    string type;
                    cout << "Enter vehicle type (Car/Bike/Truck): ";
                    getline(cin, type);
                    float val;
                    cout << "Enter new rate (Rs per minute): ";
                    if (!(cin >> val)) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "Invalid number\n";
                        break;
                    }
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    admin.updateRate(lot, type, val);
                    cout << "Rate updated.\n";
                    pause();
                    break;
                }
                case 6:
                    admin.exportReport();
                    pause();
                    break;
                case 7:
                    admin.logout();
                    cout << "Logged out.\n";
                    pause();
                    break;
                case 8:
                    backToRole = true;
                    break;
                default:
                    cout << "Invalid option.\n";
                }
            }
        }
        else if (role == 2) {
            // User role
            bool backToRole = false;
            while (!backToRole) {
                cout << "\n--- USER ---\n";
                cout << "1. Park Vehicle\n2. Exit Vehicle (Bill)\n3. View Slot Grid\n4. Find My Vehicle (by plate)\n5. Back\nChoose: ";
                int uopt = 0;
                if (!(cin >> uopt)) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    continue;
                }
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                switch (uopt) {
                case 1: {
                    cout << "Vehicle type (Car/Bike/Truck): ";
                    string type; getline(cin, type);
                    string plate = readLineNonEmpty("Vehicle number / plate: ");
                    string owner;
                    cout << "Owner name: ";
                    getline(cin, owner);

                    Vehicle* v = nullptr;
                    if (type == "Car") v = new Car(plate, owner);
                    else if (type == "Bike") v = new Bike(plate, owner);
                    else if (type == "Truck") v = new Truck(plate, owner);
                    else {
                        cout << "Unsupported vehicle type.\n";
                        pause();
                        break;
                    }

                    v->generateEntryId();
                    ParkingSlot* s = lot->assignSlot(v);
                    if (s != nullptr) {
                        cout << "Parked. Entry ID: " << v->getEntryId()
                             << ", Slot: " << s->getSlotId() << "\n";
                    }
                    else {
                        cout << "No suitable slot available. Parking failed.\n";
                        delete v;
                    }
                    pause();
                    break;
                }
                case 2: {
                    string entryId = readLineNonEmpty("Enter Entry ID: ");
                    ParkingSlot* slot = findSlotByEntryId(lot, entryId);
                    if (!slot) {
                        cout << "Entry ID not found.\n";
                        pause();
                        break;
                    }
                    Vehicle* v = slot->getParkedVehicle();
                    if (!v) {
                        cout << "Slot empty unexpectedly.\n";
                        pause();
                        break;
                    }
                    time_t exitTime = time(nullptr);
                    float rate = lot->getRate(v->getType());
                    Bill bill(v, exitTime, rate);
                    bill.calculate();
                    bill.print();
                    bill.exportToFile();

                    // After billing, vacate slot and delete vehicle object
                    slot->vacate();
                    delete v;

                    cout << "Vehicle exited and billed.\n";
                    pause();
                    break;
                }
                case 3:
                    lot->displaySlotGrid();
                    pause();
                    break;
                case 4: {
                    string plate = readLineNonEmpty("Enter vehicle number / plate: ");
                    ParkingSlot* found = lot->findVehicleByNumber(plate);
                    if (found) {
                        Vehicle* v = found->getParkedVehicle();
                        cout << "Found at slot " << found->getSlotId()
                             << ", Entry ID: " << v->getEntryId()
                             << ", Entry Time: " << v->getEntryTime() << "\n";
                    } else {
                        cout << "Vehicle not found.\n";
                    }
                    pause();
                    break;
                }
                case 5:
                    backToRole = true;
                    break;
                default:
                    cout << "Invalid option.\n";
                }
            }
        }
        else if (role == 3) {
            // Exit program: save and cleanup
            cout << "Saving data and exiting...\n";
            FileHandler::saveSlots(lot);
            FileHandler::saveRates(lot);
            delete lot;
            running = false;
        }
    }

    return 0;
}