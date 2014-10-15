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
	settings.enableLooping = true;		//default true

	omxPlayer.setup(settings);
	omxPlayer.setPaused(true);

	cout << omxPlayer.getTotalNumFrames() << endl;
    
    
    serverRegistered = false;
    serverIP = "";
    isReinitializing = false;
    
#ifdef USE_ARDUINO    
    serial.listDevices();
	//vector <ofSerialDeviceInfo> deviceList = serial.getDeviceList();
	
	// this should be set to whatever com port your serial device is connected to.
	// (ie, COM4 on a pc, /dev/tty.... on linux, /dev/tty... on a mac)
	// arduino users check in arduino app....
	int baud = 9600;
	serial.setup(0, baud); //open the first device
#endif
    
    //Settings
    //ofBuffer buffer = ofBufferFromFile("settings.xml");
    //name = buffer.getFirstLine();
}

//--------------------------------------------------------------
void ofApp::update(){

	float currTime = ofGetElapsedTimef();

	// Handle communication protocol with server
	while(receiver.hasWaitingMessages()){
		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage(&m);
        
        
        if(serverRegistered && serverIP == m.getRemoteIp()){
            if(m.getAddress() == "/keyPressed"){
                keyPressed(m.getArgAsInt32(0));
            }
            else if(m.getAddress() == "/shutDown"){
                ofSystem("sudo shutdown -h now");
                cout << "System shutdown" << endl;
            }
            /*
            else if(m.getAddress() == "/yourName"){
                ofxOscMessage namePost;
                namePost.setAddress("/myName");
                namePost.addStringArg(name);
                sender.sendMessage(namePost);
            }
             */
        }
        //Register server's IP and PORT to periodically send currentFrame
        if(m.getAddress() == "/registerServer"){
            
            if(serverRegistered){
                //Prevent form another server trying to take control while another one is registered
                if(serverIP == m.getRemoteIp()){
                    //Answer with "keep alive"
                    sendKeepAlive();
                    lastKeepAliveTimeStamp = currTime;
                }
            }
            else{
                //If no server is registered, register this one
                sender.setup(m.getRemoteIp(), m.getArgAsInt64(0));
                serverRegistered = true;
                sendKeepAlive();
                lastKeepAliveTimeStamp = currTime;
            }
        }
	}
    
    if(isReinitializing && omxPlayer.getCurrentFrame()==0){
        //omxPlayer.setPaused(true);
	omxPlayer.stepFrameForward();
        isReinitializing = false;
        cout << "video paused after restart" << endl;
    }
	if(!isReinitializing && omxPlayer.getCurrentFrame()>1){
		isReinitializing = true;
	}
    
    if(serverRegistered && (currTime>lastKeepAliveTimeStamp+SERVER_CONNECTION_TIMEOUT)){
        serverRegistered = false;
        serverIP = "";
    }
    /*
    //Send "keep alive" if necessary
    if(serverRegistered && (currTime>lastKeepAliveTimeStamp+KEEP_ALIVE_PERDIOD)){
        ofxOscMessage keepAlive;
        keepAlive.setAddress("/keepAlive");
        keepAlive.addStringArg(omxPlayer.getInfo());
        sender.sendMessage(keepAlive);
        lastKeepAliveTimeStamp = currTime;
    }
     */

#ifdef USE_ARDUINO    
    if(omxPlayer.isFrameNew() && serial.isInitialized()){
        serial.writeByte(omxPlayer.getCurrentFrame()%256);
    }
#endif
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
	else if(key == OF_KEY_UP){
		omxPlayer.fastForward();
	}
	else if(key == OF_KEY_DOWN){
		omxPlayer.rewind();
	}
	else if(key == ' '){
		omxPlayer.setNormalSpeed();
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

//--------------------------------------------------------------
void ofApp::sendKeepAlive(){
    
    ofxOscMessage keepAlive;
    keepAlive.setAddress("/keepAlive");
    /*
     keepAlive.addStringArg(name + "\n" +
     ofToString(ofGetFrameRate()) + "\n" +
     ofToString(omxPlayer.getWidth()) + "x" + ofToString(omxPlayer.getHeight()) + "\n" +
     ofToString(omxPlayer.getTotalNumFrames()) + "\n" +
     ofToString(omxPlayer.getCurrentFrame()) + "\n" +
     ofToString(omxPlayer.getVolume()) );
     */
    //NAME
    keepAlive.addStringArg(name);
    //FRAME RATE
    keepAlive.addStringArg(ofToString(ofGetFrameRate()));
    //DIMENSIONS
    keepAlive.addStringArg(ofToString(omxPlayer.getWidth()) + "x" + ofToString(omxPlayer.getHeight()));
    //TOTAL FRAMES
    keepAlive.addStringArg(ofToString(omxPlayer.getTotalNumFrames()));
    //CURRENT FRAME
    keepAlive.addStringArg(ofToString(omxPlayer.getCurrentFrame()));
    //VOLUME
    keepAlive.addStringArg(ofToString(omxPlayer.getVolume()));
    
    //keepAlive.addStringArg(omxPlayer.getInfo());
    sender.sendMessage(keepAlive);
}
