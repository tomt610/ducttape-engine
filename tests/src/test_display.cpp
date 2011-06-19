#include "Root.hpp"
#include "game/Game.hpp"
#include "scene/Scene.hpp"
#include "scene/Node.hpp"
#include "component/MeshComponent.hpp"
#include "component/PlaneComponent.hpp"
#include "component/LightComponent.hpp"
#include "event/EventListener.hpp"

class Game : public dt::Game, public dt::EventListener {
public:
    Game()
        : mScene("gamescene") {
        mRuntime = 0;
    }

    void HandleEvent(dt::Event* e) {
        if(e->GetType() == "DT_BEGINFRAMEEVENT") {
            mRuntime += ((dt::BeginFrameEvent*)e)->GetFrameTime();
            if(mRuntime > 5000) {
                RequestShutdown();
            }
        }
    }

    void OnInitialize() {
        dt::Root::get_mutable_instance().GetEventManager()->AddListener(&mScene);

        dt::Root::get_mutable_instance().GetResourceManager()->AddResourceLocation("../data/sinbad.zip","Zip", true);
        Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

        dt::Root::get_mutable_instance().GetEventManager()->AddListener(this);

        mScene.AddChildNode(new dt::Node("camnode"));
        mScene.FindChildNode("camnode", false)->AddComponent(new dt::CameraComponent("cam"));
        mScene.FindChildNode("camnode", false)->SetPosition(Ogre::Vector3(0, 10, 100));
        mScene.FindChildNode("camnode", false)->FindComponent<dt::CameraComponent>("cam")->LookAt(Ogre::Vector3(0, 0, 0));

        mScene.AddChildNode(new dt::Node("meshnode"));
        dt::MeshComponent* mesh = new dt::MeshComponent("lolmesh", "Sinbad.mesh");
        mScene.FindChildNode("meshnode", false)->AddComponent(mesh);

        /* std::cout << "Available Animations: ";
        for(std::string s: mesh->GetAvailableAnimations()) {
            std::cout << s << " ";
        }
        std::cout << std::endl; */
        mesh->SetAnimation("Dance");
        mesh->SetLoopAnimation(true);
        mesh->PlayAnimation();

        mScene.AddChildNode(new dt::Node("lightnode"));
        mScene.FindChildNode("lightnode", false)->AddComponent(new dt::LightComponent("light"));
        mScene.FindChildNode("lightnode", false)->SetPosition(Ogre::Vector3(0, 30, 0));

        mScene.AddChildNode(new dt::Node("lightnode2"));
        mScene.FindChildNode("lightnode2", false)->AddComponent(new dt::LightComponent("light2"));
        mScene.FindChildNode("lightnode2", false)->SetPosition(Ogre::Vector3(0, -10, 0));

        //Currently adding a plane would just look horrible till someone will implement the feature to add a material to a mesh or, in this case, a plane.
        /*mScene.AddChildNode(new dt::Node("planenode"));
        mScene.FindChildNode("planenode", false)->AddComponent(new dt::PlaneComponent("plane"));
        mScene.FindChildNode("planenode", false)->SetPosition(Ogre::Vector3(0, -10, 0));*/
    }

private:
    uint32_t mRuntime;
    dt::Scene mScene;

};

int main() {
    Game g;
    g.Run();
    return 0;
}
