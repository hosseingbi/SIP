#include <pjsua2.hpp>
#include <iostream>
#include <memory>
#include <pj/file_access.h>


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
        std::cout << "*** Account is being deleted: Mo of calls="
                   << calls.size() << std::endl;
    }
    virtual void onRegState(OnRegStateParam &prm)
    {
	AccountInfo ai = getInfo();
	std::cout << (ai.regIsActive? "*** Register: code=" : "*** Unregister: code=")
		  << prm.code << std::endl;
    }


};

class MyBuddy : public Buddy
{
public:
    MyBuddy() {}
    ~MyBuddy() {}

    void onBuddyState()
    {
    BuddyInfo bi = getInfo();
    std::cout << "Buddy " << bi.uri << " is " << bi.presStatus.statusText << std::endl;
    }
};




static void Prog(Endpoint &ep) throw(Error)
{
    //Initializing endpoint
    EpConfig ep_cfg;
    ep.libInit( ep_cfg );
    //Creating transpor
    TransportConfig tcfg;
    tcfg.port = 5060;
    ep.transportCreate(PJSIP_TRANSPORT_UDP, tcfg);
    ep.transportCreate(PJSIP_TRANSPORT_TCP, tcfg);
    //Initializing and creating account
    AccountConfig acc_cfg;
    acc_cfg.idUri = "sip:test@192.168.1.64";
    //Registeration
    acc_cfg.regConfig.registrarUri = "sip:192.168.1.64:5060";
    std::auto_ptr<MyAccount> acc(new MyAccount);
    acc->create(acc_cfg);
    //Creating new buddy
    BuddyConfig cfg;
    cfg.uri = "sip:test2@192.168.1.64";
    MyBuddy buddy;
    try {
    buddy.create(*acc, cfg);
    buddy.subscribePresence(true);
    } catch(Error& err) {
    }



    ep.libStart();
    std::cout << "*** PJSUA2 STARTED ***" << std::endl;

    //Sending a message to buddy
    SendInstantMessageParam prm;
    prm.content = "Hello world!";
    buddy.sendInstantMessage(prm);

    std::cout << "Press ENTER to quit..." << std::endl;
    std::cin.get();


}
/*
Buddy.sendInstantMessage();
For IM
*/

int main()
{
    int ret = 0;
    Endpoint ep;

    try {
	ep.libCreate();

	Prog(ep);
	ret = PJ_SUCCESS;
    } catch (Error & err) {
	std::cout << "Exception: " << err.info() << std::endl;
	ret = 1;
    }


    

    try {
	ep.libDestroy();
    } catch(Error &err) {
	std::cout << "Exception: " << err.info() << std::endl;
	ret = 1;
    }

    if (ret == PJ_SUCCESS) {
	std::cout << "Success" << std::endl;
    } else {
	std::cout << "Error Found" << std::endl;
    }

    return 0;

}