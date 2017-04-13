#include "CharacterModel.h"

#include "../../nclgl/OBJMesh.h"
#include "../ResourceManagment/Log.h"

#include <istream>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>

/*
  Dimensions for different scenenodes for this character model 
  are read from the passed in file.
*/
CharacterModel::CharacterModel(string filename)
{
	body		= SceneNode(nullptr, Vector4());
	head		= SceneNode(nullptr, Vector4());
	leftArm		= SceneNode(nullptr, Vector4());
	rightArm	= SceneNode(nullptr, Vector4());
	leftLeg		= SceneNode(nullptr, Vector4());
	rightLeg	= SceneNode(nullptr, Vector4());

	nodeMap["body"]		= &body;
	nodeMap["head"]		= &head;
	nodeMap["leftArm"]	= &leftArm;
	nodeMap["rightArm"] = &rightArm;
	nodeMap["leftLeg"]	= &leftLeg;
	nodeMap["rightLeg"]	= &rightLeg;

	ReadFile(filename);

	AddChild(&body);
	body.AddChild(&head);
	body.AddChild(&leftArm);
	body.AddChild(&rightArm);
	body.AddChild(&leftLeg);
	body.AddChild(&rightLeg);

	for each (std::pair<std::string, SceneNode*> nodepair in nodeMap) {
		nodepair.second->AutoSetBoundingRadius();
	}
}

CharacterModel::CharacterModel()
{
	body		= SceneNode(nullptr, Vector4());
	head		= SceneNode(nullptr, Vector4());
	leftArm		= SceneNode(nullptr, Vector4());
	rightArm	= SceneNode(nullptr, Vector4());
	leftLeg		= SceneNode(nullptr, Vector4());
	rightLeg	= SceneNode(nullptr, Vector4());

	nodeMap["body"]		= &body;
	nodeMap["head"]		= &head;
	nodeMap["leftArm"]	= &leftArm;
	nodeMap["rightArm"] = &rightArm;
	nodeMap["leftLeg"]	= &leftLeg;
	nodeMap["rightLeg"] = &rightLeg;

	AddChild(&body);
	body.AddChild(&head);
	body.AddChild(&leftArm);
	body.AddChild(&rightArm);
	body.AddChild(&leftLeg);
	body.AddChild(&rightLeg);

}

/*
  File should have sections in this format 
  for each node.
  Example in ../../CharacterModel/PlayerModel.txt

  line 0: node name			-- head
  line 1: node colour		-- colour 1 0 1 1
  line 2: node scale		-- scale 5 23 1
  line 3: node transform	-- transform 0 52 12
*/
void CharacterModel::ReadFile(string filename)
{
	std::ifstream file(filename);
	string line;

	for (int i = 0; i < 6; ++i) {
		getline(file, line);
		SceneNode* node;

		auto it = nodeMap.find(line.c_str()); //Find the pointer to the correct node
		if (it == nodeMap.end()) {
			//Probably a typo in the file if its in the unordered_map
			Log::Error("Node " + line + " not found in map.");
		}

		node = it->second;

		vector<vector<string>> tokens;
		for (int i = 0; i < 3; ++i) {
			getline(file, line);

			vector<string> lineTokens = Log::tokenise(line);
			tokens.push_back(lineTokens);
		}

		vector<string> colourLine = tokens.at(0);
		Vector4 colour(stof(colourLine.at(1)), stof(colourLine.at(2)),
						stof(colourLine.at(3)), stof(colourLine.at(4)));

		vector<string> scaleLine = tokens.at(1);
		Vector3 scale(stof(scaleLine.at(1)), stof(scaleLine.at(2)),
						stof(scaleLine.at(3)));

		vector<string> transformLine = tokens.at(2);
		Vector3 transform(stof(transformLine.at(1)), stof(transformLine.at(2)),
				stof(transformLine.at(3)));

		node->SetColour(colour);
		node->SetModelScale(scale);
		node->SetTransform(Matrix4::Translation(transform));
	}

	file.close();
}

//Applies to all nodes
void CharacterModel::UpdateMesh(Mesh* m)
{
	for each (std::pair<std::string, SceneNode*> nodepair in nodeMap) {
		nodepair.second->SetMesh(m);
	}
}

//Applies to all nodes
void CharacterModel::UpdateColour(const Vector4& col)
{
	for each (std::pair<std::string, SceneNode*> nodepair in nodeMap) {
		nodepair.second->SetColour(col);
	}
}

//For animations (if any are added in the future...
void CharacterModel::Update(const float& msec)
{
	SceneNode::Update(msec);
}
