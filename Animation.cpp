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

int convertTimeToFrame(float time) {
    return static_cast<int>(time * 60);
}

void Animation::convertToKeyframes(PlayState state, ParseTransitionPose transitionPose) {
    pl[state].duration = convertTimeToFrame(transitionPose.duration);
    for (const auto& [boneName, keyframesList] : transitionPose.tracks) {
        for (const auto& keyframesMap : keyframesList) {
            for (const auto& [time, values] : keyframesMap) {
                pl[state].keyframes[convertTimeToFrame(time)][boneName] = values;
            }
        }
    }
    if (pl[state].keyframes.find(convertTimeToFrame(transitionPose.duration)) == pl[state].keyframes.end()) {
        pl[state].keyframes[convertTimeToFrame(transitionPose.duration)] = {};
    }
    if (pl[state].keyframes.find(0) == pl[state].keyframes.end()) {
        pl[state].keyframes[0] = {};
    }
}

void Animation::flipflop() {
    if (playState == PlayState::LOOP) {
        nextState = PlayState::FINISH;
    } else if (playState == PlayState::FINISH) {
        nextState = PlayState::START;
    }
    else if (playState == PlayState::STOPPED) {
        playState = PlayState::START;
        nextState = PlayState::LOOP;
        updateActualPose(0.0f);
    }
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
    updateActualPose(currentFrameTime);
}

void Animation::updateActualPose(int frameTime) {
    auto it = pl[playState].keyframes.find(frameTime);
    if (it != pl[playState].keyframes.end()) {
        // printf("Updating actual pose for frame time: %f\n", frameTime);
        for (const auto& [boneName, values] : it->second) {
            // printf("Updating actual pose for bone: %s to values (%f, %f, %f), time is %f\n", boneName.c_str(), values[0], values[1], values[2], currentFrameTime);
            actualPose[boneName] = values;
        }
    }
}

void Animation::update() {
    if (playState == PlayState::STOPPED) return;

    currentTime += 1;

    if (currentTime >= pl[playState].duration) {
        finishFrame();
        return;
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
        updateActualPose(nextIt->first);
        currentFrameTime = nextIt->first;
    }
}

void AnimManager::changeAnim(int index) {
     if (index < 0 || index >= static_cast<int>(anims.size())) {
        std::cerr << "Invalid animation index: " << index << std::endl;
        return;
    }
    if (currentAnim == index) {
        anims[index].flipflop();
        return;
    }
    else if (currentAnim == -1)
    {
        currentAnim = index;
        anims[index].flipflop();
    }
}
