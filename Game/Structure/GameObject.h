#pragma once

#include "../../nclgl/Maths/Vector3.h"
#include "../../nclgl/Meshes/SceneNode.h"
#include "../../nclgl/Meshes/Mesh.h"
#include "../../ResourceManagment/Resource.h"

class Renderer;

/*
  Wrapper class for scene nodes.
  Any objects to be rendered must derive from this.
  No physics aplied to this. GameObjects are static unless
  SetPosition(...) is called.
*/
class GameObject : public Resource
{
public:
	explicit GameObject(Renderer& r);
	GameObject();

	virtual ~GameObject() {}

	virtual void AddToRenderer(Renderer& r);

	void AddMesh(const string& meshLocation);
	void AddMesh(Mesh& m);
	void SetTexture(const char& texloc) const;
	void SetBumpMap(const char& bmLoc) const;

	void MakeQuad();

	inline virtual Vector3 GetPosition() const;

	virtual Vector3 GetSize() const
	{
		return scale;
	}

	SceneNode* GetSceneNode()
	{
		return &node;
	}

	Mesh GetMesh() const
	{
		return *node.GetMesh();
	}

	inline virtual void SetPosition(const Vector3& pos);
	inline virtual void SetSize(const Vector3& size);

	void SetSceneNode(SceneNode& sn)
	{
		node = sn;
	}

	virtual void Read(const string resourcename) override
	{
		this->SetName(resourcename);
	}

	virtual void ReadParams(const string params) override
	{
		Read(params);
	}

protected:
	SceneNode node;
	Vector3	scale;
	Vector3	position;
};

