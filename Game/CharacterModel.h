#pragma once

#include "../../nclgl/SceneNode.h"

#include <string>
#include <unordered_map>

class OBJMesh;

/*
  Robot character model for players and NPCs
   - No textures
   - Colour can be applied (for teams)
   - Animations can be updated
  Uses multiple scenenodes.
*/
class CharacterModel : public SceneNode
{
public:
	CharacterModel(string filename);
	CharacterModel();
	~CharacterModel() {}

	void UpdateMesh(Mesh* m);
	void UpdateColour(const Vector4& col);
	void ReadFile(string file);
	virtual void Update(const float& msec);

	SceneNode body;
	SceneNode head;
	SceneNode leftArm;
	SceneNode rightArm;
	SceneNode leftLeg;
	SceneNode rightLeg;

protected:
	SceneNode nodes[6];
	unordered_map<string, SceneNode*> nodeMap;
};

