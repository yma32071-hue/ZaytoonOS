#define BUFFER_SIZE 256

// Helper to check if two strings match
int strings_equal(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *s1 == *s2;
}

void _start() {
    char input_buffer[BUFFER_SIZE];
    
    // Print a welcome message to the user
    // Note: You will replace these placeholders with actual screen write functions/syscalls
    print_string("Welcome to ZaytoonOS CLI!\n");

    while (1) {
        print_string("zaytoon_os> ");
        
        // 1. READ: Wait for user to type a command
        read_line(input_buffer, BUFFER_SIZE);
        
        // 2. EVALUATE & PRINT
        if (strings_equal(input_buffer, "help")) {
            print_string("Available commands: help, clear, version, exit\n");
        } 
        else if (strings_equal(input_buffer, "version")) {
            print_string("ZaytoonOS v1.0.0 (Custom .capp Edition)\n");
        } 
        else if (strings_equal(input_buffer, "clear")) {
            clear_screen();
        } 
        else if (strings_equal(input_buffer, "exit")) {
            print_string("Shutting down shell...\n");
            break; 
        } 
        else if (input_buffer[0] != '\0') {
            print_string("Unknown command: ");
            print_string(input_buffer);
            print_string("\n");
        }
    }

    // If the shell exits, loop forever to prevent crashing the CPU
    while(1);
}