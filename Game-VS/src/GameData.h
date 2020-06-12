/*
* @file GameData.h
* All game domain information, used in Main and some other code files, is defined here. 
* Including default window dimensions, item locations/rotations/scales, 
* some sound file locations, and more.
*/
#pragma once
#include <glm/common.hpp>
#include "Audio-Engine/SoundInfo.h"

// window size settings
const unsigned int SCREEN_WIDTH = 1920, SCREEN_HEIGHT = 1080;

// Minimum amount of time between key presses, used by non-movement related keyboard controls.
float KEY_MIN_RETRIGGER_TIME = 0.2f;
// Variables tracking the last time a particular key was pressed
float key1LastTime = 0.0f, key2LastTime = 0.0f, key3LastTime = 0.0f, key4LastTime = 0.0f, key5LastTime = 0.0f,
      key6LastTime = 0.0f, key7LastTime = 0.0f, key8LastTime = 0.0f, key9LastTime = 0.0f, key0LastTime = 0.0,
      keyKLastTime = 0.0f, keyMLastTime = 0.0f;

// black background color
glm::vec4 COLOR_BLACK(0.05f, 0.05f, 0.05f, 1.0f);
// sky background color
glm::vec4 COLOR_SKY(0.53f , 0.81f, 0.92f, 1.0f);

// The Player's starting position
glm::vec3 STARTING_PLAYER_LOCATION(-4.0f, 1.0f, -25.0f);

// obj file locations
const char* OBJ_FOUNTAIN   = "res/objects/fountains/Basic Fountain 1/Basic Fountain 1.obj";
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
const char* OBJ_GRASS      = "res/objects/ground/grass_1/grass/free grass by adam127.obj";
const char* OBJ_COOLTREE = "res/objects/ground/cart/uploads_files_2060573_HayCart.obj";
const char* OBJ_AZALEA	   = "res/objects/flora/Pink_Azalea_Flowers/Pink Azalea Flowers.obj";
const char* OBJ_COTTAGE    = "res/objects/85-cottage_obj/Japanese House 1.obj";
const char* OBJ_HOUSE2	   = "res/objects/Houses/House2/Neighbor's House (Act 1).obj";
const char* OBJ_WILLOWTREE = "res/objects/flora/trees/Willow Tree/treewillow_tslocator_gmdc.obj";
const char* OBJ_WELL	   = "res/objects/Houses/Well/Well.obj";
const char* OBJ_TOWNHOUSE  = "res/objects/Houses/Cool Town Hall/Cool Town Hall.obj";
const char* OBJ_COIN = "res/objects/coins/coin1/coin.obj";
const char* OBJ_JAPANESE_TREE  = "res/objects/flora/trees/Oak_Tree/oakTree.obj";
const char* OBJ_JAPANESE_TREE2 = "res/objects/flora/trees/GreenTree/Tree.obj";
const char* OBJ_HOUSE4 = "res/objects/Houses/Stable/uploads_files_2279663_HoiAnHouse_M2.obj";
const char* OBJ_HOUSE3 = "res/objects/Houses/abandoned_cottage/abandoned_cottage.obj";
const char* OBJ_BUSH = "res/objects/flora/trees/hazelnutbush/Hazelnut.obj";
const char* OBJ_TREE_BUSH = "res/objects/flora/Tree4/uploads_files_885045_tree_1.obj";
const char* OBJ_TREE_LINE = "res/objects/flora/Tree_Line/FKLPI_Forest/FKLPI_Forest.dae";
const char* OBJ_YUN = "res/objects/Yun/Yun.obj";

// Global object size scaling, used to bring all objects down in size proportionally
glm::vec3 GLOBAL_SCALE(0.5f);
// Global object translation scaling, used to scale down the translation locations of all game objects
glm::vec3 GLOBAL_POSITION_SCALE(0.75);

