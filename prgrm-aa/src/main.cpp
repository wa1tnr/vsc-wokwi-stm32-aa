/* NUCLEO  L031K6 */
/* was: NUCLEO  C031C6 */

#include <Arduino.h>

#ifdef __cplusplus
extern "C" {
#endif

void onemain();

#ifdef __cplusplus
}
#endif

/*  Fri 23 Jun 16:28:42 UTC 2023 */

/* Tiny interpreter,
   similar to myforth's Standalone Interpreter
   This example code is in the public domain */

/* Structure of a dictionary entry */
typedef struct {
    const char *name;
    void (*function)();
} entry;

/* Data stack for parameter passing
   This "stack" is circular,
   like a Green Arrays F18A data stack,
   so overflow and underflow are not possible
   Number of items must be a power of 2 */
const int STKSIZE = 8;
const int STKMASK = 7;
int stack[STKSIZE];
int p = 0;

/* TOS is Top Of Stack */
#define TOS stack[p]
/* NAMED creates a string in flash */
#define NAMED(x, y) const char x[] = y

/* Terminal Input Buffer for interpreter */
const byte maxtib = 16;
char tib[maxtib];
/* buffer required for strings read from flash */
char namebuf[maxtib];
byte pos;

/* push n to top of data stack */
void push(int n) {
    p = (p + 1) & STKMASK;
    TOS = n;
}

/* return top of stack */
int pop() {
    int n = TOS;
    p = (p - 1) & STKMASK;
    return n;
}

/* Global delay timer */
int spd = 15;

/* top of stack becomes current spd */
NAMED(_speed, "speed");
void speed() {
    Serial.print("  range:  4 to 21  ");
    int parm = pop();
    spd = max(4, parm);
    parm = min(21, spd);
    spd = parm;
}

/* discard top of stack */
NAMED(_drop, "drop");
void drop() {
    pop();
}

/* recover dropped stack item */
NAMED(_back, "back");
void back() {
    for (int i = 1; i < STKSIZE; i++)
        drop();
}

/* copy top of stack */
NAMED(_dup, "dup");
void dup() {
    push(TOS);
}

/* exchange top two stack items */
NAMED(_swap, "swap");
void swap() {
    int a;
    int b;
    a = pop();
    b = pop();
    push(a);
    push(b);
}

/* copy second on stack to top */
NAMED(_over, "over");
void over() {
    int a;
    int b;
    a = pop();
    b = pop();
    push(b);
    push(a);
    push(b);
}
/* add top two items */
NAMED(_add, "+");
void add() {
    int a = pop();
    TOS = a + TOS;
}

/* bitwise and top two items */
NAMED(_and, "and");
void and_() {
    int a = pop();
    TOS = a & TOS;
}

/* inclusive or top two items */
NAMED(_or, "or");
void or_() {
    int a = pop();
    TOS = a | TOS;
}

/* exclusive or top two items */
NAMED(_xor, "xor");
void xor_() {
    int a = pop();
    TOS = a ^ TOS;
}

/* invert all bits in top of stack */
NAMED(_invert, "invert");
void invert() {
    TOS = ~(TOS);
}

/* negate top of stack */
NAMED(_negate, "negate");
void negate() {
    TOS = -(TOS);
}

/* destructively display top of stack, decimal */
NAMED(_dot, ".");
void dot() {
    Serial.print(pop());
    Serial.print(" ");
}

/* destructively display top of stack, hex */
NAMED(_dotHEX, ".h");
void dotHEX() {
    Serial.print(0xffff & pop(), HEX);
    Serial.print(" ");
}

/* display whole stack, hex */
NAMED(_dotShex, ".sh");
void dotShex() {
    for (int i = 0; i < STKSIZE; i++)
        dotHEX();
}

/* display whole stack, decimal */
NAMED(_dotS, ".s");
void dotS() {
    for (int i = 0; i < STKSIZE; i++)
        dot();
}

/* delay TOS # of milliseconds */
NAMED(_delay, "delay");
void del() {
    delay(pop());
}

void bad_gpio() {
    Serial.print("Bad GPIO pin number!");
    // push(13); // shuffle it to D13, safe to do.
    drop();
    push(13);
}

void no_gpio() {
    dup();            // for test
    int test = pop(); // no more extra copies
    if (test == 36) {
        bad_gpio();
        return;
    }
    if (test == 40) {
        bad_gpio();
    }
}
#define WIGGLES 4 // blinks

