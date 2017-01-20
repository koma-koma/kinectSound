#pragma once

#include "ofMain.h"

class soundObject {
public:
    soundObject(ofPoint pos);
    ~soundObject();
    void update();
    void draw();
    void blink();
    void objectInOff();
    void setPos(ofPoint pos);
    void setColor(ofColor color);
    void setVelocity(int depth, int ground);
    void setSize(int size);
    
    ofPoint getPos();
    ofColor getColor();
    int getDepth();
    int getPan();
    int getVelocity();
    int getlength();
    int getPusle();
    int getChannel();
    int getOct();
    int getPattern();
    bool objectIn();
    
    int pan;
    int vel;
    int length;
    int pulse;
    int channel;
    int oct;
    int pattern;
    float move;
    
    ofPoint pos;
    ofPoint pos_old;
    float radius;
    ofColor color;
    int depth;
    int size;
    
private:
    
    bool bIsblinking;
    bool bObjectIn;
    int i;

};
