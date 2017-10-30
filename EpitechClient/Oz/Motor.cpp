#include "Motor.hh"

Motor::Motor(){
	std::cout << "Hello i'm a motor" << std::endl;

    serverReadThread_ = std::thread( &Motor::server_read_thread, this );
    serverWriteThread_ = std::thread( &Motor::server_write_thread, this );

    std::cout << "Thread initialised!!" << std::endl;
}

Motor::~Motor(){
	std::cout << "Motor destroyed" << std::endl;
}

void	Motor::server_read_thread(){
	std::cout << "Starting server read thread !" << std::endl;

    uint8_t receiveBuffer[ 4000000 ];

    while( !stopServerReadThreadAsked_ )
    {
        // any time : read incoming messages.
        int readSize = (int) read( socket_desc_, receiveBuffer, 4000000 );

        if (readSize > 0)
        {
            bool packetHeaderDetected = false;

            bool atLeastOnePacketReceived = naioCodec_.decode( receiveBuffer, static_cast<uint>( readSize ), packetHeaderDetected );

            // manage received messages
            if (atLeastOnePacketReceived)
            {
            	std::cout << readSize << std::endl;
                for ( auto &&packetPtr : naioCodec_.currentBasePacketList )
                {
                    manageReceivedPacket( packetPtr );
                }

                naioCodec_.currentBasePacketList.clear();
            }
        }
    }

    serverReadthreadStarted_ = false;
    stopServerReadThreadAsked_= false;
}

void	Motor::server_write_thread(){
	std::cout << "I'm the write thread" << std::endl;
}
