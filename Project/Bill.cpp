#include "Bill.h"
#include "FileHandler.h"
#include <iostream>
#include <ctime>
using namespace std;

// Constructor: stores vehicle, exit time, and rate
Bill::Bill(Vehicle* v, time_t exit, float currentRate) {
    vehicle = v;
    exitTime = exit;
    ratePerMinute = currentRate;
    durationMins = 0;
    totalAmount = 0.0f;
}

// Calculates how long the vehicle was parked and the total charge
void Bill::calculate() {
    double seconds = difftime(exitTime, vehicle->getEntryTime());
    durationMins = static_cast<int>(seconds / 60);
    if (durationMins < 1) durationMins = 1;
    totalAmount = durationMins * ratePerMinute;
}

// Prints a formatted receipt to the console
void Bill::print() const {
    auto timeToStr = [](time_t t) -> string {
        struct tm buf;
#ifdef _WIN32
        localtime_s(&buf, &t);
#else
        struct tm* tmPtr = localtime(&t);
        if (tmPtr) buf = *tmPtr;
#endif
        char out[9];
        snprintf(out, sizeof(out), "%02d:%02d:%02d", buf.tm_hour, buf.tm_min, buf.tm_sec);
        return string(out);
    };

    cout << "\n=== PARKING RECEIPT ===" << endl;
    cout << "Entry ID    : " << vehicle->getEntryId() << endl;
    cout << "Vehicle No. : " << vehicle->getVehicleNo() << endl;
    cout << "Owner       : " << vehicle->getOwnerName() << endl;
    cout << "Type        : " << vehicle->getType() << endl;
    cout << "Entry Time  : " << timeToStr(vehicle->getEntryTime()) << endl;
    cout << "Exit Time   : " << timeToStr(exitTime) << endl;
    cout << "Duration    : " << durationMins << " minutes" << endl;
    cout << "Rate        : Rs " << ratePerMinute << " / minute" << endl;
    cout << "TOTAL DUE   : Rs " << totalAmount << endl;
    cout << "=======================" << endl;
    cout << "Thank you! Drive safely." << endl;
}

void Bill::exportToFile() const {
    FileHandler::saveHistory(const_cast<Bill*>(this));
}

int Bill::getDuration() const { return durationMins; }
float Bill::getAmount() const { return totalAmount; }
string Bill::getEntryId() const { return vehicle->getEntryId(); }
string Bill::getVehicleNo()   const { return vehicle->getVehicleNo(); }
string Bill::getOwnerName()   const { return vehicle->getOwnerName(); }
string Bill::getVehicleType() const { return vehicle->getType(); }
time_t Bill::getEntryTime()   const { return vehicle->getEntryTime(); }
time_t Bill::getExitTime()    const { return exitTime; }
