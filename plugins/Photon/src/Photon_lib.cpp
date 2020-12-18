#include "Photon_lib.h"
#include "limits.h"

ExitGames::Common::JString PhotonLib::appID = L"";
ExitGames::Common::JString PhotonLib::appVersion = L"";
ExitGames::Common::JString PhotonLib::gameName = L"";
ExitGames::Common::JString PhotonLib::PLAYER_NAME = L"";

static const int MAX_SENDCOUNT = 100;

PhotonLib::PhotonLib(UIListener* uiListener)
#ifdef _EG_MS_COMPILER
#	pragma warning(push)
#	pragma warning(disable:4355)
#endif
	: mState(State::INITIALIZED)
	, mLoadBalancingClient(*this, PhotonLib::appID, appVersion, ExitGames::Photon::ConnectionProtocol::UDP, true)
	, mpOutputListener(uiListener)
	, mSendCount(0)
	, mReceiveCount(0)
#ifdef _EG_MS_COMPILER
#	pragma warning(pop)
#endif
{
	userID = GETTIMEMS();
	mLoadBalancingClient.setDebugOutputLevel(DEBUG_RELEASE(ExitGames::Common::DebugLevel::INFO, ExitGames::Common::DebugLevel::WARNINGS)); // that instance of LoadBalancingClient and its implementation details
	mLogger.setListener(*this);
	mLogger.setDebugOutputLevel(DEBUG_RELEASE(ExitGames::Common::DebugLevel::INFO, ExitGames::Common::DebugLevel::WARNINGS)); // this class
	ExitGames::Common::Base::setListener(this);
	ExitGames::Common::Base::setDebugOutputLevel(DEBUG_RELEASE(ExitGames::Common::DebugLevel::INFO, ExitGames::Common::DebugLevel::WARNINGS)); // all classes that inherit from Base
}

PhotonLib::~PhotonLib() {
	mLoadBalancingClient.opLeaveRoom();
	mState = State::LEAVING;
	while (mState != State::DISCONNECTED) {
		update();
		std::cerr << "Disconnected" << std::endl;
	}
}

void PhotonLib::update(void)
{
	switch(mState)
	{
		case State::INITIALIZED:
			mLoadBalancingClient.connect(ExitGames::LoadBalancing::AuthenticationValues().setUserID(ExitGames::Common::JString()+userID), PLAYER_NAME);
			mState = State::CONNECTING;
			break;
		case State::CONNECTED:
			mLoadBalancingClient.opJoinOrCreateRoom(gameName);
			mState = State::JOINING;
			break;
		case State::LEFT:
			mLoadBalancingClient.disconnect();
			mState = State::DISCONNECTING;
			break;
		case State::DISCONNECTED:
			break;
		default:
			break;
	}
	mLoadBalancingClient.service();
}

ExitGames::Common::JString PhotonLib::getStateString(void)
{
	switch(mState)
	{
		case State::INITIALIZED:
			return L"disconnected";
		case State::CONNECTING:
			return L"connecting";
		case State::CONNECTED:
			return L"connected";
		case State::JOINING:
			return L"joining";
		case State::JOINED:
			return ExitGames::Common::JString(L"ingame\nsent event Nr. ") + mSendCount + L"\nreceived event Nr. " + mReceiveCount;
		case State::SENT_DATA:
			return ExitGames::Common::JString(L"sending completed") + L"\nreceived event Nr. " + mReceiveCount;
		case State::RECEIVED_DATA:
			return L"receiving completed";
		case State::LEAVING:
			return L"leaving";
		case State::LEFT:
			return L"left";
		case State::DISCONNECTING:
			return L"disconnecting";
		case State::DISCONNECTED:
			return L"disconnected";
		default:
			return L"unknown state";
	}
}
#include <string>
#include <iostream>
void PhotonLib::sendData(std::string data)
{
	if (!isConnected())
		return;

	ExitGames::Common::Hashtable event;
	ExitGames::Common::JVector<unsigned char> vec;
	unsigned char * str = (unsigned char *) data.c_str();
	vec.addElements(str, data.length());
	event.put(static_cast<nByte>(0), vec.getCArray(),vec.getSize());

	//send to ourselves only
	//int myPlayerNumber = mLoadBalancingClient.getLocalPlayer().getNumber();
	//mLoadBalancingClient.opRaiseEvent(true, event, 0, ExitGames::LoadBalancing::RaiseEventOptions().setTargetPlayers(&myPlayerNumber, 1));
	mLoadBalancingClient.opRaiseEvent(true, event, 0); // ExitGames::LoadBalancing::RaiseEventOptions().setTargetPlayers(&myPlayerNumber, 1));
}

