
// ----------------------------------------------------------------------------
// This file is part of the Ducttape Project (http://ducttape-dev.org) and is
// licensed under the GNU LESSER PUBLIC LICENSE version 3. For the full license
// text, please see the LICENSE file in the root of this project or at
// http://www.gnu.org/licenses/lgpl.html
// ----------------------------------------------------------------------------

#include "Root.hpp"
#include "event/Timer.hpp"
#include "game/Game.hpp"

class CustomGame : public dt::Game {
public:
    void OnInitialize() {
        mTimer1Count = 0;
        mTimer2Count = 0;
        mTimer3Count = 0;

        std::cout << "TIMER: Starting 3 timers:" << std::endl;
        std::cout << "  1 - Event mode  - every second" << std::endl;
        std::cout << "  2 - Thread mode - every 2nd second" << std::endl;
        std::cout << "  3 - Thread mode - every second - Callback, no events" << std::endl;

        mTimer1 = std::shared_ptr<dt::Timer>(new dt::Timer("Timer 1 (event mode)", 1.0, true, false));
        mTimer2 = std::shared_ptr<dt::Timer>(new dt::Timer("Timer 2 (thread mode)", 2.01, true, true));
        mTimer3 = std::shared_ptr<dt::Timer>(new dt::Timer("Timer 3 (callback)", 1.0, true, true, false));
        mTimer3->BindSlot(boost::bind(&CustomGame::TimerCallback, this, _1));

        mTotalTime = 0;
    }

    void HandleEvent(std::shared_ptr<dt::Event> e) {
        dt::Game::HandleEvent(e);

        if(e->GetType() == "DT_TIMERTICKEVENT") {
            std::shared_ptr<dt::TimerTickEvent> t = std::dynamic_pointer_cast<dt::TimerTickEvent>(e);
            bool t1 = (t->GetMessageEvent() == "Timer 1 (event mode)");
            bool t2 = (t->GetMessageEvent() == "Timer 2 (thread mode)");

            if(t1) {
                mTimer1Count++;
                std::cout << "Timer tick " << mTimer1Count << ": " << t->GetMessageEvent() << std::endl;
            } else if(t2) {
                mTimer2Count++;
                std::cout << "Timer tick " << mTimer2Count << ": " << t->GetMessageEvent() << std::endl;
            }
        } else if(e->GetType() == "DT_BEGINFRAMEEVENT") {
            mTotalTime += std::dynamic_pointer_cast<dt::BeginFrameEvent>(e)->GetFrameTime();

            if(mTotalTime >= 10.0) {
                RequestShutdown();
                mTimer1->Stop();
                mTimer2->Stop();
                mTimer3->Stop();
            }
        }
    }

    void TimerCallback(const std::string& message) {
        mTimer3Count++;
        std::cout << "Timer tick " << mTimer3Count << ": " << message << std::endl;
    }

public:
    std::shared_ptr<dt::Timer> mTimer1;
    std::shared_ptr<dt::Timer> mTimer2;
    std::shared_ptr<dt::Timer> mTimer3;
    int mTimer1Count;
    int mTimer2Count;
    int mTimer3Count;

    double mTotalTime;
};

int main(int argc, char** argv) {
    CustomGame game;
    game.Run(argc, argv);

    // we expect to have at least 9 ticks of timer 1 and 4 ticks of timer 2 within 10 seconds
    if(game.mTimer1Count < 9) {
        std::cerr << "Timer1 (event) did not tick often enough (only " << game.mTimer1Count << " times)." << std::endl;
        return 1;
    }

    if(game.mTimer2Count < 4) {
        std::cerr << "Timer2 (thread) did not tick often enough (only " << game.mTimer1Count << " times)." << std::endl;
        return 1;
    }

    if(game.mTimer3Count < 9) {
        std::cerr << "Timer3 (thread, callback) did not tick often enough (only " << game.mTimer3Count << " times)." << std::endl;
        return 1;
    }

    std::cout << "Timers test finished." << std::endl;
    std::cout << "TIMERS: OK." << std::endl;

    return 0;
}
