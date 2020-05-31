#pragma once
#include <glm/common.hpp>
#include "Audio-Engine/AudioEngine.h"

// screen settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

// background colors
glm::vec4 COLOR_BLACK(0.05f, 0.05f, 0.05f, 1.0f);
glm::vec4 COLOR_SKY(0.53f , 0.81f, 0.92f, 1.0f);

// player data
glm::vec3 STARTING_PLAYER_LOCATION(-2.0f, -6.0f, 5.0f);

// obj file locations
const char* OBJ_FOUNTAIN   = "res/objects/fountains/fountainOBJ/fountainOBJ.obj";
const char* OBJ_BACKPACK   = "res/LearnOpenGL/objects/backpack/backpack.obj";
const char* OBJ_HOUSE      = "res/objects/Monster House/Monster House.obj";
const char* OBJ_ROCK       = "res/objects/ground/rock/rock.obj";
const char* OBJ_GROUND     = "res/objects/greenground/ground.obj";
const char* OBJ_TREE       = "res/objects/flora/trees/fir/fir.obj";
const char* OBJ_HARP       = "res/objects/instruments/harp/3d-model.obj";
const char* OBJ_STONEFLOOR = "res/objects/ground/stone-ground/test/StoneFloor_Sample.obj";
const char* OBJ_BIRDS      = "res/objects/animals/birds/two-songbirds/lowpoly_bird.obj";
const char* OBJ_PINE       = "res/objects/flora/trees/Pine/Tree2.obj";
const char* OBJ_OAK		   = "res/objects/flora/trees/GreenTree/Tree.obj";
const char* OBJ_GRASS      = "res/objects/ground/grass/Grass.obj";
const char* OBJ_COOLTREE   = "res/objects/flora/trees/Cool_Tree/Cool Tree.obj";
const char* OBJ_AZALEA	   = "res/objects/flora/Pink_Azalea_Flowers/Pink Azalea Flowers.obj";
const char* OBJ_COTTAGE    = "res/objects/85-cottage_obj/Japanese House 1.obj";
const char* OBJ_HOUSE2	   = "res/objects/Houses/House2/Neighbor's House (Act 1).obj";
const char* OBJ_WILLOWTREE = "res/objects/flora/trees/Willow Tree/treewillow_tslocator_gmdc.obj";
const char* OBJ_WELL	   = "res/objects/Houses/Well/Well.obj";
const char* OBJ_TOWNHOUSE  = "res/objects/Houses/Cool Town Hall/Cool Town Hall.obj";
const char* OBJ_COIN = "res/objects/coins/coin1/coin.obj";
const char* OBJ_JAPANESE_TREE  = "res/objects/flora/trees/Oak_Tree/oakTree.obj";
const char* OBJ_JAPANESE_TREE2 = "res/objects/flora/trees/GreenTree/Tree.obj";
const char* OBJ_HOUSE4 = "res/objects/Houses/MedievalHouse/casa.obj";
const char* OBJ_HOUSE3 = "res/objects/Houses/abandoned_cottage/abandoned_cottage.obj";
const char* OBJ_BUSH = "res/objects/flora/trees/hazelnutbush/Hazelnut.obj";





