#ifndef VRFISHEYENODE_H_
#define VRFISHEYENODE_H_

namespace MinVR {
	
	class VRFisheyeNode : public VRWindowChildNode
	{
	public:
		
		virtual ~VRFisheyeNode();



		virtual void render();
		virtual void join();
		virtual void synchronize();
	};
}

#endif