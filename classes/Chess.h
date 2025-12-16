#pragma once

#include "Game.h"
#include "Grid.h"
#include "Bitboard.h"

#include <list>

constexpr int pieceSize = 80;
constexpr int negInfinity = -1000000;
constexpr int posInfinity = +1000000;

constexpr uint64_t BitZero = 1ULL;

enum BitBoards
{
    WHITE_PAWNS,
    WHITE_KNIGHTS,
    WHITE_BISHOPS,
    WHITE_ROOKS,
    WHITE_QUEENS,
    WHITE_KING,
    WHITE_ALL,
    BLACK_PAWNS,
    BLACK_KNIGHTS,
    BLACK_BISHOPS,
    BLACK_ROOKS,
    BLACK_QUEENS,
    BLACK_KING,
    BLACK_ALL,
    OCCUPANCY,
    EMPTY_SQUARES,
    e_numBitboards
};

// MAGIC!!

struct MagicEntry {
    BitBoard mask;
    uint64_t magic;
    uint8_t indexBits;
};

extern MagicEntry ROOK_MAGICS[64];
extern MagicEntry BISHOP_MAGICS[64];

extern BitBoard* ROOK_MOVES[64];
extern BitBoard* BISHOP_MOVES[64];

class Chess : public Game
{
public:
    Chess();
    ~Chess();

    void setUpBoard() override;

    bool canBitMoveFrom(Bit &bit, BitHolder &src) override;
    bool canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst) override;
    void bitMovedFromTo(Bit &bit, BitHolder &src, BitHolder &dst) override;
    bool actionForEmptyHolder(BitHolder &holder) override;

    void stopGame() override;

    Player *checkForWinner() override;
    bool checkForDraw() override;

    std::string initialStateString() override;
    std::string stateString() override;
    void setStateString(const std::string &s) override;

    Grid* getGrid() override { return _grid; }

    // AI Methods

    void    updateAI();
    int     negamax(const std::string &state, int depth, int alpha, int beta, int playerNumber);
    int     evaluateAIBoard(const std::string &state);
    bool    isAIWinner();
    bool    isStalemate(const std::string &state);

    // BitBoard Methods

    void GenerateBitBoards();

    inline int bitScanForward(uint64_t bb)
    {
    #if defined(_MSC_VER) && !defined(__clang__)
        unsigned long index;
        _BitScanForward64(&index, bb);
        return index;
    #else
        return __builtin_ffsll(bb) - 1;
    #endif
    }

    inline size_t MagicIndex(const MagicEntry& entry, BitBoard blockers)
    {
        BitBoard mask = BitBoard(blockers.getData() & entry.mask.getData());
        uint64_t hash = mask.getData() * entry.magic;
        return hash >> (64 - entry.indexBits);
    }

    BitBoard GeneratePawnMoveBoard(int square);
    BitBoard GenerateKnightMoveBoard(int square);
    BitBoard GenerateBishopMoveBoard(int square);
    BitBoard GenerateRookMoveBoard(int square);
    BitBoard GenerateQueenMoveBoard(int square);
    BitBoard GenerateKingMoveBoard(int square);

    BitBoard RelevantOccupancy(BitBoard mask, int index, int bits);
    BitBoard GenerateRookAttacks(int square, BitBoard blockers);
    BitBoard GenerateBishopAttacks(int square, BitBoard blockers);

    // Move Methods

    static void PrecomputeMoveData();

    std::vector<BitMove> GenerateAllMoves();
    //std::vector<BitMove> GenerateMoves();

    void GeneratePawnMoveList(std::vector<BitMove>& moves, BitBoard pawnBoard, uint64_t friendlyPieces, uint64_t enemyPieces, int pieceColor);
    void AddPawnMoves(std::vector<BitMove>& moves, BitBoard pawnBoard, int shift);

    void GenerateKnightMoves(std::vector<BitMove>& moves, BitBoard knightBoard, uint64_t friendlyPieces);
    void GenerateBishopMoves(std::vector<BitMove>& moves, BitBoard bishopBoard, uint64_t friendlyPieces);
    void GenerateRookMoves(std::vector<BitMove>& moves, BitBoard rookBoard, uint64_t friendlyPieces);
    void GenerateQueenMoves(std::vector<BitMove>& moves, BitBoard queenBoard, uint64_t friendlyPieces);
    void GenerateKingMoves(std::vector<BitMove>& moves, BitBoard kingBoard, uint64_t friendlyPieces);

    void GenerateSlidingMoves(std::vector<BitMove>& moves, int from, int piece);

private:
    Bit* PieceForPlayer(const int playerNumber, ChessPiece piece);
    Player* ownerAt(int x, int y) const;
    void FENtoBoard(const std::string& fen);
    char pieceNotation(int x, int y) const;

    Grid* _grid;

    // BitBoards

    BitBoard _bitboards[e_numBitboards];

    BitBoard _pawnBitBoards[64];
    BitBoard _knightBitBoards[64];
    BitBoard _bishopBitBoards[64];
    BitBoard _rookBitBoards[64];
    BitBoard _queenBitBoards[64];
    BitBoard _kingBitBoards[64];

    // Move Data

    static int DirectionOffsets[8];
    static int NDirectionOffsets[8];
    static int NumSquaresToEdge[64][8];

    std::vector<BitMove> _moves;

};