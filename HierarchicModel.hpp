#ifndef HIERARCHICMODEL_HPP_
#define HIERARCHICMODEL_HPP_

#include "IModel.hpp"
#include "Shader.hpp"

// Class declaration
class HierarchicModel : public IModel{
	public:
		HierarchicModel();
		HierarchicModel(const HierarchicModel &other);
		HierarchicModel &operator=(const HierarchicModel &rhs);
		~HierarchicModel();
		void Draw(Shader& shader);

	private:
		
		
};

#endif // HIERARCHICMODEL_HPP_
