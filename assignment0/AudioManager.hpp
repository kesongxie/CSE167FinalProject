//
//  AudioManager.hpp
//  assignment0
//
//  Created by Xie kesong on 3/17/18.
//  Copyright © 2018 ___CSE167___. All rights reserved.
//

#ifndef AudioManager_hpp
#define AudioManager_hpp

#include <stdio.h>
#include <OpenAL/OpenAL.h>
#include <string>

class AudioManager {
public:
    void loadSound(std::string filename);
};

#endif /* AudioManager_hpp */
