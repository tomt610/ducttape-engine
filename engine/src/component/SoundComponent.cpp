#include "SoundComponent.hpp"
#include "Root.hpp"
#include "scene/Node.hpp"
#include "scene/Scene.hpp"

#include "utils/Logger.hpp"

namespace dt {

SoundComponent::SoundComponent(const std::string& name, 
                               const std::string& sound_file)
    : Component(name) {
    mSoundFile = sound_file;
    _LoadSound();
}

void SoundComponent::HandleEvent(Event* e) {
    if(e->GetType() == "DT_SOUNDSCONTROLEVENT") {
        SoundsControlEvent* s = (SoundsControlEvent*)e;

        if(s->GetAction() == SoundsControlEvent::PLAY) {
            PlaySound();
        } else if(s->GetAction() == SoundsControlEvent::PAUSE) {
            PauseSound();
        } else if(s->GetAction() == SoundsControlEvent::STOP) {
            StopSound();
        }
    }
}

void SoundComponent::OnActivate() {
    Root::get_mutable_instance().GetEventManager()->AddListener(this);
}

void SoundComponent::OnDeactivate() {
    Root::get_mutable_instance().GetEventManager()->RemoveListener(this);
}

void SoundComponent::OnUpdate(float time_diff) {
	mSound.SetPosition(mNode->GetPosition(Node::SCENE).x, 
                       mNode->GetPosition(Node::SCENE).y, 
                       mNode->GetPosition(Node::SCENE).z);
}

void SoundComponent::SetSoundFile(const std::string& sound_file) {
    if(sound_file != mSoundFile && IsActivated()) {
        // we got a new sound; load it
        _LoadSound();
    }
    mSoundFile = sound_file;
}

const std::string& SoundComponent::GetSoundFile() const {
    return mSoundFile;
}

sf::Sound& SoundComponent::GetSound() {
	return mSound;
}

void SoundComponent::PlaySound() {
    // play sound if possible
    mSound.Play();
}

void SoundComponent::PauseSound() {
    // pause sound if possible
    mSound.Pause();
}

void SoundComponent::StopSound() {
    // stop sound if possible
    mSound.Stop();
    mSound.SetPlayingOffset(0); // rewind
}

void SoundComponent::_LoadSound() {
    if(mSoundFile == "") {
        Logger::Get().Error("SoundComponent [" + mName + "]: Needs a sound file.");
    }
    if(!Root::get_mutable_instance().GetResourceManager()->AddSoundBuffer(mSoundFile)) {
        Logger::Get().Error("SoundComponent [" + mName + "]: Wasn't able to load sound file [" + mSoundFile + "].");
    } else {
		mSound.SetBuffer(Root::get_mutable_instance().GetResourceManager()->GetSoundBuffer(mSoundFile));
	}
}

}