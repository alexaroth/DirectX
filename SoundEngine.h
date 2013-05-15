#include <irrKlang.h>
using namespace irrklang;


class SoundEngine
{
  public: 
	SoundEngine();		

	
	void playAmbient();
	void stopAmbient();  
    void pause_resume_ambient();
	void playWalk();
	void pause_Walk();
	void resume_Walk();
	void drop_engines();

	  

private:

    ISoundEngine* engine; 
	ISoundSource* walk;
	ISoundSource* ambient;
	ISound* walk_sound;
	ISound* ambientS;
};