//
// Created by zhanglei on 2020/6/29.
//

#ifndef DEVICE_SERVER_MQTTTYPE_H
#define DEVICE_SERVER_MQTTTYPE_H
enum{
    MQTT_CONNECT = 0x10,
    MQTT_CONNECTACK = 0x20,
    MQTT_PUBLISH = 0x30,
    MQTT_PUBACK = 0x40,
    MQTT_PUBREC = 0x50,
    MQTT_PUBREL = 0x60,
    MQTT_PUBCOMP = 0x70,
    MQTT_SUBSCRIBE = 0x80,
    MQTT_SUBACK = 0x90,
    MQTT_UNSUBSCRIBE = 0xA0,
    MQTT_UNSUBACK = 0xB0,
    MQTT_PINGREQ = 0xC0,
    MQTT_PINGRESP = 0xD0,
    MQTT_DISCONNECT = 0xE0
};

enum{
    MQTT_CONNECT_TYPE = 1,
    MQTT_CONNECTACK_TYPE = 2,
    MQTT_PUBLISH_TYPE = 3,
    MQTT_PUBACK_TYPE = 4,
    MQTT_PUBREC_TYPE = 5,
    MQTT_PUBREL_TYPE = 6,
    MQTT_PUBCOMP_TYPE = 7,
    MQTT_SUBSCRIBE_TYPE = 8,
    MQTT_SUBACK_TYPE = 9,
    MQTT_UNSUBSCRIBE_TYPE = 10,
    MQTT_UNSUBACK_TYPE = 11,
    MQTT_PINGREQ_TYPE = 12,
    MQTT_PINGRESP_TYPE = 13,
    MQTT_DISCONNECT_TYPE = 14
};

#define QUALITY_LEVEL_ZERO 0
#define QUALITY_LEVEL_ONE 1
#define QUALITY_LEVEL_TWO 2

#define CONNACK_ACCEPTED 0
#define CONNACK_REFUSED_PROTOCOL_VERSION 1
#define CONNACK_REFUSED_IDENTIFIER_REJECTED 2

#define SUBSCRIBE_TOPIC "topic"
#define SUBSCRIBE_MESSAGE_ID "message_id"
#define SUBSCRIBE_QOS_LEVEL "qos_level"

//获取字节高位
#define MSB(message_id) static_cast<uint8_t>((message_id & 0xFF00) >> 8)
#define LSB(message_id) static_cast<uint8_t>(message_id & 0x00FF)

#define UINT8_LEN 1
#define UINT16_LEN 2
#endif //DEVICE_SERVER_MQTTTYPE_H
