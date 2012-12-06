/*
 * DefaultSceneManager.cpp
 *
 *  Created on: 2011-05-08
 *      Author: jarrett
 */

#include "DefaultSceneManager.h"

#include "CameraSceneNode.h"
#include "DefaultSceneNode.h"
#include "ModelManager.h"

namespace icee {

namespace engine {

DefaultSceneManager::DefaultSceneManager() {

}

DefaultSceneManager::~DefaultSceneManager() {
}

/**
 * FOR TESTING PURPOSES ONLY.
 */
ISceneNode* DefaultSceneManager::addDefaultSceneNode(const char* name, ISceneNode* parent, sint32 id) {
	DefaultSceneNode* testNode = new DefaultSceneNode();
	sceneNodes_.push_back(testNode);

	return testNode;
	//return 0;
}

ISceneNode* DefaultSceneManager::addSceneNode(const char* name, ISceneNode* parent, sint32 id) {
	//SceneNode* node = new SceneNode();
	//sceneNodes_.push_back(node);

	//return node;
	return 0;
}

ICameraSceneNode* DefaultSceneManager::addCamera(vmath::Vector3f position, vmath::Vector3f lookAt) {
	CameraSceneNode* node = new CameraSceneNode(position, lookAt);

	std::vector<ISceneNode*>::iterator it;

	it = sceneNodes_.begin();
	it = sceneNodes_.insert(it, node);

	//sceneNodes_.push_back(node);

	return node;
}

ICameraSceneNode* DefaultSceneManager::addCameraFPS(vmath::Vector3f position,
		vmath::Vector3f lookAt, uint32 speed, uint32 rotationSpeed) {
	CameraSceneNode* node = new CameraSceneNode();
	sceneNodes_.push_back(node);

	return node;
}

void DefaultSceneManager::drawAll() {
	for (uint32 i = 0; i < sceneNodes_.size(); i++) {
		sceneNodes_[i]->render();
	}
}

IModelManager* DefaultSceneManager::getModelManager() {
	return ModelManager::getInstance();
}

}

}
