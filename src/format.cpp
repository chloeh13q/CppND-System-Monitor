#include <string>

#include "format.h"

using std::string;
using std::to_string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
    int hour, minute, second;
    string h, m, s;
    string output;
    hour = seconds / 3600;
    minute = (seconds - hour * 3600) / 60;
    second = seconds - (hour * 3600 + minute * 60);
    h = to_string(hour);
    m = to_string(minute);
    s = to_string(second);
    if (hour < 10) {
        h = "0" + to_string(hour);
    }
    if (minute < 10) {
        m = "0" + to_string(minute);
    }
    if (second < 10) {
        s = "0" + to_string(second);
    }
    output = h + ":" + m + ":" + s;
    return output;
}