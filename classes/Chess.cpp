#include "Chess.h"
#include <limits>
#include <cmath>
#include <list>

#include "Bitboard.h"

Chess::Chess()
{
    _grid = new Grid(8, 8);

    PrecomputeMoveData();
    GenerateBitBoards();
}

Chess::~Chess()
{
    delete _grid;
}

int Chess::DirectionOffsets[8] = { 8, -8, 1, -1, 7, -7, 9, -9 };
int Chess::NDirectionOffsets[8] = { 6, 15, 17, 10, -6, -15, -17, -10 };
int Chess::NumSquaresToEdge[64][8] = {};

MagicEntry ROOK_MAGICS[64];
MagicEntry BISHOP_MAGICS[64];

BitBoard* ROOK_MOVES[64];
BitBoard* BISHOP_MOVES[64];

// Bitboards

void Chess::GenerateBitBoards()
{
    // For every square . . .
    for (int i = 0; i < 64; i++)
    { 
        // Generate and store every valid knight move in a bitboard.
        //_pawnBitBoards[i] = GeneratePawnMoveBoard(i);

        // Generate and store every valid knight move in a bitboard.
        _pawnBitBoards[i] = GeneratePawnMoveBoard(i);
        _knightBitBoards[i] = GenerateKnightMoveBoard(i);
        _bishopBitBoards[i] = GenerateBishopMoveBoard(i);
        _rookBitBoards[i] = GenerateRookMoveBoard(i);
        _queenBitBoards[i] = GenerateQueenMoveBoard(i);
        _kingBitBoards[i] = GenerateKingMoveBoard(i);
        
    }
    
}

BitBoard Chess::GeneratePawnMoveBoard(int square)
{
    BitBoard bitBoard = 0ULL;
    return bitBoard;
}

BitBoard Chess::GenerateKnightMoveBoard(int square)
{

    BitBoard bitBoard = 0ULL;
    int rank = square / 8;
    int file = square % 8;

    // Move Offset Table

    std::pair<int, int> knightOffsets[] = {
        { 2, 1 }, { 2, -1 }, { -2, 1 }, { -2, -1 },
        { 1, 2 }, { 1, -2 }, { -1, 2 }, { -1, -2 }
    };

    constexpr uint64_t oneBit = 1;

    // For every possible knight move (of 8) . . .
    for (auto [dr, df] : knightOffsets)
    {
        int r = rank + dr, f = file + df; // Target square's rank and file

        // If move is VALID ( regardless of legality* ) . . .
        if (r >= 0 && r < 8 && f >= 0 && f < 8)
        {
            bitBoard |= oneBit << (r * 8 + f); // Set bitBoard
        }
    }

    return bitBoard;
}

BitBoard Chess::GenerateBishopMoveBoard(int square)
{

    BitBoard mask = 0ULL;

    // Iterate through all four directions
    for (int dir = 4; dir < 8; dir++)
    {
        // Iterate through every square in direction
        for (int n = 1; n < NumSquaresToEdge[square][dir]; n++)
        {
            int to = square + DirectionOffsets[dir] * n;
            mask |= (1ULL << to);
        }
    }
    return mask;
}

BitBoard Chess::GenerateRookMoveBoard(int square)
{

    BitBoard mask = 0ULL;

    // Iterate through all four directions
    for (int dir = 0; dir < 4; dir++)
    {
        // Iterate through every square in direction
        for (int n = 1; n < NumSquaresToEdge[square][dir]; n++)
        {
            int to = square + DirectionOffsets[dir] * n;
            mask |= (1ULL << to);
        }
    }
    return mask;
}

BitBoard Chess::GenerateQueenMoveBoard(int square)
{

    BitBoard bitBoard = 0ULL;
    return bitBoard;
}

