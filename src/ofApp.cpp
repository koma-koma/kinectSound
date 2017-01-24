#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(0, 0, 0);
    ofEnableAlphaBlending();
    ofSetFrameRate(60);
    
    //setup kinect
    kinect.setup();
    kinect.setRegister(true);
    kinect.setMirror(true);
    kinect.addDepthGenerator();
    kinect.addImageGenerator();
    //align depth image to RGB image
    //kinect.getDepthGenerator().GetAlternativeViewPointCap().SetViewPoint(kinect.getImageGenerator());
    kinect.start();
    
    // setup OSC
    //sender.setup("192.168.3.96", 8000);
    sender.setup("192.168.3.96", 8000);
    receiver.setup(9000);
    
    //使用する画像の領域を確保
    colorImg.allocate(640,480);
    grayImage.allocate(640,480);
    grayBg.allocate(640,480);
    grayDiff.allocate(640,480);
    
    //変数の初期化
    bLearnBakground = true;
    showCvAnalysis = false;
    
    //GUIを設定
    gui.setup();
    gui.config->gridSize.x = 250;
    //gui.addContent("grayImage", grayImage); //グレーに変換した映像
    //gui.addContent("grayBg", grayBg); //キャプチャーした背景
    gui.addContent("grayDiff", grayDiff); //2値化した差分画像
    gui.addFPSCounter(); //FPS
    gui.addSlider("threshold", threshold, 0, 400); //2値化の閾値
    gui.addToggle("findHoles", findHoles); //穴を検出するか
    gui.addToggle("useApproximation", useApproximation); //近似法を使うか
    gui.addSlider("minBlobSize", minBlobSize, 0, 1); //検出する物体の最小サイズ
    gui.addSlider("maxBlobSize", maxBlobSize, 0, 1); //検出する物体の最大サイズ
    gui.addSlider("maxNumBlobs", maxNumBlobs, 1, 100); //検出する物体の最大数
    gui.loadFromXML();
    
    state = 0;
    getBang = false;
    ground = 3000;
    
    blobsSize_old = 0;
}

