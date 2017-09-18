#include "LevelConfiguration.h"
#include "PhysicsObject.h"
#include "GameObject.h"
#include "EntityConfiguration.h"
#include "../ResourceManagment/Log.h"
#include "../ResourceManagment/DataBase.h"

LevelConfiguration::LevelConfiguration(DataBase* database, std::string directory)
{
	this->database = database;

	std::string physicsObjectsConfigFile = directory + "/PhysicsObjects.txt";
	std::string gameObjectsConfigFile = directory + "/GameObjects.txt";
	std::string gamelogicFile = directory + "/GameLogic.txt";
	std::string musicListFile = directory + "/Music.txt";

	configFiles.push_back(ConfigurationFile(physicsObjectsConfigFile, std::bind(&LevelConfiguration::ReadPhysicsObject, this, physicsObjectsConfigFile)));
	configFiles.push_back(ConfigurationFile(gameObjectsConfigFile, std::bind(&LevelConfiguration::ReadGameObject, this, gameObjectsConfigFile)));
	configFiles.push_back(ConfigurationFile(gamelogicFile, std::bind(&LevelConfiguration::InitialiseGameLogic, this, gamelogicFile)));
	configFiles.push_back(ConfigurationFile(musicListFile, std::bind(&LevelConfiguration::InitialiseMusic, this, musicListFile)));
}

void LevelConfiguration::ConfigureFromAllExistingFiles()
{
	for each (ConfigurationFile configFile in configFiles)
	{
		if (Log::FileExists(configFile.file))
		{
			configFile.configurator(configFile.file);
		}
	}
}

void LevelConfiguration::ReadPhysicsObject(std::string filename)
{
	std::ifstream file(filename);
	int numObjectsToRead = ParseNumberOfEntitiesInFile(file);

	for (int i = 0; i < numObjectsToRead; ++i)
	{
		string resourceName;

		getline(file, resourceName);
		PhysicsObject* entity = database->PhysicsObjects->Find(resourceName);

		ConfigureAllEntityAttributes(file, entity);
	}
}

void LevelConfiguration::ReadGameObject(std::string filename)
{
	std::ifstream file(filename);

	int numItems = ParseNumberOfEntitiesInFile(file);

	for (int i = 0; i < numItems; ++i)
	{
		string resourceName;

		getline(file, resourceName);
		GameObject* entity = database->GameObjects->Find(resourceName);

		ConfigureAllEntityAttributes(file, entity);
	}
}

int LevelConfiguration::ParseNumberOfEntitiesInFile(std::ifstream& file)
{
	string attribute;
	getline(file, attribute);

	int numObjectsToRead = std::atoi(attribute.c_str());

	return numObjectsToRead;
}

void LevelConfiguration::InitialiseGameLogic(std::string filename)
{
	std::ifstream file(filename);
	string line;

	FSMManager* fsmManager = database->GFSMManager->Find("GFSMManager");

	while (getline(file, line))
	{
		vector<std::string> tokens = Log::tokenise(line);
		std::string FSMName = tokens.at(0);
		std::string FSMConfigurationFile = tokens.at(1);

		fsmManager->LateBuildFSM(FSMName, FSMConfigurationFile);
	}
}

void LevelConfiguration::InitialiseMusic(std::string filename)
{
	std::ifstream file(filename);
	string songFileName;

	while (getline(file, songFileName))
	{
		AudioManager::GetInstance()->AddBackgroundSound(songFileName);
	}
}

void LevelConfiguration::ConfigureAllEntityAttributes(std::ifstream& file, GameObject* entity)
{
	string attribute;

	vector<string> emptyAttribute;
	EntityConfiguration configuration(emptyAttribute, entity);

	while (getline(file, attribute))
	{
		vector<string> tokens = Log::tokenise(attribute);

		configuration.SetAttributeTokens(tokens);
		configuration.ConfigureEntityWithAttribute();

		if (configuration.IsFinished())
		{
			break;
		}
	}
}