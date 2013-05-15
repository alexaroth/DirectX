#include"SoundEngine.h"


SoundEngine::SoundEngine()
{
    engine = createIrrKlangDevice();
    ambient = engine->addSoundSourceFromFile("bkg.mp3");
    walk = engine->addSoundSourceFromFile("walk.mp3");		

    engine -> setListenerPosition(vec3df(20,0,0), vec3df(0,0,1));
}

void SoundEngine::playAmbient()
{
    ambient -> setDefaultVolume(0.4);

    ambientS = engine -> play2D(ambient,true,false,true,true);
    ambientS -> setPlaybackSpeed(1);
}


void SoundEngine::playWalk()
{
    walk_sound = engine -> play3D(walk,vec3df(0,0,0), true, false, false, true); 
}

void SoundEngine::pause_Walk()
{
    if(walk_sound -> getIsPaused() == false)
	{
        walk_sound -> setIsPaused(true);
    }
}

void SoundEngine::pause_resume_ambient()
{
    if(ambientS -> getIsPaused() == false)
	{
        ambientS -> setIsPaused(true);
    }

    else
    {
        ambientS -> setIsPaused(false);
    }
}

void SoundEngine::resume_Walk()
{
    if(walk_sound -> getIsPaused() == true)
    {
        walk_sound -> setPlaybackSpeed(1);
        walk_sound -> setIsPaused(false);
    }
}

void SoundEngine::drop_engines()
{
    engine -> drop(); 
}