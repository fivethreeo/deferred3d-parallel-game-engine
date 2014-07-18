#include <d3d/sceneobjects/gui/Font.h>

bool Font::createAsset(const std::string &name) {
	if (!_font.loadFromFile(name))
		return false;

	return true;
}