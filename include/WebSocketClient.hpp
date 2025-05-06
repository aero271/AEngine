#pragma once

#include <utility>
#include <memory>
#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <string>
#include <queue>

#include "definitions.hpp"

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace ws = beast::websocket;
using tcp = asio::ip::tcp;


class WebSocketClient
{
private:
    tcp::resolver resolver;
    ws::stream<tcp::socket> ws;
    beast::flat_buffer buffer;
    std::string host;
    asio::io_context& ioc;
    // std::queue<std::string> outgoingMessages;

    void startReading();
    void write();

public:

    WebSocketClient(asio::io_context& ioc_, const std::string& host_, const std::string port);
    ~WebSocketClient();
    void sendMessage(const std::string& message);


};

/// @brief sends player chunk data to backend, which it respondes with new objects to render, also in the future will send update server with data the player created
/// @param ws reference to websocket client
/// @param playerChunk reference to players chunk data object
void updateBackend(WebSocketClient& ws, ChunkData& playerChunk);