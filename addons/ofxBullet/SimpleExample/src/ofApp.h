#pragma once

#include "ofMain.h"
#include "ofxBullet.h"
#include "ofxCubeMap.h"
#include <ctime>

class ofApp : public ofBaseApp
{
private:
	struct myShapes
	{
		std::string ID;
		bool isDead = false;
		ofColor color;
		myShapes() {};
		myShapes(std::string ID)
		{
			this->ID = ID;
		}

		myShapes(std::string ID, ofColor color)
		{
			this->ID = ID;
			this->color = color;
		}
	};

	struct Boxes
	{
		myShapes* userData;
		ofxBulletBox* body;

		Boxes(ofxBulletBox* bod, std::string id, ofColor colour)
		{
			userData = new myShapes();
			body = bod;
			userData->ID = id;
			userData->color = colour;
		}
		~Boxes()
		{
			delete userData;
			delete body;
		}
	};

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

	void createWorld();
	void clearWorld();

	ofxBulletWorldRigid*			world;
	ofxBulletBox*				ground;

	ofxBulletSphere*			sphere;
	std::vector<Boxes*>			lvl1Boxes;

	ofCamera					camera;
	ofLight						dirLight;

	ofTexture mTex;
	ofTexture mGrass;
	ofSpherePrimitive sphereP;
	ofBoxPrimitive groundPrimitive;

	ofxCubeMap cubeMap;
	ofShader cubeShader;

	ofEasyCam easyCam;

	double box_xvel = 0, box_zvel = 0, box_yvel = -3.0;
	bool hasJumped = false;
	bool gameActive = true;
	bool switchColor = false;
	unsigned long currentTime, switchTime;
	int xOffset = 0;
	int zOffset = 1;
	int score = 0;
	std::string text;

	std::vector<ofColor> shapeColors = {
		ofColor(237, 36, 45, 255),
		ofColor(243, 119, 32, 255),
		ofColor(255, 205, 52, 255),
		ofColor(0, 128, 164, 255),
		ofColor(248, 248, 242, 255)
	};

	myShapes *sphereUserData = new myShapes("player");

	ofTrueTypeFont myFont;
};

