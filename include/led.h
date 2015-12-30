
#define RED_LED   0x20
#define GREEN_LED 0x10

void led_init(void);
void led_on(int color);
void led_off(int color);
void led_toggle(int color);
