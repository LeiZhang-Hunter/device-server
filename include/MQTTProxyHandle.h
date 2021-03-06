//
// Created by zhanglei on 2020/7/28.
//

#ifndef DEVICE_SERVER_MQTTPROXYHANDLE_H
#define DEVICE_SERVER_MQTTPROXYHANDLE_H

#include "ProxyProtocol.h"

namespace MQTTProxy {
    class MQTTProxyHandle {

    public:
        MQTTProxyHandle();

        bool OnConnectMessage(const std::shared_ptr<MQTTProxy::MQTTProxyProtocol> &);

        bool OnDisConnectMessage();

        bool OnSubscribeMessage(const std::shared_ptr<MQTTProxy::MQTTProxyProtocol> &protocol);

        bool OnUnSubscribeMessage();

        bool OnPublishMessage(const std::shared_ptr<MQTTProxy::MQTTProxyProtocol> &protocol);

        bool OnPublish();

        ~MQTTProxyHandle();

    private:

    };
}

#endif //DEVICE_SERVER_MQTTPROXYHANDLE_H
