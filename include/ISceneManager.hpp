#ifndef ISCENEMANAGER_H_
#define ISCENEMANAGER_H_

#include "ISceneNode.hpp"
#include "ICamera.hpp"
#include "ILight.hpp"
#include "models/IModelManager.hpp"
#include "models/IBillboardManager.hpp"
#include "glw/shaders/IShaderProgramManager.hpp"

namespace glr
{

namespace glmd = glm::detail;

class ISceneManager
{
public:
	virtual ~ISceneManager()
	{
	}
	;

	virtual ISceneNode* createSceneNode(const std::string& name = std::string()) = 0;
	virtual ICamera* createCamera(glm::detail::uint32 speed = 1, glm::detail::uint32 rotationSpeed = 5) = 0;
	virtual ILight* createLight(const std::string& name = std::string()) = 0;

	virtual void addCamera(std::unique_ptr<ICamera> camera) = 0;
	virtual void addSceneNode(std::unique_ptr<ISceneNode> sceneNode) = 0;

	virtual ISceneNode* getSceneNode(glm::detail::uint32 id) const = 0;
	virtual ISceneNode* getSceneNode(const std::string& name) const = 0;
	virtual ICamera* getCamera() const = 0;
	virtual ILight* getLight(glm::detail::uint32 id) const = 0;
	virtual ILight* getLight(const std::string& name) const = 0;

	virtual void destroySceneNode(glm::detail::uint32 id) = 0;
	virtual void destroySceneNode(const std::string& name) = 0;
	virtual void destroySceneNode(ISceneNode* node) = 0;
	virtual void destroyAllSceneNodes() = 0;
	virtual void destroyCamera() = 0;
	virtual void destroyLight(glm::detail::uint32 id) = 0;
	virtual void destroyLight(const std::string& name) = 0;
	virtual void destroyLight(ILight* node) = 0;
	virtual void destroyAllLights() = 0;

	virtual glmd::uint32 getNumSceneNodes() const = 0;
	virtual glmd::uint32 getNumLights() const = 0;

	virtual void drawAll() = 0;

	// @Deprecated(?)
	virtual ISceneNode* getRootSceneNode() const = 0;
	
	virtual void setDefaultShaderProgram(shaders::IShaderProgram* shaderProgram) = 0;
	
	virtual const glm::mat4& getModelMatrix() const = 0;

	virtual models::IModelManager* getModelManager() const = 0;
	virtual shaders::IShaderProgramManager* getShaderProgramManager() const = 0;

	virtual const std::vector<LightData>& getLightData() = 0;
};

}

#endif /* ISCENEMANAGER_H_ */
