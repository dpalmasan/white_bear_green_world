# Git Hooks & Code Formatting

This project uses a pre-commit git hook to automatically format C++ code before commits.

## Setup

### Prerequisites
- `clang-format` (already installed)

### Configuration Files

- **`.clang-format`** - Code formatting style configuration
  - Based on Google C++ style guide
  - Enforces consistent indentation, spacing, and line length
  - 4-space indentation
  - 100-character line limit

- **`.git/hooks/pre-commit`** - Git pre-commit hook
  - Automatically runs before each commit
  - Formats all staged C++ files (`.cpp`, `.h`, `.cc`, `.cxx`, `.hpp`)
  - Re-stages formatted files
  - Provides colored output for better visibility

## How It Works

When you run `git commit`:

1. The pre-commit hook executes automatically
2. It finds all staged C++ files
3. Runs `clang-format` on each file
4. Re-stages the formatted changes
5. Allows the commit to proceed with properly formatted code

## Example

```bash
$ git add src/Game.cpp src/Game.h
$ git commit -m "Add new feature"

# Output:
# Running clang-format on staged C++ files...
#   Formatting: src/Game.cpp
#   Formatting: src/Game.h
# ✓ Code formatting complete
# [main a1b2c3d] Add new feature
```

## Manual Formatting

To manually format C++ files without committing:

```bash
# Format a single file
clang-format -i src/Game.cpp

# Format all C++ files in the project
find src -name "*.cpp" -o -name "*.h" | xargs clang-format -i
```

## Skipping the Hook

If you need to skip the hook for a specific commit:

```bash
git commit --no-verify -m "Commit message"
```

## Troubleshooting

If the hook fails:

1. Verify `clang-format` is installed:
   ```bash
   which clang-format
   ```

2. Check hook permissions:
   ```bash
   ls -la .git/hooks/pre-commit
   ```

3. Run the hook manually:
   ```bash
   .git/hooks/pre-commit
   ```

## Customizing the Style

Edit `.clang-format` to change formatting rules. Common options:

- `IndentWidth` - Number of spaces per indent level
- `ColumnLimit` - Maximum line length
- `BasedOnStyle` - Base style (Google, LLVM, Mozilla, etc.)
- `AllowShortFunctionsOnASingleLine` - Allow brief functions on one line
- `BreakBeforeBraces` - Brace placement style

See [Clang Format Documentation](https://clang.llvm.org/docs/ClangFormatStyleOptions.html) for all options.