BitBoard Chess::GenerateKingMoveBoard(int square)
{
    BitBoard bitBoard = 0ULL;
    int rank = square / 8;
    int file = square % 8;

    // Move Offset Table

    std::pair<int, int> kingOffsets[] = {
        { 1, 0 }, { 1, 1 }, { 0, 1 }, { -1, 1 },
        { -1, 0 }, { -1, -1 }, { 0, -1 }, { 1, -1 }
    };

    constexpr uint64_t oneBit = 1;

    // For every possible knight move (of 8) . . .
    for (auto [dr, df] : kingOffsets)
    {
        int r = rank + dr, f = file + df; // Target square's rank and file

        // If move is VALID ( regardless of legality* ) . . .
        if (r >= 0 && r < 8 && f >= 0 && f < 8)
        {
            bitBoard |= oneBit << (r * 8 + f); // Set bitBoard
        }
    }

    return bitBoard;
}

BitBoard Chess::RelevantOccupancy(BitBoard mask, int index, int bits)
{
    BitBoard occupancy = 0ULL;
    int bitIndex = 0;

    for (int square = 0; square < 64; square++)
    {
        if (mask.getData() & (1ULL << square))
        {
            if (index & (1 << bitIndex))
            {
                occupancy |= (1ULL << square);
            }

            bitIndex++;
        }
    }

    return occupancy;
}

BitBoard Chess::GenerateRookAttacks(int square, BitBoard blockers)
{
    BitBoard attacks = 0ULL;

    // Iterate through all four directions
    for (int dir = 0; dir < 4; dir++)
    {
        // Iterate through every square in direction
        for (int n = 1; n < NumSquaresToEdge[square][dir]; n++)
        {
            int to = square + DirectionOffsets[dir] * n;
            attacks |= (1ULL << to);
            
            if (blockers.getData() & (1ULL << to))
            {
                break;
            }
        }
    }
    return attacks;
}

BitBoard Chess::GenerateBishopAttacks(int square, BitBoard blockers)
{
    BitBoard attacks = 0ULL;

    // Iterate through all four directions
    for (int dir = 4; dir < 8; dir++)
    {
        // Iterate through every square in direction
        for (int n = 1; n < NumSquaresToEdge[square][dir]; n++)
        {
            int to = square + DirectionOffsets[dir] * n;
            attacks |= (1ULL << to);
            
            if (blockers.getData() & (1ULL << to))
            {
                break;
            }
        }
    }
    return attacks;
}

// Moves

void Chess::PrecomputeMoveData()
{
    std::cout << "Precomputing moves!" << std::endl;
    std::cout << std::endl;

    for (int file = 0; file < 8; file++)
    {
        for (int rank = 0; rank < 8; rank++)
        {
            int north = 7 - rank;
            int south = rank;
            int east = 7 - file;
            int west = file;

            int index = rank * 8 + file;

            NumSquaresToEdge[index][0] = north; // Up
            NumSquaresToEdge[index][1] = south; // Down
            NumSquaresToEdge[index][2] = east; // Right
            NumSquaresToEdge[index][3] = west; // Left
            NumSquaresToEdge[index][4] = std::min(north, west); // Up-Left
            NumSquaresToEdge[index][5] = std::min(south, east); // Down-Right
            NumSquaresToEdge[index][6] = std::min(north, east); // Up-Right
            NumSquaresToEdge[index][7] = std::min(south, west); // Down-Left
        }
    }
}

