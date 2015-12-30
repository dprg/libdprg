
#define LCD_REG  0xf00800
#define LCD_DATA 0xf00801

int lcd_init();
int lcd_string(char *string);
int lcd_write_reg(char val);
int lcd_write_data(char val);
int lcd_cursor(int row);
void lcd_service();

