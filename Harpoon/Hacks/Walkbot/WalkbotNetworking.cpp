
#pragma once
#define WIN32_LEAN_AND_MEAN


//#include <ws2tcpip.h>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <Windows.h>
#pragma comment(lib, "ws2_32.lib")


#include "WalkbotNetworking.h"
#include "../Walkbot/Walkbot.h"
#include "../OTHER/Debug.h"



/* Connects to Master Server*/



std::shared_mutex WalkbotNetworking::NDMutex;
WalkbotNetworking::DataShare WalkbotNetworking::NetworkData;



namespace WalkbotNetworking {
    UINT_PTR WalkbotNetworkListener::ConnectToServer(char* s_IPAddress, u_short us_Port, bool pass) {
        WORD wVersionRequested = MAKEWORD(2, 2);
        WSADATA wsaData;
        UINT_PTR sock_Server;
        SOCKADDR_IN addr;

        if(WSAStartup(wVersionRequested, &wsaData)){
            char buffer[256];
            snprintf(buffer, 256, "Walkbot::WalkbotNetworking Winsock WSAStartup (ConnectToServer) failed (&d)", WSAGetLastError());
            Debug::QuickPrint(buffer);
            Server = NULL;
            return NULL;
        }
        sock_Server = socket(AF_INET, SOCK_STREAM, 0);
        if (sock_Server == INVALID_SOCKET) {
            char buffer[256];
            snprintf(buffer, 256, "Walkbot::WalkbotNetworking WinSock2 socket() (ConnectToServer) failed (&d)", WSAGetLastError());
            Debug::QuickPrint(buffer);
            Server = NULL;
            return NULL;
        }
        int inetPtonRet = InetPton(AF_INET, s_IPAddress, &addr.sin_addr.s_addr);
        if (inetPtonRet == 0) {
            Debug::QuickPrint("Walkbot::WalkbotNetworking Invalid IP Address String Provided");
            Server = NULL;
            return NULL;
        }
        else if(inetPtonRet == -1) {
            char buffer[256];
            snprintf(buffer, 256, "Walkbot::WalkbotNetworking IP Address Conversion InetPton (ConnectToServer) failed (%d)", WSAGetLastError());
            Debug::QuickPrint(buffer);
            Server = NULL;
            return NULL;
        }
        addr.sin_family = AF_INET;
        addr.sin_port = htons(us_Port);
        if (connect(sock_Server, (SOCKADDR*)&addr, sizeof(addr))) {
            char buffer[256];
            snprintf(buffer, 256, "Walkbot::WalkbotNetworking WinSock2 Connect (ConnectToServer) failed (%d)", WSAGetLastError());
            Debug::QuickPrint(buffer);
            Server = NULL;
            return NULL;
        }

        Debug::QuickPrint({ 150,150,255,255 }, ("Created Connection To Master Walkbot Server at " + std::string(s_IPAddress)).c_str());
        
        Server = (UINT_PTR)sock_Server;
        
        
        Debug::QuickPrint({ 150,150,255,255 }, "Setting Up Class Identification");
        NegotiateClass();

        char Buf[sizeof(WalkbotNetworkingStructs::DataSet)];

        recv(sock_Server, Buf, sizeof(WalkbotNetworkingStructs::DataSet), 0);


        for (int i = 0; i < sizeof(WalkbotNetworkingStructs::DataSet); i++) {
            std::uint32_t val = Buf[i];
            memory->conColorMsg({ 150,150,255,255 }, std::to_string(val).c_str());    
        }

        WalkbotNetworkingStructs::DataSet Dat = *reinterpret_cast<WalkbotNetworkingStructs::DataSet*>(Buf);
        WalkbotNetworkingStructs::WalkbotHeaderData Header = Dat.Header;
        memory->conColorMsg({ 150,150,255,255 }, ("\nClient ID: " + std::to_string(Header.clientID) + "\n").c_str());
        memory->conColorMsg({ 150,150,255,255 }, ("SteamID: " + std::to_string(Header.steamID) + "\n").c_str());
        memory->conColorMsg({ 150,150,255,255 }, ("OperationCode: " + std::to_string(Header.OperationCode) + "\n").c_str());
        memory->conColorMsg({ 150,150,255,255 }, ("Data: " + std::string(reinterpret_cast<const char*>(&Dat.Datas)) + "\n").c_str());


        OurID = Header.ClientType;

        if ((WalkbotNetworkingStructs::Operation)Header.OperationCode == WalkbotNetworkingStructs::Operation::CommandLine) {
            interfaces->engine->clientCmdUnrestricted((const char*)&(Dat.Datas));
        }



        return (UINT_PTR)sock_Server;
    }