/* Toggle pin at TOS and delay(spd), repeat... */
NAMED(_wiggle, "wiggle");
void wiggle() {
    no_gpio();
    int a = pop();
    pinMode(a, OUTPUT);

    for (int i = 0; i < WIGGLES; i++) {
        digitalWrite(a, HIGH);
        delay(spd * 3 * 3);
        digitalWrite(a, LOW);
        delay(spd * 9 * 3);
    }
}

/* TOS is pin number, set it HIGH */
NAMED(_high, "high");
void high() {
    digitalWrite(pop(), HIGH);
}

/* set TOS pin LOW */
NAMED(_low, "low");
void low() {
    digitalWrite(pop(), LOW);
}

/* read TOS pin */
NAMED(_in, "in");
void in() {
    TOS = digitalRead(TOS);
}

/* make TOS pin an input */
NAMED(_input, "input");
void input() {
    pinMode(pop(), INPUT);
}

/* make TOS pin an output */
NAMED(_output, "output");
void output() {
    pinMode(pop(), OUTPUT);
}

/* make TOS pin an input with weak pullup */
NAMED(_input_pullup, "input_pullup");
void input_pullup() {
    pinMode(pop(), INPUT_PULLUP);
}

/* print CR, LF or both */

#define LOCAL_LINE_ENDING 0x0a

NAMED(_cr, "cr");
void cr_() {
    Serial.write(LOCAL_LINE_ENDING);
}

NAMED(_nvic_reset, "reset");
void nvic_reset_() {
    NVIC_SystemReset();
}

/* dump 16 bytes of RAM in hex with ascii on the side */
void dumpRAM() {
    char buffer[9] = "";
    char *ram;
    int p = pop();
    ram = (char *)p;
    sprintf(buffer, "%4x", p); // was 4x
    Serial.print(buffer);
    Serial.print("   ");
    for (int i = 0; i < 16; i++) {
        char c = *ram++;
        sprintf(buffer, " %2x", (c & 0xff));
        Serial.print(buffer);
    }
    ram = (char *)p;
    Serial.print("   ");
    for (int i = 0; i < 16; i++) {
        buffer[0] = *ram++;
        if (buffer[0] > 0x7f || buffer[0] < ' ')
            buffer[0] = '.';
        buffer[1] = '\0';
        Serial.print(buffer);
    }
    push(p + 16);
}

/* dump 256 bytes of RAM */
NAMED(_dumpr, "dump");
void rdumps() {
    for (int i = 0; i < 16; i++) {
        Serial.println();
        dumpRAM();
    }
}

/* End of Forth interpreter words */
/* ******************************************** */
/* Beginning of application words */

/* End of application words */
/* ******************************************** */
/* Now build the dictionary */

/* empty words don't cause an error */
NAMED(_nop, " ");
void nop() {
}

/* Forward declaration required here */
NAMED(_words, "words");
void words();

NAMED(_dow, "dow");
void dow() { // dup output wiggle
    dup();
    dotS();
    output();
    wiggle(); // cr_();
}

void strobings() {
    dup();
    dotS();
    drop();
    wiggle();
}

/* strobe all ports */
NAMED(_strall, "strall");
void strall() {
    int accum = pop();         // need a starting port pin number
    int start = max(0, accum); // zero or greater
    accum = start;
    start = min(253, accum); // 253 or less
    for (int port = start; port < 255; port++) {
        push(port);
        strobings();
    }
}

/*  findings */

/*
 *    PB7 = D0
 *    PB6 = D1
 *   PA10 = D2
 *    PB3 = D3
 *   PB10 = D4
 *    PB4 = D5
 *    PB5 = D6
 *   PA15 = D7
 *    PA9 = D8
 *    PC7 = D9
 *  PB0.2 = D10
 *    PA7 = D11
 *    PA6 = D12
 *    PA5 = D13  LED_BUILTIN
 *    PB9 = D14
 *    PB8 = D15
 *    PA3 =
 * 192-197  lights up A0 thru A5
 * 200 lights up D13
 * 203 lights up PA8/D41
 * 204 lights up D19
 *
 *
 *
 */

NAMED(_onemain, "main");
// void onemain();

/* strobe the wired LEDs */
NAMED(_strobes, "strobes");
void strobes() {
    for (int index = 16; index < 24; index++) {
        push(index);
        strobings();
    }
    for (int index = 30; index < 36; index++) {
        push(index);
        strobings();
    }
}

