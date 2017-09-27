# Resource Managment

## Description
Manages memory for resources, ensuring memory does not exceed limits and resources can be retrieved safely where required.


## How To Use

### Database
A new instance of database must be created to house all assets and manage memory.
Tables are specified in TableCreation.

The following creats a database and initialises the added tables.

```cpp
	DataBase* database = new DataBase();

	TableCreation tables(database);
	tables.AddTablesToDatabase();

	database->ReserveMemoryForAllTables();
```

A static cast will be required upon retrieval of a resource from a table.

#### Adding a new table
The TableCreation class allows for a new table to be added by:

1. Creating a new function that parses a line from the File I/O into a resource through a lambda.
2. This function is added to a vector of functions in the class constructor.
3. All functions are then called on startup, adding a new table to the database.

For Example:

1. 
```cpp
	void TableCreation::AddMyTable() const
	{
		database->AddTable("TableName", new Table<Resource>(false, false, [](std::string lineFromFile)
		{
			SomeResource* resource = new SomeResource();
			resource->Read(lineFromFile);

			return resource;
		}, TableConfiguration("TableName", MAX_MEM_PER_TYPE, SINGLE_BIN)));
	}
```

1. 
```cpp
	tableAdditions.push_back(std::bind(&TableCreation::AddMyTable, this));
```

1.
```cpp
	void TableCreation::AddTablesToDatabase() const
	{
		for each (auto additionFunction in tableAdditions)
		{
			additionFunction();
		}
	}
```



### Resource Managment

#### ResourceManager
Manages the addition of resources through bins. Will enter a given resource into a bin that contains enough space.

#### ResourceMap
Acts as a single bin for resources.

#### Resource
Any class that will be used as a resource and entered into the database must inherit from this.
The resource's size must be updated upon any changes.

The Read and ReadParams functions allow for parsing of a line from a file into changes of properties.