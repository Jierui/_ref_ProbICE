#include <iostream>
#include "talk/p2p/client/httpportallocator.h"
#include "talk/base/network.h"
#include "prober.h"
#include "ppsession.h"
#include "peer.h"

IceProber::IceProber() {
    worker_thread_ = new talk_base::Thread();
    worker_thread_->Start();
    signal_thread_ = new talk_base::Thread();
    signal_thread_->Start();

    session_ = NULL;
    peer_ = NULL;
}

IceProber::~IceProber() {
    if ( peer_ ) {
        delete peer_;
        peer_ = NULL;
    }
    if ( session_ ) {
        delete session_;
        session_ = NULL;
    }
    
    if ( network_manager_ ) {
        delete network_manager_;
        network_manager_ = NULL;
    }





    if ( signal_thread_ ) {
        signal_thread_->Stop();
        delete signal_thread_;
    }        
    
    if ( worker_thread_ ) {
        worker_thread_->Stop();
        delete worker_thread_;
    } 
}

void IceProber::Login(const std::string &server, 
               const unsigned short port,
               const std::string& myName,
               const std::string& remoteName) {
    my_name_ = myName;
    remote_name_ = remoteName;

    network_manager_ = new talk_base::BasicNetworkManager();
    port_allocator_ = new  cricket::HttpPortAllocator(network_manager_, "iceprober-agent");

    peer_ =  new Peer(server, 1979, my_name_, worker_thread_);
    peer_->SignalOnline.connect(this, &IceProber::onOnLine);
    peer_->SignalOffline.connect(this, &IceProber::onOffline);
    peer_->SignalRemoteOnline.connect(this, &IceProber::onRemoteOnline);
    peer_->SignalRemoteOffline.connect(this, &IceProber::onRemoteOffline);
    peer_->SignalRemoteMessage.connect(this, &IceProber::onRemoteMessage);
    peer_->Start();
    
}

void IceProber::Run() {
    talk_base::Thread* main_thread = talk_base::Thread::Current();
    main_thread->Run();
}

void IceProber::onOnLine(bool isOk) {
    if ( isOk ) {
        std::cout << "Connected to server is OK" << std::endl;
    } else {
        std::cout << "Can't connect to server" << std::endl;
    }
}

void IceProber::onOffline() {
    std::cout << "Disconnect to server" << std::endl;
}

void IceProber::onRemoteOnline(const std::string &remote) {
    std::cout << "Remote (" << remote << ") is online" << std::endl;
}

void IceProber::onRemoteOffline(const std::string &remote){
    std::cout << "Remote (" << remote << ") is offline" << std::endl;
}

void IceProber::onRemoteMessage(const std::string &remote, const std::string &msg) {
    std::cout << "Remote (" << remote << ") say to me: " << msg << std::endl;
}

void IceProber::OnMessage(talk_base::Message *msg) { 
}

