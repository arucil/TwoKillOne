#include <emscripten.h>
#include <vector>
#include <algorithm>
#include <cmath>
#include <limits>
#include "GameAI.h"

using namespace std;


struct GameAI::Node {
    Node(const Board &board, Player player, Node *parent) :
            board(board), player(player), parent(parent), wins(0), visits(0) {}

    ~Node() {
        for (Node *n : children) {
            delete n;
        }
    }

    double getUCT(int parentVisits) const {
        if (visits == 0) {
            return std::numeric_limits<double>::infinity();
        } else {
            return static_cast<double>(wins) / visits + sqrt(2 * log(parentVisits) / visits);
        }
    }

public:
    const Board board;
    const Player player;
    Node *const parent;
    int wins;
    int visits;
    vector<Node *> children;
};


void GameAI::think(const Board &board, Player player, int milliseconds) {
    auto root = new Node(board, nextPlayer(player), nullptr);
    root->visits = 1; // Avoid playing simulation on root node
    auto endTime = emscripten_get_now() + milliseconds;

    while (emscripten_get_now() < endTime) {
        // Selection
        Node *promisingNode = selectPromisingNode(root);

        Node *nodeToExplore = promisingNode;

        // Expansion
        // Only do expansion when the selected node is not a terminal node
        if (promisingNode->visits != 0 && !promisingNode->board.isWinning(promisingNode->player)) {
            expandNode(promisingNode);

            // Select a random child node to explore
            nodeToExplore = promisingNode->children[static_cast<size_t>(emscripten_random() * promisingNode->children.size())];
        }

        // Simulation
        Player winningPlayer = simulate(nodeToExplore);

        // Back propagation
        backPropagate(nodeToExplore, winningPlayer);
    }

    printf("simulations: %d\n", root->visits);

    struct {
        bool operator()(const Node *a, const Node *b) const {
            return a->visits < b->visits;
        }
    } comparator;

    auto bestNode = *max_element(root->children.begin(), root->children.end(), comparator);

    extractPositions(root->board, bestNode->board, bestNode->player);

    delete root;
}

struct UCTComparator {
    const int parentVisits;

    explicit UCTComparator(int parentVisits) : parentVisits(parentVisits) {}

    bool operator()(const GameAI::Node *a, const GameAI::Node *b) const {
        return a->getUCT(parentVisits) < b->getUCT(parentVisits);
    }
};

GameAI::Node *GameAI::selectPromisingNode(GameAI::Node *node) {
    while (!node->children.empty()) {
        node = *max_element(node->children.begin(), node->children.end(), UCTComparator(node->visits));
    }
    return node;
}

void GameAI::expandNode(GameAI::Node *node) {
    Player player1 = nextPlayer(node->player);

    for (int y = 0; y < Board::SIZE; ++y) {
        for (int x = 0; x < Board::SIZE; ++x) {
            if (player1 != node->board(x, y)) {
                continue;
            }

            for (auto &d : Board::Directions) {
                int x1 = x + d.x;
                int y1 = y + d.y;

                if (x1 >= 0 && x1 < Board::SIZE && y1 >= 0 && y1 < Board::SIZE && Player::None == node->board(x1, y1)) {
                    Board newBoard = node->board;
                    newBoard.move(Board::Position { x, y }, Board::Position { x1, y1 }, player1);
                    node->children.push_back(new Node(newBoard, player1, node));
                }
            }
        }
    }
}

Player GameAI::simulate(const GameAI::Node *node) {
    Board board = node->board;
    Player player = node->player;

    while (!board.isWinning(player)) {
        player = nextPlayer(player);
        randomMove(board, player);
    }

    return player;
}

void GameAI::randomMove(Board &board, Player player) {
    vector<int> moves;

    for (int y = 0; y < Board::SIZE; ++y) {
        for (int x = 0; x < Board::SIZE; ++x) {
            if (player != board(x, y)) {
                continue;
            }

            for (auto &d : Board::Directions) {
                int x1 = x + d.x;
                int y1 = y + d.y;

                if (x1 >= 0 && x1 < Board::SIZE && y1 >= 0 && y1 < Board::SIZE && Player::None == board(x1, y1)) {
                    moves.push_back(x | y << 2 | x1 << 4 | y1 << 6);
                }
            }
        }
    }

    int move = moves[static_cast<size_t>(emscripten_random() * moves.size())];
    board.move(Board::Position { move & 3, move >> 2 & 3 }, Board::Position { move >> 4 & 3, move >> 6 }, player);
}

void GameAI::backPropagate(GameAI::Node *node, Player winningPlayer) {
    while (node != nullptr) {
        if (node->player == winningPlayer) {
            ++node->wins;
        }
        ++node->visits;
        node = node->parent;
    }
}

void GameAI::extractPositions(const Board &orgBoard, const Board &curBoard, Player player) {
    for (int y = 0; y < Board::SIZE; ++y) {
        for (int x = 0; x < Board::SIZE; ++x) {
            if (player == orgBoard(x, y) && Player::None == curBoard(x, y)) {
                srcPosition.x = x;
                srcPosition.y = y;
            } else if (Player::None == orgBoard(x, y) && player == curBoard(x, y)) {
                destPosition.x = x;
                destPosition.y = y;
            }
        }
    }
}
