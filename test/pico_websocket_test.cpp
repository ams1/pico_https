#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <stdarg.h>

#include "pico_http/websocket_handler.h"

const unsigned char websocket_packet[] = { 0x81, 0x94, 0x32, 0x60, 0x3e, 0x3e, 0x5a, 0x05, 0x52, 0x52, 0x5d, 0x40, 0x58, 0x4c, 0x5d, 0x0d, 0x1e, 0x49, 0x57, 0x02, 0x4d, 0x51, 0x51, 0x0b, 0x5b, 0x4a };
const uint32_t websocket_packet_size = sizeof(websocket_packet);
const uint32_t websocket_partial_packet_size = 11; // "hello"
const uint32_t websocket_header_size = 6;
const char *websocket_payload = "hello from websocket";

const unsigned char websocket_large_packet[] = { 0x81, 0xfe, 0x02, 0x3e, 0x5f, 0x23, 0x09, 0xa9, 0x13, 0x4c, 0x7b, 0xcc,
                                                 0x32, 0x03, 0x40, 0xd9, 0x2c, 0x56, 0x64, 0x89, 0x36, 0x50, 0x29, 0xda, 0x36, 0x4e, 0x79, 0xc5,
                                                 0x26, 0x03, 0x6d, 0xdc, 0x32, 0x4e, 0x70, 0x89, 0x2b, 0x46, 0x71, 0xdd, 0x7f, 0x4c, 0x6f, 0x89,
                                                 0x2b, 0x4b, 0x6c, 0x89, 0x2f, 0x51, 0x60, 0xc7, 0x2b, 0x4a, 0x67, 0xce, 0x7f, 0x42, 0x67, 0xcd,
                                                 0x7f, 0x57, 0x70, 0xd9, 0x3a, 0x50, 0x6c, 0xdd, 0x2b, 0x4a, 0x67, 0xce, 0x7f, 0x4a, 0x67, 0xcd,
                                                 0x2a, 0x50, 0x7d, 0xdb, 0x26, 0x0d, 0x29, 0xe5, 0x30, 0x51, 0x6c, 0xc4, 0x7f, 0x6a, 0x79, 0xda,
                                                 0x2a, 0x4e, 0x29, 0xc1, 0x3e, 0x50, 0x29, 0xcb, 0x3a, 0x46, 0x67, 0x89, 0x2b, 0x4b, 0x6c, 0x89,
                                                 0x36, 0x4d, 0x6d, 0xdc, 0x2c, 0x57, 0x7b, 0xd0, 0x78, 0x50, 0x29, 0xda, 0x2b, 0x42, 0x67, 0xcd,
                                                 0x3e, 0x51, 0x6d, 0x89, 0x3b, 0x56, 0x64, 0xc4, 0x26, 0x03, 0x7d, 0xcc, 0x27, 0x57, 0x29, 0xcc,
                                                 0x29, 0x46, 0x7b, 0x89, 0x2c, 0x4a, 0x67, 0xca, 0x3a, 0x03, 0x7d, 0xc1, 0x3a, 0x03, 0x38, 0x9c,
                                                 0x6f, 0x13, 0x7a, 0x85, 0x7f, 0x54, 0x61, 0xcc, 0x31, 0x03, 0x68, 0xc7, 0x7f, 0x56, 0x67, 0xc2,
                                                 0x31, 0x4c, 0x7e, 0xc7, 0x7f, 0x53, 0x7b, 0xc0, 0x31, 0x57, 0x6c, 0xdb, 0x7f, 0x57, 0x66, 0xc6,
                                                 0x34, 0x03, 0x68, 0x89, 0x38, 0x42, 0x65, 0xc5, 0x3a, 0x5a, 0x29, 0xc6, 0x39, 0x03, 0x7d, 0xd0,
                                                 0x2f, 0x46, 0x29, 0xc8, 0x31, 0x47, 0x29, 0xda, 0x3c, 0x51, 0x68, 0xc4, 0x3d, 0x4f, 0x6c, 0xcd,
                                                 0x7f, 0x4a, 0x7d, 0x89, 0x2b, 0x4c, 0x29, 0xc4, 0x3e, 0x48, 0x6c, 0x89, 0x3e, 0x03, 0x7d, 0xd0,
                                                 0x2f, 0x46, 0x29, 0xda, 0x2f, 0x46, 0x6a, 0xc0, 0x32, 0x46, 0x67, 0x89, 0x3d, 0x4c, 0x66, 0xc2,
                                                 0x71, 0x03, 0x40, 0xdd, 0x7f, 0x4b, 0x68, 0xda, 0x7f, 0x50, 0x7c, 0xdb, 0x29, 0x4a, 0x7f, 0xcc,
                                                 0x3b, 0x03, 0x67, 0xc6, 0x2b, 0x03, 0x66, 0xc7, 0x33, 0x5a, 0x29, 0xcf, 0x36, 0x55, 0x6c, 0x89,
                                                 0x3c, 0x46, 0x67, 0xdd, 0x2a, 0x51, 0x60, 0xcc, 0x2c, 0x0f, 0x29, 0xcb, 0x2a, 0x57, 0x29, 0xc8,
                                                 0x33, 0x50, 0x66, 0x89, 0x2b, 0x4b, 0x6c, 0x89, 0x33, 0x46, 0x68, 0xd9, 0x7f, 0x4a, 0x67, 0xdd,
                                                 0x30, 0x03, 0x6c, 0xc5, 0x3a, 0x40, 0x7d, 0xdb, 0x30, 0x4d, 0x60, 0xca, 0x7f, 0x57, 0x70, 0xd9,
                                                 0x3a, 0x50, 0x6c, 0xdd, 0x2b, 0x4a, 0x67, 0xce, 0x73, 0x03, 0x7b, 0xcc, 0x32, 0x42, 0x60, 0xc7,
                                                 0x36, 0x4d, 0x6e, 0x89, 0x3a, 0x50, 0x7a, 0xcc, 0x31, 0x57, 0x60, 0xc8, 0x33, 0x4f, 0x70, 0x89,
                                                 0x2a, 0x4d, 0x6a, 0xc1, 0x3e, 0x4d, 0x6e, 0xcc, 0x3b, 0x0d, 0x29, 0xe0, 0x2b, 0x03, 0x7e, 0xc8,
                                                 0x2c, 0x03, 0x79, 0xc6, 0x2f, 0x56, 0x65, 0xc8, 0x2d, 0x4a, 0x7a, 0xcc, 0x3b, 0x03, 0x60, 0xc7,
                                                 0x7f, 0x57, 0x61, 0xcc, 0x7f, 0x12, 0x30, 0x9f, 0x6f, 0x50, 0x29, 0xde, 0x36, 0x57, 0x61, 0x89,
                                                 0x2b, 0x4b, 0x6c, 0x89, 0x2d, 0x46, 0x65, 0xcc, 0x3e, 0x50, 0x6c, 0x89, 0x30, 0x45, 0x29, 0xe5,
                                                 0x3a, 0x57, 0x7b, 0xc8, 0x2c, 0x46, 0x7d, 0x89, 0x2c, 0x4b, 0x6c, 0xcc, 0x2b, 0x50, 0x29, 0xca,
                                                 0x30, 0x4d, 0x7d, 0xc8, 0x36, 0x4d, 0x60, 0xc7, 0x38, 0x03, 0x45, 0xc6, 0x2d, 0x46, 0x64, 0x89,
                                                 0x16, 0x53, 0x7a, 0xdc, 0x32, 0x03, 0x79, 0xc8, 0x2c, 0x50, 0x68, 0xce, 0x3a, 0x50, 0x25, 0x89,
                                                 0x3e, 0x4d, 0x6d, 0x89, 0x32, 0x4c, 0x7b, 0xcc, 0x7f, 0x51, 0x6c, 0xca, 0x3a, 0x4d, 0x7d, 0xc5,
                                                 0x26, 0x03, 0x7e, 0xc0, 0x2b, 0x4b, 0x29, 0xcd, 0x3a, 0x50, 0x62, 0xdd, 0x30, 0x53, 0x29, 0xd9,
                                                 0x2a, 0x41, 0x65, 0xc0, 0x2c, 0x4b, 0x60, 0xc7, 0x38, 0x03, 0x7a, 0xc6, 0x39, 0x57, 0x7e, 0xc8,
                                                 0x2d, 0x46, 0x29, 0xc5, 0x36, 0x48, 0x6c, 0x89, 0x1e, 0x4f, 0x6d, 0xdc, 0x2c, 0x03, 0x59, 0xc8,
                                                 0x38, 0x46, 0x44, 0xc8, 0x34, 0x46, 0x7b, 0x89, 0x36, 0x4d, 0x6a, 0xc5, 0x2a, 0x47, 0x60, 0xc7,
                                                 0x38, 0x03, 0x7f, 0xcc, 0x2d, 0x50, 0x60, 0xc6, 0x31, 0x50, 0x29, 0xc6, 0x39, 0x03, 0x45, 0xc6,
                                                 0x2d, 0x46, 0x64, 0x89, 0x16, 0x53, 0x7a, 0xdc, 0x32, 0x0d };