    void WalkbotNetworkListener::NegotiateClass() {

        WalkbotNetworkingStructs::ClientIdentification ClientID;
#ifdef WALKBOT_BUILD
        ClientID = WalkbotNetworkingStructs::ClientIdentification::Slave;
#endif
#ifndef WALKBOT_BUILD
        if (config->walkbotcontroller.enabled) {
            ClientID = WalkbotNetworkingStructs::ClientIdentification::Controller;
        }
        else if (config->walkbot.enabled) {
            ClientID = WalkbotNetworkingStructs::ClientIdentification::Master;
        }
        else {
            ClientID = WalkbotNetworkingStructs::ClientIdentification::Standby;
        }
#endif
#ifdef MASTER_CONTROLLER
        ClientID = WalkbotNetworkingStructs::ClientIdentification::Controller;
#endif




        WalkbotNetworkingStructs::DataSet Out;
        Out.Header.OperationCode = WalkbotNetworkingStructs::Operation::ClientIDSetup;
        Out.Header.ClientType = ClientID;
        OurID = ClientID;
        if (SendToServer(Out))
            return;

        

        return;
    }


    bool WalkbotNetworkListener::SendToServer(WalkbotNetworkingStructs::DataSet Out) {
        Out.Header.ClientType = OurID;
        const char* Buf = (const char*)calloc(1, sizeof(WalkbotNetworkingStructs::DataSet));
        memcpy((void*)Buf, &Out, sizeof(WalkbotNetworkingStructs::DataSet));
        if (send(Server, Buf, sizeof(WalkbotNetworkingStructs::DataSet), 0) == SOCKET_ERROR) {
            Debug::QuickPrint({255,0,0,255},std::string("WalkbotNetworkingListener::SendToServer Failed To Send DataSet: " + std::to_string(WSAGetLastError())).c_str());
            return true;
        }
        return false;
    }
    

    bool WalkbotNetworkListener::RecvFromServer(WalkbotNetworkingStructs::DataSet& In) {
        char* Buf = (char*)calloc(1, sizeof(WalkbotNetworkingStructs::DataSet));
        memcpy((void*)Buf, &In, sizeof(WalkbotNetworkingStructs::DataSet));

        if (recv(Server, (char*)Buf, sizeof(WalkbotNetworkingStructs::DataSet), 0) == SOCKET_ERROR) {
            Debug::QuickPrint({ 255,0,0,255 }, std::string("WalkbotNetworkingListener::RecvFromServer Failed To Send DataSet: " + std::to_string(WSAGetLastError())).c_str());
            return true;
        }

        In = *reinterpret_cast<WalkbotNetworkingStructs::DataSet*>(Buf);

        return false;
    }



    void WalkbotNetworkListener::UpdateServer() {
              WalkbotNetworkingStructs::DataSet Out;
              Out.Header.OperationCode = WalkbotNetworkingStructs::Operation::GameUpdate;
              WalkbotNetworkingStructs::UpdateData Update;
              Update.isInGame = interfaces->engine->isInGame();
              
              if(Update.isInGame && localPlayer.get() && localPlayer->isAlive()){
                  Update.MapPosition.x = localPlayer->origin().x;
                  Update.MapPosition.y = localPlayer->origin().x;
                  Update.MapPosition.z = localPlayer->origin().x;
                  Update.Armor = localPlayer->armor();
                  Update.Health = localPlayer->health();
                  Update.NavID = 1;
               }
              memcpy(&Out.Datas.data, &Update, sizeof(WalkbotNetworkingStructs::UpdateData));
              SendToServer(Out);
    }



