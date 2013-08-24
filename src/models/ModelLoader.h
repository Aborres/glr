/*
 * ModelLoader.h
 *
 *  Created on: 2011-05-08
 *      Author: jarrett
 */

#ifndef MODELLOADER_H_
#define MODELLOADER_H_

#include <string>
#include <vector>
#include <memory>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "IModel.h"

namespace glmd = glm::detail;

namespace glr {
namespace models {

class ModelLoader {
public:
	ModelLoader();
	virtual ~ModelLoader();

	std::vector< std::shared_ptr<ModelData> > loadModel(const std::string path);
	
private:
	aiLogStream stream;

	MeshData loadMesh(const std::string path, glmd::uint32 index, const aiMesh* mesh, std::map< std::string, glmd::uint32 >& boneIndexMap);
	TextureData loadTexture(const std::string path, glmd::uint32 index, const aiMaterial* material);
	MaterialData loadMaterial(const std::string path, glmd::uint32 index, const aiMaterial* material);
	BoneData loadBones(const std::string path, glmd::uint32 index, const aiMesh* mesh);
	AnimationData loadAnimation(const std::string path, const aiScene* scene);
	
	glm::mat4 convertAssImpMatrix(aiMatrix4x4 m);
};

}
}
#endif /* MODELLOADER_H_ */
