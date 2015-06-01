#pragma once

#include "ofMain.h"
#include "ofxLeapMotion.h"
#include "ofxStrip.h"
#include "ofxGui.h"
#include "ofxGif.h"
#include "ofxAssimpModelLoader.h"

#include "Math/MathUtils.h"

#include "Utils/ofTrueTypeFontExt.h"
#include "Utils/Cameras/ofxWalkingFirstPersonCamera.h"

class testApp : public ofBaseApp{

  public:
    void setup();
    void update();
    void draw();
	
    void keyPressed  (int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    void exit();
    void setNormals(ofMesh &mesh);
    float distance(ofVec3f point1, ofVec3f point2);
		
	ofxLeapMotion leap;
	vector <ofxLeapMotionSimpleHand> simpleHands;
    
	vector <int> fingersFound; 
	ofEasyCam cam;
	ofLight l1;
	ofLight l2;
    ofLight light1;
	ofMaterial m1;
    ofxFirstPersonCamera	camera;
    
    ofTrueTypeFontExt		fontSmall;
    
    ofMesh mesh;
    
    int h, w, xVal, yVal, zVal, zoo, index, xCam, yCam, zCam;
    float handAngle, snowDepth, temp;
    
    float landscape [30] [30];
    
    
    
    ofImage mountainPic;
    
    ofxGIF::fiGifLoader avalanche;
    ofxAssimpModelLoader house;
	
	map <int, ofPolyline> fingerTrails;
};
