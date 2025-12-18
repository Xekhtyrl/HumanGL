#ifndef IMODEL_HPP_
#define IMODEL_HPP_

#include "Shader.hpp"

class Shader;

// Class declaration
class IModel {
 public:
  virtual void Draw(Shader& shader) = 0;
  virtual ~IModel() = 0;  
};

#endif // IMODEL_HPP_
