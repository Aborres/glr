#ifndef ANIMATION_H_
#define ANIMATION_H_

#include <vector>
#include <map>

#include <GL/glew.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include "IAnimation.hpp"
#include "IMesh.hpp"

#include "AnimatedBoneNode.hpp"

#include "IOpenGlDevice.hpp"

namespace glr
{
namespace glw
{

namespace glmd = glm::detail;

class Animation : public IAnimation {
public:
	Animation(IOpenGlDevice* openGlDevice, std::string name);
	Animation(IOpenGlDevice* openGlDevice, std::string name, glm::detail::float64 duration, glm::detail::float64 ticksPerSecond, std::map< std::string, AnimatedBoneNode > animatedBoneNodes);
	Animation(const Animation& other);
	virtual ~Animation();

	virtual void bind() const;

	virtual void pushToVideoMemory();
	virtual void pushToVideoMemory(const std::vector< glm::mat4 >& transformations);
	virtual void pullFromVideoMemory();
	virtual void freeLocalData();
	virtual void freeVideoMemory();
	virtual void allocateVideoMemory();

	virtual GLuint getBufferId() const;
	GLuint getBindPoint() const;

	virtual void setAnimationTime(glmd::float32 runningTime);
	virtual void setFrameClampping(glm::detail::uint32 startFrame, glm::detail::uint32 endFrame);

	virtual void calculate(const glm::mat4& globalInverseTransformation, const BoneNode& rootBoneNode, const BoneData& boneData);
	virtual void calculate(const glm::mat4& globalInverseTransformation, const BoneNode& rootBoneNode, const BoneData& boneData, std::vector<glmd::uint32>& indexCache);
	virtual void calculate(std::vector< glm::mat4 >& transformations, const glm::mat4& globalInverseTransformation, const BoneNode& rootBoneNode, const BoneData& boneData);
	virtual void calculate(std::vector< glm::mat4 >& transformations, const glm::mat4& globalInverseTransformation, const BoneNode& rootBoneNode, const BoneData& boneData, std::vector<glmd::uint32>& indexCache);
	
	void generateIdentityBoneTransforms(glmd::uint32 numBones);
	
	virtual const std::string& getName() const;
	
private:
	GLuint bufferId_;
	GLuint bindPoint_;

	IOpenGlDevice* openGlDevice_;
	std::string name_;
	glm::detail::float64 duration_;
	glm::detail::float64 ticksPerSecond_;
	std::map< std::string, AnimatedBoneNode > animatedBoneNodes_;
	
	// The running time can change multiple times per frame (models share animations)
	glmd::float32 runningTime_;
	
	// Only play frames within this range
	glmd::uint32 startFrame_;
	glmd::uint32 endFrame_;
	
	// A temporary cache to hold previous rotation, position, and scaling node search indices
	std::vector<glmd::uint32> indexCache_;
	
	// The current transformation matrices generated by this animation from the information provided through calling the calculate(..) method
	std::vector< glm::mat4 > currentTransforms_;

	void setupAnimationUbo();

	glmd::uint32 findPosition(float animationTime, AnimatedBoneNode* animatedBoneNode);
	glmd::uint32 findRotation(float animationTime, AnimatedBoneNode* animatedBoneNode);
	glmd::uint32 findScaling(float animationTime, AnimatedBoneNode* animatedBoneNode);

	void calcInterpolatedPosition(glm::vec3& Out, float animationTime, AnimatedBoneNode* animatedBoneNode);
	void calcInterpolatedRotation(glm::quat& Out, float animationTime, AnimatedBoneNode* animatedBoneNode);
	void calcInterpolatedScaling(glm::vec3& Out, float animationTime, AnimatedBoneNode* animatedBoneNode);
	void readNodeHeirarchy(std::vector< glm::mat4 >& transformations, glmd::float32 animationTime, const glm::mat4& globalInverseTransform, const BoneNode& rootBoneNode, const BoneData& boneData, const glm::mat4& parentTransform);
};

}
}

#endif /* ANIMATION_H_ */
