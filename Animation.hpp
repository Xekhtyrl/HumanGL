#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include <string>
#include <vector>
#include <map>
#include "Includes/vml.hpp"


using namespace vml;

struct Animation {
    std::string state;
    float duration;
    // IModel* model;
    std::map<std::string, std::vector<std::map<float, vec3>>> tracks;
    std::map<float, std::map<std::string, vec3>> keyframes;
    std::map<std::string, vec3> actualPose;
    
    // Variables d'état pour la lecture
    bool isPlaying = false;
    float currentTime = 0.0f;
    float startTime = 0.0f;
    float currentFrameTime = 0.0f;
    
    void convertToKeyframes();
    void print() const;
    void printKeyframes() const;
    void printActualPose() const;

    void flipflop();
    void update(float deltaTime);
};

#endif