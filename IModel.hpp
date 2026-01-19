#ifndef IMODEL_HPP_
#define IMODEL_HPP_

#include "Shader.hpp"

class Shader;

// Class declaration
class IModel {
 public:
  virtual ~IModel() {}
  virtual void Draw(Shader& shader) = 0;
  virtual vec3 min() = 0;
  virtual vec3 max() = 0;
};

#endif // IMODEL_HPP_
