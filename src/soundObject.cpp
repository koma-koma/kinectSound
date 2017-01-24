//
//  soundObject.cpp
//  kinectSound
//
//  Created by Miyashita Keita on 2016/12/12.
//
//

#include "soundObject.h"

soundObject::soundObject(ofPoint _pos) {
    pos = _pos;
    radius = 10;
    color = ofColor(0, 0, 255);
    i = 0;
    pattern = ofRandom(10);
    bObjectIn = true;
    pos_old.set(0, 0);
}

soundObject::~soundObject() {}

void soundObject::update() {
    if (bIsblinking) {
        i++;
        if (i > 20) {
            bIsblinking = false;
        }
    }
    move = (pos_old.x - pos.x)*(pos_old.x - pos.x)+(pos_old.y - pos.y)*(pos_old.y - pos.y);
    move = sqrt(move);
    if (move > 1) {
        move = 1;
    }
    pos_old = pos;
}

void soundObject::draw() {
    ofSetColor(255);
    ofDrawCircle(pos, radius);
    ofSetColor(color, 255-(255/20)*i);
    ofDrawCircle(pos, radius);
}

void soundObject::blink() {
    bIsblinking = true;
    i = 0;
}

void soundObject::objectInOff() {
    bObjectIn = false;
}

void soundObject::setPos(ofPoint _pos) {
    pos = _pos;
    pan = ofMap(pos.x, 0, 640, 0, 127);
    length = ofMap(pos.y, 0, 480, 0, 500);
    int dist = sqrt((double)((320-pos.x)*(320-pos.x))+(double)((240-pos.y)*(240-pos.y)));
    pulse = ofMap(dist, 0, 240, 4, 12);
    if (pulse > 12) {
        pulse = 12;
    }
}

void soundObject::setColor(ofColor _color) {
    color = _color;
    int max = 1;
    int max_c = 1;
    if (color.r > color.g) {
        max = color.r;
        max_c = 1;
    } else {
        max = color.g;
        max_c = 3;
    }
    if (max > color.b) {
        max_c = 4;
    }
    channel = max_c;
}

void soundObject::setVelocity(int _depth, int _ground) {
    depth = _depth;
    if (depth != 0) {
        vel = ofMap(depth, _ground, 0, 31, 96);
    }
}
void soundObject::setSize(int _size) {
    size = _size;
    oct = ofMap(size, 50, 200, 10, 2);
    if (oct < 3) {
        oct = 3;
    } else if (oct > 7) {
        oct = 7;
    }
}

ofPoint soundObject::getPos() {
    return pos;
}

ofColor soundObject::getColor() {
    return color;
}

int soundObject::getDepth() {
    return depth;
}

int soundObject::getPan() {
    return pan;
}

int soundObject::getVelocity() {
    return vel;
}

int soundObject::getlength() {
    return length;
}

int soundObject:: getPusle() {
    return pulse;
}

int soundObject:: getChannel() {
    return channel;
}

int soundObject::getOct() {
    return oct;
}

int soundObject::getPattern() {
    return pattern;
}

bool soundObject::objectIn() {
    return bObjectIn;
}
