#include "../../drivers/screen.h"

#define VIDEO_MEMORY 0xB8000
#define WHITE_ON_BLACK 0x0F


// Só alguns caracteres pra exemplo!
const char* big_letters[][5] = {
    // A
    {
        " .###. ",
        " #   # ",
        " #===# ",
        " #   # ",
        " #   # "
    },
    // B
    {
        " ###.  ",
        " #   # ",
        " ####. ",
        " #   # ",
        " ####. "
    },
    // C
    {
        "  .###. ",
        " #*     ",
        " #      ",
        " #*     ",
        "  *###. "
    },
    // D
    {
        " ###.  ",
        " #  #  ",
        " #   # ",
        " #  #  ",
        " ###.  "
    },
    // E
    {
        " ##### ",
        " #     ",
        " ####  ",
        " #     ",
        " ##### "
    },
    // F
    {
        " ##### ",
        " #     ",
        " ####  ",
        " #     ",
        " #     "
    },
    // G
    {
        "  .###. ",
        " #      ",
        " #  ### ",
        " #   #  ",
        "  *###. "
    },
    // H
    {
        " #   # ",
        " #   # ",
        " ##### ",
        " #   # ",
        " #   # "
    },
    // I
    {
        "  ###  ",
        "   #   ",
        "   #   ",
        "   #   ",
        "  ###  "
    },
    // J
    {
        "   ### ",
        "    #  ",
        "    #  ",
        " #  #  ",
        "  ###  "
    },
    // K
    {
        " #   # ",
        " #  #  ",
        " ###   ",
        " #  #  ",
        " #   # "
    },
    // L
    {
        " #     ",
        " #     ",
        " #     ",
        " #     ",
        " ##### "
    },
    // M
    {
        " #   # ",
        " #   # ",
        " # # # ",
        " #   # ",
        " #   # "
    },
    // N
    {
        " #   # ",
        " ##  # ",
        " # # # ",
        " #  ## ",
        " #   # "
    },
    // O
    {
        "  .###. ",
        " #   #  ",
        " #   #  ",
        " #   #  ",
        "  *###. "
    },
    // P
    {
        " ####.  ",
        " #   #  ",
        " ####.  ",
        " #      ",
        " #      "
    },
    // Q
    {
        "  .###. ",
        " #   #  ",
        " #   #  ",
        " #  ##  ",
        "  *###* "
    },
    // R
    {
        " ####.  ",
        " #   #  ",
        " ####.  ",
        " #  #   ",
        " #   #  "
    },
    // S
    {
        "  *###. ",
        " #      ",
        "  *###. ",
        "     #  ",
        "  *###. "
    },
    // T
    {
        " ##### ",
        "   #   ",
        "   #   ",
        "   #   ",
        "   #   "
    },
    // U
    {
        " #   # ",
        " #   # ",
        " #   # ",
        " #   # ",
        "  *###. "
    },
    // V
    {
        " #   # ",
        " #   # ",
        " #   # ",
        "  # #  ",
        "   #   "
    },
    // W
    {
        " #   # ",
        " #   # ",
        " #   # ",
        " # # # ",
        " #   # "
    },
    // X
    {
        " #   # ",
        "  # #  ",
        "   #   ",
        "  # #  ",
        " #   # "
    },
    // Y
    {
        " #   # ",
        "  # #  ",
        "   #   ",
        "   #   ",
        "   #   "
    },
    // Z
    {
        " ##### ",
        "     # ",
        "    #  ",
        "   #   ",
        " ##### "
    }
};


void print_char_at(char c, int row, int col) {
    volatile unsigned char* video = (unsigned char*) VIDEO_MEMORY;
    int offset = (row * 80 + col) * 2;
    video[offset] = c;
    video[offset + 1] = VGA_COLOR;
}

void ascii_print(const char* text, int row, int col) {
    for (int line = 0; line < 5; ++line) {
        int cursor = col;
        for (int i = 0; text[i]; ++i) {
            char ch = text[i];
            if (ch >= 'A' && ch <= 'Z') { // só até C no exemplo
                const char* art_line = big_letters[ch - 'A'][line];
                for (int j = 0; art_line[j]; ++j) {
                    print_char_at(art_line[j], row + line, cursor++);
                }
            } else {
                cursor += 8; // espaço pra letra desconhecida
            }
        }
    }
    
    set_cursor(get_screen_offset(row, col+5));
}
