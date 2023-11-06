#include "PhysicsEngine.h"


//glm::vec3 RandomReflect();



PhysicsEngine::PhysicsEngine()
{
}

PhysicsEngine::~PhysicsEngine()
{
}

void PhysicsEngine::AddPhysicsObjects(PhysicsObject* objs)
{

 
	physicsObjects.push_back(objs);

}

void PhysicsEngine::RemovePhysicsObject(PhysicsObject* removeElement) 
{
    std::vector<PhysicsObject*>::iterator it = std::find(physicsObjects.begin(), physicsObjects.end(), removeElement);
    if (it != physicsObjects.end())
    {
      
        physicsObjects.erase(it);
    }
    else
    {
        std::cout << "Physics object not found in the vector." << std::endl;
    }

  
}

void PhysicsEngine::Update(float deltaTime)
{
	timer += deltaTime;

	if (timer >= fixedTimeStep)
	{
		UpdatePhysics(deltaTime);

		timer = 0;
	}
}




void PhysicsEngine::UpdatePhysics(float deltatime)
{
    float inverseMass = 1.0f / 1;

    for (size_t i = 0; i < physicsObjects.size(); i++)
    {
        if (physicsObjects[i]->mode ==ObjectMode::STATIC)
        {
            continue;
        }
        collisionPoints.clear();
        std::vector<glm::vec3> normals;

        glm::vec3 gravity(0.0f, -9.81f * physicsObjects[i]->gravityValue, 0.0f);
        glm::vec3 deltaAcceleration = gravity * deltatime * physicsObjects[i]->inverseMass;

        physicsObjects[i]->velocity += deltaAcceleration;
        glm::vec3 deltaVelocity = physicsObjects[i]->velocity * deltatime;
        glm::vec3 predictedPos = physicsObjects[i]->GetPosition() + deltaVelocity;

        physicsObjects[i]->position = predictedPos;
        physicsObjects[i]->SetPosition(physicsObjects[i]->position);

        for (size_t j = 0; j < physicsObjects.size(); j++)
        {
            if (i == j)
            {
                continue;
            }
            else
            {
                std::vector<glm::vec3> perObjectCollisions;
                std::vector<glm::vec3> perObjectNormals;
                if (physicsObjects[i]->checkCollision(physicsObjects[j], perObjectCollisions, normals))
                {
                    collisionPoints.insert(collisionPoints.end(), perObjectCollisions.begin(), perObjectCollisions.end());
                    normals.insert(normals.end(), perObjectNormals.begin(), perObjectNormals.end());
                    if (collisionPoints.size() > 0)
                    {
                        if (physicsObjects[i]->collisionCallbool)
                        {
                            if (physicsObjects[i]->GetCollisionCall() != nullptr)
                            {
                                physicsObjects[i]->GetCollisionCall()(physicsObjects[j]);
                            }
                        }

                   
                }


       }
           


            }
        }

        if (collisionPoints.size() != 0)
        {
            glm::vec3 normal = glm::vec3(0.0f);

            for (size_t k = 0; k < normals.size(); k++)
            {
                normal += glm::normalize(normals[k]);
            }

            normal = normal / (float)(normals.size());

            //glm::vec3 incident = physicsObjects[i]->velocity;
            //float dotProduct = glm::dot(incident, normal);

            //if (dotProduct < 0) {
            //    normal = -normal;
            //    dotProduct = -dotProduct;
            //}

            float bounciness = 0.1f; // adjust this value as needed
            glm::vec3 reflected = glm::reflect(glm::normalize(physicsObjects[i]->velocity), normal) ;

            physicsObjects[i]->velocity = reflected * (/*physicsObjects[i]->bounciness **/ glm::length(physicsObjects[i]->velocity));
        }
    }
}



//glm::vec3 newDirection = -glm::normalize(physicsObjects[j]->model->transform.position - physicsObjects[i]->UpdateSphere().center);
//newDirection.z = 0.0f;
//newDirection = glm::normalize(newDirection * 2.0f + RandomReflect());
//physicsObjects[i]->velocity = newDirection * (float)(rand() % 10 + 1);
//
//
//glm::vec3 RandomReflect()
//{
//	float x = (((rand() % 1000) - 1) / 1000.0f) * 2.0f - 1.0f;
//	float y = (((rand() % 1000) - 1) / 1000.0f) * 2.0f - 1.0f;
//
//	return glm::normalize(glm::vec3(x, y, 0.0f));
//}