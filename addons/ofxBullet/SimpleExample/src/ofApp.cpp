#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	srand(time(0));

	ofSetFrameRate(60);
	ofSetVerticalSync(true);
	ofBackground(10, 10, 10);
	ofDisableAlphaBlending();
	ofEnableDepthTest();
	ofDisableArbTex();
	ofLoadImage(mTex, "lined1.jpg");
	ofLoadImage(mGrass, "concrete.jpg");
	mGrass.setTextureWrap(GL_REPEAT, GL_REPEAT);
	groundPrimitive.set(30.0f, 1.0f, 270.0f);
	groundPrimitive.mapTexCoords(0, 0, 3, 27);

	cubeMap.loadImages("positive_x.png", "negative_x.png", "positive_y.png", "negative_y.png", "positive_z.png", "negative_z.png");

	myFont.load("Abirds.ttf", 48, true, true);

	dirLight.setDirectional();
	dirLight.setSpecularColor(ofColor(255));
	dirLight.setDiffuseColor(ofColor(255));
	dirLight.setAmbientColor(ofColor(50));
	dirLight.setOrientation(glm::vec3(170.0f, 0.0f, 0.0f));

	camera.setPosition(ofVec3f(0, 5.f, 10.f));
	camera.lookAt(ofVec3f(0, 0, 0), ofVec3f(0, 1, 0));
	camera.setNearClip(0.1f);


	createWorld();
}

void ofApp::createWorld()
{
	world = new ofxBulletWorldRigid();
	world->setup();
	world->setGravity(ofVec3f(0.0f, -9.8f, 0.0f));
	world->enableGrabbing();
	world->enableDebugDraw();
	world->setCamera(&camera);

	myShapes *groundUserData = new myShapes("10", ofColor(255));
	ground = new ofxBulletBox();
	ground->create(world->world, ofVec3f(0., -5., -120), 0., 30.f, 1.f, 270.f);
	ground->getCollisionObject()->setUserPointer(groundUserData);
	ground->setProperties(.25, .95);
	ground->getCollisionObject()->setUserIndex(10);
	ground->add();
	groundPrimitive.setPosition(ground->getPosition());
	groundPrimitive.setOrientation(ground->getRotationQuat());

	sphere = new ofxBulletSphere();
	sphere->create(world->world, ofVec3f(0, -1, 0), 0.1, .50);
	sphere->getCollisionObject()->setUserPointer(sphereUserData);
	sphere->setActivationState(DISABLE_DEACTIVATION);
	sphere->getRigidBody()->setAngularFactor(btVector3(1, 1, 1));
	sphere->getRigidBody()->setRestitution(0.0);
	sphere->getRigidBody()->setFriction(1);
	sphere->add();
	cout << sphere->getPosition().z << endl;

	for (int i = 0; i < 25; i++)
	{
		if (i % 5 == 0 && i != 0)
		{
			zOffset += 10;
		}
		if (/*xOffset >= 5*/ xOffset % 5 == 0)
		{
			xOffset = 0;
			random_shuffle(shapeColors.begin(), shapeColors.end());
		}
		ofxBulletBox* box = new ofxBulletBox();
		Boxes *currentBox = new Boxes(box, to_string(i), shapeColors[xOffset]);
		box->create(world->world, ofVec3f(-8 + (4 * xOffset), -4, -2 * zOffset - 10), .05, 6, 2, 2);
		box->getCollisionObject()->setUserPointer(currentBox->userData);
		box->setActivationState(DISABLE_DEACTIVATION);
		box->getRigidBody()->setActivationState(4);
		box->add();

		lvl1Boxes.push_back(currentBox);
		xOffset++;
	}

	sphereUserData->color = lvl1Boxes[rand() % lvl1Boxes.size()]->userData->color;
	box_zvel = -4.0f;
}

void ofApp::clearWorld()
{
	box_xvel = 0, box_zvel = 0, box_yvel = -3.0;
	hasJumped = false;
	gameActive = true;
	switchColor = false;
	xOffset = 0;
	zOffset = 1;
	delete sphere;
	delete ground;
	lvl1Boxes.clear();
	delete world;
	cout << lvl1Boxes.size() << endl;
}

