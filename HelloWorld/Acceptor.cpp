#include "quickfix/Application.h"
#include "quickfix/SocketAcceptor.h"
#include "quickfix/SessionSettings.h"
#include "quickfix/FileStore.h"
#include "quickfix/FileLog.h"
#include "quickfix/SocketAcceptor.h"
#include "quickfix/MessageCracker.h"
#include "quickfix/fix50/ExecutionReport.h"
#include "quickfix/config.h"
#include "quickfix/Session.h"
#include <iostream>
#include <unordered_map>
class AcceptorApp : public FIX::Application, public FIX::MessageCracker {

public:
    void onCreate(const FIX::SessionID&) override {}
    void onLogon(const FIX::SessionID& ) override {
    
    }
    void onLogout(const FIX::SessionID& sessionID) override {}
    void toAdmin(FIX::Message&, const FIX::SessionID&) override {}
    void fromAdmin(const FIX::Message&, const FIX::SessionID&) override {}
    void toApp(FIX::Message&, const FIX::SessionID&) override {}
    void fromApp(const FIX::Message& message, const FIX::SessionID& sessionID) override {
        crack(message, sessionID);
         }

   
    void onMessage(const FIX42::Heartbeat& message, const FIX::SessionID& sessionID) override {
        // Handle incoming Heartbeat message
        FIX::SenderCompID senderCompID;
        FIX::TargetCompID targetCompID;;
        std::cout << "Received Heartbeat from " << senderCompID << " to " << targetCompID << std::endl;
    }

};

void wait()
{
  std::cout << "Type Ctrl-C to quit" << std::endl;
  while(true)
  {
    FIX::process_sleep(0.1);
  }
}
int main() {
    FIX::SessionSettings settings("acceptor.cfg");
    AcceptorApp application;
    FIX::FileStoreFactory storeFactory(settings);
    FIX::ScreenLogFactory logFactory(settings);
    FIX::SocketAcceptor acceptor(application, storeFactory, settings, logFactory);
    acceptor.start();
    wait();
    acceptor.stop();
    return 0;
}
