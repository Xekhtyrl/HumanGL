#include "Includes/header.h"

void frameGlobalTransform(HierarchicModel* modelPtr, MNode* node) {
    if (!node->parent.empty()) {
        vml::mat4 parentGlobalTransform = modelPtr->getNode(node->parent)->globalTransform;	
        node->globalTransform = parentGlobalTransform * node->localTransform;
    }
    else
        node->globalTransform = node->localTransform;
}

void frameChildTransforms(HierarchicModel* modelPtr, MNode* node) {
    for (const std::string& childName : node->children) {
        MNode* childNode = modelPtr->getNode(childName);
        frameGlobalTransform(modelPtr, childNode);
        vml::vec4 pivotWorldHomogeneous = childNode->globalTransform * vml::vec4(childNode->pivotLocal, 1.0f);
        childNode->pivotWorld = vml::vec3({pivotWorldHomogeneous[0], pivotWorldHomogeneous[1], pivotWorldHomogeneous[2]});
        frameChildTransforms(modelPtr, childNode);
    }
}

void rotateNode(IModel* object, MNode* node, float angle, vec3 axis)
{
    HierarchicModel* modelPtr = dynamic_cast<HierarchicModel*>(object);
    if (!modelPtr || !node)
        return;

    node->localTransform = rotation_pivot(radians(angle), axis, node->pivotLocal) * node->localTransform;

    frameGlobalTransform(modelPtr, node);
    frameChildTransforms(modelPtr, node);
}