//--------------------------------------------------------------
void ofApp::update()
{
	world->update();
	ofSetWindowTitle(ofToString(ofGetFrameRate(), 0));

	int numManifolds = world->dispatcher->getNumManifolds();
	for (int i = 0; i < numManifolds; i++)
	{
		btPersistentManifold* contactManifold = world->dispatcher->getManifoldByIndexInternal(i);
		if (contactManifold->getBody0()->getUserIndex() == 10 || contactManifold->getBody1()->getUserIndex() == 10)
		{
			continue;
		}
		const btCollisionObject* obA = contactManifold->getBody0();
		const btCollisionObject* obB = contactManifold->getBody1();


		if (((myShapes*)obA->getUserPointer())->ID == "player" || ((myShapes*)obB->getUserPointer())->ID == "player")
		{
			if (((myShapes*)obA->getUserPointer())->color == ((myShapes*)obB->getUserPointer())->color)
			{
				if (((myShapes*)obA->getUserPointer())->ID != "player" && ((myShapes*)obB->getUserPointer())->isDead == false)
				{

					int idRemove = atoi(((myShapes*)obA->getUserIndex())->ID.c_str());
				}
				else
				{
					if (((myShapes*)obA->getUserPointer())->isDead == false)
					{
						int idRemove = atoi(((myShapes*)obB->getUserIndex())->ID.c_str());
						lvl1Boxes[idRemove]->body->getRigidBody()->applyCentralImpulse(btVector3(0.0, 0.6, -0.6));
						switchColor = true;
						switchTime = ofGetElapsedTimeMillis() + 400;
						
					}
				}
			}
			else
			{
				box_xvel = 0;
				box_yvel = 0;
				box_zvel = 0;
				gameActive = false;
			}
		}
	}

	if (gameActive == true)
	{
		if (sphere->getPosition().y <= -4)
		{
			hasJumped = false;
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw()
{
	glEnable(GL_DEPTH_TEST);
	ofEnableLighting();
	dirLight.enable();
	//easyCam.begin();
	camera.begin();
	camera.setGlobalPosition(sphere->getPosition().x, sphere->getPosition().y + 5, sphere->getPosition().z + 10);

	ofSetLineWidth(1.f);
	//ofSetColor(255, 0, 200);
	//world.drawDebug();

	cubeMap.bind();
	cubeShader.begin();
	cubeShader.setUniform1i("EnvMap", 0);
	cubeMap.drawSkybox(1000);
	cubeShader.end();
	cubeMap.unbind();

	mGrass.bind();
	ofSetColor(100, 100, 100);
	groundPrimitive.draw();
	mGrass.unbind();

	sphere->getRigidBody()->setLinearVelocity(btVector3(box_xvel, box_yvel, box_zvel));

	//sphere->getRigidBody()->applyCentralForce(btVector3(0, 0, -1));
	ofSetColor(((myShapes*)sphere->getCollisionObject()->getUserPointer())->color);

	mTex.bind();
	sphereP.setRadius(0.50);
	sphereP.setOrientation(sphere->getRotationQuat());
	sphereP.setPosition(sphere->getPosition());
	sphereP.draw();
	mTex.unbind();

	score = -sphere->getPosition().z * 10;
	//cout<<score<<endl;

	//ofSetColor(225, 225, 225);
	//box->getRigidBody()->setLinearVelocity(btVector3(box_xvel, 1.0, box_zvel));
	//box->draw();
	for (size_t i = 0; i < lvl1Boxes.size(); i++)
	{
		//ofSetColor(((myShapes*)lvl1Boxes[i]->body->getCollisionObject()->getUserPointer())->color);
		ofSetColor(lvl1Boxes[i]->userData->color);
		lvl1Boxes[i]->body->draw();
		ofNoFill();
		ofSetLineWidth(2);
		ofSetColor(0);
		lvl1Boxes[i]->body->draw();
		ofFill();
	}


	ofSetColor(255);
	ofPushMatrix();
	ofTranslate(glm::vec3(sphere->getPosition().x - 0.5f, sphere->getPosition().y + 4.0f, sphere->getPosition().z));
	ofScale(0.02f);
	myFont.drawString(to_string(score), 0, 0);
	if (score >= 1000)
	{
		text = "YOU WIN!";
		box_xvel = 0;
		box_yvel = 0;
		box_zvel = 0;
		gameActive = false;
	}
	if (gameActive == false)
	{
	
		if(score < 1000)
		{
			text = "GAME OVER";
		}
		myFont.drawString("\t" + text + "\nPress Y to restart", -150, -70.0);
	}
	ofPopMatrix();

	if (switchColor == true && ofGetElapsedTimeMillis() > switchTime)
	{
		sphereUserData->color = shapeColors[rand() % shapeColors.size()];
		switchColor = false;
	}
	dirLight.disable();
	//easyCam.end();
	camera.end();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	if (gameActive == true)
	{
		if (key == 'w')
		{

			//box_zvel = -4.0;
			//box->getRigidBody()->setLinearVelocity(btVector3(2.0, 0.0, 0.0));
		}
		if (key == 's')
		{
			//box_zvel = 4.0;
			//box->getRigidBody()->setLinearVelocity(btVector3(-2.0, 0.0, 0.0));
		}
		if (key == 'a')
		{
			box_xvel = -8.0;
			//sphere->getRigidBody()->applyCentralForce(btVector3(-8, 0, 0));
			//sphere->getRigidBody()->applyCentralForce(btVector3(-8, 0, 0));

			//box->getRigidBody()->setLinearVelocity(btVector3(0.0, 0.0, 2.0));
		}
		if (key == 'd')
		{
			box_xvel = 8.0;
			//sphere->getRigidBody()->applyCentralForce(btVector3(8, 0, 0));
			//box->getRigidBody()->setLinearVelocity(btVector3(0.0, 0.0, -2.0));
		}
		if (key == 'f')
		{
			if (sphere->getPosition().y <= 0 && hasJumped == false)
			{
				box_yvel = 10.0;
			}
			else
			{
				hasJumped = true;
				box_yvel = -6.0;
			}
		}
	}
	else
	{
		if (key == 'y')
		{
			clearWorld(/*world*/);
			createWorld(/*world*/);
		}
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{
	if (gameActive == true)
	{
		if (key == 'w')
		{
			//box_zvel = 0.0;
			//box->getRigidBody()->setLinearVelocity(btVector3(2.0, 0.0, 0.0));
		}
		if (key == 's')
		{
			//box_zvel = 0.0;

			//box->getRigidBody()->setLinearVelocity(btVector3(-2.0, 0.0, 0.0));
		}
		if (key == 'a')
		{
			box_xvel = 0.0;
			//box->getRigidBody()->setLinearVelocity(btVector3(0.0, 0.0, 2.0));
		}
		if (key == 'd')
		{
			box_xvel = 0.0;
			//box->getRigidBody()->setLinearVelocity(btVector3(0.0, 0.0, -2.0));
		}

		if (key == 'f')
		{
			hasJumped = true;
			box_yvel = -6.0;
		}
	}
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y)
{

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg)
{

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo)
{

}