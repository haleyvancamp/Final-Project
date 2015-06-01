//^{
//   <#code#>
//}
#include "testApp.h"
using namespace std;

/* Note on OS X, you must have this in the Run Script Build Phase of your project. 
where the first path ../../../addons/ofxLeapMotion/ is the path to the ofxLeapMotion addon. 

cp -f ../../../addons/ofxLeapMotion/libs/lib/osx/libLeap.dylib "$TARGET_BUILD_DIR/$PRODUCT_NAME.app/Contents/MacOS/libLeap.dylib"; install_name_tool -change ./libLeap.dylib @executable_path/libLeap.dylib "$TARGET_BUILD_DIR/$PRODUCT_NAME.app/Contents/MacOS/$PRODUCT_NAME";

   If you don't have this you'll see an error in the console: dyld: Library not loaded: @loader_path/libLeap.dylib
*/

//--------------------------------------------------------------
void testApp::setup(){

    ofSetFrameRate(60);
    ofSetVerticalSync(true);
	//ofSetLogLevel(OF_LOG_VERBOSE);

    //mountainPic.loadImage("white_mountain.jpg");
    avalanche.load("avalanche.gif");
    house.loadModel("Farmhouse.obj");
    
  
    
    // Give us a starting point for the camera
    camera.setNearClip(0.01f);
    camera.setPosition( 0, 4, 20 );
    camera.setMovementMaxSpeed( 0.1f );

	leap.open();
    
    fontSmall.loadFont("Fonts/DIN.otf", 8 );

	l1.setPosition(200, 300, 50);
	l2.setPosition(-200, -200, 50);

	
    //---------------------------------------------------------------

   
    h = 30; // initializing height for the height of the mesh
    w = 30; // initializing width for the width of the mesh
    //zoo = -100;
    
    // sets up vertices and colors of each mesh at i
    for(int y=0; y<h; y++) {
        for(int x=0; x<w; x++) {
            mesh.addVertex(ofPoint((x-w/2), (y-h/2), 0));
            mesh.addColor(ofColor(255, 255, 255));
        }
    }
    // sets up the indices of triangles to make a grid of triangles
    for(int y=0; y<h-1; y++) {
        for(int x=0; x<w-1; x++) {
            
            int index1 = x + w * y;
            int index2 = x+1 + w * y;
            int index3 = x + w * (y+1);
            int index4 = x+1 + w * (y+1);
            
            mesh.addTriangle(index1, index2, index3);
            mesh.addTriangle(index2, index4, index3);
            
        }
    }
    
    for(int y=0; y<h; y++) {
        for(int x=0; x<w; x++) {
            landscape [x] [y] = ofNoise(x, y, 0.5 ); // Perlin noise value
        }
    }
    

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_ALWAYS_SOFT_HINT_PGI);
    setNormals(mesh); // sets normals to surface
    l1.enable();
    light1.enable();
    light1.setSpecularColor(ofColor(255,255,255));
    light1.setDiffuseColor(ofColor(255,255,255));
    l1.setSpecularColor(ofColor(255,255,255));
    l1.setDiffuseColor(ofColor(255,255,255));
    yVal = 90;
    xVal = 0;
    zVal = 0;
    light1.setPosition(ofGetWidth()/2, ofGetHeight()/2, 0);
    //l1.setPosition(800, 0, 0);
    
}


