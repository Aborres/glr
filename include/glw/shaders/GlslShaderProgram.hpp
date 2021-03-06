#ifndef GLSLSHADERPROGRAM_H_
#define GLSLSHADERPROGRAM_H_

#include <vector>
#include <memory>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include "IShaderProgram.hpp"

#include "../IOpenGlDevice.hpp"

#include "GlslShader.hpp"

namespace glr
{
namespace shaders
{

/**
 * Class representing a 'glsl' shader program.  This is a shader program that holds all of the processed shader source required for this shader program.
 */
class GlslShaderProgram : public IShaderProgram
{
public:
	GlslShaderProgram(std::string name, std::vector< std::unique_ptr<GlslShader> > shaders, glw::IOpenGlDevice* openGlDevice);
	virtual ~GlslShaderProgram();

	virtual void bind();
	virtual GLuint getGLShaderProgramId() const;
	virtual IShader::BindingsMap getBindings();
	void compile();

	virtual GLint getBindPointByVariableName(const std::string& varName) const;
	virtual GLint getBindPointByBindingName(IShader::BindType bindType) const;
	
	virtual GLint getVertexAttributeLocationByName(const std::string& varName) const;
	
	const std::string& getName() const;
	
	virtual void addBindListener(IShaderProgramBindListener* bindListener);
	virtual void removeBindListener(IShaderProgramBindListener* bindListener);
	void removeAllBindListeners();

	void unbind();
	static void unbindAll();

private:
	GLuint programId_;

	std::string name_;
	std::vector< std::unique_ptr<GlslShader> > shaders_;
	glw::IOpenGlDevice* openGlDevice_;

	IShader::BindingsMap bindings_;
	
	std::vector<IShaderProgramBindListener*> bindListeners_;
	
	void generateBindings();
};

}
}

#endif /* GLSLSHADERPROGRAM_H_ */