bool PhotonLib::isConnected() {
	return mState == State::JOINED;
}

void PhotonLib::debugReturn(int /*debugLevel*/, const ExitGames::Common::JString& string)
{
	mpOutputListener->writeString(string);
}

void PhotonLib::connectionErrorReturn(int errorCode)
{
	EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"code: %d", errorCode);
	mpOutputListener->writeString(ExitGames::Common::JString(L"received connection error ") + errorCode);
	mState = State::DISCONNECTED;
}

void PhotonLib::clientErrorReturn(int errorCode)
{
	EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"code: %d", errorCode);
	mpOutputListener->writeString(ExitGames::Common::JString(L"received error ") + errorCode + L" from client");
}

void PhotonLib::warningReturn(int warningCode)
{
	EGLOG(ExitGames::Common::DebugLevel::WARNINGS, L"code: %d", warningCode);
	mpOutputListener->writeString(ExitGames::Common::JString(L"received warning ") + warningCode + L" from client");
}

void PhotonLib::serverErrorReturn(int errorCode)
{
	EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"code: %d", errorCode);
	mpOutputListener->writeString(ExitGames::Common::JString(L"received error ") + errorCode + " from server");


}

void PhotonLib::joinRoomEventAction(int playerNr, const ExitGames::Common::JVector<int>& /*playernrs*/, const ExitGames::LoadBalancing::Player& player)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"%ls joined the game", player.getName().cstr());
	mpOutputListener->writeString(L"");
	mpOutputListener->writeString(ExitGames::Common::JString(L"player ") + playerNr + L" " + player.getName() + L" has joined the game");

	std::string playername = player.getName().UTF8Representation();
	players.insert(std::make_pair(playerNr, playername));
}

void PhotonLib::leaveRoomEventAction(int playerNr, bool isInactive)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	mpOutputListener->writeString(L"");
	mpOutputListener->writeString(ExitGames::Common::JString(L"player ") + playerNr + L" has left the game");
	
	players.erase(playerNr);
}

//receive Handler
void PhotonLib::customEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object& eventContentObj)
{
	ExitGames::Common::Hashtable eventContent = ExitGames::Common::ValueObject<ExitGames::Common::Hashtable>(eventContentObj).getDataCopy();
	switch(eventCode)
	{
	case 0:
		if (eventContent.getValue((nByte)0)) {
			unsigned char * ary = (ExitGames::Common::ValueObject<unsigned char *> (eventContent.getValue((nByte)0))).getDataCopy();
			short length = (ExitGames::Common::ValueObject<unsigned char *>(eventContent.getValue((nByte)0))).getSizes()[0];
			char * c_arr = (char*) ary;
			std::string str(c_arr, length);
			m_pendingEvents.push_back(str);
		}
	default:
		break;
	}

}

void PhotonLib::connectReturn(int errorCode, const ExitGames::Common::JString& errorString, const ExitGames::Common::JString& region, const ExitGames::Common::JString& cluster)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"connected to cluster " + cluster + L" of region " + region);
	if(errorCode)
	{
		EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
		mState = State::DISCONNECTING;
		return;
	}
	mpOutputListener->writeString(L"connected to cluster " + cluster);
	mState = State::CONNECTED;
}

void PhotonLib::disconnectReturn(void)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	mpOutputListener->writeString(L"disconnected");
	mState = State::DISCONNECTED;
}