// game object starting positions
glm::vec3 tranBackpack(0.5f, -1.2f, 0.0f), scaleBackpack(0.5f), rotBackpack(0.0f);
glm::vec3 tranGround(50.0f, -8.0f, -200.0f), scaleGround(40.0f), rotGround(0.0f, 180.0f, 180.0f);
glm::vec3 tranTreeFir(0.0f, -8.0f, -10.0f), scaleTreeFir(0.008f), rotTreeFir(0.0f);
glm::vec3 tranRock(-8.0f, -8.0f, -15.0f), scaleRock(0.38f), rotRock(0.0f);
glm::vec3 tranFountain(-10.0f, -7.9f, -5.0f), scaleFountain(0.06f), rotFountain(0.0f);
glm::vec3 tranBirds(0.5f, -1.2f, 0.0f), scaleBirds(0.05f), rotBirds(0.0f, 180.0f, 0.0f);
glm::vec3 tranHarp(tranFountain.x, tranFountain.y + 4.0f, tranFountain.z), scaleHarp(0.0022f), rotHarp(0.0f, 120.0f, 0.0f);
glm::vec3 tranPine(-32.0f, -8.0f, -10.0f), scalePine(0.60f), rotPine(0.0f);
glm::vec3 tranGrass(-32.0f, -8.5f, -9.0f), scaleGrass(0.80f), rotGrass(0.0f);
glm::vec3 tranCooltree(-37.0f, -9.0f, -10.0f), scaleCooltree(0.70f), rotCooltree(0.0f);
glm::vec3 tranHouse2(-45.0f, -7.5f, 1.0f), scaleHouse2(0.60f), rotHouse2(0.0f, 70.0f, 0.0f);
glm::vec3 tranJapaneseTree(10.0f, -8.5f, -9.0f), scaleJapaneseTree(0.11f), rotJapaneseTree(0.0f);
glm::vec3 tranCottage(-30.0f, -8.5f, -5.0f), scaleCottage(0.45f), rotCottage(0.0f, 70.0f, 0.0f);
glm::vec3 tranWillowtree(-35.0f, -8.0f, 10.0f), scaleWillowtree(0.70f), rotWillowtree(0.0f);
glm::vec3 tranWell(-28.0f, -8.0f, 37.0f), scaleWell(0.03f), rotWell(0.0f);
glm::vec3 tranGreenPine(-32.0f, -8.0f, 25.0f), scaleGreenPine(0.55f), rotGreenPine(0.0f, 165.0f, 0.0f);
glm::vec3 tranJapaneseTree2(-15.0f, -7.5f, 30.0f), scaleJapaneseTree2(0.60f), rotJapaneseTree2(0.0f, 110.0f, 0.0f);
glm::vec3 tranfir1(-40.0f, -8.5f, 38.0f), scalefir1(0.65f), rotfir1(0.0f);
glm::vec3 tranfir2(-40.0f, -8.5f, 60.0f), scalefir2(0.65f), rotfir2(0.0f);
glm::vec3 tranfir3(-40.0f, -8.5f, 75.0f), scalefir3(0.65f), rotfir3(0.0f);
glm::vec3 tranfir4(-23.0f, -9.5f, -38.0f), scalefir4(0.65f), rotfir4(0.0f);
glm::vec3 tranHouse(-32.0f, -8.0f, -38.0f), scaleHouse(0.018f), rotHouse(0.0f, 30.0f, 0.0f);
glm::vec3 tranHouse4(-5.0f, -8.0f, -23.0f), scaleHouse4(0.15f), rotHouse4(0.0f, 220.0f, 0.0f);

// AABB box sizes
glm::vec3 AABB_DIMS_CHARACTER(5.0f);
glm::vec3 AABB_DIMS_COIN(5.0f);


// Coin starting scale/rotation for all coin instances
glm::vec3 scaleCoins(0.1f), rotCoins(0.0f);
// Coin translations for each individual coin
static std::vector<glm::vec3> coinTranslations {
	glm::vec3(2.0f, -8.0f, -5.0f),
	glm::vec3(2.8f, -8.0f, -12.0f),
	glm::vec3(4.0f, -8.0f, -15.0f),
	glm::vec3(5.3f, -8.0f, -21.0f),
	glm::vec3(7.0f, -8.0f, -28.0f)

};

// raw audio assets
const char* MUSIC                = "res/sound/music/Medieval Village Full Theme Demo_2.5.1.3.wav";
const char* MUSIC_2              = "res/sound/music/Medieval Village3.0 DEMO 2 Loop.wav";
const char* STINGER_1_GUITAR     = "res/sound/music/Medieval Village_Stinger1 Guitar_2.5.1.3.wav";
const char* STINGER_2_GUITAR_2   = "res/sound/music/Medieval Village_Stinger2 Guitar_2.5.1.3.wav";
const char* STINGER_3_HARP       = "res/sound/music/Medieval Village_Stinger3 Harp_2.5.1.3.wav";
const char* SFX_LOOP_FOUNTAIN    = "res/sound/fountain/Fountain_Loop2.wav";
const char* SFX_LOOP_BIRD        = "res/sound/animals/birds/SFX_LOOP_BIRDS.wav";
const char* SFX_LOOP_TREE_BIRDS  = "res/sound/animals/birds/SFX_LOOP_TREE_BIRDS.wav";
const char* STINGER_COIN_PICKUP  = "res/sound/coin/SFX_ONESHOT_COIN_PICKUP.wav";
const char* STINGER_COIN_SUCCESS = "res/sound/coin/SFX_ONESHOT_COIN_SUCCESS.wav";

