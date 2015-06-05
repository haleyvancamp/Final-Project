#pragma once

#include "ofMain.h"
#include "ofxLeapMotion.h"
#include "ofxStrip.h"
#include "ofxGui.h"
#include "ofxGif.h"
#include "ofxAssimpModelLoader.h"

#include "Math/MathUtils.h"
/*  Haley Van Camp
    Final Project
    06/01/2015
 
    Includes code from the ofxLeapMotion examples and the Simple 3D3 example
 */
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
    ofLight light1;
	ofMaterial m1;
    ofxFirstPersonCamera	camera;
    ofTrueTypeFont ttFont;
    ofMesh mesh;
    int h, w, xVal, yVal, zVal, index, xCam, yCam, zCam;
    float handAngle, snowDepth, temp;
    float landscape [30] [30];
    ofImage mountainPic;
    ofxGIF::fiGifLoader avalanche;
    ofxAssimpModelLoader house;
    ofxAssimpModelLoader tree;
    ofxAssimpModelLoader tree2;
	
	map <int, ofPolyline> fingerTrails;
};
