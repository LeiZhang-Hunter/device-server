//
// Created by zhanglei on 2020/7/14.
//
#include "autoload.h"
#include "MQTTSessionHandle.h"
#include "MQTTProtocol.h"
#include "ProxyProtocol.h"
#include "MQTTContainerGlobal.h"
#include "MQTTProxyClient.h"

//连接建立
bool MQTTProxy::MQTTSessionHandle::OnConnect(const MQTTProxy::Callback::MQTTClientSessionPtr &session) {
    uint8_t connectAck = 0;
    if (session->getCleanSession() == 0)
        connectAck |= 0x01;
    MQTTProxy::MQTTProxyProtocol protocol;
    //连接的标志
    protocol.MessageType = MQTTProxy::CONNECT;
    //返回码
    protocol.MessageNo = MQTTProxy::RETURN_OK;
    //客户端id的长度
    protocol.ClientIdLength = session->getClientId().length();
    //客户端id
    protocol.ClientId = session->getClientId();
    //查看是否要输入账号密码，如果要输入则需要在载荷中加入账号和密码
    Json::Value proto_builder;
    if (session->getUserNameFlag()) {
        proto_builder["username"] = session->getUserName();
    }
    if (session->getPasswordFlag()) {
        proto_builder["password"] = session->getPassword();
    }
    if (proto_builder.size() > 0) {
        Json::String encode_string = MQTTContainer.Util.jsonEncode(proto_builder);
        protocol.Payload.insert(
                protocol.Payload.end(),
                encode_string.begin(),
                encode_string.end());
    }
    protocol.MessageLength = protocol.Payload.size();
    //发送消息到设备中心
    bool res = MQTTContainer.getProxyClient()->sendProxyData(protocol);
    if (!res) {
        if (session->getConn()) {
            res = response.sendConnectAck(session->getConn(), CONNACK_REFUSED_IDENTIFIER_REJECTED, 0);

            if (!res) {
                session->getConn()->forceClose();
            }
        }
    }
    //response.sendConnectAck(session->getConn(), connectAck, CONNACK_ACCEPTED);
    return true;
}

//订阅事件
bool MQTTProxy::MQTTSessionHandle::OnSubscribe(const MQTTProxy::Callback::MQTTClientSessionPtr &session,
                                               const MQTTProxy::MQTTSubscribe &subscribe) {
    MQTTProxy::MQTTProxyProtocol protocol;
    //连接的标志
    protocol.MessageType = MQTTProxy::SUBSCRIBE;
    //返回码
    protocol.MessageNo = MQTTProxy::RETURN_OK;
    //客户端id的长度
    protocol.ClientIdLength = session->getClientId().length();
    //客户端id
    protocol.ClientId = session->getClientId();
    //加入订阅的主题
    Json::Value proto_builder;
    proto_builder[SUBSCRIBE_TOPIC] = subscribe.topic;
    proto_builder[SUBSCRIBE_MESSAGE_ID] = subscribe.messageId;
    proto_builder[SUBSCRIBE_QOS_LEVEL] = subscribe.QosLevel;
    if (proto_builder.size() > 0) {
        Json::String encode_string = MQTTContainer.Util.jsonEncode(proto_builder);
        protocol.Payload.insert(
                protocol.Payload.end(),
                encode_string.begin(),
                encode_string.end());
    }
    protocol.MessageLength = protocol.Payload.size();
    //发送消息到设备中心
    bool res = MQTTContainer.getProxyClient()->sendProxyData(protocol);
    if (!res) {
        session->getConn()->forceClose();
    }
    return true;
}

//取消订阅事件
bool MQTTProxy::MQTTSessionHandle::OnUnSubscribe(const MQTTProxy::Callback::MQTTClientSessionPtr &session,
                                                 const MQTTProxy::MQTTSubscribe &subscribe) {
    MQTTProxy::MQTTProxyProtocol protocol;
    //连接的标志
    protocol.MessageType = MQTTProxy::UNSUBSCRIBE;
    //返回码
    protocol.MessageNo = MQTTProxy::RETURN_OK;
    //客户端id的长度
    protocol.ClientIdLength = session->getClientId().length();
    //客户端id
    protocol.ClientId = session->getClientId();
    //加入取消订阅的主题
    Json::Value proto_builder;
    proto_builder[SUBSCRIBE_TOPIC] = subscribe.topic;
    proto_builder[SUBSCRIBE_MESSAGE_ID] = subscribe.messageId;
    proto_builder[SUBSCRIBE_QOS_LEVEL] = subscribe.QosLevel;
    if (proto_builder.size() > 0) {
        Json::String encode_string = MQTTContainer.Util.jsonEncode(proto_builder);
        protocol.Payload.insert(
                protocol.Payload.end(),
                encode_string.begin(),
                encode_string.end());
    }
    protocol.MessageLength = protocol.Payload.size();
    //发送消息到设备中心
    if (!MQTTContainer.getProxyClient()->sendProxyData(protocol)) {
        session->getConn()->forceClose();
    }
    return true;
}

//收到相关主题的推送
void MQTTProxy::MQTTSessionHandle::OnPublish(const MQTTProxy::Callback::MQTTClientSessionPtr &session,
                                             const MQTTProxy::MQTTSubscribe &subscribe, const std::string &message
) {
    MQTTProxy::MQTTProxyProtocol protocol;
    //连接的标志
    protocol.MessageType = MQTTProxy::PUBLISH;
    //返回码
    protocol.MessageNo = MQTTProxy::RETURN_OK;
    //客户端id的长度
    protocol.ClientIdLength = session->getClientId().length();
    //客户端id
    protocol.ClientId = session->getClientId();
    Json::Value proto_builder;
    proto_builder[SUBSCRIBE_TOPIC] = subscribe.topic;
    proto_builder[PUBLISH_MSG] = message;
    proto_builder[PUBLISH_MSG_ID] = subscribe.messageId;
    proto_builder[PUBLISH_QOS_LEVEL] = subscribe.QosLevel;
    if (proto_builder.size() > 0) {
        Json::String encode_string = MQTTContainer.Util.jsonEncode(proto_builder);
        protocol.Payload.insert(
                protocol.Payload.end(),
                encode_string.begin(),
                encode_string.end());
    }
    protocol.MessageLength = protocol.Payload.size();
    //发送消息到设备中心
    if (!MQTTContainer.getProxyClient()->sendProxyData(protocol)) {
        session->getConn()->forceClose();
    }
}

//关闭会话触发的事件
bool MQTTProxy::MQTTSessionHandle::OnDisConnect(const MQTTProxy::Callback::MQTTClientSessionPtr &session) {
    MQTTProxy::MQTTProxyProtocol protocol;
    //连接的标志
    protocol.MessageType = MQTTProxy::DISCONNECT;
    //返回码
    protocol.MessageNo = MQTTProxy::RETURN_OK;
    //客户端id的长度
    protocol.ClientIdLength = session->getClientId().length();
    //客户端id
    protocol.ClientId = session->getClientId();
    //发送消息到设备中心
    if (!MQTTContainer.getProxyClient()->sendProxyData(protocol)) {

    }
    return true;
}