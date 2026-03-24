
#include "simulator.h"
#include "card.h"
#include "evaluator.h"
#include "third_party/httplib.h"
#include <nlohmann/json.hpp>
#include <vector>
#include <string>
#include <stdexcept>
#include <cstdlib>

using json = nlohmann::json;

int main() {
    load_handranks();

    httplib::Server app;
    app.set_default_headers({
        {"Access-Control-Allow-Origin", "*"},
        {"Access-Control-Allow-Methods", "GET, POST, OPTIONS"},
        {"Access-Control-Allow-Headers", "Content-Type"}
    });

    app.Options(R"(.*)", [](const httplib::Request&, httplib::Response& res) {
        res.status = 204;
    });

    app.Get("/api/health", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("{\"ok\":true}", "application/json");
    });

    app.Post("/api/equity", [](const httplib::Request& req, httplib::Response& res) {
        try {
            auto body = json::parse(req.body);
            std::vector<std::string> heroStr = body.at("heroCards").get<std::vector<std::string>>();
            std::vector<std::string> boardStr = body.value("boardCards", std::vector<std::string>{});
            std::vector<std::string> opponentRange = body.value("opponentRange", std::vector<std::string>{});
            int numPlayers = body.value("numPlayers", 1);
            uint64_t seed = body.value("seed", 0ULL);

            if (heroStr.size() != 2) throw std::runtime_error("heroCards must have 2 cards");

            std::pair<int, int> heroCards = {parseCard(heroStr[0]), parseCard(heroStr[1])};
            std::vector<int> boardCards;
            boardCards.reserve(boardStr.size());
            for (const auto& c : boardStr) boardCards.push_back(parseCard(c));

            double equity = 0.0;
            if (boardCards.size() == 0) {
                equity = PreFlopSimulation(numPlayers, heroCards, opponentRange, seed);
            } else if (boardCards.size() == 3) {
                equity = FlopSimulation(numPlayers, heroCards, boardCards, opponentRange, seed);
            } else if (boardCards.size() == 4) {
                equity = TurnSimulation(numPlayers, heroCards, boardCards, opponentRange, seed);
            } else if (boardCards.size() == 5) {
                equity = RiverSimulation(numPlayers, heroCards, boardCards, opponentRange, seed);
            } else {
                throw std::runtime_error("boardCards must have 0, 3, 4, or 5 cards");
            }

            json out = {{"equity", equity}};
            res.set_content(out.dump(), "application/json");
        } catch (const std::exception& e) {
            json err = {{"error", e.what()}};
            res.status = 400;
            res.set_content(err.dump(), "application/json");
        }
    });

    int port = 8080;
    if (const char* portEnv = std::getenv("PORT")) {
        try {
            port = std::stoi(portEnv);
        } catch (...) {
            port = 8080;
        }
    }

    app.listen("0.0.0.0", port);
    return 0;
}