// Iterate through EVERY bit board to generate EVERY valid / legal move.
std::vector<BitMove> Chess::GenerateAllMoves()
{
    std::cout << "2" << std::endl;
    std::vector<BitMove> moves;
    moves.reserve(32);
    std::string state = stateString();
    float foo = 0.0f;
    
    // Current piece color

    bool whiteToMove = (getCurrentPlayer()->playerNumber() == 0);

    // Define variables for pieces (broadly)

    uint64_t friendlyPieces;
    uint64_t enemyPieces;
    uint64_t emptySquares;

    uint64_t pawns;
    uint64_t knights;
    uint64_t bishops;
    uint64_t rooks;
    uint64_t queen;
    uint64_t king;

    uint64_t whitePieces = 0LL;
    uint64_t whitePawns = 0LL;
    uint64_t whiteKnights = 0LL;
    uint64_t whiteBishops = 0LL;
    uint64_t whiteRooks = 0LL;
    uint64_t whiteQueen = 0LL;
    uint64_t whiteKing = 0LL;

    uint64_t blackPieces = 0LL;
    uint64_t blackPawns = 0LL;
    uint64_t blackKnights = 0LL;
    uint64_t blackBishops = 0LL;
    uint64_t blackRooks = 0LL;
    uint64_t blackQueen = 0LL;
    uint64_t blackKing = 0LL;


    for (int i = 0; i < e_numBitboards; i++)
    {
        _bitboards[i] = 0;
    }

    for (int i = 0; i < 64; i++)
    {
        if (state[i] == 'P')
        {
            whitePawns |= BitZero << i;
        } else if (state[i] == 'N') {
            whiteKnights |= BitZero << i;
        } else if (state[i] == 'B') {
            whiteBishops |= BitZero << i;
        } else if (state[i] == 'R') {
            whiteRooks |= BitZero << i;
        } else if (state[i] == 'Q') {
            whiteQueen |= BitZero << i;
        } else if (state[i] == 'K') {
            whiteKing |= BitZero << i;
        }

        if (state[i] == 'p')
        {
            blackPawns |= BitZero << i;
        } else if (state[i] == 'n') {
            blackKnights |= BitZero << i;
        } else if (state[i] == 'b') {
            blackBishops |= BitZero << i;
        } else if (state[i] == 'r') {
            blackRooks |= BitZero << i;
        } else if (state[i] == 'q') {
            blackQueen |= BitZero << i;
        } else if (state[i] == 'k') {
            blackKing |= BitZero << i;
        }
    }

    whitePieces = whitePawns | whiteKnights | whiteBishops 
                | whiteRooks | whiteQueen | whiteKing;

    blackPieces = blackPawns | blackKnights | blackBishops
                | blackRooks | blackQueen | blackKing;

 
    friendlyPieces = whiteToMove ? whitePieces : blackPieces;
    enemyPieces = whiteToMove ? blackPieces : whitePieces;
    emptySquares = ~(friendlyPieces | enemyPieces);

    pawns = whiteToMove ? whitePawns : blackPawns;
    knights = whiteToMove ? whiteKnights : blackKnights;
    bishops = whiteToMove ? whiteBishops : blackBishops;
    rooks = whiteToMove ? whiteRooks : blackRooks;
    queen = whiteToMove ? whiteQueen : blackQueen;
    king = whiteToMove ? whiteKing : blackKing;

    // Generate moves
    GeneratePawnMoveList(moves, pawns, emptySquares, enemyPieces, getCurrentPlayer()->playerNumber());
    GenerateKnightMoves(moves, knights, ~friendlyPieces);
    GenerateKingMoves(moves, king, ~friendlyPieces);

    // Sliding Pieces

    uint64_t sliders = bishops | rooks | queen;

    while (sliders)
    {
        int from = __builtin_ctzll(sliders);
        sliders &= sliders - 1;

        Bit* bit = _grid->getSquareByIndex(from)->bit();
        GenerateSlidingMoves(moves, from, bit->gameTag());
    }

    return moves;
}

void Chess::GenerateSlidingMoves(std::vector<BitMove>& moves, int from, int piece)
{

    int startDir = (piece == Bishop) ? 4 : 0;
    int endDir = (piece == Rook) ? 4 : 8;

    // Loop through all eight directions

    for (int dir = startDir; dir < endDir; dir++)
    {
        // Loop through every square in given direction

        for (int n = 0; n < NumSquaresToEdge[from][dir]; n++)
        {
            int to = from + DirectionOffsets[dir] * (n + 1);;
            Bit *toPiece = _grid->getSquareByIndex(to)->bit();

            if (toPiece && 
                toPiece->getOwner()->playerNumber() == 
                getCurrentPlayer()->playerNumber())
            {
                break;
            }

            moves.emplace_back(from, to, piece);

            if (toPiece &&
                toPiece->getOwner()->playerNumber() !=
                getCurrentPlayer()->playerNumber())
            {
                break;
            }

        }
    }
}

