#include "Animation.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>

Animation loadAnimation(const std::string& filepath) {
    std::ifstream file(filepath);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    Animation animation;
    
    size_t statePos = content.find("\"state\"");
    if (statePos != std::string::npos) {
        size_t colonPos = content.find(":", statePos);
        size_t quoteStart = content.find("\"", colonPos);
        size_t quoteEnd = content.find("\"", quoteStart + 1);
        animation.state = content.substr(quoteStart + 1, quoteEnd - quoteStart - 1);
    }
    
    size_t durationPos = content.find("\"duration\"");
    if (durationPos != std::string::npos) {
        size_t colonPos = content.find(":", durationPos);
        size_t numStart = content.find_first_of("0123456789", colonPos);
        size_t numEnd = content.find_first_not_of("0123456789.", numStart);
        animation.duration = std::stof(content.substr(numStart, numEnd - numStart));
    }
    
    size_t tracksPos = content.find("\"tracks\"");
    if (tracksPos != std::string::npos) {
        size_t tracksStart = content.find("{", tracksPos);
        size_t tracksEnd = content.rfind("}");
        std::string tracksStr = content.substr(tracksStart + 1, tracksEnd - tracksStart - 1);
        
        size_t pos = 0;
        while ((pos = tracksStr.find("\"", pos)) != std::string::npos) {
            size_t nameEnd = tracksStr.find("\"", pos + 1);
            std::string boneName = tracksStr.substr(pos + 1, nameEnd - pos - 1);
            
            if (boneName == "state" || boneName == "duration" || boneName == "tracks") {
                pos = nameEnd + 1;
                continue;
            }
            
            size_t boneStart = tracksStr.find("{", nameEnd);
            size_t boneEnd = tracksStr.find("}", boneStart);
            std::string boneStr = tracksStr.substr(boneStart + 1, boneEnd - boneStart - 1);
            
            animation.tracks[boneName] = {};
            std::map<float, vec3> keyframes;
            
            size_t kfPos = 0;
            while ((kfPos = boneStr.find("\"", kfPos)) != std::string::npos) {
                size_t timeEnd = boneStr.find("\"", kfPos + 1);
                float time = std::stof(boneStr.substr(kfPos + 1, timeEnd - kfPos - 1));
                
                size_t arrayStart = boneStr.find("[", timeEnd);
                size_t arrayEnd = boneStr.find("]", arrayStart);
                std::string arrayStr = boneStr.substr(arrayStart + 1, arrayEnd - arrayStart - 1);
                
                float x, y, z;
                char comma;
                std::istringstream iss(arrayStr);
                iss >> x >> comma >> y >> comma >> z;
                
                keyframes[time] = vec3{x, y, z};
                kfPos = arrayEnd + 1;
            }
            
            if (!keyframes.empty()) {
                animation.tracks[boneName].push_back(keyframes);
            }
            pos = boneEnd + 1;
        }
    }
    
    return animation;
}