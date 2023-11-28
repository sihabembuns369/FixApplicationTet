#include <quickfix/Application.h>
#include <quickfix/FileStore.h>
#include <quickfix/FileLog.h>
#include <quickfix/SocketAcceptor.h>
#include <quickfix/SessionSettings.h>
#include <quickfix/MessageCracker.h>
#include <quickfix/fix42/NewOrderSingle.h>
#include <quickfix/Session.h>
#include <iostream>

class MyFIXApplication : public FIX::Application, public FIX::MessageCracker {
public:
    void onCreate(const FIX::SessionID& sessionID) override {
        std::cout << "Session created - " << sessionID << std::endl;
    }

    void onLogon(const FIX::SessionID& sessionID) override {
        std::cout << "Logon - " << sessionID << std::endl;
    }

    void onLogout(const FIX::SessionID& sessionID) override {
        std::cout << "Logout - " << sessionID << std::endl;
    }

    void toAdmin(FIX::Message& message, const FIX::SessionID& sessionID) override {
        std::cout << "ToAdmin - " << sessionID << std::endl;
    }

    void toApp(FIX::Message& message, const FIX::SessionID& sessionID) override {
        std::cout << "ToApp - " << sessionID << std::endl;
    }

    void fromAdmin(const FIX::Message& message, const FIX::SessionID& sessionID) override {
        std::cout << "FromAdmin - " << sessionID << std::endl;
    }

    void fromApp(const FIX::Message& message, const FIX::SessionID& sessionID) override {
        crack(message, sessionID);
    }

    void onMessage(const FIX42::NewOrderSingle& message, const FIX::SessionID& sessionID) override {
        std::cout << "Received NewOrderSingle - " << sessionID << std::endl;
        
    }
   void sendNewOrderSingle(const FIX::SessionID& sessionID) {
    FIX42::NewOrderSingle newOrderSingle;
    newOrderSingle.setField(FIX::ClOrdID("12345"));
    newOrderSingle.setField(FIX::Symbol("AAPL"));
    newOrderSingle.setField(FIX::Side(FIX::Side_BUY));
    newOrderSingle.setField(FIX::TransactTime());
    newOrderSingle.setField(FIX::OrdType(FIX::OrdType_LIMIT));

    FIX::Session::sendToTarget(newOrderSingle, sessionID);
    std::cout << "Sent NewOrderSingle - " << sessionID << std::endl;
}

};
void wait()
{
  std::cout << "Type Ctrl-C to quit" << std::endl;
  while(true)
  {
    FIX::process_sleep(1);
  }
}
int main(int argc, char** argv) {
    if ( argc < 2 )
  {
    std::cout << "usage: " << argv[ 0 ]
    << " MAsukan file config." << std::endl;
    return 0;
  }
  std::string file = argv[ 1 ];
    try {
        FIX::SessionSettings settings(file);
        MyFIXApplication myApp;
        FIX::FileStoreFactory storeFactory(settings);
        FIX::FileLogFactory logFactory(settings);
        FIX::SocketAcceptor acceptor(myApp, storeFactory, settings, logFactory);
        acceptor.start();
         myApp.sendNewOrderSingle(FIX::SessionID("FIX.4.4", "SENDER", "TARGET"));
         FIX::SessionID sessionID("FIX.4.4", "YOUR_SENDER_COMP_ID", "YOUR_TARGET_COMP_ID");
        FIX::Session* session = FIX::Session::lookupSession(sessionID);
if (session != nullptr) {
    // Lakukan operasi pada sesi
} else {
    std::cerr << "Error: Session Not Found" << std::endl;
}

            wait();
         acceptor.block();

         return 0;
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
