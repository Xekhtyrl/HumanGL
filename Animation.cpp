#include "Animation.hpp"
#include <iostream>

void Animation::print() const {
    std::cout << "Animation State: " << state << "\n";
    std::cout << "Duration: " << duration << "\n";
    std::cout << "Tracks:\n";
    for (const auto& [trackName, keyframesList] : tracks) {
        std::cout << "  Track: " << trackName << "\n";
        for (const auto& keyframesMap : keyframesList) {
            std::cout << "  Keyframes:\n";
            std::cout << keyframesMap.size() << " keyframes found.\n";
            for (const auto& [time, values] : keyframesMap) {
                std::cout << "    Time: " << time << " -> Values: (" 
                            << values[0] << ", " 
                            << values[1] << ", " 
                            << values[2] << ")\n";
            }
        }
    }
}



void Animation::convertToKeyframes() {
    for (const auto& [boneName, keyframesList] : tracks) {
        for (const auto& keyframesMap : keyframesList) {
            for (const auto& [time, values] : keyframesMap) {
                keyframes[time][boneName] = values;
            }
        }
    }
    keyframes[duration] = {};
    if (keyframes.find(0) == keyframes.end()) {
        keyframes[0] = {};
    }
}

void Animation::printKeyframes() const {
    std::cout << "Keyframes:\n";
    for (const auto& [time, boneMap] : keyframes) {
        std::cout << "  Time: " << time << "\n";
        for (const auto& [boneName, values] : boneMap) {
            std::cout << "    Bone: " << boneName << " -> Values: (" 
                        << values[0] << ", " 
                        << values[1] << ", " 
                        << values[2] << ")\n";
        }
    }
}

void Animation::printActualPose() const {
    std::cout << "Actual Pose:\n";
    for (const auto& [boneName, values] : actualPose) {
        std::cout << "    Bone: " << boneName << " -> Values: (" 
                    << values[0] << ", " 
                    << values[1] << ", " 
                    << values[2] << ")\n";
    }
}

void Animation::flipflop() {
    isPlaying = !isPlaying;
    currentTime = 0.0f;
    currentFrameTime = 0.0f;
    actualPose = {};
    printf("flip flop called. isPlaying: %d\n", isPlaying);
    if (keyframes.find(currentFrameTime) != keyframes.end()) {
        for (const auto& [boneName, values] : keyframes[currentFrameTime]) {
            printf("Setting bone: %s to values (%f, %f, %f)\n", boneName.c_str(), values[0], values[1], values[2]);
            actualPose[boneName] = values;                
        }
        printActualPose();
    }
}

void Animation::update(float deltaTime) {
    if (!isPlaying) return;

    currentTime += deltaTime;
    
    if (currentTime >= duration) {
        currentTime = std::fmod(currentTime, duration);
        currentFrameTime = 0.0f;
    }

    std::map<float, std::map<std::string, vec3>>::iterator it = keyframes.find(currentFrameTime);

    std::map<float, std::map<std::string, vec3>>::iterator nextIt = std::next(it);
    
    if (currentTime >= nextIt->first) {
        currentFrameTime = nextIt->first;
        if (keyframes.find(currentFrameTime) != keyframes.end()) {
            for (const auto& [boneName, values] : keyframes[currentFrameTime]) {
                actualPose[boneName] = values;                
            }
            printActualPose();
        }
    }
}