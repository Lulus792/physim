/* Appended after generated C with main renamed to ps_test_main. */
#undef main
static void ps_test_balance(void) {
    if (psmemory.live_bytes)
        _Exit(99);
}
int main(void) {
    if (atexit(ps_test_balance))
        return 98;
    return ps_test_main();
}
