
// ----------------------------------------------------------------------------
// This file is part of the Ducttape Project (http://ducttape-dev.org) and is
// licensed under the GNU LESSER PUBLIC LICENSE version 3. For the full license
// text, please see the LICENSE file in the root of this project or at
// http://www.gnu.org/licenses/lgpl.html
// ----------------------------------------------------------------------------

#include "Timer.hpp"

#include "Root.hpp"

namespace dt {

Timer::Timer(const std::string& message, double interval, bool repeat, bool threaded, bool use_events) {
    mMessage = message;
    mInterval = interval;
    mThreaded = threaded;
    mRepeat = repeat;
    mUseEvents = use_events;

    if(threaded) {
        _RunThread();
    } else {
        Root::get_mutable_instance().GetEventManager()->AddListener(this);
        mTimeLeft = mInterval;
    }
}

void Timer::HandleEvent(std::shared_ptr<Event> e) {
    // for event mode

    if(e->GetType() == "DT_BEGINFRAMEEVENT") {
        // every frame
        std::shared_ptr<BeginFrameEvent> b = std::dynamic_pointer_cast<BeginFrameEvent>(e);

        if(mTimeLeft > b->GetFrameTime()) {
            mTimeLeft -= b->GetFrameTime();
        } else {
            // ignore if we have some delay (actually, this is the downside of event mode)
            mTimeLeft = 0;
            TriggerTickEvent();
        }
    }
}

void Timer::TriggerTickEvent() {
    if(mUseEvents)
        Root::get_mutable_instance().GetEventManager()->InjectEvent(new TimerTickEvent(mMessage, mInterval));
    mTickSignal(mMessage);

    if(mRepeat && mThreaded) {
        _RunThread();
    }

    if(!mThreaded) {
        if(!mRepeat) {
            // disable
            Root::get_mutable_instance().GetEventManager()->RemoveListener(this);
        } else {
            // reset
            mTimeLeft = mInterval;
        }
    }

}

double Timer::GetInterval() const {
    return mInterval;
}

const std::string& Timer::GetMessageEvent() const {
    return mMessage;
}

void Timer::_RunThread() {
    mThread = std::shared_ptr<sf::Thread>(new sf::Thread(&Timer::_ThreadFunction, this));
    mThread->Launch();
}

void Timer::_ThreadFunction(void* user_data) {
    Timer* timer = (Timer*)user_data;

    // wait for interval, convert to milliseconds for SFML
    sf::Sleep(timer->GetInterval() * 1000);

    // done, trigger event
    timer->TriggerTickEvent();
}

void Timer::Stop() {
    if(mThreaded) {
        mThread->Terminate();
    } else {
        Root::get_mutable_instance().GetEventManager()->RemoveListener(this);
    }
    mTimeLeft = mInterval; // reset
}

boost::signals2::connection Timer::BindSlot(boost::function<void (const std::string&)> slot) {
    return mTickSignal.connect(slot);
}

}