const size_t websocket_large_packet_size = sizeof(websocket_large_packet);
const char *decoded_large_packet = "Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book. It has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchanged. It was popularised in the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum.";

using ::testing::_;

class MockWebSocketInterface : public WebSocketInterface
{
public:
    MOCK_METHOD(bool, onWebSocketData, (uint8_t *data, size_t len), (override));
    MOCK_METHOD(bool, onWebsocketEncodedData, (const uint8_t *data, size_t len), (override));
};

TEST(WebSocketHandler, BasicWebSocketDecode) {

    MockWebSocketInterface mockListener;
    uint8_t *buffer = new uint8_t[websocket_packet_size];

    memcpy(buffer, websocket_packet, websocket_packet_size);
    
    
    EXPECT_CALL(mockListener, onWebSocketData(_,_))
        .WillOnce(testing::WithArgs<0, 1>([](uint8_t *data, size_t len) {
            std::string decoded((const char *)data, len);
            EXPECT_STREQ(decoded.c_str(), "hello from websocket");
            return true;
        }));
            

    WebSocketHandler handler;
    handler.decodeData(buffer, websocket_packet_size, &mockListener);

    delete[] buffer;
}

TEST(WebSocketHandler, PartialDecodes) {

    MockWebSocketInterface mockListener;

    uint8_t *buffer = new uint8_t[websocket_partial_packet_size];

    memcpy(buffer, websocket_packet, websocket_partial_packet_size);
    
    EXPECT_CALL(mockListener, onWebSocketData(_,_))
        .WillOnce(testing::WithArgs<0, 1>([](uint8_t *data, size_t len) {
            std::string decoded((const char *)data, len);
            EXPECT_STREQ(decoded.c_str(), "hello");
            return true;
        }));
            
    WebSocketHandler handler;
    EXPECT_EQ(true, handler.decodeData(buffer, websocket_partial_packet_size, &mockListener));

    delete[] buffer;
    size_t remaining_len = websocket_packet_size - websocket_partial_packet_size;
    buffer = new uint8_t[remaining_len];
    memcpy(buffer, &websocket_packet[websocket_partial_packet_size], remaining_len);

    EXPECT_CALL(mockListener, onWebSocketData(_,_))
        .WillOnce(testing::WithArgs<0, 1>([](uint8_t *data, size_t len) {
            std::string decoded((const char *)data, len);
            EXPECT_STREQ(decoded.c_str(), " from websocket");
            return true;
        }));

    EXPECT_EQ(true, handler.decodeData(buffer, remaining_len, &mockListener));
    
    delete[] buffer;
}

