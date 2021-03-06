#ifndef ITERRAIN_H_
#define ITERRAIN_H_

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include "ISceneNode.hpp"
#include "TerrainSettings.hpp"

namespace glr
{

namespace models
{
class IModel;
}

namespace terrain
{

struct Point
{
	glmd::float32 density;
	glm::vec3 pos;
};

struct Block
{
	Point points[2][2][2];
	glmd::int32 index;
	glm::vec3 meshPoint;
	glm::vec3 meshPointNormal;
};

class ITerrain : public virtual ISceneNode
{
public:
	virtual ~ITerrain()
	{
	}
	;
	
	/**
	 * Will render the terrain to scene.
	 * 
	 * **Not Thread Safe**: This method is *not* safe to call in a multi-threaded environment, and should only be called from the 
	 * OpenGL thread.
	 */
	virtual void render() = 0;
	
	/**
	 * Tells us whether this terrain has changes that need to be passed to OpenGL.
	 */
	virtual bool isDirty() const = 0;
	virtual void setIsDirty(bool isDirty) = 0;
	
	/**
	 * Will update this terrain.
	 * 
	 * **Not Thread Safe**: This method is *not* safe to call in a multi-threaded environment, and should only be called from the 
	 * OpenGL thread.
	 */
	virtual void update() = 0;
	
	virtual void generate(TerrainSettings settings = TerrainSettings()) = 0;
	
	virtual bool isEmptyOrSolid() const = 0;
	
	/**
	 * Will prepare or update all of the graphics for this terrain chunk.  After this function returns, the terrain will be ready to 
	 * be rendered on the screen.
	 * 
	 * **Not Thread Safe**: This method is *not* safe to call in a multi-threaded environment, and should only be called from the 
	 * OpenGL thread.
	 */
	virtual void prepareOrUpdateGraphics() = 0;
	
	/**
	 * Will set and update the level of detail of the terrain.  If the lod passed in is the same as the current lod, this method
	 * will do nothing.
	 * 
	 * If the level of detail is different than the current value, this method will do any preparation necessary for the lod change, and
	 * then mark the terrain as dirty.
	 */
	virtual void updateLod(LevelOfDetail lod) = 0;
	
	/**
	 * Returns the current lod value.
	 * 
	 * @return The current level of detail.
	 */
	virtual LevelOfDetail getLod() const = 0;
	
	virtual glm::detail::int32 getGridX() const = 0;
	virtual glm::detail::int32 getGridY() const = 0;
	virtual glm::detail::int32 getGridZ() const = 0;
	
	/**
	 * Returns the model associated with this terrain.
	 * 
	 * @return The model associated with this terrain.
	 */
	virtual models::IModel* getModel() const = 0;
};

}
}

#endif /* ITERRAIN_H_ */
