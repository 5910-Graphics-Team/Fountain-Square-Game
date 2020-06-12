#pragma once
#include <vector>

/**
 * Manages the index randomization process for a random sound container
 */
class IndexRandomizer {
public:
	/**
	 * Constructs an index randomizer
	 * @param totalEntries - the total number of indices
	 * @param roundRobinMin - the minimum number of new indices that must be returned from getNewIndex()
	 *                      before a previous index is returned again from getNewIndex()
	 */
	IndexRandomizer(unsigned int totalEntries, unsigned int roundRobinMin = 3) : totalEntries(totalEntries), roundRobinMin(roundRobinMin), queue() {
		initQueue();
	}
	/**
	 * Returns the next index which is unique within a designated number of previous generated indices
	 */
	int getNextIndex() {
		int index = queue.at(0);
		queue.erase(queue.begin());
		queue.push_back(getNewIndex());
		return index;
	}
private:

	std::vector<int> queue;
	unsigned int totalEntries;
	unsigned int roundRobinMin;// 3 new sounds must be played before a sound is retriggered

	void initQueue() {
		for (int i = 0; i < roundRobinMin; ++i)
			queue.push_back(getNewIndex());
	}
	/**
	 * Generates a new index not in the queue
	 */
	int getNewIndex() {
		bool ready = false;
		int index = rand() % totalEntries;
		while (queueContains(index)) index = rand() % totalEntries;
		return index;
	}

	bool queueContains(int val) {
		for (int i : queue)
			if (i == val) return true;
		return false;
	}
};