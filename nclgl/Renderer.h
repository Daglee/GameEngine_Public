#pragma once

#include "OGLRenderer.h"
#include "Camera.h"
#include "SceneNode.h"
#include "Frustum.h"
#include "OBJMesh.h"
#include "Mesh.h"
#include "../ResourceManagment/ResourceBase.h"
#include "../GameLogicFSM/FSMUnit.h"
#include "../Game/Subsystem.h"
#include "Text.h"
#include "TextMesh.h"

#include <algorithm> //For std::sort
#include <vector>
#include <unordered_map>

#define SHADOWSIZE 2048

class DataBase;

class Renderer : public OGLRenderer, public ResourceBase,
	public Subsystem, FSMUnit {
public:
	Renderer(DataBase* database, Window &parent);
	~Renderer(void);

	void Update(float deltatime);
	void UpdateScene(float msec);

	void InitialiseScene() {
		SwitchToPerspective();
	}
	void RenderScene();

	void InitialiseLoadingScreen() {
		loadingBar = Mesh::GenerateQuad();

		Shader* shader = new Shader(SHADERDIR"basicVertex.glsl", SHADERDIR"basicFragment.glsl");
		shader->LinkProgram();
		SetCurrentShader(shader);
		SwitchToOrthographic();
	}
	void RenderLoadingScreen(float current, float total);

	void SetCamera(Camera* cam) {
		camera = cam;
	}

	void SetLight(Light* l) {
		light = l;
	}

	void AddSceneNode(SceneNode* sn) {
		root.AddChild(sn);
		this->SetResourceSize(sizeof(*this));
	}

	void RemoveSceneNode(SceneNode* sn) {
		root.RemoveChild(sn);
		this->SetResourceSize(sizeof(*this));
	}

	float GetWidth() const 
	{
		return width;
	}

	inline void SwitchToPerspective() {
		projMatrix = Matrix4::Perspective(1.0f, 15000.0f,
			(float)width / (float)height, 45.0f);
	}

	inline void SwitchToOrthographic() {
		projMatrix = Matrix4::Orthographic(-1.0f, 10000.0f,
			width / 2.0f, -width / 2.0f, height / 2.0f,
			-height / 2.0f);
	}

	vector<Text> textbuffer;
	Font* basicFont;

	void Read(string resourcename);
	void ReadParams(string params);

	std::vector<std::string> overlays;

	Mesh* loadingBar;
	Mesh* overlay;
protected:
	Shader* textShader;

	void DrawTextBuffer();
	void DrawTextOBJ(const Text& textobj);
	void DrawLoadingScreen(float current, float total);

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
	std::vector<SceneNode*> transparentNodeList;
	std::vector<SceneNode*> nodeList;

	Window* wparent;
	float timer = 0;
	float overlayFlags[3];
};
