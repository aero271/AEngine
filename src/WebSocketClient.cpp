
#ifdef GAF_ABT_WS
#include "WebSocketClient.hpp"
#include "Item.hpp"
#include <iostream>
#include <boost/format.hpp>

#include <nlohmann/json.hpp>

using boost::format;
using json = nlohmann::json;

using std::cout;
using std::cerr;


WebSocketClient::WebSocketClient(asio::io_context& ioc_, const std::string& host_, const std::string port) 
    :resolver(ioc_), ws(ioc_), host(host_), ioc(ioc_) {
    
    cout << "INFO: Initializing Websocket\n";
    try {
        auto results = resolver.resolve(host, port);
        auto endpoint = asio::connect(ws.next_layer(), results);
        ws.handshake(host, "/");
    } catch (std::exception e) {
        cerr << format("ws error: %1%\n") % e.what();
    }
    startReading();
}


void WebSocketClient::startReading()
{
    ws.async_read(buffer, 
       [this](beast::error_code e, std::size_t bytes_transferred) { 
            if (e)
            {
                cerr << format("Read error: %1%\n") % e.message();
                return;
            }
            std::string rawData = beast::buffers_to_string(buffer.data());
            buffer.consume(bytes_transferred);
            json data = json::parse(rawData);

            // cout << data << "\n";

            if (!data.is_array())
            {
                cerr << "data from server isn't an array\n";
                return;
            }

            //loops over incoming objects and adds to game world
            for (auto& object : data)
            {
                //checks if object already exists
                const int id = std::stoi(object["id"].get<std::string>());
                if (Item::items.find(id) != Item::items.end()) continue; 

                // initalizes object (Item constructor automatically adds it to the world)
                // IMPORTANT: ensure to loop over Item::items during game or level unload to delete dynamically allocated objects
                int chunkX = std::stoi(object["chunkX"].get<std::string>());
                int chunkZ = std::stoi(object["chunkZ"].get<std::string>());
                float internalX = std::stof(object["internalX"].get<std::string>());
                float internalY = std::stof(object["internalY"].get<std::string>());
                float internalZ = std::stof(object["internalZ"].get<std::string>());

                Item* item = new Item(object["name"].get<std::string>(), 
                    (Vector3){ 
                        (chunkX * 16 + internalX), 
                        internalY,
                        (chunkZ * 16 + internalZ)
                    },
                    1.0f, /*TODO: fix scale*/
                    id
                );
                item->isDynamic = true;

            }

        startReading(); //recusion to always be reading
       });
}

void WebSocketClient::sendMessage(const std::string& message)
{
    ws.async_write(asio::buffer(message), [this](beast::error_code e, std::size_t size) {
        if (e)
        {
            cerr << format("Writing Error: %1%\n") % e.message(); 
        }
    });
}

WebSocketClient::~WebSocketClient()
{
    ws.close(ws::close_reason("Game Closed"));
}

void updateBackend(WebSocketClient& ws, ChunkData& playerChunk)
{
    json data = {
        {"chunkX", playerChunk.chunkX},
        {"chunkZ", playerChunk.chunkZ},
        {"internalX", playerChunk.internalX},
        {"internalZ", playerChunk.internalZ},
    };
    
    ws.sendMessage(data.dump());
}
#endif