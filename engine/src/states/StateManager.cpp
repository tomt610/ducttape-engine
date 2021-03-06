
// ----------------------------------------------------------------------------
// This file is part of the Ducttape Project (http://ducttape-dev.org) and is
// licensed under the GNU LESSER PUBLIC LICENSE version 3. For the full license
// text, please see the LICENSE file in the root of this project or at
// http://www.gnu.org/licenses/lgpl.html
// ----------------------------------------------------------------------------

#include <boost/foreach.hpp>

#include "StateManager.hpp"

namespace dt {

StateManager::StateManager() {}

void StateManager::SetNewState(State* new_state) {
    mNewState = std::shared_ptr<State>(new_state);
}

void StateManager::PushNewState() {
    mStates.push_back(mNewState.get());
    mNewState = std::shared_ptr<State>();
}

State* StateManager::GetCurrentState() {
    if(mStates.size() > 0)
        return &(mStates.back());
    return nullptr;
}

}
