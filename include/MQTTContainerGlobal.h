//
// Created by zhanglei on 2020/7/2.
//

#ifndef DEVICE_SERVER_MQTTCONTAINERGLOBAL_H
#define DEVICE_SERVER_MQTTCONTAINERGLOBAL_H

typedef struct _MQTTContainerGlobal : public muduo::noncopyable
{
public:
    _MQTTContainerGlobal() :mutex()
    {

    }

    bool globalInit()
    {
        sessionPool = std::make_shared<DeviceServer::MQTTClientSessionPool>();
        return true;
    }

    muduo::MutexLock mutex;
    DeviceServerLib::Util util;
    std::shared_ptr<DeviceServer::MQTTClientSessionPool> sessionPool;

    ~_MQTTContainerGlobal() = default;
}MQTTContainerGlobal;

extern MQTTContainerGlobal MQTTContainer;

#endif //DEVICE_SERVER_MQTTCONTAINERGLOBAL_H
