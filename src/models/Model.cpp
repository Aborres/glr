#include <utility>

#include "models/Model.hpp"

#include "glw/Constants.hpp"

#include "exceptions/Exception.hpp"

namespace glr
{
namespace models
{

Model::Model()
{
	id_ = Id::INVALID;
	name_ = std::string();
	openGlDevice_ = nullptr;
	initialize();
}

Model::Model(Id id, std::string name, glw::IOpenGlDevice* openGlDevice) : id_(id), name_(std::move(name)), openGlDevice_(openGlDevice)
{
	initialize();
}

Model::Model(Id id, std::string name, glw::IMesh* mesh, glw::ITexture* texture, glw::IMaterial* material, std::vector<glw::IAnimation*> animations, glw::BoneNode rootBoneNode, glm::mat4 globalInverseTransformation, glw::IOpenGlDevice* openGlDevice)
	: id_(id), name_(std::move(name)), rootBoneNode_(std::move(rootBoneNode)), globalInverseTransformation_(std::move(globalInverseTransformation)), openGlDevice_(openGlDevice)
{
	initialize();
	
	meshes_.push_back(mesh);
	textures_.push_back(texture);
	textures_.push_back(texture);
	materials_.push_back(material);
	
	// Add animations to map
	for (auto animation : animations)
	{
		animations_[ animation->getName() ] = animation;
	}
}

Model::Model(Id id, std::string name, std::vector<glw::IMesh*> meshes, std::vector<glw::ITexture*> textures, std::vector<glw::IMaterial*> materials, std::vector<glw::IAnimation*> animations, glw::BoneNode rootBoneNode, glm::mat4 globalInverseTransformation, glw::IOpenGlDevice* openGlDevice)
	: id_(id), name_(std::move(name)), meshes_(std::move(meshes)), textures_(std::move(textures)), materials_(std::move(materials)), rootBoneNode_(std::move(rootBoneNode)), globalInverseTransformation_(std::move(globalInverseTransformation)), openGlDevice_(openGlDevice)
{
	// Error check - Make sure meshes, textures, and materials vectors are the same length
	if (meshes_.size() != textures_.size() || meshes_.size() != materials_.size() || materials_.size() != textures_.size())
	{
		std::string msg = std::string("Unable to create Model...Mesh, Texture and Material vectors are not the same size.");
		LOG_ERROR( msg );
		throw exception::Exception(msg);
	}
	
	initialize();
	
	// Add animations to map
	for (auto animation : animations)
	{
		animations_[ animation->getName() ] = animation;
	}
}

Model::Model(Id id, std::string name, glw::IMesh* mesh, glw::ITexture* texture, glw::IMaterial* material, glw::IOpenGlDevice* openGlDevice)
: id_(id), name_(std::move(name)), openGlDevice_(openGlDevice)
{
	rootBoneNode_ = glw::BoneNode();
	globalInverseTransformation_ = glm::mat4();
	
	initialize();
	
	meshes_.push_back(mesh);
	textures_.push_back(texture);
	materials_.push_back(material);
}

Model::Model(const Model& other)
{
	std::string msg = std::string("Unable to copy Model...In order to copy a Model, you must provide a new id for the copy.");
	LOG_ERROR( msg );
	throw exception::Exception(msg);
}

Model::Model(Id id, const Model& other) : id_(id)
{
	copy(other);
}

Model::Model(Id id, std::string newName, const Model& other) : id_(id)
{
	copy(other);
	
	name_ = std::move(newName);
}

Model::~Model()
{
}

void Model::copy(const Model& other)
{
	name_ = other.name_;
	
	openGlDevice_ = other.openGlDevice_;
	
	meshManager_ = openGlDevice_->getMeshManager();
	materialManager_ = openGlDevice_->getMaterialManager();
	textureManager_ = openGlDevice_->getTextureManager();
	animationManager_ = openGlDevice_->getAnimationManager();
	
	meshes_ = other.meshes_;
	textures_ = other.textures_;
	materials_ = other.materials_;
	
	animations_ = other.animations_;
	
	// TODO: Do we want to actually do a copy on this each time?
	rootBoneNode_ = other.rootBoneNode_;
	
	globalInverseTransformation_ = other.globalInverseTransformation_;
	
	currentAnimation_ = nullptr;
	
	indexCache_ = other.indexCache_;
	animationTime_ = other.animationTime_;
	startFrame_ = other.startFrame_;
	endFrame_ = other.endFrame_;
	
	// Set the current animation to our copied animation
	if ( other.currentAnimation_ != nullptr )
	{
		currentAnimation_ = other.currentAnimation_;
	}

	emptyAnimation_ = openGlDevice_->getAnimationManager()->getAnimation( glw::Constants::GLR_IDENTITY_BONES );
}

void Model::initialize()
{
	meshManager_ = openGlDevice_->getMeshManager();
	materialManager_ = openGlDevice_->getMaterialManager();
	textureManager_ = openGlDevice_->getTextureManager();
	animationManager_ = openGlDevice_->getAnimationManager();
	
	animations_ = std::map< std::string, glw::IAnimation* >();
	
	currentAnimation_ = nullptr;
	animationTime_ = 0.0f;
	startFrame_ = 0;
	endFrame_ = 0;
	indexCache_ = std::vector<glmd::uint32>( 3 );
	
	emptyAnimation_ = openGlDevice_->getAnimationManager()->getAnimation( glw::Constants::GLR_IDENTITY_BONES );
}

void Model::destroy()
{
}

glw::IMesh* Model::getMesh(glmd::uint32 index) const
{
	return meshes_[index];
}

void Model::removeMesh(glmd::uint32 index)
{
	meshes_.erase(meshes_.begin() + index);
	materials_.erase(materials_.begin() + index);
	textures_.erase(textures_.begin() + index);
}

void Model::removeMesh(glw::IMesh* mesh)
{
	glmd::int32 index = getIndexOf(mesh);
	
	meshes_.erase(meshes_.begin() + index);
	materials_.erase(materials_.begin() + index);
	textures_.erase(textures_.begin() + index);
}

void Model::addMesh(glw::IMesh* mesh)
{
	assert( mesh != nullptr );
	
	meshes_.push_back(mesh);
	materials_.push_back( nullptr );
	textures_.push_back( nullptr );
}

void Model::addMesh(glw::IMesh* mesh, glmd::uint32 index)
{
	assert( mesh != nullptr );
	
	meshes_.insert(meshes_.begin() + index, mesh);
	materials_.insert(materials_.begin() + index, nullptr);
	textures_.insert(textures_.begin() + index, nullptr);
}

glmd::uint32 Model::getNumberOfMeshes() const
{
	return meshes_.size();
}

glw::ITexture* Model::getTexture(glmd::uint32 index) const
{
	return textures_[index];
}

glw::ITexture* Model::getTexture(glw::IMesh* mesh) const
{
	glmd::int32 i = getIndexOf(mesh);
	if (i >= 0)
		return textures_[i];
	
	return nullptr;
}

void Model::removeTexture(glmd::uint32 index)
{
	textures_[index] = nullptr;
}

void Model::removeTexture(glw::ITexture* texture)
{
	glmd::int32 i = getIndexOf(texture);
	if (i >= 0)
		textures_[i] = nullptr;
}

void Model::removeTexture(glw::IMesh* mesh)
{
	glmd::int32 i = getIndexOf(mesh);
	if (i >= 0)
		textures_[i] = nullptr;
}

void Model::addTexture(glw::ITexture* texture, glmd::uint32 index)
{
	assert( texture != nullptr );
	//assert( index < meshes_.size() );
	
	textures_[index] = texture;
}

void Model::addTexture(glw::ITexture* texture, glw::Mesh* mesh)
{
	glmd::int32 i = getIndexOf(mesh);
	if (i >= 0)
		textures_[i] = texture;
}

glmd::uint32 Model::getNumberOfTextures() const
{
	return textures_.size();
}

glw::IMaterial* Model::getMaterial(glmd::uint32 index) const
{
	return materials_[index];
}

glw::IMaterial* Model::getMaterial(glw::IMesh* mesh) const
{
	glmd::int32 i = getIndexOf(mesh);
	if (i >= 0)
		return materials_[i];
	
	return nullptr;
}

void Model::removeMaterial(glmd::uint32 index)
{
	materials_[index] = nullptr;
}

void Model::removeMaterial(glw::IMaterial* material)
{
	glmd::int32 i = getIndexOf(material);
	if (i >= 0)
		materials_[i] = nullptr;
}

void Model::removeMaterial(glw::IMesh* mesh)
{
	glmd::int32 i = getIndexOf(mesh);
	if (i >= 0)
		materials_[i] = nullptr;
}

void Model::addMaterial(glw::IMaterial* material, glmd::uint32 index)
{
	assert( material != nullptr );
	//assert( index < meshes_.size() );
	
	materials_[index] = material;
}

void Model::addMaterial(glw::IMaterial* material, glw::Mesh* mesh)
{
	glmd::int32 i = getIndexOf(mesh);
	if (i >= 0)
		materials_[i] = material;
}

glmd::uint32 Model::getNumberOfMaterials() const
{
	return materials_.size();
}

const Id& Model::getId() const
{
	return id_;
}

const std::string& Model::getName() const
{
	return name_;
}

// TODO: Implement loop
void Model::playAnimation(glw::IAnimation* animation, glm::detail::float32 animationTime, bool loop)
{
	currentAnimation_ = animation;
	animationTime_ = animationTime;
	startFrame_ = 0;
	endFrame_ = 0;

	indexCache_ = std::vector<glmd::uint32>( 3 );
}

// TODO: Implement loop
void Model::playAnimation(glw::IAnimation* animation, glm::detail::uint32 startFrame, glm::detail::uint32 endFrame, glm::detail::float32 animationTime, bool loop)
{
	currentAnimation_ = animation;
	animationTime_ = animationTime;
	startFrame_ = startFrame;
	endFrame_ = endFrame;

	indexCache_ = std::vector<glmd::uint32>( 3 );
}

void Model::setAnimationTime(glm::detail::float32 animationTime)
{
	animationTime_ = animationTime;
}

void Model::stopAnimation()
{
	currentAnimation_ = nullptr;
}

glw::IAnimation* Model::getPlayingAnimation() const
{
	return currentAnimation_;
}

std::vector<glw::IAnimation*> Model::getAnimations() const
{
	auto list = std::vector<glw::IAnimation*>();
	
	for ( auto& a : animations_ )
	{
		list.push_back(a.second);
	}
	
	return list;
}

void Model::removeAnimation(const std::string& name)
{
}

void Model::removeAnimation(glw::IAnimation* animation)
{
}

void Model::addAnimation(glw::IAnimation* animation)
{
}

glmd::uint32 Model::getNumberOfAnimations() const
{
	return animations_.size();
}

glmd::int32 Model::getIndexOf(glw::IMesh* mesh) const
{
	for (glmd::uint32 i=0; i < meshes_.size(); i++)
	{
		if (meshes_[i] == mesh)
			return i;
	}
	
	return -1;
}

glmd::int32 Model::getIndexOf(glw::ITexture* texture) const
{
	for (glmd::uint32 i=0; i < textures_.size(); i++)
	{
		if (textures_[i] == texture)
			return i;
	}
	
	return -1;
}

glmd::int32 Model::getIndexOf(glw::IMaterial* material) const
{
	for (glmd::uint32 i=0; i < materials_.size(); i++)
	{
		if (materials_[i] == material)
			return i;
	}
	
	return -1;
}

void Model::render(shaders::IShaderProgram* shader)
{
	assert(shader != nullptr);

	for ( glm::detail::uint32 i = 0; i < meshes_.size(); i++ )
	{
		if ( textures_[i] != nullptr )
		{
			// TODO: bind to an actual texture position (for multiple textures per mesh, which we currently don't support...maybe at some point we will???  Why would we need multiple textures?)
			//shader->bindVariableByBindingName( shaders::IShader::BIND_TYPE_TEXTURE_2D, textures_[i]->getBindPoint() );
			textures_[i]->bind();
			/*
			GLint bindPoint = shader->getBindPointByBindingName( shaders::IShader::BIND_TYPE_TEXTURE_2D );
			if (bindPoint >= 0)
			{
				textures_[i]->bind();

				openGlDevice_->bindBuffer( textures_[i]->getBufferId(), bindPoint );
			}
			*/
		}
		
		if ( materials_[i] != nullptr )
		{
			GLint bindPoint = shader->getBindPointByBindingName( shaders::IShader::BIND_TYPE_MATERIAL );
			if (bindPoint >= 0)
			{
				materials_[i]->bind();

				openGlDevice_->bindBuffer( materials_[i]->getBufferId(), bindPoint );
			}
		}		
		
		if (currentAnimation_ != nullptr)
		{
			GLint bindPoint = shader->getBindPointByBindingName( shaders::IShader::BIND_TYPE_BONE );
			if (bindPoint >= 0)
			{
				currentAnimation_->setAnimationTime( animationTime_ );
				currentAnimation_->setFrameClampping( startFrame_, endFrame_ );
				currentAnimation_->calculate(globalInverseTransformation_, rootBoneNode_, meshes_[i]->getBoneData(), indexCache_);
				currentAnimation_->pushToVideoMemory();
				
				openGlDevice_->bindBuffer( currentAnimation_->getBufferId(), bindPoint );
			}
		}
		else
		{
			// Zero out the animation data
			// TODO: Do we need to do this?
			// TODO: find a better way to load 'empty' bone data in the shader
			GLint bindPoint = shader->getBindPointByBindingName( shaders::IShader::BIND_TYPE_BONE );
			if (bindPoint >= 0)
			{
				emptyAnimation_->pushToVideoMemory();

				openGlDevice_->bindBuffer( emptyAnimation_->getBufferId(), bindPoint );
			}
		}
		
		meshes_[i]->render();
	}
}


void Model::serialize(const std::string& filename)
{
	std::ofstream ofs(filename.c_str());
	serialize::TextOutArchive textOutArchive(ofs);
	serialize(textOutArchive);
}

void Model::serialize(serialize::TextOutArchive& outArchive)
{
	outArchive << *this;
}

void Model::deserialize(const std::string& filename)
{
	std::ifstream ifs(filename.c_str());
	serialize::TextInArchive textInArchive(ifs);
	deserialize(textInArchive);
}

void Model::deserialize(serialize::TextInArchive& inArchive)
{
	inArchive >> *this;
}

// TODO: Have a more generic serialize function, so we don't duplicate meshes, textures, etc each time we serialize a model
template<class Archive> void Model::serialize(Archive& ar, const unsigned int version)
{
	boost::serialization::split_member(ar, *this, version);
}

template<class Archive> void Model::save(Archive& ar, const unsigned int version) const
{
	boost::serialization::void_cast_register<Model, IModel>(
		static_cast<Model*>(nullptr),
		static_cast<IModel*>(nullptr)
	);
	
	// Define non-default constructor for Model??
	ar & name_;
	
	{
		auto size = meshes_.size();
		ar & size;
		for (auto it : meshes_)
		{
			std::string s = std::string();
			if (it != nullptr)
			{
				s = it->getName();
			}
	
			ar & s;
		}
	}
	
	{
		auto size = textures_.size();
		ar & size;
		for (auto it : textures_)
		{
			std::string s = std::string();
			if (it != nullptr)
			{
				s = it->getName();
			}
	
			ar & s;
		}
	}
	
	{
		auto size = materials_.size();
		ar & size;
		for (auto it : materials_)
		{
			std::string s = std::string();
			if (it != nullptr)
			{
				s = it->getName();
			}
	
			ar & s;
		}
	}
	
	ar & rootBoneNode_;
	ar & globalInverseTransformation_;
	
	{
		auto size = animations_.size();
		ar & size;
		for (auto it : animations_)
		{
			ar & it.first;
		}
	}
}

template<class Archive> void Model::load(Archive& ar, const unsigned int version)
{
	boost::serialization::void_cast_register<Model, IModel>(
		static_cast<Model*>(nullptr),
		static_cast<IModel*>(nullptr)
	);

	ar & name_;

	{
		std::vector<glr::glw::IMesh*>::size_type size = 0;
		ar & size;
	
		meshes_ = std::vector< glr::glw::IMesh* >();
	
		for (glmd::uint32 i=0; i < size; i++)
		{
			auto s = std::string();
			ar & s;
			
			if (s.length() == 0)
			{
				meshes_.push_back(nullptr);
			}
			else
			{
				auto mesh = openGlDevice_->getMeshManager()->getMesh( s );
				meshes_.push_back( mesh );
			}
		}
	}
	
	{
		std::vector<glr::glw::ITexture*>::size_type size = 0;
		ar & size;
	
		textures_ = std::vector< glr::glw::ITexture* >();
	
		for (glmd::uint32 i=0; i < size; i++)
		{
			auto s = std::string();
			ar & s;
			
			if (s.length() == 0)
			{
				textures_.push_back(nullptr);
			}
			else
			{
				auto texture = openGlDevice_->getTextureManager()->getTexture( s );
				textures_.push_back( texture );
			}
		}
	}
	
	{
		std::vector<glr::glw::IMaterial*>::size_type size = 0;
		ar & size;
	
		materials_ = std::vector< glr::glw::IMaterial* >();
	
		for (glmd::uint32 i=0; i < size; i++)
		{
			auto s = std::string();
			ar & s;
			
			if (s.length() == 0)
			{
				materials_.push_back(nullptr);
			}
			else
			{
				auto material = openGlDevice_->getMaterialManager()->getMaterial( s );
				materials_.push_back( material );
			}
		}
	}
	
	ar & rootBoneNode_;
	ar & globalInverseTransformation_;
	
	{
		std::map< std::string, glr::glw::IAnimation* >::size_type size = 0;
		ar & size;
		
		animations_ = std::map< std::string, glr::glw::IAnimation* >();
	
		for (glmd::uint32 i=0; i < size; i++)
		{
			auto s = std::string();
			ar & s;
			
			if (s.length() != 0)
			{
				auto animation = openGlDevice_->getAnimationManager()->getAnimation( s );
				if (animation != nullptr)
					animations_[s] = animation;
			}
		}
	}
}

}
}

BOOST_CLASS_EXPORT(glr::models::IModel)
BOOST_CLASS_EXPORT_GUID(glr::models::Model, "glr::models::Model")
