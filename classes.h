#pragma once
#include <algorithm>
#include <iostream>
#include <string>

struct GeoPoints{
    int lon;
    int lat;
    GeoPoints(int f, int s): lon(f), lat(s) {};
    GeoPoints(GeoPoints&& g): lon(g.lon), lat(g.lat) {};
};

struct Earthquake{
    std::string year;
    std::string month;
    std::string day;
    std::string utc;
    uint64_t absTime;
    double lon;
    double lat;
    double mag;
    Earthquake(std::string y, std::string mo,
               std::string d, std::string t,
               double lo, double la, double m)
        : year(y), month(mo), day(d),
        utc(t), lon(lo), lat(la), mag(m) {
        absTime = stoi(t) % 100; // seconds
        absTime += 60 * (((stoi(t) % 10000) / 100)-1);  // minutes
        absTime += 60 * 60 * ((stoi(t) / 10000)-1); // hours;
        absTime += 24 * 60 * 60 * (stoi(day)-1);
        absTime += 30 * 24 * 60 * 60 * (stoi(month)-1);
        uint64_t tmp = 12 * 30 * 24 * 60 * 60;
        absTime += tmp * (stoi(year)-1973);

    };
    Earthquake(Earthquake&& e)
        : year(e.year), month(e.month), day(e.day), utc(e.utc),
        absTime(e.absTime), lon(e.lon), lat(e.lat), mag(e.mag) {};
};

bool operator<(const Earthquake& a, const Earthquake& b) {
    return a.absTime < b.absTime;
}
