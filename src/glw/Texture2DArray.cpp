#include <sstream>

#include "Texture2DArray.h"

#include "../exceptions/GlException.h"
#include "../exceptions/FormatException.h"


namespace glr {
namespace glw {
	
Texture2DArray::Texture2DArray(IOpenGlDevice* openGlDevice, const std::string name, const TextureSettings settings) : openGlDevice_(openGlDevice), name_(name), settings_(settings)
{
}

Texture2DArray::Texture2DArray(std::vector<utilities::Image*> images, IOpenGlDevice* openGlDevice, const std::string name, const TextureSettings settings) : openGlDevice_(openGlDevice), name_(name), settings_(settings)
{
	bufferId_ = 0;
	
	
	if ( images.size() > 0 )
	{
		// Copy image data
		setData( images );
		allocateVideoMemory();
		pushToVideoMemory();
	}
}

Texture2DArray::~Texture2DArray()
{
}

void Texture2DArray::bind(GLuint texturePosition)
{	
	//glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, bufferId_);
	
	//bindPoint_ = openGlDevice_->bindBuffer( bufferId_ );
	//std::cout << "texture: " << name_ << " | " << bufferId_ << " | " << bindPoint_ << std::endl;
	// to unbind, we use the following
	 //glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

GLuint Texture2DArray::getBufferId()
{
	return bufferId_;
}

GLuint Texture2DArray::getBindPoint()
{
	return bindPoint_;
}

void Texture2DArray::setData(std::vector<utilities::Image*> images)
{
	images_ = std::vector<utilities::Image>();
	
	for ( auto image : images )
	{
		images_.push_back( *image );
	}
}

std::vector<utilities::Image>& Texture2DArray::getData()
{
	return images_;
}

void Texture2DArray::pushToVideoMemory()
{
	glmd::uint32 index = 0;
	GLint internalFormat = utilities::Format::FORMAT_UNKNOWN;
	
	if (images_.size() > 0)
	{
		internalFormat = utilities::getOpenGlImageFormat(images_[0].format);
		if (internalFormat == utilities::Format::FORMAT_UNKNOWN)
		{
			std::string msg = std::string( "Texture2DArray::pushToVideoMemory: Unknown image format." );
			LOG_ERROR( msg );
			throw exception::FormatException( msg );
		}
	}
	

	for ( auto& image : images_ )
	{
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, index, image.width, image.height, 1, internalFormat, GL_UNSIGNED_BYTE, &(image.data[0]));
		index++;
	}

	// error check
	GlError err = openGlDevice_->getGlError();
	if (err.type != GL_NONE)
	{
		// Cleanup
		freeVideoMemory();
		
		std::string msg = std::string( "Error while loading texture '" + name_ + "' in OpenGL: " + err.name);
		LOG_ERROR( msg );
		throw exception::GlException( msg );
	}
	else
	{
		LOG_DEBUG( "Successfully loaded texture 2d array." );
	}
}

void Texture2DArray::pullFromVideoMemory()
{
	
	// TODO: implement
	/*
	GLfloat* pixels = new GLfloat[size * size];
	glGetTexImage( GL_TEXTURE_2D, 0, GL_RED, GL_FLOAT, pixels );
	
	pixelDataVector.resize( size * size );
	for ( int i = 0; i < size * size; i++ )
	{
	  pixelDataVector[i] =  (float) pixels[i];
	}
	*/
}

void Texture2DArray::freeLocalData()
{
	// Clear up the data, but leave the width, height, and format unchanged
	for ( auto& image : images_ )
	{
		image.data = std::vector<char>();
	}
}

void Texture2DArray::freeVideoMemory()
{
	if (bufferId_ == 0)
	{
		std::string msg = std::string( "Cannot free video memory - buffer does not exist for texture.");
		LOG_ERROR( msg );
		throw exception::GlException( msg );
	}
	
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	glDeleteTextures(images_.size(), &bufferId_);
	
	bufferId_ = 0;
}

void Texture2DArray::allocateVideoMemory()
{
	if (bufferId_ != 0)
	{
		std::string msg = std::string( "Cannot allocate video memory - buffer already exists for texture.");
		LOG_ERROR( msg );
		throw exception::GlException( msg );
	}
	
	if (images_.size() == 0)
	{
		std::string msg = std::string( "Allocating video memory, but there is no data.");
		LOG_WARN( msg );
	}
	
	if (!areImagesSameFormat())
	{
		std::string msg = std::string( "Cannot allocate video memory - images are not all the same format.");
		LOG_ERROR( msg );
		throw exception::GlException( msg );
	}
	
	// TODO: check images are all the same size?
	
	glGenTextures(images_.size(), &bufferId_);

	glBindTexture(GL_TEXTURE_2D_ARRAY, bufferId_);
	
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, settings_.textureWrapS);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, settings_.textureWrapT);
	
	// Get the OpenGL format for the images
	auto& image = images_[0];
	GLint internalFormat = utilities::getOpenGlImageFormat(image.format);
	if (internalFormat == utilities::Format::FORMAT_UNKNOWN)
	{
		std::string msg = std::string( "Texture2DArray::allocateVideoMemory: Unknown image format." );
		LOG_ERROR( msg );
		throw exception::FormatException( msg );
	}
	
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, internalFormat,  image.width, image.height, images_.size(), 0, internalFormat, GL_UNSIGNED_BYTE, nullptr);
}

bool Texture2DArray::areImagesSameFormat()
{
	if (images_.size() == 0)
		return true;
	
	utilities::Format format = images_[0].format;
	
	for ( auto& image : images_ )
	{
		if (format != image.format)
			return false;
	}
	
	return true;
}

}
}