//--------------------------------------------------------------
void testApp::update(){
    light1.setOrientation(ofVec3f(xVal,yVal,zVal));
    l1.setOrientation(ofVec3f(100, 100, 100));
    //camera.setOrientation(ofPoint(-20, 0, 0));
	fingersFound.clear();
    simpleHands = leap.getSimpleHands();
    
    if( leap.isFrameNew() && simpleHands.size() ){
        
        leap.setMappingX(-230, 230, -ofGetWidth()/2, ofGetWidth()/2);
		leap.setMappingY(90, 490, -ofGetHeight()/2, ofGetHeight()/2);
        leap.setMappingZ(-150, 150, -200, 200);
    
        for(int i = 0; i < simpleHands.size(); i++){
            
            ofVec3f position = simpleHands[i].handPos;
            ofVec3f farthestFingPos = ofVec3f(0,0,0);
            float farthestFingDis = 0;
            
            //look through every finger
            for(int j = 0; j < simpleHands[i].fingers.size(); j++){
                int id = simpleHands[i].fingers[j].id;
                
                //if the distance from the hand to this finger farther than the farthestFingDis
                if(distance(position, simpleHands[i].fingers[j].pos) > farthestFingDis) {
                    //if true, then update farhtestFingDis and farthestFingPos to that finger
                    farthestFingPos = simpleHands[i].fingers[j].pos;
                    farthestFingDis = distance(position, simpleHands[i].fingers[j].pos);
                }
                
                
            

               

                
                
                //if the distance between the last point and the current point is too big - lets clear the line 
                //this stops us connecting to an old drawing
                
                //add our point to our trail
                
                
                //store fingers seen this frame for drawing
                fingersFound.push_back(id);
            }
            
        //---------------------------------------------------------------
            
            handAngle = asinf(abs(position.y - farthestFingPos.y)/farthestFingDis)*180/PI;
            snowDepth = (position.y + 500)/7;
            temp = (position.x +200)/10 -20;

            float time = ofGetElapsedTimef(); // time is equal to the amount of time passed
            for(int y=0; y<h; y++) {
                for(int x=0; x<w; x++) {
                // vertex index
                    int i = x + w * y;
            
                    ofPoint pt = mesh.getVertex(i);
                    
                    pt.z = landscape [x] [y]*2; // assigns noise to the z value so the noise affects the z space of the mesh
                    pt.z = ofMap(landscape [x] [y], 0, 1, 0, snowDepth/10);
            
                               
                                
                    mesh.setVertex(i, pt);
                    mesh.setColor(i, ofColor(210, 220, 235)); // sets color of mesh
                }
            }
            
            setNormals(mesh); // updates the normals for each mesh at j
            
            if((handAngle > 25) && (handAngle < 45) && (snowDepth > 20) && (temp > 32)) {
                if(index == 0) {
                    index = 1;
                }
                if (ofGetElapsedTimeMillis() % 3){
                    if(index > 0) {
                       index++;
                    }
                    
                    if (index > avalanche.pages.size()-1) index = 0;
                }
            }
            else {
                index = 0;
            }
        }
    }
    
  

	//IMPORTANT! - tell ofxLeapMotion that the frame is no longer new. 
	leap.markFrameAsOld();	
}

//--------------------------------------------------------------
void testApp::draw(){
    //ofEnableDepthTest();
	ofDisableLighting();
    ofBackgroundGradient(ofColor(194, 192, 220), ofColor(163, 163, 169),  OF_GRADIENT_BAR);
    //mountainPic.draw(0, -200, zoo);
    ofSetColor(255, 255, 255, 255);
    
//    house.enableColors();
//    house.drawFaces();
    
    ofSetColor(255);
    ofRectRounded(20, 250, 25, 200, 90);
    ofCircle(55, 455, 23, 23);
    ofSetColor(255, 0, 0);
    ofRectRounded(22, ofMap(temp, -50, 80, 450, 250), 21, ofMap(temp, -50, 80, 0, 200), 90);
    ofCircle(52, 455, 20, 20);
    ofSetColor(255);

    ofEnableDepthTest();
    if(index > 0) {
       avalanche.pages[index].draw(0, 0);
    }
    
    //house.setPosition(ofGetWidth()/2, ofGetHeight()/2, 0);
    house.setScale(0.01, 0.01, 0.01);
    light1.draw();
    l1.draw();
	
    
	ofSetColor(0);
    if(handAngle < 45) {
        ofSetColor(ofMap(handAngle, 0, 45, 0, 255), 0, 0);
    }
    else {
        ofSetColor(ofMap(handAngle, 45, 90, 255, 0), 0, 0);
    }
	ofDrawBitmapString("Slope: " + ofToString((int)handAngle) + " degrees", 20, 20);
    ofSetColor(ofMap(snowDepth, 0, 80, 0, 255), 0, 0);
    ofDrawBitmapString("Snow Depth: " + ofToString((int)snowDepth) + " inches", 20, 40);
    ofSetColor(ofMap(temp, -50, 80, 0, 255), 0, 0);
    ofDrawBitmapString("Temperature: " + ofToString((int)temp) + " degrees F", 20, 60);
    

	cam.begin();

    ofSetColor(200);
	ofEnableLighting();
    light1.enable();
    l1.enable();
//	l2.enable();
    house.enableColors();
    house.enableMaterials();
	m1.begin();
	m1.setShininess(0.6);
    camera.begin();
    
    ofPushMatrix();
    ofRotate(180-handAngle, -1, 0, 0); // rotates the mesh so it's on the 'floor'
    ofTranslate(0, 0, -2);
    house.drawFaces();
    ofPopMatrix();
    
    ofPushMatrix();
    ofRotate(90-handAngle, -1, 0, 0); // rotates the mesh so it's on the 'floor'
    ofTranslate(0, 0, -2);
    mesh.draw(); // draws the mesh
    ofPopMatrix();
    
    camera.end();
    
    
    ofSetColor( ofColor::white );
    ofDisableDepthTest();

    
    for(int i = 0; i < simpleHands.size(); i++){
        simpleHands[i].debugDraw();
    }

    light1.disable();
    l1.disable();
    m1.end();
	cam.end();
    
   
	
}
//--------------------------------------------------------------
float testApp::distance(ofVec3f point1, ofVec3f point2){
    return sqrtf((point1.x-point2.x)*(point1.x-point2.x) + (point1.y-point2.y)*(point1.y-point2.y) +
                 (point1.z-point2.z)*(point1.z-point2.z));
}

