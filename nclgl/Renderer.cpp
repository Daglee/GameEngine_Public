#include "Renderer.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent), ResourceBase() {
	light = new Light(Vector3(500000, 1000000, 0), Vector4(1, 1, 1, 1), 10000000.0f);

	currentShader = new Shader(SHADERDIR"PerPixelVertex.glsl",
		SHADERDIR"PerPixelFragment.glsl");
	textShader = new Shader(SHADERDIR"TexturedVertex.glsl", 
		SHADERDIR"TexturedFragment.glsl");
	currentShader->LinkProgram();

	projMatrix = Matrix4::Perspective(1.0f, 15000.0f,
		(float)width / (float)height, 45.0f);

	//root = new SceneNode();
	root.SetTransform(Matrix4::Translation(Vector3(0, 0, 0)));
	basicFont = new Font(SOIL_load_OGL_texture(TEXTUREDIR"tahoma.tga",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT), 16, 16);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	init = true;
	this->SetResourceSize(sizeof(*this));

	wparent = &parent;
}

Renderer::Renderer() : OGLRenderer(*(new Window())), ResourceBase() {
	light = new Light(Vector3(750, 100, 750), Vector4(1, 1, 1, 1), 1000000.0f);

	currentShader = new Shader(SHADERDIR"PerPixelVertex.glsl",
		SHADERDIR"PerPixelFragment.glsl");
	textShader = new Shader(SHADERDIR"TexturedVertex.glsl",
		SHADERDIR"TexturedFragment.glsl");
	currentShader->LinkProgram();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	projMatrix = Matrix4::Perspective(1.0f, 15000.0f,
		(float)width / (float)height, 45.0f);
	basicFont = new Font(SOIL_load_OGL_texture(TEXTUREDIR"tahoma.tga",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT), 16, 16);

	init = true;
	this->SetResourceSize(sizeof(*this));
}

Renderer::~Renderer(void) {
	delete camera;
	delete light;
	delete basicFont;
	delete textShader;
}

void Renderer::Update(float deltatime) {
	updateTimer.StartTimer();

	UpdateScene(deltatime);
	RenderScene();

	updateTimer.StopTimer();
}

void Renderer::RenderLoadingScreen(float current, float total) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(currentShader->GetProgram());

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"currentProgress"), current);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"finalValue"), total);

	UpdateShaderMatrices();
	DrawLoadingScreen(current, total);

	glUseProgram(0);
	SwapBuffers();
}

void Renderer::RenderScene() {
	BuildNodeLists(&root);
	SortNodeLists();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(currentShader->GetProgram());

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"diffuseTex"), 0);

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(),
		"cameraPos"), 1, (float*)&camera->GetPosition());

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"bumpTex"), 1);

	UpdateShaderMatrices();
	SetShaderLight(*light);

	DrawNodes();
	if (!textbuffer.empty()) {
		DrawTextBuffer();
		textbuffer.clear();
	}

	glUseProgram(0);

	SwapBuffers();
	ClearNodeLists();
}

void Renderer::UpdateScene(float msec) {
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
	root.Update(msec);
}

void Renderer::DrawLoadingScreen(float current, float total) {
	float progress = (current / total);
	Vector3 scale((float)width * progress, 100, 100);
	Vector3 pos(-((float)width * 0.5) + scale.x, -((float)height * 0.5) + scale.y, 0);

	modelMatrix.SetPositionVector(pos);
	modelMatrix.SetScalingVector(scale);

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(),
		"modelMatrix"), 1, false, *&modelMatrix.values);

	loadingBar->Draw();
}

void Renderer::DrawNode(SceneNode* n) {
	if (n->GetMesh()) {
		Matrix4 transform = n->GetWorldTransform()*
							Matrix4::Scale(n->GetModelScale());
		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(),
			"modelMatrix"), 1, false, (float*)&transform);

		glUniform4fv(glGetUniformLocation(currentShader->GetProgram(),
				"nodeColour"), 1,(float*)&n->GetColour());

		glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
				"useTexture"), (int)n->GetMesh()->GetTexture());

		n->Draw();
	}
}

