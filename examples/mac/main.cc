

#include "mac.h" 

 
 //armarow::mac::TMAC<11> mac;
 //armarow::mac::ALOHA<12> mac;

  armarow::MAC::MAC_Base mac;


 //platform::config::mac_t mac;







 int main(int argc,char* argv[]){

	::logging::log::emit()
        << PROGMEMSTRING("Starting MAC Test APP...")
	<< ::logging::log::endl << ::logging::log::endl;

	 //armarow::MAC::DeviceAddress node_mac_adress=0;

	 // mac_t mac;

	 mac.init();

	 char buffer[10]="Hi!";

	 //mac.send(node_mac_adress,buffer,sizeof(buffer));
	while(1){
	delay(3000);
	 mac.send(buffer,sizeof(buffer));

	 //mac.receive(buffer,sizeof(buffer));

	::logging::log::emit()
        << PROGMEMSTRING("Sending DATA: '")
	<< buffer	<< PROGMEMSTRING("'")
	<< ::logging::log::endl << ::logging::log::endl;

	}



	 return 0;
 }