void Chess::GeneratePawnMoveList(std::vector<BitMove>& moves, BitBoard pawnBoard, uint64_t validSquares, uint64_t enemyPieces, int pieceColor)
{
    if (pawnBoard.getData() == 0) { return; }

    // Defined at compile-time
    constexpr uint64_t NotAFile(0xFEFEFEFEFEFEFEFEULL);
    constexpr uint64_t NotHFile(0x7F7F7F7F7F7F7F7FULL);
    constexpr uint64_t Rank3(0x0000000000FF0000ULL);
    constexpr uint64_t Rank6(0x0000FF0000000000ULL);

    // Calculate single moves
    BitBoard singleMoves = (pieceColor == 0) ? 
                            // If white, shift bits
                           (pawnBoard.getData() << 8) & 
                            // Remove invalid moves
                            validSquares : 
                            // If black, shift bits
                           (pawnBoard.getData() >> 8) & 
                            // Remove invalid moves
                            validSquares;

    BitBoard doubleMoves = (pieceColor == 0) ? 
                           ((singleMoves.getData() & Rank3) << 8) & 
                             validSquares : 
                           ((singleMoves.getData() & Rank6) >> 8) & 
                             validSquares;

    // Calculate capture moves
    BitBoard capturesLeft = (pieceColor == 0) ? 
                             // If white, shift bits
                            ((pawnBoard.getData() & NotAFile) << 7) & 
                              enemyPieces :
                            ((pawnBoard.getData() & NotAFile) >> 9) &
                              enemyPieces;

    BitBoard capturesRight = (pieceColor == 0) ? 
                             ((pawnBoard.getData() & NotHFile) << 9) &
                               enemyPieces :
                             ((pawnBoard.getData() & NotHFile) >> 7) &
                               enemyPieces;

    // Calculate pawn move offsets
    int shift = (pieceColor == 0) ? 8 : -8;
    int doubleShift = (pieceColor == 0) ? 16 : -16;
    int captureLeftShift = (pieceColor == 0) ? 7 : -9;
    int captureRightShift = (pieceColor == 0) ? 9 : -7;

    AddPawnMoves(moves, singleMoves, shift);
    AddPawnMoves(moves, doubleMoves, doubleShift);
    AddPawnMoves(moves, capturesLeft, captureLeftShift);
    AddPawnMoves(moves, capturesRight, captureRightShift);

}

void Chess::AddPawnMoves(std::vector<BitMove>& moves, BitBoard pawnBoard, int shift)
{

    if (pawnBoard.getData() == 0) { return; }

    pawnBoard.forEachBit([&](int toSquare) {
        int fromSquare = toSquare - shift;
        moves.emplace_back(fromSquare, toSquare, Pawn);
    });
    /*int startDir = 0;
    int endDir = 8;

    Bit *fromPiece = _grid->getSquareByIndex(from)->bit();

    // Loop through all eight directions

    for (int dir = startDir; dir < endDir; dir++)
    {

        // Skip directions a pawn can't move in (white or black)

        if (fromPiece && fromPiece->getOwner()->playerNumber() == 0 && DirectionOffsets[dir] < 7)
        {
            continue;
        }

        if (fromPiece && fromPiece->getOwner()->playerNumber() == 1 && DirectionOffsets[dir] > -7)
        {
            continue;
        }

        // Loop through every square in given direction

        for (int n = 0; n < NumSquaresToEdge[from][dir]; n++)
        {

            if (fromPiece && 
                // If dir is vertical . . . 
                DirectionOffsets[dir] % 2 == 0 )
            {
                // If pawn is on starting square . . .
                if (NumSquaresToEdge[from][dir] >= 6)
                {
                    if (n > 1)
                    {
                        break;
                    }
                }
                else if (n > 0)
                {
                    break;
                }
                
            }

            int to = from + DirectionOffsets[dir] * (n + 1);

            Bit *toPiece = _grid->getSquareByIndex(to)->bit();

            if ( 
                // If dir is diagonal . . . 
                abs(DirectionOffsets[dir]) % 2 == 1 )
            {
                // If enemy piece on target square . . .
                if (toPiece && toPiece->getOwner()->playerNumber() != 
                    getCurrentPlayer()->playerNumber())
                {
                    if (n > 0)
                    {
                        break;
                    }
                }
                else
                {
                    break;
                }
            }

            _moves.emplace_back(from, to, piece);
        }
    }*/
}

