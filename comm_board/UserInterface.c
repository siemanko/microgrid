#include "UserInterface.h"

#include "settings.h"
#include "lcd.h"
#include "Balance.h"
#include "DateAndTime.h"
#include "LoadBoardInterface.h"

uint32_t last_ui_update = 0;


void initUserInterface() {
    LCD_replace_line("UID:", 0); // top line
    LCD_int(getSetting(SETTINGS_UID));
    LCD_print(" type:");
    LCD_char(getSetting(SETTINGS_NODE_TYPE));
    LCD_replace_line("", LCD_BOTTOM_ROW); // bottom line
    unsigned int energy_hindi[] = {3,4,5,0};
    LCD_print_custom(energy_hindi);
    LCD_print(": LOADING");
    last_ui_update = 0;
}

void userInterfaceStep() {
    if (last_ui_update + 1 <= getTimestamp()) {
        LCD_replace_line("", LCD_BOTTOM_ROW);
        last_ui_update = getTimestamp();
        LCD_home(LCD_BOTTOM_ROW);
        unsigned int energy_hindi[] = {3,4,5,0};
        LCD_print_custom(energy_hindi);
        LCD_print(": ");
        LCD_int(getBalance());
        last_ui_update = getTimestamp();
    }

}