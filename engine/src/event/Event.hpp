
// ----------------------------------------------------------------------------
// This file is part of the Ducttape Project (http://ducttape-dev.org) and is
// licensed under the GNU LESSER PUBLIC LICENSE version 3. For the full license
// text, please see the LICENSE file in the root of this project or at
// http://www.gnu.org/licenses/lgpl.html
// ----------------------------------------------------------------------------

#ifndef DUCTTAPE_ENGINE_EVENT_EVENT
#define DUCTTAPE_ENGINE_EVENT_EVENT

#include <string>
#include <memory>

#include "network/IOPacket.hpp"

namespace dt {

/**
  * Abstract base class for any Event that is passed through the Event system.
  * @see EventManager
  * @see NetworkEvent
  */
class Event {
public:
    /**
      * Default constructor.
      */
    Event();

    /**
      * Returns the type string of the Event for serialization / identification for typecasts.
      * @returns The type string of the Event.
      */
    virtual const std::string GetType() const = 0;

    /**
      * Returns whether this Event is being sent over network. This is only applicable for Events being inherited from NetworkEvent.
      * @returns Whether this Event is being sent over network. (True for any Event inherited from NetworkEvent.)
      */
    virtual bool IsNetworkEvent() const;

    /**
      * Returns an exact copy of this Event. Used for the factory pattern in NetworkManager and for EventBindings.
      * @returns An exact copy of this Event.
      */
    virtual std::shared_ptr<Event> Clone() const = 0;

    /**
      * Returns the type ID of the Event. The ID is got from the StringManager using the type string returned by GetType().
      * @see Event::GetType()
      * @see StringManager
      * @returns The type ID of the Event.
      */
    uint32_t GetTypeID() const;

    /**
      * Serializes this Event into / from the IOPacket.
      * @param p The IOPacket to serialize into / from.
      */
 //   virtual void Serialize(IOPacket& p);
};

}

#endif
