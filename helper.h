#pragma once
#include <GL/gl.h>
#include <climits>
#include <string>
#include <vector>
#include <algorithm>
#include <set>
#include <iostream>
#include <fstream>
#include "classes.h"

std::string text = "magnitude > 3";
std::vector<GeoPoints> geoPoints;
std::array<int, 4> canvasSize{INT_MAX, -1, INT_MAX, -1};
std::set<Earthquake> earthquakes;
bool backwardsMode = 0;
bool forwardMode = 0;
bool pauseMode = 0;
bool prevF = 0;
bool prevB = 0;
int W_SIZE_X = 600;
int W_SIZE_Y = 600;
int maxMag = 8;
int minMag = 3;
int month = 30*60*60*24;
double red = 1.0;
double green = 1.0;
double blue = 0;
double zoomlevelD = 1;
double zoomlevelS = 1;
double zoomlevelA = 1;
double zoomlevelW = 1;
double viewportDelta = 0.1;
double spanDelta = 0;
double magBorder = 3;
double markerTransform = 0.03;
uint64_t startTimer = 0;
uint64_t endTimer = ULLONG_MAX;

void zoomIn() {
    zoomlevelW += viewportDelta;
    zoomlevelA += viewportDelta;
    zoomlevelS += viewportDelta;
    zoomlevelD += viewportDelta;
}
void zoomOut() {
    zoomlevelW -= viewportDelta;
    zoomlevelA -= viewportDelta;
    zoomlevelS -= viewportDelta;
    zoomlevelD -= viewportDelta;
}

void animSet(bool forward, bool back, bool pause){
    forwardMode = forward;
    backwardsMode = back;
    pauseMode = pause;
}

std::string trim(std::string&& s) {
    auto it = s.begin();
    while(*it == ' ') {
        ++it;
    }
    auto it2 = s.end();
    while(*it2 == ' ') {
        --it2;
    }
    return std::string(it, it2);
}

std::vector<std::string> parse(const std::string& s, std::vector<std::string>& v){
    auto it = std::find(s.begin(), s.end(), ',');
    if(it != s.end())
    {
        std::string tmp = trim(std::string(s.begin(), it));
        v.push_back(tmp);
        parse(std::string(it+1, s.end()), v);
    }
    else
    {
        s.begin() = s.end();
    }
    return v;
}

bool containsAlpha(const std::string& s){
    for(auto it = s.begin(); it!= s.end(); ++it){
        if(isalpha(*it)) return true;
    }
    return false;
}

void readJapanMap() {
    std::fstream newfile;
    newfile.open("/home/odal/Projects/japan/geo.txt",std::ios::in);
    if (newfile.is_open()) {
        std::string tp;
        while(getline(newfile, tp)){
            if(containsAlpha(tp)) continue;
            std::string lat = tp.substr(0, tp.find(","));
            std::string lon = tp.substr(tp.find(" ")+1, tp.size());
            geoPoints.push_back(GeoPoints(std::stoi(lat), std::stoi(lon)));
        }
        newfile.close();
    }
    else std::cerr << "Cannot open file!\n";
}

void evalCanvas() {
    for(auto& e: geoPoints) {
        if(e.lat < canvasSize[0]) canvasSize[0] = e.lat;
        if(e.lat > canvasSize[1]) canvasSize[1] = e.lat;
        if(e.lon < canvasSize[2]) canvasSize[2] = e.lon;
        if(e.lon > canvasSize[3]) canvasSize[3] = e.lon;
    }
}

void setDefault(){
    startTimer = 0;
    endTimer = ULLONG_MAX;
    animSet(0,0,0);
}

void drawQuakePoints(const Earthquake& e) {
    double markerSize = markerTransform * e.mag;
    glColor3f(red, green * (maxMag - int(e.mag)) / (maxMag - minMag), blue);
    glVertex2f(e.lat, e.lon + markerSize);
    glVertex2f(e.lat, e.lon - markerSize);
    glVertex2f(e.lat - markerSize, e.lon);
    glVertex2f(e.lat + markerSize, e.lon);
}

void readQuakesMap() {
    std::fstream newfile;
    newfile.open("/home/odal/Projects/japan/quake.txt",std::ios::in);
    if (newfile.is_open()) {
        std::string tp;
        while(getline(newfile, tp)) {
            if(containsAlpha(tp)) continue;
            std::vector<std::string> v;
            v = parse(tp, v);
            if(v[6].size() == 0) continue;
            if(stod(v[6]) < minMag) continue;
            earthquakes.emplace(Earthquake(v[0], v[1], v[2], v[3],
                                           stod(v[4]), stod(v[5]),
                                           stod(v[6])));
        }
        newfile.close(); //close the file object.
        uint64_t span = ((--earthquakes.end())->absTime+month) - earthquakes.begin()->absTime;
        spanDelta = span / 40000.0;
    }
    else std::cerr << "Cannot open file!\n";
}

