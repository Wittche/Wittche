# Good First Issues for Contributors

This file contains ready-to-create GitHub issues for attracting contributors to Wittche OS.

## Issue 1: Add Arrow Key Support for Line Editing

**Title**: [Feature] Add left/right arrow key support for cursor movement in shell

**Labels**: enhancement, good first issue, shell

**Description**:
Currently, the Wittche OS shell doesn't support cursor movement within the input line. Users should be able to use left/right arrow keys to move the cursor and edit text at any position.

**Tasks**:
- [ ] Detect arrow key scancodes in keyboard driver (kernel/keyboard.c)
- [ ] Track cursor position within the input buffer
- [ ] Implement cursor movement logic
- [ ] Update screen cursor position when arrows are pressed
- [ ] Allow character insertion at cursor position

**Technical Details**:
- Arrow key scancodes: Left (0x4B), Right (0x4D), Up (0x48), Down (0x50)
- Current keyboard handler: `kernel/keyboard.c:keyboard_handler()`
- Current input function: `kernel/keyboard.c:keyboard_get_line()`

**Difficulty**: Easy-Medium
**Expected time**: 2-4 hours
**Learning**: Input handling, scancode processing, buffer management

---

## Issue 2: Implement Tab Completion for Shell Commands

**Title**: [Feature] Add tab completion for shell commands

**Labels**: enhancement, good first issue, shell

**Description**:
Implement tab completion to make the shell more user-friendly. When the user presses Tab, the shell should autocomplete the command if there's a unique match, or show available options.

**Tasks**:
- [ ] Detect Tab key press (scancode 0x0F)
- [ ] Compare input buffer with available commands
- [ ] If unique match exists, complete the command
- [ ] If multiple matches exist, show available options
- [ ] Add visual feedback for completion

**Technical Details**:
- Tab scancode: 0x0F
- Available commands are in `kernel/shell.c:shell_process_command()`
- Use string comparison functions from `kernel/string.c`

**Difficulty**: Easy
**Expected time**: 2-3 hours
**Learning**: String matching, command parsing, user interaction

---

## Issue 3: Add Ctrl+L Shortcut for Clear Screen

**Title**: [Feature] Add Ctrl+L keyboard shortcut to clear screen

**Labels**: enhancement, good first issue, keyboard

**Description**:
Add a keyboard shortcut (Ctrl+L) to quickly clear the screen, similar to Unix terminals.

**Tasks**:
- [ ] Track Ctrl key state in keyboard driver
- [ ] Detect Ctrl+L combination
- [ ] Call screen_clear() when Ctrl+L is pressed
- [ ] Redisplay shell prompt after clearing

**Technical Details**:
- Ctrl scancode: 0x1D (Left Ctrl), 0x1D with E0 prefix (Right Ctrl)
- 'L' scancode: 0x26
- Clear function: `kernel/screen.c:screen_clear()`

**Difficulty**: Easy
**Expected time**: 1-2 hours
**Learning**: Keyboard input, key combinations, terminal shortcuts

---

## Issue 4: Improve Error Messages with Suggestions

**Title**: [Enhancement] Add "did you mean?" suggestions to error messages

**Labels**: enhancement, good first issue, shell

**Description**:
When users type an unknown command, show similar commands as suggestions to help them find what they're looking for.

**Tasks**:
- [ ] Implement string similarity function (Levenshtein distance or simple matching)
- [ ] Compare unknown command with all available commands
- [ ] Find closest matches (edit distance <= 2)
- [ ] Display "Did you mean: ..." message with suggestions
- [ ] Test with common typos

**Technical Details**:
- Error handling in: `kernel/shell.c:shell_process_command()`
- String functions in: `kernel/string.c`
- Could use simple character-by-character comparison for v1

**Difficulty**: Medium
**Expected time**: 3-4 hours
**Learning**: String algorithms, user experience design

---

## Issue 5: Add Command Line Length Indicator

**Title**: [Feature] Show remaining characters when approaching input limit

