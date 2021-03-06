
// ----------------------------------------------------------------------------
// This file is part of the Ducttape Project (http://ducttape-dev.org) and is
// licensed under the GNU LESSER PUBLIC LICENSE version 3. For the full license
// text, please see the LICENSE file in the root of this project or at
// http://www.gnu.org/licenses/lgpl.html
// ----------------------------------------------------------------------------

#include "MusicPauseEvent.hpp"

namespace dt {

MusicPauseEvent::MusicPauseEvent() {}

const std::string MusicPauseEvent::GetType() const {
   return "DT_MUSICPAUSEEVENT";
}

std::shared_ptr<Event> MusicPauseEvent::Clone() const {
    std::shared_ptr<Event> ptr(new MusicPauseEvent());
    return ptr;
}

}
