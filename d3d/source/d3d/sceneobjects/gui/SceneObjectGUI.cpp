#include <d3d/sceneobjects/gui/SceneObjectGUI.h>

#include <d3d/sceneobjects/gui/SceneObjectGUIRenderable.h>

SceneObjectGUI::SceneObjectGUI()
: _enabled(true)
{
	_renderMask = 0xffff;

	_layer = 1.0f;
}

void SceneObjectGUI::onAdd() {
	_bufferedInput = getScene()->getNamed("buffIn");

	assert(_bufferedInput != nullptr);

	_renderTexture.reset(new sf::RenderTexture());

	_renderTexture->create(getRenderScene()->getRenderWindow()->getSize().x, getRenderScene()->getRenderWindow()->getSize().y, false);

	sf::Texture::bind(&_renderTexture->getTexture());

	GLint textureID;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &textureID);
	_renderTextureID = static_cast<GLuint>(textureID);

	std::shared_ptr<d3d::Asset> assetRenderImage;

	getScene()->getAssetManager("shader", d3d::Shader::assetFactory)->getAsset("NONE resources/shaders/noTransformVertex.vert resources/shaders/renderImage.frag", assetRenderImage);

	_renderImageShader = std::static_pointer_cast<d3d::Shader>(assetRenderImage);
}

void SceneObjectGUI::update(float dt) {
	if (!_enabled)
		return;
}

void SceneObjectGUI::postRender() {
	if (!_enabled)
		return;

	glDisableVertexAttribArray(D3D_ATTRIB_POSITION);
	glDisableVertexAttribArray(D3D_ATTRIB_NORMAL);
	glDisableVertexAttribArray(D3D_ATTRIB_TEXCOORD);

	d3d::Shader::unbind();

	getRenderScene()->getRenderWindow()->resetGLStates();

	_renderTexture->setActive(true);
	_renderTexture->clear(sf::Color::Transparent);

	for (std::list<d3d::SceneObjectRef>::iterator it = _guiRenderables.begin(); it != _guiRenderables.end(); it++)
		static_cast<SceneObjectGUIRenderable*>(it->get())->guiRender(*_renderTexture);

	getRenderScene()->getRenderWindow()->resetGLStates();

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	// Reset OpenGL
	glEnableVertexAttribArray(D3D_ATTRIB_POSITION);
	glEnableVertexAttribArray(D3D_ATTRIB_NORMAL);
	glEnableVertexAttribArray(D3D_ATTRIB_TEXCOORD);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glFrontFace(GL_CCW);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glEnable(GL_CULL_FACE);

	glClearDepth(1.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glDepthFunc(GL_LESS);

	// Render renderTexture to gBuffer effect
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	_renderImageShader->bind();
	_renderImageShader->setShaderTexture("d3dScene", _renderTextureID, GL_TEXTURE_2D);
	_renderImageShader->setUniformv2f("d3dGBufferSizeInv", d3d::Vec2f(1.0f / static_cast<float>(getRenderScene()->_gBuffer.getWidth()), 1.0f / static_cast<float>(getRenderScene()->_gBuffer.getHeight())));
	_renderImageShader->bindShaderTextures();

	getRenderScene()->renderNormalizedQuad();

	glDisable(GL_BLEND);

	glEnable(GL_DEPTH_TEST);

	getRenderScene()->_gBuffer.bind();

	_guiRenderables.clear();
}