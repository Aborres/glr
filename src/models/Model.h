/*
 * Model.h
 *
 *  Created on: 2012-09-20
 *      Author: jarrett
 */

#ifndef MODEL_H_
#define MODEL_H_

#include <memory>
#include <map>
#include <string>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "IModel.h"
#include "../glw/IOpenGlDevice.h"

#include "../glw/IMaterialManager.h"
#include "../glw/ITextureManager.h"
#include "../glw/IMeshManager.h"
#include "../glw/IAnimationManager.h"

#include "../glw/Mesh.h"
#include "../glw/Texture.h"
#include "../glw/Material.h"
#include "../glw/Animation.h"

namespace glr {
namespace models {
class Model : public IModel {
public:
	Model(glw::IOpenGlDevice* openGlDevice);
	Model(std::vector< std::shared_ptr<ModelData> > modelData, glw::IOpenGlDevice* openGlDevice);
	Model(const Model& other);
	virtual ~Model();

	virtual glw::IAnimation* getAnimation(const std::string name);
	virtual glw::IAnimation* getCurrentAnimation();
	virtual void setCurrentAnimation(glw::IAnimation* animation);
	virtual void render(shaders::IShaderProgram* shader);

protected:
#define aisgl_min(x, y) (x < y ? x : y)
#define aisgl_max(x, y) (y > x ? y : x)

	glw::IOpenGlDevice* openGlDevice_;

	std::vector<glw::Mesh*> meshes_;
	std::vector<glw::Texture*> textures_;
	std::vector<glw::Material*> materials_;
	std::map< std::string, glw::Animation*> animations_;
	
	glw::Animation* currentAnimation_;
	glw::Animation* emptyAnimation_;
	
	/* 	All meshes in this model use this bone node tree for animations.
		Any animations that manipulate bone nodes will be manipulating bones in this bone node tree. */
	glw::BoneNode rootBoneNode_;
	
	glm::mat4 globalInverseTransformation_;
	
	glw::IMeshManager* meshManager_;
	glw::IMaterialManager* materialManager_;
	glw::ITextureManager* textureManager_;
	glw::IAnimationManager* animationManager_;

	void loadMeshes(const aiScene* scene);
	void loadTextures(const aiScene* scene);
	void loadMaterials(const aiScene* scene);
	void loadAnimations(const aiScene* scene);
	
private:
	void initialize(std::vector< std::shared_ptr<ModelData> > modelData = std::vector< std::shared_ptr<ModelData> >());
	void destroy();

};
}
}
#endif /* MODEL_H_ */
