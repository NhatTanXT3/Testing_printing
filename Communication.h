#ifndef COMMUNICATION_
#define COMMUNICATION_

#define CN_1 '~'
#define CN_2 '!'
#define CN_3 '@'
#define CN_4 '#'
#define CN_5 '$'
#define CN_6 '%'
#define CN_7 '^'
#define CN_8 '*'
#define CN_9 '('
#define CN_10 ')'

#define SENDING_MODE_1_ 0xAA //mistake, oxAA for left foot
#define SENDING_MODE_2_ 0xBB
#define SENDING_MODE_3_ 0xA1
#define SENDING_PASSIVE_LEFT_ARM_   0xB1
#define SENDING_PASSIVE_RIGHT_ARM_  0xB2
#define SENDING_PASSIVE_BOTH_ARM_   0xB3

#define SENDING_MODE_CONTROL_HEAD   0xB5
#define SENDING_MODE_RESET_HEAD     0xB6
//========for right foot=======
#define SENDING_KEY_ 0xAA// Mistake, 0xCC for right foot
#define ID_ZMP_OFFSET_RAW_DATA_ 4
#define ID_ZMP_OFFSET_FILTER_DATA_ 12
#define ID_ZMP_X_POS_ 19
#define ID_ZMP_Y_POS_ 20
#define ID_ZMP_AMPLITUDE_ 21
//=========for left foot======
//#define SENDING_KEY_ 0xCC //mistake, oxAA for left foot
//#define ID_ZMP_OFFSET_RAW_DATA_ 0
//#define ID_ZMP_OFFSET_FILTER_DATA_ 8
//#define ID_ZMP_X_POS_ 16
//#define ID_ZMP_Y_POS_ 17
//#define ID_ZMP_AMPLITUDE_ 18

#define MCU2PC_TERMINATOR_ 0xFE
#define MCU2PC_HEADER_ 0xFF

/*================================
   Nhận từ máy tính xuống
=================================*/
#define HELP_       'h'
#define SYSTEM_INFOR 'i'

#define CM_PRINTER_RUN_ON           'a'
#define CM_PRINTER_RUN_OFF          'b'
#define CM_BACKLIGHT_ON   'l'
#define CM_BACKLIGHT_OFF  'k'
#define CM_MOTOR_ON   'm'
#define CM_MOTOR_OFF  'n'

#define DISPLAY_ON_  'e'
#define DISPLAY_OFF_ 'f'
#define SET_SPEED_ 's'
#define SET_STEP_UP   'u'
#define SET_STEP_DOWN 'd'
#define SET_POS 'p'
#define RESET_SYSTEM  'r'

void serialEvent();

#endif


