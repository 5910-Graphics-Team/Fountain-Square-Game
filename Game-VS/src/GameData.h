#pragma once
#include <glm/common.hpp>

// screen settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

// background colors
glm::vec4 COLOR_BLACK(0.05f, 0.05f, 0.05f, 1.0f);
glm::vec4 COLOR_SKY(0.31f , 0.0f, 0.95f, 1.0f);

glm::vec3 STARTING_PLAYER_LOCATION(-2.0f, -6.0f, 5.0f);

// audio assets
const char* MUSIC         = "res/sound/Medieval Village Full Theme Demo_2.5.1.3.wav";
const char* STINGER_1     = "res/sound/Medieval Village_Stinger1 Guitar_2.5.1.3.wav";
const char* STINGER_2     = "res/sound/Medieval Village_Stinger2 Guitar_2.5.1.3.wav";
const char* STINGER_3     = "res/sound/Medieval Village_Stinger3 Harp_2.5.1.3.wav";
const char* SFX_LOOP_FOUNTAIN  = "res/sound/Fountain_Loop.wav";
const char* SFX_LOOP_BIRD = "res/sound/animals/birds/SFX_LOOP_BIRDS.wav";

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

// game object starting positions
glm::vec3 tranBackpack(0.5f, -1.2f, 0.0f),    scaleBackpack(0.5f),  rotBackpack(0.0f);
glm::vec3 tranHouse(-18.0f, -8.0f, -23.0f),   scaleHouse(0.72f),    rotHouse(0.0f, 70.0f, 0.0f);
glm::vec3 tranGround(50.0f, -8.0f, -200.0f),  scaleGround(40.0f),   rotGround(0.0f, 180.0f, 180.0f);
glm::vec3 tranTreeFir(0.0f, -8.0f, -10.0f),   scaleTreeFir(0.008f), rotTreeFir(0.0f);
glm::vec3 tranRock(-8.0f, -8.0f, -15.0f),     scaleRock(0.38f),     rotRock(0.0f);
glm::vec3 tranFountain(-10.0f, -7.9f, -5.0f), scaleFountain(0.06f), rotFountain(0.0f);
glm::vec3 tranBirds(tranBackpack), scaleBirds(0.05f), rotBirds(0.0f, 180.0f, 0.0f);
glm::vec3 tranPine(-28.0f, -9.0f, -10.0f),    scalePine(0.70f),     rotPine(0.0f);

glm::vec3 tranHarp(tranFountain.x, tranFountain.y + 4.0f, tranFountain.z), scaleHarp(0.0022f), rotHarp(0.0f, 120.0f, 0.0f); 
