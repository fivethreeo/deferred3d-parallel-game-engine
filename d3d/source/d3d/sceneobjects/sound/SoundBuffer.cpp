#include <d3d/sceneobjects/sound/SoundBuffer.h>

using namespace d3d;

bool SoundBuffer::createAsset(const std::string &name) {
	if (!_buffer.loadFromFile(name))
		return false;

	return true;
}