#ifndef UI_FONT_H
#define UI_FONT_H

#include "ui_colors.h"
#include "ui_file.h"

#ifdef __cplusplus
extern "C" {
#endif

enum FONT_IDS 
{
    ID_RESERVED,
    ID_DEBUG,
    ID_DEFAULT,
    ID_TITLE,
    ID_COUNT
};

// Array of pointers to rdpq_font_t, with each entry representing a font identified by an ID.
rdpq_font_t *font[ID_COUNT];

enum FONT_STYLES 
{
    STYLE_DEBUG,
    STYLE_DEFAULT,
    STYLE_TITLE,
    STYLE_BRIGHT,
    STYLE_GREEN,
    STYLE_COUNT
};

// RDPQ text parameters, used here primarily to set the following RDPQ font styles.
rdpq_textparms_t txt_debugParms;
rdpq_textparms_t txt_titleParms;
rdpq_textparms_t txt_gameParms;

// RDPQ font styles, used here primarily to set text color.
rdpq_fontstyle_t txt_debug_fontStyle;
rdpq_fontstyle_t txt_title_fontStyle;
rdpq_fontstyle_t txt_game_fontStyle;
rdpq_fontstyle_t txt_bright_fontStyle;
rdpq_fontstyle_t txt_green_fontStyle;

/* Declarations */

void ui_fontRegister(void);
void ui_fontUnregister(void);

/* Definitions */

/* All in one font initialization.
 - Loads and regsiters fonts.
 - Assigns colors to font styles.
 - Register font styles for each font.
 - Assigns font styles to text parameters.

Possible improvements would be to separate functionality,
to make fonts more flexible and modular. */
void ui_fontRegister(void)
{
    ui_fileFonts();

    font[ID_DEBUG] = rdpq_font_load_builtin(FONT_BUILTIN_DEBUG_VAR);
    font[ID_DEFAULT] = rdpq_font_load(uiFontFileName[0]);
    font[ID_TITLE] = rdpq_font_load(uiFontFileName[1]);

    // Create and register font styles
    txt_debug_fontStyle.color = ui_color(YELLOW);
    txt_debug_fontStyle.outline_color = ui_color(BLACK);

    txt_game_fontStyle.color = ui_color(WHITE);
    txt_debug_fontStyle.outline_color = ui_color(T_BLACK);

    txt_title_fontStyle.color = ui_color(GREEN);
    txt_title_fontStyle.outline_color = ui_color(DARK_GREEN);

    txt_bright_fontStyle.color = ui_color(YELLOW);
    txt_debug_fontStyle.outline_color = ui_color(T_BLACK);

    txt_green_fontStyle.color = ui_color(GREEN);
    txt_title_fontStyle.outline_color = ui_color(DARK_GREEN);

    for (int i = 1; i < ID_COUNT; i++)
    {
        rdpq_text_register_font(i, font[i]);
        rdpq_font_style(
            font[i], 
            STYLE_DEFAULT, 
            &txt_game_fontStyle
        );

        rdpq_font_style(
            font[i], 
            STYLE_TITLE, 
            &txt_title_fontStyle
        );

        rdpq_font_style(
            font[i], 
            STYLE_BRIGHT, 
            &txt_bright_fontStyle
        );

        rdpq_font_style(
            font[i], 
            STYLE_GREEN, 
            &txt_green_fontStyle
        );
    }

    rdpq_font_style(
        font[ID_DEBUG], 
        STYLE_DEBUG, 
        &txt_debug_fontStyle
    );

    txt_debugParms = (rdpq_textparms_t){.style_id = STYLE_DEBUG};
    txt_titleParms = (rdpq_textparms_t){.style_id = STYLE_TITLE};
    txt_gameParms = (rdpq_textparms_t){.style_id = STYLE_BRIGHT};

}

// Unregisters and frees fonts for the next minigame.
void ui_fontUnregister(void)
{
    for (int i = ID_DEBUG; i < ID_COUNT; ++i)
    {
        rdpq_text_unregister_font(i);
        rdpq_font_free(font[i]);
    }
}

#ifdef __cplusplus
}
#endif

#endif // UI_FONT_H