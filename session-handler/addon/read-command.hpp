#pragma once

#include <memory>

#include <v8.h>

class PdalSession;
class BufferTransmitter;

void errorCallback(
        v8::Persistent<v8::Function> callback,
        std::string errMsg);

class ReadCommand
{
public:
    virtual void run() = 0;

    void cancel(bool cancel) { m_cancel = cancel; }
    void errMsg(std::string errMsg) { m_errMsg = errMsg; }

    void transmit(std::size_t offset, std::size_t numBytes);

    std::size_t     numPoints() const { return m_numPoints; }
    std::size_t     numBytes()  const { return m_numBytes;  }
    std::string     errMsg()    const { return m_errMsg;    }
    bool            cancel()    const { return m_cancel;    }
    v8::Persistent<v8::Function> callback() const { return m_callback; }

    ReadCommand(
            std::shared_ptr<PdalSession> pdalSession,
            std::string host,
            std::size_t port,
            v8::Persistent<v8::Function> callback)
        : m_pdalSession(pdalSession)
        , m_host(host)
        , m_port(port)
        , m_callback(callback)
        , m_cancel(false)
        , m_data(0)
        , m_bufferTransmitter()
        , m_errMsg()
        , m_numPoints(0)
        , m_numBytes(0)
    { }

    virtual ~ReadCommand()
    {
        if (m_data)
            delete [] m_data;
    }

protected:
    const std::shared_ptr<PdalSession> m_pdalSession;
    const std::string m_host;
    const std::size_t m_port;

    v8::Persistent<v8::Function> m_callback;
    bool m_cancel;

    unsigned char* m_data;
    std::shared_ptr<BufferTransmitter> m_bufferTransmitter;
    std::string m_errMsg;

    void setNumPoints(std::size_t numPoints);

private:
    std::size_t m_numPoints;
    std::size_t m_numBytes;
};

class ReadCommandUnindexed : public ReadCommand
{
public:
    ReadCommandUnindexed(
            std::shared_ptr<PdalSession> pdalSession,
            std::string host,
            std::size_t port,
            std::size_t start,
            std::size_t count,
            v8::Persistent<v8::Function> callback)
        : ReadCommand(pdalSession, host, port, callback)
        , m_start(start)
        , m_count(count)
    { }

    virtual void run();

private:
    const std::size_t m_start;
    const std::size_t m_count;
};

class ReadCommandPointRadius : public ReadCommand
{
public:
    ReadCommandPointRadius(
            std::shared_ptr<PdalSession> pdalSession,
            std::string host,
            std::size_t port,
            bool is3d,
            double radius,
            double x,
            double y,
            double z,
            v8::Persistent<v8::Function> callback)
        : ReadCommand(pdalSession, host, port, callback)
        , m_is3d(is3d)
        , m_radius(radius)
        , m_x(x)
        , m_y(y)
        , m_z(z)
    { }

    virtual void run();

private:
    const bool m_is3d;
    const double m_radius;
    const double m_x;
    const double m_y;
    const double m_z;
};

class ReadCommandFactory
{
public:
    static ReadCommand* create(
            const v8::Arguments& args,
            std::shared_ptr<PdalSession> pdalSession);
};

