#ifndef FRAMEWORK_PROTOCOL_INCLUDE_CODEC_HPP_
#define FRAMEWORK_PROTOCOL_INCLUDE_CODEC_HPP_

#include <functional>

#include <nocopyable.hpp>
#include <Buffer.hpp>
#include <TcpConnection.hpp>

namespace Net {

class ProtobufCodec : public Base::nocopyable {
 public:
    enum ErrorCode {
        NOERROR             = 0,
        INVALIDLENGTH       = 1,
        CHECKSUMERROR       = 2,
        INVALIDNAMELEN      = 3,
        UNKOWNMESSAGETYPE   = 4,
        PARESERROR          = 5
    };

    typedef std::function<void (const TcpConnectionPtr&, const MessagePtr&)> 
        ProtobufMessageCallback;

    typedef std::function<void (const muduo::net::TcpConnectionPtr&,
                                muduo::net::Buffer*,
                                ErrorCode)> ErrorCallback;

    explicit ProtobufCodec(const ProtobufMessageCallback &callback);

    ProtobufCodec(
        const ProtobufMessageCallback &message,
        const ErrorCallback &error);

    ~ProtobufCodec();

    void OnMessage(const TcpConnection &connect, Buffer *buffer);

    void Send(
        const TcpConnection &connect,
        const google::protobuf::Message &message);
 private:
    
};
}


#endif

