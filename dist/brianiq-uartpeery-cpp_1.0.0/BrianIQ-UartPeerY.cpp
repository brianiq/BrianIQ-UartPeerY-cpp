#include "BrianIQ-UartPeerY.hpp"
#include <chrono>
#include <cstdint>
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_err.h"
#include "esp_sleep.h"
#include <stdexcept>
#include <vector>

namespace
        BrianIQ {
        UartPeerY:: UartPeerY () { }
        UartPeerY::~UartPeerY () { dnit (); }
        int8_t  UartPeerY::init (std::string &E, uint8_t UartHardwareDriver, uint8_t TXPin,
                uint8_t RXPin, uint8_t RTSPin, uint8_t DataSize, uint8_t ParityBit,
                uint8_t StopBit, uint64_t BaudRate, uint64_t TxBufferSize, uint64_t RxBufferSize) {
                /***1***/
                U = UartHardwareDriver;
                T = TXPin;
                R = RXPin;
                X = RTSPin;
                D = DataSize;
                P = ParityBit;
                S = StopBit;
                B = BaudRate;
                /***2***/
                if (U !=  0  && U !=  1  && U !=  2) {
                        E = std::string ("UartDriver: UART hardware interface ID invalid");
                        return -1;
                }
                if (D !=  5  && D !=  6  && D !=  7  && D !=  8  && D !=  9 ) {
                        E = std::string ("DataSize: Data size not between 5 and 9");
                        return -2;
                }
                if (P != 'N' && P != 'S' && P != 'M' && P != 'O' && P != 'E') {
                        E = std::string ("Parity: Parity bit specified does not exist");
                        return -3;
                }
                if (S !=  1  && S !=  2  && S !=  3) {
                        E = std::string ("StopBit: Stop bit invalid");
                        return -4;
                }
                if (B ==  0) {
                        E = std::string ("BaudRate: Baud rate invalid");
                        return -5;
                }
                /***2***/
                uint64_t TxBufferSizeMin = 0;
                uint64_t RxBufferSizeMin = 1;
                uint8_t  RxFullThreshold = 1;
                if (TxBufferSize < TxBufferSizeMin) {
                        E = std::format ("TxBufferSize: Buffer size may not be less than {}", TxBufferSizeMin);
                        return -6;
                }
                if (RxBufferSize < RxBufferSizeMin) {
                        E = std::format ("RxBufferSize: Buffer size may not be less than {}", RxBufferSizeMin);
                        return -7;
                }
                /***3***/
                uart_port_t           AE01 = UART_NUM_2;
                if        (U == 0 ) { AE01 = UART_NUM_0;
                } else if (U == 1 ) { AE01 = UART_NUM_1;
                } else if (U == 2 ) { AE01 = UART_NUM_2;
                }
                esp_err_t AF05 = uart_driver_install (AE01, RxBufferSize, TxBufferSize, 0, NULL, 0);
                if (AF05 != ESP_OK) {
                        E = std::string ("Interface driver installation failed [") + esp_err_to_name(AF05) + std::string ("]");
                        return -8;
                }
                uart_word_length_t    AF12 = UART_DATA_8_BITS;
                if        (D == 5 ) { AF12 = UART_DATA_5_BITS;
                } else if (D == 6 ) { AF12 = UART_DATA_6_BITS;
                } else if (D == 7 ) { AF12 = UART_DATA_7_BITS;
                } else if (D == 8 ) { AF12 = UART_DATA_8_BITS;
                }
                uart_parity_t         AF17 = UART_PARITY_DISABLE;
                if        (P =='E') { AF17 = UART_PARITY_EVEN;
                } else if (P =='O') { AF17 = UART_PARITY_ODD ;
                } else              { AF17 = UART_PARITY_DISABLE;
                }
                uart_stop_bits_t      AF25 = UART_STOP_BITS_1;
                if        (S == 1 ) { AF25 = UART_STOP_BITS_1;
                } else if (S == 2 ) { AF25 = UART_STOP_BITS_2;
                } else if (S == 3 ) { AF25 = UART_STOP_BITS_1_5;
                }
                uart_config_t AE35;
                AE35.source_clk= UART_SCLK_DEFAULT;
                AE35.rx_flow_ctrl_thresh = RxFullThreshold;
                AE35.baud_rate = B;
                AE35.data_bits = AF12;
                AE35.parity    = AF17;
                AE35.stop_bits = AF25;
                AE35.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
                AE35.flags.allow_pd = 0;
                esp_err_t AF40 = uart_param_config (AE01, &AE35);
                if (AF40 != ESP_OK) {
                        E = std::string ("Interface conf failed [") + esp_err_to_name(AF40) + std::string ("]");
                        return -9;
                }                
                esp_err_t AF45 = uart_set_pin (AE01, T, R, X, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
                if (AF45 != ESP_OK) {
                        E = std::string ("Interface conf set failed, 'pin' [") + esp_err_to_name(AF45) + std::string ("]");
                        return -10;
                }
                esp_err_t AF50 = uart_set_mode (AE01, UART_MODE_RS485_HALF_DUPLEX);
                if (AF50 != ESP_OK) {
                        E = std::string ("Interface conf set failed, 'mode' [") + esp_err_to_name(AF50) + std::string ("]");
                        return -11;
                }
                esp_err_t AF55 = uart_set_rx_full_threshold (AE01, 1);
                if (AF55 != ESP_OK) {
                        E = std::string ("Interface conf set failed, 'rx_full_threshold' [") + esp_err_to_name(AF55) + std::string ("]");
                        return -12;
                }
                esp_err_t AF60 = uart_set_rx_timeout (AE01, 20);
                if (AF60 != ESP_OK) {
                        E = std::string ("Interface conf set failed, 'rx_timeout' [") + esp_err_to_name(AF60) + std::string ("]");
                        return -13;
                }
                uart_set_always_rx_timeout (AE01, true);
                I = 1;
                return 0;
        }
        int8_t  UartPeerY::dnit () {
                if (I == 0) { return 0; }
                uart_port_t           AE01 = UART_NUM_2;
                if        (U == 0 ) { AE01 = UART_NUM_0;
                } else if (U == 1 ) { AE01 = UART_NUM_1;
                } else if (U == 2 ) { AE01 = UART_NUM_2;
                }
                uart_driver_delete (AE01);
                I = 0;
                return 0;
        }
        int8_t  UartPeerY::wrtt (std::string &E, std::vector<uint8_t> &S) { /*
                E: Errr note
                D: Seed data
                RETURN:
                         0: Success
                        -1: Generic failed error code
                        -2: Internl error occured
                        -3: Invalid seed
                        -4: Invalid request
                */
                uart_port_t           AE01 = UART_NUM_2;
                if        (U == 0 ) { AE01 = UART_NUM_0;
                } else if (U == 1 ) { AE01 = UART_NUM_1;
                } else if (U == 2 ) { AE01 = UART_NUM_2;
                }
                char*AE10 = reinterpret_cast<char*>(S.data());
                int  AE15 = uart_write_bytes ( AE01, AE10, S.size());
                if(  AE15 < 0) {
                        E = std::format ("Data char send failed [{}]", AE15);
                        return -2;
                }
                return  0;
        }
        int8_t  UartPeerY::read (std::string &E, std::vector<uint8_t> &Y, uint64_t Z, uint64_t T) { /*
                E: Errr note
                Y: Read data (Yield)
                Z: Read data size
                         0: All immediately available data
                         Other values: Fixed data length of that size
                T: Read timeout in millisecond:
                         0: Un-limited / No waiting
                RETURN:
                         1: Timeout
                         0: Success
                        -1: Generic failed error code
                        -2: Internl error occured
                        -3: Invalid seed
                        -4: Invalid request
                */
                uart_port_t           AE01 = UART_NUM_2;
                if        (U == 0 ) { AE01 = UART_NUM_0;
                } else if (U == 1 ) { AE01 = UART_NUM_1;
                } else if (U == 2 ) { AE01 = UART_NUM_2;
                }
                std::chrono::time_point AE05 = std::chrono::steady_clock::now ();
                int AE10[1] = {0};
                while(true) {
                        /***1***/
                        int BE05 = uart_read_bytes (AE01, AE10,1, 1);
                        /***2***/
                        if         (BE05 < 0) {
                                E = std::string ("Data read failed");
                                return -2;
                        } else  if (BE05 == 0) {
                                if (   Z == 0) { return 0; }
                        } else  {
                                Y.push_back (AE10[0]);
                                std::string  s (1, AE10[0]);
                                if (Y.size() == Z) { return 0; }
                        }
                        /***3***/
                        if(    T < 1) { continue; }
                        std::chrono::time_point  BE10 = std::chrono::steady_clock::now ();
                        std::chrono::duration<double> BE15 = BE10 - AE05;
                        std::chrono::nanoseconds BE20 = std::chrono::duration_cast<std::chrono::nanoseconds> (BE15);
                        uint64_t BE25 = static_cast<uint64_t>(BE20.count ());
                        BE25 = BE25 / 1000 / 1000 ;
                        if( BE25 > T) { return 1; }
                }
        }
}
