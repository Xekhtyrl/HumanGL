#include "Animation.hpp"
#include <iostream>

void Animation::print() const {
    std::cout << "Animation State: " << state << "\n";
    for (const auto& [state, transitionPose] : pl) {
        std::cout << "  PlayState: " << static_cast<int>(state) << ", Duration: " << transitionPose.duration << "\n";
        std::cout << "    Keyframes:\n";
        for (const auto& [time, boneMap] : transitionPose.keyframes) {
            std::cout << "      Time: " << time << "\n";
            for (const auto& [boneName, values] : boneMap) {
                std::cout << "        Bone: " << boneName << " -> Values: (" 
                          << values[0] << ", " 
                          << values[1] << ", " 
                          << values[2] << ")\n";
            }
        }
    }
 
}



void Animation::convertToKeyframes(PlayState state, ParseTransitionPose transitionPose) {
    dprintf(1, "Converting to keyframes...\n");
    pl[state].duration = transitionPose.duration;
    for (const auto& [boneName, keyframesList] : transitionPose.tracks) {
        for (const auto& keyframesMap : keyframesList) {
            for (const auto& [time, values] : keyframesMap) {
                pl[state].keyframes[time][boneName] = values;
            }
        }
    }
    if (pl[state].keyframes.find(transitionPose.duration) == pl[state].keyframes.end()) {
        pl[state].keyframes[transitionPose.duration] = {};
    }
    if (pl[state].keyframes.find(0.0f) == pl[state].keyframes.end()) {
        pl[state].keyframes[0.0f] = {};
    }
}

// void Animation::printKeyframes() const {
//     std::cout << "Keyframes:\n";
//     for (const auto& [time, boneMap] : keyframes) {
//         std::cout << "  Time: " << time << "\n";
//         for (const auto& [boneName, values] : boneMap) {
//             std::cout << "    Bone: " << boneName << " -> Values: (" 
//                         << values[0] << ", " 
//                         << values[1] << ", " 
//                         << values[2] << ")\n";
//         }
//     }
// }

// void Animation::printActualPose() const {
//     std::cout << "Actual Pose:\n";
//     for (const auto& [boneName, values] : actualPose) {
//         std::cout << "    Bone: " << boneName << " -> Values: (" 
//                     << values[0] << ", " 
//                     << values[1] << ", " 
//                     << values[2] << ")\n";
//     }
// }

void Animation::flipflop() {
    if (playState == PlayState::LOOP) {
        nextState = PlayState::FINISH;
    } else if (playState == PlayState::FINISH) {
        nextState = PlayState::START;
    }
    else if (playState == PlayState::STOPPED) {
        playState = PlayState::START;
        nextState = PlayState::LOOP;
    }
    // print();
}

void Animation::finishFrame() {
    currentTime = 0.0f;
    currentFrameTime = 0.0f;
    playState = nextState;
    if (playState == PlayState::FINISH)
        nextState = PlayState::STOPPED;
    if (playState == PlayState::START)
        nextState = PlayState::LOOP;
    if (playState == PlayState::LOOP)
        nextState = PlayState::LOOP;
    actualPose = {};

    auto it = pl[playState].keyframes.find(currentFrameTime);
    if (it != pl[playState].keyframes.end()) {
        for (const auto& [boneName, values] : it->second) {
            printf("Setting bone: %s to values (%f, %f, %f)\n", boneName.c_str(), values[0], values[1], values[2]);
            actualPose[boneName] = values;
        }
        // printActualPose();
    }
    }

void Animation::update(float deltaTime) {
    if (playState == PlayState::STOPPED) return;

    currentTime += deltaTime;

    if (currentTime >= pl[playState].duration) {
        finishFrame();
    }

    auto it = pl[playState].keyframes.find(currentFrameTime);
    if (it == pl[playState].keyframes.end()) 
    {
        print();
        printf("No keyframe found for current frame time: %f, playState: %d\n", currentFrameTime, static_cast<int>(playState));
        return;
    }

    auto nextIt = std::next(it);
    if (nextIt == pl[playState].keyframes.end()) 
    {
        printf("No next keyframe found after current frame time: %f\n", currentFrameTime);
        return;
    }

    if (currentTime >= nextIt->first) {
        currentFrameTime = nextIt->first;
        for (const auto& [boneName, values] : nextIt->second) {
            actualPose[boneName] = values;
        }
        // printActualPose();
    }
}