#ifndef MUDUO_NET_TCPSERVER_H
#define MUDUO_NET_TCPSERVER_H

#include "muduo/base/Atomic.h"
#include "muduo/base/Types.h"
#include "muduo/net/TcpConnection.h"

#include <map>

namespace muduo
{
  namespace net
  {

    class Acceptor;
    class EventLoop;
    class EventLoopThreadPool;

    class TcpServer : noncopyable
    {
    public:
      typedef std::function<void(EventLoop *)> ThreadInitCallback;
      enum Option
      {
        kNoReusePort,
        kReusePort,
      };

      //TcpServer(EventLoop* loop, const InetAddress& listenAddr);
      TcpServer(EventLoop *loop,
                const InetAddress &listenAddr,
                const string &nameArg,
                Option option = kNoReusePort);
      ~TcpServer(); // force out-line dtor, for std::unique_ptr members.

      const string &ipPort() const { return ipPort_; }
      const string &name() const { return name_; }
      EventLoop *getLoop() const { return loop_; }

      void setThreadNum(int numThreads);
      void setThreadInitCallback(const ThreadInitCallback &cb)
      {
        threadInitCallback_ = cb;
      }
      /// valid after calling start()
      std::shared_ptr<EventLoopThreadPool> threadPool()
      {
        return threadPool_;
      }

      void start();

      void setConnectionCallback(const ConnectionCallback &cb)
      {
        connectionCallback_ = cb;
      }

      void setMessageCallback(const MessageCallback &cb)
      {
        messageCallback_ = cb;
      }

      void setWriteCompleteCallback(const WriteCompleteCallback &cb)
      {
        writeCompleteCallback_ = cb;
      }

    private:
      void newConnection(int sockfd, const InetAddress &peerAddr);

      void removeConnection(const TcpConnectionPtr &conn);

      void removeConnectionInLoop(const TcpConnectionPtr &conn);

      typedef std::map<string, TcpConnectionPtr> ConnectionMap;

      EventLoop *loop_; // the acceptor loop
      const string ipPort_;
      const string name_;
      std::unique_ptr<Acceptor> acceptor_; // avoid revealing Acceptor
      std::shared_ptr<EventLoopThreadPool> threadPool_;
      ConnectionCallback connectionCallback_;
      MessageCallback messageCallback_;
      WriteCompleteCallback writeCompleteCallback_;
      ThreadInitCallback threadInitCallback_;
      AtomicInt32 started_;
      // always in loop thread
      int nextConnId_;

      // 存储连接信息
      ConnectionMap connections_;
    };

  } // namespace net
} // namespace muduo

#endif // MUDUO_NET_TCPSERVER_H
