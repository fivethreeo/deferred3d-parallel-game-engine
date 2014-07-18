#pragma once

#include <d3d/rendering/SFMLOGL.h>

#include <d3d/assetmanager/Asset.h>

#include <array>

#include <string>

namespace d3d {
	class TextureCube : public Asset {
	private:
		GLuint _textureID;

		unsigned int _size;

	public:
		// Inherited from the Asset
		bool createAsset(const std::string &name);

		void genMipMaps();

		void bind() const {
			glBindTexture(GL_TEXTURE_CUBE_MAP, _textureID);
		}

		unsigned int getSize() const {
			return _size;
		}

		GLuint getTextureID() const {
			return _textureID;
		}

		// Asset factory
		static Asset* assetFactory() {
			return new TextureCube();
		}
	};
}