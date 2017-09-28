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

	ConfigurationFile(const std::string file, std::function<void(std::string)> configurator)
	{
		this->file = file;
		this->configurator = configurator;
	}
};

class LevelConfiguration
{
public:
	LevelConfiguration(DataBase* database, const std::string directory);
	~LevelConfiguration() {}

	void ConfigureFromAllExistingFiles() const;

private:
	void ReadPhysicsObject(const std::string file);
	void ReadGameObject(const std::string file);
	const int ParseNumberOfEntitiesInFile(std::ifstream& file) const;

	void InitialiseGameLogic(const std::string filename) const;
	void InitialiseMusic(const std::string filename) const;

	void ConfigureAllEntityAttributes(std::ifstream& file, GameObject* entity) const;

	DataBase* database;
	std::vector<ConfigurationFile> configFiles;
};