void PhotonLib::createRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& /*gameProperties*/, const ExitGames::Common::Hashtable& /*playerProperties*/, int errorCode, const ExitGames::Common::JString& errorString)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	if(errorCode)
	{
		EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
		mpOutputListener->writeString(L"opCreateRoom() failed: " + errorString);
		mState = State::CONNECTED;
		return;
	}

	EGLOG(ExitGames::Common::DebugLevel::INFO, L"localPlayerNr: %d", localPlayerNr);
	mpOutputListener->writeString(L"... room " + mLoadBalancingClient.getCurrentlyJoinedRoom().getName() + " has been created");
	mState = State::JOINED;
}

void PhotonLib::joinOrCreateRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& /*gameProperties*/, const ExitGames::Common::Hashtable& /*playerProperties*/, int errorCode, const ExitGames::Common::JString& errorString)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	if(errorCode)
	{
		EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
		mpOutputListener->writeString(L"opJoinOrCreateRoom() failed: " + errorString);
		mState = State::CONNECTED;
		return;
	}

	EGLOG(ExitGames::Common::DebugLevel::INFO, L"localPlayerNr: %d", localPlayerNr);
	mpOutputListener->writeString(L"... room " + mLoadBalancingClient.getCurrentlyJoinedRoom().getName() + " has been entered");
	mpOutputListener->writeString(L"regularly sending dummy events now");
	mState = State::JOINED;

	ExitGames::LoadBalancing::MutableRoom room = mLoadBalancingClient.getCurrentlyJoinedRoom();
	ExitGames::Common::JVector< ExitGames::LoadBalancing::Player * > players_in_room = room.getPlayers();

	m_playername = room.getPlayerForNumber(localPlayerNr)->getName().UTF8Representation().cstr();
	m_playerNb = localPlayerNr;

	for (unsigned int i = 0; i < players_in_room.getSize(); ++i){
		std::string playername = players_in_room[i]->getName().UTF8Representation().cstr();
		players.insert(std::make_pair(players_in_room[i]->getNumber(), playername));
	}
}

void PhotonLib::joinRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& /*gameProperties*/, const ExitGames::Common::Hashtable& /*playerProperties*/, int errorCode, const ExitGames::Common::JString& errorString)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	if(errorCode)
	{		
		EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
		mpOutputListener->writeString(L"opJoinRoom() failed: " + errorString);
		mState = State::CONNECTED;
		return;
	}
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"localPlayerNr: %d", localPlayerNr);
	mpOutputListener->writeString(L"... room " + mLoadBalancingClient.getCurrentlyJoinedRoom().getName() + " has been successfully joined");
	mpOutputListener->writeString(L"regularly sending dummy events now");

	mState = State::JOINED;
}

void PhotonLib::joinRandomRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& /*gameProperties*/, const ExitGames::Common::Hashtable& /*playerProperties*/, int errorCode, const ExitGames::Common::JString& errorString)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	if(errorCode)
	{
		EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
		mpOutputListener->writeString(L"opJoinRandomRoom() failed: " + errorString);
		mState = State::CONNECTED;
		return;
	}

	EGLOG(ExitGames::Common::DebugLevel::INFO, L"localPlayerNr: %d", localPlayerNr);	
	mpOutputListener->writeString(L"... room " + mLoadBalancingClient.getCurrentlyJoinedRoom().getName() + " has been successfully joined");
	mpOutputListener->writeString(L"regularly sending dummy events now");
	mState = State::JOINED;
}

void PhotonLib::leaveRoomReturn(int errorCode, const ExitGames::Common::JString& errorString)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	if(errorCode)
	{
		EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
		mpOutputListener->writeString(L"opLeaveRoom() failed: " + errorString);
		mState = State::DISCONNECTING;
		return;
	}
	mState = State::LEFT;
	mpOutputListener->writeString(L"room has been successfully left");
}

void PhotonLib::joinLobbyReturn(void)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	mpOutputListener->writeString(L"joined lobby");
}

void PhotonLib::leaveLobbyReturn(void)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	mpOutputListener->writeString(L"left lobby");
}