// Definitions of all starting locations, size scales, and euler rotations of all (non-instanced) game objects 
glm::vec3 tranNPC(-5.0f, 0.0f, 0.0f), scaleNPC(13.5f), rotNPC(0.0f);
glm::vec3 tranBackpack(0.5f, -6.8f, 0.0f), scaleBackpack(0.5f), rotBackpack(0.0f);
glm::vec3 tranGround(-80.0f, 0.0f, -30.0f), scaleGround(70.0f), rotGround(0.0f, 10.0f, 180.0f);
glm::vec3 tranTreeFir(0.0f, 0.0f, -10.0f), scaleTreeFir(0.008f), rotTreeFir(0.0f);
glm::vec3 tranRock(-8.0f, 0.0f, -15.0f), scaleRock(0.38f), rotRock(0.0f);
glm::vec3 tranFountain(-10.0f, -0.3f, -5.0f), scaleFountain(0.37f), rotFountain(3.0f, 0.0f, 0.0f);
glm::vec3 tranBirds(0.5f, 6.8f, 0.0f), scaleBirds(0.05f), rotBirds(0.0f, 180.0f, 0.0f);
glm::vec3 tranHarp(tranFountain.x, tranFountain.y + 2.0f, tranFountain.z), scaleHarp(0.0022f), rotHarp(0.0f, 120.0f, 0.0f);
glm::vec3 tranPine(-32.0f, 0.0f, -10.0f), scalePine(0.60f), rotPine(0.0f);
glm::vec3 tranGrass(50.0f, -0.0f, 50.0f), scaleGrass(0.2f), rotGrass(0.0f);
glm::vec3 tranCooltree(-37.0f, 0.0f, -10.0f), scaleCooltree(0.70f), rotCooltree(0.0f);
glm::vec3 tranHouse2(10.0f, 0.2f, 20.0f), scaleHouse2(0.40f), rotHouse2(0.0f, 205.0f, 0.0f);
glm::vec3 tranJapaneseTree(12.0f, -0.5f, -15.0f), scaleJapaneseTree(0.06f), rotJapaneseTree(0.0f);
glm::vec3 tranCottage(-30.0f, -0.5f, -5.0f), scaleCottage(0.45f), rotCottage(0.0f, 70.0f, 0.0f);
glm::vec3 tranCottage1(65.0f, -0.5f, -5.0f), scaleCottage1(0.55f), rotCottage1(0.0f, 250.0f, 0.0f);
glm::vec3 tranCottage2(20.0f, -0.5f, 5.0f), scaleCottage2(0.45f), rotCottage2(0.0f, -70.0f, 0.0f);
glm::vec3 tranWillowtree(-35.0f, 0.0f, 10.0f), scaleWillowtree(0.70f), rotWillowtree(0.0f);
glm::vec3 tranWell(-28.0f, 0.0f, 37.0f), scaleWell(0.03f), rotWell(0.0f);
glm::vec3 tranGreenPine(-32.0f, 0.0f, 25.0f), scaleGreenPine(0.55f), rotGreenPine(0.0f, 165.0f, 0.0f);
glm::vec3 tranGreenPine1(80.0f, 0.0f, 25.0f), scaleGreenPine1(0.55f), rotGreenPine1(0.0f, 255.0f, 0.0f);
glm::vec3 tranJapaneseTree2(-15.0f, 0.5f, 30.0f), scaleJapaneseTree2(0.60f), rotJapaneseTree2(0.0f, 110.0f, 0.0f);
glm::vec3 tranJapaneseTree3(15.0f, 0.5f, -10.0f);
glm::vec3 tranfir1(-40.0f, -0.5f, 38.0f), scalefir1(0.65f), rotfir1(0.0f);
glm::vec3 tranfir2(-40.0f, -0.5f, 60.0f), scalefir2(0.65f), rotfir2(0.0f);
glm::vec3 tranfir3(-40.0f, -0.5f, 75.0f), scalefir3(0.65f), rotfir3(0.0f);
glm::vec3 tranfir4(-15.0f, 0.1f, -38.0f), scalefir4(0.65f), rotfir4(0.0f);
glm::vec3 tranfir5(5.0f, -0.5f, 45.0f),   scalefir5(0.65f), rotfir5(0.0f);
glm::vec3 tranfir6(30.0f, -0.5f, 45.0f), scalefir6(0.65f), rotfir6(0.0f);
glm::vec3 tranfir7(75.0f, -0.5f, 45.0f), scalefir7(0.65f), rotfir7(0.0f);
glm::vec3 tranfir8(95.0f, -0.5f, 45.0f), scalefir8(0.65f), rotfir8(0.0f);
glm::vec3 tranfir9(145.0f, -0.5f, 45.0f), scalefir9(0.65f), rotfir9(0.0f);
glm::vec3 tranfir10(20.0f, -0.5f, -20.0f);
glm::vec3 tranfir11(15.0f, -0.5f, -15.0f);
glm::vec3 tranfir12(10.0f, -0.5f, -15.0f);
glm::vec3 tranfirback1(-40.0f, -0.5f, -38.0f), scalefirback1(0.65f), rotfirback1(0.0f);
glm::vec3 tranfirback2(-40.0f, -0.5f, -60.0f), scalefirback2(0.65f), rotfirback2(0.0f);
glm::vec3 tranfirback3(-40.0f, -0.5f, -75.0f), scalefirback3(0.65f), rotfirback3(0.0f);
glm::vec3 tranfirback5(5.0f, -0.5f, -45.0f), scalefirback5(0.65f), rotfirback5(0.0f);
glm::vec3 tranfirback6(30.0f, -0.5f, -45.0f), scalefirback6(0.65f), rotfirback6(0.0f);
glm::vec3 tranfirback7(35.0f, -0.5f, -35.0f), scalefirback7(0.65f), rotfirback7(0.0f);
glm::vec3 tranfirback8(25.0f, -0.5f, -55.0f), scalefirback8(0.65f), rotfirback8(0.0f);
glm::vec3 tranfirback9(-5.0f, -0.5f, -65.0f), scalefirback9(0.65f), rotfirback9(0.0f);
glm::vec3 tranHouse(-32.0f, 0.0f, -38.0f), scaleHouse(0.018f), rotHouse(0.0f, 30.0f, 0.0f);
glm::vec3 tranHouseback2(22.0f, 0.0f, -40.0f), scaleHouseback2(0.018f), rotHouseback2(0.0f, 120.0f, 0.0f);
glm::vec3 tranHouse4(-40.0f, 0.0f, -10.0f), scaleHouse4(0.65f), rotHouse4(0.0f, 110.0f, 0.0f);
glm::vec3 tranHouse5(15.0f, 0.0f, -20.0f), rotHouse5(0.0f, 260.0f, 0.0f);
glm::vec3 tranbush(-20.5f, 0.1f, -9.0f), scalebush(0.80f), rotbush(0.0f);
glm::vec3 tranbush1(-30.5f, 0.1f, -25.0f), scalebush1(0.90f), rotbush1(0.0f);
glm::vec3 tranbush2(-10.5f, 0.1f, 25.0f), scalebush2(0.90f), rotbush2(0.0f);
glm::vec3 tranbushback4(-79.0f, -1.5f, -5.0f), scalebushback4(0.65f), rotbushback4(0.0f);
glm::vec3 tranbushback5(-75.0f, -1.5f, -15.0f), scalebushback5(0.65f), rotbushback5(0.0f);
glm::vec3 tranbush6(-60.5f, -1.5f, 5.0f), scalebush6(0.65f), rotbush6(0.0f);
glm::vec3 trantreeline(100.5f, -0.3f, 60.0f), scaletreeline(0.90f), rottreeline(0.0f);
glm::vec3 trantreeline1(120.5f, -1.5f, 60.0f), scaletreeline1(0.90f), rottreeline1(0.0f, 0.0f, -4.0f);
glm::vec3 trantreeline2(140.5f, -2.9f, 60.0f), scaletreeline2(0.90f), rottreeline2(0.0f, 0.0f, -9.0f);
glm::vec3 trantreeline3(160.5f, -7.7f, 60.0f), scaletreeline3(0.90f), rottreeline3(0.0f, 0.0f,-1.2f);
glm::vec3 trantreeline4(180.5f, -1.6f, 60.0f), scaletreeline4(0.90f), rottreeline4(0.0f, 0.0f, 7.0f);
glm::vec3 trantreeline5(-10.5f, -1.4f, -70.0f);