    WalkbotNetworkListener::WalkbotNetworkListener(const char* s_IPAddress, u_short us_Port, void* inAccessor) {
        WalkbotNetworkContainer* Container = reinterpret_cast<WalkbotNetworkContainer*>(inAccessor);
        Container->WalkbotNetworkAccess = std::make_unique<WalkNetworkAccesor>(Container);
        strcpy(m_sIPAddress, s_IPAddress);
        m_usPort = us_Port;
        ConnectToServer(const_cast<char*>(s_IPAddress), us_Port, true);
    }


    void WalkbotNetworkListener::MainLoop() {
        int Retries = 0;
        while(Server == NULL) {
            Retries++;
            if (Retries < 50) {
                Sleep(5000);
            }
            else {
                Debug::QuickPrint("Failed Connection Retry >50 Times. Decreasing Frequency of Checks");
                Sleep(20000);
            }
            ConnectToServer(m_sIPAddress, m_usPort);
        }
        while (true) {
            Sleep(1000);
            WalkbotNetworkingStructs::DataSet In;
            if (RecvFromServer(In)) {
                Server = NULL;
                return;
            }


            Debug::QuickPrint(("Data Recieved from Server Op Code Is: " + std::to_string(In.Header.OperationCode)).c_str());
            switch (In.Header.OperationCode) {
                case WalkbotNetworkingStructs::Operation::InGame:
                {
                    Debug::QuickPrint({ 150,150,255,255 }, "WalbotNetworkListener::MainLoop Updating Server On InGame Status");
                    WalkbotNetworkingStructs::DataSet Out;
                    Out.Header.OperationCode = WalkbotNetworkingStructs::Operation::InGame;
                    bool val = interfaces->engine->isInGame();
                    memcpy(&Out.Datas.data, &val, sizeof(bool));
                    if (SendToServer(Out)) {
                        Server = NULL;
                        return;
                    }
                }
                    break;
                case WalkbotNetworkingStructs::Operation::GameUpdate:
                {
                    Debug::QuickPrint({ 150,150,255,255 }, "WalbotNetworkListener::MainLoop Updating Server");
                    UpdateServer();
                }
                break;
                case WalkbotNetworkingStructs::Operation::CommandLine:
                {
                    interfaces->engine->clientCmdUnrestricted((const char*)&(In.Datas));
                }
                    break;
                case WalkbotNetworkingStructs::Operation::StatusChange:
                {
                    Debug::QuickPrint("Recieved Status Change");
                    NDMutex.lock();
                    NetworkData.shouldRead = true;
                    NetworkData.hasPos = false;
                    NetworkData.Status = *reinterpret_cast<int*>(&In.Datas.data);
                    if (NetworkData.Status == Walkbot::WalkbotStatus::ActiveStandby) {
                        NetworkData.Act = Walkbot::WalkbotActs::FollowPath;
                    }
                    NDMutex.unlock();
                    break;
                }
                case WalkbotNetworkingStructs::Operation::GoToPosition:
                {
                    Debug::QuickPrint("Recieved Position Dispatch");
                    WalkbotNetworkingStructs::NetworkVector Position;
                    Position = *reinterpret_cast<WalkbotNetworkingStructs::NetworkVector*>(&In.Datas.data);
                    NDMutex.lock();
                    Debug::QuickPrint("Locked NDMutex, Setting Network Data Values");
                    NetworkData.Pos.x = Position.x;
                    NetworkData.Pos.y = Position.y;
                    NetworkData.Pos.z = Position.z;
                    NetworkData.Status = Walkbot::WalkbotStatus::ToPosition;
                    NetworkData.Act = Walkbot::WalkbotActs::HeadToPoint;
                    NetworkData.hasPos = true;
                    NetworkData.shouldRead = true;
                    NDMutex.unlock();
                }
                    break;
                default:
                    break;
            }


        
        
        };     
    }