/* table of names and function addresses in flash */
const entry dictionary[] = {{_nop, nop},         {_nvic_reset, nvic_reset_},
                            {_cr, cr_},          {_strall, strall},
                            {_strobes, strobes}, {_words, words},
                            {_dup, dup},         {_drop, drop},
                            {_back, back},       {_swap, swap},
                            {_over, over},       {_add, add},
                            {_and, and_},        {_or, or_},
                            {_xor, xor_},        {_invert, invert},
                            {_negate, negate},   {_dotS, dotS},
                            {_dotShex, dotShex}, {_dot, dot},
                            {_dotHEX, dotHEX},   {_delay, del},
                            {_high, high},       {_low, low},
                            {_in, in},           {_input, input},
                            {_output, output},   {_input_pullup, input_pullup},
                            {_wiggle, wiggle},   {_dow, dow},
                            {_dumpr, rdumps},    {_onemain, onemain},
                            {_speed, speed}};

/* Number of words in the dictionary */
const int entries = sizeof dictionary / sizeof dictionary[0];

/* Display all words in dictionary */
void words() {
    for (int i = entries - 1; i >= 0; i--) {
        strcpy(namebuf, dictionary[i].name);
        Serial.print(namebuf);
        Serial.print(" ");
    }
}

/* Find a word in the dictionary, returning its position */
int locate() {

    //  GOOD spot to print something to help user understand prgrm flow.

    for (int i = entries - 1; i >= 1; i--) {
        int name_size = 15;

        // populate namebuf from current dictionary entry
        memcpy(namebuf, dictionary[i].name, name_size);

        if (!strcmp(tib, namebuf)) {
            namebuf[0] = '\0';
            tib[0] = '\0';
            //      namebuf[0] = '\0';
            //      tib[0] = '\0';
            return i;
        }
    }
    return 0;
}

/* Is the word in tib a number? */
int isNumber() {
    char *endptr;
    strtol(tib, &endptr, 0);
    if (endptr == tib)
        return 0;
    if (*endptr != '\0')
        return 0;
    return 1;
}

/* Convert number in tib */
int number() {
    char *endptr;
    return (int)strtol(tib, &endptr, 0);
}

char ch;

void ok() {
    // Serial.print('\n');
    if (ch == '\000') {
        Serial.println("ok");
    } // was '\r'
    Serial.print("ok  ");
}

/* Incrementally read command line from serial port */
byte reading() {
    if (!Serial.available())
        return 1;
    ch = Serial.read();
    Serial.write(ch); // new for 28 Jan 2025
    if (ch == '\n')   // rescind 28 jan 2025: '\r')
        return 1;
    if (ch == '\r') { // rescind 28 jan 2025: '\n')
        Serial.print('\n');
        return 0;
    }
    if (ch == ' ')
        return 0;
    if (pos < maxtib) {
        tib[pos++] = ch;
        tib[pos] = 0;
    }
    return 1;
}

void printTIB() {
    // cr_();
    // Serial.write(0x0d);
    // Serial.print(tib);
    // Serial.print('\r');
    // Serial.print('\n');
    // Serial.print(" ");
}

/* Block on reading the command line from serial port */
/* then echo each word */
void readword() {
    pos = 0;
    tib[0] = 0;
    while (reading()) {
        ;
    }
    printTIB();
    // Serial.print(tib);
    // Serial.print(" ");
}

/* Run a word via its name */
void runword() {
    int place = locate();
    if (place != 0) {
        dictionary[place].function();
        ok();
        return;
    }
    if (isNumber()) {
        push(number());
        ok();
        return;
    }
    Serial.println("?");
}

/* Arduino main loop */

void loopLEDinfini() {
    push(13);
    wiggle();
    delay(3800);
}

void validateLED() {
    for (;;) {
        loopLEDinfini();
    }
}

// #define TIME_STAMP " Wed 29 Jan 16:38:24 UTC 2025"
#define TIME_STAMP " Wed 12 Feb 22:23:44 UTC 2025"

void identify() {
    Serial.print(" id: ");
    Serial.println(TIME_STAMP);
    // Serial.println(" ..ueP/here-for-now/rev-ccx.d");
    Serial.println(" ../vsc-wokwi-stm32-aa/prgrm-aa/src");
}

void setup() {
    Serial.begin(115200);
    while (!Serial)
        ;
    Serial.write(' ');
    Serial.println();
    Serial.println("Forth-like interpreter:");
    Serial.println(
        "    536871082  is  0x200000AA    use  536870912 for 0x0000 \n");
    Serial.println(" https://wokwi.com/projects/******************  nucleo "
                   "C031C6 on wokwi");
    words();
    Serial.println();
    // validateLED();
    // strobes();
    // onemain();
    identify();
}

void loop() {
    readword();
    runword();
}

// 23 June 02:22z
// END.
