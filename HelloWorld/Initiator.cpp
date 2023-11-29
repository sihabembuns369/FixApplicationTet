#ifdef _MSC_VER
#pragma warning( disable : 4503 4355 4786 )
#endif

#include "quickfix/config.h"
#include "quickfix/Application.h"
#include "quickfix/SocketInitiator.h"
#include "quickfix/SessionSettings.h"
#include "quickfix/FileStore.h"
#include "quickfix/FileLog.h"
#include "quickfix/SocketAcceptor.h"
#ifdef HAVE_SSL
#include "quickfix/ThreadedSSLSocketAcceptor.h"
#include "quickfix/SSLSocketAcceptor.h"
#endif
#include <string>
#include <iostream>
#include <fstream>
#include <memory>
#include "quickfix/MessageCracker.h"
class InitiatorApp : public FIX::Application,public FIX::MessageCracker{
public:
    void onCreate(const FIX::SessionID&) override {}
    void onLogon(const FIX::SessionID&) override {}
    void onLogout(const FIX::SessionID&) override {}
    void toAdmin(FIX::Message&, const FIX::SessionID&) override {}
    void fromAdmin(const FIX::Message&, const FIX::SessionID&) override {}
    void toApp(FIX::Message&, const FIX::SessionID&) override {}
        void fromApp(const FIX::Message& message, const FIX::SessionID& sessionID) override {
        crack(message, sessionID);
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
    #ifdef HAVE_SSL
  std::string isSSL;
  if (argc > 2)
  {
    isSSL.assign(argv[2]);
  }
#endif
try{
    FIX::SessionSettings settings("initiator.cfg");
    InitiatorApp application;
    FIX::FileStoreFactory storeFactory(settings);
    FIX::ScreenLogFactory logFactory(settings);
    FIX::SocketInitiator initiator(application, storeFactory, settings, logFactory);
    #ifdef HAVE_SSL
    if (isSSL.compare("SSL") == 0)
      acceptor = std::unique_ptr<FIX::Acceptor>(
        new FIX::ThreadedSSLSocketAcceptor ( application, storeFactory, settings, logFactory ));
    else if (isSSL.compare("SSL-ST") == 0)
      acceptor = std::unique_ptr<FIX::Acceptor>(
        new FIX::SSLSocketAcceptor ( application, storeFactory, settings, logFactory ));
    else
#endif
    
    initiator.start();
    wait();
    initiator.stop();
    return 0;
}catch ( std::exception & e )
  {
    std::cout << e.what() << std::endl;
    return 1;
  }
}
