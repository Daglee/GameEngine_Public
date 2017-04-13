#include "GameObject.h"
#include "../../nclgl/Renderer.h"

GameObject::GameObject(Renderer& r) : ResourceBase()
{
	node		= SceneNode();
	scale		= Vector3();
	position	= Vector3();
	r.AddSceneNode(&node);

	this->SetResourceSize(sizeof(*this));
}

GameObject::GameObject() : ResourceBase()
{
	node		= SceneNode();
	scale		= Vector3();
	position	= Vector3();

	this->SetResourceSize(sizeof(*this));
}

void GameObject::UpdateRenderer(Renderer& r)
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
