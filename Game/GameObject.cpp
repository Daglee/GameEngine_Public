#include "GameObject.h"
#include "../nclgl/Renderer.h"

GameObject::GameObject(Renderer& r) : Resource()
{
	r.AddSceneNode(&node);
	this->SetSizeInBytes(sizeof(*this));
}

GameObject::GameObject() : Resource()
{
	this->SetSizeInBytes(sizeof(*this));
}

void GameObject::AddToRenderer(Renderer& r)
{
	r.AddSceneNode(&node);
}

void GameObject::AddMesh(const string& meshLocation)
{
	node.SetMesh(meshLocation);
}

void GameObject::AddMesh(Mesh& m)
{
	node.SetMesh(&m);
}

void GameObject::SetTexture(const char& texloc)
{
	node.GetMesh()->SetTexture(SOIL_load_OGL_texture(&texloc,
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
}

void GameObject::SetBumpMap(const char& bmloc)
{
	node.GetMesh()->SetBumpMap(SOIL_load_OGL_texture(&bmloc,
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
}

void GameObject::MakeQuad()
{
	Mesh* mesh = new Mesh();
	mesh = mesh->GenerateQuad();
	AddMesh(*mesh);
}

