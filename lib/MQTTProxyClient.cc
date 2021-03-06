//
// Created by zhanglei on 2020/7/16.
//

#include "autoload.h"
#include "MQTTProxyClient.h"
#include "MQTTContainerGlobal.h"
#include "TimingWheel.h"


void MQTTProxy::MQTTProxyClient::onMessage(const muduo::net::TcpConnectionPtr &conn, muduo::net::Buffer *buf,
                                           muduo::Timestamp receiveTime) {
    //解析返回的协议数据如果说连接成功则给下端发送连接确认
    handle->parse(buf);

    TimingCell::WeakTimeCellPtr weakCell(boost::any_cast<TimingCell::WeakTimeCellPtr>(Conn->getContext()));

    TimingCell::TimeCellPtr cell(weakCell.lock());
    if (cell) {
        muduo::ThreadLocalSingleton<TimingWheel>::instance().bindTimeCell(cell);
    }
}

void MQTTProxy::MQTTProxyClient::onConnection(const muduo::net::TcpConnectionPtr &conn) {
    Conn = conn;

    //加入时间轮
    TimingCell::TimeCellPtr cell(new TimingCell(Conn));
    cell->setMQTTProxyType(DEVICE_CENTER);
    muduo::ThreadLocalSingleton<TimingWheel>::instance().bindTimeCell(cell);
    TimingCell::WeakTimeCellPtr weakTimeCell(cell);
    Conn->setContext(weakTimeCell);

    //连接成功后发送握手协议
    MQTTProxy::MQTTProxyProtocol protocol;
    protocol.ProtocolType = MQTT_PROXY;
    protocol.MessageType = PROXY_CONNECT;
    protocol.MessageNo = RETURN_OK;
    protocol.ClientIdLength = 0;
    protocol.MessageLength = 0;
    sendProxyData(protocol);
}

void MQTTProxy::MQTTProxyClient::onClose(const muduo::net::TcpConnectionPtr &conn) {

}

bool MQTTProxy::MQTTProxyClient::sendProxyData(const MQTTProxy::MQTTProxyProtocol &protocol) {
    //防止不在一个线程中的错误
    Loop->assertInLoopThread();
    //导入协议类型
    //压缩封装协议到设备中台去查询设备是否可以接入
    std::vector<uint8_t> buffer;
    buffer.push_back(protocol.ProtocolType);
    //消息类型
    buffer.push_back(protocol.MessageType);
    //消息的错误码
    buffer.push_back(protocol.MessageNo);
    //客户端id的长度
    std::string clientIdLengthTmp;
    uint32_t clientIdLength = htonl(protocol.ClientId.length());
    for (uint8_t l = 0; l < 4; l++) {
        buffer.push_back(*((uint8_t *) &clientIdLength + l));
    }

    //客户端id的内容
    buffer.insert(buffer.end(), protocol.ClientId.begin(), protocol.ClientId.end());
    //载荷的长度
    std::vector<__uint8_t> message_len = MQTTContainer.Util.encodeRemainingLength(protocol.MessageLength);
    buffer.insert(buffer.end(), message_len.begin(), message_len.end());
    //载荷的内容
    buffer.insert(buffer.end(), protocol.Payload.begin(), protocol.Payload.end());
    //CRC校验的具体信息
    uint16_t crc16 = MQTTContainer.Util.checkCRC16(buffer.data(), buffer.size());
    buffer.push_back(MSB(crc16));
    buffer.push_back(LSB(crc16));
    //发送buffer
    if (Conn) {
        Conn->send(buffer.data(), buffer.size());
        return true;
    } else {
        return false;
    }
}

void MQTTProxy::MQTTProxyClient::heart() {
    if (Conn) {
        std::weak_ptr<muduo::net::TcpConnection> weak(Conn);
        if (!weak.expired()) {
            muduo::net::TcpConnectionPtr tcpConn = weak.lock();
            if (tcpConn->connected()) {
                //发送心跳包
                MQTTProxy::MQTTProxyProtocol protocol;
                protocol.ProtocolType = MQTT_PROXY;
                protocol.MessageType = PROXY_PINGREQ;
                protocol.MessageNo = RETURN_OK;
                protocol.ClientIdLength = 0;
                protocol.MessageLength = 0;
                sendProxyData(protocol);
            }
        }
    }
}

bool MQTTProxy::MQTTProxyClient::start() {
    Client = std::make_shared<muduo::net::TcpClient>(Loop, ConnectAddr, "MQTTProxyClient");
    Client->setConnectionCallback(
            std::bind(&MQTTProxyClient::onConnection, this, _1));
    Client->setMessageCallback(
            std::bind(&MQTTProxyClient::onMessage, this, _1, _2, _3));
    Client->enableRetry();

    //获取配置名字
    std::string proxy_heart_time_config = MQTTContainer.Config.getConfig("proxy_heart_time");
    if (proxy_heart_time_config.empty()) {
        std::cerr << "Heartbeat packet configuration cannot be empty" << std::endl;
        exit(-1);
    }

    double proxy_heart_time = atof(proxy_heart_time_config.c_str());
    if (proxy_heart_time <= 0) {
        std::cerr << "The heartbeat packet configuration cannot be less than 0" << std::endl;
        exit(-1);
    }

    Loop->runEvery(proxy_heart_time, std::bind(&MQTTProxyClient::heart, this));
    connect();
    return true;
}