#pragma once

#include <d3d/assetmanager/Asset.h>

#include <SFML/Graphics.hpp>

class Font : public d3d::Asset {
private:
public:
	sf::Font _font;

	// Inherited from Asset
	bool createAsset(const std::string &name);

	// Asset factory
	static Asset* assetFactory() {
		return new Font();
	}
};