void Chess::GenerateKnightMoves(std::vector<BitMove>& moves, BitBoard knightBoard, uint64_t emptySquares)
{
    // While there are still moves . . .
    knightBoard.forEachBit([&](int fromSquare) {
        BitBoard moveBoard = BitBoard(_knightBitBoards[fromSquare].getData() & emptySquares);

        moveBoard.printBitBoard();
        moveBoard.forEachBit([&](int toSquare) {
            moves.emplace_back(fromSquare, toSquare, Knight);
        });
    });
}

void Chess::GenerateBishopMoves(std::vector<BitMove>& moves, BitBoard bishopBoard, uint64_t emptySquares)
{
    // While there are still moves . . .
    bishopBoard.forEachBit([&](int fromSquare) {
        BitBoard moveBoard = BitBoard(_bishopBitBoards[fromSquare].getData() & emptySquares);

        moveBoard.printBitBoard();
        moveBoard.forEachBit([&](int toSquare) {
            moves.emplace_back(fromSquare, toSquare, Bishop);
        });
    });
}

void Chess::GenerateRookMoves(std::vector<BitMove>& moves, BitBoard rookBoard, uint64_t emptySquares)
{
    // While there are still moves . . .
    rookBoard.forEachBit([&](int fromSquare) {
        BitBoard moveBoard = BitBoard(_rookBitBoards[fromSquare].getData() & emptySquares);

        moveBoard.printBitBoard();
        moveBoard.forEachBit([&](int toSquare) {
            moves.emplace_back(fromSquare, toSquare, Rook);
        });
    });
}

void Chess::GenerateQueenMoves(std::vector<BitMove>& moves, BitBoard queenBoard, uint64_t emptySquares)
{
    // While there are still moves . . .
    queenBoard.forEachBit([&](int fromSquare) {
        BitBoard moveBoard = BitBoard(_queenBitBoards[fromSquare].getData() & emptySquares);

        moveBoard.printBitBoard();
        moveBoard.forEachBit([&](int toSquare) {
            moves.emplace_back(fromSquare, toSquare, Queen);
        });
    });
}

void Chess::GenerateKingMoves(std::vector<BitMove>& moves, BitBoard kingBoard, uint64_t emptySquares)
{
    // While there are still moves . . .
    kingBoard.forEachBit([&](int fromSquare) {
        BitBoard moveBoard = BitBoard(_kingBitBoards[fromSquare].getData() & emptySquares);

        moveBoard.forEachBit([&](int toSquare) {
            moves.emplace_back(fromSquare, toSquare, King);
        });
    });
}

// Board

char Chess::pieceNotation(int x, int y) const
{
    const char *wpieces = { "0PNBRQK" };
    const char *bpieces = { "0pnbrqk" };

    ChessSquare* square = _grid->getSquare(x, y);
    Bit *bit = square->bit();

    char notation = '0';
    
    if (bit != nullptr) {
        notation = bit->getOwner()->playerNumber() == 0 ? wpieces[bit->gameTag()] : bpieces[bit->gameTag()];
    }

    //std::cout << "Square: " << notation << std::endl;

    return notation;
}

