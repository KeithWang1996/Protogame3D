#include "Engine/Core/GameObject.hpp"
#include "Engine/Physics/Rigidbody2D.hpp"

GameObject::~GameObject()
{
	// delete m_rigidbody; // this should be an error if setup right
	m_rigidbody->Destroy(); // destroys through the system;  
	m_rigidbody = nullptr;
}