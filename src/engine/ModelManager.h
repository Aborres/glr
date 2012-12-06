/*
 * ModelManager.h
 *
 *  Created on: 2011-05-08
 *      Author: jarrett
 */

#ifndef MODELMANAGER_H_
#define MODELMANAGER_H_

#include <memory>
#include <string>
#include <map>

/* gl.h we need OpenGL */
#include <GL/gl.h>

// assimp include files. These three are usually needed.
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../common/compatibility/Types.h"

#include "IModelManager.h"

#include "Model.h"

namespace icee {

namespace engine {
	
using namespace compatibility;

class ModelManager : public IModelManager {
public:
	ModelManager();
	virtual ~ModelManager();
	ModelManager(ModelManager const&);
	ModelManager& operator=(ModelManager const&);
	
	static ModelManager* getInstance();

	IModel* loadModel(const std::string path);
	
	//void testLoadTexture();
	//void testDrawTest1();
	
private:
	static ModelManager* modelManager_;

	aiLogStream stream;
	
	std::map< std::string, std::unique_ptr<Model> > models_;
	
	
};

}

}

#endif /* MODELMANAGER_H_ */