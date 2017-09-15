#pragma once

#include <functional>
#include <string>
#include <vector>

class DataBase;
class GameObject;

struct ConfigurationFile
{
	std::string file;
	std::function<void(std::string)> configurator;

	ConfigurationFile(std::string file, std::function<void(std::string)> configurator)
	{
		this->file = file;
		this->configurator = configurator;
	}
};

class LevelConfiguration
{
public:
	LevelConfiguration(DataBase* database, std::string directory);
	~LevelConfiguration() {}

	void ConfigureFromAllExistingFiles();

private:
	void ReadPhysicsObject(std::string file);
	void ReadGameObject(std::string file);
	int ParseNumberOfEntitiesInFile(std::ifstream& file);

	void InitialiseGameLogic(std::string filename);
	void InitialiseMusic(std::string filename);

	void ConfigureAllEntityAttributes(std::ifstream& file, GameObject* entity);

	DataBase* database;
	std::vector<ConfigurationFile> configFiles;
};

