#include "LevelConfiguration.h"

#include "../../Structure/PhysicsObject.h"
#include "../../Structure/GameObject.h"
#include "../../Levels/Entity Configuration/EntityConfiguration.h"
#include "../../Audio/AudioManager.h"
#include "../ResourceManagement/Utilities/Log.h"
#include "../ResourceManagement/Database/DataBase.h"
#include "../GameLogicFSM/FSMManager.h"

LevelConfiguration::LevelConfiguration(DataBase* database, const string directory)
{
	this->database = database;

	string physicsObjectsConfigFile = directory + "/PhysicsObjects.txt";
	string gameObjectsConfigFile = directory + "/GameObjects.txt";
	string gamelogicFile = directory + "/GameLogic.txt";
	string musicListFile = directory + "/Music.txt";

	configFiles.push_back(ConfigurationFile(physicsObjectsConfigFile, std::bind(&LevelConfiguration::ReadPhysicsObject, this, physicsObjectsConfigFile)));
	configFiles.push_back(ConfigurationFile(gameObjectsConfigFile, std::bind(&LevelConfiguration::ReadGameObject, this, gameObjectsConfigFile)));
	configFiles.push_back(ConfigurationFile(gamelogicFile, std::bind(&LevelConfiguration::InitialiseGameLogic, this, gamelogicFile)));
	configFiles.push_back(ConfigurationFile(musicListFile, std::bind(&LevelConfiguration::InitialiseMusic, this, musicListFile)));
}

void LevelConfiguration::ConfigureFromAllExistingFiles() const
{
	for each (ConfigurationFile configFile in configFiles)
	{
		if (Log::FileExists(configFile.file))
		{
			configFile.configurator(configFile.file);
		}
	}
}

void LevelConfiguration::ReadPhysicsObject(const string filename)
{
	ifstream file(filename);
	const int numObjectsToRead = ParseNumberOfEntitiesInFile(file);

	for (int i = 0; i < numObjectsToRead; ++i)
	{
		string resourceName;

		getline(file, resourceName);
		PhysicsObject* entity = static_cast<PhysicsObject*>(database->GetTable("PhysicsObjects")->GetResources()->Find(resourceName));

		ConfigureAllEntityAttributes(file, entity);
	}
}

void LevelConfiguration::ReadGameObject(const string filename)
{
	ifstream file(filename);

	const int numItems = ParseNumberOfEntitiesInFile(file);

	for (int i = 0; i < numItems; ++i)
	{
		string resourceName;

		getline(file, resourceName);
		GameObject* entity = static_cast<GameObject*>(database->GetTable("GameObjects")->GetResources()->Find(resourceName));

		ConfigureAllEntityAttributes(file, entity);
	}
}

const int LevelConfiguration::ParseNumberOfEntitiesInFile(ifstream& file) const
{
	string attribute;
	getline(file, attribute);

	const int numObjectsToRead = atoi(attribute.c_str());

	return numObjectsToRead;
}

void LevelConfiguration::InitialiseGameLogic(const string filename) const
{
	ifstream file(filename);
	string line;

	FSMManager* fsmManager = static_cast<FSMManager*>(database->GetTable("GFSMManager")->GetResources()->Find("GFSMManager"));

	while (getline(file, line))
	{
		vector<std::string> tokens = Log::tokenise(line);
		const string FSMName = tokens.at(0);
		const string FSMConfigurationFile = tokens.at(1);

		fsmManager->LateBuildFSM(FSMName, FSMConfigurationFile);
	}
}

void LevelConfiguration::InitialiseMusic(const string filename) const
{
	ifstream file(filename);
	string songFileName;

	while (getline(file, songFileName))
	{
		AudioManager::GetInstance()->AddBackgroundSound(songFileName);
	}
}

void LevelConfiguration::ConfigureAllEntityAttributes(std::ifstream& file, GameObject* entity) const
{
	string attribute;

	const vector<string> emptyAttribute;
	EntityConfiguration configuration(emptyAttribute, entity);

	while (getline(file, attribute))
	{
		const vector<string> tokens = Log::tokenise(attribute);

		configuration.SetAttributeTokens(tokens);
		configuration.ConfigureEntityWithAttribute();

		if (configuration.IsFinished())
		{
			break;
		}
	}
}