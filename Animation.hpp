#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include <string>
#include <vector>
#include <map>
#include "Includes/vml.hpp"


using namespace vml;

struct TransitionPose {
    float duration = 0.0f;
    std::map<float, std::map<std::string, vec3>> keyframes;
};

struct ParseTransitionPose {
    float duration = 0.0f;
    std::map<std::string, std::vector<std::map<float, vec3>>> tracks;
};

enum class PlayState { STOPPED, START, LOOP, FINISH };

struct Animation {
    std::string state;
    std::map<std::string, std::vector<std::map<float, vec3>>> tracks;
    std::map<PlayState, TransitionPose> pl;
    std::map<std::string, vec3> actualPose;

    PlayState playState = PlayState::STOPPED;
    PlayState nextState;
    float currentTime = 0;
    float currentFrameTime = 0;
    std::map<float, std::map<std::string, vec3>> currentKeyframes;
    
    void convertToKeyframes(PlayState state, ParseTransitionPose transitionPose);
    void print() const;
    void finishFrame();
    void updateActualPose(int frameTime);

    void flipflop();
    void update();
};

struct AnimManager
{
    std::vector<Animation> anims;
    int currentAnim = -1;

    void changeAnim(int index);
};

#endif