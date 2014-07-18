#pragma once

#include <d3d/scene/Scene.h>

class SceneObjectGUIRenderable : public d3d::SceneObject {
private:
	d3d::SceneObjectRef _gui;

public:
	virtual void guiOnAdd() {};
	virtual void guiRender(sf::RenderTexture &renderTexture) {};

	// Inherited from SceneObject
	void onAdd();
	void deferredRender();

	const d3d::SceneObjectRef &getGUI() const {
		return _gui;
	}
};