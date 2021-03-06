#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxOMXPlayer.h"

#define USE_ARDUINO

// listen on port 12345
#define PORT 12345

#define SERVER_CONNECTION_TIMEOUT  2 //time delay from consecutive KEEP_ALIVE messages to server

class ofApp : public ofBaseApp {
	public:

		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
        void sendKeepAlive();
        void setFrame(int frame);

        string name;
        string videoPath;
    
		ofxOscReceiver receiver;
        ofxOscSender sender;
		ofxOMXPlayer omxPlayer;
    
        float lastKeepAliveTimeStamp;
        string serverIP;
        bool serverRegistered;
        //float lastKeepAliveTimeStamp;
    
        bool isReinitializing;
    
#ifdef USE_ARDUINO
        ofSerial	serial;
#endif
};
