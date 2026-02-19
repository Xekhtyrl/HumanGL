#include "Includes/header.h"


void rotateNode(MNode* node, vec3 angles)
{
    if (node->name == "torso") {
        node->translation[0] += angles[0];
        // if (node->translation[1] + angles[1] > -1)
            node->translation[1] += angles[1];
        node->translation[2] += angles[2];
        node->updateLocalMatrix();
        return;
    }
    node->rotation[0] += angles[0];
    node->rotation[1] += angles[1];
    node->rotation[2] += angles[2];
    node->updateLocalMatrix();
}

void updateNodeWorldMatrix(HierarchicModel* modelPtr, MNode* node) {
    if (!modelPtr || !node)
        return;
    if (!node->parent.empty()) {
        vml::mat4 parentworldMatrix = modelPtr->getNode(node->parent)->worldMatrix;	
        node->worldMatrix = parentworldMatrix * node->localMatrix ;
    }
    else
        node->worldMatrix = node->localMatrix;

    for (const std::string& childName : node->children) {
        MNode* childNode = modelPtr->getNode(childName);
        updateNodeWorldMatrix(modelPtr, childNode);
    }
}

void updateNodeWorldMatrixModel(IModel* object, MNode* node) {
    HierarchicModel* modelPtr = dynamic_cast<HierarchicModel*>(object);
	if (!modelPtr)
		return;
    updateNodeWorldMatrix(modelPtr, node);
    
}
