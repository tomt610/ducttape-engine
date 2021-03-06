#include "InputManager.hpp"

#include "Root.hpp"

namespace dt {

InputManager::InputManager() {
    mInputSystem = nullptr;
    mJailInput = false;
}

void InputManager::Initialize(Ogre::RenderWindow* window) {
    if(mInputSystem != nullptr) {
        Logger::Get().Warning("Input system already initialized. Aborting.");
        return;
    }

    mWindow = window;

    OIS::ParamList params;

    // getting window handle
    size_t window_handle = 0;
    mWindow->getCustomAttribute("WINDOW", &window_handle);
    params.insert(std::make_pair(std::string("WINDOW"), tostr(window_handle)));
    if(!mJailInput) {
#if defined OIS_WIN32_PLATFORM
        params.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND" )));
        params.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
        params.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
        params.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));
#elif defined OIS_LINUX_PLATFORM
        params.insert(std::make_pair(std::string("x11_mouse_grab"), std::string("false")));
        params.insert(std::make_pair(std::string("x11_mouse_hide"), std::string("false")));
        params.insert(std::make_pair(std::string("x11_keyboard_grab"), std::string("false")));
        params.insert(std::make_pair(std::string("XAutoRepeatOn"), std::string("true")));
#endif
    }

    Logger::Get().Info("Initializing input system (Window: " + tostr(window_handle) + ")");
    mInputSystem = OIS::InputManager::createInputSystem(params);

    mKeyboard = static_cast<OIS::Keyboard*>(mInputSystem->createInputObject(OIS::OISKeyboard, true));
    mMouse = static_cast<OIS::Mouse*>(mInputSystem->createInputObject(OIS::OISMouse, true));

    mKeyboard->setEventCallback(this);
    mMouse->setEventCallback(this);

    // Set initial mouse clipping size
    windowResized(mWindow);

    // Register as a Window listener
    Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);
}

void InputManager::Deinitialize() {
    if(mInputSystem != nullptr) {
        mInputSystem->destroyInputObject(mMouse);
        mInputSystem->destroyInputObject(mKeyboard);
        OIS::InputManager::destroyInputSystem(mInputSystem);
        mInputSystem = nullptr;
    }
}

void InputManager::Capture() {
    if(mInputSystem != nullptr) {
        // the system has been initialized
        mMouse->capture();
        mKeyboard->capture();
    }
}

void InputManager::SetJailInput(bool jail_input) {
    mJailInput =  jail_input;
}

bool InputManager::GetJailInput() const {
    return mJailInput;
}


bool InputManager::keyPressed(const OIS::KeyEvent& event) {
    Root::get_mutable_instance().GetEventManager()->InjectEvent(new KeyboardEvent(KeyboardEvent::PRESSED, event.key, event.text));
    return true;
}

bool InputManager::keyReleased(const OIS::KeyEvent& event) {
    Root::get_mutable_instance().GetEventManager()->InjectEvent(new KeyboardEvent(KeyboardEvent::RELEASED, event.key, event.text));
    return true;
}

bool InputManager::mouseMoved(const OIS::MouseEvent& event) {
    Root::get_mutable_instance().GetEventManager()->InjectEvent(new MouseEvent(MouseEvent::MOVED, event.state));
    return true;
}

bool InputManager::mousePressed(const OIS::MouseEvent& event, OIS::MouseButtonID button) {
    Root::get_mutable_instance().GetEventManager()->InjectEvent(new MouseEvent(MouseEvent::PRESSED, event.state, button));
    return true;
}

bool InputManager::mouseReleased(const OIS::MouseEvent& event, OIS::MouseButtonID button) {
    Root::get_mutable_instance().GetEventManager()->InjectEvent(new MouseEvent(MouseEvent::RELEASED, event.state, button));
    return true;
}

void InputManager::windowResized(Ogre::RenderWindow* window) {
    // update the mouse capture width / height

    unsigned int width, height, depth;
    int left, top;
    window->getMetrics(width, height, depth, left, top);

    const OIS::MouseState &ms = mMouse->getMouseState();
    ms.width = width;
    ms.height = height;
}

void InputManager::windowClosed(Ogre::RenderWindow* window) {
    // Only close for window that created OIS
    if(window == mWindow) {
        Logger::Get().Info("The window was closed");
        Root::get_mutable_instance().GetEventManager()->InjectEvent(new WindowClosedEvent());
    }
}

}
