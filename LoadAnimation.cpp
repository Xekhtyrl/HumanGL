#include "Animation.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>

static std::map<std::string, std::vector<std::map<float, vec3>>> parseBoneTracks(const std::string& sectionStr) {
    std::map<std::string, std::vector<std::map<float, vec3>>> result;

    size_t pos = 0;
    while ((pos = sectionStr.find("\"", pos)) != std::string::npos) {
        size_t nameEnd = sectionStr.find("\"", pos + 1);
        if (nameEnd == std::string::npos) break;

        std::string boneName = sectionStr.substr(pos + 1, nameEnd - pos - 1);

        if (boneName == "duration") {
            pos = nameEnd + 1;
            continue;
        }

        size_t boneStart = sectionStr.find("{", nameEnd);
        if (boneStart == std::string::npos) break;

        size_t boneEnd = sectionStr.find("}", boneStart);
        if (boneEnd == std::string::npos) break;

        std::string boneStr = sectionStr.substr(boneStart + 1, boneEnd - boneStart - 1);

        std::map<float, vec3> keyframes;

        size_t kfPos = 0;
        while ((kfPos = boneStr.find("\"", kfPos)) != std::string::npos) {
            size_t timeEnd = boneStr.find("\"", kfPos + 1);
            if (timeEnd == std::string::npos) break;

            float time = std::stof(boneStr.substr(kfPos + 1, timeEnd - kfPos - 1));

            size_t arrayStart = boneStr.find("[", timeEnd);
            if (arrayStart == std::string::npos) break;

            size_t arrayEnd = boneStr.find("]", arrayStart);
            if (arrayEnd == std::string::npos) break;

            std::string arrayStr = boneStr.substr(arrayStart + 1, arrayEnd - arrayStart - 1);

            float x, y, z;
            char comma;
            std::istringstream iss(arrayStr);
            iss >> x >> comma >> y >> comma >> z;

            keyframes[time] = vec3{x, y, z};
            kfPos = arrayEnd + 1;
        }

        if (!keyframes.empty()) {
            result[boneName].push_back(keyframes);
        }
        pos = boneEnd + 1;
    }

    return result;
}

static float parseDurationInSection(const std::string& sectionStr) {
    size_t durationPos = sectionStr.find("\"duration\"");
    if (durationPos == std::string::npos) return 0.0f;

    size_t colonPos = sectionStr.find(":", durationPos);
    if (colonPos == std::string::npos) return 0.0f;

    size_t numStart = sectionStr.find_first_of("-0123456789", colonPos);
    if (numStart == std::string::npos) return 0.0f;

    size_t numEnd = sectionStr.find_first_not_of("-0123456789.", numStart);
    if (numEnd == std::string::npos) numEnd = sectionStr.size();

    return std::stof(sectionStr.substr(numStart, numEnd - numStart));
}

static size_t findMatchingBrace(const std::string& content, size_t openBrace) {
    int depth = 1;
    size_t pos = openBrace + 1;
    while (pos < content.size() && depth > 0) {
        if (content[pos] == '{') depth++;
        else if (content[pos] == '}') depth--;
        pos++;
    }
    return pos - 1;
}

static ParseTransitionPose parseTransitionPose(const std::string& content, const std::string& sectionName) {
    ParseTransitionPose pose;

    size_t sectionPos = content.find("\"" + sectionName + "\"");
    if (sectionPos == std::string::npos) return pose;

    size_t sectionStart = content.find("{", sectionPos);
    if (sectionStart == std::string::npos) return pose;

    size_t sectionEnd = findMatchingBrace(content, sectionStart);
    std::string sectionStr = content.substr(sectionStart + 1, sectionEnd - sectionStart - 1);

    pose.duration = parseDurationInSection(sectionStr);
    pose.tracks = parseBoneTracks(sectionStr);

    return pose;
}

Animation loadAnimation(const std::string& filepath) {
    std::ifstream file(filepath);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    Animation animation;
    printf("Loading animation from %s\n", filepath.c_str());
    size_t statePos = content.find("\"state\"");
    if (statePos != std::string::npos) {
        size_t colonPos = content.find(":", statePos);
        size_t quoteStart = content.find("\"", colonPos);
        size_t quoteEnd = content.find("\"", quoteStart + 1);
        animation.state = content.substr(quoteStart + 1, quoteEnd - quoteStart - 1);
    }
    
    // animation.duration = parseDurationInSection(content);
    // size_t durationPos = content.find("\"duration\"");
    // if (durationPos != std::string::npos) {
    //     size_t colonPos = content.find(":", durationPos);
    //     size_t numStart = content.find_first_of("0123456789", colonPos);
    //     size_t numEnd = content.find_first_not_of("0123456789.", numStart);
    //     animation.duration = std::stof(content.substr(numStart, numEnd - numStart));
    // }
    
    // size_t tracksPos = content.find("\"tracks\"");
    // if (tracksPos != std::string::npos) {
    //     size_t tracksStart = content.find("{", tracksPos);
    //     size_t tracksEnd = findMatchingBrace(content, tracksStart);
    //     std::string tracksStr = content.substr(tracksStart + 1, tracksEnd - tracksStart - 1);
    //     animation.tracks = parseBoneTracks(tracksStr);
    // }

    animation.convertToKeyframes(PlayState::START, parseTransitionPose(content, "start"));
    animation.convertToKeyframes(PlayState::LOOP, parseTransitionPose(content, "loop"));
    animation.convertToKeyframes(PlayState::FINISH, parseTransitionPose(content, "finish"));
    animation.print();
    dprintf(1, "aaa\n");


    return animation;
}
