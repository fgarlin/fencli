/*
 * fencli: A tiny C99 command-line FEN-string visualizer.
 *
 * SPDX-FileCopyrightText: Copyright (C) 2024 Fernando García Liñán
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>

/*----------------------------------------------------------------------------*/
/* Customize me!                                                              */
/*----------------------------------------------------------------------------*/

const unsigned char white_square_color[3] = {201, 169, 116};
const unsigned char black_square_color[3] = {134,  95,  58};

const unsigned char white_piece_over_white_square_color [3] = {255, 255, 255};
const unsigned char black_piece_over_white_square_color [3] = {  0,   0,   0};
const unsigned char white_piece_over_black_square_color [3] = {255, 255, 255};
const unsigned char black_piece_over_black_square_color [3] = {  0,   0,   0};

static const char *piece_symbols[] = {
    " ",
    "♚", "♛", "♜", "♝", "♞", "♟", /* White piece over white square */
    "♚", "♛", "♜", "♝", "♞", "♟", /* Black piece over white square */
    "♚", "♛", "♜", "♝", "♞", "♟", /* White piece over black square */
    "♚", "♛", "♜", "♝", "♞", "♟", /* Black piece over black square */
};

/*----------------------------------------------------------------------------*/

#define SPACE_SEPARATOR "    "

enum square {
    SQUARE_EMPTY = 0,
    SQUARE_WHITE_KING,
    SQUARE_WHITE_QUEEN,
    SQUARE_WHITE_ROOK,
    SQUARE_WHITE_BISHOP,
    SQUARE_WHITE_KNIGHT,
    SQUARE_WHITE_PAWN,
    SQUARE_BLACK_KING,
    SQUARE_BLACK_QUEEN,
    SQUARE_BLACK_ROOK,
    SQUARE_BLACK_BISHOP,
    SQUARE_BLACK_KNIGHT,
    SQUARE_BLACK_PAWN,
};

enum castling {
    CASTLING_WHITE_KINGSIDE  = (1 << 0),
    CASTLING_WHITE_QUEENSIDE = (1 << 1),
    CASTLING_BLACK_KINGSIDE  = (1 << 2),
    CASTLING_BLACK_QUEENSIDE = (1 << 3),
};

static int parse_piece_placement(enum square board[8][8], const char *s)
{
    int rank = 7, file = 0;
    while (*s != '\0') {
        char c = *s;
        if (c == '/') {
            if (file < 8) {
                fprintf(stderr, "Invalid FEN piece placement data: missing files\n");
                return 1;
            }
            file = 0;
            --rank;
            if (rank < 0) {
                fprintf(stderr, "Invalid FEN piece placement data: found more than eight ranks\n");
                return 1;
            }
        } else if (c >= '1' && c <= '8') {
            int end_file = file + (int)(c - '0');
            if (end_file > 8) {
                fprintf(stderr, "Invalid FEN piece placement data: found more than eight files\n");
                return 1;
            }
            while (file < end_file) {
                board[rank][file++] = SQUARE_EMPTY;
            }
        } else {
            if (file >= 8) {
                fprintf(stderr, "Invalid FEN piece placement data: found more than eight files\n");
                return 1;
            }
            switch (c) {
            case 'P': board[rank][file] = SQUARE_WHITE_PAWN;   break;
            case 'N': board[rank][file] = SQUARE_WHITE_KNIGHT; break;
            case 'B': board[rank][file] = SQUARE_WHITE_BISHOP; break;
            case 'R': board[rank][file] = SQUARE_WHITE_ROOK;   break;
            case 'Q': board[rank][file] = SQUARE_WHITE_QUEEN;  break;
            case 'K': board[rank][file] = SQUARE_WHITE_KING;   break;
            case 'p': board[rank][file] = SQUARE_BLACK_PAWN;   break;
            case 'n': board[rank][file] = SQUARE_BLACK_KNIGHT; break;
            case 'b': board[rank][file] = SQUARE_BLACK_BISHOP; break;
            case 'r': board[rank][file] = SQUARE_BLACK_ROOK;   break;
            case 'q': board[rank][file] = SQUARE_BLACK_QUEEN;  break;
            case 'k': board[rank][file] = SQUARE_BLACK_KING;   break;
            default:
                fprintf(stderr, "Invalid character found in FEN piece placement data: \"%c\"\n", c);
                return 1;
            }
            ++file;
        }
        ++s;
    }
    return 0;
}

