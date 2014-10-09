#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	//ofSetLogLevel(OF_LOG_VERBOSE);
    
    ofHideCursor();

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
	settings.enableTexture = false;		//default true
	settings.enableLooping = false;		//default true

	omxPlayer.setup(settings);
	omxPlayer.setPaused(true);

	cout << omxPlayer.getTotalNumFrames() << endl;
    
    serverRegistered = false;

isReinitializing = false;
    
    
    //Settings
    //ofBuffer buffer = ofBufferFromFile("settings.xml");
    //name = buffer.getFirstLine();
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
            keepAlive.addStringArg(omxPlayer.getInfo());
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
    
    if(isReinitializing && omxPlayer.getCurrentFrame()>0 && omxPlayer.getCurrentFrame()<10){
        omxPlayer.setPaused(true);
	isReinitializing = false;
	cout << "video paused after restart" << endl;
    }
    
    //Send "keep alive" if necessary
    if(serverRegistered && (currTime>lastKeepAliveTimeStamp+KEEP_ALIVE_PERDIOD)){
        ofxOscMessage keepAlive;
        keepAlive.setAddress("/keepAlive");
        keepAlive.addStringArg(omxPlayer.getInfo());
        sender.sendMessage(keepAlive);
        lastKeepAliveTimeStamp = currTime;
    }
}


//--------------------------------------------------------------
void ofApp::draw(){
    
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
            //omxPlayer.setPaused(true);
            isReinitializing = true;
            cout << "Movie restarted" << endl;
        }                       
    }
    else if(key == 'r'){
        omxPlayer.restartMovie();
        //omxPlayer.setPaused(true);
        isReinitializing = true;
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
