#ifndef GLR_CAMERA_H_
#define GLR_CAMERA_H_

#include "ICamera.hpp"
#include "BasicSceneNode.hpp"

namespace glr
{

class Camera : public virtual ICamera, public BasicSceneNode
{
public:
	Camera(Id id, glw::IOpenGlDevice* openGlDevice);
	virtual ~Camera();

	// inherited from ICamera
	virtual bool isActive() const;

	virtual void render();

	virtual void attach(models::IModel* model);

	virtual void move(const glm::vec3& moveVector);

	virtual void tick(glm::detail::float32 time);

	virtual const glm::mat4& getViewMatrix() const;

private:
	glm::detail::int32 prevX_;
	glm::detail::int32 prevY_;

	glm::mat4 viewMatrix_;
	glm::vec3 direction_;

	glm::vec3 movementBuffer_;
	glm::detail::float32 moveSpeed_;
	glm::detail::float32 rotSpeed_;
	void clearMovementBuffer();
	glm::detail::float32 xRot_;
	glm::detail::float32 yRot_;

	void initialize();
};

}

#endif /* GLR_CAMERA_H_ */
