#include "Bill.h"
#include "FileHandler.h"
#include <iostream>

using namespace std;

// Constructor: stores vehicle, exit time, and rate
Bill::Bill(Vehicle* v, time_t exit, float currentRate) {
    vehicle       = v;
    exitTime      = exit;
    ratePerMinute = currentRate;
    durationMins  = 0;
    totalAmount   = 0.0f;
}

// Calculates how long the vehicle was parked and the total charge
void Bill::calculate() {
    // Get difference in seconds between exit and entry
    double seconds = difftime(exitTime, vehicle->getEntryTime());

    // Convert to minutes
    durationMins = (int)(seconds / 60);

    // Minimum charge is 1 minute
    if (durationMins < 1) {
        durationMins = 1;
    }

    // Multiply minutes by the rate
    totalAmount = durationMins * ratePerMinute;
}

// Prints a formatted receipt to the console
void Bill::print() const {
    // Convert time_t to readable HH:MM:SS
    auto timeToStr = [](time_t t) -> string {
        struct tm* info = localtime(&t);
        char buf[9];
        snprintf(buf, sizeof(buf), "%02d:%02d:%02d",
                 info->tm_hour, info->tm_min, info->tm_sec);
        return string(buf);
    };

    cout << "\n=== PARKING RECEIPT ===" << endl;
    cout << "Entry ID    : " << vehicle->getEntryId()   << endl;
    cout << "Vehicle No. : " << vehicle->getVehicleNo() << endl;
    cout << "Owner       : " << vehicle->getOwnerName() << endl;
    cout << "Type        : " << vehicle->getType()      << endl;
    cout << "Entry Time  : " << timeToStr(vehicle->getEntryTime()) << endl;
    cout << "Exit Time   : " << timeToStr(exitTime)               << endl;
    cout << "Duration    : " << durationMins << " minutes"        << endl;
    cout << "Rate        : Rs " << ratePerMinute << " / minute"   << endl;
    cout << "TOTAL DUE   : Rs " << totalAmount                    << endl;
    cout << "=======================" << endl;
    cout << "Thank you! Drive safely." << endl;
}

// Sends this bill to FileHandler to save in history.txt
void Bill::exportToFile() const {
    FileHandler::saveHistory(const_cast<Bill*>(this));
}

// Returns duration in minutes
int Bill::getDuration() const {
    return durationMins;
}

// Returns total amount charged
float Bill::getAmount() const {
    return totalAmount;
}

// Returns the entry ID of the vehicle
string Bill::getEntryId() const {
    return vehicle->getEntryId();
}
