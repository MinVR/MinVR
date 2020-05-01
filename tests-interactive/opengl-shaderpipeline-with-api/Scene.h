#ifndef Scene_h
#define Scene_h

#include <G3D/G3DAll.h>

class Entity : public GEntity {
private:
    Entity();

    Entity(const std::string& n, const PhysicsFrameSpline& frameSpline, 
        const ArticulatedModel::Ref& artModel, const ArticulatedModel::PoseSpline& artPoseSpline,
        const MD2Model::Ref& md2Model,
        const MD3Model::Ref& md3Model);

public:
    typedef ReferenceCountedPointer<Entity> Ref;

    static Entity::Ref create(const std::string& n, const PhysicsFrameSpline& frameSpline, const ArticulatedModel::Ref& m, const ArticulatedModel::PoseSpline& poseSpline);
    static Entity::Ref create(const std::string& n, const PhysicsFrameSpline& frameSpline, const MD2Model::Ref& m);
    static Entity::Ref create(const std::string& n, const PhysicsFrameSpline& frameSpline, const MD3Model::Ref& m);
};

/** Sample scene graph.

    Includes loading from a text file, and a GUI component for detecting
    and selecting scenes.

    G3D does not contain a Scene class in the API because it is a
    very application-specific role. This is a sample of how you 
    might begin to structure one to get you started.
*/
class Scene : public ReferenceCountedObject {
protected:
    /** Current time */
    GameTime                    m_time;
    Lighting::Ref               m_lighting;
    Texture::Ref                m_skyBox;
    Array<Entity::Ref>          m_entityArray;
	
    Scene() : m_time(0) {}

public:

    typedef ReferenceCountedPointer<Scene> Ref;

    static Scene::Ref create(const std::string& sceneName);
    
    virtual void onPose(Array<Surface::Ref>& surfaceArray);

    virtual void onSimulation(GameTime deltaTime);

    Lighting::Ref lighting() const {
        return m_lighting;
    }

    GameTime time() const {
        return m_time;
    }

    Texture::Ref skyBox() const {
        return m_skyBox;
    }

    /** Enumerate the names of all available scenes. */
    static Array<std::string> sceneNames();
};

#endif
