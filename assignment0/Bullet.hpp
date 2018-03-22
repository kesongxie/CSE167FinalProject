//
//  Bullet.hpp
//  assignment0
//
//  Created by Xie kesong on 3/17/18.
//  Copyright © 2018 ___CSE167___. All rights reserved.
//

#ifndef Bullet_hpp
#define Bullet_hpp

#include <stdio.h>
#include "OBJObject.h"
#include "Model.hpp"

class Bullet {
public:
    Bullet(OBJObject *bulletObj, Model *target);
    bool isActive;
    void draw();
    void move(glm::vec3 direction);
    void recycleIfNeeded();
    void adjustPos();
    glm::vec3 targetCenter;
    OBJObject *obj;
private:
    GLuint bulletshaderProgram;
    float shootingDistance;
    Model *target;
    glm::mat4 transform;
    glm::mat4 initTransform;
};

#endif /* Bullet_hpp */
