# Threading

## Description
Simple hardware dependant thread pool implementation. Work is submitted through lambdas.

## How To Use

### Thread Pool
Uses lambdas to add a new job for worker threads. They will be put on a queue and completed as worker threads retrieve them.

Default constructor uses as many threads as are available on the machine - 1 to take into account the main thread.

#### Submitting work

For Example:

```cpp
	int myInteger = 4;
	SomeObject myObject();

	threadPool->SubmitJob([](int i, SomeObject* object){
		SomeFunction(i, myObject);
	}, myInteger, std::ref(myObject));
```

#### Synchronising Threads
Threads will wait for completion when going out of scope. Add to vector to keep in scope to then synchronise.

For example:

```cpp
	vector<TaskFuture<int>> threads;
	
	threads.push_back(threadPool->SubmitJob([](int i){
		return 6 + i;
	}, someInt));
	
	for (auto& task : threads)
	{
		task.Complete();
	}
```