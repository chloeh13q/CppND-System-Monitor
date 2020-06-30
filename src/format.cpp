#include <string>

#include "format.h"

using std::string;
using std::to_string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
    int hour, minute;
    string output;
    minute = seconds / 60;
    hour = minute / 60;
    minute -= hour * 60;
    seconds -= hour * 3600 + minute * 60;
    output = to_string(hour) + ":" + to_string(minute) + ":" + to_string(seconds);
    return output;
}