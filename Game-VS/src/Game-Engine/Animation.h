#pragma once

/**
 * Interface which can be implemented to allow for the updating of a game object 
 * or model's time based location/animation movements
 */
class Animation {

public:
    /**
     * Updates the animation based on how much time has passed since previous frame
     * @param time - float value, in seconds
     */
	virtual	void update(float time) = 0;
};