//--------------------------------------------------------------
void ofApp::update(){
    
    kinect.update();
    
    bool bNewFrame = false;
    bNewFrame = kinect.isNewFrame();
    //フレームが切り替わった際のみ画像を解析
    if (bNewFrame){
        colorImg.setFromPixels(kinect.getImagePixels(), 640, 480);
                               
        grayImage = colorImg;
        if (bLearnBakground == true){
            grayBg = grayImage;		// the = sign copys the pixels from grayImage into grayBg (operator overloading)
            bLearnBakground = false;
        }
        
        // take the abs value of the difference between background and incoming and then threshold:
        grayDiff.absDiff(grayBg, grayImage);
        grayDiff.threshold(threshold);
        contourFinder.findContours(grayDiff,
                                   minBlobSize * minBlobSize * grayDiff.getWidth() * grayDiff.getHeight(),
                                   maxBlobSize * maxBlobSize * grayDiff.getWidth() * grayDiff.getHeight(),
                                   maxNumBlobs, findHoles, useApproximation);
        
        depthData = kinect.getDepthRawPixels().getData();
        
    // blob no num to object no num wo soroeru
        while (contourFinder.blobs.size() != object.size()){
            if (contourFinder.blobs.size() > object.size()) {
                object.push_back(new soundObject(ofPoint(contourFinder.blobs[object.size()+1].centroid.x, contourFinder.blobs[object.size()+1].centroid.y)));
                
            } else if (contourFinder.blobs.size() < object.size()) {
                object.pop_back();
            }
        }
    //
        int i = 0;
        for(vector <soundObject *>::iterator it = object.begin(); it != object.end(); ++it){
            (*it)->update();
            (*it)->setPos(ofPoint(contourFinder.blobs[i].centroid.x, contourFinder.blobs[i].centroid.y));
            (*it)->setColor(kinect.getImagePixels().getColor(contourFinder.blobs[i].centroid.x, contourFinder.blobs[i].centroid.y));
            (*it)->setVelocity(depthData[(int)(contourFinder.blobs[i].centroid.x +  640*contourFinder.blobs[i].centroid.y)], ground);
            (*it)->setSize(contourFinder.blobs[i].boundingRect.width+contourFinder.blobs[i].boundingRect.height);
            i++;
        }
        // send object data
        ofxOscMessage pan, l, d, b, c, ch, oct, col1, col2, col3, m;
        pan.setAddress("/object/pan");
        l.setAddress("/object/length");
        d.setAddress("/object/vel");
        b.setAddress("/object/on/tgl");
        c.setAddress("/object/pulse");
        ch.setAddress("/object/channel");
        oct.setAddress("/object/oct");
        col1.setAddress("/object/col/r");
        col2.setAddress("/object/col/g");
        col3.setAddress("/object/col/b");
        m.setAddress("/object/move");



        for (int i = 0; i < object.size(); i++) {
            pan.addFloatArg(object[i]->pan);
            l.addFloatArg(object[i]->length);
            d.addIntArg(object[i]->vel);
            if (object[i]->objectIn()) {
                b.addIntArg(1);
                object[i]->objectInOff();
            } else {
                b.addIntArg(0);
            }
            c.addIntArg(object[i]->pulse);
            ch.addIntArg(object[i]->channel);
            oct.addIntArg(object[i]->oct);
            col1.addIntArg(object[i]->color.r);
            col2.addIntArg(object[i]->color.g);
            col3.addIntArg(object[i]->color.b);
            m.addFloatArg(object[i]->move);
        }
        sender.sendMessage(pan);
        sender.sendMessage(l);
        sender.sendMessage(d);
        sender.sendMessage(b);
        sender.sendMessage(c);
        sender.sendMessage(ch);
        sender.sendMessage(oct);
        sender.sendMessage(col1);
        sender.sendMessage(col2);
        sender.sendMessage(col3);
        sender.sendMessage(m);
        
    // bang wo uketaka douka handan ----------------------
        while(receiver.hasWaitingMessages()){
            ofxOscMessage m;
            bang = m.getAddress();
            receiver.getNextMessage(&m);
            if (m.getAddress() == "/bang") {
                getBang = true;
            } else if (m.getAddress() == "/module") {
                int num = m.getArgAsInt(0);
                for (int i = 0; i < object.size(); i++){
                    if ((i) == num) {
                        object[i]->blink();
                    }
                }
            }
        }
    }
    // ------------------------------------
    
    // bang wo uketatoki ---------------------------------------
    if (getBang) {
        // send message
        ofxOscMessage m;
        m.setAddress("/objectSize");
        m.addIntArg(object.size());
        sender.sendMessage(m);
        
        ofxOscMessage n;
        n.setAddress("/object/on/on");
        for (int i = 0; i < maxNumBlobs; i++) {
            if (i < object.size()) {
                n.addIntArg(1);
            } else {
                n.addIntArg(0);
            }
        }
        sender.sendMessage(n);
        getBang = false;
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    //解析結果を表示
    ofSetColor(255, 255, 255);
    kinect.drawImage(0, 0);
    contourFinder.draw(0, 0);
    //GUIを表示
    gui.draw();
    
    for(vector <soundObject *>::iterator it = object.begin(); it != object.end(); ++it){
        (*it)->draw();
    }
    
    ofSetColor(255, 255, 255);
    ofDrawBitmapString(ofToString(contourFinder.blobs.size()), 10, 10);
    ofDrawBitmapString(ofToString(object.size()), 10, 30);
    ofDrawBitmapString(ofToString(getBang), 10, 50);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch(key){
        case ' ':
            bLearnBakground = true;
            break;
            
        case 'g':
            gui.toggleDraw();
            break;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
