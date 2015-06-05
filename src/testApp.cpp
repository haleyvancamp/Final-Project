/*  Haley Van Camp
    Final Project
    06/01/2015
 
    Includes code from the ofxLeapMotion examples and the Simple 3D3 example
 */

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

    
    ofSetFrameRate(60); // sets framerate to 60 frames per second
    ofSetVerticalSync(true); // prevents screen tearing
	//ofSetLogLevel(OF_LOG_VERBOSE); // sets the logging level all messages are shown

    mountainPic.loadImage("mountain2.jpg"); // loads image
    avalanche.load("avalanche.gif"); // loads gif
    house.loadModel("Farmhouse.obj"); // loads house model
    tree.loadModel("Tree1-1.obj"); // loads tree model
    tree2.loadModel("Tree1-1.obj"); // loads tree model
    
    // Give us a starting point for the camera
    camera.setNearClip(0.01f); // sets near boundary for what's visible to the camera
    camera.setPosition( 0, 4, 20 ); // sets position in x, y, z space
    camera.setMovementMaxSpeed( 0.1f ); // sets max speed of camera

	leap.open(); // listens for leap
    
    ttFont.loadFont("Arial Rounded Bold.ttf", 24); // loads font

	l1.setPosition(ofGetWidth()/2, 200, 50); // sets position of light in x, y, z space
	

	
    //---------------------------------------------------------------

   
    h = 30; // initializing height for the height of the mesh
    w = 30; // initializing width for the width of the mesh
    
    
    // sets up vertices and colors of the mesh
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
    // sets static noise for the mesh by using a two deminsional array, landscape
    for(int y=0; y<h; y++) {
        for(int x=0; x<w; x++) {
            landscape [x] [y] = ofNoise(x, y, 0.2 ); // Perlin noise value
        }
    }
    

    glEnable(GL_DEPTH_TEST); // depth comparisons and updates depth buffer
    glEnable(GL_NORMALIZE); // normalized to unit length
    setNormals(mesh); // sets normals to surface
    l1.enable(); // enables light
    light1.enable(); // enables light
    l1.setDirectional(); // light is spread evenly from direction
    light1.setSpecularColor(ofColor(255,255,255)); // uses color to separate reflection components
    light1.setDiffuseColor(ofColor(255,255,255)); // diffuses reflection
    l1.setSpecularColor(ofColor(255,255,255)); // uses color to separate reflection components
    l1.setDiffuseColor(ofColor(255,255,255)); // diffuses reflection
    // setting variables to a value
    yVal = 90;
    xVal = 0;
    zVal = 0;
    light1.setPosition(ofGetWidth()/2, ofGetHeight()/2, 0); // sets position of light
    //l1.setPosition(800, 0, 0);
    
}