Bit* Chess::PieceForPlayer(const int playerNumber, ChessPiece piece)
{
    const char* pieces[] = { "pawn.png", "knight.png", "bishop.png", "rook.png", "queen.png", "king.png" };

    Bit* bit = new Bit();
    // should possibly be cached from player class?
    const char* pieceName = pieces[piece - 1];
    std::string spritePath = std::string("") + (playerNumber == 0 ? "w_" : "b_") + pieceName;
    bit->LoadTextureFromFile(spritePath.c_str());
    bit->setOwner(getPlayerAt(playerNumber));
    bit->setSize(pieceSize, pieceSize);
    bit->setGameTag(piece);

    return bit;
}

void Chess::setUpBoard()
{
    setNumberOfPlayers(2);
    _gameOptions.rowX = 8;
    _gameOptions.rowY = 8;

    _grid->initializeChessSquares(pieceSize, "boardsquare.png");
    FENtoBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");

    startGame();
    _moves = GenerateAllMoves();
}

void Chess::FENtoBoard(const std::string& fen) {
    // convert a FEN string to a board

    // FEN is a space delimited string with 6 fields

    std::stringstream s(fen);
    std::string row;

    char rowDelimiter = '/';

    int rank = _grid->getHeight() - 1; // Chess Rank (row)

    while (getline(s, row, rowDelimiter))
    {
        int file = 0; // Chess Files (column)

        for (char c : row)
        {
            ChessSquare* square = _grid->getSquare(file, rank);
            ChessPiece piece;

            // I needed help with this one. I found the isdigit function and
            // then just needed to increase file by the number of squares,
            // and continue the loop.

            if (std::isdigit(c)) // Skips c squares if empty
            {
                file += c - '0';
                continue;
            }

            // Realized switch statements were a cleaner solution during
            // my C++ syntax research while debugging the out-of-range error.

            switch (std::tolower(c)) // Equalizes all piece codes to assign values
            {
                case 'r': piece = Rook; break;
                case 'n': piece = Knight; break;
                case 'b': piece = Bishop; break;
                case 'k': piece = King; break;
                case 'q': piece = Queen; break;
                case 'p': piece = Pawn; break;
                default: continue;
            }

            // I had found I was getting an out-of-range error when I tried
            // passing the playerNumber, and realized I was passing -1, per
            // AIPlayer. I simply hard-coded 1 and 0 for the below function. 

            int pieceOwner = std::isupper(c) ? 0 : 1; // Detects if piece is white / black

            Bit *bit = PieceForPlayer(pieceOwner, piece);

            if (bit != nullptr)
            {
                bit->setPosition(square->getPosition());
                square->setBit(bit);
            }

            //std::cout << "Player: " << bit->getOwner()->playerNumber() << std::endl;
            //std::cout << "Square: " << pieceNotation(file, rank) << " - (" << file << ", " << rank << ")" << std::endl;

            file++;
        }

        rank--;
    }

    // 2: active color (W or B)
    // 3: castling availability (KQkq or -)
    // 4: en passant target square (in algebraic notation, or -)
    // 5: halfmove clock (number of halfmoves since the last capture or pawn advance)
}

bool Chess::actionForEmptyHolder(BitHolder &holder)
{
    return false;
}

bool Chess::canBitMoveFrom(Bit &bit, BitHolder &src)
{
    // need to implement friendly/unfriendly in bit so for now this hack
    int currentPlayer = getCurrentPlayer()->playerNumber();
    int pieceColor = bit.getOwner()->playerNumber();

    if (pieceColor != currentPlayer) return false;

    bool ret = false;
    ChessSquare* square = (ChessSquare *)&src;

    if (square)
    {
        int index = square->getSquareIndex();
        for (auto move : _moves)
        {
            if (move.from == index)
            {
                ret = true;
                auto dest = _grid->getSquareByIndex(move.to);
                dest->setHighlighted(true);
            }
        }
    }

    return ret;
}

