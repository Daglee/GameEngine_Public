#pragma once

#include "OGLRenderer.h"
#include "Camera.h"
#include "SceneNode.h"
#include "Frustum.h"
#include "OBJMesh.h"
#include "Mesh.h"
#include "../ResourceManagment/Resource.h"
#include "../GameLogicFSM/FSMUnit.h"
#include "../Game/Subsystem.h"
#include "Text.h"
#include "TextMesh.h"
#include "TextRenderer.h"

#include <algorithm> //For std::sort
#include <vector>
#include <unordered_map>

#define SHADOWSIZE 2048

class DataBase;

class Renderer : public OGLRenderer, public Resource,
	public Subsystem, FSMUnit
{
public:
	Renderer(DataBase* database, Window &parent);
	~Renderer(void);

	void Update(const float& deltatime) override;
	void UpdateScene(const float& msec);

	void InitialiseScene()
	{
		SwitchToPerspective();
	}

	void RenderScene() override;


	void InitialiseLoadingScreen();
	void RenderLoadingScreen(const float current, const float total);

	void AddSceneNode(SceneNode* sn);
	void RemoveSceneNode(SceneNode* sn);

	void SetCamera(Camera* cam)
	{
		camera = cam;
	}

	Camera* GetCamera() const
	{
		return camera;
	}

	void SetLight(Light* l)
	{
		light = l;
	}

	const float GetWidth() const
	{
		return width;
	}

	const float GetHeight() const
	{
		return height;
	}


	inline void SwitchToPerspective()
	{
		projMatrix = Matrix4::Perspective(1.0f, 15000.0f,
			static_cast<float>(width) / static_cast<float>(height), 45.0f);
	}

	inline void SwitchToOrthographic()
	{
		projMatrix = Matrix4::Orthographic(-1.0f, 10000.0f,
			width / 2.0f, -width / 2.0f, height / 2.0f,
			-height / 2.0f);
	}

	void AddText(const Text& text) const
	{
		textRenderer->textbuffer.push_back(text);
	}

	Font* basicFont;

	void Read(string resourcename) override;
	void ReadParams(string params) override;

	vector<string> overlays;

	Mesh* loadingBar;
	Mesh* overlay;
protected:
	TextRenderer* textRenderer;
	Shader* textShader;

	void DrawAllText() const;
	void DrawLoadingScreen(const float current, const float total);

	void RenderOverlay();
	void DrawOverlay();

	/*<---From MSc tutorials + Slight modifications--->*/
	void BuildNodeLists(SceneNode* from);
	void SortNodeLists();
	void ClearNodeLists();
	void DrawNodes();
	void DrawNode(SceneNode* n);
	/*<---------------------------------------->*/

	void DrawShadowScene();
	void DrawCombinedScene();
	Shader* sceneShader;
	Shader* shadowShader;
	GLuint shadowTex;
	GLuint shadowFBO;

	Camera*	camera;
	Frustum frameFrustum;
	Light*	light;

	SceneNode root;
	vector<SceneNode*> transparentNodeList;
	vector<SceneNode*> nodeList;

	Window* wparent;
	float timer = 0;
	float overlayFlags[3];
};
