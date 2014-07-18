#pragma once

#include <d3d/scene/RenderScene.h>

#include <d3d/sceneobjects/input/SceneObjectBufferedInput.h>

class SceneObjectGUI : public d3d::SceneObject {
private:
	d3d::SceneObjectRef _bufferedInput;

	std::list<d3d::SceneObjectRef> _guiRenderables;

	std::shared_ptr<sf::RenderTexture> _renderTexture;

	std::shared_ptr<d3d::Shader> _renderImageShader;

	GLuint _renderTextureID;

public:
	bool _enabled;

	SceneObjectGUI();

	// Inherited from SceneObject
	void onAdd();
	void update(float dt);
	void postRender();

	sf::RenderTexture* getRenderTexture() const {
		return _renderTexture.get();
	}

	SceneObjectGUI* copyFactory() {
		return new SceneObjectGUI(*this);
	}

	friend class SceneObjectGUIRenderable;
};

