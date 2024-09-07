// Termm--Fall 2023

#include "PhongMaterial.hpp"
#include <iostream>
#include "../../tools/traverse.hpp"

using namespace std;

PhongMaterial::PhongMaterial(
	const glm::vec3& kd, const glm::vec3& ks, double shininess )
	: m_kd(kd)
	, m_ks(ks)
	, m_shininess(shininess)
    , m_reflective(0.0) // by default set it to non reflective
{}

PhongMaterial::PhongMaterial(
	const glm::vec3& kd, const glm::vec3& ks, double shininess, double reflective
)
	: m_kd(kd)
	, m_ks(ks)
	, m_shininess(shininess)
    , m_reflective(reflective) // by default set it to non reflective
{
    // enforce it to be betwen 0 and 1
    m_reflective = std::max(0.0, reflective);
    m_reflective = std::min(1.0, reflective);
}

PhongMaterial::~PhongMaterial()
{}

void PhongMaterial::print_material() const {
    cout << "{ kd: ";
    print_vec(m_kd);
    cout << ", ks: ";
    print_vec(m_ks);
    cout << ", shine: " << m_shininess;
    cout << ", reflective: " << m_reflective << "}";
}

