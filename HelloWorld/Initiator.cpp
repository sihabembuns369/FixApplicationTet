#ifdef _MSC_VER
#pragma warning(disable : 4503 4355 4786)
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
#include "quickfix/Values.h"
#include "quickfix/fix44/ExecutionReport.h"
#include "../../../../usr/include/c++/11/bits/stl_pair.h"
class InitiatorApp : public FIX::Application, public FIX::MessageCracker
{
private:
  FIX::Message message; // Menambahkan variabel message di kelas InitiatorApp

public:
  void onCreate(const FIX::SessionID &) override {}
  void onLogon(const FIX::SessionID &) override {}
  void onLogout(const FIX::SessionID &) override {}
  void toAdmin(FIX::Message &, const FIX::SessionID &) override {}
  void fromAdmin(const FIX::Message &, const FIX::SessionID &) override {}
  void toApp(FIX::Message &message, const FIX::SessionID &sessionID) override
  {
    try
    {
      FIX::PossDupFlag possDupFlag;
      message.getHeader().getField(possDupFlag);
      if (possDupFlag)
        throw FIX::DoNotSend();
    }
    catch (FIX::FieldNotFound &)
    {
    }

    std::cout << std::endl
              << "OUT: " << message << std::endl;
  }
  void fromApp(const FIX::Message &message, const FIX::SessionID &sessionID) override
  {
    crack(message, sessionID);
  }

  // Metode dari kelas MessageCracker
  void onMessage(const FIX44::ExecutionReport &executionReport, const FIX::SessionID &sessionID) override
  {
    // Logika penanganan Execution Report
    FIX::SenderCompID senderCompID;
    FIX::TargetCompID targetCompID;
    ;
    std::cout << "Received Heartbeat from " << senderCompID << " to " << targetCompID << std::endl;
    // Mendapatkan nilai field dari pesan Execution Report
    FIX::Symbol symbol;
    FIX::ExecType execType;
    FIX::OrdStatus ordStatus;
    FIX::ClOrdID clOrdID;
    FIX::OrderID orderID;

    executionReport.get(symbol);
    executionReport.get(execType);
    executionReport.get(ordStatus);
    executionReport.get(clOrdID);
    executionReport.get(orderID);

    // Menampilkan informasi
    std::cout << "Received Execution Report:" << std::endl;
    std::cout << "Symbol: " << symbol << std::endl;
    std::cout << "ExecType: " << execType << std::endl;
    std::cout << "OrdStatus: " << ordStatus << std::endl;
    std::cout << "ClOrdID: " << clOrdID << std::endl;
    std::cout << "OrderID: " << orderID << std::endl;
  }

  void wait()
  {
    std::cout << "Type Ctrl-C to quit" << std::endl;
    while (true)
    {
      // Menampilkan field dari pesan FIX
      // displayMessageFields();

      FIX::process_sleep(0.1);
    }
  }
};

int main()
{
#ifdef HAVE_SSL
  std::string isSSL;
  if (argc > 2)
  {
    isSSL.assign(argv[2]);
  }
#endif
  try
  {
    FIX::SessionSettings settings("initiator.cfg");
    InitiatorApp application;
    FIX::FileStoreFactory storeFactory(settings);
    FIX::ScreenLogFactory logFactory(settings);
    FIX::SocketInitiator initiator(application, storeFactory, settings, logFactory);
#ifdef HAVE_SSL
    if (isSSL.compare("SSL") == 0)
      acceptor = std::unique_ptr<FIX::Acceptor>(
          new FIX::ThreadedSSLSocketAcceptor(application, storeFactory, settings, logFactory));
    else if (isSSL.compare("SSL-ST") == 0)
      acceptor = std::unique_ptr<FIX::Acceptor>(
          new FIX::SSLSocketAcceptor(application, storeFactory, settings, logFactory));
    else
#endif

      initiator.start();
    application.wait();
    initiator.block();
    initiator.stop();
    return 0;
  }
  catch (std::exception &e)
  {
  
    std::cout << "error"  << e.what() << std::endl;
    return 1;
  }
}
