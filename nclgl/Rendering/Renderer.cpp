#include "Renderer.h"

#include "../ResourceManagment/DataBase.h"

Renderer::Renderer(DataBase* database, Window &parent) :
	OGLRenderer(parent), Resource(), FSMUnit("Renderer", database)
{
	light = new Light(Vector3(700, 1000, -700), Vector4(1, 1, 1, 1), 30000.0f);

	sceneShader = new Shader(SHADERDIR"shadowscenevert.glsl", SHADERDIR"shadowscenefrag.glsl");
	shadowShader = new Shader(SHADERDIR"shadowvert.glsl", SHADERDIR"shadowfrag.glsl");
	sceneShader->LinkProgram();
	shadowShader->LinkProgram();

	SetCurrentShader(sceneShader);

	textShader = new Shader(SHADERDIR"TexturedVertex.glsl",
		SHADERDIR"TexturedFragment.glsl");

	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,
		GL_COMPARE_R_TO_TEXTURE);

	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, shadowTex, 0);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glEnable(GL_MULTISAMPLE);

	projMatrix = Matrix4::Perspective(1.0f, 15000.0f,
		static_cast<float>(width) / static_cast<float>(height), 45.0f);

	root.SetTransform(Matrix4::Translation(Vector3(0, 0, 0)));
	basicFont = new Font(SOIL_load_OGL_texture(TEXTUREDIR"tahoma.tga",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT), 16, 16);

	glEnable(GL_DEPTH_TEST);

	init = true;
	this->SetSizeInBytes(sizeof(*this));

	overlay = Mesh::GenerateQuad();
	overlay->SetTexture(0);

	overlays.push_back("../Data/Textures/Movement.tga");
	overlays.push_back("../Data/Textures/Rotation.tga");
	overlays.push_back("../Data/Textures/Shoot.tga");

	properties->insert({ "overlay1", &overlayFlags[0] });
	properties->insert({ "overlay2", &overlayFlags[1] });
	properties->insert({ "overlay3", &overlayFlags[2] });
	properties->insert({ "timer", &timer });

	wparent = &parent;

	textRenderer = new TextRenderer(this);
}

Renderer::~Renderer(void)
{
	delete camera;
	delete light;
	delete basicFont;
	delete textShader;
	delete sceneShader;
	delete shadowShader;
	delete textRenderer;
}

void Renderer::Update(const float& deltatime)
{
	updateTimer.StartTimer();

	UpdateScene(deltatime);
	RenderScene();
	DrawOverlay();

	updateTimer.StopTimer();
}

void Renderer::InitialiseLoadingScreen()
{
	loadingBar = Mesh::GenerateQuad();

	Shader* shader = new Shader(SHADERDIR"basicVertex.glsl", SHADERDIR"basicFragment.glsl");
	shader->LinkProgram();
	SetCurrentShader(shader);
	SwitchToOrthographic();
}

void Renderer::RenderLoadingScreen(const float current, const float total)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(currentShader->GetProgram());

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"currentProgress"), static_cast<int>(current));
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"finalValue"), static_cast<int>(total));

	UpdateShaderMatrices();
	DrawLoadingScreen(current, total);

	SwapBuffers();
}

void Renderer::RenderScene()
{
	BuildNodeLists(&root);
	SortNodeLists();

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	DrawShadowScene();
	DrawCombinedScene();

	if (!textRenderer->textbuffer.empty())
	{
		DrawAllText();
		textRenderer->textbuffer.clear();
	}

	SwapBuffers();
	ClearNodeLists();
}

void Renderer::DrawShadowScene()
{
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	SetCurrentShader(shadowShader);

	viewMatrix = Matrix4::BuildViewMatrix(light->GetPosition(), Vector3(0, 0, 0));
	textureMatrix = biasMatrix *(projMatrix * viewMatrix);

	UpdateShaderMatrices();
	DrawNodes();

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawCombinedScene()
{
	SetCurrentShader(sceneShader);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"bumpTex"), 1);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"shadowTex"), 2);
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(),
		"cameraPos"), 1, (float*)&camera->GetPosition());

	SetShaderLight(*light);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, shadowTex);

	viewMatrix = camera->BuildViewMatrix();
	UpdateShaderMatrices();

	DrawNodes();
}

void Renderer::UpdateScene(const float& msec)
{
	++timer;

	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
	root.Update(msec);
}

