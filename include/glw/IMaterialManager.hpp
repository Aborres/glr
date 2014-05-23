#ifndef IMATERIALMANAGER_H_
#define IMATERIALMANAGER_H_

#include <string>

#include "Material.hpp"

#include "serialize/ITextSerializable.hpp"

namespace glr
{
namespace glw
{

class Material;

class IMaterialManager : public virtual serialize::ITextSerializable
{
public:
	virtual ~IMaterialManager()
	{
	}
	;

	/**
	 * Returns the material with the given name.  If a material with the given name doesn't exist, it returns
	 * nullptr.
	 * 
	 * @param name The name of the material to retrieve.
	 * 
	 * @return The Material object with name 'name', or nullptr if no material exists with that name.
	 */
	virtual Material* getMaterial(const std::string& name) const = 0;
	
	/**
	 * Creates a material with the given name.
	 * 
	 * If a material already exists with the given name, it will return that material.
	 * 
	 * **Note**: If initialize is true, this method is *not* thread safe (i.e. it should only be called from the OpenGL thread).  If it is
	 * false, then this method *is* thread safe (i.e. it is safe to call outside the OpenGL thread).
	 * 
	 * @param name The name to use for the new material.
	 * @param initialize If true, will initialize all of the resources required for this material.  Otherwise, it will
	 * just create the material and return it (without initializing it).
	 * 
	 * @return A Material object.
	 */
	virtual Material* addMaterial(const std::string& name, bool initialize = true) = 0;
	
	/**
	 * Creates a material with the given name and using the provided material data.
	 * 
	 * If a material already exists with the given name, it will return that material.
	 * 
	 * **Note**: If initialize is true, this method is *not* thread safe (i.e. it should only be called from the OpenGL thread).  If it is
	 * false, then this method *is* thread safe (i.e. it is safe to call outside the OpenGL thread).
	 * 
	 * @param name The name to use for the new material.
	 * @param ambient
	 * @param diffuse
	 * @param specular
	 * @param emission
	 * @param shininess
	 * @param strength
	 * @param initialize If true, will initialize all of the resources required for this material.  Otherwise, it will
	 * just create the material and return it (without initializing it).
	 * 
	 * @return A Material object.
	 */
	virtual Material* addMaterial(
		const std::string& name,
		glm::vec4 ambient,
		glm::vec4 diffuse,
		glm::vec4 specular,
		glm::vec4 emission,
		glm::detail::float32 shininess,
		glm::detail::float32 strength,
		bool initialize = true
	) = 0;
};

}
}

#endif /* IMATERIALMANAGER_H_ */