//--------------------------------------------------------------
void testApp::update(){
    light1.setOrientation(ofVec3f(xVal,yVal,zVal)); // sets what light is pointing at
    l1.setOrientation(ofVec3f(100, 100, 100)); // sets what light is pointing at
    //camera.setOrientation(ofPoint(-20, 0, 0));
	fingersFound.clear(); // clears previously saved fingers
    simpleHands = leap.getSimpleHands(); // sets simpleHands to the hands found with leap
    
    if( leap.isFrameNew() && simpleHands.size() ){ // makes sure frame is new
        
        leap.setMappingX(-230, 230, -ofGetWidth()/2, ofGetWidth()/2); // maps leap input to our window on the x axis
		leap.setMappingY(90, 490, -ofGetHeight()/2, ofGetHeight()/2); // maps leap input to our window on the y axis
        leap.setMappingZ(-150, 150, -200, 200); // maps leap input to our window on the x axis
    
        // looks through each hand
        for(int i = 0; i < simpleHands.size(); i++){
            
            ofVec3f position = simpleHands[i].handPos;
            ofVec3f farthestFingPos = ofVec3f(0,0,0); // setting variable to a value
            float farthestFingDis = 0; // setting a varialbe to a value
            
            //look through every finger
            for(int j = 0; j < simpleHands[i].fingers.size(); j++){
                int id = simpleHands[i].fingers[j].id;
                
                //if the distance from the hand to this finger farther than the farthestFingDis
                if(distance(position, simpleHands[i].fingers[j].pos) > farthestFingDis) {
                    // then update farhtestFingDis and farthestFingPos to that finger
                    farthestFingPos = simpleHands[i].fingers[j].pos;
                    farthestFingDis = distance(position, simpleHands[i].fingers[j].pos);
                }
                
                //store fingers seen this frame for drawing
                fingersFound.push_back(id);
            }
            
        //---------------------------------------------------------------
            // finds hand angle by using the distance between hand position and furthest finger position and the inverse of sin
            handAngle = asinf(abs(position.y - farthestFingPos.y)/farthestFingDis)*180/PI;
            // finds snow depth by the y value of the hand position and "maps" it to realistic snow depth numbers
            snowDepth = (position.y + 500)/7;
            // finds temperature by the x value of the hand position and "maps" it to realistic avalanche temperatures
            temp = (position.x +200)/10 -20;

            float time = ofGetElapsedTimef(); // time is equal to the amount of time passed
            // updates mesh and changes z value by snowdepth
            for(int y=0; y<h; y++) {
                for(int x=0; x<w; x++) {
                // vertex index
                    int i = x + w * y;
            
                    ofPoint pt = mesh.getVertex(i);
                    
                    pt.z = landscape [x] [y]*2; // assigns noise to the z value so the noise affects the z space of the mesh
                    pt.z = ofMap(landscape [x] [y], 0, 1, 0, snowDepth/10); // z space is impacted by snowdepth (y value of hand position)
            
                               
                                
                    mesh.setVertex(i, pt);
                    mesh.setColor(i, ofColor(210, 220, 235)); // sets color of mesh
                }
            }
            
            setNormals(mesh); // updates the normals for mesh
            // an avalanche only occurs when the hand angle is between 25, 45 degrees, the snow depth is greater than 20 in, and the temperature is above freezing (32)
            if((handAngle > 25) && (handAngle < 45) && (snowDepth > 20) && (temp > 32)) {
                // starts updating gif
                if(index == 0) {
                    index = 1;
                }
                // gif
                if (ofGetElapsedTimeMillis() % 3){
                    if(index > 0) {
                       index++;
                    }
                    // gif stops updating
                    if (index > avalanche.pages.size()-1) index = 0;
                }
            }
            // doesn't update gif so avalanche doesn't occur all the time
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
	ofDisableLighting(); // disables light
    
    mountainPic.draw(0, -200, 0); // draws image at a x,y location
    ofSetColor(255, 255, 255, 255); // sets color to white
    
    // draws the thermometer
    ofSetColor(255); // sets white color to back of thermometer
    ofRectRounded(20, 250, 25, 200, 90); // draws rounded rectangle
    ofCircle(55, 455, 23, 23); // draws circle
    ofSetColor(255, 0, 0); // sets color to red
    ofRectRounded(22, ofMap(temp, -50, 80, 450, 250), 21, ofMap(temp, -50, 80, 0, 200), 90); // draws rounded rectangle
    ofCircle(52, 455, 20, 20); // draws circle
    ofSetColor(255); // sets color back to white

    ofEnableDepthTest(); // begins depth buffer
    //draws avalanche gif when the index is greater than 0
    if(index > 0) {
       avalanche.pages[index].draw(0, 0);
    }
    
    house.setScale(0.01, 0.01, 0.01); // sets scale of house model
    tree.setScale(0.02, 0.02, 0.02); // sets scale of trees
    tree2.setScale(0.02, 0.02, 0.02); // sets scale of trees
    //light1.draw(); // draws the light
    //l1.draw(); // draws the light
	
    
	ofSetColor(0); // sets color to black
    // if hand angle is between 25 and 45, the font color changes to red based on hand angle
    if(handAngle < 45) {
        ofSetColor(ofMap(handAngle, 0, 45, 0, 255), 0, 0);
    }
    else {
        ofSetColor(ofMap(handAngle, 45, 90, 255, 0), 0, 0);
    }
    ttFont.drawString("Slope: " + ofToString((int)handAngle) + " degrees", 20, 46); // draws string
    ofSetColor(ofMap(snowDepth, 0, 80, 0, 255), 0, 0); // different values of red depending on snow depth
    ttFont.drawString("Snow Depth: " + ofToString((int)snowDepth) + " inches", 20, 84); // draws string
    ofSetColor(ofMap(temp, -50, 80, 0, 255), 0, 0); // different values of red depending on temp
    ttFont.drawString("Temperature: " + ofToString((int)temp) + " degrees F", 20, 122); // draws string
    

	cam.begin(); // starts camera

    ofSetColor(200); // sets color to grey
	ofEnableLighting(); // enables light
    light1.enable();
    l1.enable();
    //house.enableColors();
    //house.enableMaterials();
	m1.begin(); // starts material
	m1.setShininess(0.6); // adds shininess
    camera.begin(); // starts other camera
    
    ofPushMatrix();
    ofRotate(180-handAngle, -1, 0, 0); // rotates the house so it's on the mesh and changes with hand angle
    ofTranslate(0, 2, -2); // translates so it sits on the mesh
    house.drawFaces(); // draws the house model
    ofPopMatrix();
    
    ofPushMatrix();
    ofRotate(180-handAngle, -90, 0, 0); // rotates the trees so they're on the mesh
    ofTranslate(-5, 2, -2); // translates so they sit on the mesh
    tree.drawFaces(); // draws the tree models
    ofPopMatrix();
    
    ofPushMatrix();
    ofRotate(180-handAngle, -90, 0, 0); // rotates the trees so they're on the mesh
    ofTranslate(5, 2, -2); // translates so they sit on the mesh
    tree2.drawFaces(); // draws the tree models
    ofPopMatrix();
    
    
    ofPushMatrix();
    ofRotate(90-handAngle, -1, 0, 0); // rotates the mesh so it's on the 'floor'
    ofTranslate(0, 0, -2);
    mesh.draw(); // draws the mesh
    ofPopMatrix();
    
    camera.end(); // stops camera
    
    
    ofSetColor( ofColor::white ); // sets color to white
    ofDisableDepthTest(); // stops depth buffer

    
    for(int i = 0; i < simpleHands.size(); i++){
        simpleHands[i].debugDraw(); // draws hands
    }

   
    
    light1.disable(); // disables light
    l1.disable();
    m1.end(); // ends material
	cam.end(); // ends camera
    
    
   
	
}
//--------------------------------------------------------------
float testApp::distance(ofVec3f point1, ofVec3f point2){
    // method to calculate the distance
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
// commands to set light

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