//--------------------------------------------------------------

void testApp::setNormals( ofMesh &mesh ){
    
    //The number of the vertices
    int nV = mesh.getNumVertices();
    
    //The number of the triangles
    int nT = mesh.getNumIndices() / 3;
    
    vector<ofPoint> norm( nV ); //Array for the normals
    
    //Scan all the triangles. For each triangle add its
    //normal to norm's vectors of triangle's vertices
    for (int t=0; t<nT; t++) {
        
        //Get indices of the triangle t
        int i1 = mesh.getIndex( 3 * t );
        int i2 = mesh.getIndex( 3 * t + 1 );
        int i3 = mesh.getIndex( 3 * t + 2 );
        
        //Get vertices of the triangle
        const ofPoint &v1 = mesh.getVertex( i1 );
        const ofPoint &v2 = mesh.getVertex( i2 );
        const ofPoint &v3 = mesh.getVertex( i3 );
        
        //Compute the triangle's normal
        ofPoint dir = ( (v2 - v1).crossed( v3 - v1 ) ).normalized();
        
        //Accumulate it to norm array for i1, i2, i3
        norm[ i1 ] += dir;
        norm[ i2 ] += dir;
        norm[ i3 ] += dir;
    }
    
    //Normalize the normal's length
    for (int i=0; i<nV; i++) {
        norm[i].normalize();
    }
    
    //Set the normals to mesh
    mesh.clearNormals();
    mesh.addNormals( norm );
}
//--------------------------------------------------------------
void testApp::keyPressed(int key){
//    if(key == ' ') {
//        camera.setPosition(ofPoint(xCam, yCam, zCam));
//        if(key == OF_KEY_RIGHT) {
//            xCam += 10;
//        }
//        if(key == OF_KEY_LEFT) {
//            xCam -= 10;
//        }
//        if(key == OF_KEY_UP) {
//            yCam += 10;
//        }
//        if(key == OF_KEY_DOWN) {
//            yCam -= 10;
//        }
//        if(key == 'o') {
//            zCam += 10;
//        }
//        if(key == 'i') {
//            zCam -= 10;
//        }
//    }
 //    if(key == ' ') {
//        zoo--;
//    }
//
//      if(key == 't')
//      {
//          yVal+=10;
//      }
//    if(key == 'g')
//    {
//        yVal-=10;
//    }
//    if(key == 'y')
//    {
//        xVal+=10;
//    }
//    if(key == 'r')
//    {
//        xVal-=10;
//    }
//    if(key == 'u')
//    {
//        zVal+=10;
//    }
//    if(key == 'j')
//    {
//        zVal-=10;
//    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){
    
}

//--------------------------------------------------------------
void testApp::exit(){
    // let's close down Leap and kill the controller
    leap.close();
}
