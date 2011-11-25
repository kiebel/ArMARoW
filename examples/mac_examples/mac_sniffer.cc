
#include <armarow/mac/ieee_802.15.4.h>

/* === globals ============================================================== */
/*! \brief Configuration class for the Mac Layer. We only have to set parameters that should have different values than the default values. */
struct My_MAC_Config : public armarow::MAC::MAC_Configuration{
    enum {
        channel=3,
        mac_adress_of_node=20 //you have to set at least this
    };
};

typedef armarow::MAC::MAC_CSMA_CA<My_MAC_Config,platform::config::rc_t,armarow::MAC::Enable> Mac_Layer;
Mac_Layer mac;
armarow::MAC::mob_t messageobject;

/* === functions ============================================================ */
/*! First your have to bind this function to the mac.onMessageReceiveDelegate.bind<callback_recv>(). The MAC Layer will now call this function if a MAC Message is received. Second if you have intend to do any logging output then you have to secure the function with a global lock object, to make shure that your output will not get messed up. Third you have to get the message with the receive method of the Mac Layer. Since receive returns the number of received data bytes (payload only), you should make sure that the message really contains data.
 \brief  Callback triggered by a message received event in the Mac Layer
 */
void callback_recv() {

    avr_halib::locking::GlobalIntLock lock; //secure with lock

    if(mac.receive(messageobject)!=0){
        ::logging::log::emit()
        //some exaples how you could output information, the type casts are neccessary because the logging framework would output uint8_t as character data
        //<< PROGMEMSTRING("Node ID: ") << (int) messageobject.header.source_adress
        //<< PROGMEMSTRING("Message Sequence Number: ") << (int) messageobject.header.sequencenumber
            << (int) messageobject.header.source_adress << "," <<(int) messageobject.header.sequencenumber
            << ::logging::log::endl; // << ::logging::log::endl;
    }else{
        ::logging::log::emit()
            << PROGMEMSTRING("Failed receiving message!")
            << ::logging::log::endl << ::logging::log::endl;
    }

}
/* === main ================================================================= */
int main() {
    sei();                              // enable interrupts
    ::logging::log::emit()
        << PROGMEMSTRING("Starting sniffer!")
        << ::logging::log::endl << ::logging::log::endl;
    mac.onMessageReceiveDelegate.bind<callback_recv>();
    while(1);
}

