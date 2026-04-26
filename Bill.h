#ifndef BILL_H
#define BILL_H

#include <string>
#include <ctime>
#include "Vehicle.h"

using namespace std;

// Bill: calculates the parking charge when a vehicle exits.
class Bill {
private:
    Vehicle* vehicle;       // pointer to the vehicle being billed
    time_t   exitTime;      // time the vehicle left
    int      durationMins;  // how long the vehicle was parked (in minutes)
    float    ratePerMinute; // rate locked in at exit time
    float    totalAmount;   // final charge in Rs

public:
    // Constructor: takes the vehicle, exit time, and rate at exit moment
    Bill(Vehicle* v, time_t exit, float currentRate);

    // Calculates durationMins and totalAmount
    void calculate();

    // Prints a formatted receipt to the console
    void print() const;

    // Saves this bill to history file via FileHandler
    void exportToFile() const;

    // Getters
    int    getDuration() const;
    float  getAmount()   const;
    string getEntryId()  const;
};

#endif // BILL_H