TEST(WebSocketHandler, SingleByteFeeding) {

    MockWebSocketInterface mockListener;
    uint8_t *buffer = new uint8_t[websocket_packet_size];

    memcpy(buffer, websocket_packet, websocket_packet_size);
    
    EXPECT_CALL(mockListener, onWebSocketData(_,_)).Times(0);

    WebSocketHandler handler;

    for (int i=0;i<6;++i)
    {
        EXPECT_EQ(true, handler.decodeData(&buffer[i], 1, &mockListener));
    }

    int pos=0;
    EXPECT_CALL(mockListener, onWebSocketData(_,_))
        .WillRepeatedly(testing::WithArgs<0, 1>([&pos](uint8_t *data, size_t len) {
            EXPECT_EQ(1, len);
            EXPECT_EQ(websocket_payload[pos], data[0]);
            ++pos;
            return true;
        }));

    for (int i=6;i<websocket_packet_size;++i)
    {
        EXPECT_EQ(true, handler.decodeData(&buffer[i], 1, &mockListener)) << "Position: " << i;
    }
    
    delete[] buffer;
}


TEST(WebSocketHandler, MultipleMessages) {

    MockWebSocketInterface mockListener;
    uint8_t *buffer = new uint8_t[websocket_packet_size*2];

    memcpy(buffer, websocket_packet, websocket_packet_size);
    memcpy(&buffer[websocket_packet_size], websocket_packet, websocket_packet_size);

    int call = 1;
    
    EXPECT_CALL(mockListener, onWebSocketData(_,_))
        .Times(2).WillRepeatedly(testing::WithArgs<0, 1>([&call](uint8_t *data, size_t len) {
            std::string decoded((const char *)data, len);
            EXPECT_STREQ(decoded.c_str(), "hello from websocket") << "Failed on call: " << call << ", with len: " << len;
            ++call;
            return true;
        }));
            

    WebSocketHandler handler;
    handler.decodeData(buffer, 2*websocket_packet_size, &mockListener);

    delete[] buffer;
}

TEST(WebSocketHandler, LargeMessage) {

    MockWebSocketInterface mockListener;
    
    uint8_t *buffer = new uint8_t[2*websocket_large_packet_size];
    
    memcpy(buffer, websocket_large_packet, websocket_large_packet_size);
    memcpy(&buffer[websocket_large_packet_size], websocket_large_packet, websocket_large_packet_size);

    int call = 1;
    
    EXPECT_CALL(mockListener, onWebSocketData(_,_))
        .Times(2).WillRepeatedly(testing::WithArgs<0, 1>([&call](uint8_t *data, size_t len) {
            std::string decoded((const char *)data, len);
            EXPECT_STREQ(decoded.c_str(), decoded_large_packet) << "Failed on call: " << call << ", with len: " << len;
            ++call;
            return true;
        }));
            

    WebSocketHandler handler;
    handler.decodeData(buffer, 2*websocket_large_packet_size, &mockListener);

    delete[] buffer;
}
