#include <d3d/sceneobjects/gui/SceneObjectGUIRenderable.h>

#include <d3d/sceneobjects/gui/SceneObjectGUI.h>

void SceneObjectGUIRenderable::onAdd() {
	_gui = getScene()->getNamed("gui");

	assert(_gui.isAlive());

	guiOnAdd();
}

void SceneObjectGUIRenderable::deferredRender() {
	assert(_gui.isAlive());

	static_cast<SceneObjectGUI*>(_gui.get())->_guiRenderables.push_back(this);
}