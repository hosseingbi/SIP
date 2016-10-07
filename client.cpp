#include <pjsua2.hpp>
#include <iostream>
#include <memory>
#include <pj/file_access.h>
#include <unistd.h>


using namespace pj;

class MyAccount : public Account
{
public:
    std::vector<Call *> calls;

public:
    MyAccount()
    {}

    ~MyAccount()
    {
        std::cout << "*** Account is being deleted: Mo of calls=" << calls.size() << std::endl;
    }
    virtual void onRegState(OnRegStateParam &prm)
    {
        AccountInfo ai = getInfo();
        std::cout << (ai.regIsActive? "*** Register: code=" : "*** Unregister: code=") << prm.code << std::endl;
    }

    virtual void onInstantMessage(OnInstantMessageParam &prm)
    {
        std::cout<<"message received!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<std::endl;
    }


};

class MyBuddy : public Buddy
{
public:
    MyBuddy() {}
    ~MyBuddy() {}

    virtual void onBuddyState()
    {
        BuddyInfo bi = getInfo();
        std::cout <<"*****************************************************************************"<<std::endl;
        std::cout << "Buddy " << bi.uri << " is " << bi.presStatus.statusText << std::endl;
    }
};




static void Prog(Endpoint &ep) throw(Error)
{
    EpConfig ep_cfg;
    ep.libInit( ep_cfg );

    TransportConfig tcfg;
    tcfg.port = 0;///
    ep.transportCreate(PJSIP_TRANSPORT_UDP, tcfg);
//    ep.transportCreate(PJSIP_TRANSPORT_TCP, tcfg);

    ep.libStart();

    // Configure an AccountConfig
    AccountConfig acfg;
    acfg.idUri = "sip:test2@192.168.1.105";
    acfg.regConfig.registrarUri = "sip:192.168.1.105";
    AuthCredInfo cred("digest", "*", "test2", 0, "secret");
    acfg.sipConfig.authCreds.push_back( cred );

    // Create the account
    std::auto_ptr<MyAccount> acc(new MyAccount);
    acc->create(acfg);

    BuddyConfig cfg;
    cfg.uri = "sip:test1@192.168.1.105";
    MyBuddy buddy;
    try
    {
        buddy.create(*acc, cfg);
        buddy.subscribePresence(true);
    }
    catch(Error& err)
    {
    }




    std::cout << "*** PJSUA2 STARTED ***" << std::endl;
    SendInstantMessageParam prm;
    std::string msg;
    while(true)
    {
        std::cin>>msg;
        prm.content = msg;
        buddy.sendInstantMessage(prm);
    }

    std::cout << "Press ENTER to quit..." << std::endl;
    std::cin.get();


}

int main()
{
    int ret = 0;
    Endpoint ep;

    try
    {
        ep.libCreate();

        Prog(ep);
        ret = PJ_SUCCESS;
    }
    catch (Error & err)
    {
        std::cout << "Exception: " << err.info() << std::endl;
        ret = 1;
    }




    try
    {
        ep.libDestroy();
    }
    catch(Error &err)
    {
        std::cout << "Exception: " << err.info() << std::endl;
        ret = 1;
    }

    if (ret == PJ_SUCCESS)
    {
        std::cout << "Success" << std::endl;
    }
    else
    {
        std::cout << "Error Found" << std::endl;
    }

    return 0;

}
