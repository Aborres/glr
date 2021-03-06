#include <sstream>
#include <utility>

#include "glw/Mesh.hpp"

#include "glw/IOpenGlDevice.hpp"

#include "common/logger/Logger.hpp"
#include "common/utilities/Macros.hpp"

#include "exceptions/GlException.hpp"

namespace glr
{
namespace glw
{

Mesh::Mesh()
{
	openGlDevice_ = nullptr;
	vertexBoneData_ = std::vector< VertexBoneData >();
	boneData_ = BoneData();
	
	vaoId_ = 0;
	
	isLocalDataLoaded_ = false;
	isVideoMemoryAllocated_ = false;
	isDirty_ = false;
	
	currentNumberOfVertices_ = 0;
	currentVerticesSpaceAllocated_ = 0;
}

Mesh::Mesh(IOpenGlDevice* openGlDevice, std::string name) : openGlDevice_(openGlDevice), name_(std::move(name))
{
	vertexBoneData_ = std::vector< VertexBoneData >();
	boneData_ = BoneData();
	
	vaoId_ = 0;
	
	isLocalDataLoaded_ = false;
	isVideoMemoryAllocated_ = false;
	isDirty_ = false;
	
	currentNumberOfVertices_ = 0;
	currentVerticesSpaceAllocated_ = 0;
}

Mesh::Mesh(IOpenGlDevice* openGlDevice,
		std::string name,
		std::vector< glm::vec3 > vertices, 
		std::vector< glm::vec3 > normals,
		std::vector< glm::vec2 > textureCoordinates,
		std::vector< glm::vec4 > colors,
		std::vector<VertexBoneData > vertexBoneData,
		BoneData boneData,
		bool initialize
	)
	: openGlDevice_(openGlDevice), name_(std::move(name)), vertices_(std::move(vertices)), normals_(std::move(normals)), textureCoordinates_(std::move(textureCoordinates)), colors_(std::move(colors)), vertexBoneData_(std::move(vertexBoneData)), boneData_(std::move(boneData))
{
	vaoId_ = 0;
	
	isLocalDataLoaded_ = false;
	isVideoMemoryAllocated_ = false;
	isDirty_ = false;
	
	currentNumberOfVertices_ = 0;
	currentVerticesSpaceAllocated_ = 0;
	
	if (initialize)
	{
		loadLocalData();
		allocateVideoMemory();
		pushToVideoMemory();
	}
}

Mesh::Mesh(IOpenGlDevice* openGlDevice,
		std::string name,
		std::vector< glm::vec3 > vertices, 
		std::vector< glm::vec3 > normals,
		std::vector< glm::vec2 > textureCoordinates,
		std::vector< glm::vec4 > colors,
		bool initialize
	)
	: openGlDevice_(openGlDevice), name_(std::move(name)), vertices_(std::move(vertices)), normals_(std::move(normals)), textureCoordinates_(std::move(textureCoordinates)), colors_(std::move(colors))
{
	vertexBoneData_ = std::vector< VertexBoneData >();
	boneData_ = BoneData();
	vaoId_ = 0;
	
	isLocalDataLoaded_ = false;
	isVideoMemoryAllocated_ = false;
	isDirty_ = false;
	
	if (initialize)
	{
		loadLocalData();
		allocateVideoMemory();
		pushToVideoMemory();
	}
}

Mesh::~Mesh()
{
	freeVideoMemory();
}

void Mesh::pushToVideoMemory()
{
	LOG_DEBUG( "loading mesh '" + name_ + "' into video memory." );

	if (vaoId_ == 0)
	{
		std::string msg = std::string( FILE_AND_LINE_NUMBER + ": Mesh has not been allocated in video memory yet." );
		LOG_ERROR( msg );
		throw exception::GlException( msg );
	}
	
	// Re-allocate memory if we need more
	if (currentVerticesSpaceAllocated_ < vertices_.size())
	{
		this->freeVideoMemory();
		this->allocateVideoMemory();
	}
	
	glBindVertexArray(vaoId_);
	
	OPENGL_CHECK_ERRORS(openGlDevice_)
	
	glBindBuffer(GL_ARRAY_BUFFER, vboIds_[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertices_.size() * sizeof(glm::vec3), &vertices_[0]);

	OPENGL_CHECK_ERRORS(openGlDevice_)

	glBindBuffer(GL_ARRAY_BUFFER, vboIds_[1]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, textureCoordinates_.size() * sizeof(glm::vec2), &textureCoordinates_[0]);

	OPENGL_CHECK_ERRORS(openGlDevice_)

	glBindBuffer(GL_ARRAY_BUFFER, vboIds_[2]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, normals_.size() * sizeof(glm::vec3), &normals_[0]);
	
	OPENGL_CHECK_ERRORS(openGlDevice_)
	
	glBindBuffer(GL_ARRAY_BUFFER, vboIds_[3]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, colors_.size() * sizeof(glm::vec4), &colors_[0]);
	
	OPENGL_CHECK_ERRORS(openGlDevice_)
	
	//std::cout << "SIZE: " << vertices_.size() << " " << sizeof(glm::ivec4) << " " << vertexBoneData_.size() << " " << sizeof(VertexBoneData) << std::endl;
	
	// TODO: We might want to not load any bone data (if there is none) and use a different shader?  Not sure best way to handle this.....?
	// Deal with not having any bones
	if (vertexBoneData_.size() == 0)
	{
		vertexBoneData_.resize( vertices_.size() );
		for (auto& b : vertexBoneData_)
		{
			b.weights = glm::vec4(0.25f);
		}
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, vboIds_[4]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertexBoneData_.size() * sizeof(VertexBoneData), &vertexBoneData_[0]);
	
	OPENGL_CHECK_ERRORS(openGlDevice_)
	
	glBindVertexArray(0);
	
	GlError err = openGlDevice_->getGlError();
	if (err.type != GL_NONE)
	{		
		std::string msg = FILE_AND_LINE_NUMBER + ": Error while pushing data for mesh '" + name_ + "' to video memory in OpenGL: " + err.name;
		LOG_ERROR( msg );
		throw exception::GlException( msg );
	}
	else
	{
		LOG_DEBUG( "Successfully pushed data for mesh '" + name_ + "' to video memory." );
	}

	// Save a backup of the number of vertices (in case the user frees local data)
	currentNumberOfVertices_ = vertices_.size();

	isDirty_ = false;
}

void Mesh::pullFromVideoMemory()
{
	// TODO: Implement
}

void Mesh::loadLocalData()
{
	isLocalDataLoaded_ = true;
}

void Mesh::freeLocalData()
{
	vertices_ = std::vector< glm::vec3 >();
	normals_ = std::vector< glm::vec3 >();
	textureCoordinates_ = std::vector< glm::vec2 >();
	colors_ = std::vector< glm::vec4 >();
	vertexBoneData_ = std::vector< VertexBoneData >();
	
	isLocalDataLoaded_ = false;
}

void Mesh::freeVideoMemory()
{
	if (vaoId_ == 0)
	{
		return;
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteVertexArrays(1, &vaoId_);
	glDeleteBuffers(5, &vboIds_[0]);
	
	vaoId_ = 0;
}

void Mesh::allocateVideoMemory()
{
	if (vaoId_ > 0)
	{
		std::string msg = std::string( FILE_AND_LINE_NUMBER + ": Mesh already has a vertex array object generated." );
		LOG_ERROR( msg );
		throw exception::GlException( msg );
	}
	
	// create our vao
	glGenVertexArrays(1, &vaoId_);
	
	glBindVertexArray(vaoId_);

	OPENGL_CHECK_ERRORS(openGlDevice_)

	// create our vbos
	glGenBuffers(5, &vboIds_[0]);
	
	OPENGL_CHECK_ERRORS(openGlDevice_)
	
	glBindBuffer(GL_ARRAY_BUFFER, vboIds_[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(glm::vec3), nullptr, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	OPENGL_CHECK_ERRORS(openGlDevice_)

	glBindBuffer(GL_ARRAY_BUFFER, vboIds_[1]);
	glBufferData(GL_ARRAY_BUFFER, textureCoordinates_.size() * sizeof(glm::vec2), nullptr, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	OPENGL_CHECK_ERRORS(openGlDevice_)

	glBindBuffer(GL_ARRAY_BUFFER, vboIds_[2]);
	glBufferData(GL_ARRAY_BUFFER, normals_.size() * sizeof(glm::vec3), nullptr, GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
	OPENGL_CHECK_ERRORS(openGlDevice_)
	
	glBindBuffer(GL_ARRAY_BUFFER, vboIds_[3]);
	glBufferData(GL_ARRAY_BUFFER, colors_.size() * sizeof(glm::vec4), nullptr, GL_STATIC_DRAW);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 0, 0);
	
	OPENGL_CHECK_ERRORS(openGlDevice_)
	
	//std::cout << "SIZE: " << vertices_.size() << " " << sizeof(glm::ivec4) << " " << vertexBoneData_.size() << " " << sizeof(VertexBoneData) << std::endl;
	
	// TODO: We might want to not load any bone data (if there is none) and use a different shader?  Not sure best way to handle this.....?
	// Deal with not having any bones
	if (vertexBoneData_.size() == 0)
	{
		vertexBoneData_.resize( vertices_.size() );
		for (auto& b : vertexBoneData_)
		{
			b.weights = glm::vec4(0.25f);
		}
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, vboIds_[4]);
	glBufferData(GL_ARRAY_BUFFER, vertexBoneData_.size() * sizeof(VertexBoneData), nullptr, GL_STATIC_DRAW);
	glEnableVertexAttribArray(4);
	glVertexAttribIPointer(4, 4, GL_INT, sizeof(VertexBoneData), (const GLvoid*)0);
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*)(sizeof(glm::ivec4)));

	OPENGL_CHECK_ERRORS(openGlDevice_)

	glBindVertexArray(0);
	
	GlError err = openGlDevice_->getGlError();
	if (err.type != GL_NONE)
	{
		// Cleanup
		freeVideoMemory();
		
		std::string msg = std::string( FILE_AND_LINE_NUMBER + ": Error while allocating video memory for mesh '" + name_ + "' in OpenGL: " + err.name);
		LOG_ERROR( msg );
		throw exception::GlException( msg );
	}
	else
	{
		LOG_DEBUG( "Successfully allocated memory for mesh '" + name_ + "'." );
	}
	
	// Set the current number of vertices allocated (so we know how much space we've taken up)
	currentVerticesSpaceAllocated_ = vertices_.size();
	
	isVideoMemoryAllocated_ = true;
}

bool Mesh::isVideoMemoryAllocated() const
{
	return isVideoMemoryAllocated_;
}

bool Mesh::isLocalDataLoaded() const
{
	return isLocalDataLoaded_;
}

bool Mesh::isDirty() const
{
	return isDirty_;
}

void Mesh::render()
{
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBindVertexArray(vaoId_);

	glDrawArrays(GL_TRIANGLES, 0, currentNumberOfVertices_);
	
	glBindVertexArray(0);
}

BoneData& Mesh::getBoneData()
{
	return boneData_;
}

const std::string& Mesh::getName() const
{
	return name_;
}

void Mesh::setName(std::string name)
{
	name_ = std::move(name);
}

void Mesh::setVertices(std::vector< glm::vec3 > vertices)
{
	vertices_ = std::move(vertices);
	isDirty_ = true;
}

void Mesh::setNormals(std::vector< glm::vec3 > normals)
{
	normals_ = std::move(normals);
	isDirty_ = true;
}

void Mesh::setTextureCoordinates(std::vector< glm::vec2 > textureCoordinates)
{
	textureCoordinates_ = std::move(textureCoordinates);
	isDirty_ = true;
}

void Mesh::setColors(std::vector< glm::vec4 > colors)
{
	colors_ = std::move(colors);
	isDirty_ = true;
}

void Mesh::setVertexBoneData(std::vector< VertexBoneData > vertexBoneData)
{
	vertexBoneData_ = std::move(vertexBoneData);
	isDirty_ = true;
}

std::vector< glm::vec3 >& Mesh::getVertices()
{
	return vertices_;
}

std::vector< glm::vec3 >& Mesh::getNormals()
{
	return normals_;
}

std::vector< glm::vec2 >& Mesh::getTextureCoordinates()
{
	return textureCoordinates_;
}

std::vector< glm::vec4 >& Mesh::getColors()
{
	return colors_;
}

std::vector< VertexBoneData >& Mesh::getVertexBoneData()
{
	return vertexBoneData_;
}

void Mesh::serialize(const std::string& filename)
{
	std::ofstream ofs(filename.c_str());
	serialize::TextOutArchive textOutArchive(ofs);
	serialize(textOutArchive);
}

void Mesh::serialize(serialize::TextOutArchive& outArchive)
{
	outArchive << *this;
}

void Mesh::deserialize(const std::string& filename)
{
	std::ifstream ifs(filename.c_str());
	serialize::TextInArchive textInArchive(ifs);
	deserialize(textInArchive);
}

void Mesh::deserialize(serialize::TextInArchive& inArchive)
{
	inArchive >> *this;
	loadLocalData();
}

}
}

BOOST_CLASS_EXPORT(glr::glw::IMesh)
BOOST_CLASS_EXPORT_GUID(glr::glw::Mesh, "glr::glw::Mesh")
