#pragma once

#include <d3d/rendering/SFMLOGL.h>

#include <d3d/assetmanager/Asset.h>

#include <string>

namespace d3d {
	class Texture2DArray : public Asset {
	private:
		GLuint _textureID;

		sf::Vector2u _size;
		unsigned int _layers;

	public:
		// Inherited from the Asset
		bool createAsset(const std::string &name);

		void genMipMaps();

		void bind() const {
			glBindTexture(GL_TEXTURE_2D_ARRAY, _textureID);
		}

		unsigned int getWidth() const {
			return _size.x;
		}

		unsigned int getHeight() const {
			return _size.y;
		}

		size_t getLayers() const {
			return _layers;
		}

		GLuint getTextureID() const {
			return _textureID;
		}

		// Asset factory
		static Asset* assetFactory() {
			return new Texture2DArray();
		}
	};
}