// Coin-specific starting scale/rotation for all coin instances
glm::vec3 scaleCoins(0.085f), rotCoins(0.0f);
// Coin translations for each individual coin
static std::vector<glm::vec3> coinTranslations{
	glm::vec3(12.0f, 0.0f, -23.0f),
	glm::vec3((tranGreenPine1* GLOBAL_POSITION_SCALE) + glm::vec3(3.0f, 0.0f, 3.0f)),
	glm::vec3(4.0f, 0.0f, -15.0f),
	glm::vec3(2.0f, 0.0f, -10.0f),
	glm::vec3(-5.0f, 0.0f, 5.0f),
	glm::vec3((tranWell* GLOBAL_POSITION_SCALE) + glm::vec3(3.0f, 0.0f, 3.0f)),
	glm::vec3((tranGreenPine* GLOBAL_POSITION_SCALE) + glm::vec3(3.0f, 0.0f, 3.0f)),
	glm::vec3((tranWillowtree* GLOBAL_POSITION_SCALE) + glm::vec3(3.0f, 0.0f, 3.0f)),
	glm::vec3(-15.0f, 0.0f, -10.0f),
	glm::vec3((tranHouse* GLOBAL_POSITION_SCALE) + glm::vec3(3.0f, 0.0f, 3.0f)),
};

