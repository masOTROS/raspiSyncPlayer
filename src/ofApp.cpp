#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	//ofSetLogLevel(OF_LOG_VERBOSE);

	// listen on the given port
	cout << "listening for osc messages on port " << PORT << "\n";
	receiver.setup(PORT);

	string videoPath = ofToDataPath("../../../../CANApps/videos/720p.mov", true);
	
	// this will just set videoPath to the first video in the videos folder, if any.
	ofDirectory currentVideoDirectory(ofToDataPath("videos", true));
	if (currentVideoDirectory.exists()) 
	{
		currentVideoDirectory.listDir();
		vector<ofFile> files = currentVideoDirectory.getFiles();
		if (files.size()>0) 
		{
			videoPath = files[0].path();
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
}

//--------------------------------------------------------------
void ofApp::update(){

	float currTime = ofGetElapsedTimef();
/*
	if(frameRateTimeout<currTime){
		cout << "FR@: " << ofGetFrameRate() << endl;
		frameRateTimeout = currTime + 2;
	}
*/
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
		// check for mouse moved message
		else if(m.getAddress() == "/play"){
			cout<< "isPlaying: " << omxPlayer.isPlaying() << endl;
			if(omxPlayer.getCurrentFrame()<0.95*(float)omxPlayer.getTotalNumFrames()){
				omxPlayer.setPaused(m.getArgAsInt32(0));
				cout << "play received, currentframe: " <<
				omxPlayer.getCurrentFrame()<< endl;
			}
			else{
				omxPlayer.restartMovie();
				cout << "Movie restarted" << endl;
			}			
		}
		else if(m.getAddress() == "/restart"){
			omxPlayer.restartMovie();
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

/*
	string buf;
	buf = "listening for osc messages on port" + ofToString(PORT);
	ofDrawBitmapString(buf, 10, 20);

	if(bplay){
		if(messageTimeout > ofGetElapsedTimef())
			ofDrawBitmapString("PLAY RECEIVED", 10, 35);
		else
			bplay = false;
	}
*/
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if(key == 'p'){  
        	cout<< "isPlaying: " << omxPlayer.isPlaying() << endl;
                if(omxPlayer.getCurrentFrame()<0.95*(float)omxPlayer.getTotalNumFrames()){
                	omxPlayer.setPaused(!omxPlayer.isPaused());
                        cout << "play received, currentframe: " <<
                        omxPlayer.getCurrentFrame()<< endl;
                }
               	else{
                       	omxPlayer.restartMovie();
                	cout << "Movie restarted" << endl;
                }                       
        }
       	else if(key == 'r'){
      		omxPlayer.restartMovie();
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