static int parse_castling_availability(int *castling, const char *s)
{
    *castling = 0;
    while (*s != '\0') {
        char c = *s;
        switch (c) {
        case '-': return 0;
        case 'K': *castling |= CASTLING_WHITE_KINGSIDE;  break;
        case 'Q': *castling |= CASTLING_WHITE_QUEENSIDE; break;
        case 'k': *castling |= CASTLING_BLACK_KINGSIDE;  break;
        case 'q': *castling |= CASTLING_BLACK_QUEENSIDE; break;
        default:
            fprintf(stderr, "Invalid character found in FEN castling availability field: \"%c\"\n", c);
            return 1;
        }
        ++s;
    }
    return 0;
}

static void print_fg_color(const unsigned char fg[3])
{
    printf("\x1b[38;2;%d;%d;%dm", fg[0], fg[1], fg[2]);
}

static void print_bg_color(const unsigned char bg[3])
{
    printf("\x1b[48;2;%d;%d;%dm", bg[0], bg[1], bg[2]);
}

static void print_clear(void)
{
    printf("\x1b[0m");
}

int main(int argc, char *argv[])
{
    if (argc != 7) {
        fprintf(stderr,
                "Usage: %s <valid FEN record>\n"
                "A valid FEN-string contains six fields, each separated by a space.\n",
                argv[0]);
        return 1;
    }

    enum square board[8][8];
    if (parse_piece_placement(board, argv[1])) {
        return 1;
    }

    char active_color = argv[2][0];
    if (active_color != 'w' && active_color != 'b') {
        fprintf(stderr, "Invalid FEN record: unrecognized active color \"%c\"\n", active_color);
        return 1;
    }

    int castling;
    if (parse_castling_availability(&castling, argv[3])) {
        return 1;
    }

    const char *en_passant      = argv[4];
    const char *halfmove_clock  = argv[5];
    const char *fullmove_number = argv[6];

    printf("   ╔═════════════════╗\n");
    for (int i = 7; i >= 0; --i) {
        printf(" %d ║", i + 1);
        print_fg_color(i % 2 ? white_square_color : black_square_color);
        printf("▐");

        for (int j = 0; j < 8; ++j) {
            enum square square = board[i][j];
            int is_white_square = (i + j) % 2;

            if (square >= SQUARE_WHITE_KING && square <= SQUARE_WHITE_PAWN) {
                print_fg_color(is_white_square ? white_piece_over_white_square_color : white_piece_over_black_square_color);
            } else if (square >= SQUARE_BLACK_KING && square <= SQUARE_BLACK_PAWN) {
                print_fg_color(is_white_square ? black_piece_over_white_square_color : black_piece_over_black_square_color);
            }
            if (is_white_square) {
                print_bg_color(white_square_color);
            } else {
                print_bg_color(black_square_color);
            }

            printf("%s", piece_symbols[is_white_square ? square : square + 12]);

            if (j == 7) {
                print_clear();
                print_fg_color(is_white_square ? white_square_color : black_square_color);
                printf("▌");
            } else {
                print_fg_color(is_white_square ? black_square_color : white_square_color);
                print_bg_color(is_white_square ? white_square_color : black_square_color);
                printf("▐");
            }
        }
        print_clear();
        printf("║");

        if (i == 7) {
            printf(SPACE_SEPARATOR"\x1b[1;4m%s\x1b[0m to play",
                   (active_color == 'w') ? "White" : "Black");
        } else if (i == 5) {
            printf(SPACE_SEPARATOR"\x1b[1mCastling\x1b[0m");
        } else if (i == 4) {
            printf(SPACE_SEPARATOR" White:  %s O-O  %s O-O-O",
                   (castling & CASTLING_WHITE_KINGSIDE)  ? "☑" : "☐",
                   (castling & CASTLING_WHITE_QUEENSIDE) ? "☑" : "☐");
        } else if (i == 3) {
            printf(SPACE_SEPARATOR" Black:  %s O-O  %s O-O-O",
                   (castling & CASTLING_BLACK_KINGSIDE)  ? "☑" : "☐",
                   (castling & CASTLING_BLACK_QUEENSIDE) ? "☑" : "☐");
        } else if (i == 1) {
            if (en_passant[0] != '-') {
                printf(SPACE_SEPARATOR"\x1b[2mEn passant square:\x1b[0m %s", en_passant);
            }
        }
        printf("\n");
    }
    printf("   ╚═════════════════╝");
    printf(SPACE_SEPARATOR"Move \x1b[1m%s\x1b[0m, %s half-moves since last capture\n",
           fullmove_number, halfmove_clock);
    printf("     a b c d e f g h \n");

    return 0;
}
