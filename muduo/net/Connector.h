#ifndef MUDUO_NET_CONNECTOR_H
#define MUDUO_NET_CONNECTOR_H

#include "muduo/base/noncopyable.h"
#include "muduo/net/InetAddress.h"

#include <functional>
#include <memory>

namespace muduo
{
  namespace net
  {
    class Channel;
    class EventLoop;
    class Connector : noncopyable,
                      public std::enable_shared_from_this<Connector>
    {
    public:
      typedef std::function<void(int sockfd)> NewConnectionCallback;
      Connector(EventLoop *loop, const InetAddress &serverAddr);
      ~Connector();
      // 设置新连接回调函数
      void setNewConnectionCallback(const NewConnectionCallback &cb)
      {
        newConnectionCallback_ = cb;
      }
      // 开始
      void start(); // can be called in any thread
      // 重启
      void restart(); // must be called in loop thread
      // 停止
      void stop(); // can be called in any thread
      const InetAddress &serverAddress() const { return serverAddr_; }

    private:
      enum States
      {
        kDisconnected, // 未连接
        kConnecting,   // 连接中
        kConnected     // 已连接
      };
      static const int kMaxRetryDelayMs = 30 * 1000;
      static const int kInitRetryDelayMs = 500;
      void setState(States s) { state_ = s; }
      void startInLoop();
      void stopInLoop();
      void connect();
      void connecting(int sockfd);
      void handleWrite();
      void handleError();
      void retry(int sockfd);
      int removeAndResetChannel();
      void resetChannel();

      EventLoop *loop_;
      // 服务端地址
      InetAddress serverAddr_;

      bool connect_; // atomic
      // 连接状态
      States state_; // FIXME: use atomic variable
      // channel
      std::unique_ptr<Channel> channel_;
      NewConnectionCallback newConnectionCallback_;
      int retryDelayMs_;
    };
  } // namespace net
} // namespace muduo

#endif // MUDUO_NET_CONNECTOR_H
