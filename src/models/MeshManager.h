/*
 * MeshManager.h
 *
 *  Created on: 2011-05-08
 *      Author: jarrett
 */

#ifndef MESHMANAGER_H_
#define MESHMANAGER_H_

#include <memory>
#include <string>
#include <map>

// assimp include files. These three are usually needed.
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"

namespace glr {

namespace models {

class MeshManager {
public:
	MeshManager();
	virtual ~MeshManager();

	Mesh* getMesh(const std::string path);
	
private:
	aiLogStream stream;
	
	std::map< std::string, std::unique_ptr<Mesh> > meshes_;
	
	
};

}

}

#endif /* MESHMANAGER_H_ */