void Renderer::DrawTextBuffer() {
	//Store the old values (so we can switch back).
	Matrix4 oldViewmatrix = viewMatrix;
	Matrix4 oldProjmatrix = projMatrix;

	//Use text shader
	Shader* oldshader	= currentShader;
	currentShader		= textShader;
	currentShader		->LinkProgram();

	glUseProgram(currentShader->GetProgram());
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	for each (Text textobj in textbuffer) {
		DrawTextOBJ(textobj);
	}

	//Revert to the old matrices
	viewMatrix = oldViewmatrix;
	projMatrix = oldProjmatrix;

	//Switch shader back
	currentShader	= oldshader;
	currentShader	->LinkProgram();

	glUseProgram(currentShader->GetProgram());
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

/*<Author: Richard Davison--->*/
void Renderer::DrawTextOBJ(const Text& textobj) {
	TextMesh mesh(textobj.text, *basicFont);

	//This just does simple matrix setup to render in either perspective or
	//orthographic mode, there's nothing here that's particularly tricky.
	if (textobj.perspective) {
		modelMatrix = Matrix4::Translation(textobj.position) *
			Matrix4::Scale(Vector3(textobj.size, textobj.size, 1));

		viewMatrix = camera->BuildViewMatrix();
		projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
	}
	else {
		//In ortho mode, we subtract the y from the height, so that a height of 0
		//is at the top left of the screen, which is more intuitive
		modelMatrix = Matrix4::Translation(Vector3(textobj.position.x,
			height - textobj.position.y,
			textobj.position.z))
			* Matrix4::Scale(Vector3(textobj.size, textobj.size, 1));

		viewMatrix.ToIdentity();
		projMatrix = Matrix4::Orthographic(-1.0f, 1.0f, (float)width, 0.0f, (float)height, 0.0f);
	}

	//Either way, we update the matrices, and draw the mesh
	UpdateShaderMatrices();
	mesh.Draw();
}
/*<Author: by Richard Davison>*/

void Renderer::DrawNodes() {
	for (vector<SceneNode*>::const_iterator i = nodeList.begin();
		i != nodeList.end(); ++i) {
		DrawNode((*i));
	}

	for (vector<SceneNode*>::const_reverse_iterator i = transparentNodeList.rbegin();
		i != transparentNodeList.rend(); ++i) {
		DrawNode((*i));
	}
}

void Renderer::BuildNodeLists(SceneNode* from) {
	if (frameFrustum.InsideFrustum(*from)) {
		Vector3 dir = from->GetWorldTransform().GetPositionVector() -
			camera->GetPosition();
		from->SetCameraDistance(Vector3::Dot(dir, dir));

		if (from->GetColour().w < 1.0f) {
			transparentNodeList.push_back(from);
		}
		else {
			nodeList.push_back(from);
		}
	}

	for (vector<SceneNode*>::const_iterator i =
		from->GetChildIteratorStart();
		i != from->GetChildIteratorEnd(); ++i) {
		BuildNodeLists((*i));
	}
	this->SetResourceSize(sizeof(*this));
}

void Renderer::SortNodeLists() {
	std::sort(transparentNodeList.begin(), 
		transparentNodeList.end(),
		SceneNode::CompareByCameraDistance);
	std::sort(nodeList.begin(), 
		nodeList.end(),
		SceneNode::CompareByCameraDistance);
}

void Renderer::ClearNodeLists() {
	transparentNodeList.clear();
	nodeList.clear();
	this->SetResourceSize(sizeof(*this));
}

void Renderer::Read(string resourcename) {
	this->SetResourceName(resourcename);
}

void Renderer::ReadParams(string params) {
	std::istringstream iss(params);
	vector<string> tokens{ istream_iterator<string>{iss},
		istream_iterator<string>{} };
	string name = tokens.at(0);
	string winName = tokens.at(1);
}