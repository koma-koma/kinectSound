#pragma once

#include "ofMain.h"
#include "ofxOpenNI.h"
#include "ofxOsc.h"
#include "ofxOpenCv.h"

#include "ofxMSAInteractiveObject.h"
#include "ofxSimpleGuiToo.h"
#include "ofxXmlSettings.h"

#include "soundObject.h"


class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    // openNI
    ofxOpenNI kinect;
    
    unsigned short *depthData;
    // OSC
    ofxOscSender sender;
    ofxOscReceiver receiver;
    
    string bang;

    // openCV contourFinder-----------------------
    ofxCvColorImage colorImg;
    ofxCvGrayscaleImage grayImage;
    ofxCvGrayscaleImage grayBg;
    ofxCvGrayscaleImage grayDiff;
    
    ofxCvContourFinder contourFinder;
    
    bool bLearnBakground; //背景画像を登録したか否か
    bool showCvAnalysis; //解析結果を表示するか
    int threshold; //2値化の際の敷居値
    int videoMode; //表示する画像を選択
    
    float minBlobSize; //物体の最小サイズ
    float maxBlobSize; //物体の最大サイズ
    int maxNumBlobs; //認識する物体の最大数
    bool findHoles; //穴を検出するか
    bool useApproximation; //近似値を使用するか
    //--------------------------------------------
    // GUI
    ofxSimpleGuiToo gui;

    int state;
    bool getBang; // bangを受け取ったかどうか
    int ground;
    
    int blobsSize_old;
    vector <soundObject *> object;
		
};
