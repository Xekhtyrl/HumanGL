#include "Includes/header.h"

void rotateNode(IModel* object, MNode* node, float angle, vec3 axis)
{
    HierarchicModel* modelPtr = dynamic_cast<HierarchicModel*>(object);
    if (!modelPtr || !node)
        return;

    node->localTransform = rotation_pivot(radians(angle), axis, node->pivot) * node->localTransform;

    for (const std::string& childName : node->children) {
        MNode* childNode = modelPtr->getNode(childName);
        rotateNode(object, childNode, angle, axis);
    }
}