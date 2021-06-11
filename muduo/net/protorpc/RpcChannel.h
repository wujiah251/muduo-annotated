#ifndef MUDUO_NET_PROTORPC_RPCCHANNEL_H
#define MUDUO_NET_PROTORPC_RPCCHANNEL_H

#include "muduo/base/Atomic.h"
#include "muduo/base/Mutex.h"
#include "muduo/net/protorpc/RpcCodec.h"

#include <google/protobuf/service.h>

#include <map>

namespace google
{
  namespace protobuf
  {

    // Defined in other files.
    class Descriptor;        // descriptor.h
    class ServiceDescriptor; // descriptor.h
    class MethodDescriptor;  // descriptor.h
    class Message;           // message.h

    class Closure;

    class RpcController;
    class Service;

  } // namespace protobuf
} // namespace google

namespace muduo
{
  namespace net
  {

    class RpcChannel : public ::google::protobuf::RpcChannel
    {
    public:
      // 构造函数
      RpcChannel();
      explicit RpcChannel(const TcpConnectionPtr &conn);
      // 析构函数
      ~RpcChannel() override;
      // 设置Tcp连接
      void setConnection(const TcpConnectionPtr &conn)
      {
        conn_ = conn;
      }
      void setServices(const std::map<std::string, ::google::protobuf::Service *> *services)
      {
        services_ = services;
      }
      // RPC回调
      void CallMethod(const ::google::protobuf::MethodDescriptor *method,
                      ::google::protobuf::RpcController *controller,
                      const ::google::protobuf::Message *request,
                      ::google::protobuf::Message *response,
                      ::google::protobuf::Closure *done) override;
      void onMessage(const TcpConnectionPtr &conn,
                     Buffer *buf,
                     Timestamp receiveTime);

    private:
      // 远程过程调用消息回调
      void onRpcMessage(const TcpConnectionPtr &conn,
                        const RpcMessagePtr &messagePtr,
                        Timestamp receiveTime);
      void doneCallback(::google::protobuf::Message *response, int64_t id);
      struct OutstandingCall
      {
        ::google::protobuf::Message *response;
        ::google::protobuf::Closure *done;
      };
      RpcCodec codec_;
      TcpConnectionPtr conn_;
      AtomicInt64 id_;
      MutexLock mutex_;
      std::map<int64_t, OutstandingCall> outstandings_ GUARDED_BY(mutex_);
      const std::map<std::string, ::google::protobuf::Service *> *services_;
    };
    typedef std::shared_ptr<RpcChannel> RpcChannelPtr;
  } // namespace net
} // namespace muduo

#endif // MUDUO_NET_PROTORPC_RPCCHANNEL_H
