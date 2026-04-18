#pragma once
#include <SDL2/SDL_mixer.h>
#include <string>
#include <unordered_map>

class Audio {
public:
    static bool init() {
        Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
        return true;
    }
    
    static void shutdown() { Mix_CloseAudio(); }
    
    static Mix_Chunk* loadSound(const std::string& path) {
        return Mix_LoadWAV(path.c_str());
    }
    
    static void playSound(Mix_Chunk* sound, int loops = 0) {
        Mix_PlayChannel(-1, sound, loops);
    }
    
    static Mix_Music* loadMusic(const std::string& path) {
        return Mix_LoadMUS(path.c_str());
    }
    
    static void playMusic(Mix_Music* music, int loops = -1) {
        Mix_PlayMusic(music, loops);
    }
    
    static void stopMusic() { Mix_HaltMusic(); }
    static void setVolume(int volume) { Mix_Volume(-1, volume); }
    static void setMusicVolume(int volume) { Mix_VolumeMusic(volume); }
};
