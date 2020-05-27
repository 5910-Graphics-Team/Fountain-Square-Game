#pragma once
#include "GameObject.h"
#include <iostream>

class Loader {
private:
	
	
	void loadGameObjectSubList(std::vector<GameObject*> gameObjects) {
		for (GameObject* object : gameObjects) {
			//std::cout << "Loading object " << object->getObjFilePath() << "\n";
			object->loadAsync();
		}
	}

public:
	Loader() {}
		
		
	void loadGameObjects(std::vector<GameObject*> gameObjects, int nThreads = 1) {
		std::cout << "Loading " << gameObjects.size() << " game objects\n";
		
		if (nThreads > 1) {
			// Multi-threaded loading not supported yet, so just load all objects:
			//loadGameObjectSubList(gameObjects);

			// TODO Fix multi-threaded loading:
			int remainder = gameObjects.size() % nThreads;
			auto sublistSize = gameObjects.size() / nThreads;
			auto iter = gameObjects.begin();
			std::vector<std::future<void>> futures;

			for (std::size_t i = 0; i < nThreads; ++i) {
				sublistSize += (i == nThreads - 1) ? remainder : 0;
				std::cout << "Thread # " << i << " game object list size = " << sublistSize << '\n';
				std::vector<GameObject*> sublist(iter, iter + sublistSize);
				futures.push_back(std::async(&Loader::loadGameObjectSubList, this, sublist));
				iter += sublistSize;
			}

			for (std::size_t i = 0; i < futures.size(); ++i) {
				futures[i].get();
			}

			// load all textures on main thread:
			for (GameObject* gameObject : gameObjects)
				gameObject->loadSync();
		}
		else
			loadGameObjectSubList(gameObjects);

	}

};