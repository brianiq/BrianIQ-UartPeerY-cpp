#ifndef BrianIQ_UartPeerY
#define BrianIQ_UartPeerY

#include <cstdint>
#include <string>
#include <vector>

namespace
        BrianIQ {
class   UartPeerY {
        /***1***/
        public:
        uint8_t  U;
        uint8_t  D;
        uint8_t  P;
        uint8_t  S;
        uint8_t  T;
        uint8_t  R;
        uint8_t  X;
        uint64_t B;
        uint8_t  I; // Init status
        /***2***/
        UartPeerY ();
       ~UartPeerY ();
        /***3***/
        int8_t init (
                std::string &E, uint8_t UartHardwareDriver, uint8_t TXPin, uint8_t RXPin,
                uint8_t RTSPin, uint8_t DataSize, uint8_t ParityBit, uint8_t StopBit,
                uint64_t BaudRate, uint64_t TxBufferSize, uint64_t RxBufferSize
        );
        int8_t dnit ();
        int8_t wrtt (std::string &E, std::vector<uint8_t> &S);
        int8_t read (std::string &E, std::vector<uint8_t> &Y, uint64_t S, uint64_t T);
};
}
#endif
