
// ----------------------------------------------------------------------------
// This file is part of the Ducttape Project (http://ducttape-dev.org) and is
// licensed under the GNU LESSER PUBLIC LICENSE version 3. For the full license
// text, please see the LICENSE file in the root of this project or at
// http://www.gnu.org/licenses/lgpl.html
// ----------------------------------------------------------------------------

#include "ConnectionsManager.hpp"

#include "Root.hpp"

namespace dt {

ConnectionsManager::ConnectionsManager(ConnectionsManager::ID_t max_connections) {
    mMaxConnections = max_connections;
    mPingInterval = 1.0;
    SetTimeout(10.0);
}

ConnectionsManager::~ConnectionsManager() {}

void ConnectionsManager::Initialize() {
    Root::get_mutable_instance().GetEventManager()->AddListener(this);
    SetPingInterval(mPingInterval); // this starts the timer
}

void ConnectionsManager::Deinitialize() {
    Root::get_mutable_instance().GetEventManager()->RemoveListener(this);
}

void ConnectionsManager::SetMaxConnections(ConnectionsManager::ID_t max) {
    mMaxConnections = max;
}

ConnectionsManager::ID_t ConnectionsManager::GetMaxConnections() const {
    return mMaxConnections;
}

bool ConnectionsManager::IsKnownConnection(Connection c) {
    return GetConnectionID(c) != 0;
}

ConnectionsManager::ID_t ConnectionsManager::AddConnection(Connection* c) {
    ConnectionsManager::ID_t id = _GetNewID();
    if(id != 0)
        mConnections[id] = *c;
    return id;
}

void ConnectionsManager::RemoveConnection(ConnectionsManager::ID_t id) {
    if(GetConnection(id) != nullptr) {
        mConnections.erase(id);
    }
}

void ConnectionsManager::RemoveConnection(Connection c) {
    RemoveConnection(GetConnectionID(c));
}

ConnectionsManager::ID_t ConnectionsManager::GetConnectionID(Connection c) {
    // run through connections in a loop
    for(boost::ptr_map<ConnectionsManager::ID_t, Connection>::iterator i = mConnections.begin(); i != mConnections.end(); ++i) {
        if(i->second->GetPort() == c.GetPort() && i->second->GetIPAddress() == c.GetIPAddress()) {
            return i->first;
        }
    }

    // nothing found, return 0
    return 0;
}

Connection* ConnectionsManager::GetConnection(ConnectionsManager::ID_t id) {
    if(mConnections.count(id) > 0)
        return mConnections.find(id)->second;
    else
        return nullptr;
}

std::vector<Connection*> ConnectionsManager::GetAllConnections() {
    std::vector<Connection*> result;

    for(boost::ptr_map<ConnectionsManager::ID_t, Connection>::iterator i = mConnections.begin(); i != mConnections.end(); ++i) {
        result.push_back(i->second);
    }

    return result;
}

double ConnectionsManager::GetPing(ConnectionsManager::ID_t connection) {
    return mPings[connection];
}

ConnectionsManager::ID_t ConnectionsManager::_GetNewID() {
    if(mMaxConnections == 0) {
        // yeah maximum fun! sorry, we only support 16 bit IDs, so maximum connections limit
        // is at 65535
        mMaxConnections = 65535;
    }

    // run until we find an empty one
    for(ConnectionsManager::ID_t i = 1; i <= mMaxConnections; ++i) {
        if(GetConnection(i) == nullptr)
            return i;
    }

    // no ID left -> all slots used
    return 0;
}

uint16_t ConnectionsManager::GetConnectionCount() {
    return mConnections.size();
}

void ConnectionsManager::SetPingInterval(double ping_interval) {
    mPingInterval = ping_interval;
    // reset the timer
    if(mPingTimer.get() != nullptr) {
        mPingTimer->Stop();
    }
    if(mPingInterval != 0) {
        mPingTimer = std::shared_ptr<Timer>(new Timer("DT_SEND_PING", mPingInterval, true, false));
    }
}

double ConnectionsManager::GetPingInterval() {
    return mPingInterval;
}

void ConnectionsManager::SetTimeout(double timeout) {
    mTimeout = timeout;
}

double ConnectionsManager::GetTimeout() {
    return mTimeout;
}

void ConnectionsManager::HandleEvent(std::shared_ptr<Event> e) {
    if(e->GetType() == "DT_TIMERTICKEVENT") {
        std::shared_ptr<TimerTickEvent> t = std::dynamic_pointer_cast<TimerTickEvent>(e);
        if(t->GetMessageEvent() == "DT_SEND_PING" && t->GetInterval() == mPingInterval) {
            // this is our timer
            _Ping();
            _CheckTimeouts();
        }
    } else if(e->GetType() == "DT_PINGEVENT") {
        std::shared_ptr<PingEvent> p = std::dynamic_pointer_cast<PingEvent>(e);
        if(p->IsLocalEvent()) {
            // yes, we received this from the network
            if(p->IsReply()) {
                _HandlePing(p);
            } else {
                // just reply!
                // time's crucial, send directly
                std::shared_ptr<PingEvent> ping(new PingEvent(p->GetTimestamp(), true));
                Root::get_mutable_instance().GetNetworkManager()->QueueEvent(ping);
                Root::get_mutable_instance().GetNetworkManager()->SendQueuedEvents();
            }
        }
    }

    if(e->IsNetworkEvent()) {
        std::shared_ptr<NetworkEvent> n = std::dynamic_pointer_cast<NetworkEvent>(e);
        if(n->IsLocalEvent()) {
            // we received a network event
            mLastActivity[n->GetSenderID()] = Root::get_mutable_instance(). GetTimeSinceInitialize();
        }
    }
}

void ConnectionsManager::_Ping() {
    Root::get_mutable_instance().GetEventManager()->InjectEvent(
                new PingEvent(Root::get_mutable_instance().GetTimeSinceInitialize()));
}

void ConnectionsManager::_HandlePing(std::shared_ptr<PingEvent> ping_event) {
    uint32_t ping = Root::get_mutable_instance().GetTimeSinceInitialize() - ping_event->GetTimestamp();
    mPings[ping_event->GetSenderID()] = ping;

    Logger::Get().Debug("Ping for connection #" + tostr(ping_event->GetSenderID()) + ": " + tostr(ping));
}

void ConnectionsManager::_CheckTimeouts() {
    uint32_t time = Root::get_mutable_instance().GetTimeSinceInitialize();
    for(auto i = mConnections.begin(); i != mConnections.end(); ++i) {
        uint32_t diff = time - mLastActivity[i->first];
        if(diff > mTimeout) {
            _TimeoutConnection(i->first);
        }
    }
}

void ConnectionsManager::_TimeoutConnection(ConnectionsManager::ID_t connection) {
    Logger::Get().Warning("Connection timed out: " + tostr(connection));

    uint32_t diff = Root::get_mutable_instance().GetTimeSinceInitialize() - mLastActivity[connection];

    // Send the event, hoping it will arrive at the destination
    std::shared_ptr<GoodbyeEvent> e(new GoodbyeEvent("Timeout after " + tostr(diff) + " seconds."));
    e->ClearRecipients();
    e->AddRecipient(connection);
    // send it directly
    Root::get_mutable_instance().GetNetworkManager()->QueueEvent(e);
    Root::get_mutable_instance().GetNetworkManager()->SendQueuedEvents();

    RemoveConnection(connection);
}

}
