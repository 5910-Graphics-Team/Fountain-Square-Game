#pragma once
#include <glm/common.hpp>

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
const char* OBJ_OAK		   = "res/objects/flora/trees/Autumn_Tree/Autumn Tree.obj";
const char* OBJ_GRASS      = "res/objects/ground/grass/Grass.obj";
const char* OBJ_COOLTREE   = "res/objects/flora/trees/Cool_Tree/Cool Tree.obj";
const char* OBJ_AZALEA	   = "res/objects/flora/Pink_Azalea_Flowers/Pink Azalea Flowers.obj";
const char* OBJ_COTTAGE    = "res/objects/85-cottage_obj/Japanese House 1.obj";
const char* OBJ_HOUSE2	   = "res/objects/Houses/House2/Neighbor's House (Act 1).obj";
const char* OBJ_WILLOWTREE = "res/objects/flora/trees/Willow Tree/treewillow_tslocator_gmdc.obj";
const char* OBJ_WELL	   = "res/objects/Houses/Well/Well.obj";

const char* OBJ_JAPANESE_TREE = "res/objects/flora/trees/Oak_Tree/oakTree.obj";



// game object starting positions
glm::vec3 tranBackpack(0.5f, -1.2f, 0.0f), scaleBackpack(0.5f), rotBackpack(0.0f);
glm::vec3 tranHouse(-18.0f, -8.0f, -23.0f), scaleHouse(0.72f), rotHouse(0.0f, 70.0f, 0.0f);
glm::vec3 tranGround(50.0f, -8.0f, -200.0f), scaleGround(40.0f), rotGround(0.0f, 180.0f, 180.0f);
glm::vec3 tranTreeFir(0.0f, -8.0f, -10.0f), scaleTreeFir(0.008f), rotTreeFir(0.0f);
glm::vec3 tranRock(-8.0f, -8.0f, -15.0f), scaleRock(0.38f), rotRock(0.0f);
glm::vec3 tranFountain(-10.0f, -7.9f, -5.0f), scaleFountain(0.06f), rotFountain(0.0f);
glm::vec3 tranBirds(tranBackpack), scaleBirds(0.05f), rotBirds(0.0f, 180.0f, 0.0f);
glm::vec3 tranHarp(tranFountain.x, tranFountain.y + 4.0f, tranFountain.z), scaleHarp(0.0022f), rotHarp(0.0f, 120.0f, 0.0f);
glm::vec3 tranPine(-32.0f, -9.0f, -10.0f), scalePine(0.11f), rotPine(0.0f);
glm::vec3 tranGrass(-32.0f, -8.5f, -9.0f), scaleGrass(0.80f), rotGrass(0.0f);
glm::vec3 tranCooltree(-37.0f, -9.0f, -10.0f), scaleCooltree(0.70f), rotCooltree(0.0f);
glm::vec3 tranHouse2(-45.0f, -7.5f, 1.0f), scaleHouse2(0.60f), rotHouse2(0.0f, 70.0f, 0.0f);
glm::vec3 tranJapaneseTree(10.0f, -8.5f, -9.0f), scaleJapaneseTree(0.11f), rotJapaneseTree(0.0f);
glm::vec3 tranCottage(-30.0f, -8.5f, -5.0f), scaleCottage(0.45f), rotCottage(0.0f, 70.0f, 0.0f);
glm::vec3 tranWillowtree(-35.0f, -8.0f, 10.0f), scaleWillowtree(0.70f), rotWillowtree(0.0f);
glm::vec3 tranWell(-35.0f, -8.0f, 25.0f), scaleWell(0.03f), rotWell(0.0f);


// raw audio assets
const char* MUSIC               = "res/sound/music/Medieval Village Full Theme Demo_2.5.1.3.wav";
const char* MUSIC_2             = "res/sound/music/Medieval Village3.0 DEMO 2 Loop.wav";
const char* STINGER_1_GUITAR    = "res/sound/music/Medieval Village_Stinger1 Guitar_2.5.1.3.wav";
const char* STINGER_2_GUITAR_2  = "res/sound/music/Medieval Village_Stinger2 Guitar_2.5.1.3.wav";
const char* STINGER_3_HARP      = "res/sound/music/Medieval Village_Stinger3 Harp_2.5.1.3.wav";
const char* SFX_LOOP_FOUNTAIN   = "res/sound/fountain/Fountain_Loop2.wav";
const char* SFX_LOOP_BIRD       = "res/sound/animals/birds/SFX_LOOP_BIRDS.wav";
const char* SFX_LOOP_TREE_BIRDS = "res/sound/animals/birds/SFX_LOOP_TREE_BIRDS.wav";


// FMOD Studio sound banks
const char* FMOD_SOUNDBANK_MASTER         = "res/sound/Master.bank";
const char* FMOD_SOUNDBANK_MASTER_STRINGS = "res/sound/Master.strings.bank";
const char* FMOD_SOUNDBANK_SFX            = "res/sound/SFX.bank";

// FMOD Studio Events
const char* FMOD_EVENT_CHARACTER_FOOTSTEPS = "event:/Character/Player Footsteps";
static const std::vector<std::pair<const char*, float>> PARAM_CHARACTER_FOOTSTEPS_SURFACE{ 
	{ "Surface", 1.0f } 
};

const char* FMOD_EVENT_2D_LOOP_COUNTRY_AMBIENCE = "event:/Ambience/Country";

const char* FMOD_EVENT_2D_ONESHOT_EXPLOSION = "event:/Weapons/Explosion";

//cosnt char* FMOD_EVENT_2D_UI_


