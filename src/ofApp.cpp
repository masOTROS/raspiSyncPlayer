#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	//ofSetLogLevel(OF_LOG_VERBOSE);

	// listen on the given port
	cout << "listening for osc messages on port " << PORT << "\n";
	receiver.setup(PORT);

	videoPath = ofToDataPath("../../../../CANApps/videos/720p.mov", true);
	
	// this will just set videoPath to the first video in the videos folder, if any.
	ofDirectory currentVideoDirectory(ofToDataPath("videos", true));
	if (currentVideoDirectory.exists()) 
	{
		currentVideoDirectory.listDir();
		vector<ofFile> files = currentVideoDirectory.getFiles();
		if (files.size()>0) 
		{
			videoPath = files[0].path();
            name = files[0].getFileName();
		}		
	}
	
	//Somewhat like ofFboSettings we may have a lot of options so this is the current model
	ofxOMXPlayerSettings settings;
	settings.videoPath = videoPath;
	settings.useHDMIForAudio = true;	//default true
	settings.enableTexture = true;		//default true
	settings.enableLooping = false;		//default true

	omxPlayer.setup(settings);
	omxPlayer.setPaused(true);

	cout << omxPlayer.getTotalNumFrames() << endl;
    
    serverRegistered = false;
    
    
    //Settings
    //ofBuffer buffer = ofBufferFromFile("settings.xml");
    //name = buffer.getFirstLine();
    
    bDebug = false;
}

//--------------------------------------------------------------
void ofApp::update(){

	float currTime = ofGetElapsedTimef();

	// check for waiting messages
	while(receiver.hasWaitingMessages()){
		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage(&m);
        
        //Register server's IP and PORT to periodically send currentFrame
        if(m.getAddress() == "/registerServer"){
            sender.setup(m.getRemoteIp(), m.getArgAsInt64(0));
            serverRegistered = true;
            
            ofxOscMessage keepAlive;
            keepAlive.setAddress("/keepAlive");
            keepAlive.addInt64Arg(omxPlayer.getCurrentFrame());
            sender.sendMessage(keepAlive);
            lastKeepAliveTimeStamp = currTime;
        }
        else if(m.getAddress() == "/yourName"){
            ofxOscMessage namePost;
            namePost.setAddress("/myName");
            namePost.addStringArg(name);
            sender.sendMessage(namePost);
        }
		// check for keyboard input message
		else if(m.getAddress() == "/keyPressed"){
            keyPressed(m.getArgAsInt32(0));
		}
	}
    
    if(serverRegistered && (currTime>lastKeepAliveTimeStamp+KEEP_ALIVE_PERDIOD)){
        ofxOscMessage keepAlive;
        keepAlive.setAddress("/keepAlive");
        keepAlive.addInt64Arg(omxPlayer.getCurrentFrame());
        sender.sendMessage(keepAlive);
        lastKeepAliveTimeStamp = currTime;
    }
}


//--------------------------------------------------------------
void ofApp::draw(){
    
    if(bDebug){
        omxPlayer.draw(0,0);
        ofDrawBitmapStringHighlight(omxPlayer.getInfo(), 60, 60, ofColor(ofColor::black, 90), ofColor::yellow);
    }
	
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if(key == 'p'){
        if(omxPlayer.getCurrentFrame()<0.95*(float)omxPlayer.getTotalNumFrames()){
            omxPlayer.setPaused(!omxPlayer.isPaused());
            cout << "play received, currentframe: " << omxPlayer.getCurrentFrame() << endl;
        }
        else{
            omxPlayer.restartMovie();
            cout << "Movie restarted" << endl;
        }                       
    }
    else if(key == 'r'){
        omxPlayer.restartMovie();
    }
    else if(key == 'd'){
        bDebug = !bDebug;
        
        ofxOMXPlayerSettings settings;
        settings.videoPath = videoPath;
        settings.useHDMIForAudio = true;	//default true
        settings.enableTexture = bDebug;		//default true
        settings.enableLooping = false;		//default true
        
        omxPlayer.setup(settings);
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

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
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}