**Labels**: enhancement, good first issue, shell, ui

**Description**:
Add a visual indicator when the user is approaching the maximum command line length (currently 256 characters).

**Tasks**:
- [ ] Track current input length
- [ ] When length > 200 chars, show warning
- [ ] When length > 240 chars, show urgent warning in red
- [ ] Display character count: "250/256"
- [ ] Prevent input beyond maximum

**Technical Details**:
- Max length: `MAX_CMD_LENGTH` in `kernel/shell.c`
- Current input function: `kernel/keyboard.c:keyboard_get_line()`
- Use `kprintf_color()` for colored warnings

**Difficulty**: Easy
**Expected time**: 2-3 hours
**Learning**: Input validation, user feedback, UI design

---

## Issue 6: Add Real-Time Clock (RTC) Support

**Title**: [Feature] Implement RTC driver to show actual date/time

**Labels**: enhancement, hardware, driver, good first issue

**Description**:
Add support for the Real-Time Clock (RTC) to display actual date and time instead of just uptime.

**Tasks**:
- [ ] Create `kernel/rtc.c` and `include/rtc.h`
- [ ] Implement RTC port I/O (ports 0x70, 0x71)
- [ ] Read date/time from CMOS
- [ ] Convert BCD to binary
- [ ] Add `time` or `date` shell command
- [ ] Display in readable format

**Technical Details**:
- RTC uses ports 0x70 (address) and 0x71 (data)
- CMOS registers: 0x00 (seconds), 0x02 (minutes), 0x04 (hours), etc.
- Reference: OSDev Wiki RTC article
- Similar pattern to timer.c/h

**Difficulty**: Medium
**Expected time**: 4-6 hours
**Learning**: Hardware I/O, CMOS access, time handling

---

## Issue 7: Add Color Themes for Shell

**Title**: [Feature] Implement customizable color themes

**Labels**: enhancement, good first issue, shell, ui

**Description**:
Add ability to change shell color themes with a new `theme` command. Start with 2-3 predefined themes (e.g., "classic", "dark", "matrix").

**Tasks**:
- [ ] Create theme structure with color definitions
- [ ] Implement 3 themes: classic (current), dark, matrix (green on black)
- [ ] Add `theme` command to list/change themes
- [ ] Save current theme in a global variable
- [ ] Update prompt colors based on active theme
- [ ] Add to help command

**Technical Details**:
- Color definitions in: `include/screen.h`
- Add command in: `kernel/shell.c`
- Themes could be structs with color values for: prompt, text, errors, warnings

**Difficulty**: Easy-Medium
**Expected time**: 3-4 hours
**Learning**: Color handling, configuration management, UI customization

---

## Issue 8: Add Memory Statistics to `mem` Command

**Title**: [Enhancement] Show heap and stack usage in mem command

**Labels**: enhancement, good first issue, memory

**Description**:
Enhance the `mem` command to show more detailed memory statistics, preparing for future memory management features.

**Tasks**:
- [ ] Track kernel code size (symbols from linker)
- [ ] Calculate approximate stack usage
- [ ] Show used vs available memory regions
- [ ] Add ASCII bar chart for memory usage visualization
- [ ] Add BSS section information

**Technical Details**:
- Current `mem` command: `kernel/shell.c:cmd_mem()`
- Linker symbols can be accessed from linker.ld
- Stack pointer: read ESP register
- Use kprintf() for formatted output

**Difficulty**: Medium
**Expected time**: 3-5 hours
**Learning**: Memory layout, linker symbols, low-level debugging

---

## How to Submit These Issues

1. Go to the GitHub repository
2. Click "Issues" → "New Issue"
3. Copy the content from above (Title, Labels, Description, Tasks)
4. Add appropriate labels
5. Submit the issue

## Labels to Create

Make sure these labels exist in your repository:
- `enhancement`
- `good first issue`
- `shell`
- `keyboard`
- `hardware`
- `driver`
- `ui`
- `memory`
