#include "quickfix/config.h"
#include "quickfix/Application.h"
#include "quickfix/SocketInitiator.h"
#include "quickfix/SessionSettings.h"
#include "quickfix/FileStore.h"
#include "quickfix/FileLog.h"
#include "quickfix/SocketAcceptor.h"
#include "quickfix/MessageCracker.h"
#include "quickfix/Values.h"
#include "quickfix/fix44/ExecutionReport.h"
#include <iostream>
#include <csignal>

class InitiatorApp : public FIX::Application, public FIX::MessageCracker {
private:
    FIX::Message message;  // Menambahkan variabel message di kelas InitiatorApp

public:
    void onCreate(const FIX::SessionID &) override {}
    void onLogon(const FIX::SessionID &) override {}
    void onLogout(const FIX::SessionID &) override {}
    void toAdmin(FIX::Message &, const FIX::SessionID &) override {}
    void fromAdmin(const FIX::Message &, const FIX::SessionID &) override {}
    void toApp(FIX::Message &, const FIX::SessionID &) override {}

    void fromApp(const FIX::Message &message, const FIX::SessionID &sessionID) override {
        crack(message, sessionID);
    }

    // Metode dari kelas MessageCracker
    void onMessage(const FIX44::ExecutionReport &executionReport, const FIX::SessionID &sessionID) override {
        // Logika penanganan Execution Report

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

    void wait() {
        std::cout << "Type Ctrl-C to quit" << std::endl;
        while (true) {
            // Menampilkan field dari pesan FIX
            displayMessageFields();
            FIX::process_sleep(0.1);
        }
    }

    void displayMessageFields() const {
        std::cout << "Received Message Fields:" << std::endl;

        // Mendapatkan semua field dalam bentuk FieldMap
        const FIX::FieldMap &fieldMap = message.getHeader();
        const FIX::FieldMap &bodyFieldMap = message;

        // Menampilkan header dan body field
        displayFields("Header", fieldMap);
        displayFields("Body", bodyFieldMap);
    }

    void displayFields(const std::string &prefix, const FIX::FieldMap &fieldMap) const {
        std::cout << prefix << " Fields:" << std::endl;

        for (FIX::FieldMap::iterator it = fieldMap.begin(); it != fieldMap.end(); ++it) {
            const FIX::FieldBase &field = it->second;
            std::cout << "  " << field.getTag() << ": " << field.getString() << std::endl;
        }

        std::cout << std::endl;
    }
};

volatile std::sig_atomic_t signaled = 0;

void signalHandler(int signal) {
    signaled = 1;
}

int main() {
    std::signal(SIGINT, signalHandler);

    try {
        FIX::SessionSettings settings("initiator.cfg");
        InitiatorApp application;
        FIX::FileStoreFactory storeFactory(settings);
        FIX::ScreenLogFactory logFactory(settings);
        FIX::SocketInitiator initiator(application, storeFactory, settings, logFactory);

        while (!signaled) {
            // Start initiator and wait
            initiator.start();
            application.wait();
            initiator.stop();
        }

        return 0;
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
}