// raw audio asset paths
const char* SFX_LOOP_FOUNTAIN    = "res/sound/fountain/Fountain_Loop2.wav";
const char* DIALOGUE_TOWN_INTRO  = "res/sound/dialogue/Character1_Dialogue_TownIntroduction.wav";
const char* SFX_LOOP_TREE_BIRDS  = "res/sound/animals/birds/SFX_LOOP_TREE_BIRDS.wav";

// default reverb and volume for sounds used in main
float defReverb = 0.5, defVolume = 0.9;
// Scaled translations of in-game 3D sounds. TODO add sounds to more trees
glm::vec3 fountainSoundLocation = tranFountain * GLOBAL_POSITION_SCALE;
glm::vec3 japaneseTreeSoundLocation = tranfir1 * GLOBAL_POSITION_SCALE;
glm::vec3 treeSoundLocation = tranWillowtree * GLOBAL_POSITION_SCALE;
glm::vec3 npcSoundLocation = tranNPC * GLOBAL_POSITION_SCALE;
// SoundInfo objects used in Main
SoundInfo fountainSoundLoop(SFX_LOOP_FOUNTAIN,   defVolume, defReverb, SOUND_LOOP,     SOUND_3D, fountainSoundLocation.x, fountainSoundLocation.y,   fountainSoundLocation.z);
SoundInfo soundJapaneseTree(SFX_LOOP_TREE_BIRDS, defVolume, defReverb, SOUND_LOOP,     SOUND_3D, japaneseTreeSoundLocation.x, japaneseTreeSoundLocation.y, japaneseTreeSoundLocation.z);
SoundInfo soundTree        (SFX_LOOP_TREE_BIRDS, defVolume, defReverb, SOUND_LOOP,     SOUND_3D, treeSoundLocation.x,  treeSoundLocation.y, treeSoundLocation.z);
SoundInfo dialogue         (DIALOGUE_TOWN_INTRO, defVolume, defReverb, SOUND_ONE_SHOT, SOUND_3D, npcSoundLocation.x, npcSoundLocation.y, npcSoundLocation.z);
