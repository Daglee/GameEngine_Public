#include "GameObject.h"
#include "../../nclgl/Renderer.h"

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

void GameObject::SetTexture(const char& texloc) const
{
	node.GetMesh()->SetTexture(SOIL_load_OGL_texture(&texloc,
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
}

void GameObject::SetBumpMap(const char& bmloc) const
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

void GameObject::SetPosition(const Vector3& pos)
{
	position = pos;
	node.SetTransform(pos);
}

void GameObject::SetSize(const Vector3& size)
{
	scale = size;
	node.SetModelScale(scale);
	node.AutoSetBoundingRadius();
}

Vector3 GameObject::GetPosition() const
{
	return position;
}