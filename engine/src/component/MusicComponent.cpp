
// ----------------------------------------------------------------------------
// This file is part of the Ducttape Project (http://ducttape-dev.org) and is
// licensed under the GNU LESSER PUBLIC LICENSE version 3. For the full license
// text, please see the LICENSE file in the root of this project or at
// http://www.gnu.org/licenses/lgpl.html
// ----------------------------------------------------------------------------

#include "MusicComponent.hpp"

#include "utils/Logger.hpp"

namespace dt {

MusicComponent::MusicComponent(const std::string& name, const std::string& music_file)
    : Component(name) {
    mMusicFile = music_file;
    _LoadMusic();
}

void MusicComponent::HandleEvent(std::shared_ptr<Event> e) {
    auto resmgr = Root::get_mutable_instance().GetResourceManager();
    if(e->GetType() == "DT_MUSICPAUSEEVENT") {
        if(resmgr->GetMusicFile(mMusicFile)->GetStatus() == sf::Music::Paused) {
            resmgr->GetMusicFile(mMusicFile)->Play();
        } else {
            resmgr->GetMusicFile(mMusicFile)->Pause();
        }
    } else if(e->GetType() == "DT_MUSICSTOPEVENT") {
        resmgr->GetMusicFile(mMusicFile)->Stop();
    } else if(e->GetType() == "DT_MUSICSTARTEVENT") {
        resmgr->GetMusicFile(mMusicFile)->Stop();
        resmgr->GetMusicFile(mMusicFile)->Play();
    }
}

void MusicComponent::OnCreate() {
   _PlayMusic();
}

void MusicComponent::OnDestroy() {
    _StopMusic();
}

void MusicComponent::OnUpdate(double time_diff) {

}

void MusicComponent::SetMusicFile(const std::string& music_file) {
    if(music_file != mMusicFile && IsCreated()) {
        // we got a new music; load it
        _LoadMusic();
    }
    mMusicFile = music_file;
}

const std::string& MusicComponent::GetMusicFile() const {
    return mMusicFile;
}

void MusicComponent::_LoadMusic() {
    if(mMusicFile == "") {
        Logger::Get().Error("MusicComponent [" + mName + "]: Needs a music file.");
    }
    if(!Root::get_mutable_instance().GetResourceManager()->AddMusicFile(mMusicFile)) {
        Logger::Get().Error("MusicComponent [" + mName + "]: Wasn't able to load music file [" + mMusicFile + "].");
    }
}

void MusicComponent::_PlayMusic() {
    // play music if possible
    Root::get_mutable_instance().GetResourceManager()->GetMusicFile(mMusicFile)->Play();
}

void MusicComponent::_StopMusic() {
    // stop music if possible
    Root::get_mutable_instance().GetResourceManager()->GetMusicFile(mMusicFile)->Stop();
}

}
