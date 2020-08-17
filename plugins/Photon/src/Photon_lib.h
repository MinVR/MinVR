#define DBGPRINTF_LEVEL 6

#include "LoadBalancing-cpp/inc/Client.h"
#include "UIListener.h"
#include <vector>

class PhotonLib : private ExitGames::LoadBalancing::Listener
{
public:
	PhotonLib(UIListener* uiListener);
	~PhotonLib();

	void update(void);
	void sendData(std::string data);
	bool isConnected();
	std::vector <std::string> &getPendingEvents() {
		return m_pendingEvents;
	}

	ExitGames::Common::JString getStateString(void);

	static ExitGames::Common::JString appID;
	static ExitGames::Common::JString appVersion;
	static ExitGames::Common::JString gameName;
	static ExitGames::Common::JString PLAYER_NAME;

private:
	// receive and print out debug out here
	virtual void debugReturn(int debugLevel, const ExitGames::Common::JString& string);

	// implement your error-handling here
	virtual void connectionErrorReturn(int errorCode);
	virtual void clientErrorReturn(int errorCode);
	virtual void warningReturn(int warningCode);
	virtual void serverErrorReturn(int errorCode);

	// events, triggered by certain operations of all players in the same room
	virtual void joinRoomEventAction(int playerNr, const ExitGames::Common::JVector<int>& playernrs, const ExitGames::LoadBalancing::Player& player);
	virtual void leaveRoomEventAction(int playerNr, bool isInactive);
	virtual void customEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object& eventContent);

	// callbacks for operations on PhotonLoadBalancing server
	virtual void connectReturn(int errorCode, const ExitGames::Common::JString& errorString, const ExitGames::Common::JString& region, const ExitGames::Common::JString& cluster);
	virtual void disconnectReturn(void);
	virtual void createRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& gameProperties, const ExitGames::Common::Hashtable& playerProperties, int errorCode, const ExitGames::Common::JString& errorString);
	virtual void joinOrCreateRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& gameProperties, const ExitGames::Common::Hashtable& playerProperties, int errorCode, const ExitGames::Common::JString& errorString);
	virtual void joinRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& gameProperties, const ExitGames::Common::Hashtable& playerProperties, int errorCode, const ExitGames::Common::JString& errorString);
	virtual void joinRandomRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& gameProperties, const ExitGames::Common::Hashtable& playerProperties, int errorCode, const ExitGames::Common::JString& errorString);
	virtual void leaveRoomReturn(int errorCode, const ExitGames::Common::JString& errorString);
	virtual void joinLobbyReturn(void);
	virtual void leaveLobbyReturn(void);

	class State
	{
	public:
		enum States
		{
			INITIALIZED = 0,
			CONNECTING,
			CONNECTED,
			JOINING,
			JOINED,
			SENT_DATA,
			RECEIVED_DATA,
			LEAVING,
			LEFT,
			DISCONNECTING,
			DISCONNECTED
		};
	};
	State::States mState;
	
	ExitGames::LoadBalancing::Client mLoadBalancingClient;
	UIListener* mpOutputListener;
	ExitGames::Common::Logger mLogger;

	int64 mSendCount;
	int64 mReceiveCount;
	std::vector <std::string> m_pendingEvents;
	
};