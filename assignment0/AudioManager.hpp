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
#include <string>
#include <AL/alut.h>





class AudioManager {
public:
    static void loadSound(std::string filename);
    static void openALPlay(const char *filename, bool loop) {
        
        ALuint buffer, source;
        // Initialize the environment
        alutInit(0, NULL);
        
        // Capture errors
        alGetError();
        
        // Load pcm data into buffer
        
        buffer = alutCreateBufferFromFile(filename);
        
        // Create sound source (use buffer to fill source)
        alGenSources(1, &source);
        alSourcei(source, AL_BUFFER, buffer);
        
        // Play
        alSourcePlay(source);
        if(loop) {
            alSourcei(source,AL_LOOPING,AL_TRUE);
        } else {
            alSourcei(source,AL_LOOPING,AL_FALSE);
        }
    }
    
    static void playBackgroundMusic() {
        openALPlay("bg2.wav", true);
        openALPlay("flying.wav", true);
    }
    
};

#endif /* AudioManager_hpp */