    void WalkbotNetworking::WalkNetworkAccesor::TurnVoiceRecordOn() {
        SendCommandToServer(WalkbotNetworkingStructs::ControllerOperation::VoiceRecordOn);
    }
    void WalkbotNetworking::WalkNetworkAccesor::TurnVoiceRecordOff() {
        SendCommandToServer(WalkbotNetworkingStructs::ControllerOperation::VoiceRecordOff);
    }

    void WalkbotNetworking::WalkNetworkAccesor::JoinTeam(int Team){
        WalkbotNetworkingStructs::ControllerCommand NewCommand;
        if (Team == 2) {
            NewCommand.OP = WalkbotNetworkingStructs::ControllerOperation::JoinTOP;
        }
        else {
            NewCommand.OP = WalkbotNetworkingStructs::ControllerOperation::JoinCTOP;
        }
        WalkbotNetworkingStructs::DataSet OurSet;
        OurSet.Header.OperationCode = WalkbotNetworkingStructs::Operation::ControllerCommandRequest;
        memcpy(&OurSet.Datas.data, &NewCommand, sizeof(WalkbotNetworkingStructs::ControllerCommand));
        Debug::QuickPrint("Sending Join Team Signal");
        Container->WalkbotNetwork->SendToServer(OurSet);
    }


    void WalkbotNetworking::WalkNetworkAccesor::SendToPosition(Vector Pos) {
        WalkbotNetworkingStructs::ControllerCommand NewCommand;
        NewCommand.Vec.x = Pos.x;
        NewCommand.Vec.y = Pos.y;
        NewCommand.Vec.z = Pos.z;
        NewCommand.OP = WalkbotNetworkingStructs::DispatchToPoint;
        WalkbotNetworkingStructs::DataSet OurSet;
        OurSet.Header.OperationCode = WalkbotNetworkingStructs::Operation::ControllerCommandRequest;
        memcpy(&OurSet.Datas.data, &NewCommand, sizeof(WalkbotNetworkingStructs::ControllerCommand));
        Debug::QuickPrint("Position To Bots");
        Container->WalkbotNetwork->SendToServer(OurSet);
    }


    void WalkbotNetworking::WalkNetworkAccesor::SendStatus(int Status) {
        WalkbotNetworkingStructs::ControllerCommand NewCommand;
        NewCommand.OP = WalkbotNetworkingStructs::ChangeStatus;
        NewCommand.EnumVar = Status;
        WalkbotNetworkingStructs::DataSet OurSet;
        OurSet.Header.OperationCode = WalkbotNetworkingStructs::Operation::ControllerCommandRequest;
        memcpy(&OurSet.Datas.data, &NewCommand, sizeof(WalkbotNetworkingStructs::ControllerCommand));
        Debug::QuickPrint("Position To Bots");
        Container->WalkbotNetwork->SendToServer(OurSet);
    }




    void WalkbotNetworking::WalkNetworkAccesor::SendCommandToServer(WalkbotNetworkingStructs::ControllerOperation Op) {
        WalkbotNetworkingStructs::ControllerCommand NewCommand;
        NewCommand.OP = Op;
        WalkbotNetworkingStructs::DataSet OurSet;
        OurSet.Header.OperationCode = WalkbotNetworkingStructs::Operation::ControllerCommandRequest;
        memcpy(&OurSet.Datas.data, &NewCommand, sizeof(WalkbotNetworkingStructs::ControllerCommand));
        Debug::QuickPrint("Sending Join Team Signal");
        Container->WalkbotNetwork->SendToServer(OurSet);
    }

    void WalkbotNetworking::WalkNetworkAccesor::SetRead() {
        NDMutex.lock();
        NetworkData.shouldRead = false;
        NetworkData.hasPos = false;
        NDMutex.lock();
                
    }



}