bool Chess::canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst)
{
    int currentPlayer = getCurrentPlayer()->playerNumber();
    int pieceColor = bit.getOwner()->playerNumber();

    if (pieceColor != currentPlayer) return false;

    bool ret = false;
    ChessSquare* fromSquare = static_cast<ChessSquare*>(&src);
    ChessSquare* toSquare = static_cast<ChessSquare*>(&dst);

    if (fromSquare && toSquare)
    {
        int fromIndex = fromSquare->getSquareIndex();
        int toIndex = toSquare->getSquareIndex();

        for (auto move : _moves)
        {
            if (move.from == fromIndex && move.to == toIndex)
            {
                ret = true;
                auto dest = _grid->getSquareByIndex(move.to);
                dest->setHighlighted(true);
            }
        }
    }

    return ret;
}

void Chess::bitMovedFromTo(Bit &bit, BitHolder &src, BitHolder &dst) {
    ChessSquare* srcSquare = static_cast<ChessSquare*>(&src);
    ChessSquare* dstSquare = static_cast<ChessSquare*>(&dst);

    int srcX = srcSquare->getColumn();
    int srcY = srcSquare->getRow();
    int dstX = dstSquare->getColumn();
    int dstY = dstSquare->getRow();

    Bit* toPiece = dstSquare->bit();

    if (toPiece &&
        toPiece->getOwner()->playerNumber() != bit.getOwner()->playerNumber())
    {  
        dstSquare->destroyBit();
    }

    _grid->forEachSquare([](ChessSquare* square, int x, int y) {
        square->setHighlighted(false);
    });

    endTurn();
    _moves = GenerateAllMoves();
}

void Chess::stopGame()
{
    _grid->forEachSquare([](ChessSquare* square, int x, int y) {
        square->destroyBit();
    });
}

Player* Chess::ownerAt(int x, int y) const
{
    if (x < 0 || x >= 8 || y < 0 || y >= 8) {
        return nullptr;
    }

    auto square = _grid->getSquare(x, y);
    if (!square || !square->bit()) {
        return nullptr;
    }
    return square->bit()->getOwner();
}

Player* Chess::checkForWinner()
{
    return nullptr;
}

bool Chess::checkForDraw()
{
    return false;
}

std::string Chess::initialStateString()
{
    return stateString();
}

std::string Chess::stateString()
{
    std::string s;
    s.reserve(64);
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) 
        {
            //int newy = 7 - y;
            s += pieceNotation( x, y );
        }
    );

    return s;}

void Chess::setStateString(const std::string &s)
{
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        int index = y * 8 + x;
        char playerNumber = s[index] - '0';
        if (playerNumber) {
            square->setBit(PieceForPlayer(playerNumber - 1, Pawn));
        } else {
            square->setBit(nullptr);
        }
    });
}

// AI Methods

void Chess::updateAI()
{
    int alpha = negInfinity;
    int beta = posInfinity;

    int score = negInfinity;
    BitMove* bestMove = nullptr;

    std::string state = stateString();

    for (auto move : _moves)
    {
        int srcSquare = move.from;
        int dstSquare = move.to;

        char oldDst = state[dstSquare];
        char srcPce = state[srcSquare];

        // Make temporary move

        state[dstSquare] = srcPce;
        state[srcSquare] = '0';

        int moveVal = -negamax(state, 0, alpha, beta, HUMAN_PLAYER);

        if (moveVal >= score)
        {
            score = moveVal;
            bestMove = &move;
        }

        // Undo Move

        state[srcSquare] = srcPce;
        state[dstSquare] = oldDst;
    }

    return;
}

int Chess::negamax(const std::string &state, int depth, int alpha, int beta, int playerNumber)
{
    int bestScore = evaluateAIBoard(state);

    // Check for mate
    if (isAIWinner() || depth == 2 || abs(bestScore) >= 10000)
    {
        return bestScore * playerNumber;
    }

    // Get legal moves for correctly colored pieces 

    int bestEval = -99999;
    for (int x = 0; x < _moves.size(); x++)
    {

    }

    return alpha;
}

int Chess::evaluateAIBoard(const std::string &state)
{
    return 0;
}

bool Chess::isAIWinner()
{
    return false;
}