const char* SFX_FOOTSTEP1 = "res/sound/footsteps/SFX_FOOTSTEP1.wav";
const char* SFX_FOOTSTEP2 = "res/sound/footsteps/SFX_FOOTSTEP2.wav";
const char* SFX_FOOTSTEP3 = "res/sound/footsteps/SFX_FOOTSTEP3.wav";
const char* SFX_FOOTSTEP4 = "res/sound/footsteps/SFX_FOOTSTEP4.wav";
const char* SFX_FOOTSTEP5 = "res/sound/footsteps/SFX_FOOTSTEP5.wav";
const char* SFX_FOOTSTEP6 = "res/sound/footsteps/SFX_FOOTSTEP6.wav";
const char* SFX_FOOTSTEP7 = "res/sound/footsteps/SFX_FOOTSTEP7.wav";
const char* SFX_FOOTSTEP8 = "res/sound/footsteps/SFX_FOOTSTEP8.wav";

// TODO rename sounds for clarity
SoundInfo soundOneShot(STINGER_1_GUITAR);
SoundInfo soundOneShot3D(STINGER_3_HARP, false, true, tranHarp.x, tranHarp.y, tranHarp.z);
SoundInfo musicLoop2d(MUSIC_2, true);
SoundInfo soundLoop3D(SFX_LOOP_FOUNTAIN, true, true, tranFountain.x, tranFountain.y, tranFountain.z);
SoundInfo soundJapaneseTree(SFX_LOOP_TREE_BIRDS, true, true, tranTreeFir.x, tranTreeFir.y, tranTreeFir.z);
SoundInfo soundTree(SFX_LOOP_TREE_BIRDS, true, true, tranWillowtree.x, tranWillowtree.y, tranWillowtree.z);
SoundInfo soundLoop3DMoving(SFX_LOOP_BIRD, true, true, tranBirds.x, tranBirds.y, tranBirds.z);
SoundInfo soundCoinPickup(STINGER_COIN_PICKUP);
SoundInfo soundCoinSuccess(STINGER_COIN_SUCCESS);

std::vector<SoundInfo> soundsFootsteps {
	SoundInfo(SFX_FOOTSTEP1),
	SoundInfo(SFX_FOOTSTEP2),
	SoundInfo(SFX_FOOTSTEP3),
	SoundInfo(SFX_FOOTSTEP4),
	SoundInfo(SFX_FOOTSTEP5),
	SoundInfo(SFX_FOOTSTEP6),
	SoundInfo(SFX_FOOTSTEP7),
	SoundInfo(SFX_FOOTSTEP8)
};


// FMOD Studio sound banks
const char* FMOD_SOUNDBANK_MASTER         = "res/sound/Master.bank";
const char* FMOD_SOUNDBANK_MASTER_STRINGS = "res/sound/Master.strings.bank";
const char* FMOD_SOUNDBANK_SFX            = "res/sound/SFX.bank";

// FMOD Studio Events
const char* FMOD_EVENT_CHARACTER_FOOTSTEPS = "event:/Character/Player Footsteps";
static const std::vector<std::pair<const char*, float>> PARAM_CHARACTER_FOOTSTEPS_SURFACE{ 
	{ "Surface", 2.0f } 
};

const char* FMOD_EVENT_2D_LOOP_COUNTRY_AMBIENCE = "event:/Ambience/Country";

const char* FMOD_EVENT_2D_ONESHOT_EXPLOSION = "event:/Weapons/Explosion";

//cosnt char* FMOD_EVENT_2D_UI_