void Renderer::DrawLoadingScreen(const float current, const float total)
{
	const float progress = (current / total);
	const Vector3 scale(static_cast<float>(width) * progress, 100, 100);
	const Vector3 pos(-(static_cast<float>(width) * 0.5f) + scale.x, -(static_cast<float>(height) * 0.5f) + scale.y, 0);

	modelMatrix.SetPositionVector(pos);
	modelMatrix.SetScalingVector(scale);

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(),
		"modelMatrix"), 1, false, *&modelMatrix.values);

	loadingBar->Draw();
}

void Renderer::RenderOverlay()
{
	int overlayIndex = 0;
	for each (float flag in overlayFlags)
	{
		if (flag)
		{
			overlay->SetTexture(SOIL_load_OGL_texture(overlays[overlayIndex].c_str(),
				SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
			glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
				"useTexture"), overlay->GetTexture());
			DrawOverlay();
		}

		++overlayIndex;
	}

	overlay->SetTexture(0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"useTexture"), 0);
}

void Renderer::DrawOverlay()
{
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"useOverlay"), 1);

	modelMatrix.SetScalingVector(Vector3(width * 0.4f, height * 0.9f, 0));
	modelMatrix.SetPositionVector(Vector3(0, -300, 0));

	SwitchToOrthographic();
	UpdateShaderMatrices();
	overlay->Draw();
	SwitchToPerspective();
}

void Renderer::DrawNode(SceneNode* n)
{
	if (n->GetMesh())
	{
		modelMatrix = n->GetWorldTransform()*
			Matrix4::Scale(n->GetModelScale());

		Matrix4 tempMatrix = textureMatrix * modelMatrix;

		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram()
			, "textureMatrix"), 1, false, *&tempMatrix.values);

		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(),
			"modelMatrix"), 1, false, (float*)&modelMatrix);

		glUniform4fv(glGetUniformLocation(currentShader->GetProgram(),
			"nodeColour"), 1, (float*)&n->GetColour());

		glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
			"useTexture"), (int)n->GetMesh()->GetTexture());

		n->Draw();
	}
}

void Renderer::DrawAllText() const
{
	textRenderer->DrawTextBuffer();

	currentShader->LinkProgram();

	glUseProgram(currentShader->GetProgram());
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);
}

void Renderer::DrawNodes()
{
	for (vector<SceneNode*>::const_iterator i = nodeList.begin();
		i != nodeList.end(); ++i)
	{
		DrawNode((*i));
	}

	for (vector<SceneNode*>::const_reverse_iterator i = transparentNodeList.rbegin();
		i != transparentNodeList.rend(); ++i)
	{
		DrawNode((*i));
	}
}

void Renderer::BuildNodeLists(SceneNode* from)
{
	if (frameFrustum.InsideFrustum(*from))
	{
		const Vector3 dir = from->GetWorldTransform().GetPositionVector() -
			camera->GetPosition();
		from->SetCameraDistance(Vector3::Dot(dir, dir));

		if (from->GetColour().w < 1.0f)
		{
			transparentNodeList.push_back(from);
		}
		else
		{
			nodeList.push_back(from);
		}
	}

	for (vector<SceneNode*>::const_iterator i =
		from->GetChildIteratorStart();
		i != from->GetChildIteratorEnd(); ++i)
	{
		BuildNodeLists((*i));
	}

	this->SetSizeInBytes(sizeof(*this));
}

void Renderer::SortNodeLists()
{
	std::sort(transparentNodeList.begin(),
		transparentNodeList.end(),
		SceneNode::CompareByCameraDistance);

	std::sort(nodeList.begin(),
		nodeList.end(),
		SceneNode::CompareByCameraDistance);
}

void Renderer::ClearNodeLists()
{
	transparentNodeList.clear();
	nodeList.clear();
	this->SetSizeInBytes(sizeof(*this));
}

void Renderer::AddSceneNode(SceneNode* sn)
{
	root.AddChild(sn);
	this->SetSizeInBytes(sizeof(*this));
}

void Renderer::RemoveSceneNode(SceneNode* sn)
{
	root.RemoveChild(sn);
	this->SetSizeInBytes(sizeof(*this));
}

void Renderer::Read(const string resourcename)
{
	SetName(resourcename);
}

void Renderer::ReadParams(const string params)
{
	std::istringstream iss(params);
	vector<string> tokens{ istream_iterator<string>{iss},
		istream_iterator<string>{} };
	const string name = tokens.at(0);
	const string winName = tokens.at(1);
	SetName(name);
}